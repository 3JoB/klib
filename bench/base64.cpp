#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/base64.h"
#include "klib/util.h"

TEST_CASE("Base64", "[base64]") {
  const std::string file_name = "book.tar.gz";
  REQUIRE(std::filesystem::exists(file_name));

  const auto data = klib::read_file(file_name, true);
  const auto encoded = klib::fast_base64_encode(data);

  BENCHMARK("klib secure encoded") { return klib::secure_base64_encode(data); };
  BENCHMARK("klib fast encoded") { return klib::fast_base64_encode(data); };

  BENCHMARK("klib secure decoded") {
    return klib::secure_base64_decode(encoded);
  };
  BENCHMARK("klib fast decoded") { return klib::fast_base64_decode(encoded); };
}
