#include "klib/image.h"

#include <cstdint>

#include <webp/decode.h>
#include <webp/encode.h>
#include <scope_guard.hpp>

#include "imageio/image_dec.h"
#include "imageio/imageio_util.h"
#include "klib/exception.h"

namespace klib {

namespace {

static int ReadYUV(const std::uint8_t* data, std::size_t data_size,
                   WebPPicture* const pic) {
  const std::int32_t use_argb = pic->use_argb;
  const std::int32_t uv_width = (pic->width + 1) / 2;
  const std::int32_t uv_height = (pic->height + 1) / 2;
  const std::int32_t y_plane_size = pic->width * pic->height;
  const std::int32_t uv_plane_size = uv_width * uv_height;
  const std::size_t expected_data_size = y_plane_size + 2 * uv_plane_size;

  if (data_size != expected_data_size) {
    throw klib::RuntimeError(
        "input data doesn't have the expected size ({} instead of {})",
        data_size, expected_data_size);
  }

  pic->use_argb = 0;
  if (!WebPPictureAlloc(pic)) {
    return 0;
  }

  ImgIoUtilCopyPlane(data, pic->width, pic->y, pic->y_stride, pic->width,
                     pic->height);
  ImgIoUtilCopyPlane(data + y_plane_size, uv_width, pic->u, pic->uv_stride,
                     uv_width, uv_height);
  ImgIoUtilCopyPlane(data + y_plane_size + uv_plane_size, uv_width, pic->v,
                     pic->uv_stride, uv_width, uv_height);

  return use_argb ? WebPPictureYUVAToARGB(pic) : 1;
}

}  // namespace

std::string image_to_webp(const std::string& image) {
  return image_to_webp(std::data(image), std::size(image));
}

std::string image_to_webp(std::string_view image) {
  return image_to_webp(std::data(image), std::size(image));
}

std::string image_to_webp(const char* image, std::size_t size) {
  WebPConfig config;
  WebPAuxStats stats;

  WebPPicture picture;
  SCOPE_EXIT {
    WebPFree(picture.extra_info);
    WebPPictureFree(&picture);
  };
  WebPPicture original_picture;
  SCOPE_EXIT { WebPPictureFree(&original_picture); };
  WebPMemoryWriter memory_writer;
  SCOPE_EXIT { WebPMemoryWriterClear(&memory_writer); };

  WebPMemoryWriterInit(&memory_writer);
  if (!WebPPictureInit(&picture) || !WebPPictureInit(&original_picture) ||
      !WebPConfigInit(&config)) {
    throw RuntimeError("webp init failed");
  }

  config.quality = 80;
  if (!WebPValidateConfig(&config)) {
    throw RuntimeError("webp Invalid configuration");
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
    throw RuntimeError("Could not process");
  }

  picture.writer = WebPMemoryWrite;
  picture.custom_ptr = (void*)&memory_writer;
  picture.stats = &stats;

  if (!WebPEncode(&config, &picture)) {
    throw RuntimeError("Cannot encode picture as WebP");
  }

  return {reinterpret_cast<const char*>(memory_writer.mem), memory_writer.size};
}

}  // namespace klib
