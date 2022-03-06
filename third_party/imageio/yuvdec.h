#pragma once

#include <stddef.h>
#include <stdint.h>

#include <webp/encode.h>

#ifdef __cplusplus
extern "C" {
#endif

int ReadYUV(const uint8_t* const data, size_t data_size,
            WebPPicture* const pic);

#ifdef __cplusplus
}  // extern "C"
#endif
