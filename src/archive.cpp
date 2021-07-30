#include "klib/archive.h"

#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

#include <archive.h>
#include <archive_entry.h>

#include "klib/detail/error.h"

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
      throw std::runtime_error(archive_error_string(ar));
    }

    status = archive_write_data_block(aw, buff, size, offset);
    if (status != ARCHIVE_OK) {
      throw std::runtime_error(archive_error_string(aw));
    }
  }
}

}  // namespace

namespace klib::archive {

void compress(const std::string &path, Algorithm algorithm, bool flag) {
  auto archive = archive_write_new();

  std::string out = std::filesystem::path(path).filename();
  if (algorithm == Algorithm::Zip) {
    archive_write_set_format_zip(archive);
    out += ".zip";
  } else if (algorithm == Algorithm::Gzip) {
    archive_write_set_format_gnutar(archive);
    archive_write_add_filter_gzip(archive);
    out += ".tar.gz";
  } else {
    assert(false);
  }

  if (archive_write_open_filename(archive, out.c_str()) != ARCHIVE_OK) {
    throw std::runtime_error(archive_error_string(archive));
  }

  std::vector<std::string> paths;
  std::unique_ptr<ChangeWorkDir> p;

  if (flag) {
    paths.push_back(path);
  } else {
    p = std::make_unique<ChangeWorkDir>(path);

    for (const auto &item :
         std::filesystem::directory_iterator(std::filesystem::current_path())) {
      paths.push_back(item.path().filename().string());
    }
  }

  for (const auto &item : paths) {
    auto disk = archive_read_disk_new();
    archive_read_disk_set_standard_lookup(disk);
    if (archive_read_disk_open(disk, item.c_str()) != ARCHIVE_OK) {
      throw std::runtime_error(archive_error_string(disk));
    }

    while (true) {
      auto entry = archive_entry_new();
      auto status = archive_read_next_header2(disk, entry);
      if (status == ARCHIVE_EOF) {
        archive_entry_free(entry);
        break;
      }
      if (status != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(disk));
      }

      archive_read_disk_descend(disk);

      status = archive_write_header(archive, entry);
      if (status != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
      }

      char buff[16384];
      auto file = std::fopen(archive_entry_sourcepath(entry), "rb");
      if (!file) {
        throw std::runtime_error(std::strerror(errno));
      }

      auto len = std::fread(buff, 1, sizeof(buff), file);
      while (len > 0) {
        archive_write_data(archive, buff, len);
        len = std::fread(buff, 1, sizeof(buff), file);
      }

      std::fclose(file);
      archive_entry_free(entry);
    }

    archive_read_close(disk);
    archive_read_free(disk);
  }

  archive_write_close(archive);
  archive_write_free(archive);
}

void decompress(const std::string &file_name, const std::string &path) {
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
}

}  // namespace klib::archive
