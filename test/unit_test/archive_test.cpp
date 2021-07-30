#include <cstddef>
#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

namespace {

std::size_t folder_size(const std::string& path) {
  std::size_t size = 0;
  for (const auto& item : std::filesystem::recursive_directory_iterator(path)) {
    if (std::filesystem::is_regular_file(item)) {
      size += std::filesystem::file_size(item);
    }
  }

  return size;
}

}  // namespace

TEST_CASE("compress zip") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(klib::sha3_512("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");

  klib::archive::decompress("zlib-v1.2.11.tar.gz");
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61"));

  klib::archive::compress("madler-zlib-7085a61", klib::archive::Algorithm::Zip,
                          true);
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61.zip"));

  klib::archive::decompress("madler-zlib-7085a61.zip");
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61"));
  REQUIRE(folder_size("madler-zlib-7085a61") == 2984209);
}

TEST_CASE("compress gzip") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(klib::sha3_512("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");

  klib::archive::decompress("zlib-v1.2.11.tar.gz");
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61"));

  klib::archive::compress("madler-zlib-7085a61", klib::archive::Algorithm::Gzip,
                          true);
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61.tar.gz"));

  klib::archive::decompress("madler-zlib-7085a61.tar.gz");
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61"));
  REQUIRE(folder_size("madler-zlib-7085a61") == 2984209);
}

TEST_CASE("decompress") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(klib::sha3_512("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");

  klib::archive::decompress("zlib-v1.2.11.tar.gz");
  REQUIRE(std::filesystem::exists("madler-zlib-7085a61"));
  REQUIRE(folder_size("madler-zlib-7085a61") == 2984209);
}

TEST_CASE("decompress error") {
  REQUIRE(std::filesystem::exists("error.tar.gz"));

  REQUIRE_THROWS_MATCHES(klib::archive::decompress("error.tar.gz"),
                         std::runtime_error,
                         Catch::Message("Damaged tar archive"));
}
