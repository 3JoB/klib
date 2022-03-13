// Copyright 2014 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
// WebP decode.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "./webpdec.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "./imageio_util.h"
#include "./metadata.h"
#include "unicode.h"
#include "webp/decode.h"
#include "webp/demux.h"
#include "webp/encode.h"

//------------------------------------------------------------------------------
// WebP decoding

#ifndef NDEBUG
static const char* const kStatusMessages[VP8_STATUS_NOT_ENOUGH_DATA + 1] = {
    "OK",
    "OUT_OF_MEMORY",
    "INVALID_PARAM",
    "BITSTREAM_ERROR",
    "UNSUPPORTED_FEATURE",
    "SUSPENDED",
    "USER_ABORT",
    "NOT_ENOUGH_DATA"};
#endif

static void PrintAnimationWarning(const WebPDecoderConfig* const config) {
  if (config->input.has_animation) {
#ifndef NDEBUG
    fprintf(stderr,
            "Error! Decoding of an animated WebP file is not supported.\n"
            "       Use webpmux to extract the individual frames or\n"
            "       vwebp to view this image.\n");
#endif
  }
}

const char* GetWebPError(int status) {
  (void)status;
#ifndef NDEBUG
  if (status >= VP8_STATUS_OK && status <= VP8_STATUS_NOT_ENOUGH_DATA) {
    return kStatusMessages[status];
  }
#endif
  return NULL;
}

void PrintWebPError(const char* const in_file, int status) {
  (void)in_file;
  (void)status;
#ifndef NDEBUG
  WFPRINTF(stderr, "Decoding of %s failed.\n", (const W_CHAR*)in_file);
  fprintf(stderr, "Status: %d", status);
  if (status >= VP8_STATUS_OK && status <= VP8_STATUS_NOT_ENOUGH_DATA) {
    fprintf(stderr, "(%s)", kStatusMessages[status]);
  }
  fprintf(stderr, "\n");
#endif
}

int LoadWebP(const char* const in_file, const uint8_t** data, size_t* data_size,
             WebPBitstreamFeatures* bitstream) {
  VP8StatusCode status;
  WebPBitstreamFeatures local_features;
  if (!ImgIoUtilReadFile(in_file, data, data_size)) return 0;

  if (bitstream == NULL) {
    bitstream = &local_features;
  }

  status = WebPGetFeatures(*data, *data_size, bitstream);
  if (status != VP8_STATUS_OK) {
    WebPFree((void*)*data);
    *data = NULL;
    *data_size = 0;
    PrintWebPError(in_file, status);
    return 0;
  }
  return 1;
}

//------------------------------------------------------------------------------

VP8StatusCode DecodeWebP(const uint8_t* const data, size_t data_size,
                         WebPDecoderConfig* const config) {
  if (config == NULL) return VP8_STATUS_INVALID_PARAM;
  PrintAnimationWarning(config);
  return WebPDecode(data, data_size, config);
}

VP8StatusCode DecodeWebPIncremental(const uint8_t* const data, size_t data_size,
                                    WebPDecoderConfig* const config) {
  VP8StatusCode status = VP8_STATUS_OK;
  if (config == NULL) return VP8_STATUS_INVALID_PARAM;

  PrintAnimationWarning(config);

  // Decoding call.
  {
    WebPIDecoder* const idec = WebPIDecode(data, data_size, config);
    if (idec == NULL) {
#ifndef NDEBUG
      fprintf(stderr, "Failed during WebPIDecode().\n");
#endif
      return VP8_STATUS_OUT_OF_MEMORY;
    } else {
      status = WebPIUpdate(idec, data, data_size);
      WebPIDelete(idec);
    }
  }
  return status;
}
