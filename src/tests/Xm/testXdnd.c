/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Xdnd between Motif and a raw Xlib peer, which plays the part of a GTK
 * or Qt application: a source that drags onto a Motif drop site, and a
 * target that a Motif drag source drops on.  The peer has a connection
 * of its own and checks every message as it comes.  It runs in the test
 * thread, except while Motif drags: Motif then runs its own event loop,
 * and the peer (which also plays the user, with XTest) a thread of its
 * own.
 *
 * Regressions for:
 * - the operation of an Xdnd drag, which libXm took from all three
 *   operations instead of the action of XdndPosition, so that a copy
 *   became a move on a drop site that allows moves, and which did not
 *   change when the action did;
 * - the drop action of XdndDrop, which libXm left unset (the drop site
 *   saw whatever was on the stack and rejected drops at random);
 * - XdndFinished, which said that a successful drop had failed, carried
 *   no action, and came also after XdndLeave;
 * - the drop site manager, which kept the drop site and the drag
 *   context of an Xdnd drop (destroying the drop site then crashed);
 * - XdndAware, written from a one-byte variable read as a long;
 * - XdndStatus and XdndFinished, which named the source window instead
 *   of the target and went to the window of the last drag the target
 *   application had made itself, if it had made one;
 * - the time of the conversion of XdndSelection, which was not the one
 *   of XdndDrop;
 * - XdndTypeList, which was read although XdndEnter carried the types;
 * - and as the source: XdndEnter, which offered version 3 to a version
 *   5 target; XdndPosition, which asked for no action after the target
 *   had refused one; the drop, which failed when the target took longer
 *   than the selection timeout; and XdndFinished, which always meant
 *   success.
 */
#include <poll.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/DisplayP.h>
#include <Xm/DragDrop.h>
#include <Xm/DrawingA.h>
#ifdef XM_WITH_XTST
#include <X11/extensions/XTest.h>
#endif
#include <check.h>

#include "suites.h"

#define TARGET_DATA "Xdnd target test data"
#define SOURCE_DATA "Xdnd source test data"

static Display *mdpy;		/* the Motif application's connection */
static Display *peer;		/* the Xdnd peer's connection */
static Widget top, site;	/* Motif drop site, at (0,0) in a larger shell */
static Window peer_win;		/* the peer's source or target window */

static Atom XdndAware, XdndEnter, XdndPosition, XdndStatus, XdndLeave;
static Atom XdndDrop, XdndFinished, XdndSelection, XdndTypeList;
static Atom XdndActionCopy, XdndActionMove, XdndActionLink, XdndActionAsk;
static Atom XdndActionPrivate, peer_prop, peer_stamp;

/* What the peer saw */
static struct {
	int status, finished, requests, enter, position, drop, leave;
	int finished_sent;
	long status_l[5], finished_l[5], enter_l[5], position_l[5], drop_l[5];
	Time request_time;
	Time stamp;
	int got_stamp;
	char data[64];
	int got_data;
	Window dummy;		/* a window that must never get a message */
	int dummy_msgs;
} pr;

/* What Motif saw */
static struct {
	int drops, transfers;
	unsigned char op, ops, action, status;
	char data[64];
	int finishes, drag_done;
	unsigned char finish_op, completion;
} mr;

static void intern_atoms(Display *dpy)
{
	XdndAware = XInternAtom(dpy, "XdndAware", False);
	XdndEnter = XInternAtom(dpy, "XdndEnter", False);
	XdndPosition = XInternAtom(dpy, "XdndPosition", False);
	XdndStatus = XInternAtom(dpy, "XdndStatus", False);
	XdndLeave = XInternAtom(dpy, "XdndLeave", False);
	XdndDrop = XInternAtom(dpy, "XdndDrop", False);
	XdndFinished = XInternAtom(dpy, "XdndFinished", False);
	XdndSelection = XInternAtom(dpy, "XdndSelection", False);
	XdndTypeList = XInternAtom(dpy, "XdndTypeList", False);
	XdndActionCopy = XInternAtom(dpy, "XdndActionCopy", False);
	XdndActionMove = XInternAtom(dpy, "XdndActionMove", False);
	XdndActionLink = XInternAtom(dpy, "XdndActionLink", False);
	XdndActionAsk = XInternAtom(dpy, "XdndActionAsk", False);
	XdndActionPrivate = XInternAtom(dpy, "XdndActionPrivate", False);
	peer_prop = XInternAtom(dpy, "XDND_TEST_DATA", False);
	peer_stamp = XInternAtom(dpy, "XDND_TEST_STAMP", False);
}

