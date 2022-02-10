#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <unordered_set>

#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

namespace {

struct KeyHash {
  std::size_t operator()(const std::string &key) const {
    return klib::fast_hash(key);
  }
};

}  // namespace

TEST_CASE("fast hash", "[hash]") {
  constexpr std::int32_t times = 1000000;
  constexpr std::size_t size = 256;

  std::vector<std::string> str;
  str.reserve(times);
  for (std::int32_t i = 0; i < times; ++i) {
    str.push_back(klib::generate_random_bytes(size));
  }

  std::random_device rd;
  std::default_random_engine gen(rd());
  std::uniform_int_distribution<> dis(0, times - 1);

  BENCHMARK_ADVANCED("std hash")
  (Catch::Benchmark::Chronometer meter) {
    std::unordered_set<std::string> set;
    meter.measure([&] {
      for (const auto &s : str) {
        set.insert(s);
      }
      for (std::int32_t i = 0; i < times / 4; ++i) {
        set.find(str[dis(gen)]);
        set.find(klib::generate_random_bytes(size));
      }
    });
  };

  BENCHMARK_ADVANCED("klib fast hash")
  (Catch::Benchmark::Chronometer meter) {
    std::unordered_set<std::string, KeyHash> set;
    meter.measure([&] {
      for (const auto &s : str) {
        set.insert(s);
      }
      for (std::int32_t i = 0; i < times / 4; ++i) {
        set.find(str[dis(gen)]);
        set.find(klib::generate_random_bytes(size));
      }
    });
  };
}
