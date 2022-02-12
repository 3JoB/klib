#include <filesystem>
#include <string>

#include <archive.h>
#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

namespace {

class TestsFixture {
 public:
  TestsFixture() {
    std::filesystem::remove_all("zlib-ng-2.0.6");
    REQUIRE(std::filesystem::exists("zlib-ng-2.0.6.tar.gz"));
    REQUIRE_NOTHROW(klib::exec("tar -zxf zlib-ng-2.0.6.tar.gz"));
    REQUIRE(std::filesystem::exists("zlib-ng-2.0.6"));

    REQUIRE(std::filesystem::exists("zlib-ng-2.0.6.rar"));
  }
};

}  // namespace

TEST_CASE("libarchive version", "[archive]") {
  CHECK(archive_zlib_version() == std::string("1.2.11.zlib-ng"));
  CHECK(archive_libzstd_version() == std::string("1.5.2"));
}

TEST_CASE_METHOD(TestsFixture, "zip none", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Zip,
                                 klib::Filter::None, "zip-none.zip"));
  dbg(std::filesystem::file_size("zip-none.zip"));
  REQUIRE_NOTHROW(klib::decompress("zip-none.zip", "zip-none"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 zip-none/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("zip-none.zip"));
  CHECK(std::filesystem::remove_all("zip-none"));
}

TEST_CASE_METHOD(TestsFixture, "zip deflate", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Zip,
                                 klib::Filter::Deflate, "zip-deflate.zip"));
  dbg(std::filesystem::file_size("zip-deflate.zip"));
  REQUIRE_NOTHROW(klib::decompress("zip-deflate.zip", "zip-deflate"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 zip-deflate/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("zip-deflate.zip"));
  CHECK(std::filesystem::remove_all("zip-deflate"));
}

TEST_CASE_METHOD(TestsFixture, "zip password", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Zip,
                                 klib::Filter::Deflate, "zip-password.zip",
                                 true, "kaiser123"));
  dbg(std::filesystem::file_size("zip-password.zip"));
  REQUIRE_NOTHROW(
      klib::decompress("zip-password.zip", "zip-password", "kaiser123"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 zip-password/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("zip-password.zip"));
  CHECK(std::filesystem::remove_all("zip-password"));
}

TEST_CASE_METHOD(TestsFixture, "7-zip none", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::The7Zip,
                                 klib::Filter::None, "7-zip-none.7z"));
  dbg(std::filesystem::file_size("7-zip-none.7z"));
  REQUIRE_NOTHROW(klib::decompress("7-zip-none.7z", "7-zip-none"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 7-zip-none/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("7-zip-none.7z"));
  CHECK(std::filesystem::remove_all("7-zip-none"));
}

TEST_CASE_METHOD(TestsFixture, "7-zip deflate", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::The7Zip,
                                 klib::Filter::Deflate, "7-zip-deflate.7z"));
  dbg(std::filesystem::file_size("7-zip-deflate.7z"));
  REQUIRE_NOTHROW(klib::decompress("7-zip-deflate.7z", "7-zip-deflate"));
  CHECK_NOTHROW(
      klib::exec("diff -r zlib-ng-2.0.6 7-zip-deflate/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("7-zip-deflate.7z"));
  CHECK(std::filesystem::remove_all("7-zip-deflate"));
}

TEST_CASE_METHOD(TestsFixture, "tar none", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Tar,
                                 klib::Filter::None, "tar-none.tar"));
  dbg(std::filesystem::file_size("tar-none.tar"));
  REQUIRE_NOTHROW(klib::decompress("tar-none.tar", "tar-none"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 tar-none/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("tar-none.tar"));
  CHECK(std::filesystem::remove_all("tar-none"));
}

TEST_CASE_METHOD(TestsFixture, "tar gzip", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Tar,
                                 klib::Filter::Gzip, "tar-gzip.tar.gz"));
  dbg(std::filesystem::file_size("tar-gzip.tar.gz"));
  REQUIRE_NOTHROW(klib::decompress("tar-gzip.tar.gz", "tar-gzip"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 tar-gzip/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("tar-gzip.tar.gz"));
  CHECK(std::filesystem::remove_all("tar-gzip"));
}

TEST_CASE_METHOD(TestsFixture, "tar zstd", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Tar,
                                 klib::Filter::Zstd, "tar-zstd.tar.zst"));
  dbg(std::filesystem::file_size("tar-zstd.tar.zst"));
  REQUIRE_NOTHROW(klib::decompress("tar-zstd.tar.zst", "tar-zstd"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 tar-zstd/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove("tar-zstd.tar.zst"));
  CHECK(std::filesystem::remove_all("tar-zstd"));
}

TEST_CASE_METHOD(TestsFixture, "rar", "[archive]") {
  dbg(std::filesystem::file_size("zlib-ng-2.0.6.rar"));
  REQUIRE_NOTHROW(klib::decompress("zlib-ng-2.0.6.rar", "rar"));
  CHECK_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 rar/zlib-ng-2.0.6"));

  CHECK(std::filesystem::remove_all("rar"));
}

TEST_CASE("outermost_folder_name", "[archive]") {
  REQUIRE(std::filesystem::exists("zlib-ng-2.0.6.tar.gz"));
  CHECK(*klib::outermost_folder_name("zlib-ng-2.0.6.tar.gz") ==
        "zlib-ng-2.0.6");
}

TEST_CASE("compress data", "[archive]") {
  std::string data =
      "Hello World!! 11111111133333333333333333333333333333333333333333333";
  std::string compressed, decompressed;

  dbg(std::size(data));
  REQUIRE_NOTHROW(compressed = klib::compress_data(data));
  dbg(std::size(compressed));
  dbg(compressed);
  REQUIRE_NOTHROW(decompressed = klib::decompress_data(compressed));
  CHECK(data == decompressed);
}
