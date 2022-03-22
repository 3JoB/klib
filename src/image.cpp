#include "klib/image.h"

#include <vips/vips8>

#include "klib/exception.h"

#ifdef KLIB_SANITIZER
#include <sanitizer/lsan_interface.h>
#endif

namespace klib {

class Image::ImageImpl {
 public:
  explicit ImageImpl(const std::string &file_name);

  ~ImageImpl();

  void to_png(const std::string &out_name, std::int32_t quality,
              std::int32_t compression_level);
  void to_jpeg(const std::string &out_name, std::int32_t quality,
               bool max_compress);
  void to_webp(const std::string &out_name, std::int32_t quality,
               std::int32_t method, bool lossless);

 private:
  vips::VImage image_;
};

Image::ImageImpl::ImageImpl(const std::string &file_name) {
#ifdef KLIB_SANITIZER
  __lsan_disable();
#endif

  if (VIPS_INIT("")) {
    throw RuntimeError("vips_init() failed");
  }

  image_ = vips::VImage::new_from_file(file_name.c_str(), nullptr);
}

Image::ImageImpl::~ImageImpl() {
#ifdef KLIB_SANITIZER
  __lsan_enable();
#endif
}

void Image::ImageImpl::to_png(const std::string &out_name, std::int32_t quality,
                              std::int32_t compression_level) {
  auto option = new vips::VOption;

  option->set("Q", quality);
  option->set("compression", compression_level);

  image_.pngsave(out_name.c_str(), option);
}

void Image::ImageImpl::to_jpeg(const std::string &out_name,
                               std::int32_t quality, bool max_compress) {
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

  image_.jpegsave(out_name.c_str(), option);
}

void Image::ImageImpl::to_webp(const std::string &out_name,
                               std::int32_t quality, std::int32_t method,
                               bool lossless) {
  auto option = new vips::VOption;

  option->set("Q", quality);
  option->set("effort", method);

  if (lossless) {
    option->set("lossless", true);
  }

  image_.webpsave(out_name.c_str(), option);
}

Image::Image(const std::string &file_name)
    : impl_(std::make_unique<ImageImpl>(file_name)) {}

Image::~Image() = default;

void Image::to_png(const std::string &out_name, std::int32_t quality,
                   std::int32_t compression_level) {
  impl_->to_png(out_name, quality, compression_level);
}

void Image::to_jpeg(const std::string &out_name, std::int32_t quality,
                    bool max_compress) {
  impl_->to_jpeg(out_name, quality, max_compress);
}

void Image::to_webp(const std::string &out_name, std::int32_t quality,
                    std::int32_t method, bool lossless) {
  impl_->to_webp(out_name, quality, method, lossless);
}

}  // namespace klib
