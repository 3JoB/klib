#include <string>

#include <catch2/catch.hpp>

#include "klib/base64.h"
#include "klib/util.h"

TEST_CASE("base64", "[base64]") {
  std::string encoded;
  std::string decoded;

  SECTION("64 byte") {
    auto data = klib::generate_random_bytes(64);
    BENCHMARK("encoded") { encoded = klib::secure_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::secure_base64_decode(encoded); };
    REQUIRE(data == decoded);

    BENCHMARK("encoded") { encoded = klib::fast_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::fast_base64_decode(encoded); };
    REQUIRE(data == decoded);
  }

  SECTION("1 kb") {
    auto data = klib::generate_random_bytes(1024);
    BENCHMARK("encoded") { encoded = klib::secure_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::secure_base64_decode(encoded); };
    REQUIRE(data == decoded);

    BENCHMARK("encoded") { encoded = klib::fast_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::fast_base64_decode(encoded); };
    REQUIRE(data == decoded);
  }

  SECTION("10 kb") {
    auto data = klib::generate_random_bytes(10240);
    BENCHMARK("encoded") { encoded = klib::secure_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::secure_base64_decode(encoded); };
    REQUIRE(data == decoded);

    BENCHMARK("encoded") { encoded = klib::fast_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::fast_base64_decode(encoded); };
    REQUIRE(data == decoded);
  }

  SECTION("100 kb") {
    auto data = klib::generate_random_bytes(102400);
    BENCHMARK("encoded") { encoded = klib::secure_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::secure_base64_decode(encoded); };
    REQUIRE(data == decoded);

    BENCHMARK("encoded") { encoded = klib::fast_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::fast_base64_decode(encoded); };
    REQUIRE(data == decoded);
  }

  SECTION("10 mb") {
    auto data = klib::generate_random_bytes(10240000);
    BENCHMARK("encoded") { encoded = klib::secure_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::secure_base64_decode(encoded); };
    REQUIRE(data == decoded);

    BENCHMARK("encoded") { encoded = klib::fast_base64_encode(data); };
    BENCHMARK("decoded") { decoded = klib::fast_base64_decode(encoded); };
    REQUIRE(data == decoded);
  }
}
