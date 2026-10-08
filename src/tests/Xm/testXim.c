/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmIm against a real input method server: every test starts stubxim
 * (src/tests/xim), which speaks the XIM protocol to Xlib, and types into
 * XmTextField and XmText through it.  This runs XmIm's preedit start,
 * draw, caret and done callbacks (on-the-spot), the spot location
 * updates (over-the-spot), the status and preedit areas (off-the-spot),
 * commits, XIC reset, focus and the XIC sharing policies, end to end.
 *
 * The suite runs in $MOTIF_TEST_LOCALE (CTest runs it in the generated
 * ja_JP locales too) or in a UTF-8 locale.  The server commits
 * hiragana as compound text, which Xlib converts to the locale.
 */
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <Xm/XmP.h>
#include <Xm/BulletinB.h>
#include <Xm/VirtKeys.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/XmIm.h>
#include <check.h>

#include "i18n_util.h"
#include "suites.h"

#ifndef STUBXIM_PATH
#  define STUBXIM_PATH "stubxim"
#endif

/* XIM styles, as the server logs them */
#define ON_THE_SPOT (XIMPreeditCallbacks | XIMStatusNothing)
#define OVER_THE_SPOT (XIMPreeditPosition | XIMStatusNothing)
#define OFF_THE_SPOT (XIMPreeditArea | XIMStatusArea)
#define ROOT_STYLE (XIMPreeditNothing | XIMStatusNothing)

/* What stubxim asks for and the IM's separator (XmIm.c) */
#define STATUS_WIDTH 60
#define PREEDIT_WIDTH 200
#define AREA_HEIGHT 20
#define SEPARATOR_HEIGHT 2

static const char *const utf8_locales[] = { "C.UTF-8", "C.utf8", "en_US.UTF-8",
					    "en_US.utf8", NULL };

static Widget top, bb;
static Display *dpy;
static pid_t server_pid;
static int server_in = -1;
static char server_name[32];
static int keys_sent;

/* ---- The server ---- */

static void start_server(void)
{
	int in[2], out[2];
	char line[16];
	ssize_t n = 0;
	struct pollfd pfd;

	snprintf(server_name, sizeof server_name, "stubxim%ld", (long)getpid());
	ck_assert_int_eq(pipe(in), 0);
	ck_assert_int_eq(pipe(out), 0);
	server_pid = fork();
	ck_assert_int_ge(server_pid, 0);
	if (server_pid == 0) {
		dup2(in[0], STDIN_FILENO);
		dup2(out[1], STDOUT_FILENO);
		close(in[0]);
		close(in[1]);
		close(out[0]);
		close(out[1]);
		execl(STUBXIM_PATH, "stubxim", server_name, (char *)NULL);
		_exit(127);
	}
	close(in[0]);
	close(out[1]);
	server_in = in[1];
	fcntl(server_in, F_SETFD, FD_CLOEXEC);

	/* It says "ready" once it owns its selection */
	pfd.fd = out[0];
	pfd.events = POLLIN;
	while (n < 6 && poll(&pfd, 1, 10000) > 0) {
		ssize_t r = read(out[0], line + n, sizeof line - 1 - (size_t)n);

		if (r <= 0)
			break;
		n += r;
	}
	close(out[0]);
	line[n > 0 ? n : 0] = '\0';
	ck_assert_msg(!strncmp(line, "ready", 5), "stubxim (%s) did not start",
		      STUBXIM_PATH);
}

static void stop_server(void)
{
	int status;

	if (server_in >= 0)
		close(server_in); /* end of file: the server exits */
	server_in = -1;
	if (server_pid > 0 && waitpid(server_pid, &status, 0) == server_pid)
		ck_assert_msg(WIFEXITED(status) && WEXITSTATUS(status) == 0,
			      "stubxim failed");
	server_pid = 0;
}

/* ---- Fixture ---- */

