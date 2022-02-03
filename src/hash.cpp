#include "klib/hash.h"

#include <argon2.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/digest.h>
#include <xxhash.h>
#include <scope_guard.hpp>

#include "klib/detail/openssl_util.h"
#include "klib/exception.h"
#include "klib/util.h"

namespace klib {

namespace {

enum class SHA { MD5, SHA224, SHA256, SHA384, SHA512 };

std::string num_to_hex_string(std::size_t num) {
  std::string str;
  str.reserve(16);

  str += fmt::format(FMT_COMPILE("{:x}"), num);

  return str;
}

std::string bytes_to_hex_string(const std::string &bytes) {
  std::string str;
  str.reserve(EVP_MAX_MD_SIZE);

  for (auto byte : bytes) {
    str += fmt::format(FMT_COMPILE("{:02x}"), static_cast<std::uint8_t>(byte));
  }

  return str;
}

const EVP_MD *get_algorithm(SHA kind) {
  const EVP_MD *algorithm;

  switch (kind) {
    case SHA::MD5:
      algorithm = EVP_md5();
      break;
    case SHA::SHA224:
      algorithm = EVP_sha224();
      break;
    case SHA::SHA256:
      algorithm = EVP_sha256();
      break;
    case SHA::SHA384:
      algorithm = EVP_sha384();
      break;
    case SHA::SHA512:
      algorithm = EVP_sha512();
      break;
  }

  return algorithm;
}

// https://www.openssl.org/docs/man3.0/man3/EVP_DigestUpdate.html
std::string do_hash(const std::string &data, SHA kind) {
  bssl::ScopedEVP_MD_CTX ctx;

  auto rc = EVP_DigestInit_ex(ctx.get(), get_algorithm(kind), nullptr);
  detail::check_openssl_return(rc);

  rc = EVP_DigestUpdate(ctx.get(), std::data(data), std::size(data));
  detail::check_openssl_return(rc);

  std::string result;
  result.resize(EVP_MAX_MD_SIZE);

  std::uint32_t size;
  rc = EVP_DigestFinal_ex(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)), &size);
  detail::check_openssl_return(rc);

  result.resize(size);
  return result;
}

}  // namespace

std::size_t fast_hash(const std::string &data) {
  auto status = XXH3_createState();
  SCOPE_EXIT { XXH3_freeState(status); };
  if (!status) {
    throw RuntimeError("XXH3_createState failed");
  }

  if (XXH3_64bits_reset(status) == XXH_ERROR) {
    throw RuntimeError("XXH3_64bits_reset failed");
  }

  if (XXH3_64bits_update(status, std::data(data), std::size(data)) ==
      XXH_ERROR) {
    throw RuntimeError("XXH3_64bits_update failed");
  }

  return XXH3_64bits_digest(status);
}

std::string fast_hash_hex(const std::string &data) {
  return num_to_hex_string(fast_hash(data));
}

std::string md5(const std::string &data) { return do_hash(data, SHA::MD5); }

std::string md5_hex(const std::string &data) {
  return bytes_to_hex_string(md5(data));
}

std::string sha224(const std::string &data) {
  return do_hash(data, SHA::SHA224);
}

std::string sha224_hex(const std::string &data) {
  return bytes_to_hex_string(sha224(data));
}

std::string sha256(const std::string &data) {
  return do_hash(data, SHA::SHA256);
}

std::string sha256_hex(const std::string &data) {
  return bytes_to_hex_string(sha256(data));
}

std::string sha384(const std::string &data) {
  return do_hash(data, SHA::SHA384);
}

std::string sha384_hex(const std::string &data) {
  return bytes_to_hex_string(sha384(data));
}

std::string sha512(const std::string &data) {
  return do_hash(data, SHA::SHA512);
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
  if (rc != ARGON2_OK) {
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
  if (rc != ARGON2_OK) {
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
  if (rc != ARGON2_OK) {
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
  if (rc != ARGON2_OK) {
    throw RuntimeError(argon2_error_message(rc));
  }

  return password_hash == hash;
}

}  // namespace klib
