#include <string>

#include <catch2/catch_test_macros.hpp>

#include "klib/unicode.h"

TEST_CASE("trim", "[unicode]") {
  std::string str = " 你好世界 ";
  CHECK(klib::trim_copy(str) == "你好世界");
}

TEST_CASE("code_point", "[unicode]") {
  const std::string str = "zß水🍌";

  CHECK(klib::first_code_point(str) == 0x0000007A);
  CHECK(klib::last_code_point(str) == 0x0001F34C);
}

TEST_CASE("validate_utf8", "[unicode]") {
  std::string str = "你好世界";
  CHECK(klib::validate_utf8(str));

  str[1] = 'a';
  CHECK_FALSE(klib::validate_utf8(str));
}

TEST_CASE("validate_utf16", "[unicode]") {
  std::u16string str = u"你好世界";
  CHECK(klib::validate_utf16(str));
}

TEST_CASE("validate_utf32", "[unicode]") {
  std::u32string str = U"你好世界";
  CHECK(klib::validate_utf32(str));
}

TEST_CASE("utf8_to_utf16", "[unicode]") {
  auto utf16 = klib::utf8_to_utf16("zß水🍌");

  CHECK(std::size(utf16) == 5);
  CHECK(utf16[0] == 0x007A);
  CHECK(utf16[1] == 0x00DF);
  CHECK(utf16[2] == 0x6C34);
  CHECK(utf16[3] == 0xD83C);
  CHECK(utf16[4] == 0xDF4C);
}

TEST_CASE("utf16_to_utf8", "[unicode]") {
  auto utf8 = klib::utf16_to_utf8(u"zß水🍌");

  CHECK(std::size(utf8) == 10);
  CHECK(static_cast<std::uint8_t>(utf8[0]) == 0x7A);
  CHECK(static_cast<std::uint8_t>(utf8[1]) == 0xC3);
  CHECK(static_cast<std::uint8_t>(utf8[2]) == 0x9F);
  CHECK(static_cast<std::uint8_t>(utf8[3]) == 0xE6);
  CHECK(static_cast<std::uint8_t>(utf8[4]) == 0xB0);
  CHECK(static_cast<std::uint8_t>(utf8[5]) == 0xB4);
  CHECK(static_cast<std::uint8_t>(utf8[6]) == 0xF0);
  CHECK(static_cast<std::uint8_t>(utf8[7]) == 0x9F);
  CHECK(static_cast<std::uint8_t>(utf8[8]) == 0x8D);
  CHECK(static_cast<std::uint8_t>(utf8[9]) == 0x8C);
}

TEST_CASE("utf8_to_utf32", "[unicode]") {
  auto utf32 = klib::utf8_to_utf32("zß水🍌");

  CHECK(std::size(utf32) == 4);
  CHECK(utf32[0] == 0x0000007A);
  CHECK(utf32[1] == 0x000000DF);
  CHECK(utf32[2] == 0x00006C34);
  CHECK(utf32[3] == 0x0001F34C);
}

TEST_CASE("utf32_to_utf8", "[unicode]") {
  auto utf8 = klib::utf32_to_utf8(U"zß水🍌");

  CHECK(std::size(utf8) == 10);
  CHECK(static_cast<std::uint8_t>(utf8[0]) == 0x7A);
  CHECK(static_cast<std::uint8_t>(utf8[1]) == 0xC3);
  CHECK(static_cast<std::uint8_t>(utf8[2]) == 0x9F);
  CHECK(static_cast<std::uint8_t>(utf8[3]) == 0xE6);
  CHECK(static_cast<std::uint8_t>(utf8[4]) == 0xB0);
  CHECK(static_cast<std::uint8_t>(utf8[5]) == 0xB4);
  CHECK(static_cast<std::uint8_t>(utf8[6]) == 0xF0);
  CHECK(static_cast<std::uint8_t>(utf8[7]) == 0x9F);
  CHECK(static_cast<std::uint8_t>(utf8[8]) == 0x8D);
  CHECK(static_cast<std::uint8_t>(utf8[9]) == 0x8C);
}

TEST_CASE("utf32_to_utf8 2", "[unicode]") {
  auto utf8 = klib::utf32_to_utf8(U"🍌");

  CHECK(std::size(utf8) == 4);
  CHECK(static_cast<std::uint8_t>(utf8[0]) == 0xF0);
  CHECK(static_cast<std::uint8_t>(utf8[1]) == 0x9F);
  CHECK(static_cast<std::uint8_t>(utf8[2]) == 0x8D);
  CHECK(static_cast<std::uint8_t>(utf8[3]) == 0x8C);
}

TEST_CASE("is_ascii", "[unicode]") {
  CHECK(klib::is_ascii('A'));
  CHECK_FALSE(klib::is_ascii(190));
}

TEST_CASE("is_cjk", "[unicode]") {
  CHECK(klib::is_cjk(klib::first_code_point("你")));
  CHECK_FALSE(klib::is_cjk(klib::first_code_point("a")));
  CHECK_FALSE(klib::is_cjk(klib::first_code_point("🍌")));
  CHECK(klib::is_cjk(U'〇'));
  CHECK(klib::is_cjk(U'䀹'));
  CHECK(klib::is_cjk(U'鿃'));
  CHECK(klib::is_cjk(U'\u9FEB'));
  CHECK(klib::is_cjk(U'﨧'));
}
