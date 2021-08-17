#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/util.h"

TEST_CASE("ChangeWorkingDir", "[util]") {
  REQUIRE(!std::filesystem::exists("work-dir"));

  {
    klib::ChangeWorkingDir dir("work-dir");
    REQUIRE(std::filesystem::exists("../work-dir"));
  }

  REQUIRE(std::filesystem::remove("work-dir"));
}

TEST_CASE("read_file & write_file", "[util]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  auto data = klib::read_file("zlib-v1.2.11.tar.gz", true);
  REQUIRE(std::size(data) == 644596);

  REQUIRE_NOTHROW(klib::write_file("write-file.zip", true, data));

  REQUIRE(std::filesystem::exists("write-file.zip"));
  REQUIRE(std::filesystem::file_size("write-file.zip") == 644596);

  std::filesystem::remove("write-file.zip");
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

TEST_CASE("is_ascii", "[util]") {
  REQUIRE(klib::is_ascii('A'));
  REQUIRE_FALSE(klib::is_ascii(static_cast<char>(190)));

  REQUIRE(klib::is_ascii("AAA"));
  REQUIRE_FALSE(klib::is_ascii("你"));
}

TEST_CASE("is_chinese", "[util]") {
  REQUIRE(klib::is_chinese("你"));
  REQUIRE_FALSE(klib::is_chinese("a"));
  REQUIRE_FALSE(klib::is_chinese("🍌"));
}

TEST_CASE("base64_encode", "[util]") {
  REQUIRE(klib::base64_encode("hello") == "aGVsbG8=");
}

TEST_CASE("base64_decode", "[util]") {
  REQUIRE(klib::base64_decode("aGVsbG8=") == "hello");
}

TEST_CASE("sha3_512", "[util]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::sha3_512("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");
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
