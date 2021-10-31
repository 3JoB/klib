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
  REQUIRE_NOTHROW(klib::execute_command("gcc -v"));
  REQUIRE_NOTHROW(klib::execute_command(""));
}

TEST_CASE("cleanse", "[util]") {
  std::string password = "123456";
  klib::cleanse(password);

  REQUIRE(std::empty(password));
  REQUIRE(*(std::data(password) + 5) == 0);
}

TEST_CASE("make_file_or_dir_name_legal", "[util]") {
  REQUIRE(klib::make_file_or_dir_name_legal("你好世界 .") == "你好世界");
  REQUIRE(klib::make_file_or_dir_name_legal("你好\n世界") == "你好世界");
  REQUIRE(klib::make_file_or_dir_name_legal("你好世界***") == "你好世界");
  REQUIRE(klib::make_file_or_dir_name_legal("你好?世界 .") == "你好 世界");
  REQUIRE(klib::make_file_or_dir_name_legal("    你好|\\/世界 .") ==
          "你好   世界");
}
