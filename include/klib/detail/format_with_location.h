#pragma once

#include <cstdint>
#include <string_view>

#include <spdlog/common.h>

#ifdef __cpp_lib_source_location
#include <source_location>
using source_location = std::source_location;
#else
#include <experimental/source_location>
using source_location = std::experimental::source_location;
#endif

namespace klib::detail {

[[nodiscard]] constexpr auto get_log_source_location(
    const source_location &location) {
  return spdlog::source_loc{location.file_name(),
                            static_cast<std::int32_t>(location.line()),
                            location.function_name()};
}

struct format_with_location {
  template <typename String>
  format_with_location(const String &fmt,
                       const source_location &loc = source_location::current())
      : fmt_(fmt), loc_(get_log_source_location(loc)) {}

  std::string_view fmt_;
  spdlog::source_loc loc_;
};

}  // namespace klib::detail
