/**
 * @file hash_lib.h
 * @brief Contains secure hash module
 */

#pragma once

#include <experimental/propagate_const>
#include <memory>
#include <string>

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

}  // namespace klib
