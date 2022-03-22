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
 * @brief
 */
class KLIB_EXPORT Image {
 public:
  explicit Image(const std::string &file_name);

  ~Image();

  void to_png(const std::string &out_name, std::int32_t quality = 75,
              std::int32_t compression_level = 6);
  void to_jpeg(const std::string &out_name, std::int32_t quality = 75,
               bool max_compress = false);
  void to_webp(const std::string &out_name, std::int32_t quality = 75,
               std::int32_t method = 0, bool lossless = false);

 private:
  class ImageImpl;
  std::experimental::propagate_const<std::unique_ptr<ImageImpl>> impl_;
};

}  // namespace klib
