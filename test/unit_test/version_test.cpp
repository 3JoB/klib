#include <catch2/catch.hpp>

#include "klib/version.h"

TEST_CASE("klib_version") { REQUIRE(klib::klib_version() == "v0.1.5"); }
