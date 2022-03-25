#include "klib/image.h"

#include <vips/vips.h>
#include <scope_guard.hpp>

#include "klib/exception.h"

#define CHECK_LIBVIPS(rc)                      \
  do {                                         \
    if (rc != 0) [[unlikely]] {                \
      throw RuntimeError(vips_error_buffer()); \
    }                                          \
  } while (0)

namespace klib {

namespace {

VipsImage *get_image(const std::string &image_path) {
  if (VIPS_INIT("klib")) [[unlikely]] {
    throw RuntimeError("VIPS_INIT() failed");
  }

  auto image = vips_image_new_from_file(image_path.c_str(), nullptr);
  if (!image) {
    throw RuntimeError(vips_error_buffer());
  }

  return image;
}

}  // namespace

void image_to_png(const std::string &image_path, const std::string &out_path,
                  std::int32_t quality, std::int32_t compression_level) {
  auto image = get_image(image_path);
  SCOPE_EXIT { g_object_unref(image); };

  auto rc = vips_pngsave(image, out_path.c_str(), "Q", quality, "compression",
                         compression_level, nullptr);
  CHECK_LIBVIPS(rc);
}

void image_to_jpeg(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality, bool max_compress) {
  auto image = get_image(image_path);
  SCOPE_EXIT { g_object_unref(image); };

  if (max_compress) {
    auto rc = vips_jpegsave(image, out_path.c_str(), "Q", quality, "strip",
                            true, "optimize-coding", true, "interlace", true,
                            "optimize-scans", true, "trellis-quant", true,
                            "quant_table", 3, nullptr);
    CHECK_LIBVIPS(rc);
  } else {
    auto rc = vips_jpegsave(image, out_path.c_str(), "Q", quality, nullptr);
    CHECK_LIBVIPS(rc);
  }
}

void image_to_webp(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality, std::int32_t method) {
  auto image = get_image(image_path);
  SCOPE_EXIT { g_object_unref(image); };

  auto rc = vips_webpsave(image, out_path.c_str(), "Q", quality, "effort",
                          method, nullptr);
  CHECK_LIBVIPS(rc);
}

}  // namespace klib
