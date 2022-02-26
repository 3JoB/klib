#pragma once

#include <openssl/base.h>
#include <openssl/err.h>

#include "klib/exception.h"

#ifndef OPENSSL_IS_BORINGSSL
#error "OpenSSL is not BoringSSL"
#endif

#define CHECK_BORINGSSL(rc)                                           \
  do {                                                                \
    if (rc != 1) [[unlikely]] {                                       \
      throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr)); \
    }                                                                 \
  } while (0)
