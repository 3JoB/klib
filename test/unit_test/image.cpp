#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/image.h"
#include "klib/util.h"

TEST_CASE("png to WebP", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.webp";
  const auto webp_image = klib::image_to_webp(image);
  klib::write_file(out_name, true, webp_image);

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "7d0342e690718932748520cbb5e354c20bcd08bc74c6fa8eb8d200ed9f84fe8a");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to WebP lossless", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.lossless.webp";
  const auto webp_image = klib::image_to_webp(image, true);
  klib::write_file(out_name, true, webp_image);

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "76d7ee7c75a0f29aca00182623da63c2ee955dbe298b893b18dbf352f739bef8");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("jpg to WebP", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "avatar.webp";
  const auto webp_image = klib::image_to_webp(image);
  klib::write_file(out_name, true, webp_image);

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "8ca8e5004dcbd964fa1202c756b9f492d9e4a38b902dccfed893209bff22aa89");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("jpg to WebP lossless", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "avatar.lossless.webp";
  const auto webp_image = klib::image_to_webp(image, true);
  klib::write_file(out_name, true, webp_image);

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "f2120c9500e91392a9e5bb37093db030e51208a5eef74574b75158c53b64708b");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to png", "[image]") {
  const std::string file_name = "wallpaper.std.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string webp_image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.klib.png";
  const auto png_image = klib::webp_to_png(webp_image);
  klib::write_file(out_name, true, png_image);

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "f1b901a68bf116e3053bc0f1ec5244e907bcefbed61bba3b124e6d216959deb3");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to png lossless", "[image]") {
  const std::string file_name = "wallpaper.std.lossless.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string webp_image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.klib.lossless.png";
  const auto png_image = klib::webp_to_png(webp_image);
  klib::write_file(out_name, true, png_image);

  REQUIRE(std::filesystem::exists(out_name));
  REQUIRE(klib::sha256_hex(klib::read_file(out_name, true)) ==
          "36f936fade6b3d6e2f1c7e7f798fb95713a3b8e641a3eee964d8aca757954d7d");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}
