/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmIm: what reaches the input context.  The suite interposes the Xlib
 * input method calls XmIm makes (XSetICValues, XCreateIC, XGetIMValues)
 * and XFilterEvent, to count the XSetICValues requests (each one is a
 * round trip with an input method server) and to see the spot location
 * and area they carry.  They forward to Xlib's local input method
 * (XMODIFIERS=@im=none), except in the "over the spot" cases: the local
 * input method has no XIMPreeditPosition style, which is the one that
 * takes an area, so there XGetIMValues reports that style, XCreateIC
 * creates a plain context and XSetICValues only records its arguments.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
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
	int fake;		/* over the spot, see above */
	unsigned long set_calls;	/* XSetICValues calls */
	unsigned long creates;	/* XCreateIC calls */
	unsigned long spots;	/* XNSpotLocation values passed */
	unsigned long areas;	/* preedit XNArea values passed */
	XPoint spot;		/* the last of them */
	XRectangle area;
	long spots_at_key;	/* spots when a KeyPress was filtered */
} im = { .spots_at_key = -1 };

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
		} else if (preedit && !strcmp(a->name, XNArea)) {
			im.area = *(XRectangle *)a->value;
			im.areas++;
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
typedef char *(*get_im_fn)(XIM, ...);
typedef XIC (*create_ic_fn)(XIM, ...);
typedef Bool (*filter_fn)(XEvent *, Window);

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
	if (im.fake)
		return NULL;
	if (!real)
		real = (set_ic_fn)dlsym(RTLD_NEXT, "XSetICValues");
	return IM_FORWARD(real, ic, a);
}

char *XGetIMValues(XIM xim, ...)
{
	static get_im_fn real;
	ImArg a[IM_MAX_ARGS + 1] = { { NULL, NULL } };
	va_list ap;
	char *ret;
	int i;

	va_start(ap, xim);
	collect(ap, a);
	va_end(ap);
	if (!real)
		real = (get_im_fn)dlsym(RTLD_NEXT, "XGetIMValues");
	ret = IM_FORWARD(real, xim, a);
	for (i = 0; im.fake && !ret && a[i].name; i++) {
		if (!strcmp(a[i].name, XNQueryInputStyle)) {
			/* One block, as Xlib allocates it: XmIm XFrees it. */
			XIMStyles *styles = malloc(sizeof(XIMStyles) +
						   sizeof(XIMStyle));

			ck_assert_ptr_nonnull(styles);
			styles->count_styles = 1;
			styles->supported_styles = (XIMStyle *)(styles + 1);
			styles->supported_styles[0] =
				XIMPreeditPosition | XIMStatusNothing;
			XFree(*(XIMStyles **)a[i].value);
			*(XIMStyles **)a[i].value = styles;
		}
	}
	return ret;
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
	if (im.fake)
		return real(xim, XNInputStyle,
			    (XIMStyle)(XIMPreeditNothing | XIMStatusNothing),
			    NULL);
	return IM_FORWARD(real, xim, a);
}

Bool XFilterEvent(XEvent *event, Window window)
{
	static filter_fn real;

	if (!real)
		real = (filter_fn)dlsym(RTLD_NEXT, "XFilterEvent");
	if (im.active && event->type == KeyPress && im.spots_at_key < 0)
		im.spots_at_key = (long)im.spots;
	return real(event, window);
}

/* ------------------------------------------------------------------ */
/* Fixture                                                             */
/* ------------------------------------------------------------------ */

static Widget top, rc, text, text2;

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
	im.set_calls = im.creates = im.spots = im.areas = 0;
	im.spot.x = im.spot.y = -1;
	im.spots_at_key = -1;
}

