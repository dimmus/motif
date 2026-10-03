/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#include "PngI.h"
#include <X11/Xlibint.h>
#include <limits.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Load a PNG image from a file, returning 0 on success and yielding a
 * 32-bit ARGB ZPixmap. If the background color is specified, it is
 * blended in automatically, and the resulting image is fully opaque.
 */
int _XmPngGetImage(FILE *fp, XColor *bg, XImage **ximage)
{
  XImage *img = NULL;
  png_color_16 bgcolor;
  png_uint_32 w, h, i;
  png_structp pngp = NULL;
  png_infop infop = NULL;
  unsigned char header[8];
  /* Modified after setjmp() and used after longjmp(), so must be volatile */
  unsigned char *volatile data = NULL;
  png_bytep *volatile rows = NULL;
  if (!ximage || !fp || fread(header, 1, sizeof header, fp) != sizeof header ||
      png_sig_cmp(header, 0, sizeof header))
    return 1;
  *ximage = NULL;
  if (!(pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) ||
      !(infop = png_create_info_struct(pngp)))
  {
    if (pngp)
      png_destroy_read_struct(&pngp, NULL, NULL);
    return 2;
  }
  if (setjmp(png_jmpbuf(pngp))) {
    if (data)
      XFree(data);
    free(rows);
    png_destroy_read_struct(&pngp, &infop, NULL);
    return 3;
  }
  png_init_io(pngp, fp);
  png_set_sig_bytes(pngp, sizeof header);
  png_set_palette_to_rgb(pngp);
  png_set_filler(pngp, 0xff, PNG_FILLER_BEFORE);
  /* If we've specified a background color, use that in lieu of alpha */
  if (bg) {
    bgcolor.red = bg->red * 257;
    bgcolor.green = bg->green * 257;
    bgcolor.blue = bg->blue * 257;
    png_set_background(pngp, &bgcolor, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1);
  }
  else
    png_set_alpha_mode(pngp, PNG_ALPHA_STANDARD, PNG_DEFAULT_sRGB);
  png_read_info(pngp, infop);
  /* Same transformations as png_read_png() would apply */
  png_set_scale_16(pngp);
  png_set_expand(pngp);
  png_set_swap_alpha(pngp);
  png_set_gray_to_rgb(pngp);
  png_set_interlace_handling(pngp);
  png_read_update_info(pngp, infop);
  w = png_get_image_width(pngp, infop);
  h = png_get_image_height(pngp, infop);
  /**
   * The transformations above must have yielded 8-bit ARGB pixels, and
   * the image must stay within INT_MAX bytes, since Xlib indexes XImage
   * data with int arithmetic (y * bytes_per_line in XGetPixel.)
   */
  if (!w || !h || w > INT_MAX / 4 || h > INT_MAX / 4 / w ||
      png_get_rowbytes(pngp, infop) != (size_t)w * 4)
  {
    png_destroy_read_struct(&pngp, &infop, NULL);
    return 4;
  }
  /**
   * Allocate the row pointers, so that the [A]RGB pixels are nice and
   * contiguous in our buffer.
   */
  if (!(data = Xmalloc((size_t)w * h * 4))) {
    png_destroy_read_struct(&pngp, &infop, NULL);
    return 5;
  }
  if (!(rows = malloc(h * sizeof(*rows)))) {
    XFree(data);
    png_destroy_read_struct(&pngp, &infop, NULL);
    return 6;
  }
  for (i = 0; i < h; i++)
    rows[i] = data + (size_t)i * w * 4;
  png_read_image(pngp, rows);
  png_read_end(pngp, NULL);
  png_destroy_read_struct(&pngp, &infop, NULL);
  free(rows);
  /* Create our XImage */
  if (!(img = Xmalloc(sizeof *img))) {
    XFree(data);
    return 7;
  }
  img->data = (char *)data;
  img->obdata = NULL;
  img->width = w;
  img->height = h;
  img->xoffset = 0;
  img->depth = 32;
  img->format = ZPixmap;
  img->byte_order = MSBFirst;
  img->red_mask = 0x00ff0000;
  img->green_mask = 0x0000ff00;
  img->blue_mask = 0x000000ff;
  img->bitmap_unit = 8;
  img->bitmap_bit_order = MSBFirst;
  img->bitmap_pad = 32;
  img->bits_per_pixel = 32;
  img->bytes_per_line = w * 4;
  /* On failure the XImage functions (XGetPixel, XDestroyImage) are unset */
  if (!XInitImage(img)) {
    XFree(data);
    XFree(img);
    return 7;
  }
  *ximage = img;
  return 0;
}