static void pump(void)
{
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

static XtErrorMsgHandler default_warning;

/* "fixed" has none of the CJK charsets of the locale: no matter here */
static void warning(String name, String type, String klass, String defaultp,
		    String *params, Cardinal *num_params)
{
	if (strcmp(name, "missingCharsetList") != 0)
		default_warning(name, type, klass, defaultp, params, num_params);
}

/*
 * The text widgets created after this use a font set.  Over and off the
 * spot need one: Xlib requires XNFontSet for them.
 */
static void use_font_set(void)
{
	XtVaSetValues(bb, XtVaTypedArg, XmNtextFontList, XmRString, "fixed:", 7, NULL);
}

static void setup(void)
{
	if (!test_set_locale(utf8_locales))
		ck_abort_msg("no UTF-8 locale for the XIM tests");
	start_server();
	top = init_xt("check_Xim");
	dpy = XtDisplay(top);
	default_warning = XtAppSetWarningMsgHandler(app, warning);
	XtVaSetValues(top, XmNinputMethod, server_name, XmNallowShellResize, True,
		      NULL);
	bb = XmCreateBulletinBoard(top, "bb", NULL, 0);
	XtManageChild(bb);
	keys_sent = 0;
}

static void teardown(void)
{
	/* The widgets and the XIM go first: they talk to the server */
	uninit_xt();
	stop_server();
}

/* ---- The server's log ---- */

/* The root window's _STUBXIM_LOG, to free with free() */
static char *server_log(void)
{
	Atom type;
	int format;
	unsigned long n = 0, after;
	unsigned char *data = NULL;
	char *log;

	if (XGetWindowProperty(dpy, DefaultRootWindow(dpy),
			       XInternAtom(dpy, "_STUBXIM_LOG", False), 0, 1 << 20,
			       False, XA_STRING, &type, &format, &n, &after,
			       &data) != Success)
		n = 0;
	log = malloc(n + 1);
	ck_assert_ptr_nonnull(log);
	if (data)
		memcpy(log, data, n);
	log[n] = '\0';
	if (data)
		XFree(data);
	return log;
}

/* The number of log lines that start with prefix */
static int log_count(const char *prefix)
{
	char *log = server_log(), *p = log;
	size_t len = strlen(prefix);
	int n = 0;

	while (*p) {
		if (!strncmp(p, prefix, len))
			n++;
		if (!(p = strchr(p, '\n')))
			break;
		p++;
	}
	free(log);
	return n;
}

/* Whether the log has this line */
static int log_has(const char *line)
{
	char *log = server_log(), *p = log;
	size_t len = strlen(line);
	int found = 0;

	while (*p && !found) {
		found = !strncmp(p, line, len) && (p[len] == '\n' || !p[len]);
		if (!(p = strchr(p, '\n')))
			break;
		p++;
	}
	free(log);
	return found;
}

/* The last log line that starts with prefix, copied into buf */
static int log_last(const char *prefix, char *buf, size_t size)
{
	char *log = server_log(), *p = log, *last = NULL;
	size_t len = strlen(prefix);

	while (*p) {
		if (!strncmp(p, prefix, len))
			last = p;
		if (!(p = strchr(p, '\n')))
			break;
		p++;
	}
	if (last) {
		size_t l = strcspn(last, "\n");

		snprintf(buf, size, "%.*s", (int)l, last);
	}
	free(log);
	return last != NULL;
}

/* Handle events until the log has n lines starting with prefix */
static void wait_log(const char *prefix, int n)
{
	struct timespec ts = { 0, 2000000 };
	int i;

	for (i = 0; i < 5000; i++) {
		int done = log_count(prefix) >= n;

		/* What the server sent before the line is queued by now */
		pump();
		if (done)
			return;
		nanosleep(&ts, NULL);
	}
	ck_abort_msg("stubxim did not log %d \"%s\" lines", n, prefix);
}

/*
 * A round trip to the server through w's XIC: the server has handled
 * every request the client sent before, XSetICFocus included.
 */
static void im_sync(Widget w)
{
	unsigned long mask = 0;

	ck_assert_ptr_null(XGetICValues(XmImSetXIC(w, NULL), XNFilterEvents, &mask, NULL));
	ck_assert_uint_eq(mask, KeyPressMask);
}

/* ---- Input ---- */

/* A key typed into w: the IM gets it through the client's filter */
static void key(Widget w, KeySym sym)
{
	XEvent ev;

	memset(&ev, 0, sizeof ev);
	ev.xkey.type = KeyPress;
	ev.xkey.display = dpy;
	ev.xkey.window = XtWindow(w);
	ev.xkey.root = DefaultRootWindow(dpy);
	ev.xkey.subwindow = None;
	ev.xkey.time = CurrentTime;
	ev.xkey.same_screen = True;
	ev.xkey.keycode = XKeysymToKeycode(dpy, sym);
	ck_assert_int_ne(ev.xkey.keycode, 0);
	XSendEvent(dpy, XtWindow(w), True, KeyPressMask, &ev);
	ev.xkey.type = KeyRelease;
	XSendEvent(dpy, XtWindow(w), True, KeyReleaseMask, &ev);
	wait_log("key ", ++keys_sent);
}

static void type(Widget w, const char *s)
{
	for (; *s; s++)
		key(w, XStringToKeysym((char[]){ *s, '\0' }));
}

/* Give w the keyboard focus, and so the XIC focus */
static void focus(Widget w)
{
	XSetInputFocus(dpy, XtWindow(top), RevertToParent, CurrentTime);
	pump();
	ck_assert(XmProcessTraversal(w, XmTRAVERSE_CURRENT));
	pump();
}

static Widget text_field(const char *preedit_type, const char *value)
{
	Widget tf;

	XtVaSetValues(top, XmNpreeditType, preedit_type, NULL);
	tf = XmCreateTextField(bb, "tf", NULL, 0);
	XtVaSetValues(tf, XmNcolumns, 30, NULL);
	XtManageChild(tf);
	XtRealizeWidget(top);
	pump();
	XmTextFieldSetString(tf, (char *)value);
	focus(tf);
	return tf;
}

static void assert_tf(Widget tf, const char *expect)
{
	char *s = XmTextFieldGetString(tf);

	ck_assert_str_eq(s, expect);
	XtFree(s);
}

/* expect, given in UTF-8, in the current locale */
static void assert_tf_utf8(Widget tf, const char *expect)
{
	char *s = test_from_utf8(expect);

	ck_assert_ptr_nonnull(s);
	assert_tf(tf, s);
	free(s);
}

static void assert_style(int icid, unsigned long style)
{
	char line[64];

	snprintf(line, sizeof line, "create_ic %d 0x%lx", icid, style);
	if (!log_has(line)) {
		char *log = server_log();

		ck_abort_msg("the server did not log \"%s\":\n%s", line, log);
	}
}

/* ---- Tests ---- */

/* XmIm opens the XIM through XmNinputMethod and finds its styles */
START_TEST(open_im)
{
	Widget tf = text_field("OnTheSpot", "");
	XIC xic;

	ck_assert_ptr_nonnull(XmImGetXIM(tf));
	ck_assert(log_has("connect"));
	ck_assert_int_eq(log_count("open "), 1);
	xic = XmImSetXIC(tf, NULL);
	ck_assert_ptr_nonnull(xic);
	ck_assert_ptr_eq(XIMOfIC(xic), XmImGetXIM(tf));
	assert_style(1, ON_THE_SPOT);
	im_sync(tf);
	ck_assert(log_has("focus 1"));
}
END_TEST

/*
 * On the spot: the preedit is drawn into the text field, at the cursor,
 * the caret moves inside it and the commit replaces it.
 */
START_TEST(on_the_spot_textfield)
{
	Widget tf;

	/* With the default render table, then with a font set */
	if (_i)
		use_font_set();
	tf = text_field("OnTheSpot", "[]");

	XmTextFieldSetInsertionPosition(tf, 1);
	type(tf, "nihongo");
	assert_tf(tf, "[nihongo]");
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), 8);
	/* XmIm's preedit start callback returns -1: no length limit */
	wait_log("start_reply 1 -1", 1);

	key(tf, XK_Left);
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), 7);
	wait_log("caret_reply 1 6", 1);
	key(tf, XK_BackSpace);
	assert_tf(tf, "[nihono]");
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), 6);

	key(tf, XK_Return);
	ck_assert(log_has("commit 1 nihono"));
	assert_tf_utf8(tf, "[\xe3\x81\xab\xe3\x81\xbb\xe3\x81\xae]"); /* [にほの] */
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), 4);
	ck_assert_int_eq(XmTextFieldGetLastPosition(tf), 5);

	/* Escape cancels the preedit */
	type(tf, "ka");
	key(tf, XK_Escape);
	assert_tf_utf8(tf, "[\xe3\x81\xab\xe3\x81\xbb\xe3\x81\xae]");

	/* Keys the IM does not take reach the widget */
	key(tf, XK_BackSpace);
	assert_tf_utf8(tf, "[\xe3\x81\xab\xe3\x81\xbb]");
	ck_assert(log_has("key 1 BackSpace returned [] 0"));
}
END_TEST

