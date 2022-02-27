#include "klib/unicode.h"

#include <simdutf.h>
#include <utf8cpp/utf8.h>
#include <boost/algorithm/string.hpp>
#include <gsl/assert>

#include "klib/exception.h"
#include "utf_utils/utf_utils.h"

namespace klib {

namespace {

std::int32_t trim_left_erase_num(const std::string &str) {
  const auto begin = str.c_str();
  const auto end = begin + std::size(str);
  auto iter = begin;

  while (iter != end) {
    auto temp = iter;
    char32_t code_point = utf8::next(temp, end);

    if (is_whitespace(code_point)) {
      iter = temp;
      continue;
    } else {
      break;
    }
  }

  return iter - begin;
}

std::int32_t trim_right_erase_num(const std::string &str) {
  const auto begin = str.c_str();
  const auto end = begin + std::size(str);
  auto iter = end;

  while (iter != begin) {
    auto temp = iter;
    char32_t code_point = utf8::prior(temp, begin);

    if (is_whitespace(code_point)) {
      iter = temp;
      continue;
    } else {
      break;
    }
  }

  return end - iter;
}

}  // namespace

void trim_left(std::string &str) {
  boost::algorithm::erase_head(str, trim_left_erase_num(str));
}

void trim_right(std::string &str) {
  boost::algorithm::erase_tail(str, trim_right_erase_num(str));
}

void trim(std::string &str) {
  trim_left(str);
  trim_right(str);
}

std::string trim_left_copy(const std::string &str) {
  return boost::algorithm::erase_head_copy(str, trim_left_erase_num(str));
}

std::string trim_right_copy(const std::string &str) {
  return boost::algorithm::erase_tail_copy(str, trim_right_erase_num(str));
}

std::string trim_copy(const std::string &str) {
  return trim_right_copy(trim_left_copy(str));
}

char32_t first_code_point(const std::string &str) {
  Expects(!std::empty(str));

  const auto begin = str.c_str();
  const auto end = begin + std::size(str);
  return utf8::peek_next(begin, end);
}

char32_t last_code_point(const std::string &str) {
  Expects(!std::empty(str));

  const auto begin = str.c_str();
  auto end = begin + std::size(str);
  return utf8::prior(end, begin);
}

bool validate_utf8(const std::string &str) {
  return simdutf::validate_utf8(str.c_str(), std::size(str));
}

bool validate_utf16(const std::u16string &str) {
  return simdutf::validate_utf16(str.c_str(), std::size(str));
}

// https://github.com/simdutf/simdutf#example
std::u16string utf8_to_utf16(const std::string &str) {
  auto source = std::data(str);
  auto source_size = std::size(str);

  std::u16string result;
  result.resize(simdutf::utf16_length_from_utf8(source, source_size));

  auto check =
      simdutf::convert_utf8_to_utf16(source, source_size, std::data(result));
  if (check == 0) [[unlikely]] {
    throw RuntimeError("convert_utf8_to_utf16 failed");
  }

  return result;
}

std::string utf16_to_utf8(const std::u16string &str) {
  auto source = std::data(str);
  auto source_size = std::size(str);

  std::string result;
  result.resize(simdutf::utf8_length_from_utf16(source, source_size));

  auto check =
      simdutf::convert_utf16_to_utf8(source, source_size, std::data(result));
  if (check == 0) [[unlikely]] {
    throw RuntimeError("convert_utf16_to_utf8 failed");
  }

  return result;
}

std::u32string utf8_to_utf32(const std::string &str) {
  auto input_size = std::size(str);

  std::u32string result;
  result.resize(input_size);
  auto ptr = reinterpret_cast<const char8_t *>(std::data(str));

  auto length = uu::UtfUtils::SseBigTableConvert(ptr, ptr + input_size,
                                                 std::data(result));
  if (length == -1) [[unlikely]] {
    throw RuntimeError("SseBigTableConvert failed");
  }

  result.resize(length);
  return result;
}

std::wstring utf8_to_utf32_w(const std::string &str) {
  static_assert(sizeof(wchar_t) == 4, "wchar_t is not 4 byte");

  auto input_size = std::size(str);

  std::wstring result;
  result.resize(input_size);
  auto ptr = reinterpret_cast<const char8_t *>(std::data(str));

  auto length = uu::UtfUtils::SseBigTableConvert(
      ptr, ptr + input_size, reinterpret_cast<char32_t *>(std::data(result)));
  if (length == -1) [[unlikely]] {
    throw RuntimeError("SseBigTableConvert failed");
  }

  result.resize(length);
  return result;
}

std::string utf32_to_utf8(const std::u32string &str) {
  return utf8::utf32to8(str);
}

std::string utf32_to_utf8_w(const std::wstring &str) {
  static_assert(sizeof(wchar_t) == 4, "wchar_t is not 4 byte");

  std::string result;
  result.reserve(std::size(str) * 4);

  const auto ptr = std::data(str);
  utf8::utf32to8(ptr, ptr + std::size(str), std::back_inserter(result));

  return result;
}

}  // namespace klib
