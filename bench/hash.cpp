#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("Hash", "[hash]") {
  const std::string file_name = "book.tar.gz";
  REQUIRE(std::filesystem::exists(file_name));

  const auto data = klib::read_file(file_name, true);

  BENCHMARK("klib fast hash") { return klib::fast_hash(data); };
  BENCHMARK("klib adler32") { return klib::adler32(data); };
  BENCHMARK("klib crc32") { return klib::crc32(data); };
  BENCHMARK("klib md5") { return klib::md5(data); };
  BENCHMARK("klib sha1") { return klib::sha1(data); };
  BENCHMARK("klib sha256") { return klib::sha256(data); };
}
