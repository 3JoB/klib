#include <catch2/catch.hpp>

#include "klib/exception.h"
#include "klib/html.h"

TEST_CASE("html_tidy") {
  REQUIRE(klib::html::html_tidy("<title>Foo</title><p>Foo!") ==
          R"(<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>Foo</title>
</head>
<body>
<p>Foo!</p>
</body>
</html>
)");

  REQUIRE_THROWS_AS(klib::html::html_tidy("<titleFoo</title><p>Foo!"),
                    klib::RuntimeError);
}
