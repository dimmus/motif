/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Tests for the requests that make the client wait for the X server:
 * the ScrollBar autorepeat and XmGetVisibility.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* RTLD_NEXT */
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <Xm/XmP.h>
#include <Xm/BulletinB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ScrollBarP.h>
#include <check.h>

#include "suites.h"

static Widget shell;

/*
 * Count the calls libXm makes to two Xlib functions that wait for a
 * reply.  The definitions here take precedence over libX11's for the
 * calls from libXm, and pass them on.
 */
static int n_sync, n_query_tree;

int XSync(Display *display, Bool discard)
{
	static int (*real)(Display *, Bool);

	if (!real)
		*(void **)&real = dlsym(RTLD_NEXT, "XSync");
	n_sync++;
	return real(display, discard);
}

Status XQueryTree(Display *display, Window w, Window *root, Window *parent, Window **children,
		  unsigned int *n)
{
	static Status (*real)(Display *, Window, Window *, Window *, Window **, unsigned int *);

	if (!real)
		*(void **)&real = dlsym(RTLD_NEXT, "XQueryTree");
	n_query_tree++;
	return real(display, w, root, parent, children, n);
}

static void _init_xt(void)
{
	shell = init_xt("check_RoundTrips");
}

static void settle(void)
{
	int i;

	for (i = 0; i < 3; i++) {
		XSync(XtDisplay(shell), False);
		while (XtAppPending(app) & XtIMXEvent)
			XtAppProcessEvent(app, XtIMXEvent);
	}
}

/*
 * ScrollBar autorepeat: hold the increment arrow down (through the
 * actions, as the translations would) and record every repeat.
 */
static struct {
	int ticks;
	Pixmap pixmap;
	GC gc;
	int work;
	unsigned long last_request;
	unsigned long most_requests; /* requests made in one repeat */
	unsigned long most_ahead;    /* requests the server had not done */
} rep;

static void increment(Widget w, XtPointer client, XtPointer call)
{
	Display *display = XtDisplay(w);
	unsigned long next = NextRequest(display);
	unsigned long ahead = next - 1 - LastKnownRequestProcessed(display);
	int i;

	if (rep.ticks++ > 0 && next - rep.last_request > rep.most_requests)
		rep.most_requests = next - rep.last_request;
	rep.last_request = next;
	if (ahead > rep.most_ahead)
		rep.most_ahead = ahead;
	/* drawing for the server, as a scrolled view would */
	for (i = 0; i < rep.work; i++)
		XCopyArea(display, rep.pixmap, rep.pixmap, rep.gc, i & 1, 0, 511, 512, !(i & 1), 0);
}

static Widget repeat_scroll_bar(int work)
{
	Widget sb;

	memset(&rep, 0, sizeof(rep));
	rep.work = work;
	sb = XtVaCreateManagedWidget("sb", xmScrollBarWidgetClass, shell, XmNorientation,
				     XmVERTICAL, XmNwidth, 20, XmNheight, 200, XmNminimum, 0,
				     XmNmaximum, 1000000, XmNsliderSize, 10, XmNinitialDelay, 20,
				     XmNrepeatDelay, 5, NULL);
	XtAddCallback(sb, XmNincrementCallback, increment, NULL);
	XtRealizeWidget(shell);
	rep.pixmap = XCreatePixmap(XtDisplay(sb), XtWindow(sb), 512, 512,
				   DefaultDepthOfScreen(XtScreen(sb)));
	rep.gc = XCreateGC(XtDisplay(sb), rep.pixmap, 0, NULL);
	settle();
	return sb;
}

static void free_scroll_bar(Widget sb)
{
	XFreeGC(XtDisplay(sb), rep.gc);
	XFreePixmap(XtDisplay(sb), rep.pixmap);
}

static void button(Widget sb, int type)
{
	XmScrollBarWidget sbw = (XmScrollBarWidget)sb;
	XEvent ev;

	memset(&ev, 0, sizeof(ev));
	ev.xbutton.type = type;
	ev.xbutton.display = XtDisplay(sb);
	ev.xbutton.window = XtWindow(sb);
	ev.xbutton.button = Button1;
	ev.xbutton.time = CurrentTime;
	ev.xbutton.x = sbw->scrollBar.arrow2_x + sbw->scrollBar.arrow_width / 2;
	ev.xbutton.y = sbw->scrollBar.arrow2_y + sbw->scrollBar.arrow_height / 2;
	XtCallActionProc(sb, type == ButtonPress ? "Select" : "Release", &ev, NULL, 0);
}

static void repeat(Widget sb, int ticks)
{
	button(sb, ButtonPress);
	while (rep.ticks < ticks)
		XtAppProcessEvent(app, XtIMAll);
	button(sb, ButtonRelease);
}

static void expired(XtPointer closure, XtIntervalId *id)
{
	*(Boolean *)closure = True;
}

static void run_for(unsigned long ms)
{
	Boolean done = False;

	XtAppAddTimeOut(app, ms, expired, &done);
	while (!done)
		XtAppProcessEvent(app, XtIMAll);
}

