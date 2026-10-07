/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmClipboard against malformed records left on the root window by
 * another client.  Any X client can rewrite the _MOTIF_CLIP_* root
 * properties the clipboard code reads, so a corrupt record set must not
 * be able to terminate a Motif application.
 *
 * Regression for the corrupt-record exit: a crafted record set used to
 * reach ClipboardError() -> XtErrorMsg() -> exit(), killing every Motif
 * application that touched the clipboard (reproducer
 * fuzz/crashes/clipboard/corrupt-record-exit).  ClipboardError() now
 * warns and the operation fails cleanly, so this test, which libcheck
 * runs in its own forked process, completes instead of the child exiting.
 *
 * Failing cleanly includes giving the clipboard lock back: the entry
 * points that meet a corrupt record must not return with the lock (a
 * selection every other client then waits on) still held.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/CutPaste.h>
#include <check.h>

#include "suites.h"

static Widget top;
static Window win;
static int saw_warning;

/* The exact record set that used to make the clipboard code exit(). */
static const unsigned char corrupt_records[] = {
	2, 237, 3, 64, 0, 1, 0, 0, 0, 233, 3, 0, 0, 96, 3, 252,
	200, 18, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255,
	255, 5, 0, 0, 0, 238, 3, 0, 0, 246, 0, 0, 0, 13, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 237, 3, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 2, 235, 3, 64, 0, 1, 0, 0, 0, 233, 3,
	0, 0, 96, 3, 252, 200, 18, 0, 32, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 255, 255, 255, 255, 5, 0, 0, 0, 236, 3, 0, 0, 31, 0,
	0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 235, 3,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 3,
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 64, 0, 0, 0, 233,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 3, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 18, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233,
	3, 0, 0, 2, 233, 3, 68, 0, 2, 0, 0, 0, 0, 0, 0, 0,
	96, 3, 252, 200, 18, 0, 32, 0, 0, 0, 0, 0, 0, 0, 3, 0,
	0, 237, 0, 0, 0, 0, 0, 0, 0, 2, 235, 3, 64, 0, 1, 0,
	0, 0, 233, 3, 0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0,
	0, 0, 0, 0, 235, 3, 0, 0, 237, 3, 0, 0, 1, 0, 0, 4,
	0, 208, 7, 0, 0, 2, 2, 0, 0, 0,
};

static void count_warning(String name, String type, String klass,
			  String def, String *params, Cardinal *num)
{
	(void)name;
	(void)type;
	(void)klass;
	(void)def;
	(void)params;
	(void)num;
	saw_warning++;
}

static void setup(void)
{
	Arg args[2];

	top = init_xt("check_Clipboard");
	XtSetArg(args[0], XmNwidth, 10);
	XtSetArg(args[1], XmNheight, 10);
	XtSetValues(top, args, 2);
	XtRealizeWidget(top);
	win = XtWindow(top);
	saw_warning = 0;
	/* keep a corrupt-record warning out of the test log, and count it */
	XtAppSetWarningMsgHandler(app, count_warning);
}

static void teardown(void)
{
	uninit_xt();
}

/* Decode the [kind][id:2][len:2][data] framing the DnD/clipboard fuzzer
 * uses and write each record as the matching _MOTIF_CLIP_* property, the
 * way a hostile peer would. */
static void install_corrupt_records(Display *dpy, Window root,
				     const unsigned char *data, size_t size)
{
	size_t pos = 0;

	while (pos + 5 <= size) {
		unsigned kind = data[pos];
		unsigned id = data[pos + 1] | data[pos + 2] << 8;
		size_t n = (size_t)(data[pos + 3] | data[pos + 4] << 8);
		long values[1024];
		size_t j;
		char name[64];
		Atom atom;

		pos += 5;
		if (n > size - pos)
			n = size - pos;
		for (j = 0; j < n / 4 && j < 1024; j++)
			values[j] = (long)(int)(data[pos + 4 * j] |
				data[pos + 4 * j + 1] << 8 |
				data[pos + 4 * j + 2] << 16 |
				(unsigned)data[pos + 4 * j + 3] << 24);
		pos += n;
		if (kind == 0)
			snprintf(name, sizeof name, "_MOTIF_CLIP_HEADER");
		else if (kind == 1)
			snprintf(name, sizeof name, "_MOTIF_CLIP_NEXT_ID");
		else
			snprintf(name, sizeof name, "_MOTIF_CLIP_ITEM_%u", id);
		atom = XInternAtom(dpy, name, False);
		XChangeProperty(dpy, root, atom, XA_INTEGER, 32,
				PropModeReplace, (unsigned char *)values,
				(int)j);
	}
	XSync(dpy, False);
}

