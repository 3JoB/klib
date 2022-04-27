#include <catch2/catch.hpp>

#include "klib/http.h"
#include "klib/util.h"

TEST_CASE("HTTP GET", "[http]") {
  BENCHMARK("curl GET") {
    klib::exec(
        "curl -fsSL -o /dev/null --noproxy '*' "
        "https://cloudflare-quic.com/");
  };
  BENCHMARK("klib GET") {
    klib::Request request;
    request.set_no_proxy();

    auto response = request.get("https://cloudflare-quic.com/");
    REQUIRE(response.ok());
  };
}
