/**
 * @file hash.h
 * @brief Contains secure hash module
 */

#pragma once

#include <cstdint>
#include <experimental/propagate_const>
#include <memory>
#include <string>
#include <utility>

namespace klib {

/**
 * @brief Contains a series of secure hash algorithms
 */
class HashLib {
 public:
  /**
   * @brief Supported secure hash algorithm
   */
  enum class Algorithm {
    MD5,
    SHA_224,
    SHA_256,
    SHA_384,
    SHA_512,
    SHA3_224,
    SHA3_256,
    SHA3_384,
    SHA3_512
  };

  /**
   * @brief Constructor
   * @param kind: Specify the algorithm used
   */
  explicit HashLib(Algorithm kind);

  HashLib(const HashLib &) = delete;
  HashLib(HashLib &&) = delete;
  HashLib &operator=(const HashLib &) = delete;
  HashLib &operator=(HashLib &&) = delete;

  /**
   * @brief Destructor
   */
  ~HashLib();

  /**
   * @brief Update the hash object
   * @param data: Bytes used to update the hash object
   * @return Hash object
   */
  HashLib &update(const std::string &data);

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
  class HashLibImpl;
  std::experimental::propagate_const<std::unique_ptr<HashLibImpl>> impl_;
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
std::string sha3_224(const std::string &data);
std::string sha3_224_hex(const std::string &data);
std::string sha3_256(const std::string &data);
std::string sha3_256_hex(const std::string &data);
std::string sha3_384(const std::string &data);
std::string sha3_384_hex(const std::string &data);
std::string sha3_512(const std::string &data);
std::string sha3_512_hex(const std::string &data);

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
    std::uint32_t memory_cost = 1 * 1024 * 1024, std::uint32_t parallelism = 16,
    std::int32_t hash_len = 32, std::int32_t salt_len = 32);

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
                                  std::uint32_t memory_cost = 1 * 1024 * 1024,
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
                     std::uint32_t memory_cost = 1 * 1024 * 1024,
                     std::uint32_t parallelism = 16);

}  // namespace klib
