#include "klib/util.h"

#include <sys/ioctl.h>
#include <unistd.h>

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <openssl/mem.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "klib/detail/boringssl_util.h"
#include "klib/exception.h"
#include "klib/log.h"

namespace klib {

namespace {

bool wait_error(std::int32_t status) {
  return !WIFEXITED(status) || WEXITSTATUS(status);
}

}  // namespace

ChangeWorkingDir::ChangeWorkingDir(const std::string &path) {
  if (!std::empty(path)) {
    backup_ = std::filesystem::current_path();

    if (!std::filesystem::is_directory(path)) {
      if (!std::filesystem::create_directory(path)) [[unlikely]] {
        throw RuntimeError("Can not create directory: '{}'", path);
      }
    }

    if (chdir(path.c_str())) [[unlikely]] {
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
                                   const std::string &separate,
                                   bool remove_space) {
  std::vector<std::string> result;
  result.reserve(128);

  boost::split(result, str, boost::is_any_of(separate),
               boost::token_compress_on);

  if (remove_space) {
    for (auto &line : result) {
      boost::trim(line);
    }

    std::erase_if(result,
                  [](const std::string &line) { return std::empty(line); });
  }

  return result;
}

std::string read_file(const std::string &path, bool binary_mode) {
  return read_file(path.c_str(), binary_mode);
}

std::string read_file(std::string_view path, bool binary_mode) {
  return read_file(std::data(path), binary_mode);
}

std::string read_file(const char *path, bool binary_mode) {
  std::ifstream ifs;
  if (binary_mode) {
    ifs.open(path, std::ifstream::binary);
  } else {
    ifs.open(path);
  }

  if (!ifs) [[unlikely]] {
    throw RuntimeError("Can not open file: '{}'", path);
  }

  std::string data;

  auto size = ifs.seekg(0, std::ifstream::end).tellg();
  data.resize(size);
  ifs.seekg(0, std::ifstream::beg).read(std::data(data), size);

  return data;
}

void write_file(const std::string &path, bool binary_mode,
                const std::string &str) {
  write_file(path.c_str(), binary_mode, std::data(str), std::size(str));
}

void write_file(std::string_view path, bool binary_mode, std::string_view str) {
  write_file(std::data(path), binary_mode, std::data(str), std::size(str));
}

void write_file(const char *path, bool binary_mode, const char *str,
                std::size_t size) {
  std::ofstream ofs;
  if (binary_mode) {
    ofs.open(path, std::ofstream::binary);
  } else {
    ofs.open(path);
  }

  if (!ofs) [[unlikely]] {
    throw RuntimeError("Can not open file: '{}'", path);
  }

  ofs.write(str, size);
}

void exec(const std::string &cmd) { exec(cmd.c_str()); }

void exec(const char *cmd) {
  auto status = std::system(cmd);
  if (status == -1 || wait_error(status)) [[unlikely]] {
    throw RuntimeError(
        "When running command line '{}', error '{}' is encountered", cmd,
        std::strerror(errno));
  }
  if (status != 0) [[unlikely]] {
    throw RuntimeError("Failed when running command line '{}', status: {}", cmd,
                       status);
  }
}

std::string exec_with_output(const std::string &cmd) {
  return exec_with_output(cmd.c_str());
}

// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po?rq=1
std::string exec_with_output(const char *cmd) {
  auto pipe = popen(cmd, "r");
  if (!pipe) [[unlikely]] {
    throw RuntimeError("popen() failed");
  }

  char buffer[128];
  std::string result;
  while (std::fgets(buffer, 128, pipe) != nullptr) {
    result += buffer;
  }

  auto status = pclose(pipe);
  if (status == -1) [[unlikely]] {
    throw RuntimeError("pclose() failed");
  }
  if (status != 0) [[unlikely]] {
    throw RuntimeError("Failed when running command line '{}', status: {}", cmd,
                       status);
  }

  return result;
}

std::string uuid() {
  auto uuid = boost::uuids::random_generator{}();
  return boost::uuids::to_string(uuid);
}

void cleanse(std::string &data) {
  cleanse(static_cast<void *>(std::data(data)), std::size(data));
  data.clear();
  data.shrink_to_fit();
}

void cleanse(void *data, std::size_t size) { OPENSSL_cleanse(data, size); }

std::string make_file_name_legal(const std::string &file_name) {
  auto copy = file_name;

  boost::replace_all(copy, "<", " ");
  boost::replace_all(copy, ">", " ");
  boost::replace_all(copy, ":", " ");
  boost::replace_all(copy, "\"", " ");
  boost::replace_all(copy, "/", " ");
  boost::replace_all(copy, "\\", " ");
  boost::replace_all(copy, "|", " ");
  boost::replace_all(copy, "?", " ");
  boost::replace_all(copy, "*", " ");

  std::erase_if(copy, [](char c) { return std::iscntrl(c); });

  if (copy.ends_with('.')) {
    copy.pop_back();
  }
  boost::trim(copy);

  return copy;
}

std::string generate_random_bytes(std::size_t bytes) {
  std::int32_t rc;

  if (RAND_status() == 0) {
    rc = RAND_poll();
    CHECK_BORINGSSL(rc);
  }

  std::string result;
  result.resize(bytes);

  rc = RAND_bytes(reinterpret_cast<std::uint8_t *>(std::data(result)), bytes);
  CHECK_BORINGSSL(rc);

  return result;
}

// https://stackoverflow.com/questions/1022957/getting-terminal-width-in-c
std::pair<std::uint32_t, std::uint32_t> terminal_size() {
  winsize win_size;
  auto rc = ioctl(STDOUT_FILENO, TIOCGWINSZ, &win_size);
  if (rc != 0) [[unlikely]] {
    return {100, 25};
  }

  return {win_size.ws_col, win_size.ws_row};
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

}  // namespace klib