static Widget text_widget(const char *preedit_type, const char *value)
{
	Widget t;

	XtVaSetValues(top, XmNpreeditType, preedit_type, NULL);
	t = XmCreateText(bb, "text", NULL, 0);
	XtVaSetValues(t, XmNeditMode, XmMULTI_LINE_EDIT, XmNrows, 4, XmNcolumns, 30,
		      XmNvalue, value, NULL);
	XtManageChild(t);
	XtRealizeWidget(top);
	pump();
	focus(t);
	return t;
}

static void assert_text_utf8(Widget t, const char *expect)
{
	char *s = XmTextGetString(t), *e = test_from_utf8(expect);

	ck_assert_ptr_nonnull(e);
	ck_assert_str_eq(s, e);
	XtFree(s);
	free(e);
}

START_TEST(on_the_spot_text)
{
	Widget t;
	char *s, *expect;

	/* With the default render table, then with a font set */
	if (_i)
		use_font_set();
	XtVaSetValues(top, XmNpreeditType, "OnTheSpot", NULL);
	t = XmCreateText(bb, "text", NULL, 0);
	XtVaSetValues(t, XmNeditMode, XmMULTI_LINE_EDIT, XmNrows, 4, XmNcolumns, 30,
		      XmNvalue, "line one\n", NULL);
	XtManageChild(t);
	XtRealizeWidget(top);
	pump();
	focus(t);
	XmTextSetInsertionPosition(t, 9);
	type(t, "kana");
	s = XmTextGetString(t);
	ck_assert_str_eq(s, "line one\nkana");
	XtFree(s);
	ck_assert_int_eq(XmTextGetInsertionPosition(t), 13);
	key(t, XK_Left);
	key(t, XK_Left);
	ck_assert_int_eq(XmTextGetInsertionPosition(t), 11);
	key(t, XK_Return);
	expect = test_from_utf8("line one\n\xe3\x81\x8b\xe3\x81\xaa"); /* かな */
	ck_assert_ptr_nonnull(expect);
	s = XmTextGetString(t);
	ck_assert_str_eq(s, expect);
	XtFree(s);
	free(expect);
	ck_assert_int_eq(XmTextGetInsertionPosition(t), 11);
}
END_TEST

