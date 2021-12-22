#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/hash.h"
#include "klib/unicode.h"
#include "klib/util.h"

TEST_CASE("compress") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);

  BENCHMARK_ADVANCED("zip")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("zip -q -r zlib.zip madler-zlib-7085a61"); });

    REQUIRE(std::filesystem::is_regular_file("zlib.zip"));
    std::filesystem::remove("zlib.zip");
  };

  BENCHMARK_ADVANCED("libarchive zip")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::compress("madler-zlib-7085a61", klib::Algorithm::Zip); });

    REQUIRE(std::filesystem::is_regular_file("madler-zlib-7085a61.zip"));
    std::filesystem::remove("madler-zlib-7085a61.zip");
  };

  BENCHMARK_ADVANCED("tar gzip")(Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar -zcf zlib.tar.gz madler-zlib-7085a61"); });

    REQUIRE(std::filesystem::is_regular_file("zlib.tar.gz"));
    std::filesystem::remove("zlib.tar.gz");
  };

  BENCHMARK_ADVANCED("libarchive tar gzip")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::compress("madler-zlib-7085a61", klib::Algorithm::Gzip); });

    REQUIRE(std::filesystem::is_regular_file("madler-zlib-7085a61.tar.gz"));
    std::filesystem::remove("madler-zlib-7085a61.tar.gz");
  };

  BENCHMARK_ADVANCED("tar zstd")(Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar --zstd -cf zlib.tar.zst madler-zlib-7085a61"); });

    REQUIRE(std::filesystem::is_regular_file("zlib.tar.zst"));
    std::filesystem::remove("zlib.tar.zst");
  };

  BENCHMARK_ADVANCED("libarchive tar zstd")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::compress("madler-zlib-7085a61", klib::Algorithm::Zstd); });

    REQUIRE(std::filesystem::is_regular_file("madler-zlib-7085a61.tar.zst"));
    std::filesystem::remove("madler-zlib-7085a61.tar.zst");
  };
}

TEST_CASE("decompress") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.zip"));
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.zst"));

  BENCHMARK_ADVANCED("zip")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("unzip -q -o zlib-v1.2.11.zip"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("madler-zlib-7085a61");
  };

  BENCHMARK_ADVANCED("libarchive zip")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("zlib-v1.2.11.zip"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("madler-zlib-7085a61");
  };

  BENCHMARK_ADVANCED("tar gzip")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("tar -zxf zlib-v1.2.11.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("madler-zlib-7085a61");
  };

  BENCHMARK_ADVANCED("libarchive tar gzip")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("zlib-v1.2.11.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("madler-zlib-7085a61");
  };

  BENCHMARK_ADVANCED("tar zstd")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("tar --zstd -xf zlib-v1.2.11.tar.zst"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("madler-zlib-7085a61");
  };

  BENCHMARK_ADVANCED("libarchive tar zstd")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("zlib-v1.2.11.tar.zst"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("madler-zlib-7085a61");
  };
}

TEST_CASE("hash") {
  std::string str = klib::generate_random_bytes(102400);

  BENCHMARK("xxhash") { return klib::fast_hash(str); };

  BENCHMARK("std::hash") { return std::hash<std::string>{}(str); };

  BENCHMARK("md5") { return klib::md5(str); };
}

TEST_CASE("utf") {
  BENCHMARK("simdutf") {
    return klib::utf8_to_utf16(klib::read_file("263060.md", false));
  };

  BENCHMARK("iconv") {
    klib::exec("iconv -f=UTF-8 -t=UTF-16 -o 263060-UTF-16.txt 263060.md");
  };
}
