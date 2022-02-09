#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

namespace {

const std::string file_prefix = "book";
const std::string dir_name = file_prefix;
const std::string gzip_name = file_prefix + ".tar.gz";

const std::string std_zip_name = file_prefix + ".std.zip";
const std::string klib_zip_name = file_prefix + ".klib.zip";

const std::string std_gzip_name = file_prefix + ".std.tar.gz";
const std::string klib_gzip_name = file_prefix + ".klib.tar.gz";

const std::string std_zstd_name = file_prefix + ".std.tar.zst";
const std::string klib_zstd_name = file_prefix + ".klib.tar.zst";

class TestsFixture {
 public:
  TestsFixture() {
    std::filesystem::remove_all(dir_name);
    REQUIRE(std::filesystem::exists(gzip_name));
    REQUIRE_NOTHROW(klib::exec("tar -zxf " + gzip_name));
    REQUIRE(std::filesystem::exists(dir_name));
  }
};

}  // namespace

TEST_CASE_METHOD(TestsFixture, "zip", "[archive]") {
  BENCHMARK_ADVANCED("std compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("zip -q -r " + std_zip_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_zip_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress(dir_name, klib::Format::Zip, klib::Filter::Deflate,
                     klib_zip_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_zip_name));
  };

  BENCHMARK_ADVANCED("std decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::exec("unzip -q -o " + std_zip_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  BENCHMARK_ADVANCED("klib decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::decompress(klib_zip_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  REQUIRE(std::filesystem::remove_all(std_zip_name));
  REQUIRE(std::filesystem::remove_all(klib_zip_name));
}

TEST_CASE_METHOD(TestsFixture, "gzip", "[archive]") {
  BENCHMARK_ADVANCED("std compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar -zcf " + std_gzip_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_gzip_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress(dir_name, klib::Format::Tar, klib::Filter::Gzip,
                     klib_gzip_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_gzip_name));
  };

  BENCHMARK_ADVANCED("std decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::exec("tar -zxf " + std_gzip_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  BENCHMARK_ADVANCED("klib decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::decompress(klib_gzip_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  REQUIRE(std::filesystem::remove_all(std_gzip_name));
  REQUIRE(std::filesystem::remove_all(klib_gzip_name));
}

TEST_CASE_METHOD(TestsFixture, "zstd", "[archive]") {
  BENCHMARK_ADVANCED("std compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::exec("tar --zstd -cf " + std_zstd_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_zstd_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::compress(dir_name, klib::Format::Tar, klib::Filter::Zstd,
                     klib_zstd_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_zstd_name));
  };

  BENCHMARK_ADVANCED("std decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::exec("tar --zstd -xf " + std_zstd_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  BENCHMARK_ADVANCED("klib decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::decompress(klib_zstd_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  REQUIRE(std::filesystem::remove_all(std_zstd_name));
  REQUIRE(std::filesystem::remove_all(klib_zstd_name));
}
