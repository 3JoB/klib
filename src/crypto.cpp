/**
 * @see
 * https://github.com/google/boringssl/blob/master-with-bazel/src/crypto/cipher_extra/cipher_test.cc
 */

#include "klib/crypto.h"

#include <cstddef>
#include <cstdint>
#include <span>

#include <openssl/aes.h>
#include <openssl/cipher.h>
#include <scope_guard.hpp>

#include "klib/detail/openssl_util.h"
#include "klib/exception.h"
#include "klib/util.h"

namespace klib {

namespace {

const EVP_CIPHER *get_cipher(AesMode aes_mode) {
  switch (aes_mode) {
    case AesMode::CBC:
      return EVP_aes_256_cbc();
    case AesMode::OFB:
      return EVP_aes_256_ofb();
    case AesMode::CTR:
      return EVP_aes_256_ctr();
    default:
      throw LogicError("Unknown AES mode");
  }
}

std::string do_aes_crypt(std::span<const char> data, const std::string &key,
                         const std::string &iv, AesMode aes_mode,
                         bool encrypt) {
  if (std::size(key) != 32) [[unlikely]] {
    throw LogicError("The key must be 256 bit");
  }

  auto ctx = EVP_CIPHER_CTX_new();
  SCOPE_EXIT { EVP_CIPHER_CTX_free(ctx); };
  if (!ctx) [[unlikely]] {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  auto rc = EVP_CipherInit_ex(
      ctx, get_cipher(aes_mode), nullptr,
      reinterpret_cast<const unsigned char *>(std::data(key)),
      std::empty(iv) ? nullptr
                     : reinterpret_cast<const unsigned char *>(std::data(iv)),
      encrypt);
  CHECK_BORINGSSL(rc);

  std::string result;
  auto input_size = std::size(data);
  std::size_t max_len = input_size;
  if (encrypt) {
    auto block_size = EVP_CIPHER_CTX_block_size(ctx);
    max_len += block_size - (max_len % block_size);
  }
  result.resize(max_len);

  std::size_t total = 0;
  std::int32_t length;
  while (!data.empty()) {
    std::int32_t todo = std::min(data.size(), 16384UL);

    rc = EVP_CipherUpdate(
        ctx, reinterpret_cast<unsigned char *>(std::data(result)) + total,
        &length, reinterpret_cast<const std::uint8_t *>(std::data(data)), todo);
    CHECK_BORINGSSL(rc);

    total += length;
    data = data.subspan(todo);
  }

  rc = EVP_CipherFinal_ex(
      ctx, reinterpret_cast<unsigned char *>(std::data(result)) + total,
      &length);
  CHECK_BORINGSSL(rc);

  total += length;
  result.resize(total);

  return result;
}

}  // namespace

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            AesMode aes_mode) {
  const auto iv = generate_random_bytes(EVP_MAX_IV_LENGTH);
  return iv + do_aes_crypt(data, key, iv, aes_mode, true);
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            AesMode aes_mode) {
  return do_aes_crypt(data.substr(AES_BLOCK_SIZE), key,
                      data.substr(0, AES_BLOCK_SIZE), aes_mode, false);
}

std::string aes_256_decrypt_no_iv(const std::string &data,
                                  const std::string &key, AesMode aes_mode) {
  return do_aes_crypt(data, key, "", aes_mode, false);
}

}  // namespace klib
