#include "klib/mime.h"

#include <cerrno>
#include <cstddef>
#include <cstring>

#include <magic.h>
#include <scope_guard.hpp>

#include "klib/exception.h"

extern char magic[];
extern int magic_size;

namespace klib {

std::string mime(const std::string &file) {
  auto cookie = magic_open(MAGIC_MIME_TYPE);
  SCOPE_EXIT { magic_close(cookie); };
  if (!cookie) [[unlikely]] {
    throw RuntimeError("Unable to initialize cookie library: {}",
                       std::strerror(errno));
  }

  void *bufs[1] = {magic};
  std::size_t sizes[1] = {static_cast<std::size_t>(magic_size)};
  if (magic_load_buffers(cookie, bufs, sizes, 1) != 0) [[unlikely]] {
    throw RuntimeError("Cannot load cookie database: {}", magic_error(cookie));
  }

  auto str = magic_buffer(cookie, std::data(file), std::size(file));
  if (!str) [[unlikely]] {
    throw RuntimeError("magic_buffer() failed: {}", magic_error(cookie));
  }

  return str;
}

bool is_png(const std::string &image) { return mime(image) == "image/png"; }

bool is_jpeg(const std::string &image) { return mime(image) == "image/jpeg"; }

bool is_webp(const std::string &image) { return mime(image) == "image/webp"; }

}  // namespace klib
