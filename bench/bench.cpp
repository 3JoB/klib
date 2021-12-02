#include <string>

#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("hash") {
  std::string str = klib::generate_random_bytes(16);

  BENCHMARK("xxhash") { return klib::fast_hash(str); };

  BENCHMARK("std::hash") { return std::hash<std::string>{}(str); };

  BENCHMARK("md5") { return klib::md5(str); };
}

TEST_CASE("hash large") {
  std::string str = klib::generate_random_bytes(102400);

  BENCHMARK("xxhash") { return klib::fast_hash(str); };

  BENCHMARK("std::hash") { return std::hash<std::string>{}(str); };

  BENCHMARK("md5") { return klib::md5(str); };
}
