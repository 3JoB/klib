/**
 * @file archive.h
 * @brief Contains compression and decompression modules
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace klib::archive {

/**
 * @brief Algorithm used for compression
 */
enum class Algorithm { Zip, Gzip };

/**
 * @brief Compress file or folder
 * @param path: File or folder path
 * @param algorithm: Compression algorithm used
 * @param file_name: Compressed file name(If it is empty, the default file name
 * is used)
 * @param flag: Whether to include the outermost folder(If path refers to a
 * file, ignore it)
 */
void compress(const std::string &path, Algorithm algorithm,
              const std::string &file_name = "", bool flag = true);

/**
 * @brief Compress files or folders
 * @param paths: Files or folders path
 * @param algorithm: Compression algorithm used
 * @param file_name: Compressed file name
 */
void compress(const std::vector<std::string> &paths, Algorithm algorithm,
              const std::string &file_name);

/**
 * @brief Decompress file or folder
 * @param path: Compressed file path
 * @param decompressed_path: Specify the location of the decompressed content
 * @return Outermost folder name
 */
std::optional<std::string> decompress(
    const std::string &path, const std::string &decompressed_path = "");

}  // namespace klib::archive
