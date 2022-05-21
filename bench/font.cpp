#include <filesystem>
#include <string>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "klib/font.h"
#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("Font", "[font]") {
  const std::string file_name = "SourceHanSansSC-Bold.ttf";
  REQUIRE(std::filesystem::exists(file_name));

  const std::string klib_ttf_out_name = "SourceHanSansSC-Bold.subset.klib.ttf";
  const std::string pyftsubset_ttf_out_name =
      "SourceHanSansSC-Bold.subset.pyftsubset.ttf";
  const std::string klib_woff2_out_name =
      "SourceHanSansSC-Bold.subset.klib.woff2";
  const std::string pyftsubset_woff2_out_name =
      "SourceHanSansSC-Bold.subset.pyftsubset.woff2";
  const std::string text =
      "咲夜的能力不是时停，而是无质量的高速移动，目前茨歌仙是这样说的，虽然茨歌"
      "仙并非ZUN画的，但是基本设定都是来自于ZUN，所以出现在里面的设定基本上都可"
      "以当一设看待，此外，漫画最新一话既然都已经发出，说明ZUN并没有试图修改这"
      "个设定问题";

  BENCHMARK_ADVANCED("pyftsubset ttf")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([&] {
      klib::exec(fmt::format(
          FMT_COMPILE(R"(pyftsubset --output-file={} --text="{}" {})"),
          pyftsubset_ttf_out_name, text, file_name));
    });

    REQUIRE(std::filesystem::remove(pyftsubset_ttf_out_name));
  };

  BENCHMARK_ADVANCED("pyftsubset woff2")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([&] {
      klib::exec(fmt::format(
          FMT_COMPILE(
              R"(pyftsubset --flavor=woff2 --output-file={} --text="{}" {})"),
          pyftsubset_woff2_out_name, text, file_name));
    });

    REQUIRE(std::filesystem::remove(pyftsubset_woff2_out_name));
  };

  BENCHMARK_ADVANCED("klib ttf")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([&] {
      const auto ttf_font = klib::read_file(file_name, true);
      auto subset_font = klib::ttf_subset(ttf_font, klib::utf8_to_utf32(text));
      klib::write_file(klib_ttf_out_name, true, subset_font);
    });

    REQUIRE(std::filesystem::remove(klib_ttf_out_name));
  };

  BENCHMARK_ADVANCED("klib woff2")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([&] {
      const auto ttf_font = klib::read_file(file_name, true);
      auto subset_font = klib::ttf_subset(ttf_font, klib::utf8_to_utf32(text));
      auto woff2_font = klib::ttf_to_woff2(subset_font);
      klib::write_file(klib_woff2_out_name, true, woff2_font);
    });

    REQUIRE(std::filesystem::remove(klib_woff2_out_name));
  };
}