static void clear_records(Display *dpy, Window root)
{
	int i, n = 0;
	Atom *props = XListProperties(dpy, root, &n);

	for (i = 0; props && i < n; i++) {
		char *name = XGetAtomName(dpy, props[i]);

		if (name && !strncmp(name, "_MOTIF_CLIP_", 12))
			XDeleteProperty(dpy, root, props[i]);
		if (name)
			XFree(name);
	}
	if (props)
		XFree(props);
	XSync(dpy, False);
}

static Window lock_owner(Display *dpy)
{
	return XGetSelectionOwner(dpy, XInternAtom(dpy, "_MOTIF_CLIP_LOCK",
						   False));
}

/* Remove one clipboard record, the way a hostile client (or one that
 * died half way) leaves the record set inconsistent. */
static void drop_item(Display *dpy, Window root, long id)
{
	char name[64];

	snprintf(name, sizeof name, "_MOTIF_CLIP_ITEM_%ld", id);
	XDeleteProperty(dpy, root, XInternAtom(dpy, name, False));
	XSync(dpy, False);
}

/* Remove the format data records: they are the only _MOTIF_CLIP_ITEM_*
 * properties that are not of type INTEGER. */
static void drop_format_data(Display *dpy, Window root)
{
	int i, n = 0;
	Atom *props = XListProperties(dpy, root, &n);

	for (i = 0; props && i < n; i++) {
		char *name = XGetAtomName(dpy, props[i]);
		Atom type = None;
		int format;
		unsigned long nitems, after;
		unsigned char *data = NULL;

		if (name && !strncmp(name, "_MOTIF_CLIP_ITEM_", 17) &&
		    XGetWindowProperty(dpy, root, props[i], 0, 0, False,
				       AnyPropertyType, &type, &format,
				       &nitems, &after, &data) == Success &&
		    type != XA_INTEGER)
			XDeleteProperty(dpy, root, props[i]);
		if (data)
			XFree(data);
		if (name)
			XFree(name);
	}
	if (props)
		XFree(props);
	XSync(dpy, False);
}

/* A complete, valid copy of a string; returns the item id. */
static long copy_string(Display *dpy, Window w, const char *str)
{
	long item_id = 0, data_id = 0;

	ck_assert_int_eq(XmClipboardStartCopy(dpy, w, NULL, CurrentTime,
					      NULL, NULL, &item_id),
			 ClipboardSuccess);
	ck_assert_int_eq(XmClipboardCopy(dpy, w, item_id, "STRING",
					 (XtPointer)str, strlen(str), 0,
					 &data_id),
			 ClipboardSuccess);
	ck_assert_int_eq(XmClipboardEndCopy(dpy, w, item_id),
			 ClipboardSuccess);
	return item_id;
}

/*
 * The whole point: reaching the assertion means the clipboard calls
 * returned instead of exiting the process.  libcheck runs this in a
 * forked child, so the pre-fix exit() would have been reported as an
 * unexpected early exit and failed the test.
 */
