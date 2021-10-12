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
