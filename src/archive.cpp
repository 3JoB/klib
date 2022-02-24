/**
 * @see
 * https://github.com/libarchive/libarchive/blob/master/examples/minitar/minitar.c
 * @see https://github.com/libarchive/libarchive/wiki/Examples
 * @see https://github.com/facebook/zstd/blob/dev/examples/simple_compression.c
 * @see
 * https://github.com/facebook/zstd/blob/dev/examples/simple_decompression.c
 */

#include "klib/archive.h"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <thread>
#include <unordered_set>

#include <archive.h>
#include <archive_entry.h>
#include <dbg.h>
#include <zstd.h>
#include <boost/core/ignore_unused.hpp>
#include <scope_guard.hpp>

#include "klib/exception.h"
#include "klib/util.h"

#define check_system_io(rc)                     \
  do {                                          \
    if (rc == -1) {                             \
      throw RuntimeError(std::strerror(errno)); \
    }                                           \
  } while (0)

#define check_libarchive(rc, archive)                    \
  do {                                                   \
    if (rc < ARCHIVE_OK) {                               \
      throw RuntimeError(archive_error_string(archive)); \
    }                                                    \
  } while (0)

#define check_zstd(rc)                           \
  do {                                           \
    if (ZSTD_isError(rc)) {                      \
      throw RuntimeError(ZSTD_getErrorName(rc)); \
    }                                            \
  } while (0)

