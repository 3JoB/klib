#include <filesystem>
#include <string>
#include <vector>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("zip", "[archive]") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);

  REQUIRE_NOTHROW(klib::compress("madler-zlib-7085a61", klib::Algorithm::Zip,
                                 "zlib.zip", true, 9));
  REQUIRE(std::filesystem::is_regular_file("zlib.zip"));
  dbg(std::filesystem::file_size("zlib.zip"));

  REQUIRE(klib::decompress("zlib.zip", "zip") == "madler-zlib-7085a61");
  REQUIRE(std::filesystem::is_directory("zip/madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("zip/madler-zlib-7085a61") == 2984209);
  REQUIRE(klib::same_folder("madler-zlib-7085a61", "zip/madler-zlib-7085a61"));

  std::filesystem::remove("zlib.zip");
  std::filesystem::remove_all("zip");

  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);

  REQUIRE_NOTHROW(
      klib::compress("madler-zlib-7085a61", klib::Algorithm::Zip, "", false));
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61.zip"));

  REQUIRE(!klib::decompress("madler-zlib-7085a61.zip", "flag"));
  REQUIRE(std::filesystem::is_directory("flag"));
  REQUIRE(klib::folder_size("flag") == 2984209);
  REQUIRE(klib::same_folder("madler-zlib-7085a61", "flag"));

  std::vector<std::string> paths = {"madler-zlib-7085a61",
                                    "madler-zlib-7085a61.zip"};
  REQUIRE_NOTHROW(klib::compress(paths, klib::Algorithm::Zip, "zlib.zip"));

  REQUIRE(std::filesystem::exists("zlib.zip"));
  REQUIRE(!klib::decompress("zlib.zip", "files"));

  REQUIRE(std::filesystem::is_directory("files"));

  REQUIRE(std::filesystem::is_directory("files/madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("files/madler-zlib-7085a61") == 2984209);
  REQUIRE(
      klib::same_folder("madler-zlib-7085a61", "files/madler-zlib-7085a61"));

  REQUIRE(std::filesystem::is_regular_file("files/madler-zlib-7085a61.zip"));
  REQUIRE(std::filesystem::file_size("files/madler-zlib-7085a61.zip") ==
          790060);

  std::filesystem::remove("madler-zlib-7085a61.zip");
  std::filesystem::remove_all("flag");
  std::filesystem::remove("zlib.zip");
  std::filesystem::remove_all("files");
}

TEST_CASE("gzip", "[archive]") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);

  REQUIRE_NOTHROW(klib::compress("madler-zlib-7085a61", klib::Algorithm::Gzip,
                                 "zlib.tar.gz", true, 9));
  REQUIRE(std::filesystem::is_regular_file("zlib.tar.gz"));
  dbg(std::filesystem::file_size("zlib.tar.gz"));

  REQUIRE(klib::decompress("zlib.tar.gz", "gzip") == "madler-zlib-7085a61");
  REQUIRE(std::filesystem::is_directory("gzip/madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("gzip/madler-zlib-7085a61") == 2984209);
  REQUIRE(klib::same_folder("madler-zlib-7085a61", "gzip/madler-zlib-7085a61"));

  std::filesystem::remove("zlib.tar.gz");
  std::filesystem::remove_all("gzip");

  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(klib::sha3_512_hex(klib::read_file("zlib-v1.2.11.tar.gz", true)) ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");

  REQUIRE(klib::decompress("zlib-v1.2.11.tar.gz", "std-gzip") ==
          "madler-zlib-7085a61");

  REQUIRE(std::filesystem::is_directory("std-gzip/madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("std-gzip/madler-zlib-7085a61") == 2984209);

  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);
  REQUIRE(
      klib::same_folder("madler-zlib-7085a61", "std-gzip/madler-zlib-7085a61"));

  std::filesystem::remove_all("std-gzip");
}

TEST_CASE("gzstd", "[archive]") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("madler-zlib-7085a61") == 2984209);

  REQUIRE_NOTHROW(klib::compress("madler-zlib-7085a61", klib::Algorithm::Zstd,
                                 "zlib.tar.zst", true, 18));
  REQUIRE(std::filesystem::is_regular_file("zlib.tar.zst"));
  dbg(std::filesystem::file_size("zlib.tar.zst"));

  REQUIRE(klib::decompress("zlib.tar.zst", "zstd") == "madler-zlib-7085a61");
  REQUIRE(std::filesystem::is_directory("zstd/madler-zlib-7085a61"));
  REQUIRE(klib::folder_size("zstd/madler-zlib-7085a61") == 2984209);
  REQUIRE(klib::same_folder("madler-zlib-7085a61", "zstd/madler-zlib-7085a61"));

  std::filesystem::remove("zlib.tar.zst");
  std::filesystem::remove_all("zstd");
}

TEST_CASE("gzip2", "[archive]") {
  REQUIRE(std::filesystem::exists("bzip2-v1.0.8.tar.gz"));

  auto dir = klib::decompress("bzip2-v1.0.8.tar.gz");
  REQUIRE(dir.has_value());
  REQUIRE(*dir == "bzip2-1.0.8");

  std::filesystem::remove_all("bzip2-1.0.8");
}

TEST_CASE("zstd", "[archive]") {
  std::string data = "test string";
  std::string compressed, decompressed;

  REQUIRE_NOTHROW(compressed = klib::compress_str(data, 19));
  REQUIRE_NOTHROW(decompressed = klib::decompress_str(compressed));
  REQUIRE(data == decompressed);
}

TEST_CASE("password", "[archive]") {
  REQUIRE(std::filesystem::exists("password.zip"));
  REQUIRE(klib::decompress("password.zip", "password", "123456") ==
          "madler-zlib-7085a61");
  REQUIRE(klib::folder_size("password/madler-zlib-7085a61") == 2984209);

  std::filesystem::remove_all("password");
}
