/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * vizshot: render a fixed scene with XmString and plain X drawing into
 * an off-screen pixmap, using a bitmap (core) font so the result is
 * deterministic, capture it with XGetImage, and write or compare a
 * golden PNG.
 *
 *   vizshot --fontpath DIR --out FILE.png
 *   vizshot --fontpath DIR --check GOLDEN.png [--tolerance N]
 *   vizshot --fontpath DIR --selftest        (render twice, compare)
 *
 * The scene uses no window manager, no mapping and no Xft, so two runs
 * on the same server produce identical pixels; --check allows up to N
 * (default 0) differing pixels for small cross-build variation.  Exits
 * 77 without a display or the font.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>

#define W 300
#define H 160
#define FONT "-Misc-Fixed-Medium-R-SemiCondensed--13-120-75-75-C-60-ISO8859-1"

static XtAppContext app;

static XmFontList load_font(Display *dpy)
{
	XmFontListEntry e;
	XmFontList fl;

	e = XmFontListEntryLoad(dpy, FONT, XmFONT_IS_FONT,
				XmFONTLIST_DEFAULT_TAG);
	if (!e)
		return NULL;
	fl = XmFontListAppendEntry(NULL, e);
	XmFontListEntryFree(&e);
	return fl;
}

/* Draw the scene into a 1:1 pixmap on the default (static) visual. */
static Pixmap render(Display *dpy, XmFontList fl)
{
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);
	unsigned long white = WhitePixel(dpy, screen);
	unsigned long black = BlackPixel(dpy, screen);
	Pixmap pm = XCreatePixmap(dpy, root, W, H, DefaultDepth(dpy, screen));
	GC gc = XCreateGC(dpy, pm, 0, NULL);
	XmString s;
	XRectangle clip = { 0, 0, W, H };

	XSetForeground(dpy, gc, white);
	XFillRectangle(dpy, pm, gc, 0, 0, W, H);
	XSetForeground(dpy, gc, black);

	/* A frame and a couple of lines, as fixed landmarks */
	XDrawRectangle(dpy, pm, gc, 4, 4, W - 9, H - 9);
	XDrawLine(dpy, pm, gc, 4, 24, W - 5, 24);
	XDrawLine(dpy, pm, gc, 4, H - 24, W - 5, H - 24);

	s = XmStringCreateLocalized("Motif visual regression");
	XmStringDraw(dpy, pm, fl, s, gc, 10, 8, W - 20,
		     XmALIGNMENT_BEGINNING, XmSTRING_DIRECTION_L_TO_R, &clip);
	XmStringFree(s);

	s = XmStringConcatAndFree(
		XmStringConcatAndFree(XmStringCreateLocalized("left"),
				      XmStringComponentCreate(
					      XmSTRING_COMPONENT_TAB, 0, NULL)),
		XmStringCreateLocalized("right"));
	XmStringDraw(dpy, pm, fl, s, gc, 10, 34, W - 20,
		     XmALIGNMENT_BEGINNING, XmSTRING_DIRECTION_L_TO_R, &clip);
	XmStringFree(s);

	s = XmStringConcatAndFree(
		XmStringConcatAndFree(XmStringCreateLocalized("line one"),
				      XmStringSeparatorCreate()),
		XmStringCreateLocalized("line two"));
	XmStringDraw(dpy, pm, fl, s, gc, 10, 54, W - 20,
		     XmALIGNMENT_CENTER, XmSTRING_DIRECTION_L_TO_R, &clip);
	XmStringFree(s);

	s = XmStringCreateLocalized("0123456789 !@#$%^&*()");
	XmStringDraw(dpy, pm, fl, s, gc, 10, H - 20, W - 20,
		     XmALIGNMENT_END, XmSTRING_DIRECTION_L_TO_R, &clip);
	XmStringFree(s);

	XFreeGC(dpy, gc);
	XSync(dpy, False);
	return pm;
}

