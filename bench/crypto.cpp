#include <string>

#include <boost/core/ignore_unused.hpp>
#include <catch2/catch.hpp>

#include "klib/crypto.h"
#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("Base64", "[crypto]") {
  std::string encoded;
  std::string decoded;

  SECTION("1 kb") {
    auto data = klib::generate_random_bytes(1024);
    BENCHMARK("encoded") { encoded = klib::base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::base64_decode(encoded); };
    REQUIRE(data == decoded);
  }

  SECTION("10 kb") {
    auto data = klib::generate_random_bytes(10240);
    BENCHMARK("encoded") { encoded = klib::base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::base64_decode(encoded); };
    REQUIRE(data == decoded);
  }

  SECTION("100 kb") {
    auto data = klib::generate_random_bytes(102400);
    BENCHMARK("encoded") { encoded = klib::base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::base64_decode(encoded); };
    REQUIRE(data == decoded);
  }
}

TEST_CASE("AES", "[crypto]") {
  std::string key;
  std::string salt;
  std::tie(key, salt) = klib::password_hash_raw("kaiser123");
  boost::ignore_unused(salt);

  std::string encrypted;
  std::string decrypted;

  SECTION("1 kb") {
    auto data = klib::generate_random_bytes(1024);
    BENCHMARK("encrypt") { encrypted = klib::aes_256_encrypt(data, key); };
    BENCHMARK("decrypt") { decrypted = klib::aes_256_decrypt(encrypted, key); };
    REQUIRE(data == decrypted);
  }

  SECTION("10 kb") {
    auto data = klib::generate_random_bytes(10240);
    BENCHMARK("encrypt") { encrypted = klib::aes_256_encrypt(data, key); };
    BENCHMARK("decrypt") { decrypted = klib::aes_256_decrypt(encrypted, key); };
    REQUIRE(data == decrypted);
  }

  SECTION("100 kb") {
    auto data = klib::generate_random_bytes(102400);
    BENCHMARK("encrypt") { encrypted = klib::aes_256_encrypt(data, key); };
    BENCHMARK("decrypt") { decrypted = klib::aes_256_decrypt(encrypted, key); };
    REQUIRE(data == decrypted);
  }
}
