/**
 * @file util.h
 * @brief Contains declarations of some useful functions
 */

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace klib {

namespace detail {

// https://stackoverflow.com/questions/62531882/is-there-a-way-to-detect-chinese-characters-in-c-using-boost
template <char32_t a, char32_t b>
class UnicodeRange {
  static_assert(a <= b, "proper range");

 public:
  constexpr bool operator()(char32_t x) const noexcept {
    return x >= a && x <= b;
  }
};

using UnifiedIdeographs = UnicodeRange<0x4E00, 0x9FFF>;
using UnifiedIdeographsA = UnicodeRange<0x3400, 0x4DBF>;
using UnifiedIdeographsB = UnicodeRange<0x20000, 0x2A6DF>;
using UnifiedIdeographsC = UnicodeRange<0x2A700, 0x2B73F>;
using UnifiedIdeographsD = UnicodeRange<0x2B740, 0x2B81F>;
using UnifiedIdeographsE = UnicodeRange<0x2B820, 0x2CEAF>;
using CompatibilityIdeographs = UnicodeRange<0xF900, 0xFAFF>;
using CompatibilityIdeographsSupplement = UnicodeRange<0x2F800, 0x2FA1F>;

}  // namespace detail

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
 * @brief Convert UTF-32 encoded string to UTF-8 encoded string
 * @param c: UTF-32 encoded string
 * @return The converted UTF-8 encoded string
 */
std::string utf32_to_utf8(char32_t c);

std::string utf32_to_utf8(const std::u32string &str);

/**
 * @brief Determine whether it is an ASCII character
 * @param c: A character
 * @return If it is an ASCII character, return true, otherwise return false
 */
constexpr bool is_ascii(char c) { return static_cast<std::uint8_t>(c) <= 0x7F; }

constexpr bool is_ascii(char32_t c) {
  return static_cast<std::uint32_t>(c) <= 0x7F;
}

/**
 * @brief Determine whether a string consisting of ASCII characters
 * @param str: A string
 * @return If it is a string consisting of ASCII characters , return true,
 * otherwise return false
 */
bool is_ascii(const std::string &str);

bool is_ascii(const std::u32string &str);

/**
 * @brief Determine whether it is a Chinese character
 * @param c: A UTF-32 encoded character
 * @return If it is a Chinese character, return true, otherwise return false
 */
constexpr bool is_chinese(char32_t c) {
  using namespace detail;

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

/**
 * @brief Wait for all child processes to exit
 */
void wait_for_child_process();

/**
 * @brief Generate uuid
 */
std::string uuid();

}  // namespace klib
