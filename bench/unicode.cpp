#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("UTF convert", "[unicode]") {
  const std::string file_name = "100012892.txt";
  REQUIRE(std::filesystem::exists(file_name));

  const auto data = klib::read_file(file_name, false);

  BENCHMARK("to UTF-16") { return klib::utf8_to_utf16(data); };
  BENCHMARK("to UTF-32") { return klib::utf8_to_utf32(data); };
}
