/**
 * @file qr_code.h
 * @brief Contains generating and outputting QR code module
 */

#pragma once

#include <cstdint>
#include <string>

#include "klib/image.h"

namespace klib {

/**
 * @brief Generate QR code containing 'text'
 * @param text: Data in QR code
 * @param margin: Margin size (pixels)
 * @param zoom: Zoom level
 * @return RBG data with QR code
 */
RGB qr_code_to_rbg(const std::string &text, std::int32_t margin,
                   std::int32_t zoom);

/**
 * @brief Generate QR code containing 'text'
 * @param text: Data in QR code
 * @param margin: Margin size (pixels)
 */
std::string qr_code_to_utf8(const std::string &text, std::int32_t margin);

}  // namespace klib
