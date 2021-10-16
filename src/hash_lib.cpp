#include "klib/hash_lib.h"

#include <cstdint>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/evp.h>

#include "klib/detail/openssl_error.h"
#include "klib/exception.h"

namespace klib {

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
    throw RuntimeError(detail::openssl_err_msg());
  }
}

// https://www.openssl.org/docs/man3.0/man3/EVP_DigestUpdate.html
void HashLib::HashLibImpl::update(const std::string &data) {
  if (!doing_) {
    detail::check_openssl_return_value(EVP_DigestInit(ctx_, algorithm_));
    doing_ = true;
  }

  detail::check_openssl_return_value(
      EVP_DigestUpdate(ctx_, std::data(data), std::size(data)));
}

std::string HashLib::HashLibImpl::digest() {
  if (!doing_) {
    throw LogicError("must call update() first");
  }

  std::string digest;
  digest.resize(EVP_MAX_MD_SIZE);

  std::uint32_t size;
  detail::check_openssl_return_value(EVP_DigestFinal(
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
      throw LogicError("Unknown padding mode");
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

std::string md5(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::MD5);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string md5_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::MD5);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha_224(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_224);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha_224_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_224);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha_256(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_256);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha_256_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_256);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha_384(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_384);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha_384_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_384);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha_512(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_512);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha_512_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA_512);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha3_224(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_224);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha3_224_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_224);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha3_256(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_256);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha3_256_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_256);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha3_384(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_384);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha3_384_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_384);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

std::string sha3_512(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_512);
  hash_lib.update(data);
  return hash_lib.digest();
}

std::string sha3_512_hex(const std::string &data) {
  HashLib hash_lib(HashLib::Algorithm::SHA3_512);
  hash_lib.update(data);
  return hash_lib.hex_digest();
}

}  // namespace klib
