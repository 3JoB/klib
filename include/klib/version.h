/**
 * @file version.h
 * @brief Contains the declaration of the macro
 */

#pragma once

#include <string>

/**
 * @brief klib major version
 */
#define KLIB_VER_MAJOR 0

/**
 * @brief klib minor version
 */
#define KLIB_VER_MINOR 7

/**
 * @brief klib patch version
 */
#define KLIB_VER_PATCH 8

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

/**
 * @brief klib version string
 */
#define KLIB_VERSION_STRING \
  "v" STRINGIZE(KLIB_VER_MAJOR) "." STRINGIZE(KLIB_VER_MINOR) "." STRINGIZE(KLIB_VER_PATCH)

namespace klib {

/**
 * @brief Get klib version string
 * @return klib version string
 */
inline std::string klib_version() { return KLIB_VERSION_STRING; }

}  // namespace klib

#undef STRINGIZE2
#undef STRINGIZE
