/**
 * @file mime.h
 * @brief Contains functions to detect MIME type
 */

#pragma once

#include <string>

namespace klib {

/**
 * @brief Return MIME type based on file content
 * @param file: File content to be detected
 * @return Return the MIME type
 */
std::string mime(const std::string &data);

/**
 * @brief Check if the image is in PNG format
 * @param image: Image to be detected
 * @return Return true if it is in PNG format
 */
bool is_png(const std::string &image);

/**
 * @brief Check if the image is in JPEG format
 * @param image: Image to be detected
 * @return Return true if it is in JPEG format
 */
bool is_jpeg(const std::string &image);

/**
 * @brief Check if the image is in WebP format
 * @param image: Image to be detected
 * @return Return true if it is in WebP format
 */
bool is_webp(const std::string &image);

}  // namespace klib
