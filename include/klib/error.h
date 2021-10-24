/**
 * @file error.h
 * @brief Contains report error and warning modules
 */

#pragma once

#include <cstdlib>
#include <experimental/source_location>
#include <string_view>
#include <utility>

#include <fmt/color.h>
#include <fmt/core.h>

namespace klib {

/**
 * @brief Report warning
 * @param fmt: Format string
 * @param args: Format string parameters
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
 * @param args: Format string parameters
 */
template <typename... Args>
void warn(const std::experimental::source_location &loc, std::string_view fmt,
          Args &&...args) {
  fmt::print(fmt::fg(fmt::color::yellow), "{}:{}: warning: ", loc.file_name(),
             loc.line());
  fmt::print(fmt::fg(fmt::color::yellow), fmt, std::forward<Args>(args)...);
  fmt::print("\n");
}

/**
 * @brief Report error and exit
 * @param fmt: Format string
 * @param args: Format string parameters
 */
template <typename... Args>
[[noreturn]] void error(std::string_view fmt, Args &&...args) {
  fmt::print(fmt::fg(fmt::color::red), "error: ");
  fmt::print(fmt::fg(fmt::color::red), fmt, std::forward<Args>(args)...);
  fmt::print("\n");

  std::exit(EXIT_FAILURE);
}

/**
 * @brief Report error and exit
 * @param fmt: Format string
 * @param args: Format string parameters
 */
template <typename... Args>
[[noreturn]] void error(const std::experimental::source_location &loc,
                        std::string_view fmt, Args &&...args) {
  fmt::print(fmt::fg(fmt::color::red), "{}:{}: error: ", loc.file_name(),
             loc.line());
  fmt::print(fmt::fg(fmt::color::red), fmt, std::forward<Args>(args)...);
  fmt::print("\n");
  std::exit(EXIT_FAILURE);
}

#define KLIB_CURR_LOC std::experimental::source_location::current()

}  // namespace klib