/*
 * Resetting the XIC (here by moving the cursor) commits the preedit in
 * place, once: the IM returns the preedit string, and the preedit
 * callbacks it then sends to erase the preedit change nothing.
 */
START_TEST(on_the_spot_reset)
{
	Widget tf = text_field("OnTheSpot", "ab");

	XmTextFieldSetInsertionPosition(tf, 1);
	type(tf, "xy");
	assert_tf(tf, "axyb");
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), 3);
	XmTextFieldSetInsertionPosition(tf, 4);
	wait_log("reset_ic 1 xy", 1);
	ck_assert_int_eq(XmImGetXICResetState(tf), XIMInitialState);
	assert_tf(tf, "axyb");
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), 4);
	type(tf, "ku");
	assert_tf(tf, "axybku");
	key(tf, XK_Return);
	assert_tf_utf8(tf, "axyb\xe3\x81\x8f"); /* axybく */
}
END_TEST

START_TEST(on_the_spot_text_reset)
{
	Widget t;

	if (_i)
		use_font_set();
	t = text_widget("OnTheSpot", "ab");
	XmTextSetInsertionPosition(t, 1);
	type(t, "xy");
	assert_text_utf8(t, "axyb");
	XmTextSetInsertionPosition(t, 4);
	wait_log("reset_ic 1 xy", 1);
	assert_text_utf8(t, "axyb");
	ck_assert_int_eq(XmTextGetInsertionPosition(t), 4);
	type(t, "ku");
	assert_text_utf8(t, "axybku");
	key(t, XK_Return);
	assert_text_utf8(t, "axyb\xe3\x81\x8f"); /* axybく */
}
END_TEST

