#include <cstdint>

#include <catch2/catch.hpp>

#include "klib/unicode.h"

TEST_CASE("utf8_to_utf16", "[unicode]") {
  auto utf16 = klib::utf8_to_utf16("zß水🍌");

  REQUIRE(std::size(utf16) == 5);
  REQUIRE(utf16[0] == 0x007A);
  REQUIRE(utf16[1] == 0x00DF);
  REQUIRE(utf16[2] == 0x6C34);
  REQUIRE(utf16[3] == 0xD83C);
  REQUIRE(utf16[4] == 0xDF4C);
}

TEST_CASE("utf8_to_utf32", "[unicode]") {
  auto utf32 = klib::utf8_to_utf32("zß水🍌");

  REQUIRE(std::size(utf32) == 4);
  REQUIRE(utf32[0] == 0x0000007A);
  REQUIRE(utf32[1] == 0x000000DF);
  REQUIRE(utf32[2] == 0x00006C34);
  REQUIRE(utf32[3] == 0x0001F34C);
}

TEST_CASE("utf32_to_utf8", "[unicode]") {
  auto utf32 = klib::utf8_to_utf32("书客");
  auto utf8 = klib::utf32_to_utf8(utf32);

  REQUIRE(std::size(utf8) == 6);
  REQUIRE(static_cast<std::uint8_t>(utf8[0]) == 0xE4);
  REQUIRE(static_cast<std::uint8_t>(utf8[1]) == 0xB9);
  REQUIRE(static_cast<std::uint8_t>(utf8[2]) == 0xA6);
  REQUIRE(static_cast<std::uint8_t>(utf8[3]) == 0xE5);
  REQUIRE(static_cast<std::uint8_t>(utf8[4]) == 0xAE);
  REQUIRE(static_cast<std::uint8_t>(utf8[5]) == 0xA2);
}

TEST_CASE("is_ascii", "[unicode]") {
  REQUIRE(klib::is_ascii('A'));
  REQUIRE_FALSE(klib::is_ascii(static_cast<char>(190)));

  REQUIRE(klib::is_ascii("AAA"));
  REQUIRE_FALSE(klib::is_ascii("你"));

  REQUIRE(klib::is_ascii(klib::utf8_to_utf32("AAA")));
  REQUIRE_FALSE(klib::is_ascii(klib::utf8_to_utf32("你")));
}

TEST_CASE("is_chinese", "[unicode]") {
  REQUIRE(klib::is_chinese("你"));
  REQUIRE_FALSE(klib::is_chinese("a"));
  REQUIRE_FALSE(klib::is_chinese("🍌"));
}
