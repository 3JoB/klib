#include <catch2/catch.hpp>

#include "klib/base64.h"

TEST_CASE("base64_encode", "[base64]") {
  REQUIRE(klib::fast_base64_encode("hello") == "aGVsbG8=");
  REQUIRE(klib::fast_base64_encode("Online Tools") == "T25saW5lIFRvb2xz");
  REQUIRE(
      klib::fast_base64_encode(
          "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"") ==
      "SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2V4OiBiYWQgZGVjcnlwdC"
      "I=");
}

TEST_CASE("base64_decode", "[base64]") {
  REQUIRE(klib::fast_base64_decode("aGVsbG8=") == "hello");
  REQUIRE(klib::fast_base64_decode("T25saW5lIFRvb2xz") == "Online Tools");
  REQUIRE(klib::fast_base64_decode(
              "SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2"
              "V4OiBiYWQgZGVjcnlwdCI=") ==
          "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"");
}
