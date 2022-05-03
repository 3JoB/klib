#include "klib/mime.h"

#include <cerrno>
#include <cstring>

#include <magic.h>
#include <scope_guard.hpp>

#include "klib/exception.h"

namespace klib {

std::string mime(const std::string &file) {
  auto magic = magic_open(MAGIC_MIME_TYPE);
  SCOPE_EXIT { magic_close(magic); };
  if (!magic) [[unlikely]] {
    throw RuntimeError("Unable to initialize magic library: {}",
                       std::strerror(errno));
  }

  if (magic_load(magic, nullptr) != 0) [[unlikely]] {
    throw RuntimeError("Cannot load magic database: {}", magic_error(magic));
  }

  auto str = magic_buffer(magic, std::data(file), std::size(file));
  if (!str) [[unlikely]] {
    throw RuntimeError("magic_buffer() failed: {}", magic_error(magic));
  }

  return str;
}

bool is_png(const std::string &image) { return mime(image) == "image/png"; }

bool is_jpeg(const std::string &image) { return mime(image) == "image/jpeg"; }

bool is_webp(const std::string &image) { return mime(image) == "image/webp"; }

}  // namespace klib
