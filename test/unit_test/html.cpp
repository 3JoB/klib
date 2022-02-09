#include <catch2/catch.hpp>

#include "klib/html.h"

TEST_CASE("html_tidy", "[html]") {
  CHECK(klib::html_tidy("<title>Foo</title><p>Foo!") ==
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
}