static void setup_im(int fake)
{
	char value[2000];
	size_t i;

	/* Xlib's local input method, which needs no server. */
	setenv("XMODIFIERS", "@im=none", 1);
	im.fake = fake;
	im.active = 1;
	top = init_xt("check_XmIm");
	rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, top, NULL);
	for (i = 0; i < sizeof value - 1; i++)
		value[i] = (i % 40 == 39) ? '\n' : 'a' + i % 26;
	value[sizeof value - 1] = '\0';
	text = XtVaCreateManagedWidget("text", xmTextWidgetClass, rc,
				       XmNeditMode, XmMULTI_LINE_EDIT,
				       XmNrows, 20, XmNcolumns, 50,
				       XmNvalue, value, NULL);
	text2 = XtVaCreateManagedWidget("text2", xmTextWidgetClass, rc,
					XmNvalue, "second", NULL);
	XtRealizeWidget(top);
	pump();
	ck_assert_msg(XmImGetXIC(text, XmINHERIT_POLICY, NULL, 0) != NULL,
		      "no input context (XMODIFIERS=@im=none)");
	/* As on FocusIn: the spot is relative to the focus window. */
	XmImVaSetFocusValues(text, NULL);
	pump();
	reset_counts();
}

static void setup(void)
{
	setup_im(0);
}

static void setup_fake(void)
{
	setup_im(1);
}

static void teardown(void)
{
	uninit_xt();
	/* Without fork (CK_FORK=no) the next suites share this process. */
	im.active = im.fake = 0;
	unsetenv("XMODIFIERS");
}

static XPoint spot_of(XmTextPosition pos)
{
	Position x, y;
	XPoint pt;

	ck_assert(XmTextPosToXY(text, pos, &x, &y));
	pt.x = x;
	pt.y = y;
	return pt;
}

/* The spot location the (real, local) input context has. */
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

/* Positions in the visible lines, all but the last one different. */
#define CURSOR_POS(i) (1 + (i) % 500)

#define ck_assert_spot(pt, ex, ey) do { \
	ck_assert_int_eq((pt).x, (ex)); \
	ck_assert_int_eq((pt).y, (ey)); \
} while (0)

/* ------------------------------------------------------------------ */
/* Tests                                                               */
/* ------------------------------------------------------------------ */

/* 1000 cursor moves in one event are one XSetICValues, with the last spot. */
START_TEST(spot_cursor_moves_coalesced)
{
	XPoint last = spot_of(CURSOR_POS(999));
	int i;

	for (i = 0; i < 1000; i++)
		XmTextSetInsertionPosition(text, CURSOR_POS(i));
	ck_assert_uint_eq(im.set_calls, 0);
	pump();
	ck_assert_uint_eq(im.set_calls, 1);
	ck_assert_uint_eq(im.spots, 1);
	ck_assert_spot(im.spot, last.x, last.y);
	ck_assert_spot(ic_spot(), last.x, last.y);
}
END_TEST

START_TEST(spot_values_coalesced)
{
	int i;

	for (i = 0; i < 1000; i++)
		set_spot(text, 5 + i, 7 + i / 2);
	ck_assert_uint_eq(im.set_calls, 0);
	pump();
	ck_assert_uint_eq(im.set_calls, 1);
	ck_assert_spot(im.spot, 5 + 999, 7 + 999 / 2);
	ck_assert_spot(ic_spot(), 5 + 999, 7 + 999 / 2);

	/* Unchanged, and back to the spot the context has: nothing sent. */
	set_spot(text, 5 + 999, 7 + 999 / 2);
	set_spot(text, 1, 1);
	set_spot(text, 5 + 999, 7 + 999 / 2);
	pump();
	ck_assert_uint_eq(im.set_calls, 1);
}
END_TEST

