#include "klib/http.h"

#include <cstddef>
#include <filesystem>
#include <fstream>

#include <curl/curl.h>

#include "klib/detail/error.h"

// https://curl.se/libcurl/c/multi-single.html
// https://curl.se/libcurl/c/http2-download.html
namespace klib::http {

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
  void set_user_agent(const std::string &user_agent);

  Response get(const std::string &url);

 private:
  static std::size_t callback_func_std_string(void *contents, std::size_t size,
                                              std::size_t nmemb,
                                              std::string *s);

  CURL *http_handle_;
};

Request::RequestImpl::RequestImpl() {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  http_handle_ = curl_easy_init();
  if (!http_handle_) {
    detail::error("curl_easy_init() error");
  }

  curl_easy_setopt(http_handle_, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(http_handle_, CURLOPT_SSL_VERIFYHOST, 2L);
  curl_easy_setopt(http_handle_, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
  curl_easy_setopt(http_handle_, CURLOPT_CAPATH, "/etc/ssl/certs");
  curl_easy_setopt(http_handle_, CURLOPT_CAINFO,
                   "/etc/ssl/certs/ca-certificates.crt");

  curl_easy_setopt(http_handle_, CURLOPT_FOLLOWLOCATION, 1L);

  // navigator.userAgent
  curl_easy_setopt(
      http_handle_, CURLOPT_USERAGENT,
      "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) "
      "Chrome/92.0.4515.107 Safari/537.36 Edg/92.0.902.55");
}

Request::RequestImpl::~RequestImpl() {
  curl_easy_cleanup(http_handle_);
  curl_global_cleanup();
}

void Request::RequestImpl::verbose(bool flag) {
  curl_easy_setopt(http_handle_, CURLOPT_VERBOSE, flag);
}

void Request::RequestImpl::allow_redirects(bool flag) {
  curl_easy_setopt(http_handle_, CURLOPT_FOLLOWLOCATION, flag);
}

void Request::RequestImpl::set_proxy(const std::string &proxy) {
  curl_easy_setopt(http_handle_, CURLOPT_PROXY, proxy.c_str());
}

void Request::RequestImpl::set_user_agent(const std::string &user_agent) {
  curl_easy_setopt(http_handle_, CURLOPT_USERAGENT, user_agent.c_str());
}

Response Request::RequestImpl::get(const std::string &url) {
  Response response;

  curl_easy_setopt(http_handle_, CURLOPT_URL, url.c_str());

  curl_easy_setopt(http_handle_, CURLOPT_WRITEDATA, &response.text_);
  curl_easy_setopt(http_handle_, CURLOPT_WRITEFUNCTION,
                   RequestImpl::callback_func_std_string);

  curl_easy_setopt(http_handle_, CURLOPT_HEADERDATA, &response.header_);
  curl_easy_setopt(http_handle_, CURLOPT_HEADERFUNCTION,
                   callback_func_std_string);

  auto multi_handle = curl_multi_init();
  curl_multi_add_handle(multi_handle, http_handle_);

  std::int32_t still_running = 1;

  do {
    if (curl_multi_perform(multi_handle, &still_running) != CURLM_OK) {
      detail::error("curl_multi_perform() error");
    }

    if (curl_multi_poll(multi_handle, nullptr, 0, 1000, nullptr) != CURLM_OK) {
      detail::error("curl_multi_poll() error");
    }
  } while (still_running);

  curl_easy_getinfo(http_handle_, CURLINFO_RESPONSE_CODE,
                    &response.status_code_);

  curl_multi_remove_handle(multi_handle, http_handle_);
  curl_multi_cleanup(multi_handle);

  return response;
}

std::size_t Request::RequestImpl::callback_func_std_string(void *contents,
                                                           std::size_t size,
                                                           std::size_t nmemb,
                                                           std::string *s) {
  s->append(static_cast<char *>(contents), size * nmemb);
  return size * nmemb;
}

Request::Request() : impl_(std::make_unique<RequestImpl>()) {}

Request::~Request() = default;

void Request::verbose(bool flag) { impl_->verbose(flag); }

void Request::allow_redirects(bool flag) { impl_->allow_redirects(flag); }

void Request::set_proxy(const std::string &proxy) { impl_->set_proxy(proxy); }

void Request::set_user_agent(const std::string &user_agent) {
  impl_->set_user_agent(user_agent);
}

Response Request::get(const std::string &url) { return impl_->get(url); }

std::int64_t Response::status_code() const { return status_code_; }

std::string Response::header() const { return header_; }

std::string Response::text() const { return text_; }

void Response::save(const std::string &path) const {
  std::ofstream ofs(path);
  ofs << text_ << std::flush;
}

}  // namespace klib::http
