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

  klib::Image image(file_name);
  image.to_png(out_name);

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

  klib::Image image(file_name);
  image.to_jpeg(out_name);

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

  klib::Image image(file_name);
  image.to_jpeg(out_name, 75, true);

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

  klib::Image image(file_name);
  image.to_webp(out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "6947601e6faa622c72c1ebb3a538ef3db545b539315c97980dbd29d4bfc21133");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("png to WebP lossless", "[image]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.lossless.png.to.webp";

  klib::Image image(file_name);
  image.to_webp(out_name, 75, 0, true);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "43d1c51edff557a1144d5ad82b23379b87b63aef40ecb229d1eba474f6b11ae5");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("jpg to WebP", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "avatar.jpeg.to.webp";

  klib::Image image(file_name);
  image.to_webp(out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "bf468f34f49c9b217b8f4c7fe4c595cf4a87308358244faf902421124ba57399");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("jpg to WebP lossless", "[image]") {
  const std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "avatar.lossless.jpeg.to.webp";

  klib::Image image(file_name);
  image.to_webp(out_name, 75, 0, true);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "b2bb83fd898377c6966a21e086cd777575c8e82f10fbe1c979c6971ccc05b174");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to png", "[image]") {
  const std::string file_name = "wallpaper.std.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.webp.to.png";

  klib::Image image(file_name);
  image.to_png(out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "2abc3a803b7a619a2f728e0d3dbe703c0ac6a197442a0d1227b5e5750aeaefd6");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to png lossless", "[image]") {
  const std::string file_name = "wallpaper.std.lossless.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.lossless.webp.to.png";

  klib::Image image(file_name);
  image.to_png(out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "121c1db9fb2e2ea849081e166fbbfd244a8d03f7faccc1ba4bf123ce0471f872");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to JPEG", "[image]") {
  const std::string file_name = "wallpaper.std.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.webp.to.jpg";

  klib::Image image(file_name);
  image.to_jpeg(out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "8c96212868719e64b249569073f856b2186ada0f39d8d95e119d5f5424693f44");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}

TEST_CASE("WebP to JPEG lossless", "[image]") {
  const std::string file_name = "wallpaper.std.lossless.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const std::string out_name = "wallpaper.lossless.webp.to.jpg";

  klib::Image image(file_name);
  image.to_jpeg(out_name);

  REQUIRE(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "b0b9fac7c0225cf568d9c965335c1a74242554b1ec449f17ebb90fc327129aad");

  dbg(std::filesystem::file_size(file_name));
  dbg(std::filesystem::file_size(out_name));
}
