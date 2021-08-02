#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

TEST_CASE("Compress and decompress using the zip algorithm") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("madler-zlib-7085a61") == 2984209);

  klib::archive::compress("madler-zlib-7085a61", klib::archive::Algorithm::Zip,
                          "zlib.zip");
  REQUIRE(klib::archive::decompress("zlib.zip", "zip") ==
          "madler-zlib-7085a61");

  REQUIRE(std::filesystem::is_directory("zip/madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("zip/madler-zlib-7085a61") == 2984209);
  REQUIRE(klib::util::same_folder("madler-zlib-7085a61",
                                  "zip/madler-zlib-7085a61"));

  std::filesystem::remove("zlib.zip");
  std::filesystem::remove_all("zip");
}

TEST_CASE("Compress and decompress using the gzip algorithm") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("madler-zlib-7085a61") == 2984209);

  klib::archive::compress("madler-zlib-7085a61", klib::archive::Algorithm::Gzip,
                          "zlib.tar.gz");
  REQUIRE(klib::archive::decompress("zlib.tar.gz", "gzip") ==
          "madler-zlib-7085a61");

  REQUIRE(std::filesystem::is_directory("gzip/madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("gzip/madler-zlib-7085a61") == 2984209);
  REQUIRE(klib::util::same_folder("madler-zlib-7085a61",
                                  "gzip/madler-zlib-7085a61"));

  std::filesystem::remove("zlib.tar.gz");
  std::filesystem::remove_all("gzip");
}

TEST_CASE("Decompress using the gzip algorithm, compressed file from github") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(klib::util::sha3_512("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");

  REQUIRE(klib::archive::decompress("zlib-v1.2.11.tar.gz", "std-gzip") ==
          "madler-zlib-7085a61");

  REQUIRE(std::filesystem::is_directory("std-gzip/madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("std-gzip/madler-zlib-7085a61") == 2984209);

  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("madler-zlib-7085a61") == 2984209);
  REQUIRE(klib::util::same_folder("madler-zlib-7085a61",
                                  "std-gzip/madler-zlib-7085a61"));

  std::filesystem::remove_all("std-gzip");
}

TEST_CASE(
    "Compress and decompress using the zip algorithm, excluding the outermost "
    "folder") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("madler-zlib-7085a61") == 2984209);

  klib::archive::compress("madler-zlib-7085a61", klib::archive::Algorithm::Zip,
                          "zlib.zip", false);
  REQUIRE(std::empty(klib::archive::decompress("zlib.zip", "flag")));

  REQUIRE(std::filesystem::is_directory("flag"));
  REQUIRE(klib::util::folder_size("flag") == 2984209);
  REQUIRE(klib::util::same_folder("madler-zlib-7085a61", "flag"));

  std::filesystem::remove("zlib.zip");
  std::filesystem::remove_all("flag");
}
