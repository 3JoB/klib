/**
 * @file hash.h
 * @brief Contains hash module
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <utility>

namespace klib {

/**
 * @brief Contains fast hashing algorithm
 */
class FastHash {
 public:
  /**
   * @brief Constructor
   */
  explicit FastHash();

  FastHash(const FastHash &) = delete;
  FastHash(FastHash &&) = delete;
  FastHash &operator=(const FastHash &) = delete;
  FastHash &operator=(FastHash &&) = delete;

  /**
   * @brief Destructor
   */
  ~FastHash();

  /**
   * @brief Update the hash object
   * @param data: Bytes used to update the hash object
   * @return Hash object
   */
  FastHash &update(const std::string &data);

  /**
   * @brief Get the hash result
   * @return Hash result
   */
  std::size_t digest();

  /**
   * @brief Get the hash result
   * @return Hash result in hexadecimal form
   */
  std::string hex_digest();

 private:
  class FastHashImpl;
  std::experimental::propagate_const<std::unique_ptr<FastHashImpl>> impl_;
};

/**
 * @brief Get the hash object and calculate the hash value
 * @param data: Data to be hashed
 * @return Hash result
 */
std::size_t fast_hash(const std::string &data);
std::string fast_hash_hex(const std::string &data);

/**
 * @brief Contains a series of secure hash algorithms
 */
class SecureHash {
 public:
  /**
   * @brief Supported secure hash algorithm
   */
  enum class Algorithm { MD5, SHA_224, SHA_256, SHA_384, SHA_512 };

  /**
   * @brief Constructor
   * @param kind: Specify the algorithm used
   */
  explicit SecureHash(Algorithm kind);

  SecureHash(const SecureHash &) = delete;
  SecureHash(SecureHash &&) = delete;
  SecureHash &operator=(const SecureHash &) = delete;
  SecureHash &operator=(SecureHash &&) = delete;

  /**
   * @brief Destructor
   */
  ~SecureHash();

  /**
   * @brief Update the hash object
   * @param data: Bytes used to update the hash object
   * @return Hash object
   */
  SecureHash &update(const std::string &data);

  /**
   * @brief Get the hash result
   * @return Hash result
   */
  std::string digest();

  /**
   * @brief Get the hash result
   * @return Hash result in hexadecimal form
   */
  std::string hex_digest();

 private:
  class SecureHashImpl;
  std::experimental::propagate_const<std::unique_ptr<SecureHashImpl>> impl_;
};

/**
 * @brief Get the hash object and calculate the hash value
 * @param data: Data to be hashed
 * @return Hash result
 */
std::string md5(const std::string &data);
std::string md5_hex(const std::string &data);
std::string sha_224(const std::string &data);
std::string sha_224_hex(const std::string &data);
std::string sha_256(const std::string &data);
std::string sha_256_hex(const std::string &data);
std::string sha_384(const std::string &data);
std::string sha_384_hex(const std::string &data);
std::string sha_512(const std::string &data);
std::string sha_512_hex(const std::string &data);

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
