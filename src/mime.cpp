#include "klib/mime.h"

#include <magic.h>
#include <scope_guard.hpp>

#include "klib/exception.h"

namespace klib {

std::string mime(const std::string &file) {
  auto magic = magic_open(MAGIC_MIME_TYPE);
  SCOPE_EXIT { magic_close(magic); };
  if (!magic) {
    throw RuntimeError("Unable to initialize magic library");
  }

  if (magic_load(magic, nullptr) != 0) {
    throw RuntimeError("Cannot load magic database: {}", magic_error(magic));
  }

  return magic_buffer(magic, std::data(file), std::size(file));
}

bool is_png(const std::string &image) { return mime(image) == "image/png"; }

bool is_jpeg(const std::string &image) { return mime(image) == "image/jpeg"; }

bool is_webp(const std::string &image) { return mime(image) == "image/webp"; }

}  // namespace klib
