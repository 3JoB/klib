/**
 * @file log.h
 * @brief Contains log module
 */

#pragma once

#include <cstdlib>
#include <string_view>
#include <utility>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "detail/log_util.h"

namespace klib {

/**
 * @brief Report warning
 * @param fmt_with_loc: Format string with source location information
 * @param args: Format string parameters
 */
template <typename... Args>
void warn(detail::format_with_location fmt_with_loc, Args &&...args) {
  spdlog::default_logger_raw()->log(fmt_with_loc.loc_, spdlog::level::warn,
                                    fmt::runtime(fmt_with_loc.fmt_),
                                    std::forward<Args>(args)...);
}

/**
 * @brief Report error and exit
 * @param fmt_with_loc: Format string with source location information
 * @param args: Format string parameters
 */
template <typename... Args>
[[noreturn]] void error(detail::format_with_location fmt_with_loc,
                        Args &&...args) {
  spdlog::default_logger_raw()->log(fmt_with_loc.loc_, spdlog::level::err,
                                    fmt::runtime(fmt_with_loc.fmt_),
                                    std::forward<Args>(args)...);
  std::exit(EXIT_FAILURE);
}

}  // namespace klib
