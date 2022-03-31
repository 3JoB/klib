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
const std::string klib_zip_aes256_name = file_prefix + ".klib.aes256.zip";

const std::string std_7zip_name = file_prefix + ".std.7z";
const std::string klib_7zip_name = file_prefix + ".klib.7z";

const std::string std_tar_name = file_prefix + ".std.tar";
const std::string klib_tar_name = file_prefix + ".klib.tar";

const std::string std_gzip_name = file_prefix + ".std.tar.gz";
const std::string klib_gzip_name = file_prefix + ".klib.tar.gz";

const std::string std_xz_name = file_prefix + ".std.tar.xz";
const std::string klib_xz_name = file_prefix + ".klib.tar.xz";

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

TEST_CASE_METHOD(TestsFixture, "ZIP", "[archive]") {
  BENCHMARK_ADVANCED("zip compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(std_zip_name);

    meter.measure(
        [] { klib::exec("zip -q -r " + std_zip_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_zip_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(klib_zip_name);

    meter.measure([] {
      klib::compress(dir_name, klib::Format::Zip, klib::Filter::Deflate,
                     klib_zip_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_zip_name));
  };

  BENCHMARK_ADVANCED("unzip decompress")
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

  BENCHMARK_ADVANCED("klib compress aes256")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(klib_zip_aes256_name);

    meter.measure([] {
      klib::compress(dir_name, klib::Format::Zip, klib::Filter::Deflate,
                     klib_zip_aes256_name, true, 6, "kaiser123");
    });

    REQUIRE(std::filesystem::is_regular_file(klib_zip_aes256_name));
  };

  BENCHMARK_ADVANCED("klib decompress aes256")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure(
        [] { klib::decompress(klib_zip_aes256_name, "", "kaiser123"); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };
}

TEST_CASE_METHOD(TestsFixture, "7-Zip", "[archive]") {
  BENCHMARK_ADVANCED("7z compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(std_7zip_name);

    meter.measure([] {
      klib::exec("7z a -mm=Deflate -mx=4 -bso0 -bsp0 -aoa -mmt1 " +
                 std_7zip_name + " " + dir_name);
    });

    REQUIRE(std::filesystem::is_regular_file(std_7zip_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(klib_7zip_name);

    meter.measure([] {
      klib::compress(dir_name, klib::Format::The7Zip, klib::Filter::Deflate,
                     klib_7zip_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_7zip_name));
  };

  BENCHMARK_ADVANCED("7z decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure(
        [] { klib::exec("7z x -bso0 -bsp0 -aoa -mmt1 " + std_7zip_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  BENCHMARK_ADVANCED("klib decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::decompress(klib_7zip_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };
}

TEST_CASE_METHOD(TestsFixture, "Tar", "[archive]") {
  BENCHMARK_ADVANCED("tar compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(std_tar_name);

    meter.measure(
        [] { klib::exec("tar -cf " + std_tar_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_tar_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(klib_tar_name);

    meter.measure([] {
      klib::compress(dir_name, klib::Format::USTar, klib::Filter::None,
                     klib_tar_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_tar_name));
  };

  BENCHMARK_ADVANCED("tar decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::exec("tar -xf " + std_tar_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  BENCHMARK_ADVANCED("klib decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::decompress(klib_tar_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };
}

TEST_CASE_METHOD(TestsFixture, "Gzip", "[archive]") {
  BENCHMARK_ADVANCED("tar compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(std_gzip_name);

    meter.measure(
        [] { klib::exec("tar -zcf " + std_gzip_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_gzip_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(klib_gzip_name);

    meter.measure([] {
      klib::compress(dir_name, klib::Format::USTar, klib::Filter::Gzip,
                     klib_gzip_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_gzip_name));
  };

  BENCHMARK_ADVANCED("tar decompress")
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
}

TEST_CASE_METHOD(TestsFixture, "xz", "[archive]") {
  BENCHMARK_ADVANCED("tar compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(std_xz_name);

    meter.measure(
        [] { klib::exec("tar --xz -cf " + std_xz_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_xz_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(klib_xz_name);

    meter.measure([] {
      klib::compress(dir_name, klib::Format::USTar, klib::Filter::LZMA,
                     klib_xz_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_xz_name));
  };

  BENCHMARK_ADVANCED("tar decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::exec("tar --xz -xf " + std_xz_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };

  BENCHMARK_ADVANCED("klib decompress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove_all(dir_name);
    meter.measure([] { klib::decompress(klib_xz_name); });
    REQUIRE(std::filesystem::is_directory(dir_name));
  };
}

TEST_CASE_METHOD(TestsFixture, "Zstandard", "[archive]") {
  BENCHMARK_ADVANCED("tar compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(std_zstd_name);

    meter.measure(
        [] { klib::exec("tar --zstd -cf " + std_zstd_name + " " + dir_name); });

    REQUIRE(std::filesystem::is_regular_file(std_zstd_name));
  };

  BENCHMARK_ADVANCED("klib compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(klib_zstd_name);

    meter.measure([] {
      klib::compress(dir_name, klib::Format::USTar, klib::Filter::Zstd,
                     klib_zstd_name);
    });

    REQUIRE(std::filesystem::is_regular_file(klib_zstd_name));
  };

  BENCHMARK_ADVANCED("tar decompress")
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
}

TEST_CASE("Compress data", "[archive]") {
  const std::string file_name = "book.tar";
  REQUIRE(std::filesystem::exists(file_name));

  const auto data = klib::read_file(file_name, true);

  const auto compressed = klib::compress_data(data);

  BENCHMARK("zstd compress") { return klib::compress_data(data); };
  BENCHMARK("zstd decompress") { return klib::decompress_data(compressed); };
}
