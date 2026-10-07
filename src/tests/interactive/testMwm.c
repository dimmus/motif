/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * mwm_tests: libcheck tests of how mwm manages clients, run against the
 * mwm of the build as the window manager of $DISPLAY:
 *
 *   mwm_tests <mwm>
 *
 * They cover what the mwm performance work touched: the timestamps mwm
 * takes for focus changes, the properties it reads (or, from the list
 * of properties a window had when it was mapped, skips) when it manages
 * a window and those it reads later, the _NET_CLIENT_LIST property,
 * and the title shown after many title changes in a row.
 *
 * To know that mwm has handled what was sent before, a test restacks a
 * "fence" window mwm manages and waits for the synthetic ConfigureNotify
 * mwm answers with (ICCCM 4.1.5): mwm handles events in order.
 *
 * Exits with 77 when there is no display.
 */
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <check.h>

#define EXIT_SKIP 77
#define TIMEOUT_MS 20000

/* _MOTIF_WM_HINTS, from MwmUtil.h */
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_DECOR_BORDER (1L << 1)

static Display *dpy;
static Window rootw, fence;
static pid_t mwm_pid;
static char workdir[256];
static Atom a_wm_state, a_mwm_hints, a_client_list;
static Atom a_wm_protocols, a_wm_take_focus;
static Window clock_win;	/* for server timestamps */
static Time clock_time;
static int clock_ticks;

/* What we have seen happen to our windows. */
#define MAX_WINS 512
static struct seen {
	Window w;
	int reparented, mapped, unmapped, configured, fenced;
	int take_focus;		/* WM_TAKE_FOCUS messages */
	Time take_focus_time;	/* the time of the last one */
} seen[MAX_WINS];
static int nseen;

static struct seen *seen_for(Window w)
{
	int i;

	for (i = 0; i < nseen; i++)
		if (seen[i].w == w)
			return &seen[i];
	return NULL;
}

static double now_ms(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
}

static void handle(XEvent *ev)
{
	struct seen *s;

	switch (ev->type) {
	case ReparentNotify:
		if ((s = seen_for(ev->xreparent.window)) &&
		    ev->xreparent.parent != rootw)
			s->reparented++;
		break;
	case MapNotify:
		if ((s = seen_for(ev->xmap.window)))
			s->mapped++;
		break;
	case UnmapNotify:
		if ((s = seen_for(ev->xunmap.window)))
			s->unmapped++;
		break;
	case ClientMessage:
		if ((s = seen_for(ev->xclient.window)) &&
		    ev->xclient.message_type == a_wm_protocols &&
		    ev->xclient.format == 32 &&
		    (Atom)ev->xclient.data.l[0] == a_wm_take_focus) {
			s->take_focus++;
			s->take_focus_time = (Time)ev->xclient.data.l[1];
		}
		break;
	case PropertyNotify:
		if (ev->xproperty.window == clock_win) {
			clock_time = ev->xproperty.time;
			clock_ticks++;
		}
		break;
	case ConfigureNotify:
		if ((s = seen_for(ev->xconfigure.window))) {
			s->configured++;
			if (ev->xany.send_event)
				s->fenced++;
		}
		break;
	}
}

/* Handle events until *counter reaches target; 0 on a timeout. */
static int wait_for(int *counter, int target)
{
	double deadline = now_ms() + TIMEOUT_MS;
	struct pollfd pfd;
	XEvent ev;

	pfd.fd = ConnectionNumber(dpy);
	pfd.events = POLLIN;
	XFlush(dpy);
	while (*counter < target) {
		if (XPending(dpy)) {
			XNextEvent(dpy, &ev);
			handle(&ev);
			continue;
		}
		if (now_ms() > deadline)
			return 0;
		poll(&pfd, 1, 100);
	}
	return 1;
}

