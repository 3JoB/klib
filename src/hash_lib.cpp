#include "klib/hash_lib.h"

#include <cstdint>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/err.h>
#include <openssl/evp.h>

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
  static const EVP_MD *get_algorithm(HashLib::Algorithm kind);
  static std::string bytes_to_hex_string(const std::string &bytes);

  EVP_MD_CTX *ctx_ = EVP_MD_CTX_new();
  const EVP_MD *algorithm_;

  bool doing_ = false;
};

HashLib::HashLibImpl::HashLibImpl(HashLib::Algorithm kind)
    : algorithm_(HashLibImpl::get_algorithm(kind)) {
  if (!ctx_) {
    EVP_MD_CTX_free(ctx_);
    throw klib::RuntimeError(openssl_err_msg());
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
  if (!doing_) {
    throw LogicError("must call update() first");
  }

  std::string digest;
  digest.resize(EVP_MAX_MD_SIZE);

  std::uint32_t size;
  check_openssl_return_value(EVP_DigestFinal(
      ctx_, reinterpret_cast<unsigned char *>(std::data(digest)), &size));
  digest.resize(size);

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
  str.reserve(EVP_MAX_MD_SIZE);

  for (auto byte : bytes) {
    str += fmt::format(FMT_COMPILE("{:02x}"), static_cast<std::uint8_t>(byte));
  }

  return str;
}

const EVP_MD *HashLib::HashLibImpl::get_algorithm(HashLib::Algorithm kind) {
  const EVP_MD *algorithm = nullptr;

  switch (kind) {
    case HashLib::Algorithm::MD5:
      algorithm = EVP_md5();
      break;
    case HashLib::Algorithm::SHA_224:
      algorithm = EVP_sha224();
      break;
    case HashLib::Algorithm::SHA_256:
      algorithm = EVP_sha256();
      break;
    case HashLib::Algorithm::SHA_384:
      algorithm = EVP_sha384();
      break;
    case HashLib::Algorithm::SHA_512:
      algorithm = EVP_sha512();
      break;
    case HashLib::Algorithm::SHA3_224:
      algorithm = EVP_sha3_224();
      break;
    case HashLib::Algorithm::SHA3_256:
      algorithm = EVP_sha3_256();
      break;
    case HashLib::Algorithm::SHA3_384:
      algorithm = EVP_sha3_384();
      break;
    case HashLib::Algorithm::SHA3_512:
      algorithm = EVP_sha3_512();
      break;
    default:
      algorithm = nullptr;
  }

  return algorithm;
}

HashLib::HashLib(HashLib::Algorithm kind)
    : impl_(std::make_unique<HashLibImpl>(kind)) {}

HashLib::~HashLib() = default;

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
