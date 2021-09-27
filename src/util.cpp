#include "klib/util.h"

#include <unistd.h>
#include <wait.h>

#include <algorithm>
#include <cerrno>
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <cuchar>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "klib/error.h"
#include "klib/exception.h"

namespace klib {

namespace {

void check_openssl(std::int32_t rc) {
  if (rc != 1) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }
}

std::string bytes_to_hex_string(const std::vector<std::uint8_t> &bytes) {
  std::string str;

  // https://zh.wikipedia.org/wiki/SHA-3#SHA_%E5%AE%B6%E6%97%8F%E5%87%BD%E6%95%B0%E7%9A%84%E6%AF%94%E8%BE%83
  str.reserve(128);

  for (auto byte : bytes) {
    str += fmt::format(FMT_COMPILE("{:02x}"), byte);
  }

  return str;
}

std::map<std::string, std::string> read_folder(const std::string &path) {
  if (!std::filesystem::is_directory(path)) {
    throw RuntimeError("'{}' is not a directory", path);
  }

  std::map<std::string, std::string> folder;

  for (const auto &item : std::filesystem::recursive_directory_iterator(path)) {
    auto relative_path = item.path().string().substr(std::size(path) + 1);

    if (std::filesystem::is_regular_file(item.path())) {
      folder.emplace(relative_path, read_file(item.path(), true));
    } else {
      folder.emplace(relative_path, "");
    }
  }

  return folder;
}

std::vector<std::uint8_t> do_evp(const std::string &str,
                                 std::uint32_t digest_length,
                                 const EVP_MD *algorithm) {
  std::unique_ptr<EVP_MD_CTX, decltype(EVP_MD_CTX_free) *> context(
      EVP_MD_CTX_new(), EVP_MD_CTX_free);
  if (!context) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  check_openssl(EVP_DigestInit(context.get(), algorithm));
  check_openssl(EVP_DigestUpdate(context.get(), str.c_str(), std::size(str)));

  auto digest = std::make_unique<std::uint8_t[]>(digest_length);
  check_openssl(EVP_DigestFinal(context.get(), digest.get(), &digest_length));

  return std::vector<std::uint8_t>(digest.get(), digest.get() + digest_length);
}

}  // namespace

ChangeWorkingDir::ChangeWorkingDir(const std::string &path) {
  if (!std::empty(path)) {
    backup_ = std::filesystem::current_path();

    if (!std::filesystem::is_directory(path)) {
      if (!std::filesystem::create_directory(path)) {
        throw RuntimeError("can not create directory: '{}'", path);
      }
    }

    if (chdir(path.c_str())) {
      throw RuntimeError("chdir error");
    }
  }
}

ChangeWorkingDir::~ChangeWorkingDir() {
  if (!std::empty(backup_) && chdir(backup_.c_str())) {
    error("chdir error");
  }
}

std::vector<std::string> split_str(const std::string &str,
                                   const std::string &separate) {
  std::vector<std::string> result;
  result.reserve(128);

  boost::split(result, str, boost::is_any_of(separate),
               boost::token_compress_on);
  for (auto &line : result) {
    boost::trim(line);
  }

  std::erase_if(result,
                [](const std::string &line) { return std::empty(line); });

  return result;
}

std::string read_file(const std::string &path, bool binary_mode) {
  return read_file(path.c_str(), binary_mode);
}

std::string read_file(std::string_view path, bool binary_mode) {
  return read_file(path.data(), binary_mode);
}

std::string read_file(const char *path, bool binary_mode) {
  if (!std::filesystem::is_regular_file(path)) {
    throw RuntimeError("'{}' is not a file", path);
  }

  std::ifstream ifs;
  if (binary_mode) {
    ifs.open(path, std::ifstream::binary);
  } else {
    ifs.open(path);
  }

  if (!ifs) {
    throw RuntimeError("can not open file: '{}'", path);
  }

  std::string data;

  auto size = ifs.seekg(0, std::ifstream::end).tellg();
  data.resize(size);
  ifs.seekg(0, std::ifstream::beg).read(data.data(), size);

  return data;
}

std::vector<std::string> read_file_line(const std::string &path) {
  return read_file_line(path.c_str());
}

std::vector<std::string> read_file_line(std::string_view path) {
  return read_file_line(path.data());
}

std::vector<std::string> read_file_line(const char *path) {
  auto str = read_file(path, false);
  return split_str(str, "\n");
}

void write_file(const std::string &path, bool binary_mode,
                const std::string &content) {
  write_file(path.c_str(), binary_mode, content.c_str(), std::size(content));
}

void write_file(std::string_view path, bool binary_mode,
                std::string_view content) {
  write_file(path.data(), binary_mode, content.data(), std::size(content));
}

void write_file(const char *path, bool binary_mode, const char *content,
                std::size_t length) {
  std::ofstream ofs;
  if (binary_mode) {
    ofs.open(path, std::ofstream::binary);
  } else {
    ofs.open(path);
  }

  if (!ofs) {
    throw RuntimeError("can not open file: '{}'", path);
  }

  ofs.write(content, length);
}

