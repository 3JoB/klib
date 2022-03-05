#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/font.h"
#include "klib/hash.h"
#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("ttf_subset", "[font]") {
  const std::string file_name = "SourceHanSansSC-Bold.ttf";
  REQUIRE(std::filesystem::exists(file_name));

  const std::string out_name = "SourceHanSansSC-Bold.subset.ttf";
  const std::string text = "你好世界";

  const auto ttf_font = klib::read_file(file_name, true);
  const auto subset_font =
      klib::ttf_subset(ttf_font, klib::utf8_to_utf32(text));
  klib::write_file(out_name, true, subset_font);

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "e502ba96488e24198d73b1037c36cd0444056057e871c2f50876b468397cafff");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("woff2", "[font]") {
  const std::string file_name = "SourceHanSansSC-Bold.ttf";
  REQUIRE(std::filesystem::exists(file_name));

  const std::string ttf_font = klib::read_file(file_name, true);

  const auto woff2_font = klib::ttf_to_woff2(ttf_font);
  dbg(std::size(woff2_font));
  const auto new_ttf_font = klib::woff2_to_ttf(woff2_font);
}
