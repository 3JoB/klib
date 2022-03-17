/**
 * @see
 * https://github.com/google/boringssl/blob/master-with-bazel/src/crypto/cipher_extra/cipher_test.cc
 */

#include "klib/crypto.h"

#include <cstddef>
#include <cstdint>
#include <span>

#include <openssl/cipher.h>
#include <gsl/assert>
#include <scope_guard.hpp>

#include "klib/detail/boringssl_util.h"
#include "klib/exception.h"
#include "klib/util.h"

namespace klib {

namespace {

constexpr std::size_t gcm_iv_size = 12;
constexpr std::size_t gcm_tag_size = 16;
constexpr std::size_t iv_size = 16;
const std::string empty;

const EVP_CIPHER *get_cipher(AesMode aes_mode) {
  switch (aes_mode) {
    case AesMode::GCM:
      return EVP_aes_256_gcm();
    case AesMode::CBC:
      return EVP_aes_256_cbc();
    default:
      throw LogicError("Unknown AES mode");
  }
}

std::pair<std::string, std::string> aes_256_crypt(std::span<const char> data,
                                                  std::span<const char> key,
                                                  std::span<const char> iv,
                                                  std::span<const char> tag,
                                                  AesMode aes_mode,
                                                  bool encrypt) {
  auto ctx = EVP_CIPHER_CTX_new();
  SCOPE_EXIT { EVP_CIPHER_CTX_free(ctx); };
  if (!ctx) [[unlikely]] {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  auto rc = EVP_CipherInit_ex(ctx, get_cipher(aes_mode), nullptr, nullptr,
                              nullptr, encrypt ? 1 : 0);
  CHECK_BORINGSSL(rc);

  bool is_aead = (aes_mode == AesMode::GCM);
  if (is_aead) {
    rc = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, std::size(iv),
                             nullptr);
    CHECK_BORINGSSL(rc);
  } else {
    auto size = std::size(iv);
    if (EVP_CIPHER_CTX_iv_length(ctx) != size && size != 0) {
      throw RuntimeError("Wrong initial vector length: {}", size);
    }
  }

  auto tag_ptr = const_cast<void *>(static_cast<const void *>(std::data(tag)));
  if (is_aead && !encrypt) {
    Expects(!std::empty(tag));
    rc = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, std::size(tag),
                             tag_ptr);
    CHECK_BORINGSSL(rc);
  }

  rc = EVP_CIPHER_CTX_set_key_length(ctx, std::size(key));
  CHECK_BORINGSSL(rc);

  rc = EVP_CipherInit_ex(
      ctx, nullptr, nullptr,
      reinterpret_cast<const std::uint8_t *>(std::data(key)),
      std::empty(iv) ? nullptr
                     : reinterpret_cast<const std::uint8_t *>(std::data(iv)),
      -1);
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

  std::string new_tag;
  if (encrypt && is_aead) {
    new_tag.resize(gcm_tag_size);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, gcm_tag_size,
                        static_cast<void *>(std::data(new_tag)));
  }

  return {result, new_tag};
}

}  // namespace

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            AesMode aes_mode) {
  std::string iv;
  if (aes_mode == AesMode::GCM) {
    iv = generate_random_bytes(gcm_iv_size);
  } else {
    iv = generate_random_bytes(iv_size);
  }

  auto result = aes_256_crypt(data, key, iv, empty, aes_mode, true);
  return iv + result.first + result.second;
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            AesMode aes_mode) {
  auto data_ptr = std::data(data);

  if (aes_mode == AesMode::GCM) {
    auto data_size = std::size(data) - gcm_iv_size - gcm_tag_size;

    return aes_256_crypt(
               std::span<const char>(data_ptr + gcm_iv_size, data_size), key,
               std::span<const char>(data_ptr, gcm_iv_size),
               std::span<const char>(data_ptr + gcm_iv_size + data_size,
                                     gcm_tag_size),
               aes_mode, false)
        .first;
  } else {
    auto data_size = std::size(data) - iv_size;

    return aes_256_crypt(std::span<const char>(data_ptr + iv_size, data_size),
                         key, std::span<const char>(data_ptr, iv_size), empty,
                         aes_mode, false)
        .first;
  }
}

std::string aes_256_cbc_decrypt_no_iv(const std::string &data,
                                      const std::string &key) {
  return aes_256_crypt(data, key, empty, empty, AesMode::CBC, false).first;
}

}  // namespace klib
