#include <filesystem>
#include <string>

#include <dbg.h>
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

TEST_CASE_METHOD(TestsFixture, "zip none", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Zip,
                                 klib::Filter::None, "zip-none.zip"));
  dbg(std::filesystem::file_size("zip-none.zip"));
  REQUIRE_NOTHROW(klib::decompress("zip-none.zip", "zip-none"));
  REQUIRE_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 zip-none/zlib-ng-2.0.6"));

  std::remove("zip-none.zip");
  std::filesystem::remove_all("zip-none");
}

TEST_CASE_METHOD(TestsFixture, "zip deflate", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Zip,
                                 klib::Filter::Deflate, "zip-deflate.zip"));
  dbg(std::filesystem::file_size("zip-deflate.zip"));
  REQUIRE_NOTHROW(klib::decompress("zip-deflate.zip", "zip-deflate"));
  REQUIRE_NOTHROW(
      klib::exec("diff -r zlib-ng-2.0.6 zip-deflate/zlib-ng-2.0.6"));

  std::remove("zip-deflate.zip");
  std::filesystem::remove_all("zip-deflate");
}

TEST_CASE_METHOD(TestsFixture, "tar none", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Tar,
                                 klib::Filter::None, "tar-none.tar"));
  dbg(std::filesystem::file_size("tar-none.tar"));
  REQUIRE_NOTHROW(klib::decompress("tar-none.tar", "tar-none"));
  REQUIRE_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 tar-none/zlib-ng-2.0.6"));

  std::remove("tar-none.tar");
  std::filesystem::remove_all("tar-none");
}

TEST_CASE_METHOD(TestsFixture, "tar gzip", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Tar,
                                 klib::Filter::Gzip, "tar-gzip.tar.gz"));
  dbg(std::filesystem::file_size("tar-gzip.tar.gz"));
  REQUIRE_NOTHROW(klib::decompress("tar-gzip.tar.gz", "tar-gzip"));
  REQUIRE_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 tar-gzip/zlib-ng-2.0.6"));

  std::remove("tar-gzip.tar.gz");
  std::filesystem::remove_all("tar-gzip");
}

TEST_CASE_METHOD(TestsFixture, "tar zstd", "[archive]") {
  REQUIRE_NOTHROW(klib::compress("zlib-ng-2.0.6", klib::Format::Tar,
                                 klib::Filter::Zstd, "tar-zstd.tar.zst"));
  dbg(std::filesystem::file_size("tar-zstd.tar.zst"));
  REQUIRE_NOTHROW(klib::decompress("tar-zstd.tar.zst", "tar-zstd"));
  REQUIRE_NOTHROW(klib::exec("diff -r zlib-ng-2.0.6 tar-zstd/zlib-ng-2.0.6"));

  std::remove("tar-zstd.tar.zst");
  std::filesystem::remove_all("tar-zstd");
}

TEST_CASE("outermost_folder_name", "[archive]") {
  REQUIRE(std::filesystem::exists("zlib-ng-2.0.6.tar.gz"));
  CHECK(*klib::outermost_folder_name("zlib-ng-2.0.6.tar.gz") ==
        "zlib-ng-2.0.6");
}

TEST_CASE("compress data", "[archive]") {
  std::string data = "Hello World!!";
  std::string compressed, decompressed;

  REQUIRE_NOTHROW(compressed = klib::compress_data(data));
  REQUIRE_NOTHROW(decompressed = klib::decompress_data(compressed));
  REQUIRE(data == decompressed);
}
