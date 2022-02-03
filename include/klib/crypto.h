/**
 * @file crypto.h
 * @brief Contains encrypt and decrypt modules
 */

#pragma once

#include <string>

namespace klib {

/**
 * @brief Block cipher mode of operation
 * @see https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 */
enum class AesMode { CBC, OFB, CTR };

/**
 * @brief Use AES to encrypt bytes, key size: 256 bit
 * @param data: Data to be encrypted
 * @param key: Encryption/decryption key
 * @param use_iv: Whether to use initialization vector, if used, it will be
 * randomly generated and appended to the ciphertext before
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Encrypted bytes
 */
std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            bool use_iv = true, AesMode aes_mode = AesMode::CBC,
                            bool pad = true);

/**
 * @brief Use AES to encrypt bytes, key size: 256 bit
 * @param data: Data to be encrypted
 * @param key: Encryption/decryption key
 * @param iv: Initialization vector
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Encrypted bytes
 */
std::string aes_256_encrypt(const std::string &data, const std::string &key,
                            const std::string &iv,
                            AesMode aes_mode = AesMode::CBC, bool pad = true);

/**
 * @brief Use AES to encrypt bytes, key size: 256 bit
 * @param data: Data to be encrypted
 * @param key: Encryption/decryption key
 * @param use_iv: Whether to use initialization vector, if used, it will be
 * randomly generated and appended to the ciphertext before
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Encrypted data encoded with Base64
 */
std::string aes_256_encrypt_base64(const std::string &data,
                                   const std::string &key, bool use_iv = true,
                                   AesMode aes_mode = AesMode::CBC,
                                   bool pad = true);

/**
 * @brief Use AES to encrypt bytes, key size: 256 bit
 * @param data: Data to be encrypted
 * @param key: Encryption/decryption key
 * @param iv: Initialization vector
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Encrypted bytes encoded with Base64
 */
std::string aes_256_encrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv,
                                   AesMode aes_mode = AesMode::CBC,
                                   bool pad = true);

/**
 * @brief Use AES to decrypt bytes, key size: 256 bit
 * @param data: Encrypted bytes
 * @param key: Encryption/decryption key
 * @param has_iv: Whether the ciphertext contains initialization vector
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Decrypted bytes
 */
std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            bool has_iv = true, AesMode aes_mode = AesMode::CBC,
                            bool pad = true);

/**
 * @brief Use AES to decrypt bytes, key size: 256 bit
 * @param data: Encrypted bytes
 * @param key: Encryption/decryption key
 * @param iv: Initialization vector
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Decrypted bytes
 */
std::string aes_256_decrypt(const std::string &data, const std::string &key,
                            const std::string &iv,
                            AesMode aes_mode = AesMode::CBC, bool pad = true);

/**
 * @brief Use AES to decrypt bytes, key size: 256 bit
 * @param data: Encrypted bytes encoded with Base64
 * @param key: Encryption/decryption key
 * @param has_iv: Whether the ciphertext contains initialization vector
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Decrypted bytes
 */
std::string aes_256_decrypt_base64(const std::string &data,
                                   const std::string &key, bool has_iv = true,
                                   AesMode aes_mode = AesMode::CBC,
                                   bool pad = true);

/**
 * @brief Use AES to decrypt bytes, key size: 256 bit
 * @param data: Encrypted bytes encoded with Base64
 * @param key: Encryption/decryption key
 * @param iv: Initialization vector
 * @param aes_mode: Block cipher mode of operation
 * @param padding_mode: Padding mode
 * @return Decrypted bytes
 */
std::string aes_256_decrypt_base64(const std::string &data,
                                   const std::string &key,
                                   const std::string &iv,
                                   AesMode aes_mode = AesMode::CBC,
                                   bool pad = true);

}  // namespace klib
