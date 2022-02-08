#include "klib/http.h"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <string_view>

#include <curl/curl.h>
#include <boost/algorithm/string.hpp>
#include <scope_guard.hpp>

#include "klib/exception.h"
#include "klib/log.h"
#include "klib/util.h"

namespace klib {

namespace {

void check_curl_correct(CURLcode code) {
  if (code != CURLcode::CURLE_OK) {
    throw RuntimeError(curl_easy_strerror(code));
  }
}

void check_curl_correct(CURLUcode code) {
  if (code != CURLUcode::CURLUE_OK) {
    throw RuntimeError(curl_url_strerror(code));
  }
}

class AddURL {
 public:
  explicit AddURL(
      CURL *curl, const std::string &url,
      const std::unordered_map<std::string, std::string> &params = {})
      : curl_(curl), url_(curl_url()) {
    SCOPE_FAIL { curl_url_cleanup(url_); };

    auto rc = curl_url_set(url_, CURLUPART_URL, url.c_str(), 0);
    check_curl_correct(rc);

    for (const auto &[key, value] : params) {
      std::string query = key + "=" + value;
      rc = curl_url_set(url_, CURLUPART_QUERY, query.c_str(),
                        CURLU_APPENDQUERY | CURLU_URLENCODE);
      check_curl_correct(rc);
    }

    check_curl_correct(curl_easy_setopt(curl_, CURLOPT_CURLU, url_));
  }

  ~AddURL() { curl_url_cleanup(url_); }

 private:
  CURL *curl_;
  CURLU *url_;
};

class AddHeader {
 public:
  explicit AddHeader(
      CURL *curl, const std::unordered_map<std::string, std::string> &headers)
      : curl_(curl) {
    if (std::empty(headers)) {
      return;
    }

    SCOPE_FAIL { curl_slist_free_all(chunk_); };

    for (const auto &[key, value] : headers) {
      if (std::empty(key) || std::empty(value)) {
        throw RuntimeError("The header key and value can not be empty");
      }

      std::string str = key;
      str.append(": ").append(value);
      chunk_ = curl_slist_append(chunk_, str.c_str());
    }

    check_curl_correct(curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, chunk_));
  }

  ~AddHeader() {
    curl_slist_free_all(chunk_);

    try {
      check_curl_correct(curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, nullptr));
    } catch (...) {
      error("~AddHeader failed");
    }
  }

 private:
  CURL *curl_;
  curl_slist *chunk_ = nullptr;
};

class AddForm {
 public:
  explicit AddForm(CURL *curl,
                   const std::unordered_map<std::string, std::string> &data,
                   const std::unordered_map<std::string, std::string> &file)
      : curl_(curl) {
    if (std::empty(data) && std::empty(file)) {
      return;
    }

    form_ = curl_mime_init(curl_);
    SCOPE_FAIL { curl_mime_free(form_); };

    for (const auto &[key, value] : data) {
      if (std::empty(key) || std::empty(value)) {
        throw RuntimeError("The post form key and value can not be empty");
      }

      auto field = curl_mime_addpart(form_);
      curl_mime_name(field, key.c_str());
      curl_mime_data(field, value.c_str(), CURL_ZERO_TERMINATED);
    }

    for (const auto &[file_name, path] : file) {
      if (std::empty(file_name) || std::empty(path)) {
        throw RuntimeError("The post file_name and path can not be empty");
      }

      if (!std::filesystem::is_regular_file(path)) {
        throw RuntimeError("File '{}' not exist", path);
      }

      auto field = curl_mime_addpart(form_);
      curl_mime_name(field, file_name.c_str());
      curl_mime_filedata(field, path.c_str());
    }

    check_curl_correct(curl_easy_setopt(curl_, CURLOPT_MIMEPOST, form_));
  }

  ~AddForm() {
    curl_mime_free(form_);

    try {
      check_curl_correct(curl_easy_setopt(curl_, CURLOPT_MIMEPOST, nullptr));
    } catch (...) {
      error("~AddForm failed");
    }
  }

 private:
  CURL *curl_;
  curl_mime *form_ = nullptr;
};

}  // namespace

class Request::RequestImpl {
 public:
  RequestImpl();

  RequestImpl(const RequestImpl &) = delete;
  RequestImpl(RequestImpl &&) = delete;
  RequestImpl &operator=(const RequestImpl &) = delete;
  RequestImpl &operator=(RequestImpl &&) = delete;
  ~RequestImpl();

  void verbose(bool flag);
  void allow_redirects(bool flag);
  void set_proxy(const std::string &proxy);
  void set_proxy_from_env();
  void set_no_proxy();
  void set_doh_url(const std::string &url);
  void set_user_agent(const std::string &user_agent);
  void set_browser_user_agent();
  void set_curl_user_agent();
  void set_timeout(std::int64_t seconds);
  void set_connect_timeout(std::int64_t seconds);
  void use_cookies(bool flag);
  void set_accept_encoding(const std::string &accept_encoding);
  std::string url_encode(const std::string &str);
  std::string url_decode(const std::string &str);