START_TEST(corrupt_records_do_not_exit)
{
	Display *dpy = XtDisplay(top);
	Window root = RootWindow(dpy, 0);
	int count = 0, i;
	unsigned long maxlen = 0, outlen = 0, len = 0;
	char fmt[256], buf[256];
	long private_id = 0;
	XmClipboardPendingList list = NULL;
	unsigned long nitems = 0;

	clear_records(dpy, root);
	install_corrupt_records(dpy, root, corrupt_records,
				sizeof corrupt_records);

	/* each of these walks the malformed records */
	XmClipboardInquireCount(dpy, win, &count, &maxlen);
	for (i = 1; i <= count && i <= 16; i++) {
		if (XmClipboardInquireFormat(dpy, win, i, fmt, sizeof fmt,
					     &outlen) == ClipboardSuccess) {
			if (outlen >= sizeof fmt)
				outlen = sizeof fmt - 1;
			fmt[outlen] = '\0';
			XmClipboardInquireLength(dpy, win, fmt, &len);
			XmClipboardRetrieve(dpy, win, fmt, buf, sizeof buf,
					    &outlen, &private_id);
		}
	}
	if (XmClipboardInquirePendingItems(dpy, win, "STRING", &list,
					   &nitems) == ClipboardSuccess)
		XtFree((char *)list);

	/* We are still here: the corrupt records did not terminate us.
	 * They were reported, and the clipboard lock was given back. */
	ck_assert_int_gt(saw_warning, 0);
	ck_assert(lock_owner(dpy) == None);
	clear_records(dpy, root);
}
END_TEST

/* XmClipboardEndCopy() on an item whose record vanished mid copy. */
START_TEST(end_copy_corrupt_releases_lock)
{
	Display *dpy = XtDisplay(top);
	Window root = RootWindow(dpy, 0);
	long item_id = 0, data_id = 0;

	clear_records(dpy, root);
	ck_assert_int_eq(XmClipboardStartCopy(dpy, win, NULL, CurrentTime,
					      NULL, NULL, &item_id),
			 ClipboardSuccess);
	XmClipboardCopy(dpy, win, item_id, "STRING", "x", 1, 0, &data_id);
	drop_item(dpy, root, item_id);
	ck_assert_int_eq(XmClipboardEndCopy(dpy, win, item_id),
			 ClipboardFail);
	ck_assert_int_gt(saw_warning, 0);
	ck_assert(lock_owner(dpy) == None);
	clear_records(dpy, root);
}
END_TEST

/* XmClipboardUndoCopy() when the last copied item is gone. */
START_TEST(undo_copy_corrupt_releases_lock)
{
	Display *dpy = XtDisplay(top);
	Window root = RootWindow(dpy, 0);

	clear_records(dpy, root);
	drop_item(dpy, root, copy_string(dpy, win, "hello"));
	ck_assert_int_eq(XmClipboardUndoCopy(dpy, win), ClipboardFail);
	ck_assert_int_gt(saw_warning, 0);
	ck_assert(lock_owner(dpy) == None);
	clear_records(dpy, root);
}
END_TEST

/* XmClipboardCopyByName() for a format record that does not exist. */
START_TEST(copy_by_name_corrupt_releases_lock)
{
	Display *dpy = XtDisplay(top);
	Window root = RootWindow(dpy, 0);

	clear_records(dpy, root);
	ck_assert_int_eq(XmClipboardCopyByName(dpy, win, 4242, "x", 1, 0),
			 ClipboardFail);
	ck_assert_int_gt(saw_warning, 0);
	ck_assert(lock_owner(dpy) == None);
	clear_records(dpy, root);
}
END_TEST

/* XmClipboardRetrieve() of our own copy whose format data is gone. */
START_TEST(retrieve_corrupt_releases_lock)
{
	Display *dpy = XtDisplay(top);
	Window root = RootWindow(dpy, 0);
	char buf[64];
	unsigned long outlen = 0;
	long private_id = 0;

	clear_records(dpy, root);
	copy_string(dpy, win, "hello");
	drop_format_data(dpy, root);
	ck_assert_int_eq(XmClipboardRetrieve(dpy, win, "STRING", buf,
					     sizeof buf, &outlen,
					     &private_id),
			 ClipboardFail);
	ck_assert_int_gt(saw_warning, 0);
	ck_assert(lock_owner(dpy) == None);
	clear_records(dpy, root);
}
END_TEST

void clipboard_suite(SRunner *runner)
{
	Suite *s = suite_create("Clipboard");
	TCase *t = tcase_create("Corrupt records");

	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, corrupt_records_do_not_exit);
	tcase_add_test(t, end_copy_corrupt_releases_lock);
	tcase_add_test(t, undo_copy_corrupt_releases_lock);
	tcase_add_test(t, copy_by_name_corrupt_releases_lock);
	tcase_add_test(t, retrieve_corrupt_releases_lock);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
