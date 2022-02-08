/**
 * @file hash.h
 * @brief Contains hash module
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace klib {

/**
 * @brief Calculate the hash
 * @param data: Data to be hashed
 * @return Hash result
 */
std::size_t fast_hash(const std::string &data);
std::string fast_hash_hex(const std::string &data);

/**
 * @brief Calculate the hash
 * @param data: Data to be hashed
 * @return Hash result
 */
std::string md5(const std::string &data);
std::string md5_hex(const std::string &data);
std::string sha224(const std::string &data);
std::string sha224_hex(const std::string &data);
std::string sha256(const std::string &data);
std::string sha256_hex(const std::string &data);
std::string sha384(const std::string &data);
std::string sha384_hex(const std::string &data);
std::string sha512(const std::string &data);
std::string sha512_hex(const std::string &data);

/**
 * @brief Hashes a password
 * @param password: The password to be hashed
 * @param time_cost: Number of iterations
 * @param memory_cost: Sets memory usage to memory_cost KB
 * @param parallelism: Number of threads and compute lanes
 * @param hash_len: Desired length of the hash in bytes
 * @param salt_len: Salt size in bytes
 * @return Raw hash and salt
 * @see https://github.com/P-H-C/phc-winner-argon2/issues/235
 * @see https://www.tomczhen.com/2016/10/10/hashing-security/
 */
std::pair<std::string, std::string> password_hash_raw(
    const std::string &password, std::uint32_t time_cost = 1,
    std::uint32_t memory_cost = 4 * 1024 * 1024, std::uint32_t parallelism = 16,
    std::int32_t hash_len = 32, std::int32_t salt_len = 32);

/**
 * @brief Hashes a password
 * @param password: The password to be hashed
 * @param salt: Salt
 * @param time_cost: Number of iterations
 * @param memory_cost: Sets memory usage to memory_cost KB
 * @param parallelism: Number of threads and compute lanes
 * @param hash_len: Desired length of the hash in bytes
 * @return Raw hash
 * @see https://github.com/P-H-C/phc-winner-argon2/issues/235
 * @see https://www.tomczhen.com/2016/10/10/hashing-security/
 */
std::string password_hash_raw(const std::string &password,
                              const std::string &salt,
                              std::uint32_t time_cost = 1,
                              std::uint32_t memory_cost = 4 * 1024 * 1024,
                              std::uint32_t parallelism = 16,
                              std::int32_t hash_len = 32);

/**
 * @brief Hashes a password, producing an encoded hash
 * @param password: The password to be hashed
 * @param time_cost: Number of iterations
 * @param memory_cost: Sets memory usage to memory_cost KB
 * @param parallelism: Number of threads and compute lanes
 * @param hash_len: Desired length of the hash in bytes
 * @param salt_len: Salt size in bytes
 * @return Encoded hash
 * @see https://github.com/P-H-C/phc-winner-argon2/issues/235
 * @see https://www.tomczhen.com/2016/10/10/hashing-security/
 */
std::string password_hash_encoded(const std::string &password,
                                  std::uint32_t time_cost = 1,
                                  std::uint32_t memory_cost = 4 * 1024 * 1024,
                                  std::uint32_t parallelism = 16,
                                  std::int32_t hash_len = 32,
                                  std::int32_t salt_len = 32);

/**
 * @brief Verifies a password against an encoded string
 * @param password: The password to be verified
 * @param encoded: Encoded hash
 * @return Return true if it is verified to be valid
 */
bool password_verify(const std::string &password, const std::string &encoded);

/**
 * @brief Verifies a password against raw hash and salt
 * @param password: The password to be verified
 * @param hash: Raw hash
 * @param salt: Raw salt
 * @param time_cost: Number of iterations
 * @param memory_cost: Sets memory usage to memory_cost KB
 * @param parallelism: Number of threads and compute lanes
 * @return Return true if it is verified to be valid
 */
bool password_verify(const std::string &password, const std::string &hash,
                     const std::string &salt, std::uint32_t time_cost = 1,
                     std::uint32_t memory_cost = 4 * 1024 * 1024,
                     std::uint32_t parallelism = 16);

}  // namespace klib
