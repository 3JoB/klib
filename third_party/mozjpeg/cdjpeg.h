/*
 * cdjpeg.h
 *
 * This file was part of the Independent JPEG Group's software:
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * libjpeg-turbo Modifications:
 * Copyright (C) 2017, D. R. Commander.
 * mozjpeg Modifications:
 * Copyright (C) 2014, Mozilla Corporation.
 * For conditions of distribution and use, see the accompanying README.ijg file.
 *
 * This file contains common declarations for the sample applications
 * cjpeg and djpeg.  It is NOT used by the core JPEG library.
 */

#define JPEG_CJPEG_DJPEG      /* define proper options in jconfig.h */
#define JPEG_INTERNAL_OPTIONS /* cjpeg.c,djpeg.c need to see xxx_SUPPORTED */
#include "cderror.h"          /* get application-specific error codes */
#include "jerror.h"           /* get library error codes too */
#include "jinclude.h"
#include "jpeglib.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define JPEG_RAW_READER 0

/*
 * Object interface for cjpeg's source file decoding modules
 */

typedef struct cjpeg_source_struct *cjpeg_source_ptr;

struct cjpeg_source_struct {
  void (*start_input)(j_compress_ptr cinfo, cjpeg_source_ptr sinfo);
  JDIMENSION (*get_pixel_rows)(j_compress_ptr cinfo, cjpeg_source_ptr sinfo);
  void (*finish_input)(j_compress_ptr cinfo, cjpeg_source_ptr sinfo);

  const unsigned char *input;
  size_t input_size;

  JSAMPARRAY buffer;
  JDIMENSION buffer_height;

#if JPEG_RAW_READER
  // For reading JPEG
  JSAMPARRAY plane_pointer[4];
#endif

  jpeg_saved_marker_ptr marker_list;
};

/*
 * Object interface for djpeg's output file encoding modules
 */

typedef struct djpeg_dest_struct *djpeg_dest_ptr;

struct djpeg_dest_struct {
  /* start_output is called after jpeg_start_decompress finishes.
   * The color map will be ready at this time, if one is needed.
   */
  void (*start_output)(j_decompress_ptr cinfo, djpeg_dest_ptr dinfo);
  /* Emit the specified number of pixel rows from the buffer. */
  void (*put_pixel_rows)(j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
                         JDIMENSION rows_supplied);
  /* Finish up at the end of the image. */
  void (*finish_output)(j_decompress_ptr cinfo, djpeg_dest_ptr dinfo);
  /* Re-calculate buffer dimensions based on output dimensions (for use with
     partial image decompression.)  If this is NULL, then the output format
     does not support partial image decompression (BMP and RLE, in particular,
     cannot support partial decompression because they use an inversion buffer
     to write the image in bottom-up order.) */
  void (*calc_buffer_dimensions)(j_decompress_ptr cinfo, djpeg_dest_ptr dinfo);

  /* Target file spec; filled in by djpeg.c after object is created. */
  FILE *output_file;

  /* Output pixel-row buffer.  Created by module init or start_output.
   * Width is cinfo->output_width * cinfo->output_components;
   * height is buffer_height.
   */
  JSAMPARRAY buffer;
  JDIMENSION buffer_height;
};

/*
 * cjpeg/djpeg may need to perform extra passes to convert to or from
 * the source/destination file format.  The JPEG library does not know
 * about these passes, but we'd like them to be counted by the progress
 * monitor.  We use an expanded progress monitor object to hold the
 * additional pass count.
 */

struct cdjpeg_progress_mgr {
  struct jpeg_progress_mgr pub; /* fields known to JPEG library */
  int completed_extra_passes;   /* extra passes completed */
  int total_extra_passes;       /* total extra */
  /* last printed percentage stored here to avoid multiple printouts */
  int percent_done;
};

typedef struct cdjpeg_progress_mgr *cd_progress_ptr;

/* Module selection routines for I/O modules. */

EXTERN(cjpeg_source_ptr) jinit_read_jpeg(j_compress_ptr cinfo);
EXTERN(cjpeg_source_ptr) jinit_read_png(j_compress_ptr cinfo);

/* miscellaneous useful macros */

#ifdef DONT_USE_B_MODE /* define mode parameters for fopen() */
#define READ_BINARY "r"
#define WRITE_BINARY "w"
#else
#define READ_BINARY "rb"
#define WRITE_BINARY "wb"
#endif

#ifndef EXIT_FAILURE /* define exit() codes if not provided */
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_WARNING
#define EXIT_WARNING 2
#endif

#define IsExtRGB(cs) \
  (cs == JCS_RGB || (cs >= JCS_EXT_RGB && cs <= JCS_EXT_ARGB))

#ifdef __cplusplus
}
#endif /* __cplusplus */
