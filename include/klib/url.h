/**
 * @file url.h
 * @brief Contains URL module
 */

#pragma once

#include <cstdint>
#include <string>

#include <parallel_hashmap/phmap.h>

namespace klib {

/**
 * @brief Parse and build URL
 * @see https://en.wikipedia.org/wiki/URL
 */
class URL {
 public:
  /**
   * @brief Default constructor
   */
  URL() = default;
  /**
   * @brief Constructor
   * @param url: URL to be parsed
   */
  explicit URL(const std::string &url);

  /**
   * @brief Build URL
   * @return The URL
   */
  [[nodiscard]] std::string build() const;

  /**
   * @brief Get URL schema
   * @return URL schema
   * @example url.schema() == "https"
   */
  [[nodiscard]] const std::string &schema() const { return schema_; }

  /**
   * @brief Set URL schema
   * @param schema: URL schema
   */
  void set_schema(const std::string &schema) { schema_ = schema; }

  /**
   * @brief Get URL host
   * @return URL host
   * @example url.host() == "www.example.com"
   */
  [[nodiscard]] const std::string &host() const { return host_; }

  /**
   * @brief Set URL host
   * @param host: URL host
   */
  void set_host(const std::string &host) { host_ = host; }

  /**
   * @brief Get URL port
   * @return URL port
   * @example url.port() == 443
   */
  [[nodiscard]] std::int32_t port() const { return port_; }

  /**
   * @brief Set URL port
   * @param port: URL port
   */
  void set_port(std::int32_t port) { port_ = port; }

  /**
   * @brief Get URL path
   * @return URL path
   * @example url.path() == "/path/to/index.html"
   */
  [[nodiscard]] const std::string &path() const { return path_; }

  /**
   * @brief Set URL path
   * @param path: URL path
   */
  void set_path(const std::string &path) { path_ = path; }

  /**
   * @brief Get URL query
   * @return URL query
   */
  [[nodiscard]] const phmap::flat_hash_map<std::string, std::string> &query()
      const {
    return query_;
  }

  /**
   * @brief Set URL query
   * @param query: URL query
   */
  void set_query(const phmap::flat_hash_map<std::string, std::string> &query) {
    query_ = query;
  }

  /**
   * @brief Get URL fragment
   * @return URL fragment
   * @example url = "https://www.example.com/index.html#Somewhere"
   * @example url.fragment() == "Somewhere"
   */
  [[nodiscard]] const std::string &fragment() const { return fragment_; }

  /**
   * @brief Set URL fragment
   * @param fragment: URL fragment
   */
  void set_fragment(const std::string &fragment) { fragment_ = fragment; }

  /**
   * @brief Get URL user
   * @return URL user
   * @example url = "https://user:password@example.com"
   * @example url.user() == "user"
   */
  [[nodiscard]] const std::string &user() const { return user_; }

  /**
   * @brief Set URL user
   * @param user_info: URL user
   */
  void set_user(const std::string &user) { user_ = user; }

  /**
   * @brief Get URL password
   * @return URL password
   * @example url = "https://user:password@example.com"
   * @example url.password() == "password"
   */
  [[nodiscard]] const std::string &password() const { return password_; }

  /**
   * @brief Set URL password
   * @param user_info: URL password
   */
  void set_password(const std::string &password) { password_ = password; }

 private:
  std::string schema_ = "https";
  std::string host_;
  std::int32_t port_ = 0;
  std::string path_;
  phmap::flat_hash_map<std::string, std::string> query_;
  std::string fragment_;
  std::string user_;
  std::string password_;
};

}  // namespace klib
