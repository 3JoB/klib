/**
 * @file image.h
 * @brief Contains WebP conversion module
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace klib {

/**
 * @brief Convert PNG/JPEG format to JPEG format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @return Image in JPEG format
 */
std::string image_to_jpeg(const std::string &image, std::int32_t quality = 75);

/**
 * @brief Convert PNG/JPEG to format JPEG format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @return Image in JPEG format
 */
std::string image_to_jpeg(std::string_view image, std::int32_t quality = 75);

/**
 * @brief Convert PNG/JPEG to format JPEG format
 * @param image: Image to be converted
 * @param size: Image size
 * @param quality: Compression quality
 * @return Image in JPEG format
 */
std::string image_to_jpeg(const char *image, std::size_t size,
                          std::int32_t quality = 75);

/**
 * @brief Convert PNG/JPEG format to WebP format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string image_to_webp(const std::string &image, std::int32_t quality = 75,
                          bool lossless = false);

/**
 * @brief Convert PNG/JPEG format to WebP format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string image_to_webp(std::string_view image, std::int32_t quality = 75,
                          bool lossless = false);

/**
 * @brief Convert PNG/JPEG format to WebP format
 * @param image: Image to be converted
 * @param size: Image size
 * @param quality: Compression quality
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string image_to_webp(const char *image, std::size_t size,
                          std::int32_t quality = 75, bool lossless = false);

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