  Response get(const std::string &url,
               const std::unordered_map<std::string, std::string> &params,
               const std::unordered_map<std::string, std::string> &headers);
  Response post(const std::string &url,
                const std::unordered_map<std::string, std::string> &data,
                const std::unordered_map<std::string, std::string> &headers);
  Response post(const std::string &url, const std::string &json,
                const std::unordered_map<std::string, std::string> &headers);
  Response post_mime(
      const std::string &url,
      const std::unordered_map<std::string, std::string> &data,
      const std::unordered_map<std::string, std::string> &file,
      const std::unordered_map<std::string, std::string> &headers);

 private:
  constexpr static std::string_view cookies_path = "/tmp/cookies.txt";
  bool use_cookies_ = true;

  void set_cookies();

  Response do_post();

  static std::size_t callback_func_std_string(void *contents, std::size_t size,
                                              std::size_t nmemb,
                                              std::string *s);
  std::string splicing_post_fields(
      const std::unordered_map<std::string, std::string> &data);

  CURL *http_handle_;
};

Request::RequestImpl::RequestImpl() {
  check_curl_correct(curl_global_init(CURL_GLOBAL_DEFAULT));

  http_handle_ = curl_easy_init();
  if (!http_handle_) {
    throw RuntimeError("curl_easy_init error");
  }
  SCOPE_FAIL {
    curl_easy_cleanup(http_handle_);
    curl_global_cleanup();
  };

  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_BUFFERSIZE, 102400L));
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_FOLLOWLOCATION, 1L));
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_MAXREDIRS, 50L));
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_TCP_KEEPALIVE, 1L));

  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_WRITEFUNCTION,
                                      RequestImpl::callback_func_std_string));
}

Request::RequestImpl::~RequestImpl() {
  curl_easy_cleanup(http_handle_);
  curl_global_cleanup();
}

void Request::RequestImpl::verbose(bool flag) {
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_VERBOSE, flag));
}

void Request::RequestImpl::allow_redirects(bool flag) {
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_FOLLOWLOCATION, flag));
}

void Request::RequestImpl::set_proxy(const std::string &proxy) {
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_PROXY, proxy.c_str()));
}

void Request::RequestImpl::set_proxy_from_env() {
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_PROXY, std::getenv("HTTP_PROXY")));
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_NOPROXY, std::getenv("NO_PROXY")));
}

void Request::RequestImpl::set_no_proxy() {
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_NOPROXY, "*"));
}

void Request::RequestImpl::set_doh_url(const std::string &url) {
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_DOH_URL, url.c_str()));
}

void Request::RequestImpl::set_user_agent(const std::string &user_agent) {
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_USERAGENT, user_agent.c_str()));
}

void Request::RequestImpl::set_browser_user_agent() {
  // navigator.userAgent
  set_user_agent(
      "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) "
      "Chrome/98.0.4758.80 Safari/537.36 Edg/98.0.1108.43");
}

void Request::RequestImpl::set_curl_user_agent() {
  set_user_agent("curl/7.81.0");
}

void Request::RequestImpl::set_timeout(std::int64_t seconds) {
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_TIMEOUT, seconds));
}

void Request::RequestImpl::set_connect_timeout(std::int64_t seconds) {
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_CONNECTTIMEOUT, seconds));
}

void Request::RequestImpl::use_cookies(bool flag) { use_cookies_ = flag; }

void Request::RequestImpl::set_accept_encoding(
    const std::string &accept_encoding) {
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_ACCEPT_ENCODING,
                                      accept_encoding.c_str()));
}

std::string Request::RequestImpl::url_encode(const std::string &str) {
  auto ptr = curl_easy_escape(http_handle_, str.c_str(), std::size(str));
  SCOPE_EXIT { curl_free(ptr); };
  if (!ptr) {
    throw RuntimeError("curl_easy_escape error");
  }

  return ptr;
}

std::string Request::RequestImpl::url_decode(const std::string &str) {
  std::int32_t length;
  auto ptr =
      curl_easy_unescape(http_handle_, str.c_str(), std::size(str), &length);
  SCOPE_EXIT { curl_free(ptr); };
  if (!ptr) {
    throw RuntimeError("curl_easy_unescape error");
  }

  return std::string(ptr, length);
}

Response Request::RequestImpl::get(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &params,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_HTTPGET, 1L));

  AddHeader add_header(http_handle_, headers);
  AddURL add_url(http_handle_, url, params);

  Response response;
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_WRITEDATA, &response.text_));

  check_curl_correct(curl_easy_perform(http_handle_));

  std::int32_t status_code;
  check_curl_correct(
      curl_easy_getinfo(http_handle_, CURLINFO_RESPONSE_CODE, &status_code));
  response.status_ = static_cast<HttpStatus>(status_code);

  return response;
}

