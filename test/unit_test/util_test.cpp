#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch.hpp>

#include "klib/util.h"

TEST_CASE("ChangeWorkingDir", "[util]") {
  REQUIRE_FALSE(std::filesystem::exists("work-dir"));

  {
    klib::ChangeWorkingDir dir("work-dir");
    REQUIRE(std::filesystem::exists("../work-dir"));
  }

  REQUIRE(std::filesystem::remove("work-dir"));
}

TEST_CASE("find_last", "[util]") {
  std::vector<std::int32_t> v{1, 2, 2, 3, 4, 5, 6, 2, 5, 4};
  auto iter = klib::find_last(std::begin(v), std::end(v), 2);
  REQUIRE(iter - std::begin(v) == 7);

  v = {2, 1, 1, 1, 1, 1};
  iter = klib::find_last(std::begin(v), std::end(v), 2);
  REQUIRE(iter - std::begin(v) == 0);
}

TEST_CASE("split_str", "[util]") {
  const std::vector<std::string> std_vec = {"123", "123", "123"};

  auto result = klib::split_str("123abc123abc123abc", "abc");
  REQUIRE(result == std_vec);

  result = klib::split_str("|||123?123|123!", "|?!");
  REQUIRE(result == std_vec);
}

TEST_CASE("read_file & write_file", "[util]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  auto data = klib::read_file("zlib-v1.2.11.tar.gz", true);
  REQUIRE(std::size(data) == 644596);

  REQUIRE_NOTHROW(klib::write_file("write-file.zip", true, data));

  REQUIRE(std::filesystem::exists("write-file.zip"));
  REQUIRE(std::filesystem::file_size("write-file.zip") == 644596);

  REQUIRE(std::filesystem::remove("write-file.zip"));
}

TEST_CASE("read_file_line", "[util]") {
  std::string_view content = R"(aaa
bbb

ccc

 dd)";
  REQUIRE_NOTHROW(klib::write_file("write-file.txt", true, content));
  REQUIRE(klib::read_file_line("write-file.txt") ==
          std::vector<std::string>{"aaa", "bbb", "ccc", "dd"});

  REQUIRE(std::filesystem::remove("write-file.txt"));
}

TEST_CASE("utf8_to_utf16", "[util]") {
  auto utf16 = klib::utf8_to_utf16("zß水🍌");

  REQUIRE(std::size(utf16) == 5);
  REQUIRE(utf16[0] == 0x007A);
  REQUIRE(utf16[1] == 0x00DF);
  REQUIRE(utf16[2] == 0x6C34);
  REQUIRE(utf16[3] == 0xD83C);
  REQUIRE(utf16[4] == 0xDF4C);
}

TEST_CASE("utf8_to_utf32", "[util]") {
  auto utf32 = klib::utf8_to_utf32("zß水🍌");

  REQUIRE(std::size(utf32) == 4);
  REQUIRE(utf32[0] == 0x0000007A);
  REQUIRE(utf32[1] == 0x000000DF);
  REQUIRE(utf32[2] == 0x00006C34);
  REQUIRE(utf32[3] == 0x0001F34C);
}

TEST_CASE("utf32_to_utf8", "[util]") {
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

TEST_CASE("is_ascii", "[util]") {
  REQUIRE(klib::is_ascii('A'));
  REQUIRE_FALSE(klib::is_ascii(static_cast<char>(190)));

  REQUIRE(klib::is_ascii("AAA"));
  REQUIRE_FALSE(klib::is_ascii("你"));

  REQUIRE(klib::is_ascii(klib::utf8_to_utf32("AAA")));
  REQUIRE_FALSE(klib::is_ascii(klib::utf8_to_utf32("你")));
}

TEST_CASE("is_chinese", "[util]") {
  REQUIRE(klib::is_chinese("你"));
  REQUIRE_FALSE(klib::is_chinese("a"));
  REQUIRE_FALSE(klib::is_chinese("🍌"));
}

TEST_CASE("folder_size", "[util]") {
  REQUIRE(std::filesystem::exists("folder1"));
  REQUIRE(klib::folder_size("folder1") == 38);
}

TEST_CASE("same_folder", "[util]") {
  REQUIRE(std::filesystem::exists("folder1"));
  REQUIRE(std::filesystem::exists("folder2"));

  REQUIRE(klib::same_folder("folder1", "folder2"));
}

TEST_CASE("execute_command", "[util]") {
  std::string command = "gcc -v";
  REQUIRE_NOTHROW(klib::execute_command(command));
}