/* The repeat does not wait for a reply from the server, and stops when
 * the button is released. */
START_TEST(scrollbar_repeat_without_round_trips)
{
	Widget sb = repeat_scroll_bar(0);
	int ticks, value;

	n_sync = 0;
	repeat(sb, 40);
	ck_assert_int_eq(n_sync, 0);
	ticks = rep.ticks;
	run_for(100);
	ck_assert_int_eq(rep.ticks, ticks);
	XtVaGetValues(sb, XmNvalue, &value, NULL);
	ck_assert_int_eq(value, ticks);
	free_scroll_bar(sb);
}
END_TEST

/* With more drawing per repeat than the server can do in the repeat
 * delay, the repeat still waits for the server: it never gets more than
 * two repeats ahead of it, instead of piling up requests that the server
 * would go on drawing after the button is released. */
START_TEST(scrollbar_repeat_keeps_pace_with_server)
{
	Widget sb = repeat_scroll_bar(100);

	repeat(sb, 30);
	ck_assert_uint_gt(rep.most_requests, (unsigned long)rep.work);
	ck_assert_msg(rep.most_ahead <= 2 * rep.most_requests,
		      "%lu requests ahead of the server, %lu per repeat", rep.most_ahead,
		      rep.most_requests);
	free_scroll_bar(sb);
}
END_TEST

/*
 * XmGetVisibility: obscured by the windows of later siblings, in the
 * stacking order of the server.
 */
static Widget board(void)
{
	Widget bb = XtVaCreateManagedWidget("bb", xmBulletinBoardWidgetClass, shell, XmNwidth, 200,
					    XmNheight, 200, XmNmarginWidth, 0, XmNmarginHeight, 0,
					    XmNresizePolicy, XmRESIZE_NONE, NULL);
	return bb;
}

static Widget child(Widget parent, WidgetClass wc, const char *name, int x, int y)
{
	return XtVaCreateManagedWidget(name, wc, parent, XmNx, x, XmNy, y, XmNwidth, 80,
				       XmNheight, 40, XmNrecomputeSize, False, NULL);
}

START_TEST(visibility_widget_siblings)
{
	Widget bb = board();
	Widget a = child(bb, xmPushButtonWidgetClass, "a", 10, 10);
	Widget b = child(bb, xmPushButtonWidgetClass, "b", 10, 10);
	Display *display = XtDisplay(shell);

	XtRealizeWidget(shell);
	settle();
	XRaiseWindow(display, XtWindow(b));
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_FULLY_OBSCURED);
	XRaiseWindow(display, XtWindow(a));
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_UNOBSCURED);
	XRaiseWindow(display, XtWindow(b));
	XtVaSetValues(b, XmNx, 50, NULL);
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_PARTIALLY_OBSCURED);
	XtUnmanageChild(b);
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_UNOBSCURED);
}
END_TEST

/* Windows that are not widgets count too; only the server knows them. */
START_TEST(visibility_other_windows)
{
	Widget bb = board();
	Widget a = child(bb, xmPushButtonWidgetClass, "a", 10, 10);
	Display *display = XtDisplay(shell);
	Window w;

	XtRealizeWidget(shell);
	settle();
	w = XCreateSimpleWindow(display, XtWindow(bb), 20, 10, 40, 40, 0, 0, 0);
	XMapWindow(display, w);
	n_query_tree = 0;
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_PARTIALLY_OBSCURED);
	ck_assert_int_eq(n_query_tree, 1);
	XMoveResizeWindow(display, w, 0, 0, 100, 60);
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_FULLY_OBSCURED);
	XLowerWindow(display, w);
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_UNOBSCURED);
	XRaiseWindow(display, w);
	XUnmapWindow(display, w);
	ck_assert_int_eq(XmGetVisibility(a), XmVISIBILITY_UNOBSCURED);
	XDestroyWindow(display, w);
}
END_TEST

/* A gadget has no window to look up among its siblings': no request. */
START_TEST(visibility_gadget)
{
	Widget bb = board();
	Widget g = child(bb, xmPushButtonGadgetClass, "g", 10, 10);
	Widget b = child(bb, xmPushButtonWidgetClass, "b", 10, 10);
	Display *display = XtDisplay(shell);
	unsigned long next;

	XtRealizeWidget(shell);
	settle();
	XRaiseWindow(display, XtWindow(b));
	n_query_tree = 0;
	next = NextRequest(display);
	ck_assert_int_eq(XmGetVisibility(g), XmVISIBILITY_UNOBSCURED);
	ck_assert_int_eq(n_query_tree, 0);
	ck_assert_uint_eq(NextRequest(display), next);
}
END_TEST

void roundtrips_suite(SRunner *runner)
{
	Suite *s = suite_create("RoundTrips");
	TCase *t;

	t = tcase_create("ScrollBar");
	tcase_add_test(t, scrollbar_repeat_without_round_trips);
	tcase_add_test(t, scrollbar_repeat_keeps_pace_with_server);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Visibility");
	tcase_add_test(t, visibility_widget_siblings);
	tcase_add_test(t, visibility_other_windows);
	tcase_add_test(t, visibility_gadget);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
