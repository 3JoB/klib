/**
 * @see https://curl.se/libcurl/c/example.html
 * @see curl -s -o /dev/null --noproxy '*' --libcurl a.c https://www.baidu.com/
 */

#include "klib/http.h"

#include <cstddef>
#include <filesystem>
#include <string_view>

#include <curl/curl.h>
#include <scope_guard.hpp>

#include "klib/exception.h"
#include "klib/util.h"

namespace klib {

namespace {

#define check_curl(rc)                            \
  do {                                            \
    if (rc != CURLcode::CURLE_OK) {               \
      throw RuntimeError(curl_easy_strerror(rc)); \
    }                                             \
  } while (0)

#define check_curl_url(rc)                       \
  do {                                           \
    if (rc != CURLUcode::CURLUE_OK) {            \
      throw RuntimeError(curl_url_strerror(rc)); \
    }                                            \
  } while (0)

HttpStatus get_status(CURL *curl) {
  std::int32_t status_code;

  auto rc = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
  check_curl(rc);

  return static_cast<HttpStatus>(status_code);
}

CURLU *add_url(
    CURL *curl, const std::string &url,
    const std::unordered_map<std::string, std::string> &params = {}) {
  if (std::empty(params)) {
    auto rc = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    check_curl(rc);
    return nullptr;
  }

  auto c_url = curl_url();

  auto rc = curl_url_set(c_url, CURLUPART_URL, url.c_str(), 0);
  check_curl_url(rc);

  for (const auto &[key, value] : params) {
    std::string query;
    query.append(key).append("=").append(value);

    rc = curl_url_set(c_url, CURLUPART_QUERY, query.c_str(),
                      CURLU_APPENDQUERY | CURLU_URLENCODE);
    check_curl_url(rc);
  }

  auto rc2 = curl_easy_setopt(curl, CURLOPT_CURLU, c_url);
  check_curl(rc2);

  return c_url;
}

curl_slist *add_header(
    CURL *curl, const std::unordered_map<std::string, std::string> &headers) {
  if (std::empty(headers)) {
    return nullptr;
  }

  curl_slist *chunk = nullptr;

  for (const auto &[key, value] : headers) {
    if (std::empty(key) || std::empty(value)) {
      throw RuntimeError("The header key and value can not be empty");
    }

    std::string str = key;
    str.append(": ").append(value);
    chunk = curl_slist_append(chunk, str.c_str());
  }

  auto rc = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
  check_curl(rc);

  return chunk;
}

curl_mime *add_form(CURL *curl,
                    const std::unordered_map<std::string, std::string> &data,
                    const std::unordered_map<std::string, std::string> &file) {
  if (std::empty(data) && std::empty(file)) {
    return nullptr;
  }

  auto form = curl_mime_init(curl);

  for (const auto &[key, value] : data) {
    if (std::empty(key) || std::empty(value)) {
      throw RuntimeError("The post form key and value can not be empty");
    }

    auto field = curl_mime_addpart(form);
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

    auto field = curl_mime_addpart(form);
    curl_mime_name(field, file_name.c_str());
    curl_mime_filedata(field, path.c_str());
  }

  auto rc = curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
  check_curl(rc);

  return form;
}

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
  void set_no_proxy(const std::string &no_proxy);
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
  void set_cookies();
  Response do_post();

  static std::size_t callback_func_std_string(void *contents, std::size_t size,
                                              std::size_t nmemb,
                                              std::string *s);
  std::string splicing_post_fields(
      const std::unordered_map<std::string, std::string> &data);

  CURL *curl_;
  bool use_cookies_ = true;

