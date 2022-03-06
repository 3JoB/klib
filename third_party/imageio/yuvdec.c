#include "yuvdec.h"

#include <stdio.h>

#include "imageio_util.h"

int ReadYUV(const uint8_t* const data, size_t data_size,
            WebPPicture* const pic) {
  const int use_argb = pic->use_argb;
  const int uv_width = (pic->width + 1) / 2;
  const int uv_height = (pic->height + 1) / 2;
  const int y_plane_size = pic->width * pic->height;
  const int uv_plane_size = uv_width * uv_height;
  const size_t expected_data_size = y_plane_size + 2 * uv_plane_size;

  if (data_size != expected_data_size) {
#ifndef NDEBUG
    fprintf(stderr,
            "input data doesn't have the expected size (%d instead of %d)\n",
            (int)data_size, (int)expected_data_size);
#endif
    return 0;
  }

  pic->use_argb = 0;
  if (!WebPPictureAlloc(pic)) return 0;
  ImgIoUtilCopyPlane(data, pic->width, pic->y, pic->y_stride, pic->width,
                     pic->height);
  ImgIoUtilCopyPlane(data + y_plane_size, uv_width, pic->u, pic->uv_stride,
                     uv_width, uv_height);
  ImgIoUtilCopyPlane(data + y_plane_size + uv_plane_size, uv_width, pic->v,
                     pic->uv_stride, uv_width, uv_height);
  return use_argb ? WebPPictureYUVAToARGB(pic) : 1;
}
