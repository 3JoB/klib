#pragma once

#include <string>

namespace klib {

std::string font_subset(const std::string &font_path,
                        const std::u32string &unicodes);

std::string ttf_to_woff2(const std::string &ttf_font);

std::string woff2_to_ttf(const std::string &woff2_font);

}  // namespace klib
