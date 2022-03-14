#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/image.h"
#include "klib/util.h"

TEST_CASE("Image", "[image]") {
  const std::string jpeg_file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(jpeg_file_name));
  const auto jpg_image = klib::read_file(jpeg_file_name, true);

  const std::string png_file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(png_file_name));
  const auto png_image = klib::read_file(png_file_name, true);

  BENCHMARK("PNG to JPEG") { return klib::image_to_jpeg(png_image); };
  BENCHMARK("PNG to JPEG max compress") {
    return klib::image_to_jpeg(png_image, 75, true);
  };
  BENCHMARK("PNG to WebP") { return klib::image_to_webp(png_image); };

  BENCHMARK("JPEG to JPEG") { return klib::image_to_jpeg(jpg_image); };
  BENCHMARK("JPEG to JPEG max compress") {
    return klib::image_to_jpeg(jpg_image, 75, true);
  };
  BENCHMARK("JPEG to WebP") { return klib::image_to_webp(jpg_image); };
}
