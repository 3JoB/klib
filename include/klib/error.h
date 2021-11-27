/**
 * @file error.h
 * @brief Contains report error and warning modules
 */

#pragma once

#include <cstdlib>
#include <string_view>
#include <utility>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "detail/log.h"

namespace klib {

/**
 * @brief Report info
 * @param fmt: Format string
 * @param args: Format string parameters
 */
template <typename... Args>
void info(std::string_view fmt, Args &&...args) {
  spdlog::info(fmt::runtime(fmt), std::forward<Args>(args)...);
}

/**
 * @brief Report warning
 * @param fmt: Format string
 * @param args: Format string parameters
 */
template <typename... Args>
void warn(std::string_view fmt, Args &&...args) {
  spdlog::warn(fmt::runtime(fmt), std::forward<Args>(args)...);
}

/**
 * @brief Report error and exit
 * @param fmt_with_loc: Format string with source location information
 * @param args: Format string parameters
 */
template <typename... Args>
void error(detail::format_with_location fmt_with_loc, Args &&...args) {
  spdlog::default_logger_raw()->log(fmt_with_loc.loc_, spdlog::level::err,
                                    fmt::runtime(fmt_with_loc.fmt_),
                                    std::forward<Args>(args)...);
  std::exit(EXIT_FAILURE);
}

}  // namespace klib
