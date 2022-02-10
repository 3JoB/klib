#include <catch2/catch.hpp>

#include "klib/http.h"
#include "klib/util.h"

TEST_CASE("GET", "[http]") {
  klib::Request request;
  request.set_no_proxy();

  BENCHMARK("std HTTP GET") {
    klib::exec(
        "/usr/bin/curl -s -o /dev/null --noproxy '*' https://www.baidu.com/");
  };
  BENCHMARK("klib HTTP GET") {
    auto response = request.get("https://www.baidu.com/");
    REQUIRE(response.ok());
  };
}
