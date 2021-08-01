#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace klib {

class RuntimeError : public std::runtime_error {
 public:
  explicit RuntimeError(const char *msg) : std::runtime_error(msg) {}

  explicit RuntimeError(const std::string &msg) : std::runtime_error(msg) {}

  explicit RuntimeError(std::string_view msg)
      : std::runtime_error(msg.data()) {}
};

}  // namespace klib
