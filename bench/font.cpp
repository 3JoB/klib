#include <filesystem>
#include <string>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <catch2/catch.hpp>

#include "klib/font.h"
#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("Font", "[font]") {
  const std::string file_name = "SourceHanSansSC-Bold.ttf";
  REQUIRE(std::filesystem::exists(file_name));

  const std::string klib_out_name = "SourceHanSansSC-Bold.subset.klib.ttf";
  const std::string pyftsubset_out_name =
      "SourceHanSansSC-Bold.subset.pyftsubset.ttf";
  const std::string text =
      "咲夜的能力不是时停，而是无质量的高速移动，目前茨歌仙是这样说的，虽然茨歌"
      "仙并非ZUN画的，但是基本设定都是来自于ZUN，所以出现在里面的设定基本上都可"
      "以当一设看待，此外，漫画最新一话既然都已经发出，说明ZUN并没有试图修改这"
      "个设定问题";

  BENCHMARK("klib font_subset") {
    return klib::font_subset(file_name, klib_out_name,
                             klib::utf8_to_utf32(text));
  };
  BENCHMARK("pyftsubset") {
    klib::exec(fmt::format(
        FMT_COMPILE(R"(pyftsubset --output-file={} --text="{}" {})"),
        pyftsubset_out_name, text, file_name));
  };
}
