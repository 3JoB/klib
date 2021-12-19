#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

#include <fmt/compile.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>
#include <gsl/gsl-lite.hpp>

#include "klib/archive.h"
#include "klib/crypto.h"
#include "klib/exception.h"
#include "klib/hash.h"
#include "klib/log.h"
#include "klib/util.h"
#include "klib/version.h"

namespace {

std::string version_str() {
  std::string result;

  result +=
      fmt::format(FMT_COMPILE("kencrypt version {}\n"), KLIB_VERSION_STRING);

  result += "Libraries: ";
  result += fmt::format(FMT_COMPILE("klib/{} "), KLIB_VERSION_STRING);
  result += fmt::format(FMT_COMPILE("gsl-lite/{}.{}.{} "), gsl_lite_MAJOR,
                        gsl_lite_MINOR, gsl_lite_PATCH);
  result += fmt::format(FMT_COMPILE("CLI11/{} "), CLI11_VERSION);
  result += fmt::format(FMT_COMPILE("fmt/{}.{}.{} "), FMT_VERSION / 10000,
                        FMT_VERSION / 100 % 100, FMT_VERSION % 100);
  result += fmt::format(FMT_COMPILE("spdlog/{}.{}.{}\n"), SPDLOG_VER_MAJOR,
                        SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);

  result += "Compiler: ";
#if defined(__clang_version__)
  result.append("Clang ").append(__clang_version__);
#elif defined(__GNUG__)
  result.append("GCC ").append(__VERSION__);
#else
  result.append("Unknown compiler");
#endif
  result += "\n";

  result.append("Build time: ").append(__DATE__).append(" ").append(__TIME__);

  return result;
}

std::string num_to_str(std::int32_t num) {
  Expects(num < 1'0000'0000 && num > 0);

  auto str = std::to_string(num);
  return std::string(8 - std::size(str), '0') + str;
}

void clean_str(std::string& str) {
  str.clear();
  str.shrink_to_fit();
}

void check_password(const std::string& password) {
  if (std::all_of(std::begin(password), std::end(password),
                  [](char c) { return std::isdigit(c); })) {
    klib::error("Digit pure number password is not secure");
  } else if (std::size(password) < 6) {
    klib::error("Password is too short");
  }
}

void do_encrypt(const std::string& file_path, const std::string& password) {
  auto file_name = std::filesystem::path(file_path).filename().string();
  auto file_size_str = num_to_str(std::size(file_name));
  auto file_content = klib::read_file(file_path, true);

  spdlog::info("Start compressing file");
  auto compressed =
      klib::compress_str(file_size_str + file_name + file_content);
  klib::cleanse(file_content);

  spdlog::info("Run Key derivation function");
  auto [key, salt] = klib::password_hash_raw(password);

  spdlog::info("Start encrypting file");
  auto encrypted = klib::aes_256_encrypt(compressed, key);
  klib::cleanse(key);
  clean_str(compressed);

  auto new_file_name = klib::make_file_or_dir_name_legal(
      klib::base64_encode(klib::generate_random_bytes(32)));
  klib::write_file(new_file_name, true, salt + encrypted);

  spdlog::info("File '{}' encrypted successfully", file_name);
}

void do_decrypt(const std::string& file_path, const std::string& password) {
  auto file_content = klib::read_file(file_path, true);
  auto salt = file_content.substr(0, 32);
  auto encrypted = file_content.substr(32);
  clean_str(file_content);

  spdlog::info("Run Key derivation function");
  auto key = klib::password_hash_raw(password, salt);
  klib::cleanse(salt);

  spdlog::info("Start decrypting file");
  std::string decrypted;
  try {
    decrypted = klib::aes_256_decrypt(encrypted, key);
    clean_str(encrypted);
  } catch (const klib::RuntimeError& err) {
    klib::error("Decryption failed, most likely the password is wrong");
  }
  klib::cleanse(key);

  spdlog::info("Start decompressing file");
  auto decompressed = klib::decompress_str(decrypted);
  klib::cleanse(decrypted);

  auto file_size = std::stoi(decompressed.substr(0, 8));
  auto file_name = decompressed.substr(8, file_size);
  auto content = decompressed.substr(8 + file_size);
  clean_str(decompressed);
  klib::write_file(file_name, true, content);

  spdlog::info("File '{}' decrypted successfully", file_name);
}

}  // namespace

int main(int argc, const char* argv[]) try {
  CLI::App app;
  app.set_version_flag("-v,--version", version_str());

  std::string file;
  app.add_option("file", file, "Files that need to be encrypted/decrypted")
      ->required();

  std::string password;
  app.add_option("-p,--password", password,
                 "Specify the password for encryption/decryption")
      ->required();

  bool decrypt = false;
  app.add_flag("-d,--decrypt", decrypt, "Decrypt file");

  CLI11_PARSE(app, argc, argv);

  if (decrypt) {
    do_decrypt(file, password);
  } else {
    check_password(password);
    do_encrypt(file, password);
  }
  klib::cleanse(file);
  klib::cleanse(password);
} catch (const std::exception& err) {
  klib::error(err.what());
} catch (...) {
  klib::error("Unknown exception");
}
