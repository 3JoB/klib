#pragma once

#include <cstdlib>
#include <string_view>
#include <utility>

#include <fmt/color.h>
#include <fmt/core.h>

namespace klib {

template <typename... Args>
void warn(std::string_view fmt, Args &&...args) {
  fmt::print(fmt::fg(fmt::color::yellow), "warning:\n");
  fmt::print(fmt::fg(fmt::color::yellow), fmt, std::forward<Args>(args)...);
  fmt::print("\n");
}

template <typename... Args>
[[noreturn]] void error(std::string_view fmt, Args &&...args) {
  fmt::print(fmt::fg(fmt::color::red), "error:\n");
  fmt::print(fmt::fg(fmt::color::red), fmt, std::forward<Args>(args)...);
  fmt::print("\n");

  std::exit(EXIT_FAILURE);
}

}  // namespace klib
