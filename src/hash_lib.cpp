#include "klib/hash_lib.h"

#include <cstdint>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#include "klib/exception.h"

namespace klib {

namespace {

std::string openssl_err_msg() {
  return ERR_error_string(ERR_get_error(), nullptr);
}

void check_openssl_return_value(std::int32_t rc) {
  if (rc != 1) {
    throw klib::RuntimeError(openssl_err_msg());
  }
}

}  // namespace

class HashLib::HashLibImpl {
 public:
  explicit HashLibImpl(Algorithm kind);

  HashLibImpl(const HashLibImpl &) = delete;
  HashLibImpl(HashLibImpl &&) = delete;
  HashLibImpl &operator=(const HashLibImpl &) = delete;
  HashLibImpl &operator=(HashLibImpl &&) = delete;

  void update(const std::string &data);
  std::string digest();
  std::string hex_digest();

  ~HashLibImpl();

 private:
  static std::pair<const EVP_MD *, std::uint32_t> get_info(
      HashLib::Algorithm kind);
  static std::string bytes_to_hex_string(const std::string &bytes);

  EVP_MD_CTX *ctx_ = EVP_MD_CTX_new();

  std::uint32_t digest_length_ = 0;
  const EVP_MD *algorithm_ = nullptr;

  bool doing_ = false;
};

HashLib::HashLibImpl::HashLibImpl(HashLib::Algorithm kind) {
  try {
    if (!ctx_) {
      throw klib::RuntimeError(openssl_err_msg());
    }

    std::tie(algorithm_, digest_length_) = HashLibImpl::get_info(kind);
  } catch (...) {
    EVP_MD_CTX_free(ctx_);
    throw;
  }
}

// https://www.openssl.org/docs/man3.0/man3/EVP_DigestUpdate.html
void HashLib::HashLibImpl::update(const std::string &data) {
  if (!doing_) {
    check_openssl_return_value(EVP_DigestInit(ctx_, algorithm_));
    doing_ = true;
  }

  check_openssl_return_value(
      EVP_DigestUpdate(ctx_, std::data(data), std::size(data)));
}

std::string HashLib::HashLibImpl::digest() {
  std::string digest;

  digest.resize(digest_length_);
  check_openssl_return_value(EVP_DigestFinal(
      ctx_, reinterpret_cast<unsigned char *>(std::data(digest)),
      &digest_length_));
  digest.resize(digest_length_);

  doing_ = false;
  return digest;
}

std::string HashLib::HashLibImpl::hex_digest() {
  return HashLibImpl::bytes_to_hex_string(digest());
}

HashLib::HashLibImpl::~HashLibImpl() { EVP_MD_CTX_free(ctx_); }

std::string HashLib::HashLibImpl::bytes_to_hex_string(
    const std::string &bytes) {
  std::string str;
  str.reserve(SHA512_DIGEST_LENGTH);

  for (auto byte : bytes) {
    str += fmt::format(FMT_COMPILE("{:02x}"), static_cast<std::uint8_t>(byte));
  }

  return str;
}

std::pair<const EVP_MD *, std::uint32_t> HashLib::HashLibImpl::get_info(
    HashLib::Algorithm kind) {
  const EVP_MD *algorithm = nullptr;
  std::uint32_t digest_length = 0;

  switch (kind) {
    case HashLib::Algorithm::MD5:
      algorithm = EVP_md5();
      digest_length = MD5_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA_224:
      algorithm = EVP_sha224();
      digest_length = SHA224_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA_256:
      algorithm = EVP_sha256();
      digest_length = SHA256_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA_384:
      algorithm = EVP_sha384();
      digest_length = SHA384_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA_512:
      algorithm = EVP_sha512();
      digest_length = SHA512_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA3_224:
      algorithm = EVP_sha3_224();
      digest_length = SHA224_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA3_256:
      algorithm = EVP_sha3_256();
      digest_length = SHA256_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA3_384:
      algorithm = EVP_sha3_384();
      digest_length = SHA384_DIGEST_LENGTH;
      break;
    case HashLib::Algorithm::SHA3_512:
      algorithm = EVP_sha3_512();
      digest_length = SHA512_DIGEST_LENGTH;
      break;
    default:
      algorithm = nullptr;
      digest_length = 0;
  }

  return {algorithm, digest_length};
}

HashLib::HashLib(HashLib::Algorithm kind)
    : impl_(std::make_unique<HashLibImpl>(kind)) {}

HashLib &HashLib::update(const std::string &data) {
  impl_->update(data);
  return *this;
}

std::string HashLib::digest() { return impl_->digest(); }

std::string HashLib::hex_digest() { return impl_->hex_digest(); }

HashLib &HashLib::md5(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::MD5);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha_224(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA_224);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha_256(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA_256);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha_384(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA_384);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha_512(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA_512);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha3_224(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA3_224);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha3_256(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA3_256);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha3_384(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA3_384);
  hash_lib.update(data);
  return hash_lib;
}

HashLib &HashLib::sha3_512(const std::string &data) {
  static HashLib hash_lib(HashLib::Algorithm::SHA3_512);
  hash_lib.update(data);
  return hash_lib;
}

}  // namespace klib
