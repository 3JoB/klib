#include "klib/image.h"

#include <png.h>
#include <webp/decode.h>
#include <webp/encode.h>
#include <scope_guard.hpp>

#include "imageio/image_dec.h"
#include "imageio/image_enc.h"
#include "imageio/yuvdec.h"
#include "klib/exception.h"

extern "C" void png_write_callback(png_structp png_ptr, png_bytep data,
                                   png_size_t length) {
  auto p = static_cast<std::string*>(png_get_io_ptr(png_ptr));
  p->append(reinterpret_cast<const char*>(data), length);
}

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
      !WebPConfigInit(&config)) [[unlikely]] {
    throw RuntimeError("libwebp: Library version mismatch");
  }

  config.quality = 80;
  config.lossless = lossless;

  auto rc = WebPValidateConfig(&config);
  if (!rc) [[unlikely]] {
    throw RuntimeError("libwebp: Invalid configuration");
  }

  if (lossless) {
    picture.use_argb = 1;
  } else {
    picture.use_argb = 0;
  }

  if (picture.width == 0 || picture.height == 0) [[likely]] {
    WebPImageReader reader = WebPGuessImageReader(
        reinterpret_cast<const std::uint8_t*>(image), size);
    rc = reader(reinterpret_cast<const std::uint8_t*>(image), size, &picture, 1,
                nullptr);
  } else [[unlikely]] {
    rc = ReadYUV(reinterpret_cast<const std::uint8_t*>(image), size, &picture);
  }
  if (!rc) [[unlikely]] {
    throw RuntimeError("libwebp: Could not process");
  }

  picture.writer = WebPMemoryWrite;
  picture.custom_ptr = (void*)&memory_writer;

  rc = WebPEncode(&config, &picture);
  if (!rc) [[unlikely]] {
    throw RuntimeError("libwebp: Cannot encode picture as WebP");
  }

  return {reinterpret_cast<const char*>(memory_writer.mem), memory_writer.size};
}

std::string webp_to_png(const std::string& image) {
  return webp_to_png(std::data(image), std::size(image));
}

std::string webp_to_png(std::string_view image) {
  return webp_to_png(std::data(image), std::size(image));
}

std::string webp_to_png(const char* image, std::size_t size) {
  WebPDecoderConfig config;

  auto output_buffer = &config.output;
  SCOPE_EXIT { WebPFreeDecBuffer(output_buffer); };

  auto bitstream = &config.input;

  auto rc = WebPInitDecoderConfig(&config);
  if (!rc) [[unlikely]] {
    throw RuntimeError("libwebp: Library version mismatch");
  }

  VP8StatusCode status;
  WebPBitstreamFeatures local_features;
  status = WebPGetFeatures(reinterpret_cast<const std::uint8_t*>(image), size,
                           bitstream == nullptr ? &local_features : bitstream);
  if (status != VP8_STATUS_OK) [[unlikely]] {
    throw RuntimeError("libwebp: {}", GetWebPError(status));
  }

  output_buffer->colorspace = bitstream->has_alpha ? MODE_RGBA : MODE_RGB;

  status =
      DecodeWebP(reinterpret_cast<const std::uint8_t*>(image), size, &config);
  if (status != VP8_STATUS_OK) [[unlikely]] {
    throw RuntimeError("libwebp: {}", GetWebPError(status));
  }

  std::string result;
  result.reserve(16384);
  rc = WebPWritePNGMemory(output_buffer, &result);
  if (!rc) [[unlikely]] {
    throw RuntimeError("libwebp: WebPSaveImage failed");
  }

  return result;
}

}  // namespace klib