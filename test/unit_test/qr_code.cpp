#include <cstdint>
#include <filesystem>
#include <string>

#include <dbg.h>
#include <fmt/core.h>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/image.h"
#include "klib/qr_code.h"
#include "klib/util.h"

TEST_CASE("QR Code", "[qr_code]") {
  const std::string text = "https://github.com/KaiserLancelot";
  const std::int32_t margin = 2;
  const std::int32_t zoom = 20;
  const std::string out_name = "qr-code.png";

  auto rgb = klib::qr_code_to_rbg(text, margin, zoom);
  klib::rgb_to_png(rgb, out_name);

  CHECK(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "c085e07520667538599cd13861c8f4825622d34126abc38349cc117ba223a270");
  dbg(std::filesystem::file_size(out_name));

  auto qr_code = klib::qr_code_to_utf8(text, margin);
  fmt::print("{}", qr_code);
  dbg(std::size(qr_code));
}
