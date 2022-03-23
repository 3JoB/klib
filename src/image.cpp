#include "klib/image.h"

#include <vips/vips8>

#include "klib/exception.h"

namespace klib {

namespace {

vips::VImage get_image(const std::string &image_path) {
  if (VIPS_INIT("klib")) [[unlikely]] {
    throw RuntimeError("VIPS_INIT() failed");
  }

  return vips::VImage::new_from_file(image_path.c_str(), nullptr);
}

}  // namespace

void image_to_png(const std::string &image_path, const std::string &out_path,
                  std::int32_t quality, std::int32_t compression_level) {
  try {
    auto image = get_image(image_path);

    auto option = new vips::VOption;

    option->set("Q", quality);
    option->set("compression", compression_level);

    image.pngsave(out_path.c_str(), option);
  } catch (const vips::VError &err) {
    throw RuntimeError(err.what());
  }
}

void image_to_jpeg(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality, bool max_compress) {
  try {
    auto image = get_image(image_path);

    auto option = new vips::VOption;

    option->set("Q", quality);

    if (max_compress) {
      option->set("strip", true);
      option->set("optimize-coding", true);
      option->set("interlace", true);
      option->set("optimize-scans", true);
      option->set("trellis-quant", true);
      option->set("quant_table", 3);
    }

    image.jpegsave(out_path.c_str(), option);
  } catch (const vips::VError &err) {
    throw RuntimeError(err.what());
  }
}

void image_to_webp(const std::string &image_path, const std::string &out_path,
                   std::int32_t quality, std::int32_t method) {
  try {
    auto image = get_image(image_path);

    auto option = new vips::VOption;

    option->set("Q", quality);
    option->set("effort", method);

    image.webpsave(out_path.c_str(), option);
  } catch (const vips::VError &err) {
    throw RuntimeError(err.what());
  }
}

}  // namespace klib