/* Capture a pixmap as 24-bit RGB (3 bytes/pixel), via the colormap. */
static unsigned char *capture(Display *dpy, Pixmap pm)
{
	int screen = DefaultScreen(dpy);
	Colormap cmap = DefaultColormap(dpy, screen);
	XImage *img = XGetImage(dpy, pm, 0, 0, W, H, AllPlanes, ZPixmap);
	unsigned char *rgb;
	XColor cols[256];
	int x, y, i;

	if (!img)
		return NULL;
	/* Resolve pixel values to RGB through the colormap; for the few
	 * colours used (black and white) this is exact. */
	for (i = 0; i < 256; i++) {
		cols[i].pixel = i;
		cols[i].flags = DoRed | DoGreen | DoBlue;
	}
	XQueryColors(dpy, cmap, cols, 256);
	rgb = malloc((size_t)W * H * 3);
	for (y = 0; y < H; y++)
		for (x = 0; x < W; x++) {
			unsigned long p = XGetPixel(img, x, y);
			unsigned char *o = rgb + ((size_t)y * W + x) * 3;

			if (p < 256) {
				o[0] = cols[p].red >> 8;
				o[1] = cols[p].green >> 8;
				o[2] = cols[p].blue >> 8;
			} else {
				o[0] = (p >> 16) & 0xff;
				o[1] = (p >> 8) & 0xff;
				o[2] = p & 0xff;
			}
		}
	XDestroyImage(img);
	return rgb;
}

static int write_png(const char *path, const unsigned char *rgb)
{
	FILE *f = fopen(path, "wb");
	png_structp png;
	png_infop info;
	int y;

	if (!f)
		return -1;
	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png))) {
		png_destroy_write_struct(&png, &info);
		fclose(f);
		return -1;
	}
	png_init_io(png, f);
	png_set_IHDR(png, info, W, H, 8, PNG_COLOR_TYPE_RGB,
		     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		     PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png, info);
	for (y = 0; y < H; y++)
		png_write_row(png, (png_bytep)(rgb + (size_t)y * W * 3));
	png_write_end(png, info);
	png_destroy_write_struct(&png, &info);
	fclose(f);
	return 0;
}

static unsigned char *read_png(const char *path)
{
	FILE *f = fopen(path, "rb");
	png_structp png;
	png_infop info;
	unsigned char *rgb;
	int y;

	if (!f)
		return NULL;
	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, &info, NULL);
		fclose(f);
		return NULL;
	}
	png_init_io(png, f);
	png_read_info(png, info);
	if ((int)png_get_image_width(png, info) != W ||
	    (int)png_get_image_height(png, info) != H ||
	    png_get_color_type(png, info) != PNG_COLOR_TYPE_RGB ||
	    png_get_bit_depth(png, info) != 8) {
		png_destroy_read_struct(&png, &info, NULL);
		fclose(f);
		return NULL;
	}
	rgb = malloc((size_t)W * H * 3);
	for (y = 0; y < H; y++)
		png_read_row(png, (png_bytep)(rgb + (size_t)y * W * 3), NULL);
	png_destroy_read_struct(&png, &info, NULL);
	fclose(f);
	return rgb;
}

/* Number of pixels that differ by more than a small per-channel delta */
static long diff_pixels(const unsigned char *a, const unsigned char *b)
{
	long n = 0;
	size_t i;

	for (i = 0; i < (size_t)W * H; i++) {
		int d = 0, c;

		for (c = 0; c < 3; c++) {
			int t = a[i * 3 + c] - b[i * 3 + c];

			if (t < 0)
				t = -t;
			if (t > d)
				d = t;
		}
		if (d > 16)
			n++;
	}
	return n;
}

