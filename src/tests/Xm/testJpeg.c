/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <JpegI.h>
#include <check.h>

#include "suites.h"

START_TEST(load_invalid_header)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("jpeg/invalid_header.jpeg", "rb"), "Failed to open jpeg/invalid_header.jpeg");
	ret = _XmJpegGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(ret == 1 && !img, "Failed to recognize invalid header");
	if (img) XDestroyImage(img);
}
END_TEST

START_TEST(load_rgb24)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("jpeg/rgb24.jpeg", "rb"), "Failed to open jpeg/rgb24.jpeg");
	ret = _XmJpegGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 24-bit RGB test image");
	ck_assert_msg(img->depth == 24, "Expected 24-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg(img->red_mask   = (0xff << 16), "Expected red_mask @ <<16");
	ck_assert_msg(img->green_mask = (0xff << 8),  "Expected green_mask @ <<8");
	ck_assert_msg(img->blue_mask  = 0xff,         "Expected blue_mask @ 0");
	ck_assert_msg((unsigned char)img->data[0]  >= 0xfa, "Expected pixel 1 to be red (r)");
	ck_assert_msg((unsigned char)img->data[1]  == 0x00, "Expected pixel 1 to be red (g)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x00, "Expected pixel 1 to be red (b)");
	ck_assert_msg((unsigned char)img->data[3]  == 0x00, "Expected pixel 2 to be green (r)");
	ck_assert_msg((unsigned char)img->data[4]  >= 0xfa, "Expected pixel 2 to be green (g)");
	ck_assert_msg((unsigned char)img->data[5]  == 0x00, "Expected pixel 2 to be green (b)");
	ck_assert_msg((unsigned char)img->data[6]  == 0x00, "Expected pixel 3 to be blue (r)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x00, "Expected pixel 3 to be blue (g)");
	ck_assert_msg((unsigned char)img->data[8]  >= 0xfa, "Expected pixel 3 to be blue (b)");
	ck_assert_msg((unsigned char)img->data[9]  <  0x05, "Expected pixel 4 to be black (r)");
	ck_assert_msg((unsigned char)img->data[10] <  0x05, "Expected pixel 4 to be black (g)");
	ck_assert_msg((unsigned char)img->data[11] <  0x05, "Expected pixel 4 to be black (b)");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_grayscale)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("jpeg/grayscale.jpeg", "rb"), "Failed to open jpeg/grayscale.jpeg");
	ret = _XmJpegGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load grayscale test image");
	ck_assert_msg(img->depth == 24, "Expected 24-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg(img->red_mask   = (0xff << 16), "Expected red_mask @ <<16");
	ck_assert_msg(img->green_mask = (0xff << 8),  "Expected green_mask @ <<8");
	ck_assert_msg(img->blue_mask  = 0xff,         "Expected blue_mask @ 0");
	ck_assert_msg((unsigned char)img->data[0]  == 0x83, "Expected pixel 1 to be gray (r)");
	ck_assert_msg((unsigned char)img->data[1]  == 0x83, "Expected pixel 1 to be gray (g)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x83, "Expected pixel 1 to be gray (b)");
	ck_assert_msg((unsigned char)img->data[3]  == 0xe0, "Expected pixel 2 to be off-white (r)");
	ck_assert_msg((unsigned char)img->data[4]  == 0xe0, "Expected pixel 2 to be off-white (g)");
	ck_assert_msg((unsigned char)img->data[5]  == 0xe0, "Expected pixel 2 to be off-white (b)");
	ck_assert_msg((unsigned char)img->data[6]  == 0x44, "Expected pixel 3 to be dark-gray (r)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x44, "Expected pixel 3 to be dark-gray (g)");
	ck_assert_msg((unsigned char)img->data[8]  == 0x44, "Expected pixel 3 to be dark-gray (b)");
	ck_assert_msg((unsigned char)img->data[9]  == 0x00, "Expected pixel 4 to be black (r)");
	ck_assert_msg((unsigned char)img->data[10] == 0x00, "Expected pixel 4 to be black (g)");
	ck_assert_msg((unsigned char)img->data[11] == 0x00, "Expected pixel 4 to be black (b)");
	XDestroyImage(img);
}
END_TEST

/* Check a pixel of a 24-bit image against an expected color, +/- tol */
static int pixel_near(XImage *img, int x, int y, unsigned int rgb, int tol)
{
	unsigned char *p = (unsigned char *)img->data + y * img->bytes_per_line + x * 3;
	int i, want, got;

	for (i = 0; i < 3; i++) {
		want = (rgb >> (16 - 8 * i)) & 0xff;
		got  = p[i];
		if (got < want - tol || got > want + tol)
			return 0;
	}
	return 1;
}

START_TEST(load_rgb24_multirow)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("jpeg/rgb24_4x2.jpeg", "rb"), "Failed to open jpeg/rgb24_4x2.jpeg");
	ret = _XmJpegGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 4x2 RGB test image");
	ck_assert_msg(img->width == 4 && img->height == 2, "Expected 4x2 image");
	ck_assert_msg(img->bytes_per_line == 12, "Expected 12 bytes per line");
	ck_assert_msg(pixel_near(img, 0, 0, 0xff0000, 8), "Expected (0,0) to be red");
	ck_assert_msg(pixel_near(img, 1, 0, 0x00ff00, 8), "Expected (1,0) to be green");
	ck_assert_msg(pixel_near(img, 2, 0, 0x0000ff, 8), "Expected (2,0) to be blue");
	ck_assert_msg(pixel_near(img, 3, 0, 0x000000, 8), "Expected (3,0) to be black");
	ck_assert_msg(pixel_near(img, 0, 1, 0xffffff, 8), "Expected (0,1) to be white");
	ck_assert_msg(pixel_near(img, 1, 1, 0x808080, 8), "Expected (1,1) to be gray");
	ck_assert_msg(pixel_near(img, 2, 1, 0xffff00, 8), "Expected (2,1) to be yellow");
	ck_assert_msg(pixel_near(img, 3, 1, 0x00ffff, 8), "Expected (3,1) to be cyan");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_grayscale_multirow)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("jpeg/grayscale_4x2.jpeg", "rb"), "Failed to open jpeg/grayscale_4x2.jpeg");
	ret = _XmJpegGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 4x2 grayscale test image");
	ck_assert_msg(img->width == 4 && img->height == 2, "Expected 4x2 image");
	ck_assert_msg(img->bytes_per_line == 12, "Expected 12 bytes per line");
	ck_assert_msg(pixel_near(img, 0, 0, 0x000000, 2), "Expected (0,0) to be 0x00");
	ck_assert_msg(pixel_near(img, 1, 0, 0x404040, 2), "Expected (1,0) to be 0x40");
	ck_assert_msg(pixel_near(img, 2, 0, 0x808080, 2), "Expected (2,0) to be 0x80");
	ck_assert_msg(pixel_near(img, 3, 0, 0xc0c0c0, 2), "Expected (3,0) to be 0xc0");
	ck_assert_msg(pixel_near(img, 0, 1, 0xffffff, 2), "Expected (0,1) to be 0xff");
	ck_assert_msg(pixel_near(img, 1, 1, 0xe0e0e0, 2), "Expected (1,1) to be 0xe0");
	ck_assert_msg(pixel_near(img, 2, 1, 0x202020, 2), "Expected (2,1) to be 0x20");
	ck_assert_msg(pixel_near(img, 3, 1, 0x101010, 2), "Expected (3,1) to be 0x10");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_rgb24_odd_width)
{
	FILE *fp;
	XImage *img = NULL;
	unsigned char *p;
	int ret;

	/**
	 * 3 * 3 bytes per row isn't a multiple of 4, which the XImage must
	 * accept: XInitImage() has to succeed, or the function pointers that
	 * XGetPixel() / XDestroyImage() call are left uninitialized.
	 */
	ck_assert_msg(fp = fopen("jpeg/rgb24_3x2.jpeg", "rb"), "Failed to open jpeg/rgb24_3x2.jpeg");
	ret = _XmJpegGetImage(fp, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 3x2 RGB test image");
	ck_assert_msg(img->width == 3 && img->height == 2, "Expected 3x2 image");
	ck_assert_msg(img->bytes_per_line == 9, "Expected 9 bytes per line");
	ck_assert_msg(img->f.get_pixel && img->f.destroy_image, "Expected an initialized XImage");
	ck_assert_msg(pixel_near(img, 2, 0, 0x0000ff, 8), "Expected (2,0) to be blue");
	ck_assert_msg(pixel_near(img, 0, 1, 0xffffff, 8), "Expected (0,1) to be white");
	p = (unsigned char *)img->data + 2 * 3;
	ck_assert_msg(XGetPixel(img, 2, 0) == ((unsigned long)p[0] << 16 | p[1] << 8 | p[2]),
	              "Expected XGetPixel to read the packed pixel");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_cmyk)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	/**
	 * We only accept RGB or grayscale output from libjpeg. CMYK must
	 * either be converted to 3 bytes per pixel, or fail cleanly.
	 */
	ck_assert_msg(fp = fopen("jpeg/cmyk_4x1.jpeg", "rb"), "Failed to open jpeg/cmyk_4x1.jpeg");
	ret = _XmJpegGetImage(fp, &img);
	fclose(fp);

	if (!ret) {
		ck_assert_msg(img && img->width == 4 && img->height == 1, "Expected 4x1 image");
		ck_assert_msg(img->bytes_per_line == 12, "Expected 12 bytes per line");
		XDestroyImage(img);
	} else ck_assert_msg(!img, "Expected no image on failure");
}
END_TEST

void jpeg_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("Jpeg");

	t = tcase_create("Load JPEG images");
	tcase_add_test(t, load_rgb24);
	tcase_add_test(t, load_grayscale);
	tcase_add_test(t, load_rgb24_multirow);
	tcase_add_test(t, load_grayscale_multirow);
	tcase_add_test(t, load_rgb24_odd_width);
	tcase_add_test(t, load_cmyk);
	tcase_add_test(t, load_invalid_header);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

