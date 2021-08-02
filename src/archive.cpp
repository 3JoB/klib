#include "klib/archive.h"

#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

#include <archive.h>
#include <archive_entry.h>
#include <fmt/compile.h>
#include <fmt/format.h>

#include "klib/detail/error.h"
#include "klib/exception.h"
#include "klib/util.h"

// https://github.com/libarchive/libarchive/wiki/Examples
// https://github.com/libarchive/libarchive/blob/master/examples/minitar/minitar.c
namespace {

class ChangeWorkDir {
 public:
  explicit ChangeWorkDir(const std::string &path = "") {
    if (!std::empty(path)) {
      backup_ = std::filesystem::current_path();

      if (!(std::filesystem::exists(path) &&
            std::filesystem::is_directory(path))) {
        if (!std::filesystem::create_directory(path)) {
          klib::detail::error("can not create directory: {}", path);
        }
      }

      if (chdir(path.c_str())) {
        klib::detail::error("chdir error");
      }
    }
  }

  ChangeWorkDir(const ChangeWorkDir &) = delete;
  ChangeWorkDir(ChangeWorkDir &&) = delete;
  ChangeWorkDir &operator=(const ChangeWorkDir &) = delete;
  ChangeWorkDir &operator=(ChangeWorkDir &&) = delete;

  ~ChangeWorkDir() {
    if (!std::empty(backup_) && chdir(backup_.c_str())) {
      klib::detail::error("chdir error");
    }
  }

 private:
  std::string backup_;
};

void copy_data(struct archive *ar, struct archive *aw) {
  while (true) {
    const void *buff;
    std::size_t size;
    la_int64_t offset;

    la_ssize_t status = archive_read_data_block(ar, &buff, &size, &offset);
    if (status == ARCHIVE_EOF) {
      return;
    }
    if (status != ARCHIVE_OK) {
      throw klib::RuntimeError(archive_error_string(ar));
    }

    status = archive_write_data_block(aw, buff, size, offset);
    if (status != ARCHIVE_OK) {
      throw klib::RuntimeError(archive_error_string(aw));
    }
  }
}

void check_file_or_folder(const std::string &path) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(
        fmt::format("The file or folder does not exist: '{}'", path));
  }

  if (!std::filesystem::is_regular_file(path) &&
      !std::filesystem::is_directory(path)) {
    throw std::runtime_error(fmt::format(
        "The path does not correspond to a file or folder: '{}'", path));
  }
}

void check_archive_correctness(std::int32_t code, struct archive *archive) {
  if (code != ARCHIVE_OK) {
    throw klib::RuntimeError(archive_error_string(archive));
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
    throw klib::RuntimeError("create archive error");
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
    throw klib::RuntimeError("create archive_entry error");
  }

  return entry;
}

}  // namespace

namespace klib::archive {

void compress(const std::string &path, Algorithm algorithm,
              const std::string &file_name, bool flag) {
  check_file_or_folder(path);

  auto archive = create_unique_ptr(archive_write_new,
                                   {archive_write_close, archive_write_free});

  if (algorithm == Algorithm::Zip) {
    checked_archive_func(archive_write_set_format_zip, archive.get());
  } else if (algorithm == Algorithm::Gzip) {
    checked_archive_func(archive_write_set_format_gnutar, archive.get());
    checked_archive_func(archive_write_add_filter_gzip, archive.get());
  } else {
    assert(false);
  }

  check_archive_correctness(
      archive_write_open_filename(archive.get(), file_name.c_str()),
      archive.get());

  std::vector<std::string> paths;
  std::unique_ptr<ChangeWorkDir> p;

  if (flag || std::filesystem::is_regular_file(path)) {
    paths.push_back(path);
  } else {
    p = std::make_unique<ChangeWorkDir>(path);
    (void)p;

    for (const auto &item :
         std::filesystem::directory_iterator(std::filesystem::current_path())) {
      paths.push_back(item.path().filename().string());
    }
  }

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
      auto source_path = archive_entry_sourcepath(entry.get());
      if (std::filesystem::is_regular_file(source_path)) {
        data = util::read_file(source_path, true);
      }
      archive_write_data(archive.get(), data.data(), std::size(data));
    }
  }
}

std::string decompress(const std::string &file_name, const std::string &path) {
  std::int32_t flags = (ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
                        ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);

  auto archive = archive_read_new();
  archive_read_support_format_gnutar(archive);
  archive_read_support_format_zip(archive);
  archive_read_support_filter_gzip(archive);

  auto extract = archive_write_disk_new();
  archive_write_disk_set_options(extract, flags);
  archive_write_disk_set_standard_lookup(extract);

#define FREE                      \
  do {                            \
    archive_read_close(archive);  \
    archive_read_free(archive);   \
    archive_write_close(extract); \
    archive_write_free(extract);  \
  } while (false)

  if (archive_read_open_filename(archive, file_name.c_str(), 10240) !=
      ARCHIVE_OK) {
    std::string msg = archive_error_string(archive);
    FREE;
    throw std::runtime_error(msg);
  }

  ChangeWorkDir change_work_dir(path);

  std::string dir;
  bool check = false;
  while (true) {
    struct archive_entry *entry;
    auto status = archive_read_next_header(archive, &entry);
    if (status == ARCHIVE_EOF) {
      break;
    }
    if (status != ARCHIVE_OK) {
      std::string msg = archive_error_string(archive);
      FREE;
      throw std::runtime_error(msg);
    }

    status = archive_write_header(extract, entry);
    if (status != ARCHIVE_OK) {
      std::string msg = archive_error_string(archive);
      FREE;
      throw std::runtime_error(msg);
    }
    if (!check) {
      dir = archive_entry_pathname(entry);
      check = true;
    } else if (!std::empty(dir)) {
      if (!std::string(archive_entry_pathname(entry)).starts_with(dir)) {
        dir.clear();
      }
    }

    if (archive_entry_size(entry) > 0) {
      try {
        copy_data(archive, extract);
      } catch (const std::runtime_error &error) {
        FREE;
        throw error;
      }
    }

    status = archive_write_finish_entry(extract);
    if (status != ARCHIVE_OK) {
      std::string msg = archive_error_string(archive);
      FREE;
      throw std::runtime_error(msg);
    }
  }

  FREE;
#undef FREE

  if (dir.ends_with("/")) {
    return dir.substr(0, std::size(dir) - 1);
  }

  return dir;
}

}  // namespace klib::archive
