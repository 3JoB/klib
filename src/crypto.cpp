#include "klib/crypto.h"

#include <cstddef>
#include <cstdint>

#include <openssl/aes.h>
#include <openssl/base64.h>
#include <openssl/evp.h>
#include <openssl/span.h>

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
      throw LogicError("Unknown mode");
  }
}

// https://www.openssl.org/docs/man3.0/man3/EVP_EncryptInit_ex.html
std::string do_aes_crypt(bssl::Span<const char> data, const std::string &key,
                         const std::string &iv, AesMode aes_mode, bool pad,
                         bool encrypt) {
  bssl::ScopedEVP_CIPHER_CTX ctx;

  auto rc = EVP_CipherInit_ex(ctx.get(), get_cipher(aes_mode), nullptr, nullptr,
                              nullptr, encrypt);
  detail::check_openssl_return_1(rc);

  if (std::size(key) != EVP_CIPHER_CTX_key_length(ctx.get())) {
    throw klib::LogicError("Wrong key length");
  }
  if (!std::empty(iv)) {
    if (std::size(iv) != EVP_CIPHER_CTX_iv_length(ctx.get())) {
      throw klib::LogicError("Wrong initial vector length");
    }
  }

  rc = EVP_CipherInit_ex(
      ctx.get(), nullptr, nullptr,
      reinterpret_cast<const unsigned char *>(std::data(key)),
      std::empty(iv) ? nullptr
                     : reinterpret_cast<const unsigned char *>(std::data(iv)),
      encrypt);
  detail::check_openssl_return_1(rc);

  rc = EVP_CIPHER_CTX_set_padding(ctx.get(), pad);
  detail::check_openssl_return_1(rc);

  std::string result;
  auto data_size = std::size(data);
  std::size_t max_len = data_size;
  if ((EVP_CIPHER_CTX_flags(ctx.get()) & EVP_CIPH_NO_PADDING) == 0 && encrypt) {
    auto block_size = EVP_CIPHER_CTX_block_size(ctx.get());
    max_len += block_size - (max_len % block_size);
  }
  result.resize(max_len);

  std::size_t total = 0;
  std::int32_t len;
  while (!data.empty()) {
    auto todo = data_size;
    rc = EVP_CipherUpdate(
        ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)) + total,
        &len, reinterpret_cast<const std::uint8_t *>(data.data()),
        static_cast<int>(todo));
    detail::check_openssl_return_1(rc);

    total += static_cast<std::size_t>(len);
    data = data.subspan(todo);
  }

  rc = EVP_CipherFinal_ex(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)) + total,
      &len);
  detail::check_openssl_return_1(rc);

  total += static_cast<size_t>(len);
  result.resize(total);

  return result;
}

}  // namespace

std::string base64_encode(const std::string &data) {
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

std::string base64_decode(const std::string &data) {
  std::size_t max_len;
  const auto data_size = std::size(data);
  EVP_DecodedLength(&max_len, data_size);

  std::string result;
  result.resize(max_len);

  std::size_t len;
  auto rc = EVP_DecodeBase64(
      reinterpret_cast<std::uint8_t *>(std::data(result)), &len, max_len,
      reinterpret_cast<const std::uint8_t *>(std::data(data)), data_size);
  detail::check_openssl_return_1(rc);

  result.resize(len);
  return result;
}

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            bool use_iv, AesMode aes_mode, bool pad) {
  std::string iv;
  if (use_iv) {
    iv = generate_random_bytes(EVP_MAX_IV_LENGTH);
  }

  return iv + do_aes_crypt(data, key, iv, aes_mode, pad, true);
}

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode, bool pad) {
  return do_aes_crypt(data, key, iv, aes_mode, pad, true);
}

std::string aes_256_encrypt_base64(const std::string &data,
                                   const std::string &key, bool use_iv,
                                   AesMode aes_mode, bool pad) {
  return base64_encode(aes_256_encrypt(data, key, use_iv, aes_mode, pad));
}

std::string aes_256_encrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv, AesMode aes_mode,
                                   bool pad) {
  return base64_encode(aes_256_encrypt(data, key, iv, aes_mode, pad));
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            bool has_iv, AesMode aes_mode, bool pad) {
  if (has_iv) {
    return do_aes_crypt(data.substr(AES_BLOCK_SIZE), key,
                        data.substr(0, AES_BLOCK_SIZE), aes_mode, pad, false);
  } else {
    return do_aes_crypt(data, key, "", aes_mode, pad, false);
  }
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode, bool pad) {
  return do_aes_crypt(data, key, iv, aes_mode, pad, false);
}

std::string aes_256_decrypt_base64(const std::string &data,
                                   const std::string &key, bool has_iv,
                                   AesMode aes_mode, bool pad) {
  return aes_256_decrypt(base64_decode(data), key, has_iv, aes_mode, pad);
}

std::string aes_256_decrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv, AesMode aes_mode,
                                   bool pad) {
  return aes_256_decrypt(base64_decode(data), key, iv, aes_mode, pad);
}

}  // namespace klib
