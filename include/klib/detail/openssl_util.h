#pragma once

#include <cstdint>
#include <string>

namespace klib::detail {

std::string openssl_err_msg();

void check_openssl_return(std::int32_t rc);

}  // namespace klib::detail