// https://zh.cppreference.com/w/c/string/multibyte/mbrtoc16
std::u16string utf8_to_utf16(const std::string &str) {
  if (std::empty(str)) {
    return {};
  }

  std::setlocale(LC_ALL, "en_US.utf8");

  std::u16string result;

  char16_t out = 0;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = std::mbrtoc16(&out, begin, size, &state)) {
    if (rc == static_cast<std::size_t>(-1)) {
      throw RuntimeError(std::strerror(errno));
    }

    if (rc == static_cast<std::size_t>(-3)) {
      result.push_back(out);
    } else if (rc <= std::numeric_limits<std::size_t>::max() / 2) {
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
  if (std::empty(str)) {
    return {};
  }

  std::setlocale(LC_ALL, "en_US.utf8");

  std::u32string result;

  char32_t out = 0;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = std::mbrtoc32(&out, begin, size, &state)) {
    if (rc == static_cast<std::size_t>(-1)) {
      throw RuntimeError(std::strerror(errno));
    }

    if (rc > static_cast<std::size_t>(-1) / 2) {
      break;
    }

    begin += rc;
    result.push_back(out);
  }

  return result;
}

// https://zh.cppreference.com/w/c/string/multibyte/c32rtomb
std::string utf32_to_utf8(char32_t c) {
  std::setlocale(LC_ALL, "en_US.utf8");

  std::string result;
  result.resize(MB_CUR_MAX);

  mbstate_t state = {};
  auto rc = std::c32rtomb(std::data(result), c, &state);

  if (rc == static_cast<std::size_t>(-1)) {
    throw RuntimeError(std::strerror(errno));
  } else if (rc == 0) {
    throw RuntimeError("utf32_to_utf8 error");
  }

  result.resize(rc);
  return result;
}

std::string utf32_to_utf8(const std::u32string &str) {
  std::string result;

  for (auto c : str) {
    result.append(utf32_to_utf8(c));
  }

  return result;
}

bool is_ascii(const std::string &str) {
  return std::all_of(std::begin(str), std::end(str),
                     [](char c) { return is_ascii(c); });
}

bool is_ascii(const std::u32string &str) {
  return std::all_of(std::begin(str), std::end(str),
                     [](char32_t c) { return is_ascii(c); });
}

bool is_chinese(const std::string &c) {
  if (std::empty(c)) {
    return false;
  }

  auto utf32 = utf8_to_utf32(c);

  if (std::size(utf32) != 1) {
    throw RuntimeError("not a UTF-32 encoded character: '{}'", c);
  }

  return is_chinese(utf32.front());
}

// https://github.com/gspark/cryptool/blob/main/src/crypto/digest/base64.cpp
std::string base64_encode(const std::string &str) {
  std::unique_ptr<EVP_ENCODE_CTX, decltype(EVP_ENCODE_CTX_free) *> context(
      EVP_ENCODE_CTX_new(), EVP_ENCODE_CTX_free);
  if (!context) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  EVP_EncodeInit(context.get());

  std::int32_t chunk_len = 0;
  auto size = std::size(str);
  auto digest = std::make_unique<std::uint8_t[]>(EVP_ENCODE_LENGTH(size));
  if (auto code = EVP_EncodeUpdate(
          context.get(), digest.get(), &chunk_len,
          reinterpret_cast<const unsigned char *>(std::data(str)), size);
      code != 0 && code != 1) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  std::int32_t output_len = chunk_len;
  EVP_EncodeFinal(context.get(), digest.get() + chunk_len, &chunk_len);
  output_len += chunk_len;

  auto output =
      std::string(reinterpret_cast<const char *>(digest.get()), output_len);

  boost::replace_all(output, "\n", "");
  return output;
}

std::string base64_decode(const std::string &str) {
  std::unique_ptr<EVP_ENCODE_CTX, decltype(EVP_ENCODE_CTX_free) *> context(
      EVP_ENCODE_CTX_new(), EVP_ENCODE_CTX_free);
  if (!context) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  EVP_DecodeInit(context.get());

  std::int32_t chunk_len = 0;
  auto size = std::size(str);
  auto digest = std::make_unique<std::uint8_t[]>(EVP_DECODE_LENGTH(size));
  if (auto code = EVP_DecodeUpdate(
          context.get(), digest.get(), &chunk_len,
          reinterpret_cast<const unsigned char *>(std::data(str)), size);
      code != 0 && code != 1) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  std::int32_t output_len = chunk_len;
  check_openssl(
      EVP_DecodeFinal(context.get(), digest.get() + chunk_len, &chunk_len));
  output_len += chunk_len;

  return std::string(reinterpret_cast<const char *>(digest.get()), output_len);
}

std::string md5(const std::string &str) {
  return bytes_to_hex_string(md5_raw(str));
}

std::vector<std::uint8_t> md5_raw(const std::string &str) {
  return do_evp(str, MD5_DIGEST_LENGTH, EVP_md5());
}

std::string md5_file(const std::string &path) {
  return md5(read_file(path, true));
}

