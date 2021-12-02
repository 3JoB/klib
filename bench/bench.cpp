#include <cuchar>
#include <string>

#include <catch2/catch.hpp>

#include "klib/exception.h"
#include "klib/hash.h"
#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("hash") {
  std::string str = klib::generate_random_bytes(16);

  BENCHMARK("xxhash") { return klib::fast_hash(str); };

  BENCHMARK("std::hash") { return std::hash<std::string>{}(str); };

  BENCHMARK("md5") { return klib::md5(str); };
}

TEST_CASE("hash large") {
  std::string str = klib::generate_random_bytes(102400);

  BENCHMARK("xxhash") { return klib::fast_hash(str); };

  BENCHMARK("std::hash") { return std::hash<std::string>{}(str); };

  BENCHMARK("md5") { return klib::md5(str); };
}

// https://zh.cppreference.com/w/c/string/multibyte/mbrtoc16
std::u16string utf8_to_utf16(const std::string &str) {
  if (std::empty(str)) {
    return {};
  }

  if (std::setlocale(LC_ALL, "en_US.utf8") == nullptr) {
    throw klib::RuntimeError("Does not support en_US.utf8");
  }

  std::u16string result;

  char16_t out = 0;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = std::mbrtoc16(&out, begin, size, &state)) {
    if (rc == static_cast<std::size_t>(-1)) {
      throw klib::RuntimeError(std::strerror(errno));
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

TEST_CASE("utf") {
  std::string str = klib::read_file("263060.txt", false);

  BENCHMARK("simdutf") { return klib::utf8_to_utf16(str); };

  BENCHMARK("std::mbrtoc16") { return utf8_to_utf16(str); };
}
