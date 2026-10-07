/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmIm: what reaches the input context.  The suite interposes the Xlib
 * input method calls XmIm makes (XSetICValues, XCreateIC) to count the
 * XSetICValues requests (each one is a round trip with an input method
 * server) and to see the spot location they carry.  They forward to
 * Xlib's local input method (XMODIFIERS=@im=none).
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/XmIm.h>
#include <check.h>

#include "suites.h"

/* ------------------------------------------------------------------ */
/* Interposed Xlib calls                                               */
/* ------------------------------------------------------------------ */

/*
 * The argument lists of the XIC calls, and Xlib's nested lists, are
 * name/value pairs ending with a NULL name (XIMArg in Xlib's Xlcint.h).
 * XmIm passes a handful of pairs; forward up to IM_MAX_ARGS.
 */
typedef struct {
	char *name;
	XPointer value;
} ImArg;

#define IM_MAX_ARGS 16

static struct {
	int active;		/* record the calls */
	unsigned long set_calls;	/* XSetICValues calls */
	unsigned long creates;	/* XCreateIC calls */
	unsigned long spots;	/* XNSpotLocation values passed */
	XPoint spot;		/* the last of them */
} im;

static void record(const ImArg *a, int preedit)
{
	for (; a && a->name; a++) {
		if (!strcmp(a->name, XNVaNestedList))
			record((const ImArg *)a->value, preedit);
		else if (!strcmp(a->name, XNPreeditAttributes))
			record((const ImArg *)a->value, 1);
		else if (preedit && !strcmp(a->name, XNSpotLocation)) {
			im.spot = *(XPoint *)a->value;
			im.spots++;
		}
	}
}

static void collect(va_list ap, ImArg *a)
{
	int n;

	for (n = 0; n < IM_MAX_ARGS; n++) {
		if (!(a[n].name = va_arg(ap, char *)))
			break;
		a[n].value = va_arg(ap, XPointer);
	}
	ck_assert_msg(n < IM_MAX_ARGS, "too many arguments to forward");
}

#define IM_FORWARD(real, first, a) \
	(real)(first, a[0].name, a[0].value, a[1].name, a[1].value, \
	       a[2].name, a[2].value, a[3].name, a[3].value, \
	       a[4].name, a[4].value, a[5].name, a[5].value, \
	       a[6].name, a[6].value, a[7].name, a[7].value, \
	       a[8].name, a[8].value, a[9].name, a[9].value, \
	       a[10].name, a[10].value, a[11].name, a[11].value, \
	       a[12].name, a[12].value, a[13].name, a[13].value, \
	       a[14].name, a[14].value, a[15].name, a[15].value, NULL)

typedef char *(*set_ic_fn)(XIC, ...);
typedef XIC (*create_ic_fn)(XIM, ...);

char *XSetICValues(XIC ic, ...)
{
	static set_ic_fn real;
	ImArg a[IM_MAX_ARGS + 1] = { { NULL, NULL } };
	va_list ap;

	va_start(ap, ic);
	collect(ap, a);
	va_end(ap);
	if (im.active) {
		im.set_calls++;
		record(a, 0);
	}
	if (!real)
		real = (set_ic_fn)dlsym(RTLD_NEXT, "XSetICValues");
	return IM_FORWARD(real, ic, a);
}

XIC XCreateIC(XIM xim, ...)
{
	static create_ic_fn real;
	ImArg a[IM_MAX_ARGS + 1] = { { NULL, NULL } };
	va_list ap;

	va_start(ap, xim);
	collect(ap, a);
	va_end(ap);
	if (im.active) {
		im.creates++;
		record(a, 0);
	}
	if (!real)
		real = (create_ic_fn)dlsym(RTLD_NEXT, "XCreateIC");
	return IM_FORWARD(real, xim, a);
}

/* ------------------------------------------------------------------ */
/* Fixture                                                             */
/* ------------------------------------------------------------------ */

static Widget top, rc, text;

static void pump(void)
{
	Display *dpy = XtDisplay(top);
	int idle = 0;

	XSync(dpy, False);
	while (idle < 3) {
		if (XtAppPending(app)) {
			XtAppProcessEvent(app, XtIMAll);
			idle = 0;
		} else {
			XSync(dpy, False);
			idle++;
		}
	}
}

static void reset_counts(void)
{
	im.set_calls = im.creates = im.spots = 0;
	im.spot.x = im.spot.y = -1;
}

static void setup(void)
{
	/* Xlib's local input method, which needs no server. */
	setenv("XMODIFIERS", "@im=none", 1);
	im.active = 1;
	top = init_xt("check_XmIm");
	rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, top, NULL);
	text = XtVaCreateManagedWidget("text", xmTextWidgetClass, rc,
				       XmNeditMode, XmMULTI_LINE_EDIT,
				       XmNrows, 20, XmNcolumns, 50,
				       XmNvalue, "some text", NULL);
	XtRealizeWidget(top);
	pump();
	ck_assert_msg(XmImGetXIC(text, XmINHERIT_POLICY, NULL, 0) != NULL,
		      "no input context (XMODIFIERS=@im=none)");
	/* As on FocusIn: the spot is relative to the focus window. */
	XmImVaSetFocusValues(text, NULL);
	pump();
	reset_counts();
}

static void teardown(void)
{
	uninit_xt();
}

/* The spot location the input context has. */
static XPoint ic_spot(void)
{
	XIC xic = XmImSetXIC(text, NULL);
	XPoint *ret = NULL, pt;
	XVaNestedList list;

	ck_assert_ptr_nonnull(xic);
	list = XVaCreateNestedList(0, XNSpotLocation, &ret, NULL);
	ck_assert_ptr_null(XGetICValues(xic, XNPreeditAttributes, list, NULL));
	XFree(list);
	ck_assert_ptr_nonnull(ret);
	pt = *ret;
	XFree(ret);
	return pt;
}

static void set_spot(Widget w, short x, short y)
{
	XPoint pt;

	pt.x = x;
	pt.y = y;
	XmImVaSetValues(w, XmNspotLocation, &pt, NULL);
}

#define ck_assert_spot(pt, ex, ey) do { \
	ck_assert_int_eq((pt).x, (ex)); \
	ck_assert_int_eq((pt).y, (ey)); \
} while (0)

/* ------------------------------------------------------------------ */
/* Tests                                                               */
/* ------------------------------------------------------------------ */

/*
 * A value the input method refuses makes XmIm recreate the context;
 * this reused the argument lists after freeing them.
 */
START_TEST(recreate_after_refused_value)
{
	XmImVaSetValues(text, "motifTestBogusValue", (XtPointer)1, NULL);
	ck_assert_uint_eq(im.set_calls, 1);
	ck_assert_uint_ge(im.creates, 1);
	ck_assert_ptr_nonnull(XmImSetXIC(text, NULL));
	pump();
	set_spot(text, 31, 32);
	pump();
	ck_assert_spot(ic_spot(), 31, 32);
}
END_TEST

void xmim_suite(SRunner *runner)
{
	Suite *s = suite_create("XmIm");
	TCase *t;

	t = tcase_create("Input context");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, recreate_after_refused_value);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
