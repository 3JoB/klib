#include "klib/unicode.h"

#include <algorithm>

#include <simdutf.h>

#include "klib/exception.h"
#include "utf_utils/utf_utils.h"

namespace klib {

// https://github.com/simdutf/simdutf#example
std::u16string utf8_to_utf16(const std::string &str) {
  auto source = std::data(str);
  auto source_size = std::size(str);

  std::u16string result;
  result.resize(simdutf::utf16_length_from_utf8(source, source_size));

  auto check =
      simdutf::convert_utf8_to_utf16(source, source_size, std::data(result));
  if (check == 0) {
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
  if (check == 0) {
    throw RuntimeError("convert_utf16_to_utf8 failed");
  }

  return result;
}

char32_t utf8_to_unicode(const std::string &str) {
  char32_t result;
  auto input_size = std::size(str);
  auto ptr = reinterpret_cast<const char8_t *>(std::data(str));

  auto check = uu::UtfUtils::GetCodePoint(ptr, ptr + input_size, result);
  if (!check) {
    throw RuntimeError("GetCodePoint failed");
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
  if (length == -1) {
    throw RuntimeError("SseBigTableConvert failed");
  }

  result.resize(length);
  return result;
}

bool is_ascii(const std::string &str) {
  return std::all_of(std::begin(str), std::end(str),
                     [](char c) { return is_ascii(c); });
}

bool is_ascii(const std::u32string &str) {
  return std::all_of(std::begin(str), std::end(str),
                     [](char32_t c) { return is_ascii(c); });
}

bool is_chinese(const std::string &c) { return is_chinese(utf8_to_unicode(c)); }

}  // namespace klib
