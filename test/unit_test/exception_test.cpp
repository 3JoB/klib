#include <string_view>

#include <catch2/catch.hpp>

#include "klib/exception.h"

namespace {

void func() {
  std::string_view msg = "a runtime error";
  throw klib::RuntimeError(msg);
}

}  // namespace

TEST_CASE("RuntimeError", "[exception]") {
  REQUIRE_THROWS_MATCHES(func(), klib::RuntimeError,
                         Catch::Message("a runtime error"));
}