Response Request::RequestImpl::post(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &data,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_HTTPPOST, 1L));

  auto post_fields = splicing_post_fields(data);
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_POSTFIELDS, post_fields.c_str()));

  AddHeader add_header(http_handle_, headers);
  AddURL add_url(http_handle_, url);

  return do_post();
}

Response Request::RequestImpl::post(
    const std::string &url, const std::string &json,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_HTTPPOST, 1L));

  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_POSTFIELDS, json.c_str()));

  auto headers_copy = headers;
  headers_copy["Content-Type"] = "application/json";
  AddHeader add_header(http_handle_, headers_copy);

  AddURL add_url(http_handle_, url);

  return do_post();
}

Response Request::RequestImpl::post_mime(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &data,
    const std::unordered_map<std::string, std::string> &file,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_HTTPPOST, 1L));

  AddForm add_form(http_handle_, data, file);
  AddHeader add_header(http_handle_, headers);
  AddURL add_url(http_handle_, url);

  return do_post();
}

void Request::RequestImpl::set_cookies() {
  if (use_cookies_) {
    check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_COOKIEJAR,
                                        std::data(RequestImpl::cookies_path)));
    check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_COOKIEFILE,
                                        std::data(RequestImpl::cookies_path)));
  } else {
    check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_COOKIEJAR, ""));
    check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_COOKIEFILE, ""));
  }
}

Response Request::RequestImpl::do_post() {
  Response response;

  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_WRITEDATA, &response.text_));

  check_curl_correct(curl_easy_perform(http_handle_));

  std::int32_t status_code;
  check_curl_correct(
      curl_easy_getinfo(http_handle_, CURLINFO_RESPONSE_CODE, &status_code));
  response.status_ = static_cast<HttpStatus>(status_code);

  return response;
}

std::size_t Request::RequestImpl::callback_func_std_string(void *contents,
                                                           std::size_t size,
                                                           std::size_t nmemb,
                                                           std::string *s) {
  s->append(static_cast<const char *>(contents), size * nmemb);
  return size * nmemb;
}

std::string Request::RequestImpl::splicing_post_fields(
    const std::unordered_map<std::string, std::string> &data) {
  std::string result;

  for (const auto &[key, value] : data) {
    result.append(url_encode(key));
    result.append("=");
    result.append(url_encode(value));
    result.append("&");
  }

  if (result.ends_with("&")) {
    result.pop_back();
  }

  return result;
}

Request::Request() : impl_(std::make_unique<RequestImpl>()) {}

Request::~Request() = default;

void Request::verbose(bool flag) { impl_->verbose(flag); }

void Request::allow_redirects(bool flag) { impl_->allow_redirects(flag); }

void Request::set_proxy(const std::string &proxy) { impl_->set_proxy(proxy); }

void Request::set_proxy_from_env() { impl_->set_proxy_from_env(); }

void Request::set_no_proxy() { impl_->set_no_proxy(); }

void Request::set_doh_url(const std::string &url) { impl_->set_doh_url(url); }

void Request::set_user_agent(const std::string &user_agent) {
  impl_->set_user_agent(user_agent);
}

void Request::set_browser_user_agent() { impl_->set_browser_user_agent(); }

void Request::set_curl_user_agent() { impl_->set_curl_user_agent(); }

void Request::set_timeout(std::int64_t seconds) { impl_->set_timeout(seconds); }

void Request::set_connect_timeout(std::int64_t seconds) {
  impl_->set_connect_timeout(seconds);
}

void Request::use_cookies(bool flag) { impl_->use_cookies(flag); }

void Request::set_accept_encoding(const std::string &accept_encoding) {
  impl_->set_accept_encoding(accept_encoding);
}

std::string Request::url_encode(const std::string &str) {
  return impl_->url_encode(str);
}

std::string Request::url_decode(const std::string &str) {
  return impl_->url_decode(str);
}

Response Request::get(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &params,
    const std::unordered_map<std::string, std::string> &header) {
  return impl_->get(url, params, header);
}

Response Request::post(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &data,
    const std::unordered_map<std::string, std::string> &header) {
  return impl_->post(url, data, header);
}

Response Request::post(
    const std::string &url, const std::string &json,
    const std::unordered_map<std::string, std::string> &header) {
  return impl_->post(url, json, header);
}

Response Request::post_mime(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &data,
    const std::unordered_map<std::string, std::string> &file,
    const std::unordered_map<std::string, std::string> &header) {
  return impl_->post_mime(url, data, file, header);
}

HttpStatus Response::status() const { return status_; }

bool Response::ok() const { return status_ == HttpStatus::HTTP_STATUS_OK; }

std::string Response::text() const { return text_; }

void Response::save_to_file(const std::string &path, bool binary_mode) const {
  write_file(path, binary_mode, text_);
}

}  // namespace klib
