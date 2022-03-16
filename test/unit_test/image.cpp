#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/image.h"
#include "klib/util.h"

TEST_CASE("jpeg to jpeg", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "avatar.compress.jpg";
  const auto compressed_image = klib::image_to_jpeg(image);
  klib::write_file(out_name, true, compressed_image);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "34723e33eab435b47f10454f25c56a6894bfccacb0ce55e287004772df2e112f");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("jpeg to jpeg max compress", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "avatar.max.compress.jpg";
  const auto compressed_image = klib::image_to_jpeg(image, 75, true);
  klib::write_file(out_name, true, compressed_image);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "84f83f262ac2343efcba538d12714bc2f18876011111abc65fa7af1a2f8de533");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to jpeg", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.compress.jpg";
  const auto jpg_image = klib::image_to_jpeg(image);
  klib::write_file(out_name, true, jpg_image);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "bd57b1a08d8a84742ff882bd4dd2d0b0ff0581bc8a467ee378b22c7a35dc7f33");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to jpeg max compress", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.max.compress.jpg";
  const auto jpg_image = klib::image_to_jpeg(image, 75, true);
  klib::write_file(out_name, true, jpg_image);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "5d857b1affb231c7b183e6c54b7212c2ede12db8bebcf8e73647f8a9b32be836");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to WebP", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.webp";
  const auto webp_image = klib::image_to_webp(image);
  klib::write_file(out_name, true, webp_image);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "6947601e6faa622c72c1ebb3a538ef3db545b539315c97980dbd29d4bfc21133");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to WebP lossless", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "wallpaper.lossless.webp";
  const auto webp_image = klib::image_to_webp(image, 75, 0, true);
  klib::write_file(out_name, true, webp_image);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "43d1c51edff557a1144d5ad82b23379b87b63aef40ecb229d1eba474f6b11ae5");

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
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "5d323891d34749811f35d5bcd9bc55c07ffb0dccd2b8b160281a4e55904ad257");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("jpg to WebP lossless", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string image = klib::read_file(file_name, true);

  const std::string out_name = "avatar.lossless.webp";
  const auto webp_image = klib::image_to_webp(image, 75, 0, true);
  klib::write_file(out_name, true, webp_image);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "94975542b14cae76fd2dd7bfa68a71678f7eecd55292cdb249181d52b46b715a");

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
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
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
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "36f936fade6b3d6e2f1c7e7f798fb95713a3b8e641a3eee964d8aca757954d7d");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}
