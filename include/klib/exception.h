/**
 * @file exception.h
 * @brief Contains exception class
 */

#pragma once

#include <stdexcept>
#include <string>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

#include "klib/detail/format_with_location.h"

namespace klib {

/**
 * @brief Exception class, which means runtime error
 */
class RuntimeError : public std::runtime_error {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit RuntimeError(detail::format_with_location fmt_with_loc,
                        Args &&...args)
      : std::runtime_error(std::string(fmt_with_loc.loc_.filename) + ":" +
                           std::to_string(fmt_with_loc.loc_.line) + ": " +
                           fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                                       std::forward<Args>(args)...)) {}
};

/**
 * @brief Exception class, which means logic error
 */
class LogicError : public std::logic_error {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit LogicError(detail::format_with_location fmt_with_loc, Args &&...args)
      : std::logic_error(std::string(fmt_with_loc.loc_.filename) + ":" +
                         std::to_string(fmt_with_loc.loc_.line) + ": " +
                         fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                                     std::forward<Args>(args)...)) {}
};

/**
 * @brief Exception class, which means invalid argument
 */
class InvalidArgument : std::invalid_argument {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit InvalidArgument(detail::format_with_location fmt_with_loc,
                           Args &&...args)
      : std::invalid_argument(std::string(fmt_with_loc.loc_.filename) + ":" +
                              std::to_string(fmt_with_loc.loc_.line) + ": " +
                              fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                                          std::forward<Args>(args)...)) {}
};

/**
 * @brief Exception class, which means out of range
 */
class OutOfRange : std::out_of_range {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit OutOfRange(detail::format_with_location fmt_with_loc, Args &&...args)
      : std::out_of_range(std::string(fmt_with_loc.loc_.filename) + ":" +
                          std::to_string(fmt_with_loc.loc_.line) + ": " +
                          fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                                      std::forward<Args>(args)...)) {}
};

}  // namespace klib