  constexpr static std::string_view cookies_path = "/tmp/cookies.txt";
  constexpr static std::string_view altsvc_path = "/tmp/altsvc.txt";
};

Request::RequestImpl::RequestImpl() {
  auto rc = curl_global_init(CURL_GLOBAL_DEFAULT);
  check_curl(rc);

  curl_ = curl_easy_init();
  if (!curl_) {
    throw RuntimeError("curl_easy_init failed");
  }
  SCOPE_FAIL {
    curl_easy_cleanup(curl_);
    curl_global_cleanup();
  };

  rc = curl_easy_setopt(curl_, CURLOPT_BUFFERSIZE, 102400);
  check_curl(rc);

  rc = curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1);
  check_curl(rc);

  rc = curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 50);
  check_curl(rc);

  rc = curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1);
  check_curl(rc);

  rc = curl_easy_setopt(curl_, CURLOPT_ALTSVC, std::data(altsvc_path));
  check_curl(rc);

  rc = curl_easy_setopt(curl_, CURLOPT_ALTSVC_CTRL,
                        CURLALTSVC_H1 | CURLALTSVC_H2);
  check_curl(rc);

  rc = curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION,
                        RequestImpl::callback_func_std_string);
  check_curl(rc);
}

Request::RequestImpl::~RequestImpl() {
  curl_easy_cleanup(curl_);
  curl_global_cleanup();
}

void Request::RequestImpl::verbose(bool flag) {
  auto rc = curl_easy_setopt(curl_, CURLOPT_VERBOSE, flag);
  check_curl(rc);
}

void Request::RequestImpl::allow_redirects(bool flag) {
  auto rc = curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, flag);
  check_curl(rc);
}

void Request::RequestImpl::set_proxy(const std::string &proxy) {
  auto rc = curl_easy_setopt(curl_, CURLOPT_PROXY, proxy.c_str());
  check_curl(rc);
}

void Request::RequestImpl::set_proxy_from_env() {
  set_proxy(std::getenv("HTTP_PROXY"));
  set_no_proxy(std::getenv("NO_PROXY"));
}

void Request::RequestImpl::set_no_proxy(const std::string &no_proxy) {
  auto rc = curl_easy_setopt(curl_, CURLOPT_NOPROXY, no_proxy.c_str());
  check_curl(rc);
}

void Request::RequestImpl::set_doh_url(const std::string &url) {
  auto rc = curl_easy_setopt(curl_, CURLOPT_DOH_URL, url.c_str());
  check_curl(rc);
}

void Request::RequestImpl::set_user_agent(const std::string &user_agent) {
  auto rc = curl_easy_setopt(curl_, CURLOPT_USERAGENT, user_agent.c_str());
  check_curl(rc);
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
  auto rc = curl_easy_setopt(curl_, CURLOPT_TIMEOUT, seconds);
  check_curl(rc);
}

void Request::RequestImpl::set_connect_timeout(std::int64_t seconds) {
  auto rc = curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, seconds);
  check_curl(rc);
}

void Request::RequestImpl::use_cookies(bool flag) { use_cookies_ = flag; }

void Request::RequestImpl::set_accept_encoding(
    const std::string &accept_encoding) {
  auto rc =
      curl_easy_setopt(curl_, CURLOPT_ACCEPT_ENCODING, accept_encoding.c_str());
  check_curl(rc);
}

std::string Request::RequestImpl::url_encode(const std::string &str) {
  auto ptr = curl_easy_escape(curl_, str.c_str(), std::size(str));
  SCOPE_EXIT { curl_free(ptr); };
  if (!ptr) {
    throw RuntimeError("curl_easy_escape failed");
  }

  return ptr;
}

std::string Request::RequestImpl::url_decode(const std::string &str) {
  std::int32_t length;
  auto ptr = curl_easy_unescape(curl_, str.c_str(), std::size(str), &length);
  SCOPE_EXIT { curl_free(ptr); };
  if (!ptr) {
    throw RuntimeError("curl_easy_unescape failed");
  }

  return std::string(ptr, length);
}

