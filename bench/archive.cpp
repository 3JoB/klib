#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

TEST_CASE("compress", "[archive]") {
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

TEST_CASE("decompress", "[archive]") {
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
