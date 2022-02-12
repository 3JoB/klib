#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/crypto.h"
#include "klib/hash.h"
#include "klib/util.h"

TEST_CASE("AES-CBC-256", "[crypto]") {
  const std::string file_name = "book.tar.gz";
  REQUIRE(std::filesystem::exists(file_name));

  const auto data = klib::read_file(file_name, true);
  const auto key = klib::sha256("kaiser123");
  const auto encrypted = klib::aes_256_encrypt(data, key);

  BENCHMARK("klib encrypt") { return klib::aes_256_encrypt(data, key); };
  BENCHMARK("klib decrypt") { return klib::aes_256_decrypt(encrypted, key); };
}
