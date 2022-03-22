/**
 * @file image.h
 * @brief Contains image conversion module
 */

#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>

#include "klib/detail/config.h"

namespace klib {

/**
 * @brief Class for Image reading and writing
 */
class KLIB_EXPORT Image {
 public:
  /**
   * @brief Constructor
   * @param file_name: The name of the image to be read
   */
  explicit Image(const std::string &file_name);

  Image(const Image &) = delete;
  Image(Image &&) = delete;
  Image &operator=(const Image &) = delete;
  Image &operator=(Image &&) = delete;

  /**
   * @brief Destructor
   */
  ~Image();

  /**
   * @brief Convert to PNG format
   * @param out_name: Output filename
   * @param quality: Compression quality
   * @param compression_level: Compression level (0-9)
   */
  void to_png(const std::string &out_name, std::int32_t quality = 75,
              std::int32_t compression_level = 6);

  /**
   * @brief Convert to JPEG format
   * @param out_name: Output filename
   * @param quality: Compression quality
   * @param max_compress: Increase the compression ratio as much as possible,
   * at the expense of increased encoding time
   */
  void to_jpeg(const std::string &out_name, std::int32_t quality = 75,
               bool max_compress = false);

  /**
   * @brief Convert to WebP format
   * @param out_name: Output filename
   * @param quality: Compression quality
   * @param method: Compression method (0=fast, 6=slowest)
   * @param lossless: Whether to do lossless conversion
   */
  void to_webp(const std::string &out_name, std::int32_t quality = 75,
               std::int32_t method = 0, bool lossless = false);

 private:
  class ImageImpl;
  std::experimental::propagate_const<std::unique_ptr<ImageImpl>> impl_;
};

}  // namespace klib