static long now_ms(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

static void copy_l(long *to, const XClientMessageEvent *ev)
{
	int i;

	for (i = 0; i < 5; i++)
		to[i] = ev->data.l[i];
}

/*
 * The peer as a target: how it answers the source.  status_refuse is
 * the number of XdndPositions it refuses before it accepts.
 */
static struct {
	int status_refuse;
	Atom status_action;
	int finish_accept;
	long finish_delay_ms;
} target_policy;

static long pending_finish_at;		/* when the delayed finish is due */
static Window pending_finish_to;
static Atom pending_finish_action;

static void peer_send(Window to, Atom type, long l0, long l1, long l2, long l3,
		      long l4)
{
	XEvent ev;

	memset(&ev, 0, sizeof ev);
	ev.xclient.type = ClientMessage;
	ev.xclient.window = to;
	ev.xclient.message_type = type;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = l0;
	ev.xclient.data.l[1] = l1;
	ev.xclient.data.l[2] = l2;
	ev.xclient.data.l[3] = l3;
	ev.xclient.data.l[4] = l4;
	XSendEvent(peer, to, False, NoEventMask, &ev);
	XFlush(peer);
}

static void peer_selection_request(XSelectionRequestEvent *req)
{
	XEvent reply;
	Atom property = req->property != None ? req->property : req->target;

	memset(&reply, 0, sizeof reply);
	reply.xselection.type = SelectionNotify;
	reply.xselection.requestor = req->requestor;
	reply.xselection.selection = req->selection;
	reply.xselection.target = req->target;
	reply.xselection.time = req->time;
	reply.xselection.property = None;
	if (req->selection == XdndSelection) {
		pr.requests++;
		pr.request_time = req->time;
		if (req->target == XA_STRING) {
			XChangeProperty(peer, req->requestor, property, XA_STRING, 8,
					PropModeReplace,
					(const unsigned char *)TARGET_DATA,
					(int)strlen(TARGET_DATA));
			reply.xselection.property = property;
		}
	}
	XSendEvent(peer, req->requestor, False, NoEventMask, &reply);
	XFlush(peer);
}

static void peer_finish(Window source, Atom action)
{
	pr.finished_sent++;
	if (target_policy.finish_accept)
		peer_send(source, XdndFinished, (long)peer_win, 1, (long)action, 0, 0);
	else
		peer_send(source, XdndFinished, (long)peer_win, 0, None, 0, 0);
}

static void peer_client_message(XClientMessageEvent *ev)
{
	if (ev->window == pr.dummy && pr.dummy != None) {
		pr.dummy_msgs++;
		return;
	}
	if (ev->message_type == XdndStatus) {
		copy_l(pr.status_l, ev);
		pr.status++;
	} else if (ev->message_type == XdndFinished) {
		copy_l(pr.finished_l, ev);
		pr.finished++;
	} else if (ev->message_type == XdndEnter) {
		copy_l(pr.enter_l, ev);
		pr.enter++;
	} else if (ev->message_type == XdndPosition) {
		long accept;
		Atom action;

		copy_l(pr.position_l, ev);
		pr.position++;
		accept = pr.position > target_policy.status_refuse;
		action = target_policy.status_action != None ?
			 target_policy.status_action : (Atom)ev->data.l[4];
		peer_send((Window)ev->data.l[0], XdndStatus, (long)peer_win,
			  accept ? 3 : 2, 0, 0, accept ? (long)action : None);
	} else if (ev->message_type == XdndLeave) {
		pr.leave++;
	} else if (ev->message_type == XdndDrop) {
		copy_l(pr.drop_l, ev);
		pr.drop++;
		/* the source's time: XdndDrop's */
		XConvertSelection(peer, XdndSelection, XA_STRING, peer_prop, peer_win,
				  (Time)((unsigned long)ev->data.l[2] & 0xffffffffUL));
		XFlush(peer);
	}
}

static void peer_selection_notify(XSelectionEvent *ev)
{
	Atom type;
	int format;
	unsigned long n, after;
	unsigned char *data = NULL;
	Window source = (Window)pr.enter_l[0];
	Atom action = target_policy.status_action != None ?
		      target_policy.status_action : (Atom)pr.position_l[4];

	if (ev->property != None &&
	    XGetWindowProperty(peer, peer_win, ev->property, 0, 64, True,
			       AnyPropertyType, &type, &format, &n, &after,
			       &data) == Success && data) {
		snprintf(pr.data, sizeof pr.data, "%.*s", (int)n, (char *)data);
		pr.got_data = 1;
	}
	if (data)
		XFree(data);
	if (target_policy.finish_delay_ms > 0) {
		pending_finish_at = now_ms() + target_policy.finish_delay_ms;
		pending_finish_to = source;
		pending_finish_action = action;
	} else {
		peer_finish(source, action);
	}
}

static void peer_event(XEvent *ev)
{
	switch (ev->type) {
	case SelectionRequest:
		peer_selection_request(&ev->xselectionrequest);
		break;
	case SelectionNotify:
		peer_selection_notify(&ev->xselection);
		break;
	case ClientMessage:
		peer_client_message(&ev->xclient);
		break;
	case PropertyNotify:
		if (ev->xproperty.atom == peer_stamp) {
			pr.stamp = ev->xproperty.time;
			pr.got_stamp = 1;
		}
		break;
	default:
		break;
	}
}

/*
 * The peer runs in a thread of its own while Motif drags: Motif runs a
 * loop of its own for the drag, in which the test cannot answer for the
 * peer.  Otherwise both ends run in this thread.
 */
static int peer_threaded;

static void peer_events(void)
{
	XEvent ev;

	XFlush(peer);
	while (XPending(peer)) {
		XNextEvent(peer, &ev);
		peer_event(&ev);
	}
	if (pending_finish_at && now_ms() >= pending_finish_at) {
		pending_finish_at = 0;
		peer_finish(pending_finish_to, pending_finish_action);
	}
}

/* Run the peer until cond() holds, or for ms milliseconds. */
static int peer_pump(int (*cond)(void), long ms)
{
	long deadline = now_ms() + ms;
	struct pollfd fd;

	for (;;) {
		peer_events();
		if (cond && cond())
			return 1;
		if (now_ms() >= deadline)
			return 0;
		fd.fd = ConnectionNumber(peer);
		fd.events = POLLIN;
		(void)poll(&fd, 1, 10);
	}
}

/* Run both ends (or Motif only) until cond() holds, or for ms milliseconds. */
static int pump(int (*cond)(void), long ms)
{
	long deadline = now_ms() + ms;
	struct pollfd fds[2];

	for (;;) {
		XFlush(mdpy);
		while (XtAppPending(app))
			XtAppProcessEvent(app, XtIMAll);
		if (!peer_threaded)
			peer_events();
		if (cond && cond())
			return 1;
		if (now_ms() >= deadline)
			return 0;
		fds[0].fd = ConnectionNumber(mdpy);
		fds[0].events = POLLIN;
		fds[1].fd = peer_threaded ? -1 : ConnectionNumber(peer);
		fds[1].events = POLLIN;
		(void)poll(fds, 2, 10);
	}
}

static int got_stamp(void) { return pr.got_stamp; }

/* A server time for the peer, as a real client gets it from an event. */
static Time peer_time(void)
{
	pr.got_stamp = 0;
	XChangeProperty(peer, peer_win, peer_stamp, XA_STRING, 8, PropModeAppend,
			(const unsigned char *)"", 0);
	ck_assert_msg(pump(got_stamp, 5000), "no PropertyNotify for the peer");
	return pr.stamp;
}

/* ------------------------------------------------------------------ */
/* Motif as the target                                                 */
/* ------------------------------------------------------------------ */

static void transfer_proc(Widget w, XtPointer closure, Atom *selection,
			  Atom *type, XtPointer value, unsigned long *length,
			  int *format)
{
	(void)w;
	(void)closure;
	(void)selection;
	(void)format;
	if (*type == XA_STRING && value) {
		snprintf(mr.data, sizeof mr.data, "%.*s", (int)*length,
			 (char *)value);
		mr.transfers++;
	}
	XtFree((char *)value);
}

/* A drop proc that does what CDE's does: refuse anything but a drop. */
static void drop_proc(Widget w, XtPointer client, XtPointer call)
{
	XmDropProcCallbackStruct *cb = (XmDropProcCallbackStruct *)call;
	XmDropTransferEntryRec entry;
	Arg args[4];
	Cardinal n = 0;

	(void)w;
	(void)client;
	mr.drops++;
	mr.op = cb->operation;
	mr.ops = cb->operations;
	mr.action = cb->dropAction;
	mr.status = cb->dropSiteStatus;
	if (cb->dropSiteStatus != XmVALID_DROP_SITE ||
	    cb->operation == XmDROP_NOOP || cb->dropAction != XmDROP) {
		XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
		XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
	} else {
		entry.client_data = NULL;
		entry.target = XA_STRING;
		XtSetArg(args[n], XmNdropTransfers, &entry); n++;
		XtSetArg(args[n], XmNnumDropTransfers, 1); n++;
		XtSetArg(args[n], XmNtransferProc, transfer_proc); n++;
	}
	XmDropTransferStart(cb->dragContext, args, n);
}

static int has_xdnd_aware(void)
{
	Atom type;
	int format;
	unsigned long n, after;
	unsigned char *data = NULL;
	int ok;

	if (!top || !XtIsRealized(top))
		return 0;
	ok = XGetWindowProperty(mdpy, XtWindow(top), XdndAware, 0, 1, False,
				AnyPropertyType, &type, &format, &n, &after,
				&data) == Success && data && n == 1;
	if (data)
		XFree(data);
	return ok;
}

static void set_site_operations(unsigned char ops)
{
	Arg args[1];

	XtSetArg(args[0], XmNdropSiteOperations, ops);
	XmDropSiteUpdate(site, args, 1);
}

static void open_peer(void)
{
	XSetWindowAttributes attr;

	peer = XOpenDisplay(NULL);
	ck_assert_msg(peer != NULL, "the peer cannot open the display");
	intern_atoms(peer);
	memset(&pr, 0, sizeof pr);
	memset(&mr, 0, sizeof mr);
	memset(&target_policy, 0, sizeof target_policy);
	target_policy.finish_accept = 1;
	pending_finish_at = 0;
	attr.override_redirect = True;
	attr.event_mask = PropertyChangeMask;
	peer_win = XCreateWindow(peer, DefaultRootWindow(peer), 400, 40, 200, 200,
				 0, CopyFromParent, InputOutput, CopyFromParent,
				 CWOverrideRedirect | CWEventMask, &attr);
}

static void setup_target(void)
{
	Arg args[6];
	Cardinal n;
	Widget area;
	Atom targets[1] = { XA_STRING };

	top = init_xt("check_Xdnd");
	mdpy = XtDisplay(top);
	n = 0;
	XtSetArg(args[n], XmNx, 30); n++;
	XtSetArg(args[n], XmNy, 30); n++;
	XtSetArg(args[n], XmNwidth, 200); n++;
	XtSetArg(args[n], XmNheight, 200); n++;
	XtSetValues(top, args, n);
	area = XtVaCreateManagedWidget("area", xmDrawingAreaWidgetClass, top,
				       XmNwidth, 200, XmNheight, 200,
				       XmNmarginWidth, 0, XmNmarginHeight, 0,
				       NULL);
	site = XtVaCreateManagedWidget("site", xmDrawingAreaWidgetClass, area,
				       XmNx, 0, XmNy, 0,
				       XmNwidth, 100, XmNheight, 100, NULL);
	n = 0;
	XtSetArg(args[n], XmNimportTargets, targets); n++;
	XtSetArg(args[n], XmNnumImportTargets, 1); n++;
	XtSetArg(args[n], XmNdropSiteOperations,
		 XmDROP_COPY | XmDROP_MOVE | XmDROP_LINK); n++;
	XtSetArg(args[n], XmNdropProc, drop_proc); n++;
	XmDropSiteRegister(site, args, n);
	XtRealizeWidget(top);
	open_peer();
	ck_assert_msg(pump(has_xdnd_aware, 5000), "the shell has no XdndAware");
}

static void teardown(void)
{
	if (peer) {
		XCloseDisplay(peer);
		peer = NULL;
	}
	uninit_xt();
	top = site = NULL;
	mdpy = NULL;
}

/* The root position of (x, y) in the drop site's shell */
static long site_position(int x, int y)
{
	int rx, ry;
	Window child;

	XTranslateCoordinates(mdpy, XtWindow(top), DefaultRootWindow(mdpy), x, y,
			      &rx, &ry, &child);
	return ((long)rx << 16) | ry;
}

static int status_count;
static int finished_count;
static int status_arrived(void) { return pr.status > status_count; }
static int finished_arrived(void) { return pr.finished > finished_count; }

/* XdndEnter with the types in the message, as GTK does for few types */
static void send_enter(Atom type1)
{
	peer_send(XtWindow(top), XdndEnter, (long)peer_win, 5L << 24,
		  (long)type1, None, None);
}

/* XdndPosition; returns whether the drop site accepted the action */
static int send_position(long where, Atom action, Time t)
{
	status_count = pr.status;
	peer_send(XtWindow(top), XdndPosition, (long)peer_win, 0, where, (long)t,
		  (long)action);
	ck_assert_msg(pump(status_arrived, 5000), "no XdndStatus");
	ck_assert_msg((Window)pr.status_l[0] == XtWindow(top),
		      "XdndStatus names window 0x%lx, not the target 0x%lx",
		      (unsigned long)pr.status_l[0], XtWindow(top));
	return (int)(pr.status_l[1] & 1);
}

/* XdndDrop; waits for XdndFinished and checks the time of the transfer */
static void send_drop(void)
{
	Time t = peer_time();

	finished_count = pr.finished;
	pr.requests = 0;
	peer_send(XtWindow(top), XdndDrop, (long)peer_win, 0, (long)t, 0, 0);
	ck_assert_msg(pump(finished_arrived, 10000), "no XdndFinished");
	ck_assert_msg((Window)pr.finished_l[0] == XtWindow(top),
		      "XdndFinished names window 0x%lx, not the target 0x%lx",
		      (unsigned long)pr.finished_l[0], XtWindow(top));
	if (pr.requests)
		ck_assert_msg(pr.request_time == t,
			      "XdndSelection converted at %lu, not at the drop's %lu",
			      (unsigned long)pr.request_time, (unsigned long)t);
}

/*
 * One complete drag of the peer onto the drop site with action; checks
 * that it is a drop of op that succeeds.
 */
static void drag_and_drop(Atom action, unsigned char op, Atom done)
{
	Time t = peer_time();
	int drops = mr.drops, transfers = mr.transfers;

	XSetSelectionOwner(peer, XdndSelection, peer_win, t);
	send_enter(XA_STRING);
	ck_assert_msg(send_position(site_position(50, 50), action, t),
		      "the drop site refused the drag");
	ck_assert_msg((Atom)pr.status_l[4] == done,
		      "XdndStatus accepts action %ld, not %ld",
		      pr.status_l[4], (long)done);
	send_drop();
	ck_assert_int_eq(mr.drops, drops + 1);
	ck_assert_int_eq(mr.action, XmDROP);
	ck_assert_int_eq(mr.status, XmVALID_DROP_SITE);
	ck_assert_int_eq(mr.op, op);
	ck_assert_int_eq(mr.ops, op);
	ck_assert_int_eq(mr.transfers, transfers + 1);
	ck_assert_str_eq(mr.data, TARGET_DATA);
	ck_assert_msg(pr.finished_l[1] & 1, "XdndFinished reports a failure");
	ck_assert_msg((Atom)pr.finished_l[2] == done,
		      "XdndFinished reports action %ld, not %ld",
		      pr.finished_l[2], (long)done);
}

START_TEST(target_xdnd_aware)
{
	Atom type;
	int format;
	unsigned long n, after;
	unsigned char *data = NULL;

	ck_assert_int_eq(XGetWindowProperty(mdpy, XtWindow(top), XdndAware, 0, 4,
					    False, AnyPropertyType, &type,
					    &format, &n, &after, &data), Success);
	ck_assert_ptr_nonnull(data);
	ck_assert_int_eq(type, XA_ATOM);
	ck_assert_int_eq(format, 32);
	ck_assert_int_eq(n, 1);
	ck_assert_int_eq(*(unsigned long *)data, 5);
	XFree(data);
}
END_TEST

START_TEST(target_copy_stays_copy)
{
	drag_and_drop(XdndActionCopy, XmDROP_COPY, XdndActionCopy);
}
END_TEST

START_TEST(target_move_is_move)
{
	drag_and_drop(XdndActionMove, XmDROP_MOVE, XdndActionMove);
}
END_TEST

START_TEST(target_link_is_link)
{
	drag_and_drop(XdndActionLink, XmDROP_LINK, XdndActionLink);
}
END_TEST

START_TEST(target_ask_and_private_are_copy)
{
	drag_and_drop(XdndActionAsk, XmDROP_COPY, XdndActionCopy);
	drag_and_drop(XdndActionPrivate, XmDROP_COPY, XdndActionCopy);
}
END_TEST

/* Every drop is accepted, with its own operation, many times over. */
START_TEST(target_repeated_drops)
{
	static const struct {
		const char *name;
		unsigned char op;
	} actions[] = {
		{ "XdndActionCopy", XmDROP_COPY },
		{ "XdndActionMove", XmDROP_MOVE },
		{ "XdndActionLink", XmDROP_LINK },
	};
	int i;

	for (i = 0; i < 150; i++) {
		Atom a = XInternAtom(peer, actions[i % 3].name, False);

		drag_and_drop(a, actions[i % 3].op, a);
	}
	ck_assert_int_eq(mr.drops, 150);
	ck_assert_int_eq(pr.finished, 150);
}
END_TEST

/* A move onto a site that only copies is refused, and no finish follows */
START_TEST(target_refuses_disallowed_move)
{
	Time t = peer_time();

	set_site_operations(XmDROP_COPY);
	XSetSelectionOwner(peer, XdndSelection, peer_win, t);
	send_enter(XA_STRING);
	ck_assert_msg(!send_position(site_position(50, 50), XdndActionMove, t),
		      "a move onto a site that only copies was accepted");
	ck_assert_int_eq(pr.status_l[4], None);
	ck_assert_msg(send_position(site_position(50, 50), XdndActionCopy, t),
		      "a copy was refused");
	ck_assert_int_eq((Atom)pr.status_l[4], XdndActionCopy);
	peer_send(XtWindow(top), XdndLeave, (long)peer_win, 0, 0, 0, 0);
	pump(NULL, 300);
	ck_assert_int_eq(pr.finished, 0);
	ck_assert_int_eq(mr.drops, 0);
}
END_TEST

/* XdndFinished answers XdndDrop, never XdndLeave */
START_TEST(target_leave_sends_no_finished)
{
	int i;

	for (i = 0; i < 5; i++) {
		Time t = peer_time();

		XSetSelectionOwner(peer, XdndSelection, peer_win, t);
		send_enter(XA_STRING);
		ck_assert(send_position(site_position(50, 50), XdndActionCopy, t));
		peer_send(XtWindow(top), XdndLeave, (long)peer_win, 0, 0, 0, 0);
		pump(NULL, 200);
	}
	ck_assert_int_eq(pr.finished, 0);
	/* and the next drag still works */
	drag_and_drop(XdndActionCopy, XmDROP_COPY, XdndActionCopy);
	ck_assert_int_eq(pr.finished, 1);
}
END_TEST

/* A drop outside the drop site fails, and says so once */
START_TEST(target_drop_outside_site)
{
	Time t = peer_time();

	XSetSelectionOwner(peer, XdndSelection, peer_win, t);
	send_enter(XA_STRING);
	ck_assert(!send_position(site_position(150, 150), XdndActionCopy, t));
	ck_assert_int_eq(pr.status_l[4], None);
	send_drop();
	pump(NULL, 300);
	ck_assert_int_eq(pr.finished, 1);
	ck_assert_int_eq(pr.finished_l[1] & 1, 0);
	ck_assert_int_eq(pr.finished_l[2], None);
	ck_assert_int_eq(mr.drops, 0);
}
END_TEST

/*
 * The replies go to the source, not to the window of the last drag the
 * Motif application made itself (the display's proxy window).
 */
START_TEST(target_replies_to_the_source)
{
	XmDisplay xmd = (XmDisplay)XmGetXmDisplay(mdpy);

	pr.dummy = XCreateSimpleWindow(peer, DefaultRootWindow(peer), 0, 0, 1, 1,
				       0, 0, 0);
	XSync(peer, False);
	xmd->display.proxyWindow = pr.dummy;
	drag_and_drop(XdndActionCopy, XmDROP_COPY, XdndActionCopy);
	ck_assert_int_eq(pr.dummy_msgs, 0);
}
END_TEST

/* More than three types: XdndEnter says so, and XdndTypeList has them */
START_TEST(target_type_list)
{
	Atom types[5];
	Time t = peer_time();

	types[0] = XInternAtom(peer, "text/uri-list", False);
	types[1] = XInternAtom(peer, "text/plain;charset=utf-8", False);
	types[2] = XInternAtom(peer, "UTF8_STRING", False);
	types[3] = XInternAtom(peer, "text/plain", False);
	types[4] = XA_STRING;
	XChangeProperty(peer, peer_win, XdndTypeList, XA_ATOM, 32,
			PropModeReplace, (const unsigned char *)types, 5);
	XSetSelectionOwner(peer, XdndSelection, peer_win, t);
	peer_send(XtWindow(top), XdndEnter, (long)peer_win, (5L << 24) | 1,
		  (long)types[0], (long)types[1], (long)types[2]);
	ck_assert(send_position(site_position(50, 50), XdndActionCopy, t));
	send_drop();
	ck_assert_int_eq(mr.transfers, 1);
	ck_assert_msg(pr.finished_l[1] & 1, "XdndFinished reports a failure");
}
END_TEST

/* XdndEnter's own types win over an XdndTypeList left from an earlier drag */
START_TEST(target_stale_type_list)
{
	Atom stale = XInternAtom(peer, "text/uri-list", False);

	XChangeProperty(peer, peer_win, XdndTypeList, XA_ATOM, 32,
			PropModeReplace, (const unsigned char *)&stale, 1);
	drag_and_drop(XdndActionCopy, XmDROP_COPY, XdndActionCopy);
}
END_TEST

/* ------------------------------------------------------------------ */
/* Motif as the source                                                 */
/* ------------------------------------------------------------------ */

#ifdef XM_WITH_XTST
static Widget src_shell, src;
static unsigned char drag_operations;

static Boolean convert_proc(Widget w, Atom *selection, Atom *target,
			    Atom *type, XtPointer *value, unsigned long *length,
			    int *format)
{
	(void)w;
	(void)selection;
	if (*target != XA_STRING)
		return False;
	*value = XtNewString(SOURCE_DATA);
	*length = strlen(SOURCE_DATA);
	*type = XA_STRING;
	*format = 8;
	return True;
}

static void drop_finish(Widget w, XtPointer client, XtPointer call)
{
	XmDropFinishCallbackStruct *cb = (XmDropFinishCallbackStruct *)call;

	(void)w;
	(void)client;
	mr.finishes++;
	mr.finish_op = cb->operation;
	mr.completion = cb->completionStatus;
}

static void drag_drop_finish(Widget w, XtPointer client, XtPointer call)
{
	(void)w;
	(void)client;
	(void)call;
	mr.drag_done++;
}

static void start_drag(Widget w, XtPointer client, XEvent *event,
		       Boolean *cont)
{
	static XtCallbackRec finish_cb[] = { { drop_finish, NULL }, { NULL, NULL } };
	static XtCallbackRec done_cb[] = { { drag_drop_finish, NULL }, { NULL, NULL } };
	static Atom targets[1];
	Arg args[8];
	Cardinal n = 0;

	(void)client;
	(void)cont;
	if (event->type != ButtonPress)
		return;
	targets[0] = XA_STRING;
	XtSetArg(args[n], XmNexportTargets, targets); n++;
	XtSetArg(args[n], XmNnumExportTargets, 1); n++;
	XtSetArg(args[n], XmNdragOperations, drag_operations); n++;
	XtSetArg(args[n], XmNconvertProc, convert_proc); n++;
	XtSetArg(args[n], XmNdropFinishCallback, finish_cb); n++;
	XtSetArg(args[n], XmNdragDropFinishCallback, done_cb); n++;
	(void)XmDragStart(w, event, args, n);
}

static int viewable(void)
{
	XWindowAttributes a;

	return XtIsRealized(src_shell) &&
	       XGetWindowAttributes(mdpy, XtWindow(src_shell), &a) &&
	       a.map_state == IsViewable &&
	       XGetWindowAttributes(peer, peer_win, &a) &&
	       a.map_state == IsViewable;
}

static void setup_source(void)
{
	Arg args[4];
	Cardinal n = 0;
	int major, minor, ev, err;
	long version = 5;

	/* the peer has a thread of its own (libX11 before 1.8 needs this) */
	XInitThreads();
	src_shell = init_xt("check_XdndSource");
	mdpy = XtDisplay(src_shell);
	XtSetArg(args[n], XmNx, 30); n++;
	XtSetArg(args[n], XmNy, 30); n++;
	XtSetArg(args[n], XmNwidth, 100); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
	XtSetValues(src_shell, args, n);
	src = XtVaCreateManagedWidget("src", xmDrawingAreaWidgetClass, src_shell,
				      XmNwidth, 100, XmNheight, 100, NULL);
	XtAddEventHandler(src, ButtonPressMask, False, start_drag, NULL);
	XtRealizeWidget(src_shell);
	open_peer();
	if (!XTestQueryExtension(peer, &ev, &err, &major, &minor)) {
		/* without XTest there is no real drag to make */
		XCloseDisplay(peer);
		peer = NULL;
		return;
	}
	/* a test that failed in the middle of a drag may have left it down */
	XTestFakeButtonEvent(peer, 1, False, CurrentTime);
	XChangeProperty(peer, peer_win, XdndAware, XA_ATOM, 32, PropModeReplace,
			(const unsigned char *)&version, 1);
	XMapRaised(peer, peer_win);
	drag_operations = XmDROP_COPY;
	ck_assert_msg(pump(viewable, 5000), "the windows are not mapped");
}

static void teardown_source(void)
{
	if (peer) {
		XTestFakeButtonEvent(peer, 1, False, CurrentTime);
		XSync(peer, False);
	}
	teardown();
	src_shell = src = NULL;
}

static int peer_positions;
static int enter_seen(void) { return pr.enter > 0; }
static int more_positions(void) { return pr.position > peer_positions; }
static int finished_sent(void) { return pr.finished_sent > 0 && !pending_finish_at; }

static void move_to(int x, int y)
{
	XTestFakeMotionEvent(peer, -1, x, y, CurrentTime);
	XFlush(peer);
}

static char peer_error[128];
static atomic_int peer_done;

#define PEER_FAIL(msg) do { snprintf(peer_error, sizeof peer_error, "%s", msg); goto out; } while (0)

/*
 * The user, and the peer as the target: press the button over the
 * Motif source, move into the peer's window, release.
 */
static void *peer_main(void *arg)
{
	int i;

	(void)arg;
	move_to(80, 80);
	peer_pump(NULL, 100);
	XTestFakeButtonEvent(peer, 1, True, CurrentTime);
	XFlush(peer);
	/* Motif starts the drag when it gets the press */
	for (i = 0; i < 50 && !pr.enter; i++) {
		move_to(450 + i % 2, 100);
		peer_pump(enter_seen, 100);
	}
	if (!pr.enter)
		PEER_FAIL("no XdndEnter");
	for (i = 0; i < 5; i++) {
		peer_positions = pr.position;
		move_to(460 + i * 10, 110 + i * 5);
		if (!peer_pump(more_positions, 5000))
			PEER_FAIL("no XdndPosition");
	}
	XTestFakeButtonEvent(peer, 1, False, CurrentTime);
	XFlush(peer);
	if (!peer_pump(finished_sent, 20000) && !pr.leave)
		PEER_FAIL("no XdndDrop, or no data to finish it");
out:
	XFlush(peer);
	atomic_store(&peer_done, 1);
	return NULL;
}

static int drag_over(void) { return mr.drag_done > 0 && atomic_load(&peer_done); }

/* Drag from the Motif source onto the peer and drop. */
static void motif_drag(void)
{
	pthread_t thread;

	peer_threaded = 1;
	atomic_store(&peer_done, 0);
	peer_error[0] = 0;
	ck_assert_int_eq(pthread_create(&thread, NULL, peer_main, NULL), 0);
	(void)pump(drag_over, 40000);
	ck_assert_msg(atomic_load(&peer_done), "the peer is stuck");
	pthread_join(thread, NULL);
	peer_threaded = 0;
	ck_assert_msg(!peer_error[0], "%s", peer_error);
	ck_assert_msg(mr.drag_done, "the drag did not finish");
}

static void check_enter(long version)
{
	Atom type;
	int format;
	unsigned long n, after;
	unsigned char *data = NULL;
	Window source = (Window)pr.enter_l[0];

	ck_assert_int_eq(pr.enter, 1);
	ck_assert_int_eq((pr.enter_l[1] >> 24) & 0xff, version);
	/* the types are in XdndTypeList */
	ck_assert_int_eq(pr.enter_l[1] & 1, 1);
	ck_assert_int_eq(XGetWindowProperty(peer, source, XdndTypeList, 0, 16,
					    False, XA_ATOM, &type, &format, &n,
					    &after, &data), Success);
	ck_assert_ptr_nonnull(data);
	ck_assert_int_eq(n, 1);
	ck_assert_int_eq(((unsigned long *)data)[0], XA_STRING);
	XFree(data);
}

static void check_drop(void)
{
	ck_assert_int_eq(pr.drop, 1);
	ck_assert_int_eq(pr.leave, 0);
	ck_assert_int_eq(pr.drop_l[0], pr.enter_l[0]);
	ck_assert_int_ne(pr.drop_l[2], 0);
	ck_assert_msg(pr.got_data, "the drop's data was not converted");
	ck_assert_str_eq(pr.data, SOURCE_DATA);
	ck_assert_int_eq(mr.finishes, 1);
}

#define NEED_PEER() do { if (!peer) return; } while (0)

START_TEST(source_copy)
{
	NEED_PEER();
	motif_drag();
	check_enter(5);
	ck_assert_int_eq((Atom)pr.position_l[4], XdndActionCopy);
	check_drop();
	ck_assert_int_eq(mr.completion, XmDROP_SUCCESS);
	ck_assert_int_eq(mr.finish_op, XmDROP_COPY);
}
END_TEST

START_TEST(source_move)
{
	NEED_PEER();
	drag_operations = XmDROP_MOVE | XmDROP_COPY;
	motif_drag();
	ck_assert_int_eq((Atom)pr.position_l[4], XdndActionMove);
	check_drop();
	ck_assert_int_eq(mr.completion, XmDROP_SUCCESS);
	ck_assert_int_eq(mr.finish_op, XmDROP_MOVE);
}
END_TEST

/* The target does a copy of the move it was asked for */
START_TEST(source_target_copies)
{
	NEED_PEER();
	drag_operations = XmDROP_MOVE | XmDROP_COPY;
	target_policy.status_action = XdndActionCopy;
	motif_drag();
	/* the source keeps asking for its own operation */
	ck_assert_int_eq((Atom)pr.position_l[4], XdndActionMove);
	check_drop();
	ck_assert_int_eq(mr.completion, XmDROP_SUCCESS);
	ck_assert_int_eq(mr.finish_op, XmDROP_COPY);
}
END_TEST

/* After a refusal the source still asks for its operation */
START_TEST(source_refused_then_accepted)
{
	NEED_PEER();
	target_policy.status_refuse = 2;
	motif_drag();
	ck_assert_int_gt(pr.position, 2);
	ck_assert_int_eq((Atom)pr.position_l[4], XdndActionCopy);
	check_drop();
	ck_assert_int_eq(mr.completion, XmDROP_SUCCESS);
}
END_TEST

/* A version 5 target that fails the drop */
START_TEST(source_target_fails)
{
	NEED_PEER();
	target_policy.finish_accept = 0;
	motif_drag();
	check_drop();
	ck_assert_int_eq(mr.completion, XmDROP_FAILURE);
}
END_TEST

/* A target slower than the selection timeout */
START_TEST(source_slow_target)
{
	NEED_PEER();
	XtAppSetSelectionTimeout(app, 300);
	target_policy.finish_delay_ms = 1000;
	motif_drag();
	check_drop();
	ck_assert_int_eq(mr.completion, XmDROP_SUCCESS);
}
END_TEST

/* A version 3 target: XdndEnter says 3, and XdndFinished has no status */
START_TEST(source_version_3_target)
{
	long version = 3;

	NEED_PEER();
	XChangeProperty(peer, peer_win, XdndAware, XA_ATOM, 32, PropModeReplace,
			(const unsigned char *)&version, 1);
	target_policy.finish_accept = 0;	/* sends l[1] = 0 */
	motif_drag();
	check_enter(3);
	check_drop();
	ck_assert_int_eq(mr.completion, XmDROP_SUCCESS);
}
END_TEST
#endif /* XM_WITH_XTST */

void xdnd_suite(SRunner *runner)
{
	Suite *s = suite_create("Xdnd");
	TCase *t = tcase_create("Motif target");

	tcase_add_checked_fixture(t, setup_target, teardown);
	tcase_add_test(t, target_xdnd_aware);
	tcase_add_test(t, target_copy_stays_copy);
	tcase_add_test(t, target_move_is_move);
	tcase_add_test(t, target_link_is_link);
	tcase_add_test(t, target_ask_and_private_are_copy);
	tcase_add_test(t, target_repeated_drops);
	tcase_add_test(t, target_refuses_disallowed_move);
	tcase_add_test(t, target_leave_sends_no_finished);
	tcase_add_test(t, target_drop_outside_site);
	tcase_add_test(t, target_replies_to_the_source);
	tcase_add_test(t, target_type_list);
	tcase_add_test(t, target_stale_type_list);
	tcase_set_timeout(t, 120);
	suite_add_tcase(s, t);
#ifdef XM_WITH_XTST
	t = tcase_create("Motif source");
	tcase_add_checked_fixture(t, setup_source, teardown_source);
	tcase_add_test(t, source_copy);
	tcase_add_test(t, source_move);
	tcase_add_test(t, source_target_copies);
	tcase_add_test(t, source_refused_then_accepted);
	tcase_add_test(t, source_target_fails);
	tcase_add_test(t, source_slow_target);
	tcase_add_test(t, source_version_3_target);
	tcase_set_timeout(t, 120);
	suite_add_tcase(s, t);
#endif
	srunner_add_suite(runner, s);
}