Response Request::RequestImpl::get(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &params,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();

  auto rc = curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
  check_curl(rc);

  auto c_url = add_url(curl_, url, params);
  SCOPE_EXIT { curl_url_cleanup(c_url); };

  auto chunk = add_header(curl_, headers);
  SCOPE_EXIT {
    curl_slist_free_all(chunk);
    rc = curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, nullptr);
    check_curl(rc);
  };

  Response response;
  rc = curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response.text_);
  check_curl(rc);

  rc = curl_easy_perform(curl_);
  check_curl(rc);

  response.status_ = get_status(curl_);

  return response;
}

Response Request::RequestImpl::post(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &data,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();

  auto rc = curl_easy_setopt(curl_, CURLOPT_HTTPPOST, 1L);
  check_curl(rc);

  auto post_fields = splicing_post_fields(data);
  rc = curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, post_fields.c_str());
  check_curl(rc);

  auto c_url = add_url(curl_, url);
  SCOPE_EXIT { curl_url_cleanup(c_url); };

  auto chunk = add_header(curl_, headers);
  SCOPE_EXIT {
    curl_slist_free_all(chunk);
    rc = curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, nullptr);
    check_curl(rc);
  };

  return do_post();
}

Response Request::RequestImpl::post(
    const std::string &url, const std::string &json,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();

  auto rc = curl_easy_setopt(curl_, CURLOPT_HTTPPOST, 1L);
  check_curl(rc);

  rc = curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, json.c_str());
  check_curl(rc);

  auto c_url = add_url(curl_, url);
  SCOPE_EXIT { curl_url_cleanup(c_url); };

  auto headers_copy = headers;
  headers_copy["Content-Type"] = "application/json";
  auto chunk = add_header(curl_, headers_copy);
  SCOPE_EXIT {
    curl_slist_free_all(chunk);
    rc = curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, nullptr);
    check_curl(rc);
  };

  return do_post();
}

Response Request::RequestImpl::post_mime(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &data,
    const std::unordered_map<std::string, std::string> &file,
    const std::unordered_map<std::string, std::string> &headers) {
  set_cookies();

  auto rc = curl_easy_setopt(curl_, CURLOPT_HTTPPOST, 1L);
  check_curl(rc);

  auto c_url = add_url(curl_, url);
  SCOPE_EXIT { curl_url_cleanup(c_url); };

  auto chunk = add_header(curl_, headers);
  SCOPE_EXIT {
    curl_slist_free_all(chunk);
    rc = curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, nullptr);
    check_curl(rc);
  };

  auto form = add_form(curl_, data, file);
  SCOPE_EXIT {
    curl_mime_free(form);
    auto rc = curl_easy_setopt(curl_, CURLOPT_MIMEPOST, nullptr);
    check_curl(rc);
  };

  return do_post();
}

void Request::RequestImpl::set_cookies() {
  if (use_cookies_) {
    auto rc = curl_easy_setopt(curl_, CURLOPT_COOKIEJAR,
                               std::data(RequestImpl::cookies_path));
    check_curl(rc);

    rc = curl_easy_setopt(curl_, CURLOPT_COOKIEFILE,
                          std::data(RequestImpl::cookies_path));
    check_curl(rc);
  } else {
    auto rc = curl_easy_setopt(curl_, CURLOPT_COOKIEJAR, "");
    check_curl(rc);

    rc = curl_easy_setopt(curl_, CURLOPT_COOKIEFILE, "");
    check_curl(rc);
  }
}

Response Request::RequestImpl::do_post() {
  Response response;

  auto rc = curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response.text_);
  check_curl(rc);

  rc = curl_easy_perform(curl_);
  check_curl(rc);

  response.status_ = get_status(curl_);

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

void Request::set_no_proxy(const std::string &no_proxy) {
  impl_->set_no_proxy(no_proxy);
}

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

Response::Response() { text_.reserve(16384); }

HttpStatus Response::status() const { return status_; }

bool Response::ok() const { return status_ == HttpStatus::HTTP_STATUS_OK; }

std::string Response::text() const { return text_; }

void Response::save_to_file(const std::string &path) const {
  write_file(path, true, text_);
}

}  // namespace klib
