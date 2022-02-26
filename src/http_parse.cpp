/**
 * @see https://github.com/nodejs/llhttp#usage
 */

#include "klib/http_parse.h"

#include <charconv>
#include <cstddef>
#include <utility>

#include <llhttp.h>
#include <boost/algorithm/string.hpp>

#include "http-parser/http_parser.h"

namespace klib {

namespace {

class Header {
 public:
  std::string url_;
  std::vector<std::pair<std::string, std::string>> field_value_;
  std::string_view body_;
};

std::int32_t on_url(llhttp_t* parser, const char* at, std::size_t length) {
  auto data = static_cast<Header*>(parser->data);
  data->url_ = std::string_view(at, length);
  return 0;
}

std::int32_t on_header_field(llhttp_t* parser, const char* at,
                             std::size_t length) {
  static_cast<Header*>(parser->data)
      ->field_value_.emplace_back(std::string_view(at, length), "");
  return 0;
}

std::int32_t on_header_value(llhttp_t* parser, const char* at,
                             std::size_t length) {
  static_cast<Header*>(parser->data)->field_value_.back().second =
      std::string_view(at, length);
  return 0;
}

std::int32_t on_body(llhttp_t* parser, const char* at, std::size_t length) {
  auto data = static_cast<Header*>(parser->data);
  data->body_ = std::string_view(at, length);
  return 0;
}

}  // namespace

URL::URL(std::string url) : url_(std::move(url)) {
  auto str = url_.c_str();

  http_parser_url parser;
  http_parser_url_init(&parser);
  http_parser_parse_url(str, std::size(url_), false, &parser);

  const auto& field_data = parser.field_data;
  schema_ = std::string_view(str + field_data[UF_SCHEMA].off,
                             field_data[UF_SCHEMA].len);
  host_ =
      std::string_view(str + field_data[UF_HOST].off, field_data[UF_HOST].len);

  auto port_begin = str + field_data[UF_PORT].off;
  auto port_size = field_data[UF_PORT].len;
  std::from_chars(port_begin, port_begin + port_size, port_);

  path_ =
      std::string_view(str + field_data[UF_PATH].off, field_data[UF_PATH].len);
  query_ = std::string_view(str + field_data[UF_QUERY].off,
                            field_data[UF_QUERY].len);
  fragment_ = std::string_view(str + field_data[UF_FRAGMENT].off,
                               field_data[UF_FRAGMENT].len);
  user_info_ = std::string_view(str + field_data[UF_USERINFO].off,
                                field_data[UF_USERINFO].len);
}

std::unordered_map<std::string, std::string> URL::query_map() const {
  std::unordered_map<std::string, std::string> result;

  std::vector<std::string> query;
  for (const auto& item : boost::split(query, query_, boost::is_any_of("&"),
                                       boost::token_compress_on)) {
    auto index = item.find('=');
    result.emplace(item.substr(0, index), item.substr(index + 1));
  }

  return result;
}

HTTPHeader::HTTPHeader(std::string header) : header_(std::move(header)) {
  llhttp_settings_t settings;
  llhttp_settings_init(&settings);
  settings.on_url = on_url;
  settings.on_header_field = on_header_field;
  settings.on_header_value = on_header_value;
  settings.on_body = on_body;

  llhttp_t parser;
  llhttp_init(&parser, HTTP_BOTH, &settings);

  Header result;
  parser.data = &result;

  auto err = llhttp_execute(&parser, header_.c_str(), std::size(header_));
  if (err != HPE_OK) [[unlikely]] {
    throw RuntimeError("llhttp_execute failed");
  }

  status_ = static_cast<HttpStatus>(parser.status_code);
  method_ = static_cast<HttpMethod>(parser.method);
  http_major_ = parser.http_major;
  http_minor_ = parser.http_minor;

  url_ = URL(std::move(result.url_));
  body_ = result.body_;

  for (const auto& [field, value] : result.field_value_) {
    if (std::empty(value)) {
      continue;
    }

    auto lower_field = boost::to_lower_copy(field);

    if (field_value_.contains(lower_field)) {
      auto& old_value = field_value_[lower_field];
      old_value.append(", ").append(value);
    } else {
      field_value_.emplace(lower_field, value);
    }
  }
}

const std::string& HTTPHeader::value(const std::string& field) const {
  auto lower_field = boost::to_lower_copy(field);
  return field_value_.at(lower_field);
}

bool HTTPHeader::contains(const std::string& field) const {
  auto lower_field = boost::to_lower_copy(field);
  return field_value_.contains(lower_field);
}

}  // namespace klib
