/**
 * @file http.h
 * @brief Contains an HTTP module
 */

#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <unordered_map>

namespace klib {

class Response;

/**
 * @brief Constructs and sends a Request
 */
class Request {
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
   * @brief Whether to follow any location(The default is true)
   * @param flag: True to follow any location
   */
  void allow_redirects(bool flag);

  /**
   * @brief Set up proxy
   * @param proxy: String representing proxy
   * @example request.set_proxy("http://127.0.0.1:1080");
   */
  void set_proxy(const std::string &proxy);

  /**
   * @brief No proxy
   */
  void set_no_proxy();

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
   * @brief Use cookies
   * @param flag: True to use cookies
   */
  void use_cookies(bool flag);

  /**
   * @brief Set Accept-Encoding
   * @param accept_encoding: Accept-Encoding
   * @example request.set_accept_encoding("gzip, deflate, br");
   */
  void set_accept_encoding(const std::string &accept_encoding);

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
   * @param data: Data string
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
   * @param file: File name and content
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
 * @brief Response headers
 */
class Headers {
  friend class Response;

 public:
  /**
   * @brief Get the value specified by key(not case sensitive)
   * @param key: Key
   * @return The value
   */
  [[nodiscard]] const std::string &at(const std::string &key) const;

  bool empty() const { return std::empty(map_); }

 private:
  void add(const std::string &key, const std::string &value);

  std::unordered_map<std::string, std::string> map_;
};

/**
 * @brief Response content
 */
class Response {
  friend class Request::RequestImpl;

 public:
  /**
   * @brief HTTP Status Code
   * @see https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status
   */
  enum StatusCode : std::int64_t {
    None = 0,
    Ok = 200,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404
  };

  /**
   * @brief Get status code
   * @return Status code
   */
  [[nodiscard]] std::int64_t status_code() const;

  /**
   * @brief Determine whether the status code is OK
   * @return True if the status code is OK
   */
  [[nodiscard]] bool ok() const;

  /**
   * @brief Get server response
   * @return Server response
   */
  [[nodiscard]] const Headers &headers_map();

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
  void save_to_file(const std::string &path, bool binary_mode) const;

 private:
  std::int64_t status_code_ = StatusCode::None;
  std::string headers_;
  std::string text_;

  Headers headers_map_;
};

}  // namespace klib
