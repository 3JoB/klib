/**
 * @file url_parse.h
 * @brief Contains URL parse module
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <parallel_hashmap/phmap.h>

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
   * @brief Get URL schema
   * @return URL schema
   * @example url.schema() == "https"
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view schema() const noexcept { return schema_; }

  /**
   * @brief Get URL host
   * @return URL host
   * @example url.host() == "www.example.com"
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view host() const noexcept { return host_; }

  /**
   * @brief Get URL port
   * @return URL port
   * @example url.port() == 443
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::int32_t port() const noexcept { return port_; }

  /**
   * @brief Get URL path
   * @return URL path
   * @example url.path() == "/path/to/index.html"
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view path() const noexcept { return path_; }

  /**
   * @brief Get URL query
   * @return URL query
   * @example url.query() == "key1=value1&key2=value2"
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view query() const noexcept { return query_; }

  /**
   * @brief Get URL fragment
   * @return URL fragment
   * @example url = "https://www.example.com/index.html#Somewhere"
   * @example url.fragment() == "Somewhere"
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view fragment() const noexcept { return fragment_; }

  /**
   * @brief Get URL user info
   * @return URL user info
   * @example url = "https://user:password@example.com"
   * @example url.user_info() == "user:password"
   * @see https://en.wikipedia.org/wiki/URL
   */
  [[nodiscard]] std::string_view user_info() const noexcept {
    return user_info_;
  }

  /**
   * @brief Parse query parameters
   * @return Query key-value pairs
   */
  [[nodiscard]] phmap::flat_hash_map<std::string, std::string> query_map()
      const;

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

}  // namespace klib