/*
 * With XmPER_SHELL (the default), the text fields of a shell share one
 * XIC, and the preedit moves with the focus.
 */
START_TEST(on_the_spot_shared_xic)
{
	Widget tf1 = text_field("OnTheSpot", "");
	Widget tf2 = XmCreateTextField(bb, "tf2", NULL, 0);

	XtVaSetValues(tf2, XmNy, 50, XmNcolumns, 30, NULL);
	XtManageChild(tf2);
	pump();
	ck_assert_ptr_eq(XmImSetXIC(tf1, NULL), XmImSetXIC(tf2, NULL));
	ck_assert_int_eq(log_count("create_ic "), 1);

	type(tf1, "abc");
	assert_tf(tf1, "abc");
	focus(tf2);
	assert_tf(tf1, "");
	assert_tf(tf2, "abc");
	key(tf2, XK_Return);
	assert_tf_utf8(tf2, "\xe3\x81\x82" "bc"); /* あbc */
	assert_tf(tf1, "");
}
END_TEST

/* With XmPER_WIDGET every text field has an XIC of its own */
START_TEST(per_widget_xic)
{
	Widget tf1, tf2;
	XIC xic1, xic2;

	XtVaSetValues(top, XmNinputPolicy, XmPER_WIDGET, NULL);
	tf1 = text_field("OnTheSpot", "");
	tf2 = XmCreateTextField(bb, "tf2", NULL, 0);
	XtVaSetValues(tf2, XmNy, 50, NULL);
	XtManageChild(tf2);
	pump();
	xic1 = XmImSetXIC(tf1, NULL);
	xic2 = XmImSetXIC(tf2, NULL);
	ck_assert_ptr_nonnull(xic1);
	ck_assert_ptr_nonnull(xic2);
	ck_assert_ptr_ne(xic1, xic2);
	ck_assert_int_eq(log_count("create_ic "), 2);

	type(tf1, "u");
	key(tf1, XK_Return);
	assert_tf_utf8(tf1, "\xe3\x81\x86"); /* う */
	focus(tf2);
	type(tf2, "e");
	key(tf2, XK_Return);
	assert_tf_utf8(tf2, "\xe3\x81\x88"); /* え */
	assert_tf_utf8(tf1, "\xe3\x81\x86");

	XmImFreeXIC(tf2, xic2);
	wait_log("destroy_ic 2", 1);
}
END_TEST

