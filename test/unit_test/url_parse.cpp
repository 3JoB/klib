#include <catch2/catch.hpp>

#include "klib/url_parse.h"

TEST_CASE("url", "[url_parse]") {
  klib::URL url(
      R"(https://user:password@www.example.com:80/path/to/myfile.html?key1=value1&key2=value2#SomewhereInTheDocument)");

  CHECK(url.schema() == "https");
  CHECK(url.host() == "www.example.com");
  CHECK(url.port() == 80);
  CHECK(url.path() == "/path/to/myfile.html");
  CHECK(url.query() == "key1=value1&key2=value2");
  CHECK(url.fragment() == "SomewhereInTheDocument");
  CHECK(url.user_info() == "user:password");

  const auto map = url.query_map();
  CHECK(map.at("key1") == "value1");
  CHECK(map.at("key2") == "value2");
}
