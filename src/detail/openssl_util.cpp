#include "klib/detail/openssl_util.h"

#include <openssl/err.h>

#include "klib/exception.h"

namespace klib::detail {

std::string openssl_err_msg() {
  return ERR_error_string(ERR_get_error(), nullptr);
}

void check_openssl_return(std::int32_t rc) {
  if (rc != 1) {
    throw RuntimeError(openssl_err_msg());
  }
}

}  // namespace klib::detail
