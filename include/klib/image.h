/**
 * @file image.h
 * @brief Contains WebP conversion module
 */

#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace klib {

/**
 * @brief Convert to WebP format
 * @param image: Image to be converted
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string image_to_webp(const std::string &image, bool lossless = false);

/**
 * @brief Convert to WebP format
 * @param image: Image to be converted
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string image_to_webp(std::string_view image, bool lossless = false);

/**
 * @brief Convert to WebP format
 * @param image: Image to be converted
 * @param size: Image size
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string image_to_webp(const char *image, std::size_t size,
                          bool lossless = false);

/**
 * @brief Convert from WebP format to PNG format
 * @param image: Image to be converted
 * @return Image in PNG format
 */
std::string webp_to_png(const std::string &image);

/**
 * @brief Convert from WebP format to PNG format
 * @param image: Image to be converted
 * @return Image in PNG format
 */
std::string webp_to_png(std::string_view image);

/**
 * @brief Convert from WebP format to PNG format
 * @param image: Image to be converted
 * @param size: Image size
 * @return Image in PNG format
 */
std::string webp_to_png(const char *image, std::size_t size);

}  // namespace klib