/* A key event is filtered by the input method after the pending spot. */
START_TEST(spot_sent_before_next_key)
{
	Display *dpy = XtDisplay(text);
	XEvent ev;
	int i;

	for (i = 0; i < 2; i++) {
		memset(&ev, 0, sizeof ev);
		ev.xkey.type = KeyPress;
		ev.xkey.display = dpy;
		ev.xkey.window = XtWindow(text);
		ev.xkey.root = DefaultRootWindow(dpy);
		ev.xkey.time = CurrentTime;
		ev.xkey.same_screen = True;
		ev.xkey.keycode = XKeysymToKeycode(dpy, XK_b);
		reset_counts();
		set_spot(text, 30 + i, 40);
		XPutBackEvent(dpy, &ev);
		if (i == 0) {
			/* XtAppMainLoop */
			while (im.spots_at_key < 0)
				XtAppProcessEvent(app, XtIMAll);
		} else {
			/* XtAppNextEvent and XtDispatchEvent */
			XEvent next;

			while (im.spots_at_key < 0) {
				XtAppNextEvent(app, &next);
				XtDispatchEvent(&next);
			}
		}
		ck_assert_int_eq(im.spots_at_key, 1);
		pump();
	}
}
END_TEST

/* Values other than the spot are sent at once, with the pending spot. */
START_TEST(spot_sent_with_other_values)
{
	Pixel fg;

	XtVaGetValues(text, XmNforeground, &fg, NULL);
	set_spot(text, 11, 12);
	ck_assert_uint_eq(im.set_calls, 0);
	XmImVaSetValues(text, XmNforeground, fg, NULL);
	ck_assert_uint_eq(im.set_calls, 1);
	ck_assert_uint_eq(im.spots, 1);
	ck_assert_spot(im.spot, 11, 12);
	pump();
	ck_assert_uint_eq(im.set_calls, 1);
	ck_assert_spot(ic_spot(), 11, 12);
}
END_TEST

START_TEST(spot_flushed_on_focus)
{
	/* Focus in on the same widget */
	set_spot(text, 13, 14);
	XmImVaSetFocusValues(text, NULL);
	ck_assert_uint_eq(im.spots, 1);
	ck_assert_spot(im.spot, 13, 14);

	/* Focus out */
	set_spot(text, 15, 16);
	XmImUnsetFocus(text);
	ck_assert_uint_eq(im.spots, 2);
	ck_assert_spot(im.spot, 15, 16);
	pump();
	ck_assert_uint_eq(im.spots, 2);
	ck_assert_spot(ic_spot(), 15, 16);
}
END_TEST

/* The shared context moves to another widget: the old spot is dropped. */
START_TEST(spot_focus_window_change)
{
	XPoint pt = { 17, 18 };

	set_spot(text, 99, 99);
	XmImVaSetFocusValues(text2, XmNspotLocation, &pt, NULL);
	ck_assert_uint_eq(im.spots, 1);
	ck_assert_spot(im.spot, 17, 18);
	pump();
	ck_assert_uint_eq(im.spots, 1);

	/* The same coordinates, relative to the first widget, are sent. */
	XmImVaSetFocusValues(text, XmNspotLocation, &pt, NULL);
	ck_assert_uint_eq(im.spots, 2);
}
END_TEST

START_TEST(spot_flushed_for_xic_users)
{
	set_spot(text, 19, 20);
	ck_assert_ptr_nonnull(XmImGetXIC(text, XmINHERIT_POLICY, NULL, 0));
	ck_assert_uint_eq(im.spots, 1);
	set_spot(text, 21, 22);
	ck_assert_ptr_nonnull(XmImSetXIC(text, NULL));
	ck_assert_uint_eq(im.spots, 2);
	ck_assert_spot(im.spot, 21, 22);
}
END_TEST

/* A pending spot does not outlive the widget or its window. */
START_TEST(spot_with_widget_destroyed)
{
	/*
	 * The context is shared with "text" and outlives "text2": it gets
	 * the spot while the focus window still exists.
	 */
	XmImVaSetFocusValues(text2, NULL);
	reset_counts();
	set_spot(text2, 23, 24);
	ck_assert_uint_eq(im.set_calls, 0);
	XtDestroyWidget(text2);
	text2 = NULL;
	ck_assert_uint_eq(im.spots, 1);
	ck_assert_spot(im.spot, 23, 24);
	pump();
	ck_assert_uint_eq(im.spots, 1);

	/* The focus window is destroyed: the spot is dropped. */
	XmImVaSetFocusValues(text, NULL);
	reset_counts();
	set_spot(text, 25, 26);
	ck_assert_uint_eq(im.set_calls, 0);
	XtUnrealizeWidget(top);
	pump();
	ck_assert_uint_eq(im.spots, 0);

	/* With the last reference, the context is destroyed. */
	XtRealizeWidget(top);
	pump();
	XmImVaSetFocusValues(text, NULL);
	reset_counts();
	set_spot(text, 27, 28);
	ck_assert_uint_eq(im.set_calls, 0);
	XtDestroyWidget(text);
	text = NULL;
	pump();
	ck_assert_uint_eq(im.set_calls, 0);
}
END_TEST

