#include <filesystem>
#include <string>

#include <catch2/catch.hpp>

#include "klib/image.h"
#include "klib/util.h"

TEST_CASE("Image", "[image]") {
  const std::string file_name = "cover.jpg";
  REQUIRE(std::filesystem::exists(file_name));

  const auto jpg_image = klib::read_file(file_name, true);

  BENCHMARK("WebP") { return klib::image_to_webp(jpg_image); };
}
