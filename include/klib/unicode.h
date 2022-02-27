/**
 * @file unicode.h
 * @brief Contains Unicode module
 */

#pragma once

#include <cstdint>
#include <string>

#include "klib/detail/unicode_util.h"

namespace klib {

/**
 * @brief Remove all leading spaces from the input
 * @param str: Input string
 */
void trim_left(std::string &str);

/**
 * @brief Remove all leading spaces from the input
 * @param str: Input string
 * @return A trimmed copy of the input
 */
std::string trim_left_copy(const std::string &str);

/**
 * @brief Remove all trailing spaces from the input
 * @param str: Input string
 */
void trim_right(std::string &str);

/**
 * @brief Remove all trailing spaces from the input
 * @param str: Input string
 * @return A trimmed copy of the input
 */
std::string trim_right_copy(const std::string &str);

/**
 * @brief Remove all leading and trailing spaces from the input
 * @param str: Input string
 */
void trim(std::string &str);

/**
 * @brief Remove all leading and trailing spaces from the input
 * @param str: Input string
 * @return A trimmed copy of the input
 */
std::string trim_copy(const std::string &str);

/**
 * @brief Get first code point
 * @param str: Input string
 * @return The first code point
 */
char32_t first_code_point(const std::string &str);

/**
 * @brief Get last code point
 * @param str: Input string
 * @return The last code point
 */
char32_t last_code_point(const std::string &str);

/**
 * @brief Validate the UTF-8 string
 * @param str: The UTF-8 string to validate
 * @return True if and only if the string is valid UTF-8
 */
bool validate_utf8(const std::string &str);

/**
 * @brief Validate the UTF-16 string
 * @param str: The UTF-16 string to validate
 * @return True if and only if the string is valid UTF-16
 */
bool validate_utf16(const std::u16string &str);

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
 * @return The converted UTF-32 encoded string
 */
std::u32string utf8_to_utf32(const std::string &str);

/**
 * @brief Convert UTF-32 encoded string to UTF-8 encoded string
 * @param str: UTF-32 encoded string
 * @return The converted UTF-8 encoded string
 */
std::string utf32_to_utf8(const std::u32string &str);

/**
 * @brief Determine whether it is an ASCII character
 * @param code_point: Code point
 * @return If it is an ASCII character, return true, otherwise return false
 */
constexpr bool is_ascii(char32_t code_point) {
  return static_cast<std::uint32_t>(code_point) <= 0x7F;
}

/**
 * @brief Determine whether it is an whitespace character
 * @param code_point: Code point
 * @return If it is an whitespace character, return true, otherwise return false
 * @see https://en.wikipedia.org/wiki/Whitespace_character
 */
constexpr bool is_whitespace(char32_t code_point) {
  return code_point == U'\u0009' || code_point == U'\u000A' ||
         code_point == U'\u000B' || code_point == U'\u000C' ||
         code_point == U'\u000D' || code_point == U'\u0020' ||
         code_point == U'\u0085' || code_point == U'\u00A0' ||
         code_point == U'\u1680' || code_point == U'\u2000' ||
         code_point == U'\u2001' || code_point == U'\u2002' ||
         code_point == U'\u2003' || code_point == U'\u2004' ||
         code_point == U'\u2005' || code_point == U'\u2006' ||
         code_point == U'\u2007' || code_point == U'\u2008' ||
         code_point == U'\u2009' || code_point == U'\u200A' ||
         code_point == U'\u2028' || code_point == U'\u2029' ||
         code_point == U'\u202F' || code_point == U'\u205F' ||
         code_point == U'\u3000';
}

/**
 * @brief Determine whether it is an Chinese punctuation character
 * @param code_point: Code point
 * @return If it is an Chinese punctuation character, return true, otherwise
 * return false
 * @see https://zh.wikipedia.org/wiki/%E6%A0%87%E7%82%B9%E7%AC%A6%E5%8F%B7
 */
constexpr bool is_chinese_punctuation(char32_t code_point) {
  return code_point == U'。' || code_point == U'？' || code_point == U'！' ||
         code_point == U'，' || code_point == U'、' || code_point == U'；' ||
         code_point == U'：' || code_point == U'“' || code_point == U'”' ||
         code_point == U'﹃' || code_point == U'﹄' || code_point == U'‘' ||
         code_point == U'’' || code_point == U'﹁' || code_point == U'﹂' ||
         code_point == U'（' || code_point == U'）' || code_point == U'［' ||
         code_point == U'］' || code_point == U'〔' || code_point == U'〕' ||
         code_point == U'【' || code_point == U'】' ||
         // ——
         code_point == U'—' ||
         // ……
         code_point == U'…' || code_point == U'－' || code_point == U'-' ||
         code_point == U'～' || code_point == U'·' || code_point == U'《' ||
         code_point == U'》' || code_point == U'〈' || code_point == U'〉' ||
         // ﹏﹏
         code_point == U'﹏' ||
         // ＿＿
         code_point == U'＿' || code_point == U'.';
}

/**
 * @brief Determine whether it is an English punctuation character
 * @param code_point: Code point
 * @return If it is an English punctuation character, return true, otherwise
 * return false
 * @see https://zh.wikipedia.org/wiki/%E6%A0%87%E7%82%B9%E7%AC%A6%E5%8F%B7
 */
constexpr bool is_english_punctuation(char32_t code_point) {
  return code_point == U'.' || code_point == U'?' || code_point == U'!' ||
         code_point == U',' || code_point == U':' || code_point == U'…' ||
         code_point == U';' || code_point == U'-' || code_point == U'–' ||
         code_point == U'—' || code_point == U'(' || code_point == U')' ||
         code_point == U'[' || code_point == U']' || code_point == U'{' ||
         code_point == U'}' || code_point == U'"' || code_point == U'\'' ||
         code_point == U'/';
}

/**
 * @brief Determine whether it is a Chinese character
 * @param code_point: Code point
 * @return If it is a Chinese character, return true, otherwise return false
 */
constexpr bool is_chinese(char32_t code_point) {
  using namespace detail;

  return UnifiedIdeographs{}(code_point) || UnifiedIdeographsA{}(code_point) ||
         UnifiedIdeographsB{}(code_point) || UnifiedIdeographsC{}(code_point) ||
         UnifiedIdeographsD{}(code_point) || UnifiedIdeographsE{}(code_point) ||
         CompatibilityIdeographs{}(code_point) ||
         CompatibilityIdeographsSupplement{}(code_point);
}

}  // namespace klib