int main(int argc, char **argv)
{
	const char *fontpath = NULL, *out = NULL, *golden = NULL;
	int selftest = 0, tolerance = 0, i, argc2 = 1;
	char *argv2[] = { "vizshot", NULL };
	Display *dpy;
	Widget shell;
	XmFontList fl;
	Pixmap pm;
	unsigned char *rgb, *rgb2 = NULL, *gold = NULL;
	int rc;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--fontpath") && i + 1 < argc)
			fontpath = argv[++i];
		else if (!strcmp(argv[i], "--out") && i + 1 < argc)
			out = argv[++i];
		else if (!strcmp(argv[i], "--check") && i + 1 < argc)
			golden = argv[++i];
		else if (!strcmp(argv[i], "--tolerance") && i + 1 < argc)
			tolerance = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--selftest"))
			selftest = 1;
	}
	if (!getenv("DISPLAY") || !*getenv("DISPLAY")) {
		printf("vizshot: SKIP: DISPLAY is not set\n");
		return 77;
	}
	XtSetLanguageProc(NULL, NULL, NULL);
	XtToolkitInitialize();
	app = XtCreateApplicationContext();
	dpy = XtOpenDisplay(app, NULL, "vizshot", "Vizshot", NULL, 0,
			    &argc2, argv2);
	if (!dpy) {
		printf("vizshot: SKIP: cannot open the display\n");
		return 77;
	}
	if (fontpath) {
		int n = 0;
		char **old = XGetFontPath(dpy, &n);
		char **path = malloc((n + 1) * sizeof *path);
		int j;

		path[0] = (char *)fontpath;
		for (j = 0; j < n; j++)
			path[j + 1] = old[j];
		XSetFontPath(dpy, path, n + 1);
		free(path);
		if (old)
			XFreeFontPath(old);
	}

	/* An (unmapped) shell, so XmDisplay is created and later destroyed
	 * with it, keeping its per-display allocations out of the leak
	 * report. */
	shell = XtAppCreateShell("vizshot", "Vizshot",
				 applicationShellWidgetClass, dpy, NULL, 0);

	fl = load_font(dpy);
	if (!fl) {
		printf("vizshot: SKIP: cannot load %s\n", FONT);
		return 77;
	}

	pm = render(dpy, fl);
	rgb = capture(dpy, pm);
	XFreePixmap(dpy, pm);
	if (!rgb) {
		fprintf(stderr, "vizshot: capture failed\n");
		return 1;
	}

	rc = 2;
	if (selftest) {
		pm = render(dpy, fl);
		rgb2 = capture(dpy, pm);
		XFreePixmap(dpy, pm);
		if (!rgb2 || memcmp(rgb, rgb2, (size_t)W * H * 3) != 0) {
			fprintf(stderr, "vizshot: rendering is not deterministic"
				" (%ld pixels differ)\n",
				rgb2 ? diff_pixels(rgb, rgb2) : -1);
			rc = 1;
		} else {
			printf("vizshot: deterministic across two runs\n");
			rc = 0;
		}
		free(rgb2);
	} else if (out) {
		if (write_png(out, rgb) != 0) {
			fprintf(stderr, "vizshot: cannot write %s\n", out);
			rc = 1;
		} else {
			printf("vizshot: wrote %s\n", out);
			rc = 0;
		}
	} else if (golden) {
		gold = read_png(golden);
		if (!gold) {
			fprintf(stderr, "vizshot: cannot read golden %s\n",
				golden);
			rc = 1;
		} else {
			long n = diff_pixels(rgb, gold);

			if (n > tolerance) {
				fprintf(stderr, "vizshot: %ld pixels differ "
					"from the golden (tolerance %d)\n",
					n, tolerance);
				rc = 1;
			} else {
				printf("vizshot: matches the golden (%ld "
				       "pixels differ, tolerance %d)\n",
				       n, tolerance);
				rc = 0;
			}
			free(gold);
		}
	} else {
		fprintf(stderr, "usage: see the header\n");
	}

	free(rgb);
	XmFontListFree(fl);
	XtDestroyWidget(shell);
	XtDestroyApplicationContext(app);
	return rc;
}
