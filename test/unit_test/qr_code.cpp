#include <cstdint>
#include <filesystem>
#include <string>

#include <dbg.h>
#include <catch2/catch.hpp>

#include "klib/hash.h"
#include "klib/image.h"
#include "klib/qr_code.h"
#include "klib/util.h"

TEST_CASE("QR Code", "[qr_code]") {
  const std::string text = "https://github.com/KaiserLancelot";
  const std::int32_t border = 2;
  const std::int32_t zoom = 20;
  const std::string out_name = "qr-code.webp";

  auto rgb = klib::qr_code(text, border, zoom);
  klib::rgb_to_webp(rgb, out_name);

  CHECK(std::filesystem::exists(out_name));
  CHECK(klib::sha256_hex(klib::read_file(out_name, true)) ==
        "1f6a82f289a4f5c1f8e827d999997c12a0b007c858dec89d9f1dd0538bb347bb");

  dbg(std::filesystem::file_size(out_name));
}
