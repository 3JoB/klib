/**
 * @file archive.h
 * @brief Contains compression and decompression modules
 */

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace klib {

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
 * @brief Decompress file
 * @param path: Compressed file path
 * @param decompressed_path: Specify the location of the decompressed content(If
 * it is empty, decompress to the current directory)
 * @return Outermost folder name(If there is not only one folder, then empty)
 */
std::optional<std::string> decompress(
    const std::string &path, const std::string &decompressed_path = "");

/**
 * @brief Use Zstandard algorithm to compress data
 * @param data: Data to be compressed
 * @return Compressed data
 */
std::string compress_str(const std::string &data);

/**
 * @brief Use Zstandard algorithm to compress data
 * @param data: Data to be compressed
 * @param size: The size of the data to be compressed
 * @return Compressed data
 */
std::string compress_str(const char *data, std::size_t size);

/**
 * @brief Use Zstandard algorithm to decompress data
 * @param data: Data to be decompressed
 * @return Decompressed data
 */
std::string decompress_str(const std::string &data);

/**
 * @brief Use Zstandard algorithm to decompress data
 * @param data: Data to be decompressed
 * @param size: The size of the data to be decompressed
 * @return Decompressed data
 */
std::string decompress_str(const char *data, std::size_t size);

}  // namespace klib
