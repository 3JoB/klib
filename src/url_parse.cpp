#include "klib/url_parse.h"

#include <charconv>
#include <utility>

#include <boost/algorithm/string.hpp>

#include "http-parser/http_parser.h"

namespace klib {

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

phmap::flat_hash_map<std::string, std::string> URL::query_map() const {
  phmap::flat_hash_map<std::string, std::string> result;

  std::vector<std::string> query;
  for (const auto& item : boost::split(query, query_, boost::is_any_of("&"),
                                       boost::token_compress_on)) {
    auto index = item.find('=');
    result.emplace(item.substr(0, index), item.substr(index + 1));
  }

  return result;
}

}  // namespace klib