/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Pixel tests for XmeDrawShadows: every shadow type, thicknesses 0 to
 * 10 and box sizes down to ones too small for the thickness, read back
 * with XGetImage and compared with a model of the shadow as one line
 * per pixel row and column (the top and left in the top GC, the bottom
 * and right in the bottom GC, the etched ones as two such shadows).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <Xm/XmP.h>
#include <Xm/DrawP.h>
#include <check.h>

#include "suites.h"

#define PW 72
#define PH 56
#define BG 0x101010
#define TOP 0xe0e0e0
#define BOT 0x606060

static Widget shell;

static void _init_xt(void)
{
	shell = init_xt("check_Draw");
}

/* The pixels the model expects, as colors. */
static unsigned long model[PH][PW];

/* An axis-aligned line, both ends included, as the X server draws a
 * zero-width one, clipped to the model. */
static void model_line(const XSegment *s, unsigned long color)
{
	int x0 = s->x1 < s->x2 ? s->x1 : s->x2, x1 = s->x1 < s->x2 ? s->x2 : s->x1;
	int y0 = s->y1 < s->y2 ? s->y1 : s->y2, y1 = s->y1 < s->y2 ? s->y2 : s->y1;
	int x, y;

	for (y = y0 < 0 ? 0 : y0; y <= y1 && y < PH; y++)
		for (x = x0 < 0 ? 0 : x0; x <= x1 && x < PW; x++)
			model[y][x] = color;
}

/*
 * One simple shadow: thickness rows along the top and columns along the
 * left in <top>, then rows along the bottom and columns along the right
 * in <bot>, with the 16-bit arithmetic of the XSegment coordinates.
 */
static void model_simple(unsigned long top, unsigned long bot, Position x, Position y,
			 Dimension width, Dimension height, Dimension t, Dimension cor)
{
	XSegment s;
	int i;

	if (t > (width >> 1))
		t = width >> 1;
	if (t > (height >> 1))
		t = height >> 1;
	for (i = 0; i < t; i++) {
		s.x1 = x, s.y1 = s.y2 = y + i, s.x2 = x + width - i - 1;
		model_line(&s, top);
	}
	for (i = 0; i < t; i++) {
		s.x1 = s.x2 = x + i, s.y1 = y + t, s.y2 = y + height - i - 1;
		model_line(&s, top);
	}
	for (i = 0; i < t; i++) {
		s.x1 = x + i + (cor ? 0 : 1), s.y1 = s.y2 = y + height - i - 1;
		s.x2 = x + width - 1;
		model_line(&s, bot);
	}
	for (i = 0; i < t; i++) {
		s.x1 = s.x2 = x + width - i - 1, s.y1 = y + i + 1 - cor;
		s.y2 = y + height - 1;
		model_line(&s, bot);
	}
}

static void model_shadow(Position x, Position y, Dimension w, Dimension h, Dimension t,
			 unsigned int type)
{
	unsigned long top = TOP, bot = BOT, tmp;

	if (type == XmSHADOW_IN || type == XmSHADOW_ETCHED_IN)
		tmp = top, top = bot, bot = tmp;
	if ((type == XmSHADOW_ETCHED_IN || type == XmSHADOW_ETCHED_OUT) && t != 1) {
		model_simple(top, bot, x, y, w, h, t / 2, 1);
		model_simple(bot, top, x + t / 2, y + t / 2, w - (t / 2) * 2, h - (t / 2) * 2,
			     t / 2, 1);
	} else
		model_simple(top, bot, x, y, w, h, t, 0);
}

START_TEST(shadows)
{
	static const unsigned int types[] = { XmSHADOW_IN, XmSHADOW_OUT, XmSHADOW_ETCHED_IN,
					      XmSHADOW_ETCHED_OUT };
	static const int sizes[][2] = { { 40, 30 }, { 21, 21 }, { 8, 30 }, { 30, 7 },
					{ 3, 3 }, { 1, 9 }, { 20, 2 }, { 0, 5 } };
	Display *dpy = XtDisplay(shell);
	Window root = DefaultRootWindow(dpy);
	Pixmap pm;
	GC top, bot, clear;
	int ti, si, t, x, y;

	if (DefaultDepth(dpy, DefaultScreen(dpy)) != 24)
		return; /* the colors below are 24-bit pixels */
	pm = XCreatePixmap(dpy, root, PW, PH, 24);
	clear = XCreateGC(dpy, pm, 0, NULL);
	top = XCreateGC(dpy, pm, 0, NULL);
	bot = XCreateGC(dpy, pm, 0, NULL);
	XSetForeground(dpy, clear, BG);
	XSetForeground(dpy, top, TOP);
	XSetForeground(dpy, bot, BOT);

	for (ti = 0; ti < 4; ti++)
		for (si = 0; si < (int)(sizeof sizes / sizeof sizes[0]); si++)
			for (t = 0; t <= 10; t++) {
				XImage *img;

				XFillRectangle(dpy, pm, clear, 0, 0, PW, PH);
				XmeDrawShadows(dpy, pm, top, bot, 5, 4, sizes[si][0],
					       sizes[si][1], t, types[ti]);
				img = XGetImage(dpy, pm, 0, 0, PW, PH, AllPlanes, ZPixmap);
				ck_assert_ptr_nonnull(img);
				for (y = 0; y < PH; y++)
					for (x = 0; x < PW; x++)
						model[y][x] = BG;
				model_shadow(5, 4, sizes[si][0], sizes[si][1], t, types[ti]);
				for (y = 0; y < PH; y++)
					for (x = 0; x < PW; x++)
						ck_assert_msg((XGetPixel(img, x, y) & 0xffffff) ==
							      model[y][x],
							      "type %u, %dx%d, thickness %d: "
							      "pixel %d,%d is %06lx, not %06lx",
							      types[ti], sizes[si][0], sizes[si][1],
							      t, x, y, XGetPixel(img, x, y),
							      model[y][x]);
				XDestroyImage(img);
			}

	XFreeGC(dpy, clear);
	XFreeGC(dpy, top);
	XFreeGC(dpy, bot);
	XFreePixmap(dpy, pm);
}
END_TEST

void draw_suite(SRunner *runner)
{
	Suite *s = suite_create("Draw");
	TCase *t = tcase_create("Shadows");

	tcase_add_test(t, shadows);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
