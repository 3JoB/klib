#include <catch2/catch.hpp>

#include "klib/http_parse.h"

TEST_CASE("url", "[http_parse]") {
  klib::URL url(
      R"(https://www.example.com:80/path/to/myfile.html?key1=value1&key2=value2#SomewhereInTheDocument)");

  CHECK(url.schema() == "https");
  CHECK(url.host() == "www.example.com");
  CHECK(url.port() == 80);
  CHECK(url.path() == "/path/to/myfile.html");
  CHECK(url.query() == "key1=value1&key2=value2");
  CHECK(url.fragment() == "SomewhereInTheDocument");
  CHECK(std::empty(url.user_info()));

  const auto map = url.query_map();
  CHECK(map.at("key1") == "value1");
  CHECK(map.at("key2") == "value2");
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

  const auto uri = header.uri();
  klib::URL url(uri);
  CHECK(std::empty(url.schema()));
  CHECK(std::empty(url.host()));
  CHECK(url.port() == 0);
  CHECK(url.path() == "/path/to/myfile.html");
  CHECK(url.query() == "key1=value1&key2=value2");
  CHECK(url.fragment() == "SomewhereInTheDocument");
  CHECK(std::empty(url.user_info()));

  const auto &map = url.query_map();
  CHECK(map.at("key1") == "value1");
  CHECK(map.at("key2") == "value2");

  CHECK(header.http_major() == 1);
  CHECK(header.http_minor() == 1);
  CHECK(header.method() == klib::HttpMethod::HTTP_METHOD_POST);
  CHECK(klib::http_method_str(header.method()) == "POST");
  CHECK(header.body() == "Hello World!!");

  CHECK(header.value("host") == "www.example.com");
  CHECK(header.value("user-agent") ==
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) "
        "Gecko/20100101 Firefox/50.0");
  CHECK(header.value("content-Length") == "128");
  CHECK(header.value("Accept-Encoding") == "gzip, deflate, br");

  CHECK(header.contains("CONTENT-Length"));
  CHECK_FALSE(header.contains("abc"));
}

TEST_CASE("header2", "[http_parse]") {
  auto request_header = "GET /index.html HTTP/1.0\r\n\r\n";

  klib::HTTPHeader header(request_header);

  const auto &uri = header.uri();
  klib::URL url(uri);
  CHECK(url.path() == "/index.html");
}
