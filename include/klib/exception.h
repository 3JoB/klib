/**
 * @file exception.h
 * @brief Contains exception class
 */

#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace klib::exception {

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
};

}  // namespace klib::exception
