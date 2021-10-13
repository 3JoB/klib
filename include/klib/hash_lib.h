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
  ~HashLib() = default;

  /**
   * @brief Update the hash object
   * @param Bytes used to update the hash object
   * @return Hash object
   */
  HashLib &update(const std::string &data);

  /**
   * @brief Return the digest of the data passed to the update() method so far
   * @return
   */
  std::string digest();

  /**
   *
   * @return
   */
  std::string hex_digest();

  static HashLib &md5(const std::string &data);
  static HashLib &sha_224(const std::string &data);
  static HashLib &sha_256(const std::string &data);
  static HashLib &sha_384(const std::string &data);
  static HashLib &sha_512(const std::string &data);
  static HashLib &sha3_224(const std::string &data);
  static HashLib &sha3_256(const std::string &data);
  static HashLib &sha3_384(const std::string &data);
  static HashLib &sha3_512(const std::string &data);

 private:
  class HashLibImpl;
  std::experimental::propagate_const<std::unique_ptr<HashLibImpl>> impl_;
};

}  // namespace klib
