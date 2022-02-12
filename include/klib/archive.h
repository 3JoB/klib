/**
 * @file archive.h
 * @brief Contains compression and decompression module
 */

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace klib {

/**
 * @brief Supported Archive Formats
 */
enum class Format { Zip, The7Zip, Tar };

/**
 * @brief Supported Compression Algorithms
 */
enum class Filter { None, Deflate, Gzip, Zstd };

/**
 * @brief Compress file or folder
 * @param path: File or folder path
 * @param format: Archive format
 * @param filter: Compression algorithm
 * @param out_name: Compressed file name
 * @param flag: Whether to include the outermost folder
 * @param password: Set password for compressed file(Can only be set for ZIP
 * format)
 */
void compress(const std::string &path, Format format = Format::Tar,
              Filter filter = Filter::Gzip, const std::string &out_name = "",
              bool flag = true, const std::string &password = "");

/**
 * @brief Compress files or folders
 * @param paths: Files or folders path
 * @param out_name: Compressed file name
 * @param format: Archive format
 * @param filter: Compression algorithm
 * @param password: Set password for compressed file(Can only be set for ZIP
 * format)
 */
void compress(const std::vector<std::string> &paths,
              const std::string &out_name, Format format = Format::Tar,
              Filter filter = Filter::Gzip, const std::string &password = "");

/**
 * @brief Decompress file
 * @param file_path: Compressed file name
 * @param out_dir: Specify the location of the decompressed content
 * @param password: Add decompression password
 */
void decompress(const std::string &file_path, const std::string &out_dir = "",
                const std::string &password = "");

/**
 * @brief Get the outermost folder name
 * @param file_path: Compressed file name
 * @return The outermost folder name
 */
std::optional<std::string> outermost_folder_name(const std::string &file_path);

/**
 * @brief Compress data
 * @param data: Data to be compressed
 * @return Compressed data
 */
std::string compress_data(const std::string &data);

/**
 * @brief Compress data
 * @param data: Data to be compressed
 * @param size: The size of the data to be compressed
 * @return Compressed data
 */
std::string compress_data(const char *data, std::size_t size);

/**
 * @brief Decompress data
 * @param data: Data to be decompressed
 * @return Decompressed data
 */
std::string decompress_data(const std::string &data);

/**
 * @brief Decompress data
 * @param data: Data to be decompressed
 * @param size: The size of the data to be decompressed
 * @return Decompressed data
 */
std::string decompress_data(const char *data, std::size_t size);

}  // namespace klib
