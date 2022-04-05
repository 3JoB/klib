#include "klib/qr_code.h"

#include <cstring>
#include <string_view>

#include <fmt/core.h>
#include <qrencode.h>
#include <scope_guard.hpp>

namespace klib {

namespace {

void print_margin(std::int32_t real_width, std::int32_t margin,
                  std::string_view full) {
  for (std::int32_t y = 0; y < margin / 2; ++y) {
    for (std::int32_t x = 0; x < real_width; ++x) {
      fmt::print("{}", full);
    }
    fmt::print("\n");
  }
}

}  // namespace

RGB qr_code(const std::string &text, std::int32_t margin, std::int32_t zoom) {
  const auto qr = QRcode_encodeString(text.c_str(), 0, QRecLevel::QR_ECLEVEL_H,
                                      QRencodeMode::QR_MODE_8, true);
  SCOPE_EXIT { QRcode_free(qr); };

  margin *= zoom;
  const auto qr_size = qr->width;

  const auto width = qr_size * zoom + 2 * margin;
  const auto height = qr_size * zoom + 2 * margin;
  const auto size = width * height * 3;

  auto rgb = new std::uint8_t[size];
  std::memset(rgb, 255, size);

  auto set_xy = [=](std::uint8_t *rgb, std::int32_t x, std::int32_t y) {
    auto offset = ((y + margin) * width + (x + margin));
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
      if (*(qr->data + (y * qr_size + x)) & 1) {
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

void print_qr_code(const std::string &text, std::int32_t margin) {
  const auto qr_code = QRcode_encodeString(
      text.c_str(), 0, QRecLevel::QR_ECLEVEL_H, QRencodeMode::QR_MODE_8, true);
  SCOPE_EXIT { QRcode_free(qr_code); };

  std::string_view empty = " ";
  std::string_view low_half = "\342\226\204";
  std::string_view up_half = "\342\226\200";
  std::string_view full = "\342\226\210";

  auto real_width = (qr_code->width + margin * 2);

  print_margin(real_width, margin, full);

  for (std::int32_t y = 0; y < qr_code->width; y += 2) {
    auto row1 = qr_code->data + y * qr_code->width;
    auto row2 = row1 + qr_code->width;

    for (std::int32_t x = 0; x < margin; x++) {
      fmt::print("{}", full);
    }

    for (std::int32_t x = 0; x < qr_code->width; ++x) {
      if (row1[x] & 1) {
        if (y < qr_code->width - 1 && row2[x] & 1) {
          fmt::print("{}", empty);
        } else {
          fmt::print("{}", low_half);
        }
      } else if (y < qr_code->width - 1 && row2[x] & 1) {
        fmt::print("{}", up_half);
      } else {
        fmt::print("{}", full);
      }
    }

    for (std::int32_t x = 0; x < margin; ++x) {
      fmt::print("{}", full);
    }

    fmt::print("\n");
  }

  print_margin(real_width, margin, full);
}

}  // namespace klib