/* Closing the input method frees the contexts: no timeout is left. */
START_TEST(spot_with_xim_closed)
{
	set_spot(text, 33, 34);
	ck_assert_uint_eq(im.set_calls, 0);
	XmImCloseXIM(text);
	/* The shared context had another reference: it got the spot first. */
	ck_assert_uint_le(im.spots, 1);
	reset_counts();
	pump();
	ck_assert_uint_eq(im.set_calls, 0);
	ck_assert_ptr_null(XmImSetXIC(text, NULL));
}
END_TEST

/*
 * A value the input method refuses makes XmIm recreate the context;
 * this reused the argument lists after freeing them.
 */
START_TEST(recreate_after_refused_value)
{
	set_spot(text, 29, 30);
	XmImVaSetValues(text, "motifTestBogusValue", (XtPointer)1, NULL);
	ck_assert_uint_eq(im.set_calls, 1);
	ck_assert_uint_ge(im.creates, 1);
	ck_assert_spot(im.spot, 29, 30);
	ck_assert_ptr_nonnull(XmImSetXIC(text, NULL));
	pump();
	set_spot(text, 31, 32);
	pump();
	ck_assert_spot(ic_spot(), 31, 32);
}
END_TEST

/* Over the spot, Text passes its display area with every spot. */
START_TEST(over_the_spot_coalesced)
{
	XPoint last = spot_of(CURSOR_POS(999));
	XRectangle area;
	int i;

	/* Make the context have the current area. */
	XmTextSetInsertionPosition(text, 0);
	pump();
	ck_assert_uint_ge(im.areas, 1);
	area = im.area;
	reset_counts();

	for (i = 0; i < 1000; i++)
		XmTextSetInsertionPosition(text, CURSOR_POS(i));
	ck_assert_uint_eq(im.set_calls, 0);
	pump();
	ck_assert_uint_eq(im.set_calls, 1);
	ck_assert_uint_eq(im.areas, 0);
	ck_assert_spot(im.spot, last.x, last.y);

	/* A new area is sent at once, with the spot. */
	area.width -= 10;
	last.x += 3;
	XmImVaSetValues(text, XmNspotLocation, &last, XmNarea, &area, NULL);
	ck_assert_uint_eq(im.set_calls, 2);
	ck_assert_uint_eq(im.areas, 1);
	ck_assert_uint_eq(im.area.width, area.width);
	ck_assert_spot(im.spot, last.x, last.y);
	pump();
	ck_assert_uint_eq(im.set_calls, 2);
}
END_TEST

void xmim_suite(SRunner *runner)
{
	Suite *s = suite_create("XmIm");
	TCase *t;

	t = tcase_create("Spot location");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, spot_cursor_moves_coalesced);
	tcase_add_test(t, spot_values_coalesced);
	tcase_add_test(t, spot_sent_before_next_key);
	tcase_add_test(t, spot_sent_with_other_values);
	tcase_add_test(t, spot_flushed_on_focus);
	tcase_add_test(t, spot_focus_window_change);
	tcase_add_test(t, spot_flushed_for_xic_users);
	tcase_add_test(t, spot_with_widget_destroyed);
	tcase_add_test(t, spot_with_xim_closed);
	tcase_add_test(t, recreate_after_refused_value);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Over the spot");
	tcase_add_checked_fixture(t, setup_fake, teardown);
	tcase_add_test(t, over_the_spot_coalesced);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
