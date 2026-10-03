/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Compound text to XmString, as pasted from other clients: the NUL
 * terminated parser (XmCvtCTToXmString) and the sized one
 * (XmCvtTextToXmString), then the result back to compound text.
 * Xlib converts some charsets through the display, so this needs an X
 * server (run under xvfb-run).
 */
#include <Xm/Xm.h>

#include "fuzz_common.h"

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	(void)argc;
	fuzz_open_display((*argv)[0]);
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	Display *dpy = XtDisplay(fuzz_top);
	char *text = fuzz_strdup(data, size);
	XmString s;
	XrmValue from, to;
	Cardinal nargs = 0;

	if (!text)
		return 0;

	s = XmCvtCTToXmString(text);
	if (s) {
		char *ct = XmCvtXmStringToCT(s);

		XtFree(ct);
		XmStringFree(s);
	}

	/* The sized parser, over an exactly sized copy */
	from.addr = malloc(size ? size : 1);
	memcpy(from.addr, data, size);
	from.size = size;
	to.addr = NULL;
	to.size = 0;
	if (size && XmCvtTextToXmString(dpy, NULL, &nargs, &from, &to, NULL) &&
	    to.addr)
		XmStringFree((XmString)to.addr);	/* the string itself */
	free(from.addr);
	free(text);
	return 0;
}
