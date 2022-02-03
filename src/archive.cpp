#include "klib/archive.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <unordered_set>

#include <archive.h>
#include <archive_entry.h>
#include <zlib.h>
#include <zstd.h>
#include <boost/core/ignore_unused.hpp>
#include <scope_guard.hpp>

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

void check_archive(std::int32_t rc, archive *archive) {
  if (rc != ARCHIVE_OK) {
    throw RuntimeError(archive_error_string(archive));
  }
}

void check_compression_level(std::int32_t level, std::int32_t min,
                             std::int32_t max) {
  if (level < min) {
    throw InvalidArgument("The compression level cannot be less than {}: {}",
                          min, level);
  } else if (level > max) {
    throw InvalidArgument("The compression level cannot be greater than {}: {}",
                          max, level);
  }
}

void set_format_compression_level(archive *archive, std::int32_t level) {
  auto rc = archive_write_set_format_option(
      archive, nullptr, "compression-level", std::to_string(level).c_str());
  check_archive(rc, archive);
}

void set_filter_compression_level(archive *archive, std::int32_t level) {
  auto rc = archive_write_set_filter_option(
      archive, nullptr, "compression-level", std::to_string(level).c_str());
  check_archive(rc, archive);
}

void check_zstd(std::size_t rc) {
  if (ZSTD_isError(rc)) {
    throw RuntimeError(ZSTD_getErrorName(rc));
  }
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

void copy_data(archive *archive_read, archive *archive_write) {
  while (true) {
    const void *buff;
    std::size_t size;
    la_int64_t offset;

    auto rc = archive_read_data_block(archive_read, &buff, &size, &offset);
    if (rc == ARCHIVE_EOF) {
      return;
    }
    check_archive(rc, archive_read);

    rc = archive_write_data_block(archive_write, buff, size, offset);
    check_archive(rc, archive_write);
  }
}

}  // namespace

void compress(const std::string &path, Algorithm algorithm,
              const std::string &file_name, bool flag,
              std::optional<std::int32_t> level) {
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

  compress(paths, algorithm, out, level);
}

void compress(const std::vector<std::string> &paths, Algorithm algorithm,
              const std::string &file_name, std::optional<std::int32_t> level) {
  auto archive = archive_write_new();
  SCOPE_EXIT {
    archive_write_close(archive);
    archive_write_free(archive);
  };

  std::int32_t rc;

  if (algorithm == Algorithm::Zip) {
    rc = archive_write_set_format_zip(archive);
    check_archive(rc, archive);
    rc = archive_write_add_filter_none(archive);
    check_archive(rc, archive);

    std::int32_t compression_level = level ? *level : 6;
    check_compression_level(compression_level, Z_NO_COMPRESSION,
                            Z_BEST_COMPRESSION);
    set_format_compression_level(archive, compression_level);
  } else if (algorithm == Algorithm::Gzip) {
    rc = archive_write_set_format_gnutar(archive);
    check_archive(rc, archive);
    rc = archive_write_add_filter_gzip(archive);
    check_archive(rc, archive);

    std::int32_t compression_level = level ? *level : 6;
    check_compression_level(compression_level, Z_NO_COMPRESSION,
                            Z_BEST_COMPRESSION);
    set_filter_compression_level(archive, compression_level);
  } else if (algorithm == Algorithm::Zstd) {
    rc = archive_write_set_format_gnutar(archive);
    check_archive(rc, archive);
    rc = archive_write_add_filter_zstd(archive);
    check_archive(rc, archive);

    std::int32_t compression_level = level ? *level : ZSTD_defaultCLevel();
    check_compression_level(compression_level, ZSTD_minCLevel(),
                            ZSTD_maxCLevel());
    set_filter_compression_level(archive, compression_level);
  } else {
    throw LogicError("Unknown algorithm");
  }

  rc = archive_write_open_filename(archive, file_name.c_str());
  check_archive(rc, archive);

  for (const auto &item : paths) {
    auto disk = archive_read_disk_new();
    SCOPE_EXIT {
      archive_read_close(disk);
      archive_read_free(disk);
    };

    rc = archive_read_disk_set_standard_lookup(disk);
    check_archive(rc, disk);

    rc = archive_read_disk_open(disk, item.c_str());
    check_archive(rc, disk);

    while (true) {
      auto entry = archive_entry_new();
      SCOPE_EXIT { archive_entry_free(entry); };

      rc = archive_read_next_header2(disk, entry);
      if (rc == ARCHIVE_EOF) {
        break;
      }
      check_archive(rc, disk);

      rc = archive_read_disk_descend(disk);
      check_archive(rc, disk);

      rc = archive_write_header(archive, entry);
      check_archive(rc, archive);

      std::string data;
      if (auto source_path = archive_entry_sourcepath(entry);
          std::filesystem::is_regular_file(source_path)) {
        data = read_file(source_path, true);
      }
      archive_write_data(archive, std::data(data), std::size(data));
    }
  }
}

