#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/base64.h"
#include "klib/util.h"

TEST_CASE("base64", "[base64]") {
  const std::string file_name = "zlib-ng-2.0.6.tar.gz";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_file_name = "encoded.txt";

  std::string encoded;
  BENCHMARK("std encoded") {
    klib::exec_with_output("base64 -w0 " + file_name);
  };
  BENCHMARK("klib encoded") {
    encoded = klib::fast_base64_encode(klib::read_file(file_name, true));
  };

  klib::write_file(out_file_name, true, encoded);

  BENCHMARK("std decoded") {
    klib::exec_with_output("base64 -d " + out_file_name);
  };
  BENCHMARK("klib decoded") {
    return klib::fast_base64_decode(klib::read_file(out_file_name, true));
  };

  REQUIRE(std::filesystem::remove(out_file_name));
}
