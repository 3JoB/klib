#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("fast hash", "[hash]") {
  SECTION("16 byte") {
    auto data = klib::generate_random_bytes(16);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("32 byte") {
    auto data = klib::generate_random_bytes(32);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("64 byte") {
    auto data = klib::generate_random_bytes(64);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("1 kb") {
    auto data = klib::generate_random_bytes(1024);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("10 kb") {
    auto data = klib::generate_random_bytes(10240);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }

  SECTION("100 kb") {
    auto data = klib::generate_random_bytes(102400);
    BENCHMARK("hash") { return klib::fast_hash(data); };
    BENCHMARK("std hash") { return std::hash<std::string>{}(data); };
  }
}

TEST_CASE("secure hash", "[hash]") {
  SECTION("16 byte") {
    auto data = klib::generate_random_bytes(16);
    BENCHMARK("md5") { return klib::md5(data); };
    BENCHMARK("sha256") { return klib::sha256(data); };
  }

  SECTION("32 byte") {
    auto data = klib::generate_random_bytes(32);
    BENCHMARK("md5") { return klib::md5(data); };
    BENCHMARK("sha256") { return klib::sha256(data); };
  }

  SECTION("64 byte") {
    auto data = klib::generate_random_bytes(64);
    BENCHMARK("md5") { return klib::md5(data); };
    BENCHMARK("sha256") { return klib::sha256(data); };
  }

  SECTION("1 kb") {
    auto data = klib::generate_random_bytes(1024);
    BENCHMARK("md5") { return klib::md5(data); };
    BENCHMARK("sha256") { return klib::sha256(data); };
  }

  SECTION("10 kb") {
    auto data = klib::generate_random_bytes(10240);
    BENCHMARK("md5") { return klib::md5(data); };
    BENCHMARK("sha256") { return klib::sha256(data); };
  }

  SECTION("100 kb") {
    auto data = klib::generate_random_bytes(102400);
    BENCHMARK("md5") { return klib::md5(data); };
    BENCHMARK("sha256") { return klib::sha256(data); };
  }

  SECTION("10 mb") {
    auto data = klib::generate_random_bytes(10240000);
    BENCHMARK("md5") { return klib::md5(data); };
    BENCHMARK("sha256") { return klib::sha256(data); };
  }
}
