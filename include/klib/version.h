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
#define KLIB_VER_MINOR 7

/**
 * @brief klib patch version
 */
#define KLIB_VER_PATCH 13

/**
 * @brief klib version
 */
#define KLIB_VERSION \
  (KLIB_VER_MAJOR * 10000 + KLIB_VER_MINOR * 100 + KLIB_VER_PATCH)

#define KLIB_STRINGIZE2(s) #s
#define KLIB_STRINGIZE(s) KLIB_STRINGIZE2(s)

/**
 * @brief klib version string
 */
#define KLIB_VERSION_STRING      \
  KLIB_STRINGIZE(KLIB_VER_MAJOR) \
  "." KLIB_STRINGIZE(KLIB_VER_MINOR) "." KLIB_STRINGIZE(KLIB_VER_PATCH)
