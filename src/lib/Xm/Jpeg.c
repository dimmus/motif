/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#include "JpegI.h"
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <limits.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* jpeglib.h must come first: jerror.h depends on JPEG_LIB_VERSION */
#include <jpeglib.h>
#include <jerror.h>

/**
 * Error handling context info
 */
struct jerr {
  struct jpeg_error_mgr jpeg_error;
  jmp_buf jmp;
};

/**
 * Error handling routine
 */
static void on_jpeg_error(j_common_ptr errinfo)
{
  int ret;
  struct jerr *err = (struct jerr *)errinfo->err;
  switch (errinfo->err->msg_code) {
    case JERR_NO_SOI:
      ret = 1;
      break; /* Invalid JPEG: No start of image */
    case JERR_OUT_OF_MEMORY:
      ret = 5;
      break;
    default:
      ret = -1;
  }
  longjmp(err->jmp, ret);
}

int _XmJpegGetImage(FILE *fp, XImage **ximage)
{
  int ret = 0;
  XImage *img = NULL;
  size_t n, stride;
  unsigned int i, w, h, ncomp;
  unsigned char *src, *dst;
  /* Modified after setjmp() and used after longjmp(), so must be volatile */
  unsigned char *volatile data = NULL;
  JSAMPARRAY volatile rows = NULL;
  struct jerr err;
  struct jpeg_decompress_struct jpeg;
  if (!fp || !ximage)
    return 1;
  /* Setup error handling */
  *ximage = NULL;
  memset(&err, 0, sizeof err);
  memset(&jpeg, 0, sizeof jpeg);
  jpeg.err = jpeg_std_error(&err.jpeg_error);
  err.jpeg_error.error_exit = on_jpeg_error;
  if ((ret = setjmp(err.jmp))) {
    if (data)
      XFree(data);
    if (rows)
      XFree(rows);
    jpeg_destroy_decompress(&jpeg);
    return ret;
  }
  /* Initialize the JPEG struct */
  jpeg_create_decompress(&jpeg);
  jpeg_stdio_src(&jpeg, fp);
  jpeg_read_header(&jpeg, True);
  /**
   * We only know how to deal with RGB and grayscale output, so ask for
   * that explicitly instead of trusting the colorspace of the file
   * (e.g. CMYK would yield 4 components per pixel.)
   */
  jpeg.out_color_space = (jpeg.jpeg_color_space == JCS_GRAYSCALE) ? JCS_GRAYSCALE : JCS_RGB;
  jpeg_start_decompress(&jpeg);
  w = jpeg.output_width;
  h = jpeg.output_height;
  ncomp = jpeg.output_components;
  if (!w || !h || ncomp != (jpeg.out_color_space == JCS_GRAYSCALE ? 1U : 3U) ||
      w > INT_MAX / 3 || h > INT_MAX || h > SIZE_MAX / 3 / w)
  {
    jpeg_destroy_decompress(&jpeg);
    return 2;
  }
  /* Allocate our data buffer (always 3 bytes per pixel) */
  n = (size_t)w * h;
  if (!(data = Xmalloc(n * 3))) {
    jpeg_destroy_decompress(&jpeg);
    return 2;
  }
  /* Setup our row pointers */
  if (!(rows = Xmalloc(h * sizeof(*rows)))) {
    XFree(data);
    jpeg_destroy_decompress(&jpeg);
    return 3;
  }
  stride = (size_t)w * ncomp;
  for (i = 0; i < h; i++)
    rows[i] = data + i * stride;
  /* Read scanlines */
  while (jpeg.output_scanline < h)
    jpeg_read_scanlines(&jpeg, rows + jpeg.output_scanline, h - jpeg.output_scanline);
  /**
   * Do grayscale expansion if needed: the gray pixels are packed at
   * the start of the buffer, so expand them in place from the end.
   */
  if (ncomp == 1) {
    src = data + n;
    dst = data + 3 * n;
    while (src > data) {
      --src;
      *(--dst) = *src;
      *(--dst) = *src;
      *(--dst) = *src;
    }
  }
  jpeg_finish_decompress(&jpeg);
  jpeg_destroy_decompress(&jpeg);
  XFree(rows);
  /* Create our XImage */
  if (!(img = Xmalloc(sizeof *img))) {
    XFree(data);
    return 4;
  }
  img->data = (char *)data;
  img->obdata = NULL;
  img->width = w;
  img->height = h;
  img->xoffset = 0;
  img->depth = 24;
  img->format = ZPixmap;
  img->byte_order = MSBFirst;
  img->red_mask = 0xff0000;
  img->green_mask = 0x00ff00;
  img->blue_mask = 0x0000ff;
  img->bitmap_unit = 8;
  img->bitmap_bit_order = MSBFirst;
  /* The rows are packed (3 * w bytes), so they're only 8-bit aligned */
  img->bitmap_pad = 8;
  img->bits_per_pixel = 24;
  img->bytes_per_line = w * 3;
  /* On failure the XImage functions (XGetPixel, XDestroyImage) are unset */
  if (!XInitImage(img)) {
    XFree(data);
    XFree(img);
    return 4;
  }
  *ximage = img;
  return 0;
}