std::optional<std::string> decompress(const std::string &file_name,
                                      const std::string &path) {
  auto archive = archive_read_new();
  SCOPE_EXIT {
    archive_read_close(archive);
    archive_read_free(archive);
  };

  auto rc = archive_read_support_format_zip(archive);
  check_archive(rc, archive);

  rc = archive_read_support_format_gnutar(archive);
  check_archive(rc, archive);

  rc = archive_read_support_filter_none(archive);
  check_archive(rc, archive);

  rc = archive_read_support_filter_gzip(archive);
  check_archive(rc, archive);

  rc = archive_read_support_filter_zstd(archive);
  check_archive(rc, archive);

  auto extract = archive_write_disk_new();
  SCOPE_EXIT {
    archive_write_close(extract);
    archive_write_free(extract);
  };

  rc = archive_write_disk_set_options(
      extract, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
                   ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);
  check_archive(rc, extract);

  rc = archive_write_disk_set_standard_lookup(extract);
  check_archive(rc, extract);

  rc = archive_read_open_filename(archive, file_name.c_str(), 102400);
  check_archive(rc, extract);

  ChangeWorkingDir change_work_dir(path);
  boost::ignore_unused(change_work_dir);

  std::unordered_set<std::string> dirs;
  while (true) {
    archive_entry *entry;
    rc = archive_read_next_header(archive, &entry);
    if (rc == ARCHIVE_EOF) {
      break;
    }
    check_archive(rc, archive);

    rc = archive_write_header(extract, entry);
    check_archive(rc, extract);

    dirs.insert(get_top_level_dir(archive_entry_pathname(entry)));

    if (archive_entry_size(entry) > 0) {
      copy_data(archive, extract);
    }

    rc = archive_write_finish_entry(extract);
    check_archive(rc, extract);
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

std::string compress_str(const std::string &data,
                         std::optional<std::int32_t> level) {
  return compress_str(std::data(data), std::size(data), level);
}

std::string compress_str(const char *data, std::size_t size,
                         std::optional<std::int32_t> level) {
  std::int32_t compression_level = level ? *level : ZSTD_defaultCLevel();
  check_compression_level(compression_level, ZSTD_minCLevel(),
                          ZSTD_maxCLevel());

  auto compressed_size = ZSTD_compressBound(size);
  std::string compressed_data;
  compressed_data.resize(compressed_size);

  compressed_size = ZSTD_compress(std::data(compressed_data), compressed_size,
                                  data, size, compression_level);
  check_zstd(compressed_size);
  compressed_data.resize(compressed_size);

  return compressed_data;
}

std::string decompress_str(const std::string &data) {
  return decompress_str(std::data(data), std::size(data));
}

std::string decompress_str(const char *data, std::size_t size) {
  auto decompressed_size = ZSTD_getFrameContentSize(data, size);
  if (decompressed_size == ZSTD_CONTENTSIZE_ERROR) {
    throw RuntimeError("Not compressed by zstd");
  } else if (decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
    throw RuntimeError("Original size unknown");
  }

  std::string decompressed_data;
  decompressed_data.resize(decompressed_size);
  auto decompress_size_new = ZSTD_decompress(std::data(decompressed_data),
                                             decompressed_size, data, size);
  check_zstd(decompress_size_new);

  if (decompressed_size != decompress_size_new) {
    throw RuntimeError("Impossible because zstd will check this condition");
  }

  return decompressed_data;
}

}  // namespace klib
