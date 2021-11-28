#pragma once

#include <cstdint>
#include <string>

namespace klib::detail {

std::string openssl_err_msg();

void check_openssl_return_1(std::int32_t rc);

void check_openssl_return_1_or_0(std::int32_t rc);

}  // namespace klib::detail
