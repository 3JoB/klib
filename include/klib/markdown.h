/**
 * @file markdown.h
 * @brief Contains Markdown to HTML module
 */

#pragma once

#include <string>

#include "klib/config.h"

namespace klib {

/**
 * @brief Convert from CommonMark Markdown to HTML,
 * @param markdown: CommonMark Markdown to be converted
 * @return Converted HTML
 */
std::string KLIB_PUBLIC markdown_to_html(const std::string &markdown);

}  // namespace klib
