#include "klib/unicode.h"

#include <simdutf.h>
#include <utf8cpp/utf8.h>
#include <boost/algorithm/string.hpp>
#include <gsl/assert>

#include "klib/exception.h"

namespace klib {

namespace {

std::int32_t trim_left_erase_num(const std::string &str) {
  const auto begin = str.c_str();
  const auto end = begin + std::size(str);
  auto iter = begin;

  try {
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
  } catch (const utf8::exception &err) {
    throw klib::RuntimeError(err.what());
  }

  return iter - begin;
}

std::int32_t trim_right_erase_num(const std::string &str) {
  const auto begin = str.c_str();
  const auto end = begin + std::size(str);
  auto iter = end;

  try {
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
  } catch (const utf8::exception &err) {
    throw klib::RuntimeError(err.what());
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

  try {
    return utf8::peek_next(begin, end);
  } catch (const utf8::exception &err) {
    throw klib::RuntimeError(err.what());
  }
}

char32_t last_code_point(const std::string &str) {
  Expects(!std::empty(str));

  const auto begin = str.c_str();
  auto end = begin + std::size(str);

  try {
    return utf8::prior(end, begin);
  } catch (const utf8::exception &err) {
    throw klib::RuntimeError(err.what());
  }
}

bool validate_utf8(const std::string &str) {
  return simdutf::validate_utf8(str.c_str(), std::size(str));
}

bool validate_utf16(const std::u16string &str) {
  return simdutf::validate_utf16(str.c_str(), std::size(str));
}

bool validate_utf32(const std::u32string &str) {
  return simdutf::validate_utf32(str.c_str(), std::size(str));
}

// https://github.com/simdutf/simdutf#example
std::u16string utf8_to_utf16(const std::string &str) {
  const auto source = std::data(str);
  const auto source_size = std::size(str);
  const auto result_size = simdutf::utf16_length_from_utf8(source, source_size);

  std::u16string result;
  result.resize(result_size);

  const auto check = simdutf::convert_valid_utf8_to_utf16(source, source_size,
                                                          std::data(result));
  if (check != result_size) [[unlikely]] {
    throw RuntimeError("convert_utf8_to_utf16() failed");
  }

  return result;
}

std::string utf16_to_utf8(const std::u16string &str) {
  const auto source = std::data(str);
  const auto source_size = std::size(str);
  const auto result_size = simdutf::utf8_length_from_utf16(source, source_size);

  std::string result;
  result.resize(result_size);

  const auto check = simdutf::convert_valid_utf16_to_utf8(source, source_size,
                                                          std::data(result));
  if (check != result_size) [[unlikely]] {
    throw RuntimeError("convert_utf16_to_utf8() failed");
  }

  return result;
}

std::u32string utf8_to_utf32(const std::string &str) {
  const auto source = std::data(str);
  const auto source_size = std::size(str);
  const auto result_size = simdutf::utf32_length_from_utf8(source, source_size);

  std::u32string result;
  result.resize(result_size);

  const auto check = simdutf::convert_valid_utf8_to_utf32(source, source_size,
                                                          std::data(result));
  if (check != result_size) [[unlikely]] {
    throw RuntimeError("convert_utf8_to_utf32() failed");
  }

  return result;
}

std::string utf32_to_utf8(const std::u32string &str) {
  const auto source = std::data(str);
  const auto source_size = std::size(str);
  const auto result_size = simdutf::utf8_length_from_utf32(source, source_size);

  std::string result;
  result.resize(result_size);

  const auto check = simdutf::convert_valid_utf32_to_utf8(source, source_size,
                                                          std::data(result));
  if (check != result_size) [[unlikely]] {
    throw RuntimeError("convert_utf32_to_utf8() failed");
  }

  return result;
}

std::string utf32_to_utf8(char32_t code_point) {
  std::u32string str;
  str.push_back(code_point);
  return utf32_to_utf8(str);
}

}  // namespace klib
