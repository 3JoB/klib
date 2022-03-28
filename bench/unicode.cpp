#include <filesystem>
#include <string>

#include <unicode/unistr.h>
#include <catch2/catch.hpp>

#include "klib/unicode.h"
#include "klib/util.h"

namespace {

std::string trim(const std::string &str) {
  auto icu_str = icu::UnicodeString::fromUTF8(str.c_str());
  icu_str.trim();

  std::string temp;
  return icu_str.toUTF8String(temp);
}

}  // namespace

TEST_CASE("UTF convert short", "[unicode]") {
  const auto str =
      "咲夜的能力不是时停，而是无质量的高速移动，目前茨歌仙是这样说的，虽然茨歌"
      "仙并非ZUN画的，但是基本设定都是来自于ZUN，所以出现在里面的设定基本上都可"
      "以当一设看待，此外，漫画最新一话既然都已经发出，说明ZUN并没有试图修改这"
      "个设定问题";

  const auto utf16 = klib::utf8_to_utf16(str);
  const auto utf32 = klib::utf8_to_utf32(str);

  BENCHMARK("UTF-8 to UTF-16") { return klib::utf8_to_utf16(str); };
  BENCHMARK("UTF-16 to UTF-8") { return klib::utf16_to_utf8(utf16); };

  BENCHMARK("UTF-8 to UTF-32") { return klib::utf8_to_utf32(str); };
  BENCHMARK("UTF-32 to UTF-8") { return klib::utf32_to_utf8(utf32); };
}

TEST_CASE("UTF convert long", "[unicode]") {
  const std::string file_name = "100012892.txt";
  REQUIRE(std::filesystem::exists(file_name));

  const auto str = klib::read_file(file_name, false);

  const auto utf16 = klib::utf8_to_utf16(str);
  const auto utf32 = klib::utf8_to_utf32(str);

  BENCHMARK("UTF-8 to UTF-16") { return klib::utf8_to_utf16(str); };
  BENCHMARK("UTF-16 to UTF-8") { return klib::utf16_to_utf8(utf16); };

  BENCHMARK("UTF-8 to UTF-32") { return klib::utf8_to_utf32(str); };
  BENCHMARK("UTF-32 to UTF-8") { return klib::utf32_to_utf8(utf32); };
}

TEST_CASE("trim", "[unicode]") {
  std::string str =
      "\t  "
      "咲夜的能力不是时停，而是无质量的高速移动，目前茨歌仙是这样说的，虽然茨歌"
      "仙并非ZUN画的，但是基本设定都是来自于ZUN，所以出现在里面的设定基本上都可"
      "以当一设看待，此外，漫画最新一话既然都已经发出，说明ZUN并没有试图修改这"
      "个设定问题   ";

  std::string s1, s2;

  BENCHMARK("klib trim") { s1 = klib::trim_copy(str); };
  BENCHMARK("ICU trim") { s2 = trim(str); };

  REQUIRE(s1 == s2);
}
