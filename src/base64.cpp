/**
 * @see https://github.com/chromium/chromium/blob/main/base/base64.cc
 * @see
 * https://github.com/google/boringssl/blob/master-with-bazel/src/crypto/base64/base64_test.cc
 */

#include "klib/base64.h"

#include <cstddef>

#include <openssl/base64.h>

#include "fastbase64/fastavxbase64.h"
#include "klib/detail/openssl_util.h"
#include "klib/exception.h"

namespace klib {

std::string fast_base64_encode(const std::string &data) {
  const auto input_size = std::size(data);

  std::string result;
  result.resize(modp_b64_encode_len(input_size));

  auto length =
      fast_avx2_base64_encode(std::data(result), std::data(data), input_size);
  if (length == MODP_B64_ERROR) [[unlikely]] {
    throw RuntimeError("fast_avx2_base64_encode failed");
  }

  result.resize(length);
  return result;
}

std::string fast_base64_decode(const std::string &data) {
  const auto input_size = std::size(data);

  std::string result;
  result.resize(modp_b64_decode_len(input_size));

  auto length =
      fast_avx2_base64_decode(std::data(result), std::data(data), input_size);
  if (length == MODP_B64_ERROR) [[unlikely]] {
    throw RuntimeError("fast_avx2_base64_decode failed");
  }

  result.resize(length);
  return result;
}

std::string secure_base64_encode(const std::string &data) {
  std::size_t max_len;
  const auto input_size = std::size(data);
  EVP_EncodedLength(&max_len, input_size);

  std::string result;
  result.resize(max_len);

  const auto length = EVP_EncodeBlock(
      reinterpret_cast<std::uint8_t *>(std::data(result)),
      reinterpret_cast<const std::uint8_t *>(std::data(data)), input_size);

  result.resize(length);
  return result;
}

std::string secure_base64_decode(const std::string &data) {
  std::size_t max_len;
  const auto input_size = std::size(data);
  EVP_DecodedLength(&max_len, input_size);

  std::string result;
  result.resize(max_len);

  std::size_t length;
  auto rc = EVP_DecodeBase64(
      reinterpret_cast<std::uint8_t *>(std::data(result)), &length, max_len,
      reinterpret_cast<const std::uint8_t *>(std::data(data)), input_size);
  CHECK_BORINGSSL(rc);

  result.resize(length);
  return result;
}

}  // namespace klib
