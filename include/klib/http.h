/**
 * @file http.h
 * @brief Contains an HTTP module
 */

#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>

namespace klib::http {

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
   */
  void set_proxy(const std::string &proxy);

  /**
   * @brief No proxy
   */
  void set_no_proxy();

  /**
   * @brief Set up user agent
   * @param user_agent: String representing user agent
   */
  void set_user_agent(const std::string &user_agent);

  /**
   * @brief Sends a GET request
   * @param url: Requested url
   * @return Response content
   */
  Response get(const std::string &url);

 private:
  class RequestImpl;
  std::experimental::propagate_const<std::unique_ptr<RequestImpl>> impl_;
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
  enum StatusCode : std::int64_t { None, Ok = 200 };

  /**
   * @brief Get status code
   * @return Status code
   */
  [[nodiscard]] std::int64_t status_code() const;

  /**
   * @brief Get server response
   * @return Server response
   */
  [[nodiscard]] std::string header() const;

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
  std::string header_;
  std::string text_;
};

}  // namespace klib::http
