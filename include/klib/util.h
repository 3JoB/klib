/**
 * @file util.h
 * @brief Contains declarations of some useful functions
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "klib/config.h"

namespace klib {

/**
 * @brief Change working directory
 */
class KLIB_PUBLIC ChangeWorkingDir {
 public:
  /**
   * @brief Default constructor
   * @param path: Target working directory, do nothing if it is empty, create if
   * the directory does not exist
   */
  explicit ChangeWorkingDir(const std::string &path = "");

  ChangeWorkingDir(const ChangeWorkingDir &) = delete;
  ChangeWorkingDir(ChangeWorkingDir &&) = delete;
  ChangeWorkingDir &operator=(const ChangeWorkingDir &) = delete;
  ChangeWorkingDir &operator=(ChangeWorkingDir &&) = delete;

  /**
   * @brief Destructor
   */
  ~ChangeWorkingDir();

 private:
  std::string backup_;
};

/**
 * @brief Split string
 * @param str: String to be split
 * @param separate: Separate characters
 * @param remove_space: Whether to remove spaces and empty strings
 * @return Result after separation
 */
std::vector<std::string> KLIB_PUBLIC split_str(const std::string &str,
                                               const std::string &separate,
                                               bool remove_space = true);

/**
 * @brief Read a file at a time and store it in a string
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @return A string that holds the contents of the file
 */
std::string KLIB_PUBLIC read_file(const std::string &path, bool binary_mode);

/**
 * @brief Read a file at a time and store it in a string
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @return A string that holds the contents of the file
 */
std::string KLIB_PUBLIC read_file(std::string_view path, bool binary_mode);

/**
 * @brief Read a file at a time and store it in a string
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @return A string that holds the contents of the file
 */
std::string KLIB_PUBLIC read_file(const char *path, bool binary_mode);

/**
 * @brief Write string to file
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @param str: String to be written
 */
void KLIB_PUBLIC write_file(const std::string &path, bool binary_mode,
                            const std::string &str);

/**
 * @brief Write string to file
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @param str: String to be written
 */
void KLIB_PUBLIC write_file(std::string_view path, bool binary_mode,
                            std::string_view str);

/**
 * @brief Write string to file
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @param str: String to be written
 * @param size: String size
 */
void KLIB_PUBLIC write_file(const char *path, bool binary_mode, const char *str,
                            std::size_t size);

/**
 * @brief Execute the given line as a shell command
 * @param cmd: Command to be execute
 */
void KLIB_PUBLIC exec(const std::string &cmd);

/**
 * @brief Execute the given line as a shell command
 * @param cmd: Command to be execute
 */
void KLIB_PUBLIC exec(const char *cmd);

/**
 * @brief Execute the given line as a shell command and return its output
 * @param cmd: Command to be execute
 * @return Command line output
 */
std::string KLIB_PUBLIC exec_with_output(const std::string &cmd);

/**
 * @brief Execute the given line as a shell command and return its output
 * @param cmd: Command to be execute
 * @return Command line output
 */
std::string KLIB_PUBLIC exec_with_output(const char *cmd);

/**
 * @brief Wait for all child processes to exit
 */
void KLIB_PUBLIC wait_for_child_process();

/**
 * @brief Generate uuid
 */
std::string KLIB_PUBLIC uuid();

/**
 * @brief Clear data
 * @param data: Data to be cleared
 */
void KLIB_PUBLIC cleanse(std::string &data);

/**
 * @brief Clear data
 * @param ptr: Point to the data to be cleared
 * @param size: The size of the data to be cleared
 */
void KLIB_PUBLIC cleanse(void *ptr, std::size_t size);

/**
 * @brief According to the given file or directory name, return a legal file or
 * directory name
 * @param file_name: File or directory name
 * @return Legal file or directory name
 * @see
 * https://docs.microsoft.com/zh-cn/windows/win32/fileio/naming-a-file#file-and-directory-names
 */
std::string KLIB_PUBLIC make_file_name_legal(const std::string &file_name);

/**
 * @brief Generate random data of specified bytes
 * @param bytes: The number of bytes to generate
 * @return Random data generated
 */
std::string KLIB_PUBLIC generate_random_bytes(std::size_t bytes);

/**
 * @brief Get terminal size
 * @return Terminal length and width
 */
std::pair<std::uint32_t, std::uint32_t> KLIB_PUBLIC terminal_size();

}  // namespace klib
