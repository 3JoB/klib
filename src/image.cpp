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

VipsImage *get_image(const RGB &rgb) {
  if (VIPS_INIT("klib")) [[unlikely]] {
    throw RuntimeError("VIPS_INIT() failed");
  }

  const auto width = rgb.width();
  const auto height = rgb.height();
  auto image = vips_image_new_from_memory(rgb.rgb(), width * height * 3, width,
                                          height, 3, VIPS_FORMAT_UCHAR);
  if (!image) {
    throw RuntimeError(vips_error_buffer());
  }

  return image;
}

void do_to_png(VipsImage *vips_image, const std::string &out_path,
               std::int32_t quality, std::int32_t compression_level) {
  auto rc = vips_pngsave(vips_image, out_path.c_str(), "Q", quality,
                         "compression", compression_level, nullptr);
  CHECK_LIBVIPS(rc);
}

void do_to_jpeg(VipsImage *vips_image, const std::string &out_path,
                std::int32_t quality, bool max_compress) {
  if (max_compress) {
    auto rc = vips_jpegsave(vips_image, out_path.c_str(), "Q", quality, "strip",
                            true, "optimize-coding", true, "interlace", true,
                            "optimize-scans", true, "trellis-quant", true,
                            "quant_table", 3, nullptr);
    CHECK_LIBVIPS(rc);
  } else {
    auto rc =
        vips_jpegsave(vips_image, out_path.c_str(), "Q", quality, nullptr);
    CHECK_LIBVIPS(rc);
  }
}

void do_to_webp(VipsImage *vips_image, const std::string &out_path,
                std::int32_t quality, std::int32_t method) {
  auto rc = vips_webpsave(vips_image, out_path.c_str(), "Q", quality, "effort",
                          method, nullptr);
  CHECK_LIBVIPS(rc);
}

}  // namespace

RGB::RGB(std::int32_t width, std::int32_t height)
    : width_(width), height_(height) {
  rgb_ = new std::uint8_t[height_ * width_ * 3];
}

RGB::RGB(std::uint8_t *rgb, std::int32_t width, std::int32_t height, bool free)
    : rgb_(rgb), width_(width), height_(height), free_(free) {}

RGB::~RGB() {
  if (free_) {
    delete[] rgb_;
  }
}

// https://en.wikipedia.org/wiki/List_of_file_signatures
bool is_png(const std::string &image) {
  return image.starts_with("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A");
}

bool is_jpeg(const std::string &image) {
  return image.starts_with("\xFF\xD8\xFF\xE0");
}

bool is_webp(const std::string &image) {
  return image.starts_with("RIFF") && image.substr(8, 4) == "WEBP";
}

void image_to_png(const std::string &image_path, const std::string &out_path,
                  std::int32_t quality, std::int32_t compression_level) {
  auto vips_image = get_image(image_path);
  SCOPE_EXIT { g_object_unref(vips_image); };
  return do_to_png(vips_image, out_path, quality, compression_level);
}

void rgb_to_png(const RGB &rgb, const std::string &out_path,
                std::int32_t quality, std::int32_t compression_level) {
  auto vips_image = get_image(rgb);
  SCOPE_EXIT { g_object_unref(vips_image); };
  return do_to_png(vips_image, out_path, quality, compression_level);
}

void image_to_jpeg(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality, bool max_compress) {
  auto vips_image = get_image(image_path);
  SCOPE_EXIT { g_object_unref(vips_image); };
  return do_to_jpeg(vips_image, out_path, quality, max_compress);
}

void rgb_to_jpeg(const RGB &rgb, const std::string &out_path,
                 std::int32_t quality, bool max_compress) {
  auto vips_image = get_image(rgb);
  SCOPE_EXIT { g_object_unref(vips_image); };
  return do_to_jpeg(vips_image, out_path, quality, max_compress);
}

void image_to_webp(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality, std::int32_t method) {
  auto vips_image = get_image(image_path);
  SCOPE_EXIT { g_object_unref(vips_image); };
  return do_to_webp(vips_image, out_path, quality, method);
}

void rgb_to_webp(const RGB &rgb, const std::string &out_path,
                 std::int32_t quality, std::int32_t method) {
  auto vips_image = get_image(rgb);
  SCOPE_EXIT { g_object_unref(vips_image); };
  return do_to_webp(vips_image, out_path, quality, method);
}

}  // namespace klib
