/**
 * @file version.h
 * @brief Contains the declaration of the macro
 */

#pragma once

/**
 * @brief klib major version
 */
#define KLIB_VER_MAJOR 1

/**
 * @brief klib minor version
 */
#define KLIB_VER_MINOR 18

/**
 * @brief klib patch version
 */
#define KLIB_VER_PATCH 2

#define KLIB_STRINGIZE2(s) #s
#define KLIB_STRINGIZE(s) KLIB_STRINGIZE2(s)

/**
 * @brief klib version string
 */
#define KLIB_VERSION_STRING      \
  KLIB_STRINGIZE(KLIB_VER_MAJOR) \
  "." KLIB_STRINGIZE(KLIB_VER_MINOR) "." KLIB_STRINGIZE(KLIB_VER_PATCH)