/* Over the spot: the IM is told where the cursor is, once per move */
START_TEST(over_the_spot)
{
	Widget tf;
	Position x, y;
	char line[64], expect[64];
	int spots;

	use_font_set();
	tf = text_field("OverTheSpot", "abcdef");
	assert_style(1, OVER_THE_SPOT);
	XmTextFieldSetInsertionPosition(tf, 3);
	pump();
	ck_assert(XmTextFieldPosToXY(tf, 3, &x, &y));
	ck_assert(log_last("spot 1 ", line, sizeof line));
	snprintf(expect, sizeof expect, "spot 1 %d %d", x, y);
	ck_assert_str_eq(line, expect);

	/* The same spot again is not sent */
	spots = log_count("spot 1 ");
	XmImVaSetValues(tf, XmNspotLocation, &(XPoint){ x, y }, NULL);
	pump();
	ck_assert_int_eq(log_count("spot 1 "), spots);

	XmTextFieldSetInsertionPosition(tf, 6);
	pump();
	ck_assert(XmTextFieldPosToXY(tf, 6, &x, &y));
	ck_assert(log_last("spot 1 ", line, sizeof line));
	snprintf(expect, sizeof expect, "spot 1 %d %d", x, y);
	ck_assert_str_eq(line, expect);
	ck_assert_int_eq(log_count("spot 1 "), spots + 1);

	/* The IM shows the preedit itself; the text field gets the commit */
	type(tf, "ko");
	assert_tf(tf, "abcdef");
	key(tf, XK_Return);
	assert_tf_utf8(tf, "abcdef\xe3\x81\x93"); /* abcdefこ */
}
END_TEST

/*
 * Off the spot: the shell grows by the height the IM asks for, plus a
 * separator, and the status and preedit areas are placed at its bottom.
 */
START_TEST(off_the_spot)
{
	Widget tf;
	Dimension h0, h, w;
	char expect[64];

	use_font_set();
	XtVaSetValues(top, XmNpreeditType, "OffTheSpot", NULL);
	tf = XmCreateTextField(bb, "tf", NULL, 0);
	XtManageChild(tf);
	XtRealizeWidget(top);
	pump();
	focus(tf);
	assert_style(1, OFF_THE_SPOT);
	/* The shell holds its child and, below it, the IM's area */
	h0 = XtHeight(bb) + 2 * XtBorderWidth(bb);
	XtVaGetValues(top, XmNheight, &h, XmNwidth, &w, NULL);
	ck_assert_int_eq(h, h0 + AREA_HEIGHT + SEPARATOR_HEIGHT);
	ck_assert_int_eq(XtY(bb), 0);

	snprintf(expect, sizeof expect, "area 1 status 0 %d %d %d", h - AREA_HEIGHT,
		 STATUS_WIDTH, AREA_HEIGHT);
	ck_assert_msg(log_has(expect), "no \"%s\"", expect);
	snprintf(expect, sizeof expect, "area 1 preedit %d %d %d %d", STATUS_WIDTH,
		 h - AREA_HEIGHT,
		 w - STATUS_WIDTH < PREEDIT_WIDTH ? w - STATUS_WIDTH : PREEDIT_WIDTH,
		 AREA_HEIGHT);
	ck_assert_msg(log_has(expect), "no \"%s\"", expect);

	type(tf, "mi");
	assert_tf(tf, "");
	key(tf, XK_Return);
	assert_tf_utf8(tf, "\xe3\x81\xbf"); /* み */
}
END_TEST

/* Root: the IM shows the preedit in a window of its own */
START_TEST(root_style)
{
	Widget tf = text_field("Root", "");

	assert_style(1, ROOT_STYLE);
	type(tf, "sa");
	assert_tf(tf, "");
	key(tf, XK_Return);
	assert_tf_utf8(tf, "\xe3\x81\x95"); /* さ */
}
END_TEST

/* ---- A widget of the application's own, through the XmIm API ---- */

static struct {
	int start, done, draw, caret;
	char preedit[64];
	int caret_pos;
	XIMCaretDirection direction;
	char committed[64];
} app_im;

