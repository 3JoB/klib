#include "klib/crypto.h"

#include <cstddef>
#include <memory>

#include <openssl/aes.h>
#include <openssl/evp.h>

#include "klib/detail/openssl_util.h"
#include "klib/exception.h"

namespace klib {

namespace {

const EVP_CIPHER *get_algorithm(AesMode aes_mode) {
  switch (aes_mode) {
    case AesMode::CBC:
      return EVP_aes_256_cbc();
    case AesMode::OFB:
      return EVP_aes_256_ofb();
    case AesMode::CFB:
      return EVP_aes_256_cfb();
    case AesMode::CTR:
      return EVP_aes_256_ctr();
    case AesMode::XTS:
      return EVP_aes_256_xts();
    default:
      throw LogicError("Unknown algorithm");
  }
}

std::int32_t get_padding_mode(PaddingMode padding_mode) {
  switch (padding_mode) {
    case PaddingMode::None:
      return 0;
    case PaddingMode::PKCS7:
      return EVP_PADDING_PKCS7;
    case PaddingMode::ISO7816_4:
      return EVP_PADDING_ISO7816_4;
    case PaddingMode::ANSI923:
      return EVP_PADDING_ANSI923;
    case PaddingMode::ISO10126:
      return EVP_PADDING_ISO10126;
    case PaddingMode::ZERO:
      return EVP_PADDING_ZERO;
    default:
      throw LogicError("Unknown padding mode");
  }
}

enum class Crypt { Encrypt, Decrypt };

// https://www.openssl.org/docs/man3.0/man3/EVP_EncryptInit_ex.html
std::string do_aes_crypt(const std::string &data, const std::string &key,
                         const std::string &iv, AesMode aes_mode,
                         PaddingMode padding_mode, Crypt crypt) {
  if (std::size(key) != 32) {
    throw klib::RuntimeError("key must be 256 bit");
  }
  if (std::size(iv) != EVP_MAX_IV_LENGTH && !std::empty(iv)) {
    throw klib::RuntimeError("iv must be 128 bit");
  }

  std::unique_ptr<EVP_CIPHER_CTX, decltype(EVP_CIPHER_CTX_free) *> ctx(
      EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
  if (!ctx) {
    throw RuntimeError(detail::openssl_err_msg());
  }

  auto do_encrypt = crypt == Crypt::Encrypt ? 1 : 0;
  auto rc = EVP_CipherInit(ctx.get(), get_algorithm(aes_mode), nullptr, nullptr,
                           do_encrypt);
  detail::check_openssl_return_value(rc);
  OPENSSL_assert(EVP_CIPHER_CTX_get_key_length(ctx.get()) == 32);
  OPENSSL_assert(EVP_CIPHER_CTX_get_iv_length(ctx.get()) == EVP_MAX_IV_LENGTH);

  rc = EVP_CipherInit(
      ctx.get(), nullptr,
      reinterpret_cast<const unsigned char *>(std::data(key)),
      std::empty(iv) ? nullptr
                     : reinterpret_cast<const unsigned char *>(std::data(iv)),
      do_encrypt);
  detail::check_openssl_return_value(rc);

  if (auto padding_mode_num = get_padding_mode(padding_mode);
      padding_mode_num != 0) {
    rc = EVP_CIPHER_CTX_set_padding(ctx.get(), padding_mode_num);
    detail::check_openssl_return_value(rc);
  }

  std::string result;
  auto input_size = std::size(data);
  result.resize(input_size + EVP_MAX_BLOCK_LENGTH);

  std::int32_t chunk_len = 0;
  rc = EVP_CipherUpdate(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)),
      &chunk_len, reinterpret_cast<const unsigned char *>(std::data(data)),
      input_size);
  detail::check_openssl_return_value(rc);

  std::int32_t output_len = chunk_len;
  rc = EVP_CipherFinal(
      ctx.get(),
      reinterpret_cast<unsigned char *>(std::data(result)) + chunk_len,
      &chunk_len);
  detail::check_openssl_return_value(rc);
  output_len += chunk_len;

  result.resize(output_len);
  return result;
}

std::string do_base64(const std::string &data, Crypt crypt) {
  decltype(EVP_EncodeBlock) *func;

  auto input_size = std::size(data);
  std::size_t size;
  if (crypt == Crypt::Decrypt) {
    func = EVP_DecodeBlock;
    size = EVP_DECODE_LENGTH(input_size);
  } else {
    func = EVP_EncodeBlock;
    size = EVP_ENCODE_LENGTH(input_size);
  }

  std::string result;
  result.resize(size);

  auto output_size =
      func(reinterpret_cast<unsigned char *>(std::data(result)),
           reinterpret_cast<const unsigned char *>(std::data(data)),
           static_cast<std::int32_t>(input_size));

  // https://gist.github.com/cameronehrlich/6f77a4982f141ee516fe52242eb803db
  if (crypt == Crypt::Decrypt) {
    if (data[input_size - 2] == '=') {
      output_size -= 2;
    } else if (data[input_size - 1] == '=') {
      output_size -= 1;
    }
  }

  result.resize(output_size);
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
                            bool use_iv, AesMode aes_mode,
                            PaddingMode padding_mode) {
  std::string iv;
  if (use_iv) {
    iv = detail::generate_random_bytes(EVP_MAX_IV_LENGTH);
  }

  return iv +
         do_aes_crypt(data, key, iv, aes_mode, padding_mode, Crypt::Encrypt);
}

std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode,
                            PaddingMode padding_mode) {
  return do_aes_crypt(data, key, iv, aes_mode, padding_mode, Crypt::Encrypt);
}

std::string aes_256_encrypt_base64(const std::string &data,
                                   const std::string &key, bool use_iv,
                                   AesMode aes_mode, PaddingMode padding_mode) {
  return base64_encode(
      aes_256_encrypt(data, key, use_iv, aes_mode, padding_mode));
}

std::string aes_256_encrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv, AesMode aes_mode,
                                   PaddingMode padding_mode) {
  return base64_encode(aes_256_encrypt(data, key, iv, aes_mode, padding_mode));
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            bool has_iv, AesMode aes_mode,
                            PaddingMode padding_mode) {
  if (has_iv) {
    return do_aes_crypt(data.substr(AES_BLOCK_SIZE), key,
                        data.substr(0, AES_BLOCK_SIZE), aes_mode, padding_mode,
                        Crypt::Decrypt);
  } else {
    return do_aes_crypt(data, key, "", aes_mode, padding_mode, Crypt::Decrypt);
  }
}

std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode,
                            PaddingMode padding_mode) {
  return do_aes_crypt(data, key, iv, aes_mode, padding_mode, Crypt::Decrypt);
}

std::string aes_256_decrypt_base64(const std::string &data,
                                   const std::string &key, bool has_iv,
                                   AesMode aes_mode, PaddingMode padding_mode) {
  return aes_256_decrypt(base64_decode(data), key, has_iv, aes_mode,
                         padding_mode);
}

std::string aes_256_decrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv, AesMode aes_mode,
                                   PaddingMode padding_mode) {
  return aes_256_decrypt(base64_decode(data), key, iv, aes_mode, padding_mode);
}

}  // namespace klib
