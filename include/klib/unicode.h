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
 * @brief Convert code point to UTF-8 encoded string
 * @param code_point: Code point
 * @return The converted UTF-8 encoded string
 */
std::string utf32_to_utf8(char32_t code_point);

/**
 * @brief Determine whether it is an ASCII character
 * @param code_point: Code point
 * @return If it is an ASCII character, return true, otherwise return false
 */
constexpr bool is_ascii(char32_t code_point) noexcept {
  return static_cast<std::uint32_t>(code_point) <= 0x7F;
}

/**
 * @brief Determine whether it is an whitespace character
 * @param code_point: Code point
 * @return If it is an whitespace character, return true, otherwise return false
 * @see https://en.wikipedia.org/wiki/Whitespace_character
 */
constexpr bool is_whitespace(char32_t code_point) noexcept {
  using namespace detail;

  return code_point == U'\u0009' || range(code_point, U'\u000A', U'\u000D') ||
         code_point == U'\u0020' || code_point == U'\u0085' ||
         code_point == U'\u00A0' || code_point == U'\u1680' ||
         range(code_point, U'\u2000', U'\u200A') ||
         range(code_point, U'\u2028', U'\u2029') || code_point == U'\u202F' ||
         code_point == U'\u205F' || code_point == U'\u3000';
}

/**
 * @brief Determine whether it is an control character
 * @param code_point: Code point
 * @return If it is an control character, return true, otherwise return false
 * @see https://zh.wikipedia.org/wiki/%E6%8E%A7%E5%88%B6%E5%AD%97%E7%AC%A6
 */
constexpr bool is_control(char32_t code_point) noexcept {
  using namespace detail;

  return range(code_point, U'\u0000', U'\u001F') ||
         range(code_point, U'\u007F', U'\u009F');
}

/**
 * @brief Determine whether it is an Chinese punctuation character
 * @param code_point: Code point
 * @return If it is an Chinese punctuation character, return true, otherwise
 * return false
 * @see https://zh.wikipedia.org/wiki/%E6%A0%87%E7%82%B9%E7%AC%A6%E5%8F%B7
 */
constexpr bool is_chinese_punctuation(char32_t code_point) noexcept {
  return code_point == U'。' || code_point == U'？' || code_point == U'！' ||
         code_point == U'，' || code_point == U'、' || code_point == U'；' ||
         code_point == U'：' || code_point == U'“' || code_point == U'”' ||
         code_point == U'『' || code_point == U'』' || code_point == U'‘' ||
         code_point == U'’' || code_point == U'「' || code_point == U'」' ||
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
constexpr bool is_english_punctuation(char32_t code_point) noexcept {
  return code_point == U'.' || code_point == U'?' || code_point == U'!' ||
         code_point == U',' || code_point == U':' || code_point == U'…' ||
         code_point == U';' || code_point == U'-' || code_point == U'–' ||
         code_point == U'—' || code_point == U'(' || code_point == U')' ||
         code_point == U'[' || code_point == U']' || code_point == U'{' ||
         code_point == U'}' || code_point == U'"' || code_point == U'\'' ||
         code_point == U'/';
}

/**
 * @brief Determine whether it is a CJK Unified Ideographs
 * @param code_point: Code point
 * @return If it is a CJK Unified Ideographs, return true, otherwise return
 * false
 * @see
 * https://zh.wikipedia.org/wiki/%E4%B8%AD%E6%97%A5%E9%9F%93%E7%B5%B1%E4%B8%80%E8%A1%A8%E6%84%8F%E6%96%87%E5%AD%97
 */
constexpr bool is_cjk(char32_t code_point) noexcept {
  using namespace detail;

  return code_point == U'\u3007' || range(code_point, U'\u3400', U'\u4DBF') ||
         range(code_point, U'\u4E00', U'\u9FFF') ||
         range(code_point, U'\uFA0E', U'\uFA0F') || code_point == U'\uFA11' ||
         range(code_point, U'\uFA13', U'\uFA14') || code_point == U'\uFA1F' ||
         code_point == U'\uFA21' || range(code_point, U'\uFA23', U'\uFA24') ||
         range(code_point, U'\uFA27', U'\uFA29') ||
         range(code_point, U'\U00020000', U'\U0002A6DF') ||
         range(code_point, U'\U0002A700', U'\U0002B738') ||
         range(code_point, U'\U0002B740', U'\U0002B81D') ||
         range(code_point, U'\U0002B820', U'\U0002CEA1') ||
         range(code_point, U'\U0002CEB0', U'\U0002EBE0') ||
         range(code_point, U'\U00030000', U'\U0003134A');
}

}  // namespace klib
