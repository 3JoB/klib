#include "klib/unicode.h"

#include <algorithm>
#include <cerrno>
#include <clocale>
#include <cstddef>
#include <cuchar>
#include <limits>

#include "klib/exception.h"

namespace klib {

namespace {

void set_locale() {
  const char *locale = "en_US.utf8";

  if (std::setlocale(LC_ALL, locale) == nullptr) {
    throw RuntimeError("Does not support '{}'", locale);
  }
}

}  // namespace

// https://zh.cppreference.com/w/c/string/multibyte/mbrtoc16
std::u16string utf8_to_utf16(const std::string &str) {
  if (std::empty(str)) {
    return {};
  }

  set_locale();

  std::u16string result;

  char16_t out = 0;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = std::mbrtoc16(&out, begin, size, &state)) {
    if (rc == static_cast<std::size_t>(-1)) {
      throw RuntimeError(std::strerror(errno));
    }

    if (rc == static_cast<std::size_t>(-3)) {
      result.push_back(out);
    } else if (rc <= std::numeric_limits<std::size_t>::max() / 2) {
      begin += rc;
      result.push_back(out);
    } else {
      break;
    }
  }

  return result;
}

// https://zh.cppreference.com/w/c/string/multibyte/mbrtoc32
std::u32string utf8_to_utf32(const std::string &str) {
  if (std::empty(str)) {
    return {};
  }

  set_locale();

  std::u32string result;

  char32_t out = 0;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = std::mbrtoc32(&out, begin, size, &state)) {
    if (rc == static_cast<std::size_t>(-1)) {
      throw RuntimeError(std::strerror(errno));
    }

    if (rc > static_cast<std::size_t>(-1) / 2) {
      break;
    }

    begin += rc;
    result.push_back(out);
  }

  return result;
}

// https://zh.cppreference.com/w/c/string/multibyte/c32rtomb
std::string utf32_to_utf8(char32_t c) {
  set_locale();

  std::string result;
  result.resize(MB_CUR_MAX);

  mbstate_t state = {};
  auto rc = std::c32rtomb(std::data(result), c, &state);

  if (rc == static_cast<std::size_t>(-1)) {
    throw RuntimeError(std::strerror(errno));
  } else if (rc == 0) {
    throw RuntimeError("utf32_to_utf8 error");
  }

  result.resize(rc);
  return result;
}

std::string utf32_to_utf8(const std::u32string &str) {
  std::string result;

  for (auto c : str) {
    result.append(utf32_to_utf8(c));
  }

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

bool is_chinese(const std::string &c) {
  if (std::empty(c)) {
    return false;
  }

  auto utf32 = utf8_to_utf32(c);

  if (std::size(utf32) != 1) {
    throw RuntimeError("not a UTF-32 encoded character: '{}'", c);
  }

  return is_chinese(utf32.front());
}

}  // namespace klib
