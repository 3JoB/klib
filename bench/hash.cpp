#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("secure hash", "[hash]") {
  const std::string file_name = "book.tar.gz";
  REQUIRE(std::filesystem::exists(file_name));

  const auto data = klib::read_file(file_name, true);

  BENCHMARK("klib md5") { return klib::md5(data); };
  BENCHMARK("klib sha256") { return klib::sha256(data); };
}
