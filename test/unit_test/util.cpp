#include <filesystem>
#include <string>
#include <vector>

#include <dbg.h>
#include <catch2/catch_test_macros.hpp>

#include "klib/util.h"

TEST_CASE("ChangeWorkingDir", "[util]") {
  REQUIRE_FALSE(std::filesystem::exists("work-dir"));

  {
    klib::ChangeWorkingDir dir("work-dir");
    REQUIRE(std::filesystem::exists("../work-dir"));
  }

  CHECK(std::filesystem::remove("work-dir"));
}

TEST_CASE("split_str", "[util]") {
  const std::vector<std::string> std_vec = {"123", "123", "123"};

  auto result = klib::split_str("123abc123abc123abc", "abc");
  CHECK(result == std_vec);

  result = klib::split_str("|||123?123|123!", "|?!");
  CHECK(result == std_vec);

  result = klib::split_str("a\nb\n  c\n", "\n", false);
  CHECK(result == std::vector<std::string>{"a", "b", "  c", ""});
}

TEST_CASE("read_file & write_file", "[util]") {
  REQUIRE(std::filesystem::exists("zlib-ng-2.0.6.tar.gz"));

  auto data = klib::read_file("zlib-ng-2.0.6.tar.gz", true);
  CHECK(std::size(data) == 817951);

  REQUIRE_NOTHROW(klib::write_file("write-file.tar.gz", true, data));

  REQUIRE(std::filesystem::exists("write-file.tar.gz"));
  CHECK(std::filesystem::file_size("write-file.tar.gz") == 817951);

  CHECK(std::filesystem::remove("write-file.tar.gz"));
}

TEST_CASE("execute_command", "[util]") {
  CHECK_NOTHROW(klib::exec("gcc -v"));
  CHECK_NOTHROW(klib::exec(""));

  CHECK_NOTHROW(klib::exec_with_output("clang -v"));
}

TEST_CASE("cleanse", "[util]") {
  std::string password = "123456";
  klib::cleanse(password);

  CHECK(std::empty(password));
}

TEST_CASE("make_file_name_legal", "[util]") {
  CHECK(klib::make_file_name_legal("你好世界 .") == "你好世界");
  CHECK(klib::make_file_name_legal("你好\n世界") == "你好世界");
  CHECK(klib::make_file_name_legal("你好世界***") == "你好世界");
  CHECK(klib::make_file_name_legal("你好?世界 .") == "你好 世界");
  CHECK(klib::make_file_name_legal("    你好|\\/世界 .") == "你好   世界");
}

TEST_CASE("terminal_size", "[util]") {
  auto size = klib::terminal_size();
  dbg(size);
}

TEST_CASE("get_env", "[util]") {
  auto value = klib::get_env("HOME");
  dbg(value);

  value = klib::get_env("HOME2");
  dbg(value);
}
