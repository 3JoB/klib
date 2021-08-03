/**
 * @file util.h
 * @brief Contains declarations of some useful functions
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "klib/detail/unicode.h"

namespace klib::util {

/**
 * @brief Read a file at a time and store it in a string
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @return A string that holds the contents of the file
 */
std::string read_file(const std::string &path, bool binary_mode);

/**
 * @brief Write string to file
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @param content: String to be written
 */
void write_file(const std::string &path, bool binary_mode,
                const std::string &content);

/**
 * @brief Convert UTF-8 encoded string to UTF-16 encoded string
 * @param str: UTF-8 encoded string
 * @return The converted UTF-16 encoded string
 */
std::u16string utf8_to_utf16(const std::string &str);

/**
 * @brief Convert UTF-8 encoded string to UTF-32 encoded string
 * @param str: UTF-8 encoded string
 * @return The converted UTF-32 encoded string
 */
std::u32string utf8_to_utf32(const std::string &str);

/**
 * @brief Determine whether it is an ASCII character
 * @param c: A character
 * @return If it is an ASCII character, return true, otherwise return false
 */
constexpr bool is_ascii(char c) { return static_cast<std::uint8_t>(c) <= 0x7F; }

/**
 * @brief Determine whether it is a Chinese character
 * @param c: A UTF-32 encoded character
 * @return If it is a Chinese character, return true, otherwise return false
 */
constexpr bool is_chinese(char32_t c) {
  using namespace klib::detail;

  return UnifiedIdeographs{}(c) || UnifiedIdeographsA{}(c) ||
         UnifiedIdeographsB{}(c) || UnifiedIdeographsC{}(c) ||
         UnifiedIdeographsD{}(c) || UnifiedIdeographsE{}(c) ||
         CompatibilityIdeographs{}(c) || CompatibilityIdeographsSupplement{}(c);
}

/**
 * @brief Determine whether it is a Chinese character
 * @param c: A UTF-8 encoded string representing a UTF-32 encoded character
 * @return If it is a Chinese character, return true, otherwise return false
 */
bool is_chinese(const std::string &c);

/**
 * @brief Calculate SHA3-512
 * @param path: The path of the file to be calculated
 * @return SHA3-512 result
 */
std::string sha3_512(const std::string &path);

/**
 * @brief Count the sum of the size of all files in the folder
 * @param path: The path of the folder to be counted
 * @return Folder size
 */
std::size_t folder_size(const std::string &path);

/**
 * @brief Determine whether the two folders are the same
 * @param path1: Folder path
 * @param path2: Folder path
 * @return If the two folders are the same, return true, otherwise return false
 */
bool same_folder(const std::string &path1, const std::string &path2);

/**
 * @brief Execute the given line as a shell command
 * @param command: Command to be execute
 */
void execute_command(const std::string &command);

/**
 * @brief Execute the given line as a shell command
 * @param command: Command to be execute
 */
void execute_command(const char *command);

}  // namespace klib::util
