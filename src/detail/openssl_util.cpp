#include "klib/detail/openssl_util.h"

#include <openssl/err.h>
#include <openssl/rand.h>

#include "klib/exception.h"

namespace klib::detail {

std::string openssl_err_msg() {
  return ERR_error_string(ERR_get_error(), nullptr);
}

void check_openssl_return_value(std::int32_t rc) {
  if (rc != 1) {
    throw RuntimeError(openssl_err_msg());
  }
}

std::string generate_random_bytes(std::int32_t num) {
  if (RAND_status() == 0) {
    check_openssl_return_value(RAND_poll());
  }

  std::string bytes;
  bytes.resize(num);
  check_openssl_return_value(RAND_bytes(
      reinterpret_cast<unsigned char *>(std::data(bytes)), std::size(bytes)));

  return bytes;
}

}  // namespace klib::detail