static Window make_client(const char *title, int x, int y, int w, int h,
			  int uspos)
{
	XSetWindowAttributes swa;
	XSizeHints *sh = XAllocSizeHints();
	XWMHints *wmh = XAllocWMHints();
	XClassHint ch = { (char *)"mwmtest", (char *)"MwmTest" };
	XTextProperty name;
	char *list[1];
	Window win;

	ck_assert_int_lt(nseen, MAX_WINS);
	swa.background_pixel = WhitePixel(dpy, DefaultScreen(dpy));
	swa.event_mask = StructureNotifyMask;
	win = XCreateWindow(dpy, rootw, x, y, w, h, 0, CopyFromParent,
			    InputOutput, CopyFromParent,
			    CWBackPixel | CWEventMask, &swa);
	sh->flags = uspos > 0 ? USPosition | USSize : PSize;
	sh->x = x;
	sh->y = y;
	sh->width = w;
	sh->height = h;
	wmh->flags = InputHint | StateHint;
	wmh->input = True;
	wmh->initial_state = NormalState;
	list[0] = (char *)title;
	ck_assert(XStringListToTextProperty(list, 1, &name));
	/* uspos < 0: no WM_NORMAL_HINTS */
	XSetWMProperties(dpy, win, &name, &name, NULL, 0, uspos < 0 ? NULL : sh,
			 wmh, &ch);
	XFree(name.value);
	XFree(sh);
	XFree(wmh);
	memset(&seen[nseen], 0, sizeof seen[nseen]);
	seen[nseen++].w = win;
	return win;
}

static void manage(Window w)
{
	struct seen *s = seen_for(w);

	XMapWindow(dpy, w);
	ck_assert_msg(wait_for(&s->reparented, 1), "mwm did not reparent 0x%lx",
		      w);
	ck_assert_msg(wait_for(&s->mapped, 1), "mwm did not map 0x%lx", w);
}

/* Return once mwm has handled everything sent so far. */
static void sync_mwm(void)
{
	struct seen *s = seen_for(fence);
	XWindowChanges wc;

	wc.stack_mode = Above;
	XConfigureWindow(dpy, fence, CWStackMode, &wc);
	ck_assert_msg(wait_for(&s->fenced, s->fenced + 1),
		      "mwm did not answer the fence");
	XSync(dpy, False);
}

/* The mwm frame of a managed window: its ancestor that is a root child. */
static Window frame_of(Window w)
{
	Window r, parent, *kids;
	unsigned int nkids;

	for (;;) {
		ck_assert(XQueryTree(dpy, w, &r, &parent, &kids, &nkids));
		if (kids)
			XFree(kids);
		if (parent == rootw)
			return w;
		w = parent;
	}
}

struct geom {
	int x, y, w, h;		/* root position and size */
};

static struct geom geometry(Window w)
{
	struct geom g;
	Window r, child;
	unsigned int uw, uh, bw, depth;
	int x, y;

	ck_assert(XGetGeometry(dpy, w, &r, &x, &y, &uw, &uh, &bw, &depth));
	XTranslateCoordinates(dpy, w, rootw, 0, 0, &g.x, &g.y, &child);
	g.w = (int)uw;
	g.h = (int)uh;
	return g;
}

static void destroy(Window w)
{
	struct seen *s = seen_for(w);

	XDestroyWindow(dpy, w);
	if (s)
		s->w = None;
}

/* The server time now, from a zero-length property append. */
static Time server_time(void)
{
	long none = 0;

	XChangeProperty(dpy, clock_win, XA_WM_NAME, XA_STRING, 8,
			PropModeAppend, (unsigned char *)&none, 0);
	ck_assert(wait_for(&clock_ticks, clock_ticks + 1));
	return clock_time;
}

/* ------------------------------------------------------------------ */

static void stop_mwm(void)
{
	if (mwm_pid > 0) {
		kill(mwm_pid, SIGKILL);
		waitpid(mwm_pid, NULL, 0);
		mwm_pid = 0;
	}
	if (*workdir) {
		char path[300];

		snprintf(path, sizeof path, "%s/mwm.log", workdir);
		unlink(path);
		rmdir(workdir);
		*workdir = '\0';
	}
}

