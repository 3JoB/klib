#include "klib/util.h"

#include <sys/wait.h>
#include <unistd.h>

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>

#include <openssl/mem.h>
#include <openssl/rand.h>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "klib/detail/openssl_util.h"
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

  if (!ifs) {
    throw RuntimeError("Can not open file: '{}'", path);
  }

  std::string data;

  auto size = ifs.seekg(0, std::ifstream::end).tellg();
  data.resize(size);
  ifs.seekg(0, std::ifstream::beg).read(std::data(data), size);

  return data;
}

std::vector<std::string> read_file_line(const std::string &path) {
  return read_file_line(path.c_str());
}

std::vector<std::string> read_file_line(std::string_view path) {
  return read_file_line(std::data(path));
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
  write_file(std::data(path), binary_mode, std::data(content),
             std::size(content));
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
    throw RuntimeError("Can not open file: '{}'", path);
  }

  ofs.write(content, length);
}

void exec(const std::string &cmd) { exec(cmd.c_str()); }

void exec(const char *cmd) {
  auto status = std::system(cmd);
  if (status == -1 || wait_error(status)) {
    throw RuntimeError(
        "When running command line '{}', error '{}' is encountered", cmd,
        std::strerror(errno));
  }
  if (status != 0) {
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
  if (!pipe) {
    throw RuntimeError("popen() failed");
  }

  char buffer[128];
  std::string result;
  while (std::fgets(buffer, 128, pipe) != nullptr) {
    result += buffer;
  }

  auto status = pclose(pipe);
  if (status == -1) {
    throw RuntimeError("pclose() failed");
  }
  if (status != 0) {
    throw RuntimeError("Failed when running command line '{}', status: {}", cmd,
                       status);
  }

  return result;
}

void exec_without_output(const std::string &cmd) {
  exec_with_output(cmd.c_str());
}

void exec_without_output(const char *cmd) {
  auto pipe = popen(cmd, "r");
  if (!pipe) {
    throw RuntimeError("popen() failed");
  }

  auto status = pclose(pipe);
  if (status == -1) {
    throw RuntimeError("pclose() failed");
  }
  if (status != 0) {
    throw RuntimeError("Failed when running command line '{}', status: {}", cmd,
                       status);
  }
}

void wait_for_child_process() {
  std::int32_t status = 0;

  while (waitpid(-1, &status, 0) > 0) {
    if (wait_error(status)) {
      throw RuntimeError("Waitpid error: {}", std::strerror(errno));
    }
  }
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

std::string make_file_or_dir_name_legal(const std::string &file_name) {
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

  // https://zh.cppreference.com/w/c/string/byte/iscntrl
  std::erase_if(copy, [](char c) { return std::iscntrl(c); });

  if (copy.ends_with('.')) {
    copy.pop_back();
  }
  boost::trim(copy);

  return copy;
}

std::string generate_random_bytes(std::int32_t num) {
  if (RAND_status() == 0) {
    check_openssl_return(RAND_poll());
  }

  std::string bytes;
  bytes.resize(num);
  check_openssl_return(RAND_bytes(
      reinterpret_cast<unsigned char *>(std::data(bytes)), std::size(bytes)));

  return bytes;
}

}  // namespace klib
