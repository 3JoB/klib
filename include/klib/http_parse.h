/**
 * @file url.h
 * @brief Contains HTTP parse module
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace klib {

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
  [[nodiscard]] std::int32_t status_code() const { return status_code_; }
  [[nodiscard]] std::int32_t method() const { return method_; }
  [[nodiscard]] std::int32_t http_major() const { return http_major_; }
  [[nodiscard]] std::int32_t http_minor() const { return http_minor_; }
  [[nodiscard]] const URL& url() const { return url_; }
  [[nodiscard]] const std::string& body() const { return body_; }

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

  std::int32_t status_code_;
  std::int32_t method_;
  std::int32_t http_major_;
  std::int32_t http_minor_;

  URL url_;
  std::unordered_map<std::string, std::string> field_value_;
  std::string body_;
};

}  // namespace klib
