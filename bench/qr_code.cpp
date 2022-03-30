#include <cstdint>
#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/image.h"
#include "klib/qr_code.h"

TEST_CASE("QR Code", "[qr_code]") {
  const std::string text = "https://github.com/KaiserLancelot";
  const std::int32_t border = 2;
  const std::int32_t zoom = 20;

  const std::string png_out_name = "qr-code.png";
  const std::string jpeg_out_name = "qr-code.jpeg";
  const std::string max_jpeg_out_name = "qr-code.max.jpeg";
  const std::string webp_out_name = "qr-code.webp";

  BENCHMARK("QR Code") { return klib::qr_code(text, border, zoom); };

  BENCHMARK_ADVANCED("QR Code to PNG")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(png_out_name);
    meter.measure([&] {
      auto rgb = klib::qr_code(text, border, zoom);
      klib::rgb_to_png(rgb, png_out_name);
    });
  };

  BENCHMARK_ADVANCED("QR Code to JPEG")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(jpeg_out_name);
    meter.measure([&] {
      auto rgb = klib::qr_code(text, border, zoom);
      klib::rgb_to_jpeg(rgb, jpeg_out_name);
    });
  };

  BENCHMARK_ADVANCED("QR Code to JPEG max compress")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(max_jpeg_out_name);
    meter.measure([&] {
      auto rgb = klib::qr_code(text, border, zoom);
      klib::rgb_to_jpeg(rgb, max_jpeg_out_name, 75, true);
    });
  };

  BENCHMARK_ADVANCED("QR Code to WebP")
  (Catch::Benchmark::Chronometer meter) {
    std::filesystem::remove(webp_out_name);
    meter.measure([&] {
      auto rgb = klib::qr_code(text, border, zoom);
      klib::rgb_to_webp(rgb, webp_out_name);
    });
  };
}
