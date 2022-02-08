/**
 * @file exception.h
 * @brief Contains exception module
 */

#pragma once

#include <exception>
#include <string>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

#include "klib/detail/format_with_location.h"

namespace klib {

/**
 * @brief Exception class
 */
class Exception : public std::exception {
 public:
  /**
   * @brief Constructor
   * @param msg: Exception message
   */
  explicit Exception(std::string msg) : msg_(std::move(msg)) {}

  /**
   * @brief Get exception message
   * @return Exception message
   */
  [[nodiscard]] const char *what() const noexcept override {
    return msg_.c_str();
  }

 private:
  std::string msg_;
};

/**
 * @brief Exception class, which means runtime error
 */
class RuntimeError : public Exception {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit RuntimeError(detail::format_with_location fmt_with_loc,
                        Args &&...args)
      : Exception(detail::location_to_string(fmt_with_loc.loc_) +
                  fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                              std::forward<Args>(args)...)) {}
};

/**
 * @brief Exception class, which means logic error
 */
class LogicError : public Exception {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit LogicError(detail::format_with_location fmt_with_loc, Args &&...args)
      : Exception(detail::location_to_string(fmt_with_loc.loc_) +
                  fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                              std::forward<Args>(args)...)) {}
};

/**
 * @brief Exception class, which means invalid argument
 */
class InvalidArgument : public Exception {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit InvalidArgument(detail::format_with_location fmt_with_loc,
                           Args &&...args)
      : Exception(detail::location_to_string(fmt_with_loc.loc_) +
                  fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                              std::forward<Args>(args)...)) {}
};

/**
 * @brief Exception class, which means out of range
 */
class OutOfRange : public Exception {
 public:
  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit OutOfRange(detail::format_with_location fmt_with_loc, Args &&...args)
      : Exception(detail::location_to_string(fmt_with_loc.loc_) +
                  fmt::format(fmt::runtime(fmt_with_loc.fmt_),
                              std::forward<Args>(args)...)) {}
};

}  // namespace klib
