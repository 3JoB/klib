#include "klib/image.h"

#include <cstdlib>

#include <jpeglib.h>
#include <png.h>
#include <webp/decode.h>
#include <webp/encode.h>
#include <scope_guard.hpp>

#include "imageio/image_dec.h"
#include "imageio/image_enc.h"
#include "imageio/yuvdec.h"
#include "klib/exception.h"
#include "mozjpeg/cdjpeg.h"

extern "C" void png_write_callback(png_structp png_ptr, png_bytep data,
                                   png_size_t length) {
  auto p = static_cast<std::string*>(png_get_io_ptr(png_ptr));
  p->append(reinterpret_cast<const char*>(data), length);
}

namespace klib {

namespace {

void check_quality(std::int32_t quality) {
  if (quality < 0 || quality > 100) {
    throw InvalidArgument(
        "The quality must be greater than or equal to 0 and less than or equal "
        "to 100");
  }
}

bool is_png(std::uint8_t c) { return c == 0x89; }

bool is_jpeg(std::uint8_t c) { return c == 0xff; }

cjpeg_source_ptr select_file_type(j_compress_ptr cinfo,
                                  const unsigned char* data) {
  const auto c = data[0];
  if (is_png(c)) {
    return jinit_read_png(cinfo);
  } else if (is_jpeg(c)) {
    return jinit_read_jpeg(cinfo);
  } else {
    throw RuntimeError("Unknown format");
  }
}

}  // namespace

std::string image_to_jpeg(const std::string& image, std::int32_t quality,
                          bool max_compress) {
  return image_to_jpeg(std::data(image), std::size(image), quality,
                       max_compress);
}

std::string image_to_jpeg(std::string_view image, std::int32_t quality,
                          bool max_compress) {
  return image_to_jpeg(std::data(image), std::size(image), quality,
                       max_compress);
}

std::string image_to_jpeg(const char* image, std::size_t size,
                          std::int32_t quality, bool max_compress) {
  if (size == 0) [[unlikely]] {
    throw RuntimeError("The image is empty");
  }
  check_quality(quality);

  jpeg_compress_struct cinfo;
  jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  cinfo.err->trace_level = 0;

  jpeg_create_compress(&cinfo);

  cinfo.in_color_space = JCS_RGB;

  if (max_compress) {
    jpeg_c_set_int_param(&cinfo, JINT_COMPRESS_PROFILE, JCP_MAX_COMPRESSION);
  } else {
    jpeg_c_set_int_param(&cinfo, JINT_COMPRESS_PROFILE, JCP_FASTEST);
  }
  jpeg_set_defaults(&cinfo);

  jpeg_set_quality(&cinfo, quality, true);

  auto src_mgr =
      select_file_type(&cinfo, reinterpret_cast<const unsigned char*>(image));
  src_mgr->input = reinterpret_cast<const unsigned char*>(image);
  src_mgr->input_size = size;

  (*src_mgr->start_input)(&cinfo, src_mgr);

  jpeg_default_colorspace(&cinfo);

  unsigned char* buffer = nullptr;
  SCOPE_EXIT { std::free(buffer); };
  std::size_t buffer_size = 0;
  jpeg_mem_dest(&cinfo, &buffer, &buffer_size);

  jpeg_start_compress(&cinfo, true);

  while (cinfo.next_scanline < cinfo.image_height) {
    auto num_scanlines = (*src_mgr->get_pixel_rows)(&cinfo, src_mgr);
    jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
  }

  (*src_mgr->finish_input)(&cinfo, src_mgr);
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  return {reinterpret_cast<const char*>(buffer), buffer_size};
}

std::string image_to_webp(const std::string& image, std::int32_t quality,
                          bool lossless) {
  return image_to_webp(std::data(image), std::size(image), quality, lossless);
}

std::string image_to_webp(std::string_view image, std::int32_t quality,
                          bool lossless) {
  return image_to_webp(std::data(image), std::size(image), quality, lossless);
}

std::string image_to_webp(const char* image, std::size_t size,
                          std::int32_t quality, bool lossless) {
  if (size == 0) [[unlikely]] {
    throw RuntimeError("The image is empty");
  }
  const auto c = image[0];
  if (!is_png(c) && !is_jpeg(c)) [[unlikely]] {
    throw RuntimeError("Unknown format");
  }
  check_quality(quality);

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

  config.quality = quality;
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
