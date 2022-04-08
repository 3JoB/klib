#include <catch2/catch.hpp>

#include "klib/http.h"
#include "klib/util.h"

TEST_CASE("HTTP GET", "[http]") {
  BENCHMARK("curl HTTP/2 GET") {
    klib::exec(
        "curl -fsSL -o /dev/null --noproxy '*' "
        "https://www.litespeedtech.com/");
  };
  BENCHMARK("klib HTTP/2 GET") {
    klib::Request request;
    request.set_no_proxy();

    auto response = request.get("https://www.litespeedtech.com/");
    REQUIRE(response.ok());
  };

  BENCHMARK("curl HTTP/3 GET") {
    klib::exec(
        "curl --http3 -fsSL -o /dev/null --noproxy '*' "
        "https://www.litespeedtech.com/");
  };
  BENCHMARK("klib HTTP/3 GET") {
    klib::Request request;
    request.set_no_proxy();
    request.http_version(klib::Request::HTTP3);

    auto response = request.get("https://www.litespeedtech.com/");
    REQUIRE(response.ok());
  };
}
