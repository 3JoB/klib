#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/hash_lib.h"
#include "klib/util.h"

TEST_CASE("md5", "[hash_lib]") {
  klib::HashLib md5(klib::HashLib::Algorithm::MD5);

  REQUIRE(md5.update("MD5 online hash function").hex_digest() ==
          "71f6cb39c6d09c6fae36b69ee0b2b9cd");
  REQUIRE(md5.update("MD5 online hash function").hex_digest() ==
          "71f6cb39c6d09c6fae36b69ee0b2b9cd");
  REQUIRE(md5.update("MD5 online hash function").hex_digest() ==
          "71f6cb39c6d09c6fae36b69ee0b2b9cd");
  REQUIRE(md5.update("MD5 online hash function").hex_digest() ==
          "71f6cb39c6d09c6fae36b69ee0b2b9cd");

  REQUIRE(klib::md5_hex("SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2") ==
          "ee60cfe37f9a60b9ceba008be6f1c034");
}

TEST_CASE("sha_256", "[hash_lib]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::sha_256_hex(klib::read_file("zlib-v1.2.11.tar.gz", true)) ==
          "143df9ab483578ce7a1019b96aaa10f6e1ebc64b1a3d97fa14f4b4e4e7ec95e7");
}

TEST_CASE("sha3_512", "[hash_lib]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::sha3_512_hex(klib::read_file("zlib-v1.2.11.tar.gz", true)) ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");
}
