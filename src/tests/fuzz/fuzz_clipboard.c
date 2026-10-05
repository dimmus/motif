/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * The clipboard records other clients keep on the root window: the
 * input is a list of records stored as _MOTIF_CLIP_HEADER,
 * _MOTIF_CLIP_NEXT_ID and _MOTIF_CLIP_ITEM_<n> properties (32-bit
 * INTEGER data, like the real ones), then the inquire and retrieve
 * functions read them and a copy is made on top of them.  Needs an X
 * server (run under xvfb-run).
 *
 * Input: records of [kind][id (2 bytes)][length (2 bytes)][data], where
 * kind 0 is the header, 1 the next id record and anything else item id.
 *
 * A record it finds corrupt is reported with a warning now and the
 * clipboard operation fails cleanly, so a hostile client can no longer
 * make the application exit (it used to: ClipboardError() called
 * XtErrorMsg(), whose default handler exits).  The former reproducer is
 * now the clean corpus seed corpus/clipboard/corrupt-record-exit.  The
 * fatal error handler below, and FUZZ_CLIPBOARD_EXIT, are kept so the
 * old behaviour reappears if anything regresses to XtErrorMsg.
 */
#include <setjmp.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/CutPaste.h>

#include "fuzz_common.h"

static Window window;
static jmp_buf on_error;
static int in_input;

static _X_NORETURN void error_msg(String name, String type, String klass,
				  String defaultp, String *params,
				  Cardinal *num_params)
{
	(void)name; (void)type; (void)klass; (void)defaultp;
	(void)params; (void)num_params;
	if (in_input)
		longjmp(on_error, 1);
	abort();
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	Widget top;

	(void)argc;
	top = fuzz_open_display((*argv)[0]);
	XtVaSetValues(top, XmNwidth, 10, XmNheight, 10, NULL);
	XtRealizeWidget(top);
	window = XtWindow(top);
	if (!getenv("FUZZ_CLIPBOARD_EXIT"))
		XtAppSetErrorMsgHandler(fuzz_app, error_msg);
	return 0;
}

static void clear_records(Display *dpy, Window root)
{
	int i, n = 0;
	Atom *props = XListProperties(dpy, root, &n);

	for (i = 0; props && i < n; i++) {
		char *name = XGetAtomName(dpy, props[i]);

		if (name && !strncmp(name, "_MOTIF_CLIP_", 12))
			XDeleteProperty(dpy, root, props[i]);
		XFree(name);
	}
	XFree(props);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	Display *dpy = XtDisplay(fuzz_top);
	Window root = RootWindow(dpy, 0);
	size_t pos = 0;
	unsigned long count = 0, max_len = 0, len = 0, outlen = 0;
	long private_id = 0, item_id = 0, data_id = 0;
	int i, n_formats = 0;
	char buf[256];

	if (size > 16 * 1024)
		return 0;
	clear_records(dpy, root);
	if (setjmp(on_error)) {
		in_input = 0;
		XSync(dpy, False);
		return 0;
	}
	in_input = 1;

	while (pos + 5 <= size) {
		unsigned kind = data[pos];
		unsigned id = data[pos + 1] | data[pos + 2] << 8;
		size_t n = data[pos + 3] | data[pos + 4] << 8;
		long values[1024];
		size_t j;
		char name[64];
		Atom atom;

		pos += 5;
		if (n > size - pos)
			n = size - pos;
		/* 32-bit property data is passed as longs */
		for (j = 0; j < n / 4 && j < 1024; j++)
			values[j] = (long)(int32_t)(data[pos + 4 * j] |
				data[pos + 4 * j + 1] << 8 |
				data[pos + 4 * j + 2] << 16 |
				(uint32_t)data[pos + 4 * j + 3] << 24);
		pos += n;
		if (kind == 0)
			snprintf(name, sizeof name, "_MOTIF_CLIP_HEADER");
		else if (kind == 1)
			snprintf(name, sizeof name, "_MOTIF_CLIP_NEXT_ID");
		else
			snprintf(name, sizeof name, "_MOTIF_CLIP_ITEM_%u", id);
		atom = XInternAtom(dpy, name, False);
		XChangeProperty(dpy, root, atom, XA_INTEGER, 32,
				PropModeReplace, (unsigned char *)values, (int)j);
	}

	XmClipboardInquireCount(dpy, window, &n_formats, &max_len);
	for (i = 1; i <= n_formats && i <= 16; i++) {
		if (XmClipboardInquireFormat(dpy, window, i, buf, sizeof buf,
					     &outlen) == ClipboardSuccess) {
			char *fmt = fuzz_strdup((const uint8_t *)buf,
						outlen < sizeof buf ? outlen
								    : sizeof buf - 1);

			XmClipboardInquireLength(dpy, window, fmt, &len);
			XmClipboardRetrieve(dpy, window, fmt, buf, sizeof buf,
					    &outlen, &private_id);
			free(fmt);
		}
	}
	{
		XmClipboardPendingList list = NULL;

		if (XmClipboardInquirePendingItems(dpy, window, "STRING", &list,
						   &count) == ClipboardSuccess)
			XtFree((char *)list);
	}

	/* A copy on top of whatever is there */
	if (XmClipboardStartCopy(dpy, window, NULL, CurrentTime, NULL, NULL,
				 &item_id) == ClipboardSuccess) {
		XmClipboardCopy(dpy, window, item_id, "STRING", "fuzz", 4, 0,
				&data_id);
		XmClipboardEndCopy(dpy, window, item_id);
	}
	in_input = 0;
	XSync(dpy, False);
	return 0;
}
