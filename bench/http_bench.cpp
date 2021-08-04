#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/http.h"
#include "klib/util.h"

TEST_CASE("download file") {
  BENCHMARK_ADVANCED("klib http")(Catch::Benchmark::Chronometer meter) {
    klib::http::Request request;
    request.verbose(true);

#ifdef KLIB_TEST_USE_PROXY
    request.set_proxy();
#endif

    meter.measure([&request] {
      auto response = request.get(
          "https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip");
      REQUIRE(response.status_code() == klib::http::Response::StatusCode::Ok);
      response.save_to_file("8.0.1.zip", true);
    });

    REQUIRE(std::filesystem::exists("8.0.1.zip"));
    REQUIRE(std::filesystem::file_size("8.0.1.zip") == 871270);
    REQUIRE(
        klib::util::sha3_512("8.0.1.zip") ==
        "18f064baad48aae210673ade98e86e54f7a721d8510eb1d7c74efd36561d3528c5fb"
        "2f7f0c4279edb7963fde41b08244edece4bbc8a6d6cdaa93344b174feaa3");
    std::filesystem::remove("8.0.1.zip");
  };

  BENCHMARK_ADVANCED("curl")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
#ifdef KLIB_TEST_USE_PROXY
      klib::util::execute_command(
          "curl -v -x socks5://127.0.0.1:1080 -L "
          "https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip -o "
          "fmt.zip");
#else
      klib::util::execute_command(
          "curl -v -L "
          "https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.zip "
          "-o fmt.zip");
#endif
    });

    REQUIRE(std::filesystem::exists("fmt.zip"));
    REQUIRE(std::filesystem::file_size("fmt.zip") == 871270);
    REQUIRE(
        klib::util::sha3_512("fmt.zip") ==
        "18f064baad48aae210673ade98e86e54f7a721d8510eb1d7c74efd36561d3528c5fb"
        "2f7f0c4279edb7963fde41b08244edece4bbc8a6d6cdaa93344b174feaa3");
    std::filesystem::remove("fmt.zip");
  };
}
