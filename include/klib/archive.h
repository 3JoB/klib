/**
 * @file archive.h
 * @brief Contains compression and decompression functions
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
enum class Format { Zip, Tar };

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
 */
void compress(const std::string &path, Format format = Format::Tar,
              Filter filter = Filter::Gzip, const std::string &out_name = "",
              bool flag = true);

/**
 * @brief Compress files or folders
 * @param paths: Files or folders path
 * @param out_name: Compressed file name
 * @param format: Archive format
 * @param filter: Compression algorithm
 */
void compress(const std::vector<std::string> &paths,
              const std::string &out_name, Format format = Format::Tar,
              Filter filter = Filter::Gzip);

/**
 * @brief Decompress file
 * @param file_path: Compressed file name
 * @param out_dir: Specify the location of the decompressed content
 */
void decompress(const std::string &file_path, const std::string &out_dir = "");

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
