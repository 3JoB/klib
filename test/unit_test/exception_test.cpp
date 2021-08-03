#include <string_view>

#include <catch2/catch.hpp>

#include "klib/exception.h"

namespace {

void func() {
  std::string_view msg = "a runtime error";
  throw klib::exception::RuntimeError(msg);
}

}  // namespace

TEST_CASE("RuntimeError") {
  REQUIRE_THROWS_MATCHES(func(), klib::exception::RuntimeError,
                         Catch::Message("a runtime error"));
}
