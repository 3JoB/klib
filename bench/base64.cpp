#include <string>

#include <catch2/catch.hpp>

#include "klib/base64.h"
#include "klib/util.h"

TEST_CASE("base64", "[base64]") {
  std::string secure_encoded;
  std::string secure_decoded;
  std::string fast_encoded;
  std::string fast_decoded;

  SECTION("64 byte") {
    const auto data = klib::generate_random_bytes(64);

    BENCHMARK("secure encoded") {
      secure_encoded = klib::secure_base64_encode(data);
    };
    BENCHMARK("fast encoded") {
      fast_encoded = klib::fast_base64_encode(data);
    };

    BENCHMARK("secure decoded") {
      secure_decoded = klib::secure_base64_decode(secure_encoded);
    };
    REQUIRE(data == secure_decoded);

    BENCHMARK("fast decoded") {
      fast_decoded = klib::fast_base64_decode(fast_encoded);
    };
    REQUIRE(data == fast_decoded);
  }

  SECTION("1 kb") {
    const auto data = klib::generate_random_bytes(1024);

    BENCHMARK("secure encoded") {
      secure_encoded = klib::secure_base64_encode(data);
    };
    BENCHMARK("fast encoded") {
      fast_encoded = klib::fast_base64_encode(data);
    };

    BENCHMARK("secure decoded") {
      secure_decoded = klib::secure_base64_decode(secure_encoded);
    };
    REQUIRE(data == secure_decoded);

    BENCHMARK("fast decoded") {
      fast_decoded = klib::fast_base64_decode(fast_encoded);
    };
    REQUIRE(data == fast_decoded);
  }

  SECTION("10 kb") {
    const auto data = klib::generate_random_bytes(10240);

    BENCHMARK("secure encoded") {
      secure_encoded = klib::secure_base64_encode(data);
    };
    BENCHMARK("fast encoded") {
      fast_encoded = klib::fast_base64_encode(data);
    };

    BENCHMARK("secure decoded") {
      secure_decoded = klib::secure_base64_decode(secure_encoded);
    };
    REQUIRE(data == secure_decoded);

    BENCHMARK("fast decoded") {
      fast_decoded = klib::fast_base64_decode(fast_encoded);
    };
    REQUIRE(data == fast_decoded);
  }

  SECTION("100 kb") {
    const auto data = klib::generate_random_bytes(102400);

    BENCHMARK("secure encoded") {
      secure_encoded = klib::secure_base64_encode(data);
    };
    BENCHMARK("fast encoded") {
      fast_encoded = klib::fast_base64_encode(data);
    };

    BENCHMARK("secure decoded") {
      secure_decoded = klib::secure_base64_decode(secure_encoded);
    };
    REQUIRE(data == secure_decoded);

    BENCHMARK("fast decoded") {
      fast_decoded = klib::fast_base64_decode(fast_encoded);
    };
    REQUIRE(data == fast_decoded);
  }

  SECTION("10 mb") {
    const auto data = klib::generate_random_bytes(10240000);

    BENCHMARK("secure encoded") {
      secure_encoded = klib::secure_base64_encode(data);
    };
    BENCHMARK("fast encoded") {
      fast_encoded = klib::fast_base64_encode(data);
    };

    BENCHMARK("secure decoded") {
      secure_decoded = klib::secure_base64_decode(secure_encoded);
    };
    REQUIRE(data == secure_decoded);

    BENCHMARK("fast decoded") {
      fast_decoded = klib::fast_base64_decode(fast_encoded);
    };
    REQUIRE(data == fast_decoded);
  }
}
