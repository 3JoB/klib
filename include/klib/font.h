#pragma once

#include <string>

namespace klib {

void font_subset(const std::string &font_path, const std::string &out_name,
                 const std::u32string &unicodes);

}
