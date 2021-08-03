#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/util.h"

TEST_CASE("read_file") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  auto data = klib::util::read_file("zlib-v1.2.11.tar.gz", true);
  REQUIRE(std::size(data) == 644596);
}

TEST_CASE("utf8_to_utf16") {
  auto utf16 = klib::util::utf8_to_utf16("zß水🍌");

  REQUIRE(std::size(utf16) == 5);
  REQUIRE(utf16[0] == 0x007A);
  REQUIRE(utf16[1] == 0x00DF);
  REQUIRE(utf16[2] == 0x6C34);
  REQUIRE(utf16[3] == 0xD83C);
  REQUIRE(utf16[4] == 0xDF4C);
}

TEST_CASE("utf8_to_utf32") {
  auto utf32 = klib::util::utf8_to_utf32("zß水🍌");

  REQUIRE(std::size(utf32) == 4);
  REQUIRE(utf32[0] == 0x0000007A);
  REQUIRE(utf32[1] == 0x000000DF);
  REQUIRE(utf32[2] == 0x00006C34);
  REQUIRE(utf32[3] == 0x0001F34C);
}

TEST_CASE("is_ascii") {
  REQUIRE(klib::util::is_ascii('A'));
  REQUIRE_FALSE(klib::util::is_ascii(static_cast<char>(190)));
}

TEST_CASE("is_chinese") {
  REQUIRE(klib::util::is_chinese("你"));
  REQUIRE_FALSE(klib::util::is_chinese("a"));
  REQUIRE_FALSE(klib::util::is_chinese("🍌"));
}

TEST_CASE("sha3_512") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::util::sha3_512("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");
}

TEST_CASE("folder_size") {
  REQUIRE(std::filesystem::exists("folder1"));
  REQUIRE(klib::util::folder_size("folder1") == 38);
}

TEST_CASE("same_folder") {
  REQUIRE(std::filesystem::exists("folder1"));
  REQUIRE(std::filesystem::exists("folder2"));

  REQUIRE(klib::util::same_folder("folder1", "folder2"));
}

TEST_CASE("execute_command") {
  std::string command = "gcc -v";
  REQUIRE_NOTHROW(klib::util::execute_command(command));
}
