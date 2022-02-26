/**
 * @see https://github.com/Cyan4973/xxHash/blob/dev/README.md
 * @see
 * https://github.com/chromium/chromium/blob/main/base/hash/md5_boringssl.cc
 * @see
 * https://github.com/chromium/chromium/blob/main/base/hash/sha1_boringssl.cc
 * @see https://github.com/P-H-C/phc-winner-argon2/blob/master/README.md
 */

#include "klib/hash.h"

#include <argon2.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <xxhash.h>
#include <memory>

#include "klib/exception.h"
#include "klib/util.h"

namespace klib {

namespace {

std::string num_to_hex_string(std::size_t num) {
  return fmt::format(FMT_COMPILE("{:x}"), num);
}

std::string bytes_to_hex_string(const std::string &bytes) {
  std::string str;
  str.reserve(SHA256_DIGEST_LENGTH * 2);

  for (auto byte : bytes) {
    str.append(
        fmt::format(FMT_COMPILE("{:02x}"), static_cast<std::uint8_t>(byte)));
  }

  return str;
}

}  // namespace

std::size_t fast_hash(const std::string &data) {
  static std::unique_ptr<XXH3_state_t, decltype(XXH3_freeState) *> status1(
      XXH3_createState(), XXH3_freeState);
  auto status = status1.get();

  if (XXH3_64bits_reset(status) == XXH_ERROR) [[unlikely]] {
    throw RuntimeError("XXH3_64bits_reset failed");
  }

  if (XXH3_64bits_update(status, std::data(data), std::size(data)) == XXH_ERROR)
      [[unlikely]] {
    throw RuntimeError("XXH3_64bits_update failed");
  }

  return XXH3_64bits_digest(status);
}

std::string fast_hash_hex(const std::string &data) {
  return num_to_hex_string(fast_hash(data));
}

std::string md5(const std::string &data) {
  std::string result;
  result.resize(MD5_DIGEST_LENGTH);

  MD5(reinterpret_cast<const std::uint8_t *>(std::data(data)), std::size(data),
      reinterpret_cast<std::uint8_t *>(std::data(result)));

  return result;
}

std::string md5_hex(const std::string &data) {
  return bytes_to_hex_string(md5(data));
}

std::string sha224(const std::string &data) {
  std::string result;
  result.resize(SHA224_DIGEST_LENGTH);

  SHA224(reinterpret_cast<const std::uint8_t *>(std::data(data)),
         std::size(data), reinterpret_cast<std::uint8_t *>(std::data(result)));

  return result;
}

std::string sha224_hex(const std::string &data) {
  return bytes_to_hex_string(sha224(data));
}

std::string sha256(const std::string &data) {
  std::string result;
  result.resize(SHA256_DIGEST_LENGTH);

  SHA256(reinterpret_cast<const std::uint8_t *>(std::data(data)),
         std::size(data), reinterpret_cast<std::uint8_t *>(std::data(result)));

  return result;
}

std::string sha256_hex(const std::string &data) {
  return bytes_to_hex_string(sha256(data));
}

std::string sha384(const std::string &data) {
  std::string result;
  result.resize(SHA384_DIGEST_LENGTH);

  SHA384(reinterpret_cast<const std::uint8_t *>(std::data(data)),
         std::size(data), reinterpret_cast<std::uint8_t *>(std::data(result)));

  return result;
}

std::string sha384_hex(const std::string &data) {
  return bytes_to_hex_string(sha384(data));
}

std::string sha512(const std::string &data) {
  std::string result;
  result.resize(SHA512_DIGEST_LENGTH);

  SHA512(reinterpret_cast<const std::uint8_t *>(std::data(data)),
         std::size(data), reinterpret_cast<std::uint8_t *>(std::data(result)));

  return result;
}

std::string sha512_hex(const std::string &data) {
  return bytes_to_hex_string(sha512(data));
}

std::pair<std::string, std::string> password_hash_raw(
    const std::string &password, std::uint32_t time_cost,
    std::uint32_t memory_cost, std::uint32_t parallelism, std::int32_t hash_len,
    std::int32_t salt_len) {
  std::string hash;
  hash.resize(hash_len);

  auto salt = generate_random_bytes(salt_len);

  auto rc =
      argon2id_hash_raw(time_cost, memory_cost, parallelism,
                        std::data(password), std::size(password),
                        std::data(salt), salt_len, std::data(hash), hash_len);
  if (rc != ARGON2_OK) [[unlikely]] {
    throw RuntimeError(argon2_error_message(rc));
  }

  return {hash, salt};
}

std::string password_hash_raw(const std::string &password,
                              const std::string &salt, std::uint32_t time_cost,
                              std::uint32_t memory_cost,
                              std::uint32_t parallelism,
                              std::int32_t hash_len) {
  std::string hash;
  hash.resize(hash_len);

  auto rc = argon2id_hash_raw(time_cost, memory_cost, parallelism,
                              std::data(password), std::size(password),
                              std::data(salt), std::size(salt), std::data(hash),
                              hash_len);
  if (rc != ARGON2_OK) [[unlikely]] {
    throw RuntimeError(argon2_error_message(rc));
  }

  return hash;
}

std::string password_hash_encoded(const std::string &password,
                                  std::uint32_t time_cost,
                                  std::uint32_t memory_cost,
                                  std::uint32_t parallelism,
                                  std::int32_t hash_len,
                                  std::int32_t salt_len) {
  auto encode_len =
      argon2_encodedlen(time_cost, memory_cost, parallelism, salt_len, hash_len,
                        argon2_type::Argon2_id);
  std::string encoded;
  encoded.resize(encode_len);

  auto salt = generate_random_bytes(salt_len);

  auto rc = argon2id_hash_encoded(time_cost, memory_cost, parallelism,
                                  std::data(password), std::size(password),
                                  std::data(salt), salt_len, hash_len,
                                  std::data(encoded), encode_len);
  if (rc != ARGON2_OK) [[unlikely]] {
    throw RuntimeError(argon2_error_message(rc));
  }
  // '\0'
  encoded.resize(encode_len - 1);

  return encoded;
}

bool password_verify(const std::string &password, const std::string &encoded) {
  return argon2id_verify(encoded.c_str(), std::data(password),
                         std::size(password)) == ARGON2_OK;
}

bool password_verify(const std::string &password, const std::string &hash,
                     const std::string &salt, std::uint32_t time_cost,
                     std::uint32_t memory_cost, std::uint32_t parallelism) {
  std::int32_t hash_len = std::size(hash);
  std::int32_t salt_len = std::size(salt);

  std::string password_hash;
  password_hash.resize(hash_len);

  auto rc = argon2id_hash_raw(time_cost, memory_cost, parallelism,
                              std::data(password), std::size(password),
                              std::data(salt), salt_len,
                              std::data(password_hash), hash_len);
  if (rc != ARGON2_OK) [[unlikely]] {
    throw RuntimeError(argon2_error_message(rc));
  }

  return password_hash == hash;
}

}  // namespace klib
