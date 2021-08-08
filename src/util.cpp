#include "klib/util.h"

#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <cuchar>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "klib/error.h"
#include "klib/exception.h"

namespace klib {

namespace {

std::string bytes_to_hex_string(const std::vector<std::uint8_t> &bytes) {
  assert(!std::empty(bytes));

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
    throw klib::RuntimeError("'{}' is not a directory", path);
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

}  // namespace

ChangeWorkingDir::ChangeWorkingDir(const std::string &path) {
  if (!std::empty(path)) {
    backup_ = std::filesystem::current_path();

    if (!(std::filesystem::exists(path) &&
          std::filesystem::is_directory(path))) {
      if (!std::filesystem::create_directory(path)) {
        throw klib::RuntimeError("can not create directory: '{}'", path);
      }
    }

    if (chdir(path.c_str())) {
      throw klib::RuntimeError("chdir error");
    }
  }
}

ChangeWorkingDir::~ChangeWorkingDir() {
  if (!std::empty(backup_) && chdir(backup_.c_str())) {
    error("chdir error");
  }
}

std::string read_file(const std::string &path, bool binary_mode) {
  return read_file(path.c_str(), binary_mode);
}

std::string read_file(std::string_view path, bool binary_mode) {
  return read_file(path.data(), binary_mode);
}

std::string read_file(const char *path, bool binary_mode) {
  if (!std::filesystem::is_regular_file(path)) {
    throw klib::RuntimeError("'{}' is not a file", path);
  }

  std::ifstream ifs;
  if (binary_mode) {
    ifs.open(path, std::ifstream::binary);
  } else {
    ifs.open(path);
  }

  if (!ifs) {
    throw klib::RuntimeError("can not open file: '{}'", path);
  }

  std::string data;

  auto size = ifs.seekg(0, std::ifstream::end).tellg();
  data.resize(size);
  ifs.seekg(0, std::ifstream::beg).read(data.data(), size);

  return data;
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
    throw klib::RuntimeError("can not open file: '{}'", path);
  }

  ofs.write(content, length);
}

// https://zh.cppreference.com/w/c/string/multibyte/mbrtoc16
std::u16string utf8_to_utf16(const std::string &str) {
  assert(!std::empty(str));

  std::setlocale(LC_ALL, "en_US.utf8");

  std::u16string result;

  char16_t out = 0;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = std::mbrtoc16(&out, begin, size, &state)) {
    if (rc == static_cast<std::size_t>(-1)) {
      throw klib::RuntimeError(std::strerror(errno));
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
  assert(!std::empty(str));

  std::setlocale(LC_ALL, "en_US.utf8");

  std::u32string result;

  char32_t out = 0;
  auto begin = str.c_str();
  auto size = std::size(str);
  mbstate_t state = {};

  while (auto rc = std::mbrtoc32(&out, begin, size, &state)) {
    assert(rc != static_cast<std::size_t>(-3));

    if (rc == static_cast<std::size_t>(-1)) {
      throw klib::RuntimeError(std::strerror(errno));
    }

    if (rc > static_cast<std::size_t>(-1) / 2) {
      break;
    }

    begin += rc;
    result.push_back(out);
  }

  return result;
}

bool is_ascii(const std::string &str) {
  return std::all_of(std::begin(str), std::end(str),
                     [](char c) { return is_ascii(c); });
}

bool is_chinese(const std::string &c) {
  assert(!std::empty(c));

  auto utf32 = utf8_to_utf32(c);

  if (std::size(utf32) != 1) {
    throw klib::RuntimeError("not a UTF-32 encoded character: '{}'", c);
  }

  return is_chinese(utf32.front());
}

std::string sha3_512(const std::string &path) {
  auto data = read_file(path, true);

  std::uint32_t digest_length = SHA512_DIGEST_LENGTH;
  auto digest = static_cast<std::uint8_t *>(OPENSSL_malloc(digest_length));

  EVP_MD_CTX *context = EVP_MD_CTX_new();
  auto algorithm = EVP_sha3_512();
  EVP_DigestInit_ex(context, algorithm, nullptr);
  EVP_DigestUpdate(context, data.c_str(), std::size(data));
  EVP_DigestFinal_ex(context, digest, &digest_length);
  EVP_MD_CTX_destroy(context);

  auto output = bytes_to_hex_string(
      std::vector<std::uint8_t>(digest, digest + digest_length));

  OPENSSL_free(digest);

  return output;
}

std::size_t folder_size(const std::string &path) {
  if (!std::filesystem::is_directory(path)) {
    throw klib::RuntimeError("'{}' is not a directory", path);
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
  assert(!std::empty(command));
  execute_command(command.c_str());
}

void execute_command(const char *command) {
  assert(command);

  auto status = std::system(command);
  if (status == -1 || !WIFEXITED(status) || WEXITSTATUS(status)) {
    throw klib::RuntimeError("execute command error: '{}'", command);
  }
}

}  // namespace klib
