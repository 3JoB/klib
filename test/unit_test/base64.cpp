#include <catch2/catch_test_macros.hpp>

#include "klib/base64.h"

TEST_CASE("fast base64", "[base64]") {
  CHECK(klib::fast_base64_encode("hello") == "aGVsbG8=");
  CHECK(klib::fast_base64_decode("aGVsbG8=") == "hello");

  CHECK(klib::fast_base64_encode("Online Tools") == "T25saW5lIFRvb2xz");
  CHECK(klib::fast_base64_decode("T25saW5lIFRvb2xz") == "Online Tools");

  CHECK(klib::fast_base64_encode(
            "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"") ==
        "SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2V4OiBiYWQgZGVjcnlwdC"
        "I=");
  CHECK(klib::fast_base64_decode(
            "SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2"
            "V4OiBiYWQgZGVjcnlwdCI=") ==
        "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"");
}

TEST_CASE("secure base64 ", "[base64]") {
  CHECK(klib::secure_base64_encode("hello") == "aGVsbG8=");
  CHECK(klib::secure_base64_decode("aGVsbG8=") == "hello");

  CHECK(klib::secure_base64_encode("Online Tools") == "T25saW5lIFRvb2xz");
  CHECK(klib::secure_base64_decode("T25saW5lIFRvb2xz") == "Online Tools");

  CHECK(klib::secure_base64_encode(
            "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"") ==
        "SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2V4OiBiYWQgZGVjcnlwdC"
        "I=");
  CHECK(klib::secure_base64_decode(
            "SG93IHRvIHJlc29sdmUgdGhlICJFVlBfRGVjcnlwdEZJbmFsX2"
            "V4OiBiYWQgZGVjcnlwdCI=") ==
        "How to resolve the \"EVP_DecryptFInal_ex: bad decrypt\"");
}
