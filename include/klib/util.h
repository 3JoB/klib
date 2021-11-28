/**
 * @file util.h
 * @brief Contains declarations of some useful functions
 */

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace klib {

/**
 * @brief Change working directory
 */
class ChangeWorkingDir {
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
 * @brief Search for an element equal to value
 * @param first: Starting iterator
 * @param last: Post-tail iterator
 * @param value: Value to compare the elements to
 * @return Iterator to the first element satisfying the condition or last if no
 * such element is found
 */
template <typename InputIt, typename T>
requires std::same_as<typename InputIt::value_type, T>
constexpr inline InputIt find_last(InputIt first, InputIt last,
                                   const T &value) {
  for (; last != first;) {
    --last;

    if (*last == value) {
      return last;
    }
  }

  return last;
}

/**
 * @brief Search for an element for which predicate p returns true
 * @param first: Starting iterator
 * @param last: Post-tail iterator
 * @param p: Unary predicate which returns true for the required element
 * @return Iterator to the first element satisfying the condition or last if no
 * such element is found
 */
template <class InputIt, class UnaryPredicate>
requires std::predicate<UnaryPredicate, typename InputIt::value_type>
constexpr inline InputIt find_last(InputIt first, InputIt last,
                                   UnaryPredicate p) {
  for (; last != first;) {
    --last;

    if (p(*last)) {
      return last;
    }
  }

  return last;
}

/**
 * @brief Split string
 * @param str: String to be split
 * @param separate: Separate characters
 * @return Result after separation
 */
std::vector<std::string> split_str(const std::string &str,
                                   const std::string &separate);

/**
 * @brief Read a file at a time and store it in a string
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @return A string that holds the contents of the file
 */
std::string read_file(const std::string &path, bool binary_mode);

std::string read_file(std::string_view path, bool binary_mode);

std::string read_file(const char *path, bool binary_mode);

/**
 * @brief Read a file at a time and store it in a string
 * @param path: File path
 * @return A string that holds the contents of the file
 */
std::vector<std::string> read_file_line(const std::string &path);

std::vector<std::string> read_file_line(std::string_view path);

std::vector<std::string> read_file_line(const char *path);

/**
 * @brief Write string to file
 * @param path: File path
 * @param binary_mode: Whether to open in binary mode
 * @param content: String to be written
 */
void write_file(const std::string &path, bool binary_mode,
                const std::string &content);

void write_file(std::string_view path, bool binary_mode,
                std::string_view content);

void write_file(const char *path, bool binary_mode, const char *content,
                std::size_t length);

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
 * @param cmd: Command to be execute
 */
void exec(const std::string &cmd);

/**
 * @brief Execute the given line as a shell command
 * @param cmd: Command to be execute
 */
void exec(const char *cmd);

/**
 * @brief Execute the given line as a shell command and return its output
 * @param cmd: Command to be execute
 * @return Command line output
 */
std::string exec_with_output(const std::string &cmd);

/**
 * @brief Execute the given line as a shell command and return its output
 * @param cmd: Command to be execute
 * @return Command line output
 */
std::string exec_with_output(const char *cmd);

/**
 * @brief Execute the given line as a shell command and does not show its output
 * @param cmd: Command to be execute
 */
void exec_without_output(const std::string &cmd);

/**
 * @brief Execute the given line as a shell command and does not show its output
 * @param cmd: Command to be execute
 */
void exec_without_output(const char *cmd);

/**
 * @brief Wait for all child processes to exit
 */
void wait_for_child_process();

/**
 * @brief Generate uuid
 */
std::string uuid();

/**
 * @brief Clear data
 * @param data: Data to be cleared
 */
void cleanse(std::string &data);

/**
 * @brief Clear data
 * @param ptr: Point to the data to be cleared
 * @param size: The size of the data to be cleared
 */
void cleanse(void *ptr, std::size_t size);

/**
 * @brief According to the given file or directory name, return a legal file or
 * directory name
 * @param file_name: File or directory name
 * @return Legal file or directory name
 * @see
 * https://docs.microsoft.com/zh-cn/windows/win32/fileio/naming-a-file#file-and-directory-names
 */
std::string make_file_or_dir_name_legal(const std::string &file_name);

/**
 * @brief Generate random data of specified bytes
 * @param num: The number of bytes to generate
 * @return Random data generated
 */
std::string generate_random_bytes(std::int32_t num);

}  // namespace klib
