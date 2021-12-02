#include "klib/archive.h"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <unordered_set>

#include <archive.h>
#include <archive_entry.h>
#include <zstd.h>
#include <boost/core/ignore_unused.hpp>

#include "klib/exception.h"
#include "klib/util.h"

// https://github.com/libarchive/libarchive/wiki/Examples
// https://github.com/libarchive/libarchive/blob/master/examples/minitar/minitar.c
namespace klib {

namespace {

std::string get_top_level_dir(const std::filesystem::path &path) {
  if (path.has_parent_path()) {
    return get_top_level_dir(path.parent_path());
  }

  if (std::filesystem::is_directory(path)) {
    return path;
  }

  return {};
}

void check_file_exists(const std::string &path) {
  if (!std::filesystem::exists(path)) {
    throw RuntimeError("The file does not exist: '{}'", path);
  }

  if (!std::filesystem::is_regular_file(path)) {
    throw RuntimeError("The path does not correspond to a file: '{}'", path);
  }
}

void check_file_or_folder_exists(const std::string &path) {
  if (!std::filesystem::exists(path)) {
    throw RuntimeError("The file or folder does not exist: '{}'", path);
  }

  if (!std::filesystem::is_regular_file(path) &&
      !std::filesystem::is_directory(path)) {
    throw RuntimeError("The path does not correspond to a file or folder: '{}'",
                       path);
  }
}

void check_archive_correctness(std::int32_t code, struct archive *archive) {
  if (code != ARCHIVE_OK) {
    throw RuntimeError(archive_error_string(archive));
  }
}

void checked_archive_func(
    const std::function<std::int32_t(struct archive *)> &func,
    struct archive *archive) {
  check_archive_correctness(func(archive), archive);
}

auto create_unique_ptr(
    const std::function<struct archive *()> &init,
    const std::vector<std::function<std::int32_t(struct archive *)>>
        &free_func) {
  auto free_archive = [free_func](struct archive *archive) {
    for (const auto &func : free_func) {
      checked_archive_func(func, archive);
    }
  };

  auto archive = std::unique_ptr<struct archive, decltype(free_archive)>(
      init(), free_archive);

  if (!archive) {
    throw RuntimeError("create archive error");
  }

  return archive;
}

auto create_unique_ptr(
    const std::function<struct archive_entry *()> &init,
    const std::vector<std::function<void(struct archive_entry *)>> &free_func) {
  auto free_archive = [free_func](struct archive_entry *archive) {
    for (const auto &func : free_func) {
      func(archive);
    }
  };

  auto entry = std::unique_ptr<struct archive_entry, decltype(free_archive)>(
      init(), free_archive);

  if (!entry) {
    throw RuntimeError("create archive_entry error");
  }

  return entry;
}

std::string compressed_file_name(const std::string &path, Algorithm algorithm) {
  auto name = std::filesystem::path(path).filename();

  if (algorithm == Algorithm::Zip) {
    name += ".zip";
  } else if (algorithm == Algorithm::Gzip) {
    name += ".tar.gz";
  } else if (algorithm == Algorithm::Zstd) {
    name += ".tar.zst";
  } else {
    throw LogicError("Unknown algorithm");
  }

  return name;
}

void copy_data(struct archive *ar, struct archive *aw) {
  while (true) {
    const void *buff = nullptr;
    std::size_t size = 0;
    la_int64_t offset = 0;

    auto status = archive_read_data_block(ar, &buff, &size, &offset);
    if (status == ARCHIVE_EOF) {
      return;
    }
    if (status != ARCHIVE_OK) {
      throw RuntimeError(archive_error_string(ar));
    }

    check_archive_correctness(archive_write_data_block(aw, buff, size, offset),
                              aw);
  }
}

void check_zstd(std::size_t error) {
  if (ZSTD_isError(error)) {
    throw RuntimeError(ZSTD_getErrorName(error));
  }
}

}  // namespace

void compress(const std::string &path, Algorithm algorithm,
              const std::string &file_name, bool flag) {
  check_file_or_folder_exists(path);

  std::string out =
      (std::empty(file_name) ? compressed_file_name(path, algorithm)
                             : file_name);
  out = std::filesystem::current_path() / out;

  std::vector<std::string> paths;
  std::unique_ptr<ChangeWorkingDir> ptr;

  if (flag || std::filesystem::is_regular_file(path)) {
    paths.push_back(path);
  } else {
    ptr = std::make_unique<ChangeWorkingDir>(path);
    boost::ignore_unused(ptr);

    for (const auto &item :
         std::filesystem::directory_iterator(std::filesystem::current_path())) {
      paths.push_back(item.path().filename().string());
    }
  }

  compress(paths, algorithm, out);
}

void compress(const std::vector<std::string> &paths, Algorithm algorithm,
              const std::string &file_name) {
  for (const auto &path : paths) {
    check_file_or_folder_exists(path);
  }

  auto archive = create_unique_ptr(archive_write_new,
                                   {archive_write_close, archive_write_free});

  if (algorithm == Algorithm::Zip) {
    checked_archive_func(archive_write_set_format_zip, archive.get());
  } else if (algorithm == Algorithm::Gzip) {
    checked_archive_func(archive_write_set_format_gnutar, archive.get());
    checked_archive_func(archive_write_add_filter_gzip, archive.get());
  } else if (algorithm == Algorithm::Zstd) {
    checked_archive_func(archive_write_set_format_gnutar, archive.get());
    checked_archive_func(archive_write_add_filter_zstd, archive.get());
  } else {
    throw LogicError("Unknown algorithm");
  }

  check_archive_correctness(
      archive_write_open_filename(archive.get(), file_name.c_str()),
      archive.get());

  for (const auto &item : paths) {
    auto disk = create_unique_ptr(archive_read_disk_new,
                                  {archive_read_close, archive_read_free});

    checked_archive_func(archive_read_disk_set_standard_lookup, disk.get());
    check_archive_correctness(archive_read_disk_open(disk.get(), item.c_str()),
                              disk.get());

    while (true) {
      auto entry = create_unique_ptr(archive_entry_new, {archive_entry_free});

      auto status = archive_read_next_header2(disk.get(), entry.get());
      if (status == ARCHIVE_EOF) {
        break;
      }
      check_archive_correctness(status, disk.get());

      checked_archive_func(archive_read_disk_descend, disk.get());
      check_archive_correctness(
          archive_write_header(archive.get(), entry.get()), archive.get());

      std::string data;
      const auto source_path = archive_entry_sourcepath(entry.get());
      if (std::filesystem::is_regular_file(source_path)) {
        data = read_file(source_path, true);
      }
      archive_write_data(archive.get(), std::data(data), std::size(data));
    }
  }
}

std::optional<std::string> decompress(const std::string &file_name,
                                      const std::string &path,
                                      const std::string &password) {
  check_file_exists(file_name);

  std::int32_t flags = (ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
                        ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);

  auto archive = create_unique_ptr(archive_read_new,
                                   {archive_read_close, archive_read_free});
  checked_archive_func(archive_read_support_format_gnutar, archive.get());
  checked_archive_func(archive_read_support_format_zip, archive.get());
  checked_archive_func(archive_read_support_filter_gzip, archive.get());
  checked_archive_func(archive_read_support_filter_zstd, archive.get());
  if (!std::empty(password)) {
    archive_read_add_passphrase(archive.get(), password.c_str());
  }

  auto extract = create_unique_ptr(archive_write_disk_new,
                                   {archive_write_close, archive_write_free});
  check_archive_correctness(
      archive_write_disk_set_options(extract.get(), flags), extract.get());
  checked_archive_func(archive_write_disk_set_standard_lookup, extract.get());

  check_archive_correctness(
      archive_read_open_filename(archive.get(), file_name.c_str(), 10240),
      archive.get());

  ChangeWorkingDir change_work_dir(path);
  boost::ignore_unused(change_work_dir);

  std::unordered_set<std::string> dirs;
  while (true) {
    struct archive_entry *entry = nullptr;
    auto status = archive_read_next_header(archive.get(), &entry);
    if (status == ARCHIVE_EOF) {
      break;
    }
    if (status != ARCHIVE_OK) {
      throw RuntimeError(archive_error_string(archive.get()));
    }

    check_archive_correctness(archive_write_header(extract.get(), entry),
                              extract.get());

    dirs.insert(get_top_level_dir(archive_entry_pathname(entry)));

    if (archive_entry_size(entry) > 0) {
      copy_data(archive.get(), extract.get());
    }

    checked_archive_func(archive_write_finish_entry, extract.get());
  }

  if (std::size(dirs) == 1) {
    auto dir = *dirs.begin();
    if (dir.ends_with("/")) {
      dir = dir.substr(0, std::size(dir) - 1);
    }
    return dir;
  }

  return {};
}

std::string compress_str(const std::string &data) {
  return compress_str(std::data(data), std::size(data));
}

std::string compress_str(const char *data, std::size_t size) {
  auto compress_size = ZSTD_compressBound(size);
  std::string compress_data;
  compress_data.resize(compress_size);

  compress_size =
      ZSTD_compress(compress_data.data(), compress_size, data, size, 1);
  check_zstd(compress_size);
  compress_data.resize(compress_size);

  return compress_data;
}

std::string decompress_str(const std::string &data) {
  return decompress_str(std::data(data), std::size(data));
}

std::string decompress_str(const char *data, std::size_t size) {
  auto decompress_size = ZSTD_getFrameContentSize(data, size);
  if (decompress_size == ZSTD_CONTENTSIZE_ERROR) {
    throw RuntimeError("not compressed by zstd");
  } else if (decompress_size == ZSTD_CONTENTSIZE_UNKNOWN) {
    throw RuntimeError("original size unknown");
  }

  std::string decompress_data;
  decompress_data.resize(decompress_size);
  auto decompress_size_new =
      ZSTD_decompress(decompress_data.data(), decompress_size, data, size);
  check_zstd(decompress_size_new);

  if (decompress_size != decompress_size_new) {
    throw RuntimeError("Impossible because zstd will check this condition");
  }

  return decompress_data;
}

}  // namespace klib
