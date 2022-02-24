#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include <oneapi/tbb/parallel_sort.h>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include <boost/sort/pdqsort/pdqsort.hpp>
#include <boost/sort/spreadsort/spreadsort.hpp>
#include <catch2/catch.hpp>

#include "klib/util.h"

TEST_CASE("integer", "[sort]") {
  constexpr std::size_t size = 1000'0000;
  std::vector<std::int32_t> nums;
  nums.reserve(size);

  std::random_device rd;
  std::default_random_engine gen(rd());
  for (std::size_t i = 0; i < size; ++i) {
    nums.push_back(gen());
  }

  BENCHMARK_ADVANCED("std sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] { std::sort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("spread sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] {
      boost::sort::spreadsort::spreadsort(std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("pdq sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure(
        [&] { boost::sort::pdqsort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("tbb sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure(
        [&] { tbb::parallel_sort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("std sort parallel")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] {
      std::sort(std::execution::par, std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("block indirect sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] {
      boost::sort::block_indirect_sort(std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };
}

TEST_CASE("double", "[sort]") {
  constexpr std::size_t size = 1000'0000;
  std::vector<double> nums;
  nums.reserve(size);

  std::random_device rd;
  std::default_random_engine gen(rd());
  std::uniform_real_distribution<> dis(std::numeric_limits<double>::min(),
                                       std::numeric_limits<double>::max());
  for (std::size_t i = 0; i < size; ++i) {
    nums.push_back(dis(gen));
  }

  BENCHMARK_ADVANCED("std sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] { std::sort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("spread sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] {
      boost::sort::spreadsort::spreadsort(std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("pdq sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure(
        [&] { boost::sort::pdqsort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("tbb sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure(
        [&] { tbb::parallel_sort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("std sort parallel")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] {
      std::sort(std::execution::par, std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("block indirect sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = nums;
    meter.measure([&] {
      boost::sort::block_indirect_sort(std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };
}

TEST_CASE("string", "[sort]") {
  constexpr std::size_t size = 100'0000;
  std::vector<std::string> strs;
  strs.reserve(size);

  for (std::size_t i = 0; i < size; ++i) {
    strs.push_back(klib::generate_random_bytes(32));
  }

  BENCHMARK_ADVANCED("std sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = strs;
    meter.measure([&] { std::sort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("spread sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = strs;
    meter.measure([&] {
      boost::sort::spreadsort::spreadsort(std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("pdq sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = strs;
    meter.measure(
        [&] { boost::sort::pdqsort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("tbb sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = strs;
    meter.measure(
        [&] { tbb::parallel_sort(std::begin(copy), std::end(copy)); });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("std sort parallel")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = strs;
    meter.measure([&] {
      std::sort(std::execution::par, std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };

  BENCHMARK_ADVANCED("block indirect sort")
  (Catch::Benchmark::Chronometer meter) {
    auto copy = strs;
    meter.measure([&] {
      boost::sort::block_indirect_sort(std::begin(copy), std::end(copy));
    });
    REQUIRE(std::is_sorted(std::begin(copy), std::end(copy)));
  };
}
