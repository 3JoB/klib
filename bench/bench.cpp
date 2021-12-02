#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/hash.h"
#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("hash") {
  std::string str = klib::generate_random_bytes(102400);

  BENCHMARK("xxhash") { return klib::fast_hash(str); };

  BENCHMARK("std::hash") { return std::hash<std::string>{}(str); };

  BENCHMARK("md5") { return klib::md5(str); };
}

TEST_CASE("utf") {
  BENCHMARK("simdutf") {
    std::string str = klib::read_file("263060.txt", false);
    return klib::utf8_to_utf16(str);
  };

  BENCHMARK("iconv") {
    klib::exec("iconv -f=UTF-8 -t=UTF-16 -o 263060-UTF-16.txt 263060.txt");
  };
}

TEST_CASE("compress") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);

  BENCHMARK_ADVANCED("tar compress")(Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar -zcf zlib.tar.gz madler-zlib-7085a61"); });

    REQUIRE(std::filesystem::is_regular_file("zlib.tar.gz"));
    std::filesystem::remove("zlib.tar.gz");
  };

  BENCHMARK_ADVANCED("libarchive compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::compress("madler-zlib-7085a61", klib::Algorithm::Gzip); });

    REQUIRE(std::filesystem::is_regular_file("madler-zlib-7085a61.tar.gz"));
    std::filesystem::remove("madler-zlib-7085a61.tar.gz");
  };
}

TEST_CASE("decompress") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(klib::sha3_512_hex(klib::read_file("zlib-v1.2.11.tar.gz", true)) ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");

  BENCHMARK_ADVANCED("tar decompress")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("tar -zxf zlib-v1.2.11.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
  };

  BENCHMARK_ADVANCED("libarchive decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("zlib-v1.2.11.tar.gz", "gzip"); });

    REQUIRE(std::filesystem::is_directory("gzip/madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("gzip/madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("gzip");
  };
}
