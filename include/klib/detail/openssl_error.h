#pragma once

#include <cstdint>
#include <string>

#include <openssl/err.h>

#include "klib/exception.h"

namespace klib::detail {

inline std::string openssl_err_msg() {
  return ERR_error_string(ERR_get_error(), nullptr);
}

inline void check_openssl_return_value(std::int32_t rc) {
  if (rc != 1) {
    throw RuntimeError(openssl_err_msg());
  }
}

}  // namespace klib::detail