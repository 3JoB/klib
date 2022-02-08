#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

namespace {

class TestsFixture {
 public:
  TestsFixture() {
    std::filesystem::remove_all("CMake-3.22.2");
    REQUIRE(std::filesystem::exists("CMake-3.22.2.tar.gz"));
    REQUIRE_NOTHROW(klib::exec("tar -zxf CMake-3.22.2.tar.gz"));
    REQUIRE(std::filesystem::exists("CMake-3.22.2"));
  }
};

}  // namespace

TEST_CASE_METHOD(TestsFixture, "zip", "[archive]") {
  BENCHMARK_ADVANCED("system zip compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("zip -q -r CMake-3.22.2-zip.zip CMake-3.22.2"); });

    REQUIRE(std::filesystem::is_regular_file("CMake-3.22.2-zip.zip"));
  };

  BENCHMARK_ADVANCED("libarchive compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress("CMake-3.22.2", klib::Format::Zip, klib::Filter::Deflate,
                     "CMake-3.22.2-libarchive.zip");
    });

    REQUIRE(std::filesystem::is_regular_file("CMake-3.22.2-libarchive.zip"));
  };

  BENCHMARK_ADVANCED("system zip decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("unzip -q -o CMake-3.22.2-zip.zip"); });

    REQUIRE(std::filesystem::is_directory("CMake-3.22.2"));
  };

  BENCHMARK_ADVANCED("libarchive decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("CMake-3.22.2-libarchive.zip"); });

    REQUIRE(std::filesystem::is_directory("CMake-3.22.2"));
  };

  std::filesystem::remove_all("CMake-3.22.2-zip.zip");
  std::filesystem::remove_all("CMake-3.22.2-libarchive.zip");
}

TEST_CASE_METHOD(TestsFixture, "gzip", "[archive]") {
  BENCHMARK_ADVANCED("system tar compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar -zcf CMake-3.22.2-tar.tar.gz CMake-3.22.2"); });

    REQUIRE(std::filesystem::is_regular_file("CMake-3.22.2-tar.tar.gz"));
  };

  BENCHMARK_ADVANCED("libarchive compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress("CMake-3.22.2", klib::Format::Tar, klib::Filter::Gzip,
                     "CMake-3.22.2-libarchive.tar.gz");
    });

    REQUIRE(std::filesystem::is_regular_file("CMake-3.22.2-libarchive.tar.gz"));
  };

  BENCHMARK_ADVANCED("system tar decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::exec("tar -zxf CMake-3.22.2-tar.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("CMake-3.22.2"));
  };

  BENCHMARK_ADVANCED("libarchive decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("CMake-3.22.2-libarchive.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("CMake-3.22.2"));
  };

  std::filesystem::remove_all("CMake-3.22.2-tar.tar.gz");
  std::filesystem::remove_all("CMake-3.22.2-libarchive.tar.gz");
}

TEST_CASE_METHOD(TestsFixture, "zstd", "[archive]") {
  BENCHMARK_ADVANCED("system tar compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::exec("tar --zstd -cf CMake-3.22.2-tar.tar.zst CMake-3.22.2");
    });

    REQUIRE(std::filesystem::is_regular_file("CMake-3.22.2-tar.tar.zst"));
  };

  BENCHMARK_ADVANCED("libarchive compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress("CMake-3.22.2", klib::Format::Tar, klib::Filter::Zstd,
                     "CMake-3.22.2-libarchive.tar.zst");
    });

    REQUIRE(
        std::filesystem::is_regular_file("CMake-3.22.2-libarchive.tar.zst"));
  };

  BENCHMARK_ADVANCED("system tar decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar --zstd -xf CMake-3.22.2-tar.tar.zst"); });

    REQUIRE(std::filesystem::is_directory("CMake-3.22.2"));
  };

  BENCHMARK_ADVANCED("libarchive decompress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] { klib::decompress("CMake-3.22.2-libarchive.tar.zst"); });

    REQUIRE(std::filesystem::is_directory("CMake-3.22.2"));
  };

  std::filesystem::remove_all("CMake-3.22.2-tar.tar.zst");
  std::filesystem::remove_all("CMake-3.22.2-libarchive.tar.zst");
}
