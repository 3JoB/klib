#include <string>

#include <catch2/catch_test_macros.hpp>

#include "klib/url.h"

TEST_CASE("URL", "[url]") {
  const std::string url_str =
      "https://user:password@www.example.com:80/path/to/"
      "myfile.html?key1=value1&key2=value2#SomewhereInTheDocument";

  klib::URL url(url_str);

  CHECK(url.schema() == "https");
  CHECK(url.host() == "www.example.com");
  CHECK(url.port() == 80);
  CHECK(url.path() == "/path/to/myfile.html");

  const auto &map = url.query();
  CHECK(map.at("key1") == "value1");
  CHECK(map.at("key2") == "value2");

  CHECK(url.fragment() == "SomewhereInTheDocument");
  CHECK(url.user() == "user");
  CHECK(url.password() == "password");

  CHECK(url.to_string() == url_str);
}

TEST_CASE("URL2", "[url]") {
  const std::string url_str =
      "https://masiro.me/images/encode/other-220322144937-1J7I.jpg?quality=60";

  klib::URL url(url_str);

  auto query = url.query();
  query.erase("quality");
  url.set_query(query);

  CHECK(url.to_string() ==
        "https://masiro.me/images/encode/other-220322144937-1J7I.jpg");
}

TEST_CASE("URL3", "[url]") {
  klib::URL url;

  url.set_host("www.example.com");
  url.set_path("/引き割り.html");
  url.set_query({{"a", "1"}, {"b", "你"}});

  CHECK(url.to_string() ==
        "https://www.example.com/"
        "%e5%bc%95%e3%81%8d%e5%89%b2%e3%82%8a.html?b=%e4%bd%a0&a=1");
}
