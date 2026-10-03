/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * The image loaders, built once per format with FUZZ_PNG, FUZZ_JPEG or
 * FUZZ_SVG: decode the input into an XImage, as the image cache does
 * for files named in resources.  No display is needed.
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "fuzz_common.h"

#if defined(FUZZ_PNG)
#include "PngI.h"
#elif defined(FUZZ_JPEG)
#include "JpegI.h"
#elif defined(FUZZ_SVG)
#include "SvgI.h"
#else
#error "define FUZZ_PNG, FUZZ_JPEG or FUZZ_SVG"
#endif

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	XImage *img = NULL;
	FILE *fp = fuzz_fmemopen(data, size);

	if (!fp)
		return 0;
#if defined(FUZZ_PNG)
	{
		XColor bg;

		memset(&bg, 0, sizeof bg);
		bg.red = bg.green = bg.blue = 0x8000;
		_XmPngGetImage(fp, (size & 1) ? &bg : NULL, &img);
	}
#elif defined(FUZZ_JPEG)
	_XmJpegGetImage(fp, &img);
#else
	_XmSvgGetImage(fp, &img);
#endif
	fclose(fp);
	if (img) {
		/* Read the pixels back, a bounded number of them */
		int x, y;
		long n = 0;
		unsigned long sum = 0;

		for (y = 0; y < img->height && n < 65536; y++)
			for (x = 0; x < img->width && n < 65536; x++, n++)
				sum += XGetPixel(img, x, y);
		(void)sum;
		XDestroyImage(img);
	}
	return 0;
}
