#pragma once

#include <string>

namespace klib::epub {

std::string trans_str(const std::string &str);

std::string get_date(std::string_view time_zone = "Asia/Shanghai");

}  // namespace klib::epub
