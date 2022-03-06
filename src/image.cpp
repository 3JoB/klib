#include "klib/image.h"

#include <webp/decode.h>
#include <webp/encode.h>
#include <scope_guard.hpp>

#include "imageio/image_dec.h"
#include "imageio/yuvdec.h"
#include "klib/exception.h"

namespace klib {

std::string image_to_webp(const std::string& image, bool lossless) {
  return image_to_webp(std::data(image), std::size(image), lossless);
}

std::string image_to_webp(std::string_view image, bool lossless) {
  return image_to_webp(std::data(image), std::size(image), lossless);
}

std::string image_to_webp(const char* image, std::size_t size, bool lossless) {
  WebPPicture picture;
  SCOPE_EXIT {
    WebPFree(picture.extra_info);
    WebPPictureFree(&picture);
  };

  WebPPicture original_picture;
  SCOPE_EXIT { WebPPictureFree(&original_picture); };

  WebPMemoryWriter memory_writer;
  SCOPE_EXIT { WebPMemoryWriterClear(&memory_writer); };

  WebPConfig config;

  WebPMemoryWriterInit(&memory_writer);
  if (!WebPPictureInit(&picture) || !WebPPictureInit(&original_picture) ||
      !WebPConfigInit(&config)) {
    throw RuntimeError("libwebp: Init failed");
  }

  config.quality = 80;
  config.lossless = lossless;

  if (!WebPValidateConfig(&config)) {
    throw RuntimeError("libwebp: Invalid configuration");
  }

  if (lossless) {
    picture.use_argb = 1;
  } else {
    picture.use_argb = 0;
  }

  bool ok;
  if (picture.width == 0 || picture.height == 0) {
    WebPImageReader reader = WebPGuessImageReader(
        reinterpret_cast<const std::uint8_t*>(image), size);
    ok = reader(reinterpret_cast<const std::uint8_t*>(image), size, &picture, 1,
                nullptr);
  } else {
    ok = ReadYUV(reinterpret_cast<const std::uint8_t*>(image), size, &picture);
  }
  if (!ok) {
    throw RuntimeError("libwebp: Could not process");
  }

  picture.writer = WebPMemoryWrite;
  picture.custom_ptr = (void*)&memory_writer;

  if (!WebPEncode(&config, &picture)) {
    throw RuntimeError("libwebp: Cannot encode picture as WebP");
  }

  return {reinterpret_cast<const char*>(memory_writer.mem), memory_writer.size};
}

}  // namespace klib
