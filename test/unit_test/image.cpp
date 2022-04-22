#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/image.h"
#include "klib/util.h"

TEST_CASE("jpeg to png", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "avatar.jpeg.to.png";

  klib::image_to_png(file_name, out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "fc26852eb0bfd622a96d4fbf6bd5711f63b4b3abcc9a03698230250dce0a37c4");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to jpeg", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.png.to.jpg";

  klib::image_to_jpeg(file_name, out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "90682af5c3cbe7eb661ed5b72a36a6c35c4844edbe13a0fec11c970eaa49fbd8");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to jpeg max compress", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.max.compress.png.to.jpg";

  klib::image_to_jpeg(file_name, out_name, 75, true);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "c45d0395b298479b41130d0cba09ccb68c53483349e301ab7ec5dde50a0769f3");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to WebP", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.png.to.webp";

  klib::image_to_webp(file_name, out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "6947601e6faa622c72c1ebb3a538ef3db545b539315c97980dbd29d4bfc21133");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("jpg to WebP", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "avatar.jpeg.to.webp";

  klib::image_to_webp(file_name, out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "bf468f34f49c9b217b8f4c7fe4c595cf4a87308358244faf902421124ba57399");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to png", "[image]") {
  const std::string file_name = "wallpaper.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.webp.to.png";

  klib::image_to_png(file_name, out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "2abc3a803b7a619a2f728e0d3dbe703c0ac6a197442a0d1227b5e5750aeaefd6");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to JPEG", "[image]") {
  const std::string file_name = "wallpaper.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.webp.to.jpg";

  klib::image_to_jpeg(file_name, out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "8c96212868719e64b249569073f856b2186ada0f39d8d95e119d5f5424693f44");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("is_png", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const auto image = klib::read_file(file_name, true);

  CHECK(klib::is_png(image));
}

TEST_CASE("is_jpeg", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const auto image = klib::read_file(file_name, true);

  CHECK(klib::is_jpeg(image));
}

TEST_CASE("is_webp", "[image]") {
  const std::string file_name = "wallpaper.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const auto image = klib::read_file(file_name, true);

  CHECK(klib::is_webp(image));
}
