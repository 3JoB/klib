#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/font.h"
#include "klib/unicode.h"

TEST_CASE("font_subset", "[font]") {
  const std::string file_name = "SourceHanSansSC-Bold.ttf";
  REQUIRE(std::filesystem::exists(file_name));

  const std::string out_name = "SourceHanSansSC-Bold.subset.ttf";
  const std::string text = "你好世界";
  REQUIRE_NOTHROW(
      klib::font_subset(file_name, out_name, klib::utf8_to_utf32(text)));

  REQUIRE(std::filesystem::exists(out_name));

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}
