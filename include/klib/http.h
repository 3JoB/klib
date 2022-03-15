/**
 * @file http.h
 * @brief Contains HTTP module
 */

#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <unordered_map>

#include "klib/config.h"
#include "klib/detail/http-inl.h"
#include "klib/exception.h"

namespace klib {

/**
 * @brief HTTP Status
 * @see https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status
 */
enum class KLIB_PUBLIC HttpStatus {
#define XX(num, name, string) HTTP_STATUS_##name = num,
  KLIB_HTTP_STATUS_MAP(XX)
#undef XX
};

/**
 * @brief Get the description string of HTTP Status
 * @param http_status: HTTP Status
 * @return The description string
 */
inline std::string KLIB_PUBLIC http_status_str(HttpStatus http_status) {
  switch (http_status) {
#define XX(num, name, string)          \
  case HttpStatus::HTTP_STATUS_##name: \
    return #string;
    KLIB_HTTP_STATUS_MAP(XX)
#undef XX
    default:
      throw InvalidArgument("Unknown HTTP status");
  }
}

#undef KLIB_HTTP_STATUS_MAP

/**
 * @brief HTTP request methods
 * @see https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Methods
 */
enum class KLIB_PUBLIC HttpMethod {
#define XX(num, name, string) HTTP_METHOD_##name = num,
  KLIB_HTTP_METHOD_MAP(XX)
#undef XX
};

/**
 * @brief Get the description string of HTTP request method
 * @param http_method: HTTP request method
 * @return The description string
 */
inline std::string KLIB_PUBLIC http_method_str(HttpMethod http_method) {
  switch (http_method) {
#define XX(num, name, string)          \
  case HttpMethod::HTTP_METHOD_##name: \
    return #string;
    KLIB_HTTP_METHOD_MAP(XX)
#undef XX
    default:
      throw InvalidArgument("Unknown Http method");
  }
}

#undef KLIB_HTTP_METHOD_MAP

class Response;

/**
 * @brief Constructs and sends a Request
 */
class KLIB_PUBLIC Request {
  friend class Response;

 public:
  /**
   * @brief Default constructor
   */
  Request();

  Request(const Request &) = delete;
  Request(Request &&) = delete;
  Request &operator=(const Request &) = delete;
  Request &operator=(Request &&) = delete;

  /**
   * @brief Destructor
   */
  ~Request();

  /**
   * @brief Whether to display verbose information(The default is false)
   * @param flag: True to display verbose information
   */
  void verbose(bool flag);

  /**
   * @brief Set up proxy
   * @param proxy: String representing proxy
   * @example request.set_proxy("http://127.0.0.1:1080");
   */
  void set_proxy(const std::string &proxy);

  /**
   * @brief Read proxy from environment variable and set
   */
  void set_proxy_from_env();

  /**
   * @brief Don't use any proxy
   * @param no_proxy: String representing no_proxy
   */
  void set_no_proxy(const std::string &no_proxy = "*");

  /**
   * @brief Set up a URL for the DoH server
   * @param url: DoH server URL
   * @example request.set_doh_url("https://dns.google/dns-query");
   * @see https://github.com/curl/curl/wiki/DNS-over-HTTPS
   */
  void set_doh_url(const std::string &url);

  /**
   * @brief Set up user agent
   * @param user_agent: String representing user agent
   */
  void set_user_agent(const std::string &user_agent);

  /**
   * @brief Set up browser user agent
   */
  void set_browser_user_agent();

  /**
   * @brief Set up curl user agent
   */
  void set_curl_user_agent();

  /**
   * @brief Set up timeout
   * @param seconds: Time in seconds
   */
  void set_timeout(std::int64_t seconds);

  /**
   * @brief Set up connect timeout
   * @param seconds: Time in seconds
   */
  void set_connect_timeout(std::int64_t seconds);

  /**
   * @brief Set Accept-Encoding
   * @param accept_encoding: Accept-Encoding
   * @example request.set_accept_encoding("gzip, deflate, br");
   */
  void set_accept_encoding(const std::string &accept_encoding);

  /**
   * @brief Set cookie
   * @param cookies: Cookies
   */
  void set_cookie(const std::unordered_map<std::string, std::string> &cookies);

  /**
   * @brief Encoding URL string
   * @param str: String to be encoded
   * @return Encoded string
   */
  std::string url_encode(const std::string &str);

  /**
   * @brief Decoding URL string
   * @param str: String to be decoded
   * @return Decoded string
   */
  std::string url_decode(const std::string &str);

  /**
   * @brief Sends a GET request
   * @param url: Requested url
   * @param params: URL parameters
   * @param headers: HTTP headers
   * @return Response content
   */
  Response get(
      const std::string &url,
      const std::unordered_map<std::string, std::string> &params = {},
      const std::unordered_map<std::string, std::string> &headers = {});

  /**
   * @brief Sends a POST request
   * @param url: Requested url
   * @param data: Data name and value
   * @param headers: HTTP headers
   * @return Response content
   */
  Response post(
      const std::string &url,
      const std::unordered_map<std::string, std::string> &data,
      const std::unordered_map<std::string, std::string> &headers = {});

  /**
   * @brief Sends a POST request
   * @param url: Requested url
   * @param json: Data in json format
   * @param headers: HTTP headers
   * @return Response content
   */
  Response post(
      const std::string &url, const std::string &json,
      const std::unordered_map<std::string, std::string> &headers = {});

  /**
   * @brief Sends a POST request
   * @param url: Requested url
   * @param data: Data name and value
   * @param file: File name and path
   * @param headers: HTTP headers
   * @return Response content
   */
  Response post_mime(
      const std::string &url,
      const std::unordered_map<std::string, std::string> &data,
      const std::unordered_map<std::string, std::string> &file,
      const std::unordered_map<std::string, std::string> &headers = {});

 private:
  class RequestImpl;
  std::experimental::propagate_const<std::unique_ptr<RequestImpl>> impl_;
};

/**
 * @brief Response content
 */
class KLIB_PUBLIC Response {
  friend class Request::RequestImpl;

 public:
  /**
   * @brief Default constructor
   */
  Response();

  /**
   * @brief Get status code
   * @return Status code
   */
  [[nodiscard]] HttpStatus status() const;

  /**
   * @brief Determine whether the status code is OK
   * @return True if the status code is OK
   */
  [[nodiscard]] bool ok() const;

  /**
   * @brief Get response content
   * @return Response content
   */
  [[nodiscard]] std::string text() const;

  /**
   * @brief Save response content to file
   * @param binary_mode: Whether to open in binary mode
   * @param path: File path
   */
  void save_to_file(const std::string &path) const;

 private:
  HttpStatus status_;
  std::string text_;
};

}  // namespace klib
