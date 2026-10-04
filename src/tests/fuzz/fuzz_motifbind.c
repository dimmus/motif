/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Virtual key bindings, as read from a .motifbind file (or the
 * _MOTIF_BINDINGS property that xmbind and other clients set): load
 * the input as a bindings file, then install it as the display's
 * bindings, which parses every binding with the String to
 * VirtualBinding converter, and translate a few keys through it.
 * Needs an X server (run under xvfb-run).
 */
#include <X11/keysym.h>
#include <Xm/XmP.h>
#include <Xm/DisplayP.h>
#include <Xm/VirtKeysI.h>

#include "fuzz_common.h"

static XmDisplay xmdisplay;

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	(void)argc;
	fuzz_open_display((*argv)[0]);
	xmdisplay = (XmDisplay)XmGetXmDisplay(XtDisplay(fuzz_top));
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	Display *dpy = XtDisplay(fuzz_top);
	String binding = NULL, saved;
	const char *path;
	KeySym out;
	Modifiers mods;
	KeyCode kc;

	if (size > 16 * 1024)
		return 0;
	path = fuzz_write_file(".motifbind", data, size);
	if (!_XmVirtKeysLoadFileBindings((char *)path, &binding))
		return 0;

	saved = xmdisplay->display.bindingsString;
	_XmVirtKeysDestroy((Widget)xmdisplay);
	xmdisplay->display.bindings = NULL;
	xmdisplay->display.lastKeyEvent = NULL;
	xmdisplay->display.bindingsString = binding;
	_XmVirtKeysInitialize((Widget)xmdisplay);
	xmdisplay->display.bindingsString = saved;

	kc = XKeysymToKeycode(dpy, XK_Tab);
	if (kc)
		XmTranslateKey(dpy, kc, ShiftMask | ControlMask, &mods, &out);
	kc = XKeysymToKeycode(dpy, XK_Left);
	if (kc)
		XmTranslateKey(dpy, kc, 0, &mods, &out);

	XtFree(binding);
	return 0;
}
