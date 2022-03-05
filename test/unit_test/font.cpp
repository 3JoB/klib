#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/font.h"
#include "klib/hash.h"
#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("font_subset", "[font]") {
  const std::string file_name = "SourceHanSansSC-Bold.ttf";
  REQUIRE(std::filesystem::exists(file_name));

  const std::string out_name = "SourceHanSansSC-Bold.subset.ttf";
  const std::string text = "你好世界";
  REQUIRE_NOTHROW(
      klib::font_subset(file_name, out_name, klib::utf8_to_utf32(text)));

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "e502ba96488e24198d73b1037c36cd0444056057e871c2f50876b468397cafff");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}
