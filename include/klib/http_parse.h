/**
 * @file url.h
 * @brief Contains HTTP parse module
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

#include "klib/detail/http_parse-inl.h"
#include "klib/exception.h"

namespace klib {

/**
 * @brief HTTP Status
 * @see https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status
 */
enum class HttpStatus {
#define XX(num, name, string) HTTP_STATUS_##name = num,
  KLIB_HTTP_STATUS_MAP(XX)
#undef XX
};

/**
 * @brief Get the description string of HTTP Status
 * @param http_status: HTTP Status
 * @return The description string
 */
inline std::string http_status_str(HttpStatus http_status) {
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
enum class HttpMethod {
#define XX(num, name, string) HTTP_METHOD_##name = num,
  KLIB_HTTP_METHOD_MAP(XX)
#undef XX
};

/**
 * @brief Get the description string of HTTP request method
 * @param http_method: HTTP request method
 * @return The description string
 */
inline std::string http_method_str(HttpMethod http_method) {
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

/**
 * @brief Parse URL
 */
class URL {
 public:
  URL() = default;
  /**
   * @brief Constructor
   * @param url: URL to be parsed
   */
  explicit URL(std::string url);

  /**
   * @brief Get parts of the URL
   * @return parts of the URL
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view schema() const { return schema_; }
  [[nodiscard]] std::string_view host() const { return host_; }
  [[nodiscard]] std::int32_t port() const { return port_; }
  [[nodiscard]] std::string_view path() const { return path_; }
  [[nodiscard]] std::string_view query() const { return query_; }
  [[nodiscard]] std::string_view fragment() const { return fragment_; }
  [[nodiscard]] std::string_view user_info() const { return user_info_; }

  /**
   * @brief Parse query parameters
   * @return Query key-value pairs
   */
  [[nodiscard]] std::unordered_map<std::string, std::string> query_map() const;

 private:
  std::string url_;

  std::string_view schema_;
  std::string_view host_;
  std::int32_t port_ = 0;
  std::string_view path_;
  std::string_view query_;
  std::string_view fragment_;
  std::string_view user_info_;
};

/**
 * @brief HTTP header
 * @see https://developer.mozilla.org/zh-CN/docs/Glossary/HTTP_header
 * @see https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Messages
 */
class HTTPHeader {
 public:
  /**
   * @brief Constructor
   * @param header: HTTP header to be parsed
   */
  explicit HTTPHeader(std::string header);

  /**
   * @brief Get parts of the header
   * @return parts of the header
   */
  [[nodiscard]] HttpStatus status() const { return status_; }
  [[nodiscard]] HttpMethod method() const { return method_; }
  [[nodiscard]] std::int32_t http_major() const { return http_major_; }
  [[nodiscard]] std::int32_t http_minor() const { return http_minor_; }
  [[nodiscard]] const std::string& uri() const { return uri_; }
  [[nodiscard]] std::string_view body() const { return body_; }

  /**
   * @brief Query the value corresponding to the field
   * @param field: Field name
   * @return Corresponding value
   */
  [[nodiscard]] const std::string& value(const std::string& field) const;

  /**
   * @brief Check if this field is included
   * @param field: Field name
   * @return Return true if exists
   */
  [[nodiscard]] bool contains(const std::string& field) const;

 private:
  std::string header_;

  HttpStatus status_;
  HttpMethod method_;
  std::int32_t http_major_;
  std::int32_t http_minor_;

  std::string uri_;
  std::unordered_map<std::string, std::string> field_value_;
  std::string_view body_;
};

}  // namespace klib
