#include <filesystem>
#include <string>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "klib/image.h"

TEST_CASE("Image", "[image]") {
  const std::string jpeg_file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(jpeg_file_name));

  const std::string png_file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(png_file_name));

  const std::string out_name = "out.image";

  BENCHMARK_ADVANCED("PNG to JPEG")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(out_name);
    meter.measure([&] { klib::image_to_jpeg(png_file_name, out_name); });
  };
  BENCHMARK_ADVANCED("PNG to JPEG max compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(out_name);
    meter.measure(
        [&] { klib::image_to_jpeg(png_file_name, out_name, 75, true); });
  };
  BENCHMARK_ADVANCED("PNG to WebP")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(out_name);
    meter.measure([&] { klib::image_to_webp(png_file_name, out_name); });
  };

  BENCHMARK_ADVANCED("JPEG to JPEG")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(out_name);
    meter.measure([&] { klib::image_to_jpeg(jpeg_file_name, out_name); });
  };
  BENCHMARK_ADVANCED("JPEG to JPEG max compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(out_name);
    meter.measure(
        [&] { klib::image_to_jpeg(jpeg_file_name, out_name, 75, true); });
  };
  BENCHMARK_ADVANCED("JPEG to WebP")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(out_name);
    meter.measure([&] { klib::image_to_webp(jpeg_file_name, out_name); });
  };
}
