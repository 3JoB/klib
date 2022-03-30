#include "klib/qr_code.h"

#include <cstring>

#include "QR-Code-generator/qrcodegen.hpp"

namespace klib {

RGB qr_code(const std::string &text, std::int32_t border, std::int32_t zoom) {
  const auto qr =
      qrcodegen::QrCode::encodeText(text.c_str(), qrcodegen::QrCode::Ecc::HIGH);

  border *= zoom;
  const auto qr_size = qr.getSize();

  const auto width = qr_size * zoom + 2 * border;
  const auto height = qr_size * zoom + 2 * border;
  const auto size = width * height * 3;

  auto rgb = new std::uint8_t[size];
  std::memset(rgb, 255, size);

  auto set_xy = [=](std::uint8_t *rgb, std::int32_t x, std::int32_t y) {
    auto offset = ((x + border) * width + (y + border));
    if (offset >= width * height) {
      return;
    }

    auto ptr = (rgb + offset * 3);
    ptr[0] = 0;
    ptr[1] = 0;
    ptr[2] = 0;
  };

  for (std::int32_t y = 0; y < qr_size; ++y) {
    for (std::int32_t x = 0; x < qr_size; ++x) {
      if (qr.getModule(x, y)) {
        for (std::int32_t i = 0; i < zoom; ++i) {
          for (std::int32_t j = 0; j < zoom; ++j) {
            set_xy(rgb, x * zoom + i, y * zoom + j);
          }
        }
      }
    }
  }

  return {rgb, width, height};
}

}  // namespace klib
