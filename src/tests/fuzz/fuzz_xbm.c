/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XBM files, read the way the image cache reads bitmap files named in
 * resources (_XmReadImageAndHotSpotFromFile: Xlib parses the file,
 * Motif builds the XImage), then a pixmap made from the image.  Needs
 * an X server (run under xvfb-run).
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/ReadImageI.h>

#include "fuzz_common.h"

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	(void)argc;
	fuzz_open_display((*argv)[0]);
	return 0;
}

/*
 * XReadBitmapFileData() (libX11, not Motif) allocates what the
 * _width and _height defines ask for before reading any data; skip
 * inputs that ask for gigabytes, which only stop the fuzzer.
 */
static int too_large(const uint8_t *data, size_t size)
{
	char *text = fuzz_strdup(data, size), *p;
	int big = 0;

	for (p = text; p && (p = strstr(p, "#define")) != NULL; p++) {
		char name[256];
		unsigned long v;

		if (sscanf(p, "#define %255s %lu", name, &v) == 2 && v > 4096)
			big = 1;
	}
	free(text);
	return big;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	Display *dpy = XtDisplay(fuzz_top);
	const char *path;
	XImage *img;
	int hot_x = -1, hot_y = -1;

	if (size > 64 * 1024 || too_large(data, size))
		return 0;
	path = fuzz_write_file(".xbm", data, size);
	img = _XmReadImageAndHotSpotFromFile(dpy, (char *)path, &hot_x, &hot_y);
	if (!img)
		return 0;
	if (img->width > 0 && img->height > 0 &&
	    img->width <= 4096 && img->height <= 4096) {
		Window root = DefaultRootWindow(dpy);
		Pixmap pm = XCreatePixmap(dpy, root, img->width, img->height, 1);
		GC gc = XCreateGC(dpy, pm, 0, NULL);

		XPutImage(dpy, pm, gc, img, 0, 0, 0, 0, img->width,
			  img->height);
		XFreeGC(dpy, gc);
		XFreePixmap(dpy, pm);
	}
	XDestroyImage(img);
	return 0;
}
