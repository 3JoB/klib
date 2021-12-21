/**
 * @file unicode.h
 * @brief Contains an Unicode module
 */

#pragma once

#include <cstdint>
#include <string>

#include "klib/detail/unicode_util.h"

namespace klib {

/**
 * @brief Convert UTF-8 encoded string to UTF-16 encoded string
 * @param str: UTF-8 encoded string
 * @return The converted UTF-16 encoded string
 */
std::u16string utf8_to_utf16(const std::string &str);

/**
 * @brief Convert UTF-16 encoded string to UTF-8 encoded string
 * @param str: UTF-16 encoded string
 * @return The converted UTF-8 encoded string
 */
std::string utf16_to_utf8(const std::u16string &str);

/**
 * @brief Convert UTF-8 encoded string to UTF-32 encoded string
 * @param str: UTF-8 encoded string
 * @return The converted UTF-32 encoded char
 */
char32_t utf8_to_unicode(const std::string &str);

/**
 * @brief Convert UTF-8 encoded string to UTF-32 encoded string
 * @param str: UTF-8 encoded string
 * @return The converted UTF-32 encoded string
 */
std::u32string utf8_to_utf32(const std::string &str);

/**
 * @brief Convert UTF-32 encoded string to UTF-8 encoded string
 * @param c: UTF-32 encoded char
 * @return The converted UTF-8 encoded string
 */
std::string utf32_to_utf8(char32_t c);

/**
 * @brief Convert UTF-32 encoded string to UTF-8 encoded string
 * @param str: UTF-32 encoded string
 * @return The converted UTF-8 encoded string
 */
std::string utf32_to_utf8(const std::u32string &str);

/**
 * @brief Determine whether it is an ASCII character
 * @param c: A character
 * @return If it is an ASCII character, return true, otherwise return false
 */
constexpr bool is_ascii(char c) { return static_cast<std::uint8_t>(c) <= 0x7F; }

constexpr bool is_ascii(char32_t c) {
  return static_cast<std::uint32_t>(c) <= 0x7F;
}

/**
 * @brief Determine whether a string consisting of ASCII characters
 * @param str: A string
 * @return If it is a string consisting of ASCII characters , return true,
 * otherwise return false
 */
bool is_ascii(const std::string &str);

bool is_ascii(const std::u32string &str);

/**
 * @brief Determine whether it is a Chinese character
 * @param c: A UTF-32 encoded character
 * @return If it is a Chinese character, return true, otherwise return false
 */
constexpr bool is_chinese(char32_t c) {
  using namespace detail;

  return UnifiedIdeographs{}(c) || UnifiedIdeographsA{}(c) ||
         UnifiedIdeographsB{}(c) || UnifiedIdeographsC{}(c) ||
         UnifiedIdeographsD{}(c) || UnifiedIdeographsE{}(c) ||
         CompatibilityIdeographs{}(c) || CompatibilityIdeographsSupplement{}(c);
}

/**
 * @brief Determine whether it is a Chinese character
 * @param c: A UTF-8 encoded string representing a UTF-32 encoded character
 * @return If it is a Chinese character, return true, otherwise return false
 */
bool is_chinese(const std::string &c);

}  // namespace klib
