#include "klib/base64.h"

#include <cstddef>
#include <cstdint>

#include <openssl/base64.h>

#include "fastbase64/fastavxbase64.h"
#include "klib/detail/openssl_util.h"

namespace klib {

std::string fast_base64_encode(const std::string &data) {
  const auto data_size = std::size(data);

  std::string result;
  result.resize(modp_b64_encode_len(data_size));

  auto len =
      fast_avx2_base64_encode(std::data(result), std::data(data), data_size);

  result.resize(len);
  return result;
}

std::string fast_base64_decode(const std::string &data) {
  const auto data_size = std::size(data);

  std::string result;
  result.resize(modp_b64_decode_len(data_size));

  auto len =
      fast_avx2_base64_decode(std::data(result), std::data(data), data_size);

  result.resize(len);
  return result;
}

std::string secure_base64_encode(const std::string &data) {
  std::size_t max_len;
  const auto data_size = std::size(data);
  EVP_EncodedLength(&max_len, data_size);

  std::string result;
  result.resize(max_len);

  const auto len = EVP_EncodeBlock(
      reinterpret_cast<std::uint8_t *>(std::data(result)),
      reinterpret_cast<const std::uint8_t *>(std::data(data)), data_size);

  result.resize(len);
  return result;
}

std::string secure_base64_decode(const std::string &data) {
  std::size_t max_len;
  const auto data_size = std::size(data);
  EVP_DecodedLength(&max_len, data_size);

  std::string result;
  result.resize(max_len);

  std::size_t len;
  auto rc = EVP_DecodeBase64(
      reinterpret_cast<std::uint8_t *>(std::data(result)), &len, max_len,
      reinterpret_cast<const std::uint8_t *>(std::data(data)), data_size);
  check_openssl_return(rc);

  result.resize(len);
  return result;
}

}  // namespace klib
