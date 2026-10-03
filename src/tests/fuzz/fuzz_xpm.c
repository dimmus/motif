/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XPM parsing through the buffer interface (XpmCreateXpmImageFromBuffer),
 * then writing the image back to a buffer.  No display is needed.
 */
#include <X11/Xlib.h>
#include <Xm/XpmP.h>

#include "fuzz_common.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	char *buf = fuzz_strdup(data, size);
	XpmImage image;
	XpmInfo info;

	if (!buf)
		return 0;
	memset(&image, 0, sizeof image);
	memset(&info, 0, sizeof info);
	info.valuemask = XpmReturnComments | XpmReturnExtensions;
	if (XpmCreateXpmImageFromBuffer(buf, &image, &info) == XpmSuccess) {
		char *out = NULL;

		/* Skip the writer for images too large to be worth it */
		if ((unsigned long)image.width * image.height < (1UL << 20) &&
		    XpmCreateBufferFromXpmImage(&out, &image, &info) ==
		    XpmSuccess)
			XpmFree(out);
		XpmFreeXpmImage(&image);
		XpmFreeXpmInfo(&info);
	}
	free(buf);
	return 0;
}
