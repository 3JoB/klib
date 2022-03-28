/**
 * @file image.h
 * @brief Contains image conversion module
 */

#pragma once

#include <cstdint>
#include <string>

namespace klib {

/**
 * @brief Convert to PNG format
 * @param image_path: The path of the image to be converted
 * @param out_path: Output filename
 * @param quality: Compression quality
 * @param compression_level: Compression level (0-9)
 */
void image_to_png(const std::string &image_path, const std::string &out_path,
                  std::int32_t quality = 75,
                  std::int32_t compression_level = 6);

/**
 * @brief Convert to JPEG format
 * @param image_path: The path of the image to be converted
 * @param out_path: Output filename
 * @param quality: Compression quality
 * @param max_compress: Increase the compression ratio as much as possible,
 * at the expense of increased encoding time
 */
void image_to_jpeg(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality = 75, bool max_compress = false);

/**
 * @brief Convert to WebP format
 * @param image_path: The path of the image to be converted
 * @param out_path: Output filename
 * @param quality: Compression quality
 * @param method: Compression method (0=fast, 6=slowest)
 */
void image_to_webp(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality = 75, std::int32_t method = 0);

}  // namespace klib
