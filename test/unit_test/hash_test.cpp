#include <filesystem>
#include <string_view>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("fast_hash", "[hash]") {
  std::string str = "Hello World";
  REQUIRE(klib::fast_hash(str) == 16376800918595593011UL);
  REQUIRE(klib::fast_hash_hex(str) == "e34615aade2e6333");
}

TEST_CASE("md5", "[hash]") {
  klib::SecureHash md5(klib::SecureHash::Algorithm::MD5);

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

TEST_CASE("sha_256", "[hash]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::sha_256_hex(klib::read_file("zlib-v1.2.11.tar.gz", true)) ==
          "143df9ab483578ce7a1019b96aaa10f6e1ebc64b1a3d97fa14f4b4e4e7ec95e7");
}

TEST_CASE("sha3_512", "[hash]") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));

  REQUIRE(klib::sha3_512_hex(klib::read_file("zlib-v1.2.11.tar.gz", true)) ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");
}

TEST_CASE("password_hash_raw", "[hash]") {
  std::string password = "test-password";
  std::string hash, salt;
  REQUIRE_NOTHROW(std::tie(hash, salt) = klib::password_hash_raw(password));
  REQUIRE(klib::password_verify(password, hash, salt));
}

TEST_CASE("password_hash_encoded", "[hash]") {
  std::string password = "test-password";
  std::string encoded;

  REQUIRE_NOTHROW(encoded = klib::password_hash_encoded(password));
  dbg(encoded);
  REQUIRE(klib::password_verify(password, encoded));
}
