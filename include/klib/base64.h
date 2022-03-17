/**
 * @file base64.h
 * @brief Contains Base64 module
 */

#pragma once

#include <string>

#include "klib/config.h"

namespace klib {

/**
 * @brief Encode bytes using Base64 and return the encoded bytes
 * @param data: Bytes to be encoded
 * @return Encoded bytes
 */
std::string KLIB_EXPORT fast_base64_encode(const std::string &data);

/**
 * @brief Decode the Base64 encoded bytes and return the decoded bytes
 * @param data: Bytes to be decoded
 * @return Decoded bytes
 */
std::string KLIB_EXPORT fast_base64_decode(const std::string &data);

/**
 * @brief Encode bytes using Base64 and return the encoded bytes
 * @param data: Bytes to be encoded
 * @return Encoded bytes
 * @note https://zh.wikipedia.org/wiki/%E6%97%81%E8%B7%AF%E6%94%BB%E5%87%BB
 */
std::string KLIB_EXPORT secure_base64_encode(const std::string &data);

/**
 * @brief Decode the Base64 encoded bytes and return the decoded bytes
 * @param data: Bytes to be decoded
 * @return Decoded bytes
 * @note https://zh.wikipedia.org/wiki/%E6%97%81%E8%B7%AF%E6%94%BB%E5%87%BB
 */
std::string KLIB_EXPORT secure_base64_decode(const std::string &data);

}  // namespace klib
