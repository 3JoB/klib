#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/http.h"
#include "klib/util.h"

TEST_CASE("download html", "[http]") {
  klib::Request request;
  request.verbose(true);
  request.set_browser_user_agent();
  request.set_timeout(10);

#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy("socks5://127.0.0.1:1080");
#endif

  auto response = request.get("https://api.github.com/repos/madler/zlib/tags");

  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);
  REQUIRE(!std::empty(response.header()));
  REQUIRE(!std::empty(response.text()));

  request.set_curl_user_agent();
  request.set_no_proxy();
  request.use_http_1_1();

  response = request.get("https://www.baidu.com");

  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);
  REQUIRE(!std::empty(response.header()));
  REQUIRE(!std::empty(response.text()));
}

TEST_CASE("download file", "[http]") {
  klib::Request request;
  request.verbose(true);
  request.allow_redirects(true);
  request.use_http_2();

#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy("socks5://127.0.0.1:1080");
#endif

  auto response =
      request.get("https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip");

  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);

  response.save_to_file("8.0.1.zip", true);

  REQUIRE(std::filesystem::exists("8.0.1.zip"));
  REQUIRE(std::filesystem::file_size("8.0.1.zip") == 871270);
  REQUIRE(klib::sha3_512_file("8.0.1.zip") ==
          "18f064baad48aae210673ade98e86e54f7a721d8510eb1d7c74efd36561d3528c5fb"
          "2f7f0c4279edb7963fde41b08244edece4bbc8a6d6cdaa93344b174feaa3");

  std::filesystem::remove("8.0.1.zip");
}

TEST_CASE("post", "[http]") {
  klib::Request request;
  request.verbose(true);
#ifdef KLIB_TEST_USE_PROXY
  request.set_proxy("socks5://127.0.0.1:1080");
#endif

  auto response = request.post("", {});

  REQUIRE(response.status_code() == klib::Response::StatusCode::Ok);
}
