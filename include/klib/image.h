/**
 * @file image.h
 * @brief Contains image conversion module
 */

#pragma once

#include <cstdint>
#include <string>

namespace klib {

/**
 * @brief
 */
class RGB {
 public:
  /**
   * @brief Constructor
   * @param width: Image width
   * @param height: Image height
   */
  RGB(std::int32_t width, std::int32_t height);

  /**
   * @brief Constructor
   * @param rgb: RGB data
   * @param width: Image width
   * @param height: Image height
   * @param free: Whether to destroy RBG data when destructing
   */
  RGB(std::uint8_t *rgb, std::int32_t width, std::int32_t height,
      bool free = true);

  RGB(const RGB &) = delete;
  RGB(RGB &&) = delete;
  RGB &operator=(const RGB &) = delete;
  RGB &operator=(RGB &&) = delete;

  /**
   * @brief Destructor
   */
  ~RGB();

  /**
   * @brief Get RGB data
   * @return RGB data
   */
  [[nodiscard]] std::uint8_t *rgb() const { return rgb_; }

  /**
   * @brief Get width
   * @return Width
   */
  [[nodiscard]] std::int32_t width() const { return width_; }

  /**
   * @brief Get height
   * @return Height
   */
  [[nodiscard]] std::int32_t height() const { return height_; }

 private:
  std::uint8_t *rgb_;
  std::int32_t width_;
  std::int32_t height_;

  bool free_ = true;
};

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
 * @brief Convert to PNG format
 * @param rgb: The RGB to be converted
 * @param out_path: Output filename
 * @param quality: Compression quality
 * @param compression_level: Compression level (0-9)
 */
void rgb_to_png(const RGB &rgb, const std::string &out_path,
                std::int32_t quality = 75, std::int32_t compression_level = 6);

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
 * @brief Convert to JPEG format
 * @param rgb: The RGB to be converted
 * @param out_path: Output filename
 * @param quality: Compression quality
 * @param max_compress: Increase the compression ratio as much as possible,
 * at the expense of increased encoding time
 */
void rgb_to_jpeg(const RGB &rgb, const std::string &out_path,
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

/**
 * @brief Convert to WebP format
 * @param rgb: The RGB to be converted
 * @param out_path: Output filename
 * @param quality: Compression quality
 * @param method: Compression method (0=fast, 6=slowest)
 */
void rgb_to_webp(const RGB &rgb, const std::string &out_path,
                 std::int32_t quality = 75, std::int32_t method = 0);

}  // namespace klib
