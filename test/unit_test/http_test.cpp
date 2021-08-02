#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/http.h"

TEST_CASE("download html") {
  klib::http::Request request;
  request.verbose(true);
  request.set_user_agent("curl/7.78.0");
  request.set_no_proxy();

  auto response = request.get("https://www.baidu.com");

  REQUIRE(response.status_code() == klib::http::Response::StatusCode::Ok);
  REQUIRE(!std::empty(response.header()));
  REQUIRE(!std::empty(response.text()));
}

TEST_CASE("download file") {
  klib::http::Request request;
  request.verbose(true);
  request.allow_redirects(true);

#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy("socks5://127.0.0.1:1080");
#endif

  auto response =
      request.get("https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip");
  REQUIRE(response.status_code() == klib::http::Response::StatusCode::Ok);

  response.save_to_file("8.0.1.zip");
  REQUIRE(std::filesystem::exists("8.0.1.zip"));
  REQUIRE(std::filesystem::file_size("8.0.1.zip") == 871270);

  std::filesystem::remove("8.0.1.zip");
}
