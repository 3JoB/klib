#include "klib/crypto.h"

#include <cstddef>
#include <cstdint>

#include <openssl/aes.h>
#include <openssl/cipher.h>
#include <openssl/span.h>

#include "klib/base64.h"
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

// https://www.openssl.org/docs/man3.0/man3/EVP_EncryptInit_ex.html
std::string do_aes_crypt(bssl::Span<const char> data, const std::string &key,
                         const std::string &iv, AesMode aes_mode,
                         bool encrypt) {
  bssl::ScopedEVP_CIPHER_CTX ctx;

  auto rc = EVP_CipherInit_ex(ctx.get(), get_cipher(aes_mode), nullptr, nullptr,
                              nullptr, encrypt);
  check_openssl_return(rc);

  if (std::size(key) != EVP_CIPHER_CTX_key_length(ctx.get())) {
    throw LogicError("Wrong key length");
  }

  rc = EVP_CipherInit_ex(
      ctx.get(), nullptr, nullptr,
      reinterpret_cast<const unsigned char *>(std::data(key)),
      std::empty(iv) ? nullptr
                     : reinterpret_cast<const unsigned char *>(std::data(iv)),
      encrypt);
  check_openssl_return(rc);

  std::string result;
  auto data_size = std::size(data);
  std::size_t max_len = data_size;
  if (encrypt) {
    auto block_size = EVP_CIPHER_CTX_block_size(ctx.get());
    max_len += block_size - (max_len % block_size);
  }
  result.resize(max_len);

  std::size_t total = 0;
  std::int32_t len;
  while (!data.empty()) {
    std::int32_t chunk = std::min(data.size(), 102400UL);

    rc = EVP_CipherUpdate(
        ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)) + total,
        &len, reinterpret_cast<const std::uint8_t *>(data.data()), chunk);
    check_openssl_return(rc);

    total += len;
    data = data.subspan(chunk);
  }

  rc = EVP_CipherFinal_ex(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)) + total,
      &len);
  check_openssl_return(rc);

  total += len;
  result.resize(total);

  return result;
}

}  // namespace

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            bool use_iv, AesMode aes_mode) {
  std::string iv;
  if (use_iv) {
    iv = generate_random_bytes(EVP_MAX_IV_LENGTH);
  }

  return iv + do_aes_crypt(data, key, iv, aes_mode, true);
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            bool has_iv, AesMode aes_mode) {
  if (has_iv) {
    return do_aes_crypt(data.substr(AES_BLOCK_SIZE), key,
                        data.substr(0, AES_BLOCK_SIZE), aes_mode, false);
  } else {
    return do_aes_crypt(data, key, "", aes_mode, false);
  }
}

}  // namespace klib
