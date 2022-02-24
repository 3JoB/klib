#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <random>
#include <vector>

#include <oneapi/tbb/parallel_sort.h>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include <boost/sort/pdqsort/pdqsort.hpp>
#include <catch2/catch.hpp>

TEST_CASE("sort", "[sort]") {
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