namespace klib {

namespace {

std::string compressed_file_name(const std::string &path, Format format,
                                 Filter filter) {
  auto name = std::filesystem::path(path).filename().string();

  if (format == Format::Zip) {
    return name + ".zip";
  } else if (format == Format::The7Zip) {
    return name + ".7z";
  } else if (format == Format::Tar) {
    if (filter == Filter::None) {
      return name + ".tar";
    } else if (filter == Filter::Deflate || filter == Filter::Gzip) {
      return name + ".tar.gz";
    } else if (filter == Filter::Zstd) {
      return name + ".tar.zst";
    }
  }

  throw InvalidArgument("Unknown format or filter");
}

void init_write_format_filter(archive *archive, Format format, Filter filter) {
  std::int32_t rc;

  if (format == Format::Zip) {
    rc = archive_write_set_format_zip(archive);
    check_libarchive(rc, archive);

    if (filter == Filter::None) {
      rc = archive_write_zip_set_compression_store(archive);
      check_libarchive(rc, archive);
    } else if (filter == Filter::Deflate) {
      rc = archive_write_zip_set_compression_deflate(archive);
      check_libarchive(rc, archive);
    } else {
      throw InvalidArgument(
          "Filter other than Deflate should not be used in the ZIP archive "
          "format");
    }
  } else if (format == Format::The7Zip) {
    rc = archive_write_set_format_7zip(archive);
    check_libarchive(rc, archive);

    if (filter == Filter::None) {
      rc = archive_write_set_format_option(archive, "7zip", "compression",
                                           "store");
      check_libarchive(rc, archive);
    } else if (filter == Filter::Deflate) {
      rc = archive_write_set_format_option(archive, "7zip", "compression",
                                           "deflate");
      check_libarchive(rc, archive);
    } else {
      throw InvalidArgument(
          "Filter other than Deflate should not be used in the 7-Zip archive "
          "format");
    }
  } else if (format == Format::Tar) {
    rc = archive_write_set_format_gnutar(archive);
    check_libarchive(rc, archive);

    if (filter == Filter::None) {
      rc = archive_write_add_filter_none(archive);
      check_libarchive(rc, archive);
    } else if (filter == Filter::Deflate || filter == Filter::Gzip) {
      rc = archive_write_add_filter_gzip(archive);
      check_libarchive(rc, archive);
    } else if (filter == Filter::Zstd) {
      rc = archive_write_add_filter_zstd(archive);
      check_libarchive(rc, archive);

      auto hardware_thread =
          std::to_string(std::thread::hardware_concurrency());
      dbg(hardware_thread);
      rc = archive_write_set_filter_option(archive, "zstd", "threads",
                                           hardware_thread.c_str());
      check_libarchive(rc, archive);
    }
  }
}

void init_read_format_filter(archive *archive) {
  auto rc = archive_read_support_format_zip(archive);
  check_libarchive(rc, archive);

  rc = archive_read_support_format_7zip(archive);
  check_libarchive(rc, archive);

  rc = archive_read_support_format_gnutar(archive);
  check_libarchive(rc, archive);

  rc = archive_read_support_format_rar5(archive);
  check_libarchive(rc, archive);

  rc = archive_read_support_filter_none(archive);
  check_libarchive(rc, archive);

  rc = archive_read_support_filter_gzip(archive);
  check_libarchive(rc, archive);

  rc = archive_read_support_filter_zstd(archive);
  check_libarchive(rc, archive);
}

std::string get_top_level_dir(const std::filesystem::path &path) {
  if (path.has_parent_path()) {
    return get_top_level_dir(path.parent_path());
  }

  return path;
}

void copy_data(archive *archive_read, archive *archive_write) {
  std::int32_t rc;
  const void *buff;
  std::size_t size;
  la_int64_t offset;

  while (true) {
    rc = archive_read_data_block(archive_read, &buff, &size, &offset);
    if (rc == ARCHIVE_EOF) {
      return;
    }
    check_libarchive(rc, archive_read);

    rc = archive_write_data_block(archive_write, buff, size, offset);
    check_libarchive(rc, archive_write);
  }
}

}  // namespace

void compress(const std::string &path, Format format, Filter filter,
              const std::string &out_name, bool flag,
              const std::string &password) {
  if (!std::empty(password) && format != Format::Zip) {
    throw InvalidArgument("This format does not support encryption");
  }

  std::string name =
      (std::empty(out_name) ? compressed_file_name(path, format, filter)
                            : out_name);
  name = std::filesystem::current_path() / name;

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

  compress(paths, name, format, filter, password);
}

void compress(const std::vector<std::string> &paths,
              const std::string &out_name, Format format, Filter filter,
              const std::string &password) {
  auto archive = archive_write_new();
  SCOPE_EXIT {
    archive_write_close(archive);
    archive_write_free(archive);
  };

  init_write_format_filter(archive, format, filter);

  if (!std::empty(password)) {
    auto rc =
        archive_write_set_format_option(archive, "zip", "encryption", "aes256");
    check_libarchive(rc, archive);
    rc = archive_write_set_passphrase(archive, password.c_str());
    check_libarchive(rc, archive);
  }

  auto rc = archive_write_open_filename(archive, out_name.c_str());
  check_libarchive(rc, archive);

  for (const auto &path : paths) {
    auto disk = archive_read_disk_new();
    SCOPE_EXIT {
      archive_read_close(disk);
      archive_read_free(disk);
    };

    rc = archive_read_disk_set_standard_lookup(disk);
    check_libarchive(rc, disk);

    rc = archive_read_disk_open(disk, path.c_str());
    check_libarchive(rc, disk);

    while (true) {
      auto entry = archive_entry_new();
      SCOPE_EXIT { archive_entry_free(entry); };

      rc = archive_read_next_header2(disk, entry);
      if (rc == ARCHIVE_EOF) {
        break;
      }
      check_libarchive(rc, disk);

      rc = archive_read_disk_descend(disk);
      check_libarchive(rc, disk);

      rc = archive_write_header(archive, entry);
      check_libarchive(rc, archive);

      auto source_path = archive_entry_sourcepath(entry);
      if (std::filesystem::is_regular_file(source_path)) {
        auto fd = open(source_path, O_RDONLY);
        SCOPE_EXIT { close(fd); };
        check_system_io(fd);

        char buff[16384];
        auto length = read(fd, buff, sizeof(buff));
        check_system_io(length);
        while (length > 0) {
          rc = archive_write_data(archive, buff, length);
          check_libarchive(rc, archive);

          length = read(fd, buff, sizeof(buff));
          check_system_io(length);
        }
      }
    }
  }
}

void decompress(const std::string &file_name, const std::string &out_dir,
                const std::string &password) {
  auto archive = archive_read_new();
  SCOPE_EXIT {
    archive_read_close(archive);
    archive_read_free(archive);
  };

  init_read_format_filter(archive);

  if (!std::empty(password)) {
    auto rc = archive_read_add_passphrase(archive, password.c_str());
    check_libarchive(rc, archive);
  }

  auto rc = archive_read_open_filename(archive, file_name.c_str(), 10240);
  check_libarchive(rc, archive);

  auto extract = archive_write_disk_new();
  SCOPE_EXIT {
    archive_write_close(extract);
    archive_write_free(extract);
  };

  rc = archive_write_disk_set_options(
      extract, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
                   ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);
  check_libarchive(rc, extract);

  rc = archive_write_disk_set_standard_lookup(extract);
  check_libarchive(rc, extract);

  ChangeWorkingDir change_work_dir(out_dir);
  boost::ignore_unused(change_work_dir);

  while (true) {
    archive_entry *entry;
    rc = archive_read_next_header(archive, &entry);
    if (rc == ARCHIVE_EOF) {
      break;
    }
    check_libarchive(rc, archive);

    rc = archive_write_header(extract, entry);
    check_libarchive(rc, extract);

    if (archive_entry_size(entry) > 0) {
      copy_data(archive, extract);
    }

    rc = archive_write_finish_entry(extract);
    check_libarchive(rc, extract);
  }
}

std::optional<std::string> outermost_folder_name(const std::string &file_name) {
  auto archive = archive_read_new();
  SCOPE_EXIT {
    archive_read_close(archive);
    archive_read_free(archive);
  };

  init_read_format_filter(archive);

  auto rc = archive_read_open_filename(archive, file_name.c_str(), 10240);
  check_libarchive(rc, archive);

  std::unordered_set<std::string> dirs;
  while (true) {
    archive_entry *entry;
    rc = archive_read_next_header(archive, &entry);
    if (rc == ARCHIVE_EOF) {
      break;
    }
    check_libarchive(rc, archive);

    dirs.insert(get_top_level_dir(archive_entry_pathname(entry)));
  }

  if (std::size(dirs) == 1) {
    return *dirs.begin();
  }

  return {};
}

std::string compress_data(const std::string &data) {
  return compress_data(std::data(data), std::size(data));
}

std::string compress_data(const char *data, std::size_t size) {
  std::string result;

  auto max_size = ZSTD_compressBound(size);
  result.resize(max_size);

  auto length = ZSTD_compress(std::data(result), max_size, data, size,
                              ZSTD_defaultCLevel());
  check_zstd(length);
  result.resize(length);

  return result;
}

std::string decompress_data(const std::string &data) {
  return decompress_data(std::data(data), std::size(data));
}

std::string decompress_data(const char *data, std::size_t size) {
  std::string result;

  auto length = ZSTD_getFrameContentSize(data, size);
  if (length == ZSTD_CONTENTSIZE_ERROR) {
    throw RuntimeError("Not compressed by zstd");
  } else if (length == ZSTD_CONTENTSIZE_UNKNOWN) {
    throw RuntimeError("Original size unknown");
  }
  result.resize(length);

  auto rc = ZSTD_decompress(std::data(result), length, data, size);
  check_zstd(rc);

  return result;
}

}  // namespace klib
