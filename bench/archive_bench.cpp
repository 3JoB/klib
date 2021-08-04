#include <filesystem>

#include <catch2/catch.hpp>

#include "klib/archive.h"
#include "klib/util.h"

TEST_CASE("compress") {
  REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
  REQUIRE(klib::util::folder_size("madler-zlib-7085a61") == 2984209);

  BENCHMARK_ADVANCED("tar compress")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::util::execute_command("tar -zcf zlib.tar.gz madler-zlib-7085a61");
    });

    REQUIRE(std::filesystem::is_regular_file("zlib.tar.gz"));
    std::filesystem::remove("zlib.tar.gz");
  };

  BENCHMARK_ADVANCED("klib compress")(Catch::Benchmark::Chronometer meter) {
    meter.measure([] {
      klib::archive::compress("madler-zlib-7085a61",
                              klib::archive::Algorithm::Gzip,
                              "madler-zlib-7085a61.tar.gz");
    });

    REQUIRE(std::filesystem::is_regular_file("madler-zlib-7085a61.tar.gz"));
    std::filesystem::remove("madler-zlib-7085a61.tar.gz");
  };
}

TEST_CASE("decompress") {
  REQUIRE(std::filesystem::exists("zlib-v1.2.11.tar.gz"));
  REQUIRE(klib::util::sha3_512("zlib-v1.2.11.tar.gz") ==
          "38af19362e48ec80f6565cf18245f520c8ee5348374cb0c11286f3b23cc93fd05a6a"
          "2a2b8784f20bb2307211a2a776241797857b133056f4b33de1d363db7bb2");

  BENCHMARK_ADVANCED("tar decompress")(Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::util::execute_command("tar -zxf zlib-v1.2.11.tar.gz"); });

    REQUIRE(std::filesystem::is_directory("madler-zlib-7085a61"));
    REQUIRE(klib::util::folder_size("madler-zlib-7085a61") == 2984209);
  };

  BENCHMARK_ADVANCED("klib decompress")(Catch::Benchmark::Chronometer meter) {
    meter.measure(
        [] { klib::archive::decompress("zlib-v1.2.11.tar.gz", "gzip"); });

    REQUIRE(std::filesystem::is_directory("gzip/madler-zlib-7085a61"));
    REQUIRE(klib::util::folder_size("gzip/madler-zlib-7085a61") == 2984209);
    std::filesystem::remove_all("gzip");
  };
}