static int app_preedit_start(XIC xic, XPointer client_data, XPointer call_data)
{
	(void)xic;
	(void)call_data;
	ck_assert_str_eq(XtName((Widget)client_data), "canvas");
	app_im.start++;
	return -1;
}

static void app_preedit_done(XIC xic, XPointer client_data, XPointer call_data)
{
	(void)xic;
	(void)client_data;
	(void)call_data;
	app_im.done++;
}

/* The preedit is ASCII here: apply the change to app_im.preedit */
static void app_preedit_draw(XIC xic, XPointer client_data, XPointer call_data)
{
	XIMPreeditDrawCallbackStruct *d = (XIMPreeditDrawCallbackStruct *)call_data;
	char *p = app_im.preedit;
	size_t len = strlen(p), n = d->text ? d->text->length : 0;

	(void)xic;
	(void)client_data;
	app_im.draw++;
	ck_assert_int_le(d->chg_first + d->chg_length, (int)len);
	ck_assert_uint_lt(len - (size_t)d->chg_length + n, sizeof app_im.preedit);
	memmove(p + d->chg_first + n, p + d->chg_first + d->chg_length,
		len - (size_t)(d->chg_first + d->chg_length) + 1);
	if (n) {
		ck_assert(!d->text->encoding_is_wchar);
		memcpy(p + d->chg_first, d->text->string.multi_byte, n);
		ck_assert_ptr_nonnull(d->text->feedback);
		ck_assert_int_eq(d->text->feedback[0], XIMUnderline);
	}
	app_im.caret_pos = d->caret;
}

static void app_preedit_caret(XIC xic, XPointer client_data, XPointer call_data)
{
	XIMPreeditCaretCallbackStruct *c = (XIMPreeditCaretCallbackStruct *)call_data;

	(void)xic;
	(void)client_data;
	app_im.caret++;
	app_im.direction = c->direction;
}

static void app_key(Widget w, XtPointer client_data, XEvent *event, Boolean *cont)
{
	char buf[32];
	KeySym sym;
	int status, n;

	(void)client_data;
	(void)cont;
	n = XmImMbLookupString(w, &event->xkey, buf, sizeof buf - 1, &sym, &status);
	if (status == XLookupChars || status == XLookupBoth) {
		buf[n] = '\0';
		strncat(app_im.committed, buf, sizeof app_im.committed - strlen(app_im.committed) - 1);
	}
}

/*
 * XmImRegister, XmImGetXIC with XmPER_SHELL and the preedit callbacks
 * of a widget that is not a text widget.
 */
START_TEST(xmim_api)
{
	XIMCallback cb[4];
	Arg args[4];
	Widget w;
	XIC xic;
	char *expect;

	memset(&app_im, 0, sizeof app_im);
	XtVaSetValues(top, XmNpreeditType, "OnTheSpot", NULL);
	w = XtVaCreateManagedWidget("canvas", coreWidgetClass, bb, XmNwidth, 100,
				    XmNheight, 50, NULL);
	XtAddEventHandler(w, KeyPressMask, False, app_key, NULL);
	XtRealizeWidget(top);
	pump();

	XmImRegister(w, 0);
	cb[0].client_data = (XPointer)w;
	cb[0].callback = (XIMProc)app_preedit_start;
	cb[1].client_data = (XPointer)w;
	cb[1].callback = (XIMProc)app_preedit_done;
	cb[2].client_data = (XPointer)w;
	cb[2].callback = (XIMProc)app_preedit_draw;
	cb[3].client_data = (XPointer)w;
	cb[3].callback = (XIMProc)app_preedit_caret;
	XtSetArg(args[0], XmNpreeditStartCallback, &cb[0]);
	XtSetArg(args[1], XmNpreeditDoneCallback, &cb[1]);
	XtSetArg(args[2], XmNpreeditDrawCallback, &cb[2]);
	XtSetArg(args[3], XmNpreeditCaretCallback, &cb[3]);
	xic = XmImGetXIC(w, XmPER_SHELL, args, 4);
	ck_assert_ptr_nonnull(xic);
	ck_assert_ptr_eq(XmImSetXIC(w, NULL), xic);
	XmImSetFocusValues(w, NULL, 0);
	im_sync(w);
	assert_style(1, ON_THE_SPOT);

	type(w, "ab");
	ck_assert_int_eq(app_im.start, 1);
	ck_assert_int_eq(app_im.draw, 2);
	ck_assert_str_eq(app_im.preedit, "ab");
	ck_assert_int_eq(app_im.caret_pos, 2);
	key(w, XK_Left);
	ck_assert_int_eq(app_im.caret, 1);
	ck_assert_int_eq(app_im.direction, XIMBackwardChar);
	key(w, XK_Return);
	ck_assert_int_eq(app_im.done, 1);
	ck_assert_str_eq(app_im.preedit, "");
	expect = test_from_utf8("\xe3\x81\x82" "b"); /* あb */
	ck_assert_ptr_nonnull(expect);
	ck_assert_str_eq(app_im.committed, expect);
	free(expect);

	XmImFreeXIC(w, xic);
	wait_log("destroy_ic 1", 1);
	XmImUnregister(w);
}
END_TEST

