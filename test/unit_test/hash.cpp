#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("fast_hash", "[hash]") {
  std::string str = "Hello World";
  CHECK(klib::fast_hash(str) == 16376800918595593011UL);
  CHECK(klib::fast_hash(str) == 16376800918595593011UL);
  CHECK(klib::fast_hash_hex(str) == "e34615aade2e6333");
}

TEST_CASE("md5", "[hash]") {
  CHECK(klib::md5_hex("SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2") ==
        "ee60cfe37f9a60b9ceba008be6f1c034");
}

TEST_CASE("sha256", "[hash]") {
  CHECK(std::filesystem::exists("zlib-ng-2.0.6.tar.gz"));

  CHECK(klib::sha256_hex(klib::read_file("zlib-ng-2.0.6.tar.gz", true)) ==
        "8258b75a72303b661a238047cb348203d88d9dddf85d480ed885f375916fcab6");
}

TEST_CASE("password_hash_raw", "[hash]") {
  std::string password = "test-password";
  std::string hash, salt;
  CHECK_NOTHROW(std::tie(hash, salt) = klib::password_hash_raw(password));
  CHECK(klib::password_verify(password, hash, salt));
}

TEST_CASE("password_hash_encoded", "[hash]") {
  std::string password = "test-password";
  std::string encoded;

  CHECK_NOTHROW(encoded = klib::password_hash_encoded(password));
  dbg(encoded);
  CHECK(klib::password_verify(password, encoded));
}
