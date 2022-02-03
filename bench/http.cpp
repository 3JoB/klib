#include <catch2/catch.hpp>

#include "klib/http.h"
#include "klib/util.h"

TEST_CASE("get", "[http]") {
  klib::Request request;
  request.set_no_proxy();
  klib::Response response;

  BENCHMARK("get") {
    response = request.get("https://www.baidu.com/");
    REQUIRE(response.ok());
  };
  BENCHMARK("curl get") {
    klib::exec(
        "/usr/bin/curl -s -o /dev/null --noproxy '*' https://www.baidu.com/");
  };
}
