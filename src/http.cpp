#include "klib/http.h"

#include <cstddef>
#include <memory>

#include <curl/curl.h>

#include "klib/exception.h"
#include "klib/util.h"

// https://curl.se/libcurl/c/multi-single.html
// https://curl.se/libcurl/c/http2-download.html
namespace klib::http {

namespace {

void check_curl_correct(CURLcode code) {
  if (code != CURLcode::CURLE_OK) {
    throw klib::RuntimeError(curl_easy_strerror(code));
  }
}

void check_curl_correct(CURLMcode code) {
  if (code != CURLMcode::CURLM_OK) {
    throw klib::RuntimeError(curl_multi_strerror(code));
  }
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
  void set_no_proxy();
  void set_user_agent(const std::string &user_agent);

  Response get(const std::string &url);

 private:
  static std::size_t callback_func_std_string(void *contents, std::size_t size,
                                              std::size_t nmemb,
                                              std::string *s);

  CURL *http_handle_;
};

Request::RequestImpl::RequestImpl() {
  check_curl_correct(curl_global_init(CURL_GLOBAL_DEFAULT));

  http_handle_ = curl_easy_init();
  if (!http_handle_) {
    throw klib::RuntimeError("curl_easy_init() error");
  }

  try {
    check_curl_correct(
        curl_easy_setopt(http_handle_, CURLOPT_SSL_VERIFYPEER, 1L));
    check_curl_correct(
        curl_easy_setopt(http_handle_, CURLOPT_SSL_VERIFYHOST, 2L));
    check_curl_correct(
        curl_easy_setopt(http_handle_, CURLOPT_CAPATH, "/etc/ssl/certs"));
    check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_CAINFO,
                                        "/etc/ssl/certs/ca-certificates.crt"));
    check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_HTTP_VERSION,
                                        CURL_HTTP_VERSION_2_0));

    check_curl_correct(
        curl_easy_setopt(http_handle_, CURLOPT_FOLLOWLOCATION, 1L));

    // navigator.userAgent
    check_curl_correct(
        curl_easy_setopt(http_handle_, CURLOPT_USERAGENT,
                         "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                         "(KHTML, like Gecko) "
                         "Chrome/93.0.4577.18 Safari/537.36 Edg/93.0.961.11"));
  } catch (const klib::RuntimeError &error) {
    curl_easy_cleanup(http_handle_);
    curl_global_cleanup();

    throw error;
  }
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

void Request::RequestImpl::set_no_proxy() {
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_NOPROXY, "*"));
}

void Request::RequestImpl::set_user_agent(const std::string &user_agent) {
  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_USERAGENT, user_agent.c_str()));
}

// TODO check url
Response Request::RequestImpl::get(const std::string &url) {
  Response response;

  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_URL, url.c_str()));

  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_WRITEDATA, &response.text_));
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_WRITEFUNCTION,
                                      RequestImpl::callback_func_std_string));

  check_curl_correct(
      curl_easy_setopt(http_handle_, CURLOPT_HEADERDATA, &response.header_));
  check_curl_correct(curl_easy_setopt(http_handle_, CURLOPT_HEADERFUNCTION,
                                      callback_func_std_string));

  auto free_multi_handle = [this](CURLM *multi) {
    check_curl_correct(curl_multi_remove_handle(multi, this->http_handle_));
    check_curl_correct(curl_multi_cleanup(multi));
  };
  auto multi_handle = std::unique_ptr<CURLM, decltype(free_multi_handle)>(
      curl_multi_init(), free_multi_handle);

  if (!multi_handle) {
    throw klib::RuntimeError("create multi_handle error");
  }

  check_curl_correct(curl_multi_add_handle(multi_handle.get(), http_handle_));

  std::int32_t still_running = 1;
  do {
    check_curl_correct(curl_multi_perform(multi_handle.get(), &still_running));
    check_curl_correct(
        curl_multi_poll(multi_handle.get(), nullptr, 0, 1000, nullptr));
  } while (still_running);

  check_curl_correct(curl_easy_getinfo(http_handle_, CURLINFO_RESPONSE_CODE,
                                       &response.status_code_));

  return response;
}

std::size_t Request::RequestImpl::callback_func_std_string(void *contents,
                                                           std::size_t size,
                                                           std::size_t nmemb,
                                                           std::string *s) {
  s->append(static_cast<const char *>(contents), size * nmemb);
  return size * nmemb;
}

Request::Request() : impl_(std::make_unique<RequestImpl>()) {}

Request::~Request() = default;

void Request::verbose(bool flag) { impl_->verbose(flag); }

void Request::allow_redirects(bool flag) { impl_->allow_redirects(flag); }

void Request::set_proxy(const std::string &proxy) { impl_->set_proxy(proxy); }

void Request::set_no_proxy() { impl_->set_no_proxy(); }

void Request::set_user_agent(const std::string &user_agent) {
  impl_->set_user_agent(user_agent);
}

Response Request::get(const std::string &url) { return impl_->get(url); }

std::int64_t Response::status_code() const { return status_code_; }

std::string Response::header() const { return header_; }

std::string Response::text() const { return text_; }

void Response::save_to_file(const std::string &path, bool binary_mode) const {
  klib::util::write_file(path, binary_mode, text_);
}

}  // namespace klib::http