static const char *mwm_path;

static void start_mwm(void)
{
	const char *mwm = mwm_path;
	const char *tmp = getenv("TMPDIR");
	char log[300];

	snprintf(workdir, sizeof workdir, "%s/mwm_tests.XXXXXX",
		 tmp && *tmp ? tmp : "/tmp");
	if (!mkdtemp(workdir)) {
		perror("mkdtemp");
		exit(1);
	}
	snprintf(log, sizeof log, "%s/mwm.log", workdir);
	fflush(NULL);
	mwm_pid = fork();
	if (mwm_pid == 0) {
		int fd = open(log, O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (fd >= 0) {
			dup2(fd, 1);
			dup2(fd, 2);
			close(fd);
		}
		close(ConnectionNumber(dpy));
		setenv("HOME", workdir, 1);	/* no .mwmrc of the user */
		execl(mwm, mwm, "-xrm", "Mwm*showFeedback: -kill",
		      "-xrm", "Mwm*useIconBox: False", (char *)NULL);
		_exit(127);
	}
	fence = make_client("mwmtest fence", 0, 0, 20, 20, 1);
	manage(fence);
	sync_mwm();
}

/* ------------------------------------------------------------------ */

/*
 * mwm gives the keyboard focus to each new window that takes input
 * (startupKeyFocus), and sends WM_TAKE_FOCUS to one that asks for it,
 * with the server time it takes for that: the client passes it on to
 * XSetInputFocus, which ignores a time later than the server's.
 */
START_TEST(test_focus_new_windows)
{
	Window a, b, focus;
	Time before, after;
	struct seen *s;
	int revert;

	a = make_client("mwmtest focus a", 600, 40, 120, 60, 1);
	manage(a);
	sync_mwm();
	XGetInputFocus(dpy, &focus, &revert);
	ck_assert_msg(focus == a, "focus is 0x%lx, not the new window 0x%lx",
		      focus, a);

	b = make_client("mwmtest focus b", 760, 40, 120, 60, 1);
	XSetWMProtocols(dpy, b, &a_wm_take_focus, 1);
	s = seen_for(b);
	before = server_time();
	manage(b);
	ck_assert_msg(wait_for(&s->take_focus, 1), "no WM_TAKE_FOCUS");
	after = server_time();
	ck_assert_msg((long)(s->take_focus_time - before) >= 0 &&
		      (long)(after - s->take_focus_time) >= 0,
		      "WM_TAKE_FOCUS time %lu is not between %lu and %lu",
		      s->take_focus_time, before, after);
	sync_mwm();
	XGetInputFocus(dpy, &focus, &revert);
	ck_assert_msg(focus == b, "focus is 0x%lx, not the new window 0x%lx",
		      focus, b);
	destroy(a);
	destroy(b);
	sync_mwm();
}
END_TEST

/* A window that asks to start iconic is managed iconic. */
START_TEST(test_initial_iconic)
{
	XWMHints *wmh = XAllocWMHints();
	Window w = make_client("mwmtest iconic", 40, 40, 120, 60, 1);
	struct seen *s = seen_for(w);
	Atom type;
	int format;
	unsigned long n, after;
	unsigned char *data = NULL;

	wmh->flags = InputHint | StateHint;
	wmh->input = True;
	wmh->initial_state = IconicState;
	XSetWMHints(dpy, w, wmh);
	XFree(wmh);
	XMapWindow(dpy, w);
	ck_assert(wait_for(&s->reparented, 1));
	sync_mwm();
	ck_assert_int_eq(s->mapped, 0);
	ck_assert_int_eq(XGetWindowProperty(dpy, w, a_wm_state, 0, 2, False,
					    a_wm_state, &type, &format, &n,
					    &after, &data), Success);
	ck_assert_ptr_nonnull(data);
	ck_assert_int_ge(n, 1);
	ck_assert_int_eq(((long *)data)[0], IconicState);
	XFree(data);
	destroy(w);
	sync_mwm();
}
END_TEST

/*
 * The properties a window has when it is mapped are honoured, and so are
 * those it sets once managed: mwm skips reading the properties a window
 * did not have when it was mapped, until the window is managed.
 */
START_TEST(test_properties)
{
	long hints[5] = { MWM_HINTS_DECORATIONS, 0, MWM_DECOR_BORDER, 0, 0 };
	Window plain, bare, transient, late;
	struct geom gp, gb, gt, fp, fb, ft;
	XSizeHints *sh;

	plain = make_client("mwmtest plain", 100, 150, 200, 100, 1);
	bare = make_client("mwmtest bare", 400, 150, 200, 100, 1);
	XChangeProperty(dpy, bare, a_mwm_hints, a_mwm_hints, 32,
			PropModeReplace, (unsigned char *)hints, 5);
	transient = make_client("mwmtest transient", 700, 150, 200, 100, 1);
	XSetTransientForHint(dpy, transient, plain);
	late = make_client("mwmtest late", 100, 450, 200, 100, -1);
	manage(plain);
	manage(bare);
	manage(transient);
	manage(late);
	sync_mwm();

	gp = geometry(plain);
	gb = geometry(bare);
	gt = geometry(transient);
	fp = geometry(frame_of(plain));
	fb = geometry(frame_of(bare));
	ft = geometry(frame_of(transient));

	/* WM_NORMAL_HINTS: USPosition is the position of the frame */
	ck_assert_int_eq(fp.x, 100);
	ck_assert_int_eq(fp.y, 150);
	/* _MOTIF_WM_HINTS: no title bar, no resize handles */
	ck_assert_int_lt(gb.y - fb.y, gp.y - fp.y);
	ck_assert_int_lt(gb.x - fb.x, gp.x - fp.x);
	/* WM_TRANSIENT_FOR: decorated as the leader, iconified with it */
	ck_assert_int_eq(gt.x - ft.x, gp.x - fp.x);
	ck_assert_int_eq(gt.y - ft.y, gp.y - fp.y);
	XIconifyWindow(dpy, plain, DefaultScreen(dpy));
	ck_assert_msg(wait_for(&seen_for(transient)->unmapped, 1),
		      "the transient was not iconified with its leader");
	XMapWindow(dpy, plain);
	ck_assert(wait_for(&seen_for(transient)->mapped, 2));

	/* WM_NORMAL_HINTS, which it did not have, set once managed */
	sh = XAllocSizeHints();
	sh->flags = PMinSize;
	sh->min_width = 260;
	sh->min_height = 140;
	XSetWMNormalHints(dpy, late, sh);
	XFree(sh);
	XResizeWindow(dpy, late, 50, 50);
	sync_mwm();
	gp = geometry(late);
	ck_assert_msg(gp.w >= 260 && gp.h >= 140,
		      "the minimum size set after mapping was ignored: %dx%d",
		      gp.w, gp.h);

	destroy(late);
	destroy(transient);
	destroy(bare);
	destroy(plain);
	sync_mwm();
}
END_TEST

static int client_list_has(const Window *list, unsigned long n, Window w)
{
	unsigned long i;

	for (i = 0; i < n; i++)
		if (list[i] == w)
			return 1;
	return 0;
}

static Window *client_list(unsigned long *n)
{
	Atom type;
	int format;
	unsigned long after;
	unsigned char *data = NULL;

	ck_assert_int_eq(XGetWindowProperty(dpy, rootw, a_client_list, 0,
					    100000, False, XA_WINDOW, &type,
					    &format, n, &after, &data),
			 Success);
	ck_assert_int_eq(type, XA_WINDOW);
	ck_assert_int_eq(format, 32);
	return (Window *)data;
}

/*
 * _NET_CLIENT_LIST lists the managed windows, past the first growth
 * of the array mwm builds it in.
 */
START_TEST(test_client_list)
{
	enum { N = 100 };
	Window w[N], *list;
	unsigned long n;
	int i;

	for (i = 0; i < N; i++) {
		char title[32];

		snprintf(title, sizeof title, "mwmtest list %d", i);
		w[i] = make_client(title, 20 + i * 5, 400 + i * 2, 80, 40, 1);
		XMapWindow(dpy, w[i]);
	}
	for (i = 0; i < N; i++)
		ck_assert(wait_for(&seen_for(w[i])->mapped, 1));
	sync_mwm();
	list = client_list(&n);
	ck_assert_int_eq(n, N + 1);	/* and the fence */
	ck_assert(client_list_has(list, n, fence));
	for (i = 0; i < N; i++)
		ck_assert_msg(client_list_has(list, n, w[i]),
			      "window %d is not in _NET_CLIENT_LIST", i);
	XFree(list);

	for (i = 0; i < N; i += 2)
		destroy(w[i]);
	sync_mwm();
	list = client_list(&n);
	ck_assert_int_eq(n, N / 2 + 1);
	for (i = 0; i < N; i++)
		ck_assert_int_eq(client_list_has(list, n, w[i]), i % 2);
	XFree(list);

	for (i = 1; i < N; i += 2)
		destroy(w[i]);
	sync_mwm();
	list = client_list(&n);
	ck_assert_int_eq(n, 1);
	ck_assert(list[0] == fence);
	XFree(list);
}
END_TEST

/*
 * A window destroyed while mwm manages it is dropped: mwm notices it
 * with the round trip it makes once it has reparented the window, and
 * neither lists it nor gives it the focus.  The windows go away at
 * different points of their management, depending on how far the
 * MapRequest got before the destroy.
 */
START_TEST(test_destroyed_while_managed)
{
	enum { N = 120 };
	Window w, focus, *list;
	unsigned long n;
	int i, revert, status;

	for (i = 0; i < N; i++) {
		char title[32];

		snprintf(title, sizeof title, "mwmtest gone %d", i);
		w = make_client(title, 20 + (i % 40) * 5, 300, 80, 40, 1);
		XMapWindow(dpy, w);
		if (i % 4 == 1) {
			XFlush(dpy);
		} else if (i % 4 >= 2) {
			XSync(dpy, False);
			usleep(25 * (i / 2));	/* up to 1.5 ms */
		}
		destroy(w);
	}
	sync_mwm();
	ck_assert_msg(waitpid(mwm_pid, &status, WNOHANG) == 0, "mwm exited");
	list = client_list(&n);
	ck_assert_int_eq(n, 1);
	ck_assert(list[0] == fence);
	XFree(list);

	/* and the next window is managed, and has the focus */
	w = make_client("mwmtest after gone", 600, 300, 120, 60, 1);
	manage(w);
	sync_mwm();
	XGetInputFocus(dpy, &focus, &revert);
	ck_assert_msg(focus == w, "focus is 0x%lx, not the new window 0x%lx",
		      focus, w);
	destroy(w);
	sync_mwm();
}
END_TEST

/* The pixels of the decoration above a managed window. */
static XImage *title_image(Window w)
{
	struct geom g = geometry(w), f = geometry(frame_of(w));

	ck_assert_int_gt(g.y - f.y, 0);
	return XGetImage(dpy, rootw, f.x, f.y, (unsigned)f.w,
			 (unsigned)(g.y - f.y), AllPlanes, ZPixmap);
}

static int same_image(XImage *a, XImage *b)
{
	int y;

	if (a->width != b->width || a->height != b->height ||
	    a->bytes_per_line != b->bytes_per_line)
		return 0;
	for (y = 0; y < a->height; y++)
		if (memcmp(a->data + y * a->bytes_per_line,
			   b->data + y * b->bytes_per_line,
			   (size_t)a->bytes_per_line))
			return 0;
	return 1;
}

/*
 * After many title changes in a row, the title bar shows the last one:
 * it looks the same as that of a window mapped with that title.
 */
START_TEST(test_title_changes)
{
	Window a, b, c, other;
	XImage *ia, *ib, *ic;
	char title[64];
	int round, i, len;

	a = make_client("mwmtest start", 40, 700, 300, 60, 1);
	b = make_client("mwmtest final", 400, 700, 300, 60, 1);
	c = make_client("mwmtest other", 760, 700, 300, 60, 1);
	manage(a);
	manage(b);
	manage(c);
	/* take the focus from all three, so they are drawn alike */
	other = make_client("mwmtest focus", 900, 40, 120, 60, 1);
	manage(other);
	sync_mwm();
	ib = title_image(b);
	ic = title_image(c);
	ck_assert(ib && ic);
	ck_assert_msg(!same_image(ib, ic),
		      "different titles look the same; the test cannot tell");

	/*
	 * Whether mwm has the last change queued behind others when it
	 * reads the title depends on timing: vary the delay before it.
	 */
	for (round = 0; round < 16; round++) {
		for (i = 0; i < 500; i++) {
			len = snprintf(title, sizeof title, "mwmtest %d", i);
			XChangeProperty(dpy, a, XA_WM_NAME, XA_STRING, 8,
					PropModeReplace,
					(unsigned char *)title, len);
		}
		XFlush(dpy);
		usleep(round * 50);
		XStoreName(dpy, a, "mwmtest final");
		sync_mwm();
		ia = title_image(a);
		ck_assert(ia);
		ck_assert_msg(same_image(ia, ib), "the title bar does not show "
			      "the last title (round %d)", round);
		XDestroyImage(ia);
	}
	XDestroyImage(ib);
	XDestroyImage(ic);
	destroy(a);
	destroy(b);
	destroy(c);
	destroy(other);
	sync_mwm();
}
END_TEST

static int x_error(Display *d, XErrorEvent *ev)
{
	char text[128];

	XGetErrorText(d, ev->error_code, text, sizeof text);
	fprintf(stderr, "mwm_tests: X error: %s (request %d.%d)\n", text,
		ev->request_code, ev->minor_code);
	return 0;
}

int main(int argc, char **argv)
{
	SRunner *sr;
	Suite *s;
	TCase *tc;
	int failed;

	if (argc != 2) {
		fprintf(stderr, "usage: mwm_tests <mwm>\n");
		return 1;
	}
	if (!getenv("DISPLAY") || !*getenv("DISPLAY") ||
	    !(dpy = XOpenDisplay(NULL))) {
		printf("mwm_tests: skipped (no display)\n");
		return EXIT_SKIP;
	}
	XSetErrorHandler(x_error);
	rootw = DefaultRootWindow(dpy);
	a_wm_state = XInternAtom(dpy, "WM_STATE", False);
	a_mwm_hints = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);
	a_client_list = XInternAtom(dpy, "_NET_CLIENT_LIST", False);
	a_wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
	a_wm_take_focus = XInternAtom(dpy, "WM_TAKE_FOCUS", False);
	clock_win = XCreateWindow(dpy, rootw, -10, -10, 1, 1, 0, 0, InputOnly,
				  CopyFromParent, 0, NULL);
	XSelectInput(dpy, clock_win, PropertyChangeMask);
	signal(SIGPIPE, SIG_IGN);
	mwm_path = argv[1];

	s = suite_create("Mwm");
	tc = tcase_create("manage");
	tcase_set_timeout(tc, 120);
	tcase_add_unchecked_fixture(tc, start_mwm, stop_mwm);
	tcase_add_test(tc, test_focus_new_windows);
	tcase_add_test(tc, test_initial_iconic);
	tcase_add_test(tc, test_properties);
	tcase_add_test(tc, test_client_list);
	tcase_add_test(tc, test_destroyed_while_managed);
	tcase_add_test(tc, test_title_changes);
	suite_add_tcase(s, tc);
	sr = srunner_create(s);
	/* one mwm and one connection for all the tests */
	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	stop_mwm();
	XCloseDisplay(dpy);
	return failed ? 1 : 0;
}
