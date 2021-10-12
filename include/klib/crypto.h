#pragma once

#include <string>

namespace klib {

/**
 * @brief Base64 encode
 * @param data: Data to be encoded
 * @return Base64 result
 */
std::string base64_encode(const std::string &data);

/**
 * @brief Base64 decode
 * @param data: Data to be decoded
 * @return Raw data
 */
std::string base64_decode(const std::string &data);

enum class AesMode { ECB, CBC, CFB, OFB, CTR };

enum class PaddingMode { None, PKCS7, ISO7816_4, ANSI923, ISO10126, ZERO };

/**
 * @brief AES 256-cbc encryption
 * @param data: Data to be encrypted
 * @param key: Key
 * @param iv: iv
 * @return Encrypted data
 */
std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode,
                            PaddingMode padding_mode);

std::string aes_256_encrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv, AesMode aes_mode,
                                   PaddingMode padding_mode);

/**
 * @brief AES 256-cbc decryption
 * @param data: Data to be decrypted
 * @param key: Key
 * @param iv: iv
 * @return Raw data
 */
std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            const std::string &iv, AesMode aes_mode,
                            PaddingMode padding_mode);

std::string aes_256_decrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv, AesMode aes_mode,
                                   PaddingMode padding_mode);

}  // namespace klib
