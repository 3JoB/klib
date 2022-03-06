#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace klib {

std::string image_to_webp(const std::string &image);

std::string image_to_webp(std::string_view image);

std::string image_to_webp(const char *image, std::size_t size);

}  // namespace klib
