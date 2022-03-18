/**
 * @file image.h
 * @brief Contains WebP conversion module
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include "klib/detail/config.h"

namespace klib {

/**
 * @brief Convert PNG/JPEG format to JPEG format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @param max_compress: Increase the compression ratio as much as possible,
 * at the expense of increased encoding time
 * @return Image in JPEG format
 */
std::string KLIB_EXPORT image_to_jpeg(const std::string &image,
                                      std::int32_t quality = 75,
                                      bool max_compress = false);

/**
 * @brief Convert PNG/JPEG to format JPEG format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @param max_compress: Increase the compression ratio as much as possible,
 * at the expense of increased encoding time
 * @return Image in JPEG format
 */
std::string KLIB_EXPORT image_to_jpeg(std::string_view image,
                                      std::int32_t quality = 75,
                                      bool max_compress = false);

/**
 * @brief Convert PNG/JPEG to format JPEG format
 * @param image: Image to be converted
 * @param size: Image size
 * @param quality: Compression quality
 * @param max_compress: Increase the compression ratio as much as possible,
 * at the expense of increased encoding time
 * @return Image in JPEG format
 */
std::string KLIB_EXPORT image_to_jpeg(const char *image, std::size_t size,
                                      std::int32_t quality = 75,
                                      bool max_compress = false);

/**
 * @brief Convert PNG/JPEG format to WebP format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @param method: Compression method (0=fast, 6=slowest)
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string KLIB_EXPORT image_to_webp(const std::string &image,
                                      std::int32_t quality = 75,
                                      std::int32_t method = 0,
                                      bool lossless = false);

/**
 * @brief Convert PNG/JPEG format to WebP format
 * @param image: Image to be converted
 * @param quality: Compression quality
 * @param method: Compression method (0=fast, 6=slowest)
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string KLIB_EXPORT image_to_webp(std::string_view image,
                                      std::int32_t quality = 75,
                                      std::int32_t method = 0,
                                      bool lossless = false);

/**
 * @brief Convert PNG/JPEG format to WebP format
 * @param image: Image to be converted
 * @param size: Image size
 * @param quality: Compression quality
 * @param method: Compression method (0=fast, 6=slowest)
 * @param lossless: Whether to do lossless conversion
 * @return Image in WebP format
 */
std::string KLIB_EXPORT image_to_webp(const char *image, std::size_t size,
                                      std::int32_t quality = 75,
                                      std::int32_t method = 0,
                                      bool lossless = false);

/**
 * @brief Convert from WebP format to PNG format
 * @param image: Image to be converted
 * @return Image in PNG format
 */
std::string KLIB_EXPORT webp_to_png(const std::string &image);

/**
 * @brief Convert from WebP format to PNG format
 * @param image: Image to be converted
 * @return Image in PNG format
 */
std::string KLIB_EXPORT webp_to_png(std::string_view image);

/**
 * @brief Convert from WebP format to PNG format
 * @param image: Image to be converted
 * @param size: Image size
 * @return Image in PNG format
 */
std::string KLIB_EXPORT webp_to_png(const char *image, std::size_t size);

}  // namespace klib
