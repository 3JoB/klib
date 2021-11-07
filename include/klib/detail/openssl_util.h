#pragma once

#include <cstdint>
#include <string>

namespace klib::detail {

std::string openssl_err_msg();

void check_openssl_return_value(std::int32_t rc);

std::string generate_random_bytes(std::int32_t num);

}  // namespace klib::detail
