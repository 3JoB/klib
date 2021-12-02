#include "klib/hash.h"

#include <argon2.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/evp.h>
#include <xxhash.h>

#include "klib/detail/openssl_util.h"
#include "klib/exception.h"
#include "klib/util.h"

namespace klib {

namespace {

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

}  // namespace

class FastHash::FastHashImpl {
 public:
  explicit FastHashImpl();

  FastHashImpl(const FastHashImpl &) = delete;
  FastHashImpl(FastHashImpl &&) = delete;
  FastHashImpl &operator=(const FastHashImpl &) = delete;
  FastHashImpl &operator=(FastHashImpl &&) = delete;

  void update(const std::string &data);
  std::size_t digest();
  std::string hex_digest();

  ~FastHashImpl();

 private:
  XXH3_state_t *status_;

  bool doing_ = false;
};

FastHash::FastHashImpl::FastHashImpl() : status_(XXH3_createState()) {
  if (!status_) {
    throw RuntimeError("XXH3_createState failed");
  }
}

void FastHash::FastHashImpl::update(const std::string &data) {
  if (!doing_) {
    if (XXH3_64bits_reset(status_) == XXH_ERROR) {
      throw RuntimeError("XXH3_64bits_reset failed");
    }

    doing_ = true;
  }

  if (XXH3_64bits_update(status_, std::data(data), std::size(data)) ==
      XXH_ERROR) {
    throw RuntimeError("XXH3_64bits_update failed");
  }
}

std::size_t FastHash::FastHashImpl::digest() {
  if (!doing_) {
    throw LogicError("must call update() first");
  }

  doing_ = false;
  return XXH3_64bits_digest(status_);
}

std::string FastHash::FastHashImpl::hex_digest() {
  return num_to_hex_string(digest());
}

FastHash::FastHashImpl::~FastHashImpl() { XXH3_freeState(status_); }

FastHash::FastHash() : impl_(std::make_unique<FastHashImpl>()) {}

FastHash::~FastHash() = default;

FastHash &FastHash::update(const std::string &data) {
  impl_->update(data);
  return *this;
}

std::size_t FastHash::digest() { return impl_->digest(); }

std::string FastHash::hex_digest() { return impl_->hex_digest(); }

std::size_t fast_hash(const std::string &data) {
  FastHash fast_hash;
  fast_hash.update(data);
  return fast_hash.digest();
}

std::string fast_hash_hex(const std::string &data) {
  FastHash fast_hash;
  fast_hash.update(data);
  return fast_hash.hex_digest();
}

class SecureHash::SecureHashImpl {
 public:
  explicit SecureHashImpl(Algorithm kind);

  SecureHashImpl(const SecureHashImpl &) = delete;
  SecureHashImpl(SecureHashImpl &&) = delete;
  SecureHashImpl &operator=(const SecureHashImpl &) = delete;
  SecureHashImpl &operator=(SecureHashImpl &&) = delete;

  void update(const std::string &data);
  std::string digest();
  std::string hex_digest();

  ~SecureHashImpl();

 private:
  static const EVP_MD *get_algorithm(SecureHash::Algorithm kind);

  EVP_MD_CTX *ctx_ = EVP_MD_CTX_new();
  const EVP_MD *algorithm_;

  bool doing_ = false;
};

SecureHash::SecureHashImpl::SecureHashImpl(SecureHash::Algorithm kind)
    : algorithm_(SecureHashImpl::get_algorithm(kind)) {
  if (!ctx_) {
    throw RuntimeError(detail::openssl_err_msg());
  }
}

// https://www.openssl.org/docs/man3.0/man3/EVP_DigestUpdate.html
void SecureHash::SecureHashImpl::update(const std::string &data) {
  if (!doing_) {
    detail::check_openssl_return_1(EVP_DigestInit(ctx_, algorithm_));
    doing_ = true;
  }

  detail::check_openssl_return_1(
      EVP_DigestUpdate(ctx_, std::data(data), std::size(data)));
}

std::string SecureHash::SecureHashImpl::digest() {
  if (!doing_) {
    throw LogicError("must call update() first");
  }

  std::string digest;
  digest.resize(EVP_MAX_MD_SIZE);

  std::uint32_t size;
  detail::check_openssl_return_1(EVP_DigestFinal(
      ctx_, reinterpret_cast<unsigned char *>(std::data(digest)), &size));
  digest.resize(size);

  doing_ = false;
  return digest;
}

std::string SecureHash::SecureHashImpl::hex_digest() {
  return bytes_to_hex_string(digest());
}

SecureHash::SecureHashImpl::~SecureHashImpl() { EVP_MD_CTX_free(ctx_); }

const EVP_MD *SecureHash::SecureHashImpl::get_algorithm(
    SecureHash::Algorithm kind) {
  const EVP_MD *algorithm = nullptr;

  switch (kind) {
    case SecureHash::Algorithm::MD5:
      algorithm = EVP_md5();
      break;
    case SecureHash::Algorithm::SHA_224:
      algorithm = EVP_sha224();
      break;
    case SecureHash::Algorithm::SHA_256:
      algorithm = EVP_sha256();
      break;
    case SecureHash::Algorithm::SHA_384:
      algorithm = EVP_sha384();
      break;
    case SecureHash::Algorithm::SHA_512:
      algorithm = EVP_sha512();
      break;
    case SecureHash::Algorithm::SHA3_224:
      algorithm = EVP_sha3_224();
      break;
    case SecureHash::Algorithm::SHA3_256:
      algorithm = EVP_sha3_256();
      break;
    case SecureHash::Algorithm::SHA3_384:
      algorithm = EVP_sha3_384();
      break;
    case SecureHash::Algorithm::SHA3_512:
      algorithm = EVP_sha3_512();
      break;
    default:
      throw LogicError("Unknown hash algorithm");
  }

  return algorithm;
}

SecureHash::SecureHash(SecureHash::Algorithm kind)
    : impl_(std::make_unique<SecureHashImpl>(kind)) {}

SecureHash::~SecureHash() = default;

SecureHash &SecureHash::update(const std::string &data) {
  impl_->update(data);
  return *this;
}

std::string SecureHash::digest() { return impl_->digest(); }

std::string SecureHash::hex_digest() { return impl_->hex_digest(); }

std::string md5(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::MD5);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string md5_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::MD5);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha_224(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_224);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha_224_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_224);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha_256(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_256);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha_256_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_256);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha_384(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_384);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha_384_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_384);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha_512(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_512);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha_512_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA_512);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha3_224(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_224);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha3_224_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_224);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha3_256(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_256);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha3_256_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_256);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha3_384(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_384);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha3_384_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_384);
  secure_hash.update(data);
  return secure_hash.hex_digest();
}

std::string sha3_512(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_512);
  secure_hash.update(data);
  return secure_hash.digest();
}

std::string sha3_512_hex(const std::string &data) {
  SecureHash secure_hash(SecureHash::Algorithm::SHA3_512);
  secure_hash.update(data);
  return secure_hash.hex_digest();
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
