#include <filesystem>
#include <iostream>

#include <catch2/catch.hpp>

#include "klib/http.h"

TEST_CASE("download html") {
  klib::http::Request request;
  request.verbose(true);
  request.set_user_agent("curl/7.78.0");

  auto response = request.get("https://www.baidu.com");
  REQUIRE(response.status_code() == klib::http::Response::StatusCode::Ok);

  std::cout << response.header() << '\n';
  std::cout << response.text() << '\n';
}

TEST_CASE("download file") {
  klib::http::Request request;
  request.verbose(true);
  request.allow_redirects(true);
  request.set_proxy("socks5://127.0.0.1:1080");

  auto response =
      request.get("https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip");
  REQUIRE(response.status_code() == klib::http::Response::StatusCode::Ok);

  response.save("8.0.1.zip");
  REQUIRE(std::filesystem::exists("8.0.1.zip"));
  REQUIRE(std::filesystem::file_size("8.0.1.zip") == 871270);
}
