/**
 * @file font.h
 * @brief Contains handling font module
 */

#pragma once

#include <string>
#include <string_view>

namespace klib {

/**
 * @brief Take a font subset using the given Unicode
 * @param ttf_font_path: TTF font path
 * @param unicodes: Unicode that needs to be preserved
 * @return TTF font subset
 */
std::string ttf_subset(const std::string &ttf_font_path,
                       const std::u32string &unicodes);

std::string ttf_subset(std::string_view ttf_font_path,
                       const std::u32string &unicodes);

std::string ttf_subset(const char *ttf_font_path,
                       const std::u32string &unicodes);

/**
 * @brief Convert TTF font to WOFF2 font
 * @param ttf_font: TTF font
 * @return WOFF2 font
 */
std::string ttf_to_woff2(const std::string &ttf_font);

/**
 * @brief Convert WOFF2 font to TTF font
 * @param woff2_font: WOFF2 font
 * @return TTF font
 */
std::string woff2_to_ttf(const std::string &woff2_font);

}  // namespace klib
