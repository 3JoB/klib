#pragma once

#include <cstdlib>
#include <string_view>
#include <utility>

#include <fmt/color.h>
#include <fmt/core.h>

namespace klib {

/**
 * @brief Report error
 * @param fmt: Format string
 * @param args: Parameter
 */
template <typename... Args>
void warn(std::string_view fmt, Args &&...args) {
  fmt::print(fmt::fg(fmt::color::yellow), "warning: ");
  fmt::print(fmt::fg(fmt::color::yellow), fmt, std::forward<Args>(args)...);
  fmt::print("\n");
}

/**
 * @brief Report warning
 * @param fmt: Format string
 * @param args: Parameter
 */
template <typename... Args>
[[noreturn]] void error(std::string_view fmt, Args &&...args) {
  fmt::print(fmt::fg(fmt::color::red), "error: ");
  fmt::print(fmt::fg(fmt::color::red), fmt, std::forward<Args>(args)...);
  fmt::print("\n");

  std::exit(EXIT_FAILURE);
}

}  // namespace klib
