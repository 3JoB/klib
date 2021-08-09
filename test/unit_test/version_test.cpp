#include <catch2/catch.hpp>

#include "klib/version.h"

TEST_CASE("klib_version", "[version]") {
  REQUIRE(klib::klib_version() == "v0.2.2");
}
