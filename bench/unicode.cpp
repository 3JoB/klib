#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("UTF convert short", "[unicode]") {
  const auto data =
      "咲夜的能力不是时停，而是无质量的高速移动，目前茨歌仙是这样说的，虽然茨歌"
      "仙并非ZUN画的，但是基本设定都是来自于ZUN，所以出现在里面的设定基本上都可"
      "以当一设看待，此外，漫画最新一话既然都已经发出，说明ZUN并没有试图修改这"
      "个设定问题";

  BENCHMARK("to UTF-16") { return klib::utf8_to_utf16(data); };
  BENCHMARK("to UTF-32") { return klib::utf8_to_utf32(data); };
}

TEST_CASE("UTF convert long", "[unicode]") {
  const std::string file_name = "100012892.txt";
  REQUIRE(std::filesystem::exists(file_name));

  const auto data = klib::read_file(file_name, false);

  BENCHMARK("to UTF-16") { return klib::utf8_to_utf16(data); };
  BENCHMARK("to UTF-32") { return klib::utf8_to_utf32(data); };
}
