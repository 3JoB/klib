#include <catch2/catch.hpp>

#include "klib/http_parse.h"

TEST_CASE("url", "[http_parse]") {
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

TEST_CASE("header", "[http_parse]") {
  // clang-format off
  auto request_header =
      "POST /path/to/myfile.html?key1=value1&key2=value2#SomewhereInTheDocument HTTP/1.1\r\n"
      "Host: www.example.com\r\n"
      "Accept-Encoding: gzip\r\n"
      "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\r\n"
      "Accept-Encoding: deflate, br\r\n"
      "Content-Length: 128\r\n\r\n"
      "Hello World!!";
  // clang-format on

  klib::HTTPHeader header(request_header);

  const auto &url = header.url();
  REQUIRE(std::empty(url.schema()));
  REQUIRE(std::empty(url.host()));
  REQUIRE(url.port() == 0);
  REQUIRE(url.path() == "/path/to/myfile.html");
  REQUIRE(url.query() == "key1=value1&key2=value2");
  REQUIRE(url.fragment() == "SomewhereInTheDocument");
  REQUIRE(std::empty(url.user_info()));

  const auto &map = url.query_map();
  REQUIRE(map.at("key1") == "value1");
  REQUIRE(map.at("key2") == "value2");

  REQUIRE(header.http_major() == 1);
  REQUIRE(header.http_minor() == 1);
  REQUIRE(header.body() == "Hello World!!");

  REQUIRE(header.value("host") == "www.example.com");
  REQUIRE(header.value("user-agent") ==
          "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) "
          "Gecko/20100101 Firefox/50.0");
  REQUIRE(header.value("content-Length") == "128");
  REQUIRE(header.value("Accept-Encoding") == "gzip, deflate, br");
}
