/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * X errors that Motif expects around a few of its requests (a bad atom
 * in a list of targets, a window that went away) are caught by error
 * traps that leave the application's error handler in place: they keep
 * only the errors of the requests made inside them, and pass any other
 * error on to the application.  Driven here through XmeClipboardSource,
 * which asks a widget for its clipboard targets and names each one with
 * XGetAtomName.
 */
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/CutPaste.h>
#include <Xm/DrawingA.h>
#include <Xm/TransferP.h>
#include <check.h>

#include "suites.h"

/* Not an atom of any server: XGetAtomName fails with BadAtom */
#define BOGUS_ATOM ((Atom)0x1fffffff)

static Widget top, da;
static int errors, bad_window_errors, atom_warnings;
static Window gone;		/* a window that was destroyed */
static Boolean make_bad_request;
static char payload[] = "trapped";

static int count_errors(Display *dpy, XErrorEvent *ev)
{
	(void)dpy;
	errors++;
	if (ev->error_code == BadWindow && ev->resourceid == gone)
		bad_window_errors++;
	return 0;
}

/* XmeWarning without a widget goes to the default warning handler */
static void count_warnings(String message)
{
	if (strstr(message, "atom"))
		atom_warnings++;
}

static Time server_time(Widget w)
{
	Display *dpy = XtDisplay(w);
	Atom prop = XInternAtom(dpy, "_MOTIF_TEST_TIME", False);
	XEvent ev;

	XSelectInput(dpy, XtWindow(w), PropertyChangeMask);
	XChangeProperty(dpy, XtWindow(w), prop, XA_STRING, 8, PropModeAppend,
			(unsigned char *)"", 0);
	XWindowEvent(dpy, XtWindow(w), PropertyChangeMask, &ev);
	return ev.xproperty.time;
}

/* The clipboard targets are STRING and an atom that does not exist. */
static void convert(Widget w, XtPointer client, XtPointer call)
{
	XmConvertCallbackStruct *cs = (XmConvertCallbackStruct *)call;
	Display *dpy = XtDisplay(w);
	Atom string = XA_STRING;

	(void)client;
	if (cs->target == XInternAtom(dpy, XmS_MOTIF_CLIPBOARD_TARGETS, False)) {
		Atom *targets = (Atom *)XtMalloc(2 * sizeof(Atom));

		targets[0] = string;
		targets[1] = BOGUS_ATOM;
		cs->value = (XtPointer)targets;
		cs->type = XA_ATOM;
		cs->format = 32;
		cs->length = 2;
		cs->status = XmCONVERT_DONE;
		/* A request whose error is still on its way when Motif
		 * starts to trap the errors of its own requests. */
		if (make_bad_request)
			XMapWindow(dpy, gone);
	} else if (cs->target == string) {
		cs->value = XtNewString(payload);
		cs->type = string;
		cs->format = 8;
		cs->length = strlen(payload);
		cs->status = XmCONVERT_DONE;
	} else {
		cs->status = XmCONVERT_REFUSE;
	}
}

static void setup(void)
{
	Arg args[2];
	Display *dpy;

	top = init_xt("check_XErrors");
	XtSetArg(args[0], XmNwidth, 100);
	XtSetArg(args[1], XmNheight, 100);
	da = XmCreateDrawingArea(top, "da", args, 2);
	XtAddCallback(da, XmNconvertCallback, convert, NULL);
	XtManageChild(da);
	XtRealizeWidget(top);
	dpy = XtDisplay(top);
	gone = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1, 1, 0, 0, 0);
	XDestroyWindow(dpy, gone);
	XSync(dpy, False);
	errors = bad_window_errors = atom_warnings = 0;
	make_bad_request = False;
	XSetErrorHandler(count_errors);
	XtSetWarningHandler(count_warnings);
}

static void teardown(void)
{
	XSetErrorHandler(NULL);
	XtSetWarningHandler(NULL);
	uninit_xt();
}

static void check_clipboard(Widget w)
{
	Display *dpy = XtDisplay(w);
	char buf[64];
	unsigned long len = 0;
	long id;

	memset(buf, 0, sizeof(buf));
	ck_assert_int_eq(XmClipboardRetrieve(dpy, XtWindow(w), "STRING", buf, sizeof(buf), &len,
					     &id),
			 XmClipboardSuccess);
	ck_assert_uint_eq(len, strlen(payload));
	ck_assert_str_eq(buf, payload);
}

/* The BadAtom of the bogus target stays inside Motif, which warns about
 * it, the good target is copied, and the application's handler is in
 * place afterwards. */
START_TEST(bad_target_atom_is_trapped)
{
	Display *dpy = XtDisplay(da);

	ck_assert(XmeClipboardSource(da, XmCOPY, server_time(da)));
	XSync(dpy, False);
	ck_assert_int_eq(errors, 0);
	ck_assert_int_eq(atom_warnings, 1);
	check_clipboard(da);
	ck_assert_ptr_eq(XSetErrorHandler(count_errors), count_errors);
}
END_TEST

/* An error of a request the application made before Motif's trap
 * started reaches the application's handler, even when it arrives
 * while the trap is on. */
START_TEST(earlier_error_reaches_application)
{
	Display *dpy = XtDisplay(da);

	make_bad_request = True;
	ck_assert(XmeClipboardSource(da, XmCOPY, server_time(da)));
	XSync(dpy, False);
	ck_assert_int_eq(bad_window_errors, 1);
	ck_assert_int_eq(errors, 1);
	check_clipboard(da);
	ck_assert_ptr_eq(XSetErrorHandler(count_errors), count_errors);
}
END_TEST

void xerrors_suite(SRunner *runner)
{
	Suite *s = suite_create("XErrors");
	TCase *t;

	t = tcase_create("Clipboard source");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, bad_target_atom_is_trapped);
	tcase_add_test(t, earlier_error_reaches_application);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