std::string sha_256(const std::string &str) {
  return bytes_to_hex_string(sha_256_raw(str));
}

std::vector<std::uint8_t> sha_256_raw(const std::string &str) {
  return do_evp(str, SHA256_DIGEST_LENGTH, EVP_sha256());
}

std::string sha_256_file(const std::string &path) {
  return sha_256(read_file(path, true));
}

std::string sha3_512(const std::string &str) {
  return bytes_to_hex_string(sha3_512_raw(str));
}

std::vector<std::uint8_t> sha3_512_raw(const std::string &str) {
  return do_evp(str, SHA512_DIGEST_LENGTH, EVP_sha3_512());
}

std::string sha3_512_file(const std::string &path) {
  return sha3_512(read_file(path, true));
}

// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption#C.2B.2B_Programs
std::string aes_256_cbc_encrypt(const std::string &str,
                                const std::vector<std::uint8_t> &key,
                                const std::vector<std::uint8_t> &iv) {
  if (std::size(key) != 32) {
    throw RuntimeError("The key is not 256 bit");
  }
  if (std::size(iv) != AES_BLOCK_SIZE) {
    throw RuntimeError("The iv is not 16 bit");
  }

  check_openssl(EVP_add_cipher(EVP_aes_256_cbc()));
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx(
      EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
  if (!ctx) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  check_openssl(EVP_EncryptInit_ex(
      ctx.get(), EVP_aes_256_cbc(), nullptr,
      reinterpret_cast<const unsigned char *>(std::data(key)),
      reinterpret_cast<const unsigned char *>(std::data(iv))));

  check_openssl(EVP_CIPHER_CTX_set_padding(ctx.get(), EVP_PADDING_PKCS7));

  std::string result;
  auto input_size = std::size(str);
  result.resize(input_size + AES_BLOCK_SIZE);
  std::int32_t len;

  check_openssl(EVP_EncryptUpdate(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)), &len,
      reinterpret_cast<const unsigned char *>(std::data(str)), input_size));

  std::int32_t len2;
  check_openssl(EVP_EncryptFinal_ex(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)) + len,
      &len2));

  result.resize(len + len2);
  return result;
}

std::string aes_256_cbc_decrypt(const std::string &str,
                                const std::vector<std::uint8_t> &key,
                                const std::vector<std::uint8_t> &iv) {
  if (std::size(key) != 32) {
    throw RuntimeError("The key is not 256 bit");
  }
  if (std::size(iv) != AES_BLOCK_SIZE) {
    throw RuntimeError("The iv is not 16 bit");
  }

  check_openssl(EVP_add_cipher(EVP_aes_256_cbc()));
  std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx(
      EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
  if (!ctx) {
    throw RuntimeError(ERR_error_string(ERR_get_error(), nullptr));
  }

  check_openssl(EVP_DecryptInit_ex(
      ctx.get(), EVP_aes_256_cbc(), nullptr,
      reinterpret_cast<const unsigned char *>(std::data(key)),
      reinterpret_cast<const unsigned char *>(std::data(iv))));

  check_openssl(EVP_CIPHER_CTX_set_padding(ctx.get(), EVP_PADDING_PKCS7));

  std::string result;
  auto input_size = std::size(str);
  result.resize(input_size + AES_BLOCK_SIZE);
  std::int32_t len;

  check_openssl(EVP_DecryptUpdate(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)), &len,
      reinterpret_cast<const unsigned char *>(std::data(str)), input_size));

  std::int32_t len2;
  check_openssl(EVP_DecryptFinal_ex(
      ctx.get(), reinterpret_cast<unsigned char *>(std::data(result)) + len,
      &len2));

  result.resize(len + len2);
  return result;
}

std::size_t folder_size(const std::string &path) {
  if (!std::filesystem::is_directory(path)) {
    throw RuntimeError("'{}' is not a directory", path);
  }

  std::size_t size = 0;

  for (const auto &item : std::filesystem::recursive_directory_iterator(path)) {
    if (std::filesystem::is_regular_file(item)) {
      size += std::filesystem::file_size(item);
    }
  }

  return size;
}

bool same_folder(const std::string &path1, const std::string &path2) {
  return read_folder(path1) == read_folder(path2);
}

void execute_command(const std::string &command) {
  if (std::empty(command)) {
    return;
  }

  execute_command(command.c_str());
}

void execute_command(const char *command) {
  if (!command) {
    return;
  }

  auto status = std::system(command);
  if (status == -1 || !WIFEXITED(status) || WEXITSTATUS(status)) {
    throw RuntimeError("execute command error: '{}'", command);
  }
}

void wait_for_child_process() {
  std::int32_t status = 0;

  while (waitpid(-1, &status, 0) > 0) {
    if (!WIFEXITED(status) || WEXITSTATUS(status)) {
      throw RuntimeError("Waitpid error: {}", status);
    }
  }
}

std::string uuid() {
  auto uuid = boost::uuids::random_generator()();
  return boost::uuids::to_string(uuid);
}

}  // namespace klib
