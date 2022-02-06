/**
 * @file url.h
 * @brief Contains URL parse module
 */

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace klib {

class URL {
 public:
  explicit URL(std::string url);

  [[nodiscard]] std::string_view schema() const { return schema_; }
  [[nodiscard]] std::string_view host() const { return host_; }
  [[nodiscard]] std::string_view port() const { return port_; }
  [[nodiscard]] std::string_view path() const { return path_; }
  [[nodiscard]] std::string_view query() const { return query_; }
  [[nodiscard]] std::string_view fragment() const { return fragment_; }
  [[nodiscard]] std::string_view user_info() const { return user_info_; }

  [[nodiscard]] std::unordered_map<std::string, std::string> query_map() const;

 private:
  std::string url_;

  std::string_view schema_;
  std::string_view host_;
  std::string_view port_;
  std::string_view path_;
  std::string_view query_;
  std::string_view fragment_;
  std::string_view user_info_;
};

}  // namespace klib
