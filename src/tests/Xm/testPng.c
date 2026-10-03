/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <PngI.h>
#include <check.h>

#include "suites.h"

START_TEST(load_invalid_header)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/invalid_header.png", "rb"), "Failed to open png/invalid_header.png");
	ret = _XmPngGetImage(fp, NULL, &img);
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

	ck_assert_msg(fp = fopen("png/rgb24.png", "rb"), "Failed to open png/rgb24.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 24-bit RGB test image");
	ck_assert_msg(img->depth == 32, "Expected 32-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg(img->red_mask   = (0xff << 16), "Expected red_mask @ <<16");
	ck_assert_msg(img->green_mask = (0xff << 8),  "Expected green_mask @ <<8");
	ck_assert_msg(img->blue_mask  = 0xff,         "Expected blue_mask @ 0");
	ck_assert_msg((unsigned char)img->data[0]  == 0xff, "Expected pixel 1 to be red (a)");
	ck_assert_msg((unsigned char)img->data[1]  == 0xff, "Expected pixel 1 to be red (r)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x00, "Expected pixel 1 to be red (g)");
	ck_assert_msg((unsigned char)img->data[3]  == 0x00, "Expected pixel 1 to be red (b)");
	ck_assert_msg((unsigned char)img->data[4]  == 0xff, "Expected pixel 2 to be green (a)");
	ck_assert_msg((unsigned char)img->data[5]  == 0x00, "Expected pixel 2 to be green (r)");
	ck_assert_msg((unsigned char)img->data[6]  == 0xff, "Expected pixel 2 to be green (g)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x00, "Expected pixel 2 to be green (b)");
	ck_assert_msg((unsigned char)img->data[8]  == 0xff, "Expected pixel 3 to be blue (a)");
	ck_assert_msg((unsigned char)img->data[9]  == 0x00, "Expected pixel 3 to be blue (r)");
	ck_assert_msg((unsigned char)img->data[10] == 0x00, "Expected pixel 3 to be blue (g)");
	ck_assert_msg((unsigned char)img->data[11] == 0xff, "Expected pixel 3 to be blue (b)");
	ck_assert_msg((unsigned char)img->data[12] == 0xff, "Expected pixel 4 to be black (a)");
	ck_assert_msg((unsigned char)img->data[13] == 0x00, "Expected pixel 4 to be black (r)");
	ck_assert_msg((unsigned char)img->data[14] == 0x00, "Expected pixel 4 to be black (g)");
	ck_assert_msg((unsigned char)img->data[15] == 0x00, "Expected pixel 4 to be black (b)");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_rgba32)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/rgba32.png", "rb"), "Failed to open png/rgba32.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 32-bit RGBA test image");
	ck_assert_msg(img->depth == 32, "Expected 32-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg(img->red_mask   = (0xff << 16), "Expected red_mask @ <<16");
	ck_assert_msg(img->green_mask = (0xff << 8),  "Expected green_mask @ <<8");
	ck_assert_msg(img->blue_mask  = 0xff,         "Expected blue_mask @ 0");
	ck_assert_msg((unsigned char)img->data[0]  == 0xff, "Expected pixel 1 to be opaque red (a)");
	ck_assert_msg((unsigned char)img->data[1]  == 0xff, "Expected pixel 1 to be opaque red (r)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x00, "Expected pixel 1 to be opaque red (g)");
	ck_assert_msg((unsigned char)img->data[3]  == 0x00, "Expected pixel 1 to be opaque red (b)");
	ck_assert_msg((unsigned char)img->data[4]  == 0xff, "Expected pixel 2 to be opaque green (a)");
	ck_assert_msg((unsigned char)img->data[5]  == 0x00, "Expected pixel 2 to be opaque green (r)");
	ck_assert_msg((unsigned char)img->data[6]  == 0xff, "Expected pixel 2 to be opaque green (g)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x00, "Expected pixel 2 to be opaque green (b)");
	ck_assert_msg((unsigned char)img->data[8]  == 0xff, "Expected pixel 3 to be opaque blue (a)");
	ck_assert_msg((unsigned char)img->data[9]  == 0x00, "Expected pixel 3 to be opaque blue (r)");
	ck_assert_msg((unsigned char)img->data[10] == 0x00, "Expected pixel 3 to be opaque blue (g)");
	ck_assert_msg((unsigned char)img->data[11] == 0xff, "Expected pixel 3 to be opaque blue (b)");
	ck_assert_msg((unsigned char)img->data[12] == 0x00, "Expected pixel 4 to be transparent (a)");
	ck_assert_msg((unsigned char)img->data[13] == 0x00, "Expected pixel 4 to be transparent (r)");
	ck_assert_msg((unsigned char)img->data[14] == 0x00, "Expected pixel 4 to be transparent (g)");
	ck_assert_msg((unsigned char)img->data[15] == 0x00, "Expected pixel 4 to be transparent (b)");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_rgba32_bgcolor)
{
	FILE *fp;
	XImage *img = NULL;
	XColor bg;
	int ret;

	bg.red   = 0xcc;
	bg.green = 0xcc;
	bg.blue  = 0xcc;

	ck_assert_msg(fp = fopen("png/rgba32.png", "rb"), "Failed to open png/rgba32.png");
	ret = _XmPngGetImage(fp, &bg, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 32-bit RGBA test image");
	ck_assert_msg(img->depth == 32, "Expected 32-bit depth");
	ck_assert_msg(img->width == 4 && img->height == 1, "Expected 4x1 image");
	ck_assert_msg((unsigned char)img->data[0]  == 0xff, "Expected pixel 1 to be red (a)");
	ck_assert_msg((unsigned char)img->data[1]  == 0xff, "Expected pixel 1 to be red (r)");
	ck_assert_msg((unsigned char)img->data[2]  == 0x00, "Expected pixel 1 to be red (g)");
	ck_assert_msg((unsigned char)img->data[3]  == 0x00, "Expected pixel 1 to be red (b)");
	ck_assert_msg((unsigned char)img->data[4]  == 0xff, "Expected pixel 2 to be green (a)");
	ck_assert_msg((unsigned char)img->data[5]  == 0x00, "Expected pixel 2 to be green (r)");
	ck_assert_msg((unsigned char)img->data[6]  == 0xff, "Expected pixel 2 to be green (g)");
	ck_assert_msg((unsigned char)img->data[7]  == 0x00, "Expected pixel 2 to be green (b)");
	ck_assert_msg((unsigned char)img->data[8]  == 0xff, "Expected pixel 3 to be blue (a)");
	ck_assert_msg((unsigned char)img->data[9]  == 0x00, "Expected pixel 3 to be blue (r)");
	ck_assert_msg((unsigned char)img->data[10] == 0x00, "Expected pixel 3 to be blue (g)");
	ck_assert_msg((unsigned char)img->data[11] == 0xff, "Expected pixel 3 to be blue (b)");
	ck_assert_msg((unsigned char)img->data[12] == 0xff, "Expected pixel 4 to be bg (a)");
	ck_assert_msg((unsigned char)img->data[13] == 0xcc, "Expected pixel 4 to be bg (r)");
	ck_assert_msg((unsigned char)img->data[14] == 0xcc, "Expected pixel 4 to be bg (g)");
	ck_assert_msg((unsigned char)img->data[15] == 0xcc, "Expected pixel 4 to be bg (b)");
	XDestroyImage(img);
}
END_TEST

/* Fetch the 32-bit ARGB pixel at (x, y) */
static unsigned long argb(XImage *img, int x, int y)
{
	unsigned char *p = (unsigned char *)img->data + y * img->bytes_per_line + x * 4;

	return ((unsigned long)p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

START_TEST(load_palette_trns)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/palette_trns_2x2.png", "rb"), "Failed to open png/palette_trns_2x2.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load palette test image");
	ck_assert_msg(img->width == 2 && img->height == 2, "Expected 2x2 image");
	ck_assert_msg(img->bytes_per_line == 8, "Expected 8 bytes per line");
	ck_assert_msg(argb(img, 0, 0) == 0xffff0000, "Expected (0,0) to be opaque red");
	ck_assert_msg(argb(img, 1, 0) == 0xff00ff00, "Expected (1,0) to be opaque green");
	ck_assert_msg(argb(img, 0, 1) == 0xff0000ff, "Expected (0,1) to be opaque blue");
	ck_assert_msg(argb(img, 1, 1) == 0x00000000, "Expected (1,1) to be transparent");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_interlaced)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/interlaced_2x2.png", "rb"), "Failed to open png/interlaced_2x2.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load interlaced test image");
	ck_assert_msg(img->width == 2 && img->height == 2, "Expected 2x2 image");
	ck_assert_msg(argb(img, 0, 0) == 0xffff0000, "Expected (0,0) to be red");
	ck_assert_msg(argb(img, 1, 0) == 0xff00ff00, "Expected (1,0) to be green");
	ck_assert_msg(argb(img, 0, 1) == 0xff0000ff, "Expected (0,1) to be blue");
	ck_assert_msg(argb(img, 1, 1) == 0xffffffff, "Expected (1,1) to be white");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_gray16)
{
	FILE *fp;
	XImage *img = NULL;
	unsigned long p;
	int ret;

	ck_assert_msg(fp = fopen("png/gray16_2x2.png", "rb"), "Failed to open png/gray16_2x2.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(!ret && img, "Failed to load 16-bit grayscale test image");
	ck_assert_msg(img->width == 2 && img->height == 2, "Expected 2x2 image");
	ck_assert_msg(img->bytes_per_line == 8, "Expected 8 bytes per line");
	ck_assert_msg(argb(img, 0, 0) == 0xff000000, "Expected (0,0) to be black");
	ck_assert_msg(argb(img, 1, 0) == 0xffffffff, "Expected (1,0) to be white");
	p = argb(img, 0, 1);
	ck_assert_msg((p >> 24) == 0xff && ((p >> 16) & 0xff) == (p & 0xff) &&
	              ((p >> 8) & 0xff) == (p & 0xff), "Expected (0,1) to be opaque gray");
	p = argb(img, 1, 1);
	ck_assert_msg((p >> 24) == 0xff && ((p >> 16) & 0xff) == (p & 0xff) &&
	              ((p >> 8) & 0xff) == (p & 0xff), "Expected (1,1) to be opaque gray");
	XDestroyImage(img);
}
END_TEST

START_TEST(load_corrupt_idat)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	/* libpng errors out while decoding the rows */
	ck_assert_msg(fp = fopen("png/corrupt_idat.png", "rb"), "Failed to open png/corrupt_idat.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(ret && !img, "Expected a corrupt image data stream to fail");
}
END_TEST

START_TEST(load_truncated)
{
	FILE *fp;
	XImage *img = NULL;
	int ret;

	ck_assert_msg(fp = fopen("png/truncated.png", "rb"), "Failed to open png/truncated.png");
	ret = _XmPngGetImage(fp, NULL, &img);
	fclose(fp);

	ck_assert_msg(ret && !img, "Expected a truncated image to fail");
}
END_TEST

void png_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("Png");

	t = tcase_create("Load PNG images");
	tcase_add_test(t, load_invalid_header);
	tcase_add_test(t, load_rgb24);
	tcase_add_test(t, load_rgba32);
	tcase_add_test(t, load_rgba32_bgcolor);
	tcase_add_test(t, load_palette_trns);
	tcase_add_test(t, load_interlaced);
	tcase_add_test(t, load_gray16);
	tcase_add_test(t, load_corrupt_idat);
	tcase_add_test(t, load_truncated);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

