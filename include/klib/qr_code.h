#pragma once

#include <cstdint>
#include <string>

#include "klib/image.h"

namespace klib {

/**
 * @brief Generate QR code containing 'text'
 * @param text: Data in QR code
 * @param border: Border size (pixels)
 * @param zoom: Zoom level
 * @return RBG data with QR code
 */
RGB qr_code(const std::string &text, std::int32_t border, std::int32_t zoom);

}  // namespace klib
