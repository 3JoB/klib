#pragma once

namespace klib::detail {

constexpr auto range =
    [](char32_t code_point, char32_t min, char32_t max) constexpr {
  return code_point >= min && code_point <= max;
};

}  // namespace klib::detail
