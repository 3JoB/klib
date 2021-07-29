/**
 * @file archive.h
 * @brief Contains compression and decompression modules
 */

#pragma once

#include <string>

namespace klib::archive {

enum class Algorithm { Zip, Gzip };

/**
 * @brief Use the zip algorithm to compress files or folders
 * @param path: File or folder path
 * (String cannot be empty)
 * @param flag: Whether to include the outermost folder
 */
void compress(const std::string &path, Algorithm algorithm, bool flag);

/**
 * @brief Use the zip algorithm to decompress the file
 * @param file_name: File path
 * (String cannot be empty)
 * @param path: Specify the location of the decompressed content
 */
void decompress(const std::string &file_name, const std::string &path = "");

}  // namespace klib::archive
