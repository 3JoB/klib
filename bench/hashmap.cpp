#include <cstdint>
#include <set>
#include <unordered_set>
#include <vector>

#include <parallel_hashmap/btree.h>
#include <parallel_hashmap/phmap.h>
#include <catch2/catch.hpp>

TEST_CASE("Hashmap", "[hashmap]") {
  constexpr std::int32_t times = 100'0000;

  std::random_device rd;
  std::default_random_engine gen(rd());
  std::vector<std::int32_t> v;
  v.reserve(times);
  for (std::int32_t i = 0; i < times; ++i) {
    v.push_back(gen());
  }

  std::set<std::int32_t> std_set;
  phmap::btree_set<std::int32_t> ph_set;
  std::unordered_set<std::int32_t> std_hashset;
  phmap::flat_hash_set<std::int32_t> ph_hashset;

  BENCHMARK("std::set insert") {
    for (auto i : v) {
      std_set.insert(i);
    }
  };
  BENCHMARK("phmap::btree_set insert") {
    for (auto i : v) {
      ph_set.insert(i);
    }
  };
  BENCHMARK("std::unordered_set insert") {
    for (auto i : v) {
      std_hashset.insert(i);
    }
  };
  BENCHMARK("phmap::flat_hash_set insert") {
    for (auto i : v) {
      ph_hashset.insert(i);
    }
  };

  std::vector<std::int32_t> keys;
  keys.reserve(times);
  for (std::int32_t i = 0; i < times / 2; ++i) {
    keys.push_back(v[gen() % times]);
  }
  for (std::int32_t i = 0; i < times / 2; ++i) {
    keys.push_back(gen());
  }

  BENCHMARK("std::set find") {
    bool flag;
    for (auto key : keys) {
      flag = std_set.contains(key);
    }
    return flag;
  };
  BENCHMARK("phmap::btree_set find") {
    bool flag;
    for (auto key : keys) {
      flag = ph_set.contains(key);
    }
    return flag;
  };
  BENCHMARK("std::unordered_set find") {
    bool flag;
    for (auto key : keys) {
      flag = std_hashset.contains(key);
    }
    return flag;
  };
  BENCHMARK("phmap::flat_hash_set find") {
    bool flag;
    for (auto key : keys) {
      flag = ph_hashset.contains(key);
    }
    return flag;
  };
}
