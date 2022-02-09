#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("fast hash", "[hash]") {
  SECTION("64 byte") {
    auto data = klib::generate_random_bytes(64);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("256 byte") {
    auto data = klib::generate_random_bytes(256);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("512 byte") {
    auto data = klib::generate_random_bytes(512);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("1 kb") {
    auto data = klib::generate_random_bytes(1024);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }
}
