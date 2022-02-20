/**
 * @file version.h
 * @brief Contains the declaration of the macro
 */

#pragma once

/**
 * @brief klib major version
 */
#define KLIB_VER_MAJOR 0

/**
 * @brief klib minor version
 */
#define KLIB_VER_MINOR 14

/**
 * @brief klib patch version
 */
#define KLIB_VER_PATCH 9

#define KLIB_STRINGIZE2(s) #s
#define KLIB_STRINGIZE(s) KLIB_STRINGIZE2(s)

/**
 * @brief klib version string
 */
#define KLIB_VERSION_STRING      \
  KLIB_STRINGIZE(KLIB_VER_MAJOR) \
  "." KLIB_STRINGIZE(KLIB_VER_MINOR) "." KLIB_STRINGIZE(KLIB_VER_PATCH)
