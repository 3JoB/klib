/**
 * @file html.h
 * @brief Contains HTML Tidy module
 */

#pragma once

#include <string>

namespace klib {

/**
 * @brief Execute HTML Tidy
 * @param html: The string containing the HTML document
 * @param ignore_error: Ignore error
 * @return Tidy HTML document
 */
std::string html_tidy(const std::string &html, bool ignore_error = false);

}  // namespace klib
