#include <catch2/catch.hpp>

#include "klib/url.h"

TEST_CASE("url", "[url]") {
  klib::URL url(
      R"(https://www.example.com:80/path/to/myfile.html?key1=value1&key2=value2#SomewhereInTheDocument)");

  REQUIRE(url.schema() == "https");
  REQUIRE(url.host() == "www.example.com");
  REQUIRE(url.port() == 80);
  REQUIRE(url.path() == "/path/to/myfile.html");
  REQUIRE(url.query() == "key1=value1&key2=value2");
  REQUIRE(url.fragment() == "SomewhereInTheDocument");
  REQUIRE(std::empty(url.user_info()));

  const auto map = url.query_map();
  REQUIRE(map.at("key1") == "value1");
  REQUIRE(map.at("key2") == "value2");
}
