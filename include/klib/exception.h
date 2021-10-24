/**
 * @file exception.h
 * @brief Contains exception class
 */

#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

// TODO DO not use fmt
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
  explicit RuntimeError(std::string_view msg) : RuntimeError(std::data(msg)) {}

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
  explicit LogicError(std::string_view msg) : LogicError(std::data(msg)) {}

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

class InvalidArgument : std::invalid_argument {
 public:
  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit InvalidArgument(const char *msg) : std::invalid_argument(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit InvalidArgument(const std::string &msg)
      : std::invalid_argument(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit InvalidArgument(std::string_view msg)
      : InvalidArgument(std::data(msg)) {}

  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit InvalidArgument(std::string_view fmt, Args &&...args)
      : InvalidArgument(
            fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...)) {}
};

class OutOfRange : std::out_of_range {
 public:
  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit OutOfRange(const char *msg) : std::out_of_range(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit OutOfRange(const std::string &msg) : std::out_of_range(msg) {}

  /**
   * @brief Constructor
   * @param msg: Exception information
   */
  explicit OutOfRange(std::string_view msg) : OutOfRange(std::data(msg)) {}

  /**
   * @brief Constructor
   * @param fmt: Format string
   * @param args: Format string parameters
   */
  template <typename... Args>
  explicit OutOfRange(std::string_view fmt, Args &&...args)
      : OutOfRange(
            fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...)) {}
};

}  // namespace klib