/*
 * The commit comes in a key event with keycode 0, which Motif's key
 * translator must take as no key (it read before the keysym table).
 */
START_TEST(translate_keycode_zero)
{
	Modifiers mods = 0;
	KeySym sym = XK_a;

	(void)text_field("OnTheSpot", "");
	XmTranslateKey(dpy, 0, 0, &mods, &sym);
	ck_assert_uint_eq(sym, NoSymbol);
	XmTranslateKey(dpy, XKeysymToKeycode(dpy, XK_BackSpace), 0, &mods, &sym);
	ck_assert(sym == osfXK_BackSpace || sym == XK_BackSpace);
}
END_TEST

/* XmImUnsetFocus and XmImSetFocusValues reach the IM */
START_TEST(xic_focus)
{
	Widget tf = text_field("OnTheSpot", "");
	int focused, unfocused;

	im_sync(tf);
	focused = log_count("focus 1");
	unfocused = log_count("unfocus 1");
	XmImUnsetFocus(tf);
	im_sync(tf);
	ck_assert_int_eq(log_count("unfocus 1"), unfocused + 1);
	XmImSetFocusValues(tf, NULL, 0);
	im_sync(tf);
	ck_assert_int_eq(log_count("focus 1"), focused + 1);
}
END_TEST

/* XmImCloseXIM closes the IM; the next XmIm call opens it again */
START_TEST(close_im)
{
	Widget tf = text_field("OnTheSpot", "");

	ck_assert_ptr_nonnull(XmImGetXIM(tf));
	XmImCloseXIM(tf);
	wait_log("close", 1);
	wait_log("disconnect", 1);
	ck_assert_ptr_nonnull(XmImGetXIM(tf));
	ck_assert_int_eq(log_count("open "), 2);
}
END_TEST

void xim_suite(SRunner *runner)
{
	Suite *s = suite_create("Xim");
	TCase *t = tcase_create("Input method");

	tcase_add_checked_fixture(t, setup, teardown);
	tcase_set_timeout(t, 60);
	tcase_add_test(t, open_im);
	tcase_add_loop_test(t, on_the_spot_textfield, 0, 2);
	tcase_add_loop_test(t, on_the_spot_text, 0, 2);
	tcase_add_test(t, on_the_spot_reset);
	tcase_add_loop_test(t, on_the_spot_text_reset, 0, 2);
	tcase_add_test(t, on_the_spot_shared_xic);
	tcase_add_test(t, per_widget_xic);
	tcase_add_test(t, over_the_spot);
	tcase_add_test(t, off_the_spot);
	tcase_add_test(t, root_style);
	tcase_add_test(t, xmim_api);
	tcase_add_test(t, translate_keycode_zero);
	tcase_add_test(t, xic_focus);
	tcase_add_test(t, close_im);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
