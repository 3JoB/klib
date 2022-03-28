/**
 * @file crypto.h
 * @brief Contains encrypt and decrypt module
 */

#pragma once

#include <string>

namespace klib {

/**
 * @brief Block cipher mode of operation
 * @see https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 */
enum class AesMode { GCM, CBC };

/**
 * @brief Use AES to encrypt data, key size: 256 bit
 * @param data: Data to be encrypted
 * @param key: Encryption/decryption key
 * @param aes_mode: Block cipher mode of operation
 * @param aad: Additional authenticated data
 * @return Encrypted data
 */
std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            AesMode aes_mode = AesMode::GCM,
                            const std::string &aad = "");

/**
 * @brief Use AES to decrypt data, key size: 256 bit
 * @param data: Encrypted bytes
 * @param key: Encryption/decryption key
 * @param aes_mode: Block cipher mode of operation
 * @param aad: Additional authenticated data
 * @return Decrypted data
 */
std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            AesMode aes_mode = AesMode::GCM,
                            const std::string &aad = "");

/**
 * @brief Use AES to decrypt data, key size: 256 bit
 * @param data: Encrypted bytes
 * @param key: Encryption/decryption key
 * @return Decrypted data
 * @note It is not safe to not use the initial vector, this function is only
 * used to decrypt the foreign encrypted data
 */
std::string aes_256_cbc_decrypt_no_iv(const std::string &data,
                                      const std::string &key);

}  // namespace klib
