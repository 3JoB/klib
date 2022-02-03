#pragma once

#include <string>

namespace klib {

std::string fast_base64_encode(const std::string &data);

std::string fast_base64_decode(const std::string &data);

/**
 * @brief Encode bytes using Base64 and return the encoded bytes
 * @param data: Bytes to be encoded
 * @return Encoded bytes
 */
std::string secure_base64_encode(const std::string &data);

/**
 * @brief Decode the Base64 encoded bytes and return the decoded bytes
 * @param data: Bytes to be decoded
 * @return Decoded bytes
 */
std::string secure_base64_decode(const std::string &data);

}  // namespace klib
