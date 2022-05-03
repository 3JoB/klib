#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/mime.h"
#include "klib/util.h"

TEST_CASE("is_png", "[mime]") {
  const std::string file_name = "wallpaper.png";
  REQUIRE(std::filesystem::exists(file_name));
  const auto image = klib::read_file(file_name, true);

  CHECK(klib::is_png(image));
}

TEST_CASE("is_jpeg", "[mime]") {
  std::string file_name = "avatar.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  auto image = klib::read_file(file_name, true);
  CHECK(klib::is_jpeg(image));

  file_name = "test.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  image = klib::read_file(file_name, true);
  CHECK(klib::is_jpeg(image));

  file_name = "test2.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  image = klib::read_file(file_name, true);
  CHECK(klib::is_jpeg(image));

  file_name = "test3.jpg";
  REQUIRE(std::filesystem::exists(file_name));
  image = klib::read_file(file_name, true);
  CHECK(klib::is_jpeg(image));
}

TEST_CASE("is_webp", "[mime]") {
  const std::string file_name = "wallpaper.webp";
  REQUIRE(std::filesystem::exists(file_name));
  const auto image = klib::read_file(file_name, true);

  CHECK(klib::is_webp(image));
}
