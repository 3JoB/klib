#include "klib/qr_code.h"

#include <cerrno>
#include <cstring>
#include <string_view>

#include <fmt/core.h>
#include <qrencode.h>
#include <scope_guard.hpp>

#include "klib/exception.h"

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
  if (!qr) {
    throw RuntimeError(std::strerror(errno));
  }

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
  const auto qr = QRcode_encodeString(text.c_str(), 0, QRecLevel::QR_ECLEVEL_H,
                                      QRencodeMode::QR_MODE_8, true);
  SCOPE_EXIT { QRcode_free(qr); };
  if (!qr) {
    throw RuntimeError(std::strerror(errno));
  }

  constexpr std::string_view empty = " ";
  constexpr std::string_view low_half = "▄";
  constexpr std::string_view up_half = "▀";
  constexpr std::string_view full = "█";

  const auto real_width = (qr->width + margin * 2);

  print_margin(real_width, margin, full);

  for (std::int32_t y = 0; y < qr->width; y += 2) {
    const auto row1 = qr->data + y * qr->width;
    const auto row2 = row1 + qr->width;

    for (std::int32_t x = 0; x < margin; x++) {
      fmt::print("{}", full);
    }

    for (std::int32_t x = 0; x < qr->width; ++x) {
      if (row1[x] & 1) {
        if (y < qr->width - 1 && row2[x] & 1) {
          fmt::print("{}", empty);
        } else {
          fmt::print("{}", low_half);
        }
      } else if (y < qr->width - 1 && row2[x] & 1) {
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
