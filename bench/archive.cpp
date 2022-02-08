#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

namespace {

class TestsFixture {
 public:
  TestsFixture() {
    if (!std::filesystem::exists("zlib-ng-2.0.6")) {
      REQUIRE(std::filesystem::exists("zlib-ng-2.0.6.tar.gz"));
      REQUIRE_NOTHROW(klib::exec("tar -zxf zlib-ng-2.0.6.tar.gz"));
    }
  }
};

}  // namespace

TEST_CASE_METHOD(TestsFixture, "zip", "[archive]") {
  BENCHMARK_ADVANCED("system zip compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("zip -q -r zlib-ng-2.0.6-zip.zip zlib-ng-2.0.6"); });

    REQUIRE(std::filesystem::is_regular_file("zlib-ng-2.0.6-zip.zip"));
  };

  BENCHMARK_ADVANCED("libarchive compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress("zlib-ng-2.0.6", klib::Format::Zip, klib::Filter::Deflate,
                     "zlib-ng-2.0.6-libarchive.zip");
    });

    REQUIRE(std::filesystem::is_regular_file("zlib-ng-2.0.6-libarchive.zip"));
  };

  BENCHMARK_ADVANCED("system zip decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("unzip -q -o zlib-ng-2.0.6-zip.zip"); });

    REQUIRE(std::filesystem::is_directory("zlib-ng-2.0.6"));
  };

  BENCHMARK_ADVANCED("libarchive decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("zlib-ng-2.0.6-libarchive.zip"); });

    REQUIRE(std::filesystem::is_directory("zlib-ng-2.0.6"));
  };

  std::filesystem::remove_all("zlib-ng-2.0.6-zip.zip");
  std::filesystem::remove_all("zlib-ng-2.0.6-libarchive.zip");
}

TEST_CASE_METHOD(TestsFixture, "gzip", "[archive]") {
  BENCHMARK_ADVANCED("system tar compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar -zcf zlib-ng-2.0.6-tar.tar.gz zlib-ng-2.0.6"); });

    REQUIRE(std::filesystem::is_regular_file("zlib-ng-2.0.6-tar.tar.gz"));
  };

  BENCHMARK_ADVANCED("libarchive compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress("zlib-ng-2.0.6", klib::Format::Tar, klib::Filter::Gzip,
                     "zlib-ng-2.0.6-libarchive.tar.gz");
    });

    REQUIRE(
        std::filesystem::is_regular_file("zlib-ng-2.0.6-libarchive.tar.gz"));
  };

  BENCHMARK_ADVANCED("system tar decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("tar -zxf zlib-ng-2.0.6-tar.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("zlib-ng-2.0.6"));
  };

  BENCHMARK_ADVANCED("libarchive decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("zlib-ng-2.0.6-libarchive.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("zlib-ng-2.0.6"));
  };

  std::filesystem::remove_all("zlib-ng-2.0.6-tar.tar.gz");
  std::filesystem::remove_all("zlib-ng-2.0.6-libarchive.tar.gz");
}

TEST_CASE_METHOD(TestsFixture, "zstd", "[archive]") {
  BENCHMARK_ADVANCED("system tar compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::exec("tar --zstd -cf zlib-ng-2.0.6-tar.tar.zst zlib-ng-2.0.6");
    });

    REQUIRE(std::filesystem::is_regular_file("zlib-ng-2.0.6-tar.tar.zst"));
  };

  BENCHMARK_ADVANCED("libarchive compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress("zlib-ng-2.0.6", klib::Format::Tar, klib::Filter::Zstd,
                     "zlib-ng-2.0.6-libarchive.tar.zst");
    });

    REQUIRE(
        std::filesystem::is_regular_file("zlib-ng-2.0.6-libarchive.tar.zst"));
  };

  BENCHMARK_ADVANCED("system tar decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar --zstd -xf zlib-ng-2.0.6-tar.tar.zst"); });

    REQUIRE(std::filesystem::is_directory("zlib-ng-2.0.6"));
  };

  BENCHMARK_ADVANCED("libarchive decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("zlib-ng-2.0.6-libarchive.tar.zst"); });

    REQUIRE(std::filesystem::is_directory("zlib-ng-2.0.6"));
  };

  std::filesystem::remove_all("zlib-ng-2.0.6-tar.tar.zst");
  std::filesystem::remove_all("zlib-ng-2.0.6-libarchive.tar.zst");
}
