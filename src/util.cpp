#include "klib/util.h"

#include <cassert>
#include <clocale>
#include <cstddef>
#include <cstdint>
#include <cuchar>
#include <filesystem>
#include <fstream>
#include <vector>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "klib/detail/error.h"

namespace {

std::string bytes_to_hex_string(const std::vector<std::uint8_t> &bytes) {
  std::string str;
  for (auto byte : bytes) {
    str += fmt::format(FMT_COMPILE("{:02x}"), static_cast<int>(byte));
  }

  return str;
}

}  // namespace

namespace klib {

std::string read_file(const std::string &path, bool binary_mode) {
  assert(!std::empty(path) && "'path' cannot be empty");

  if (!std::filesystem::exists(path)) {
    detail::error("'{}' does not exist", path);
  }
  if (!std::filesystem::is_regular_file(path)) {
    detail::error("'{}' is not a file", path);
  }

  std::ifstream ifs;
  if (binary_mode) {
    ifs.open(path, std::ifstream::binary);
  } else {
    ifs.open(path);
  }

  if (!ifs) {
    detail::error("can not open file: '{}'", path);
  }

  std::string data;
  data.reserve(16384);

  auto size = ifs.seekg(0, std::ifstream::end).tellg();
  data.resize(size);
  ifs.seekg(0, std::ifstream::beg).read(data.data(), size);

  return data;
}

// https://zh.cppreference.com/w/c/string/multibyte/mbrtoc16
std::u16string utf8_to_utf16(const std::string &str) {
  std::setlocale(LC_ALL, "en_US.utf8");

  std::u16string result;

  char16_t out;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = mbrtoc16(&out, begin, size, &state)) {
    if (rc == static_cast<std::size_t>(-3)) {
      result.push_back(out);
    } else if (rc <= SIZE_MAX / 2) {
      begin += rc;
      result.push_back(out);
    } else {
      break;
    }
  }

  return result;
}

// https://zh.cppreference.com/w/c/string/multibyte/mbrtoc32
std::u32string utf8_to_utf32(const std::string &str) {
  std::setlocale(LC_ALL, "en_US.utf8");

  std::u32string result;

  char32_t out;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = mbrtoc32(&out, begin, size, &state)) {
    assert(rc != static_cast<std::size_t>(-3));

    if (rc > static_cast<std::size_t>(-1) / 2) {
      break;
    }

    begin += rc;
    result.push_back(out);
  }

  return result;
}

bool is_chinese(const std::string &c) {
  auto utf32 = utf8_to_utf32(c);

  if (std::size(utf32) != 1) {
    detail::error("not a char");
  }

  return is_chinese(utf32.front());
}

std::string sha3_512(const std::string &path) {
  auto data = read_file(path, true);

  std::uint32_t digest_length = SHA512_DIGEST_LENGTH;
  auto digest = static_cast<uint8_t *>(OPENSSL_malloc(digest_length));

  EVP_MD_CTX *context = EVP_MD_CTX_new();
  auto algorithm = EVP_sha3_512();
  EVP_DigestInit_ex(context, algorithm, nullptr);
  EVP_DigestUpdate(context, data.c_str(), data.size());
  EVP_DigestFinal_ex(context, digest, &digest_length);
  EVP_MD_CTX_destroy(context);

  auto output = bytes_to_hex_string(
      std::vector<std::uint8_t>(digest, digest + digest_length));

  OPENSSL_free(digest);

  return output;
}

}  // namespace klib
