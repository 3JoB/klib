#include "klib/crypto.h"

#include <cstddef>
#include <cstdint>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <boost/algorithm/string.hpp>
#include <scope_guard.hpp>

#include "klib/detail/openssl_util.h"
#include "klib/exception.h"
#include "klib/util.h"

#define EVP_ENCODE_LENGTH(l) ((((l) + 2) / 3 * 4) + ((l) / 48 + 1) * 2 + 80)
#define EVP_DECODE_LENGTH(l) (((l) + 3) / 4 * 3 + 80)

namespace klib {

namespace {

const EVP_CIPHER *get_algorithm(AesMode aes_mode) {
  switch (aes_mode) {
    case AesMode::CBC:
      return EVP_aes_256_cbc();
    case AesMode::OFB:
      return EVP_aes_256_ofb();
    case AesMode::CTR:
      return EVP_aes_256_ctr();
    default:
      throw LogicError("Unknown algorithm");
  }
}

std::pair<const unsigned char *, std::int32_t> read_string(
    const std::string &str, std::size_t &last_read, std::int32_t n) {
  std::int32_t read_size = 0;
  if (last_read + n > std::size(str)) {
    read_size = std::size(str) - last_read;
  } else {
    read_size = n;
  }

  auto begin = reinterpret_cast<const unsigned char *>(str.data()) + last_read;
  last_read += read_size;

  return {begin, read_size};
}

enum class Crypt { Encrypt, Decrypt };

// https://www.openssl.org/docs/man3.0/man3/EVP_EncryptInit_ex.html
std::string do_aes_crypt(const std::string &data, const std::string &key,
                         const std::string &iv, AesMode aes_mode, bool pad,
                         Crypt crypt) {
  if (std::size(key) != 32) {
    throw klib::RuntimeError("The key must be 256 bit");
  }
  if (std::size(iv) != EVP_MAX_IV_LENGTH && !std::empty(iv)) {
    throw klib::RuntimeError("Initialization vector must be 128 bit");
  }

  auto ctx = EVP_CIPHER_CTX_new();
  SCOPE_EXIT { EVP_CIPHER_CTX_free(ctx); };
  if (!ctx) {
    throw RuntimeError(detail::openssl_err_msg());
  }

  auto do_encrypt = crypt == Crypt::Encrypt ? 1 : 0;
  auto rc = EVP_CipherInit(ctx, get_algorithm(aes_mode), nullptr, nullptr,
                           do_encrypt);
  detail::check_openssl_return_1(rc);

  rc = EVP_CipherInit(
      ctx, nullptr, reinterpret_cast<const unsigned char *>(std::data(key)),
      std::empty(iv) ? nullptr
                     : reinterpret_cast<const unsigned char *>(std::data(iv)),
      do_encrypt);
  detail::check_openssl_return_1(rc);

  rc = EVP_CIPHER_CTX_set_padding(ctx, pad);
  detail::check_openssl_return_1(rc);

  std::string result;
  auto input_size = std::size(data);
  result.resize(input_size + EVP_MAX_BLOCK_LENGTH);

  std::size_t output_len = 0;
  std::int32_t write_len = 0;
  std::size_t last_read = 0;
  while (true) {
    auto [ptr, read_size] = read_string(data, last_read, 102400);
    if (read_size == 0) {
      break;
    }

    rc = EVP_CipherUpdate(
        ctx, reinterpret_cast<unsigned char *>(std::data(result)) + output_len,
        &write_len, ptr, read_size);
    detail::check_openssl_return_1(rc);
    output_len += write_len;
  }

  rc = EVP_CipherFinal(
      ctx, reinterpret_cast<unsigned char *>(std::data(result)) + output_len,
      &write_len);
  detail::check_openssl_return_1(rc);
  output_len += write_len;

  result.resize(output_len);
  return result;
}

std::string do_base64(const std::string &data, Crypt crypt) {
  auto ctx = EVP_ENCODE_CTX_new();
  SCOPE_EXIT { EVP_ENCODE_CTX_free(ctx); };
  if (!ctx) {
    throw RuntimeError(detail::openssl_err_msg());
  }

  std::string result;
  auto input_size = std::size(data);

  if (crypt == Crypt::Encrypt) {
    EVP_EncodeInit(ctx);
    result.resize(EVP_ENCODE_LENGTH(input_size));
  } else {
    EVP_DecodeInit(ctx);
    result.resize(EVP_DECODE_LENGTH(input_size));
  }

  std::size_t output_len = 0;
  std::int32_t write_len = 0;
  std::size_t last_read = 0;
  while (true) {
    auto [ptr, read_size] = read_string(data, last_read, 102400);
    if (read_size == 0) {
      break;
    }

    if (crypt == Crypt::Encrypt) {
      EVP_EncodeUpdate(
          ctx,
          reinterpret_cast<unsigned char *>(std::data(result)) + output_len,
          &write_len, ptr, read_size);
    } else {
      auto rc = EVP_DecodeUpdate(
          ctx,
          reinterpret_cast<unsigned char *>(std::data(result)) + output_len,
          &write_len, ptr, read_size);
      detail::check_openssl_return_1_or_0(rc);
    }

    output_len += write_len;
  }

  if (crypt == Crypt::Encrypt) {
    EVP_EncodeFinal(
        ctx, reinterpret_cast<unsigned char *>(std::data(result)) + output_len,
        &write_len);
    output_len += write_len;
  } else {
    auto rc = EVP_DecodeFinal(
        ctx, reinterpret_cast<unsigned char *>(std::data(result)) + output_len,
        &write_len);
    detail::check_openssl_return_1(rc);
    output_len += write_len;
  }

  result.resize(output_len);
  if (crypt == Crypt::Encrypt) {
    boost::replace_all(result, "\n", "");
  }

  return result;
}

}  // namespace

std::string base64_encode(const std::string &data) {
  return do_base64(data, Crypt::Encrypt);
}

std::string base64_decode(const std::string &data) {
  return do_base64(data, Crypt::Decrypt);
}

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            bool use_iv, AesMode aes_mode, bool pad) {
  std::string iv;
  if (use_iv) {
    iv = generate_random_bytes(EVP_MAX_IV_LENGTH);
  }

  return iv + do_aes_crypt(data, key, iv, aes_mode, pad, Crypt::Encrypt);
}

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode, bool pad) {
  return do_aes_crypt(data, key, iv, aes_mode, pad, Crypt::Encrypt);
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
                        data.substr(0, AES_BLOCK_SIZE), aes_mode, pad,
                        Crypt::Decrypt);
  } else {
    return do_aes_crypt(data, key, "", aes_mode, pad, Crypt::Decrypt);
  }
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode, bool pad) {
  return do_aes_crypt(data, key, iv, aes_mode, pad, Crypt::Decrypt);
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
