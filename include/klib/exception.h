/**
 * @file exception.h
 * @brief Contains exception class
 */

#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>

namespace klib {

/**
 * @brief Exception class, which means runtime error
 */
class RuntimeError : public std::runtime_error {
 public:
  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit RuntimeError(const char *msg) : std::runtime_error(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit RuntimeError(const std::string &msg) : std::runtime_error(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit RuntimeError(std::string_view msg) : RuntimeError(msg.data()) {}

  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit RuntimeError(std::string_view fmt, Args &&...args)
      : RuntimeError(
            fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...)) {}
};

/**
 * @brief Exception class, which means logic error
 */
class LogicError : public std::logic_error {
 public:
  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit LogicError(const char *msg) : std::logic_error(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit LogicError(const std::string &msg) : std::logic_error(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit LogicError(std::string_view msg) : LogicError(msg.data()) {}

  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit LogicError(std::string_view fmt, Args &&...args)
      : LogicError(
            fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...)) {}
};

}  // namespace klib
