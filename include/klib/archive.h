/**
 * @file archive.h
 * @brief Contains compression and decompression modules
 */

#pragma once

#include <string>

namespace klib::archive {

/**
 * @brief Algorithm used for compression
 */
enum class Algorithm { Zip, Gzip };

/**
 * @brief Compress file or folder
 * @param path: File or folder path
 * @param algorithm: Compression algorithm used
 * @param file_name: Compressed file name
 * @param flag: Whether to include the outermost folder(If the outermost folder
 * does not exist, ignore it)
 */
void compress(const std::string &path, Algorithm algorithm,
              const std::string &file_name, bool flag = true);

/**
 * @brief Decompress file or folder
 * @param path: Compressed file path
 * @param decompressed_path: Specify the location of the decompressed content
 * @return Outermost folder name(If it does not exist, return an empty string)
 */
std::string decompress(const std::string &path,
                       const std::string &decompressed_path = "");

}  // namespace klib::archive
