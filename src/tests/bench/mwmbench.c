/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * mwmbench: macro-benchmarks for mwm.
 *
 *   mwmbench [-r REPEAT] [-s SCALE] [-j FILE] [-m MWM] [-L LOG] [-l]
 *            [CASE ...]
 *
 * mwmbench starts the mwm under test (-m, by default the one of this
 * build) as the window manager of $DISPLAY, which must have none, and
 * drives it as a client would: it maps and destroys hundreds of
 * windows, retitles a window thousands of times, and drags a window by
 * its title bar and by its resize handle with XTest.
 *
 * Each case reports, per operation, as xmbench does:
 *   - ns       median wall time over REPEAT runs (default 5), from the
 *              first request to the moment mwm has handled all of it and
 *              is idle again in its event loop
 *   - cpu      median CPU time of mwm (not of the X server or mwmbench)
 *   - xcpu     median CPU time of the X server, when it is local (found
 *              with SO_PEERCRED); 0 otherwise
 *   - mallocs  mwm's calls to malloc/calloc/realloc
 *   - requests X requests mwm issued
 *   - rtrips   round trips mwm made (calls to _XReply)
 * The "map" case also reports the time until every client is reparented
 * and mapped (mapped-ns, the rest of ns is mwm settling), and, in KiB,
 * mwm's resident set once the windows are managed (rss) and its growth
 * over the run (rss+).
 *
 * The counters come from libmwmbench_preload.so, which mwmbench preloads
 * into mwm (see mwmpreload.c).  It also tells mwmbench when mwm has gone
 * idle and, during a drag, when mwm has seen each pointer step: mwm then
 * runs its own modal loop with the pointer grabbed, and queries the
 * pointer once for every motion hint.
 *
 * To know that mwm has handled everything sent before, mwmbench asks it
 * to restack a small "fence" window it manages: mwm answers with a
 * synthetic ConfigureNotify (ICCCM 4.1.5), and handles events in order.
 *
 * The outline move and the resize run with freezeOnConfig off: with it
 * on, mwm grabs the server during the drag, and no client, XTest
 * included, could move the pointer.
 *
 * mwm, the X server and mwmbench wake each other up for every
 * operation.  When they run on different CPUs, the wall times include
 * the latency of those wake-ups, which on a virtual machine can be ten
 * times the work itself and varies from run to run; run mwmbench under
 * "taskset -c 0" (the X server included) to measure the work.
 *
 * mwmbench exits with 77 when there is no display, and with 1 when a
 * case fails (mwm died, timed out, or left a window in the wrong place).
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <linux/futex.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

#include "mwmbench.h"

#define EXIT_SKIP 77
#define MAX_REPEAT 64
#define TIMEOUT_MS 30000

#ifndef MWMBENCH_MWM
#define MWMBENCH_MWM "mwm"
#endif
#ifndef MWMBENCH_PRELOAD
#define MWMBENCH_PRELOAD "libmwmbench_preload.so"
#endif

struct bench_case {
	const char *name;
	const char *desc;
	const char *const *xrm;    /* extra mwm resources */
	long n;                    /* operations at scale 1 */
	void (*setup)(long n);     /* before each run, untimed */
	long (*run)(long n);       /* timed, returns the operations done */
	void (*teardown)(void);    /* after each run, untimed */
	int memory;                /* report the mapping time and mwm's
				      resident set */
};

struct result {
	double ns, cpu, xcpu, mallocs, requests, rtrips;
	double phase, rss, rss_delta;
};

static Display *dpy;
static Window rootw;
static Atom a_utf8, a_net_wm_name;
static const char *mwm_path = MWMBENCH_MWM;
static pid_t mwm_pid;
static pid_t server_pid;       /* the X server, when it is local */
static const char *const *mwm_xrm;
static struct mwmbench_shm *shm;
static int shm_fd = -1;
static char workdir[PATH_MAX];
static char logpath[PATH_MAX + 16];
static int keep_log;           /* -L: the log is the user's file */

/* Our windows and what we have seen happen to them. */
static Window *wins;
static long nwins, reparented, mapped;
static Window fence;
static long fences;

/* ------------------------------------------------------------------ */
/* Utilities                                                           */
/* ------------------------------------------------------------------ */

static void stop_mwm(void);

static double now_ns(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1e9 + ts.tv_nsec;
}

static void cleanup(void)
{
	stop_mwm();
	if (*logpath && !keep_log)
		unlink(logpath);
	if (*workdir)
		rmdir(workdir);
	*logpath = *workdir = '\0';
}

static void die(const char *fmt, ...)
{
	char line[512];
	va_list ap;
	FILE *f;

	fflush(stdout);
	fprintf(stderr, "mwmbench: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	if (*logpath && (f = fopen(logpath, "r"))) {
		while (fgets(line, sizeof line, f))
			fprintf(stderr, "mwm: %s", line);
		fclose(f);
	}
	cleanup();
	exit(1);
}

static void on_signal(int sig)
{
	if (mwm_pid > 0)
		kill(mwm_pid, SIGKILL);
	signal(sig, SIG_DFL);
	raise(sig);
}

static int x_error(Display *d, XErrorEvent *ev)
{
	char text[128];

	XGetErrorText(d, ev->error_code, text, sizeof text);
	die("X error: %s (request %d.%d)", text, ev->request_code,
	    ev->minor_code);
	return 0;
}

static void check_mwm(void)
{
	int status;

	if (mwm_pid > 0 && waitpid(mwm_pid, &status, WNOHANG) == mwm_pid) {
		mwm_pid = 0;
		die("mwm exited (status 0x%x)", status);
	}
}

/* The CPU time of a process in ns, from the scheduler's accounting. */
static double cpu_ns(pid_t pid)
{
	char path[64];
	unsigned long long ns = 0;
	FILE *f;

	if (pid <= 0)
		return 0;
	snprintf(path, sizeof path, "/proc/%d/schedstat", (int)pid);
	if ((f = fopen(path, "r"))) {
		if (fscanf(f, "%llu", &ns) != 1)
			ns = 0;
		fclose(f);
	}
	return (double)ns;
}

/* The process at the other end of a local socket, or 0. */
static pid_t peer_pid(int fd)
{
	struct ucred cred;
	socklen_t len = sizeof cred;

	if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cred, &len) < 0 ||
	    kill(cred.pid, 0) < 0)
		return 0;
	return cred.pid;
}

/* mwm's resident set in KiB. */
static long mwm_rss_kib(void)
{
	char path[64], line[128];
	long kib = 0;
	FILE *f;

	snprintf(path, sizeof path, "/proc/%d/status", (int)mwm_pid);
	if ((f = fopen(path, "r"))) {
		while (fgets(line, sizeof line, f))
			if (sscanf(line, "VmRSS: %ld", &kib) == 1)
				break;
		fclose(f);
	}
	return kib;
}

/* Wait for *word to change from val, for at most ms. */
static void futex_wait(unsigned int *word, unsigned int val, int ms)
{
	struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };

	syscall(SYS_futex, word, FUTEX_WAIT, val, &ts, NULL, 0);
}

static int our_window(Window w)
{
	long i;

	for (i = 0; i < nwins; i++)
		if (wins[i] == w)
			return 1;
	return 0;
}

static void handle(XEvent *ev)
{
	switch (ev->type) {
	case ReparentNotify:
		if (ev->xreparent.parent != rootw &&
		    our_window(ev->xreparent.window))
			reparented++;
		break;
	case MapNotify:
		if (our_window(ev->xmap.window))
			mapped++;
		break;
	case ConfigureNotify:
		if (ev->xconfigure.window == fence && ev->xany.send_event)
			fences++;
		break;
	}
}

/* Handle events until *counter reaches target. */
static void wait_count(long *counter, long target, const char *what)
{
	double deadline = now_ns() + TIMEOUT_MS * 1e6;
	struct pollfd pfd = { ConnectionNumber(dpy), POLLIN, 0 };
	XEvent ev;

	XFlush(dpy);
	while (*counter < target) {
		if (XPending(dpy)) {
			XNextEvent(dpy, &ev);
			handle(&ev);
			continue;
		}
		check_mwm();
		if (now_ns() > deadline)
			die("timed out waiting for %s (%ld of %ld)", what,
			    *counter, target);
		poll(&pfd, 1, 100);
	}
}

/* Wait until mwm is blocked in its event loop. */
static void wait_idle(void)
{
	double deadline = now_ns() + TIMEOUT_MS * 1e6;

	for (;;) {
		unsigned int seq = __atomic_load_n(&shm->idle_seq,
						   __ATOMIC_ACQUIRE);

		if (__atomic_load_n(&shm->idle, __ATOMIC_RELAXED))
			return;
		check_mwm();
		if (now_ns() > deadline)
			die("timed out waiting for mwm to go idle");
		futex_wait(&shm->idle_seq, seq, 100);
	}
}

/*
 * Return once mwm has handled every request sent so far and is idle.
 * Restacking a managed window makes it send a synthetic ConfigureNotify.
 */
static void sync_mwm(void)
{
	XWindowChanges wc;

	wc.stack_mode = Above;
	XConfigureWindow(dpy, fence, CWStackMode, &wc);
	wait_count(&fences, fences + 1, "the fence window's ConfigureNotify");
	wait_idle();
}

/* Wait until mwm's last XQueryPointer returned (x, y). */
static void wait_query(int x, int y, const char *what)
{
	double deadline = now_ns() + TIMEOUT_MS * 1e6;

	XFlush(dpy);
	for (;;) {
		unsigned int seq = __atomic_load_n(&shm->query_seq,
						   __ATOMIC_ACQUIRE);

		if (__atomic_load_n(&shm->query_x, __ATOMIC_RELAXED) == x &&
		    __atomic_load_n(&shm->query_y, __ATOMIC_RELAXED) == y)
			return;
		check_mwm();
		if (now_ns() > deadline)
			die("timed out waiting for mwm to see the pointer at "
			    "%d,%d (%s; it last saw %d,%d)", x, y, what,
			    shm->query_x, shm->query_y);
		futex_wait(&shm->query_seq, seq, 100);
	}
}

/* Wait until mwm has grabbed the pointer since grab_seq was seq. */
static void wait_grab(unsigned int seq, const char *what)
{
	double deadline = now_ns() + TIMEOUT_MS * 1e6;

	XFlush(dpy);
	while (__atomic_load_n(&shm->grab_seq, __ATOMIC_ACQUIRE) == seq) {
		check_mwm();
		if (now_ns() > deadline)
			die("timed out waiting for mwm to grab the pointer (%s)",
			    what);
		futex_wait(&shm->grab_seq, seq, 100);
	}
}

/* ------------------------------------------------------------------ */
/* mwm                                                                 */
/* ------------------------------------------------------------------ */

static const char *const xrm_base[] = {
	/* so that it can be stopped normally, and has no icon box */
	"Mwm*showFeedback: -kill",
	"Mwm*useIconBox: False",
	NULL
};

static Window create_window(const char *title, int x, int y,
			    unsigned int w, unsigned int h, int uspos)
{
	XSetWindowAttributes swa;
	XSizeHints *sh = XAllocSizeHints();
	XWMHints *wmh = XAllocWMHints();
	XClassHint ch = { (char *)"mwmbench", (char *)"MwmBench" };
	XTextProperty name;
	char *list[1];
	Window win;

	swa.background_pixel = WhitePixel(dpy, DefaultScreen(dpy));
	swa.event_mask = StructureNotifyMask;
	win = XCreateWindow(dpy, rootw, x, y, w, h, 0, CopyFromParent,
			    InputOutput, CopyFromParent,
			    CWBackPixel | CWEventMask, &swa);
	sh->flags = uspos ? USPosition | USSize : PSize;
	sh->x = x;
	sh->y = y;
	sh->width = w;
	sh->height = h;
	wmh->flags = InputHint | StateHint;
	wmh->input = True;
	wmh->initial_state = NormalState;
	list[0] = (char *)title;
	if (!XStringListToTextProperty(list, 1, &name))
		die("out of memory");
	XSetWMProperties(dpy, win, &name, &name, NULL, 0, sh, wmh, &ch);
	XFree(name.value);
	XFree(sh);
	XFree(wmh);
	return win;
}

static void add_window(Window w)
{
	wins[nwins++] = w;
}

static void stop_mwm(void)
{
	if (mwm_pid > 0) {
		kill(mwm_pid, SIGKILL);
		waitpid(mwm_pid, NULL, 0);
	}
	mwm_pid = 0;
	mwm_xrm = NULL;
	if (dpy && fence) {
		XDestroyWindow(dpy, fence);
		XSync(dpy, True);
	}
	fence = 0;
}

static void start_mwm(const char *const *xrm)
{
	const char *argv[32];
	char fdstr[16];
	int argc = 0, i, fd;
	long seen;

	stop_mwm();
	memset(shm, 0, sizeof *shm);
	argv[argc++] = mwm_path;
	for (i = 0; xrm_base[i]; i++) {
		argv[argc++] = "-xrm";
		argv[argc++] = xrm_base[i];
	}
	for (i = 0; xrm && xrm[i] && argc < 29; i++) {
		argv[argc++] = "-xrm";
		argv[argc++] = xrm[i];
	}
	argv[argc] = NULL;

	fflush(NULL);
	mwm_pid = fork();
	if (mwm_pid < 0)
		die("fork: %s", strerror(errno));
	if (mwm_pid == 0) {
		const char *preload = getenv("MWMBENCH_PRELOAD");

		fd = open(logpath, O_WRONLY | O_CREAT | O_APPEND, 0600);
		if (fd >= 0) {
			dup2(fd, 1);
			dup2(fd, 2);
			close(fd);
		}
		close(ConnectionNumber(dpy));
		/* An empty home: no .mwmrc, no resources of the user. */
		setenv("HOME", workdir, 1);
		unsetenv("XENVIRONMENT");
		unsetenv("XAPPLRESDIR");
		snprintf(fdstr, sizeof fdstr, "%d", shm_fd);
		setenv("MWMBENCH_SHM_FD", fdstr, 1);
		setenv("LD_PRELOAD", preload ? preload : MWMBENCH_PRELOAD, 1);
		execvp(mwm_path, (char *const *)argv);
		fprintf(stderr, "exec %s: %s\n", mwm_path, strerror(errno));
		_exit(127);
	}
	mwm_xrm = xrm;

	/*
	 * The fence window doubles as the check that mwm is up: whether it
	 * is mapped before or after mwm starts, mwm manages it.
	 */
	fence = create_window("mwmbench fence", 0, 0, 20, 20, 0);
	seen = reparented;
	nwins = 0;
	add_window(fence);
	XMapWindow(dpy, fence);
	wait_count(&reparented, seen + 1, "mwm to manage the fence window");
	wait_count(&mapped, mapped + 1, "mwm to map the fence window");
	nwins = 0;
	sync_mwm();
}

/* ------------------------------------------------------------------ */
/* Cases                                                               */
/* ------------------------------------------------------------------ */

static double phase_ns;    /* time to a milestone within a run */

static void destroy_windows(void)
{
	long i;

	for (i = 0; i < nwins; i++)
		XDestroyWindow(dpy, wins[i]);
	nwins = 0;
	sync_mwm();
}

/* Create n windows at the size of a small application, unmapped. */
static void create_windows(long n)
{
	char title[64];
	long i;

	nwins = 0;
	reparented = mapped = 0;
	for (i = 0; i < n; i++) {
		snprintf(title, sizeof title, "mwmbench client %ld", i);
		add_window(create_window(title, 0, 0, 240, 160, 0));
	}
	XSync(dpy, False);
}

static long map_windows(long n)
{
	double t0 = now_ns();
	long i;

	for (i = 0; i < n; i++)
		XMapWindow(dpy, wins[i]);
	wait_count(&reparented, n, "mwm to reparent the clients");
	wait_count(&mapped, n, "mwm to map the clients");
	phase_ns = now_ns() - t0;
	return n;
}

static void map_setup_destroy(long n)
{
	create_windows(n);
	map_windows(n);
	sync_mwm();
}

static long destroy_run(long n)
{
	long i;

	for (i = 0; i < nwins; i++)
		XDestroyWindow(dpy, wins[i]);
	nwins = 0;
	return n;
}

/* One managed window, for the title cases. */
static void one_window_setup(long n)
{
	(void)n;
	nwins = 0;
	reparented = mapped = 0;
	add_window(create_window("mwmbench title", 300, 300, 400, 120, 1));
	XMapWindow(dpy, wins[0]);
	wait_count(&mapped, 1, "mwm to map the client");
}

/*
 * Retitle the client n times, back to back (mwm may skip the changes it
 * sees queued behind a newer one) or, with paced, waiting each time
 * until mwm has handled the change.
 */
static long title_run(Atom prop, Atom type, long n, int paced)
{
	char title[64];
	long i;

	for (i = 0; i < n; i++) {
		int len = snprintf(title, sizeof title,
				   "mwmbench title %ld", i);

		XChangeProperty(dpy, wins[0], prop, type, 8, PropModeReplace,
				(unsigned char *)title, len);
		if (paced)
			sync_mwm();
	}
	return n;
}

static long title_wm_name_run(long n)
{
	return title_run(XA_WM_NAME, XA_STRING, n, 0);
}

static long title_paced_run(long n)
{
	return title_run(XA_WM_NAME, XA_STRING, n, 1);
}

static long title_net_wm_name_run(long n)
{
	return title_run(a_net_wm_name, a_utf8, n, 0);
}

/*
 * The drags.  The client is at a known place; press on its title bar
 * (or resize handle), move the pointer past mwm's move threshold, then
 * one pixel down and right per step, waiting each time until mwm has
 * seen the new position, and release.
 */
#define DRAG_START 12
static int drag_x, drag_y;          /* where the button was pressed */
static int client_x, client_y;      /* client's root position before */
static int client_w, client_h;
static long drag_steps;
static int drag_resize;

static void client_geometry(Window w, int *x, int *y, int *wd, int *ht)
{
	Window r, child;
	unsigned int uw, uh, bw, depth;
	int gx, gy;

	XGetGeometry(dpy, w, &r, &gx, &gy, &uw, &uh, &bw, &depth);
	XTranslateCoordinates(dpy, w, rootw, 0, 0, x, y, &child);
	*wd = (int)uw;
	*ht = (int)uh;
}

/* The mwm frame of a managed window: its ancestor that is a root child. */
static Window frame_of(Window w)
{
	Window r, parent, *kids;
	unsigned int nkids;

	for (;;) {
		if (!XQueryTree(dpy, w, &r, &parent, &kids, &nkids))
			die("XQueryTree failed");
		if (kids)
			XFree(kids);
		if (parent == rootw)
			return w;
		w = parent;
	}
}

static void drag_setup(int resize)
{
	one_window_setup(1);
	sync_mwm();
	client_geometry(wins[0], &client_x, &client_y, &client_w, &client_h);
	drag_resize = resize;
	if (resize) {
		Window r;
		int fx, fy;
		unsigned int fw, fh, bw, depth;

		/*
		 * The last pixel of the frame, in its resize handle: mwm
		 * keeps that pixel under the pointer, so the client grows
		 * by exactly as much as the pointer moves.
		 */
		XGetGeometry(dpy, frame_of(wins[0]), &r, &fx, &fy, &fw, &fh,
			     &bw, &depth);
		drag_x = fx + (int)(fw + 2 * bw) - 1;
		drag_y = fy + (int)(fh + 2 * bw) - 1;
	} else {
		/* the middle of the title bar, just above the client */
		drag_x = client_x + client_w / 2;
		drag_y = client_y - 8;
	}
	XTestFakeMotionEvent(dpy, -1, drag_x, drag_y, CurrentTime);
	sync_mwm();
}

static void move_setup(long n)
{
	(void)n;
	drag_setup(0);
}

static void resize_setup(long n)
{
	(void)n;
	drag_setup(1);
}

static long drag_run(long n)
{
	unsigned int grabs = __atomic_load_n(&shm->grab_seq, __ATOMIC_ACQUIRE);
	long i;

	/*
	 * Past the move threshold, mwm grabs the pointer and enters its
	 * own loop, where it queries the pointer for each motion hint.
	 */
	XTestFakeButtonEvent(dpy, 1, True, CurrentTime);
	XTestFakeMotionEvent(dpy, -1, drag_x + DRAG_START, drag_y + DRAG_START,
			     CurrentTime);
	wait_grab(grabs, "the start of the drag");
	for (i = 1; i <= n; i++) {
		int x = drag_x + DRAG_START + (int)i;
		int y = drag_y + DRAG_START + (int)i;

		XTestFakeMotionEvent(dpy, -1, x, y, CurrentTime);
		wait_query(x, y, "a drag step");
	}
	XTestFakeButtonEvent(dpy, 1, False, CurrentTime);
	drag_steps = n;
	return n;
}

/* Check where the drag left the window, then remove it. */
static void drag_teardown(void)
{
	int x, y, w, h, d = DRAG_START + (int)drag_steps;
	int ex = client_x, ey = client_y, ew = client_w, eh = client_h;

	client_geometry(wins[0], &x, &y, &w, &h);
	if (drag_resize) {
		ew += d;
		eh += d;
	} else {
		ex += d;
		ey += d;
	}
	if (x != ex || y != ey || w != ew || h != eh)
		die("the drag left the client at %dx%d+%d+%d, not %dx%d+%d+%d",
		    w, h, x, y, ew, eh, ex, ey);
	destroy_windows();
}

static const char *const xrm_opaque[] = {
	"Mwm*moveOpaque: True",
	NULL
};

static const char *const xrm_outline[] = {
	"Mwm*moveOpaque: False",
	"Mwm*freezeOnConfig: False",
	NULL
};

static const struct bench_case cases[] = {
	{ "map", "map clients until all are managed and mwm is idle",
	  NULL, 500, create_windows, map_windows, destroy_windows, 1 },
	{ "destroy", "destroy managed clients until mwm is idle",
	  NULL, 500, map_setup_destroy, destroy_run, NULL, 0 },
	{ "title", "change WM_NAME of a managed client, back to back",
	  NULL, 10000, one_window_setup, title_wm_name_run, destroy_windows, 0 },
	{ "title-paced", "change WM_NAME, each time until mwm is idle",
	  NULL, 2000, one_window_setup, title_paced_run, destroy_windows, 0 },
	{ "title-net", "change _NET_WM_NAME (mwm does not read it)",
	  NULL, 10000, one_window_setup, title_net_wm_name_run,
	  destroy_windows, 0 },
	{ "drag-move", "drag a window by its title bar, opaque move, per step",
	  xrm_opaque, 500, move_setup, drag_run, drag_teardown, 0 },
	{ "drag-move-outline", "drag a window by its title bar, outline, per step",
	  xrm_outline, 500, move_setup, drag_run, drag_teardown, 0 },
	{ "drag-resize", "resize a window by its corner, per step",
	  xrm_outline, 500, resize_setup, drag_run, drag_teardown, 0 },
};

#define N_CASES (sizeof cases / sizeof cases[0])

/* ------------------------------------------------------------------ */
/* Harness                                                             */
/* ------------------------------------------------------------------ */

static int cmp_double(const void *a, const void *b)
{
	double x = *(const double *)a, y = *(const double *)b;

	return (x > y) - (x < y);
}

static double median(double *v, int n)
{
	qsort(v, n, sizeof *v, cmp_double);
	return (n & 1) ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2;
}

static void run_case(const struct bench_case *bc, long n, int repeat,
		     struct result *res)
{
	double ns[MAX_REPEAT], cpu[MAX_REPEAT], xcpu[MAX_REPEAT];
	double ma[MAX_REPEAT], rq[MAX_REPEAT], rt[MAX_REPEAT];
	double ph[MAX_REPEAT], rss[MAX_REPEAT], drss[MAX_REPEAT];
	int r;

	if (!mwm_pid || mwm_xrm != bc->xrm)
		start_mwm(bc->xrm);
	for (r = 0; r < repeat; r++) {
		unsigned long m0, q0, p0;
		double t0, t1, c0, c1, x0, x1;
		long ops, rss0;

		if (bc->setup)
			bc->setup(n);
		sync_mwm();
		rss0 = mwm_rss_kib();
		m0 = shm->mallocs;
		p0 = shm->replies;
		q0 = shm->requests;
		c0 = cpu_ns(mwm_pid);
		x0 = cpu_ns(server_pid);
		phase_ns = 0;
		t0 = now_ns();
		ops = bc->run(n);
		sync_mwm();
		t1 = now_ns();
		c1 = cpu_ns(mwm_pid);
		x1 = cpu_ns(server_pid);
		if (ops < 1)
			ops = 1;
		/* This includes one restack of the fence window. */
		ns[r] = (t1 - t0) / ops;
		cpu[r] = (c1 - c0) / ops;
		xcpu[r] = (x1 - x0) / ops;
		ph[r] = phase_ns / ops;
		ma[r] = (double)(shm->mallocs - m0) / ops;
		rq[r] = (double)(shm->requests - q0) / ops;
		rt[r] = (double)(shm->replies - p0) / ops;
		rss[r] = (double)mwm_rss_kib();
		drss[r] = rss[r] - rss0;
		if (bc->teardown)
			bc->teardown();
	}
	res->ns = median(ns, repeat);
	res->cpu = median(cpu, repeat);
	res->xcpu = median(xcpu, repeat);
	res->phase = median(ph, repeat);
	res->mallocs = median(ma, repeat);
	res->requests = median(rq, repeat);
	res->rtrips = median(rt, repeat);
	res->rss = median(rss, repeat);
	res->rss_delta = median(drss, repeat);
}

static void usage(FILE *f)
{
	fprintf(f, "usage: mwmbench [-r REPEAT] [-s SCALE] [-j FILE] [-m MWM] "
		   "[-L LOG] [-l] [CASE|all ...]\n");
}

static int selected(const struct bench_case *bc, int argc, char **argv,
		    int first)
{
	int i;

	if (first >= argc)
		return 1;
	for (i = first; i < argc; i++)
		if (!strcmp(argv[i], "all") || !strcmp(argv[i], bc->name) ||
		    !strcmp(argv[i], "mwm"))
			return 1;
	return 0;
}

int main(int argc, char **argv)
{
	int repeat = 5, opt, first, ran = 0, ev, er, maj, min, fd;
	double scale = 1.0;
	const char *json = NULL, *tmp;
	FILE *jf = NULL;
	long maxn = 1;
	size_t i;

	while ((opt = getopt(argc, argv, "r:s:j:m:L:lh")) != -1) {
		switch (opt) {
		case 'r':
			repeat = atoi(optarg);
			if (repeat < 1 || repeat > MAX_REPEAT) {
				fprintf(stderr, "mwmbench: bad repeat count\n");
				return 1;
			}
			break;
		case 's':
			scale = atof(optarg);
			if (!(scale > 0)) {
				fprintf(stderr, "mwmbench: bad scale\n");
				return 1;
			}
			break;
		case 'j':
			json = optarg;
			break;
		case 'm':
			mwm_path = optarg;
			break;
		case 'L':
			snprintf(logpath, sizeof logpath, "%s", optarg);
			keep_log = 1;
			break;
		case 'l':
			for (i = 0; i < N_CASES; i++)
				printf("%-24s %-6s %s\n", cases[i].name, "mwm",
				       cases[i].desc);
			return 0;
		case 'h':
			usage(stdout);
			return 0;
		default:
			usage(stderr);
			return 1;
		}
	}
	first = optind;
	for (opt = first; opt < argc; opt++) {
		int known = !strcmp(argv[opt], "all") ||
			    !strcmp(argv[opt], "mwm");

		for (i = 0; i < N_CASES && !known; i++)
			known = !strcmp(argv[opt], cases[i].name);
		if (!known) {
			fprintf(stderr, "mwmbench: unknown case %s\n",
				argv[opt]);
			return 1;
		}
	}

	if (!getenv("DISPLAY") || !*getenv("DISPLAY") ||
	    !(dpy = XOpenDisplay(NULL))) {
		printf("mwmbench: skipped (no display)\n");
		return EXIT_SKIP;
	}
	if (!XTestQueryExtension(dpy, &ev, &er, &maj, &min)) {
		printf("mwmbench: skipped (no XTEST extension)\n");
		return EXIT_SKIP;
	}
	XSetErrorHandler(x_error);
	rootw = DefaultRootWindow(dpy);
	server_pid = peer_pid(ConnectionNumber(dpy));
	a_utf8 = XInternAtom(dpy, "UTF8_STRING", False);
	a_net_wm_name = XInternAtom(dpy, "_NET_WM_NAME", False);

	tmp = getenv("TMPDIR");
	snprintf(workdir, sizeof workdir, "%s/mwmbench.XXXXXX",
		 tmp && *tmp ? tmp : "/tmp");
	if (!mkdtemp(workdir)) {
		*workdir = '\0';
		die("mkdtemp: %s", strerror(errno));
	}
	if (!keep_log)
		snprintf(logpath, sizeof logpath, "%s/mwm.log", workdir);
	if ((fd = open(logpath, O_WRONLY | O_CREAT | O_TRUNC, 0600)) >= 0)
		close(fd);
	shm_fd = memfd_create("mwmbench", 0);
	if (shm_fd < 0 || ftruncate(shm_fd, sizeof *shm) < 0)
		die("memfd: %s", strerror(errno));
	shm = mmap(NULL, sizeof *shm, PROT_READ | PROT_WRITE, MAP_SHARED,
		   shm_fd, 0);
	if (shm == MAP_FAILED)
		die("mmap: %s", strerror(errno));
	signal(SIGINT, on_signal);
	signal(SIGTERM, on_signal);
	signal(SIGHUP, on_signal);

	for (i = 0; i < N_CASES; i++) {
		long n = (long)(cases[i].n * scale);

		if (n > maxn)
			maxn = n;
	}
	/* the clients, the fence */
	wins = calloc(maxn + 2, sizeof *wins);
	if (!wins)
		die("out of memory");

	if (json) {
		jf = strcmp(json, "-") ? fopen(json, "w") : stdout;
		if (!jf)
			die("%s: %s", json, strerror(errno));
		fprintf(jf, "{\n  \"bench\": \"mwmbench\",\n  \"version\": 1,\n"
			"  \"repeat\": %d,\n  \"scale\": %g,\n"
			"  \"mwm\": \"%s\",\n"
			"  \"counters\": true,\n  \"cases\": [", repeat, scale,
			mwm_path);
	}
	printf("%-18s %6s %11s %10s %10s %8s %8s %6s %11s %6s %5s\n",
	       "case", "n", "ns/op", "cpu-ns/op", "xcpu-ns/op", "mallocs",
	       "requests", "rtrips", "mapped-ns", "rss", "rss+");
	for (i = 0; i < N_CASES; i++) {
		const struct bench_case *bc = &cases[i];
		struct result res;
		long n;

		if (!selected(bc, argc, argv, first))
			continue;
		n = (long)(bc->n * scale);
		if (n < 1)
			n = 1;
		run_case(bc, n, repeat, &res);
		printf("%-18s %6ld %11.1f %10.1f %10.1f %8.3f %8.3f %6.3f",
		       bc->name, n, res.ns, res.cpu, res.xcpu, res.mallocs,
		       res.requests, res.rtrips);
		if (bc->memory)
			printf(" %11.1f %6.0f %5.0f", res.phase, res.rss,
			       res.rss_delta);
		printf("\n");
		fflush(stdout);
		if (jf) {
			fprintf(jf, "%s\n    {\"name\": \"%s\", \"group\": \"mwm\", "
				"\"n\": %ld, \"ns_per_op\": %.2f, "
				"\"cpu_ns_per_op\": %.2f, "
				"\"server_cpu_ns_per_op\": %.2f, "
				"\"mallocs_per_op\": %.4f, "
				"\"requests_per_op\": %.4f, "
				"\"round_trips_per_op\": %.4f",
				ran ? "," : "", bc->name, n, res.ns, res.cpu,
				res.xcpu, res.mallocs, res.requests,
				res.rtrips);
			if (bc->memory)
				fprintf(jf, ", \"mapped_ns_per_op\": %.2f, "
					"\"rss_kib\": %.0f, "
					"\"rss_delta_kib\": %.0f",
					res.phase, res.rss, res.rss_delta);
			fprintf(jf, "}");
		}
		ran++;
	}
	if (jf) {
		fprintf(jf, "\n  ]\n}\n");
		if (jf != stdout)
			fclose(jf);
	}
	cleanup();
	XCloseDisplay(dpy);
	free(wins);
	return 0;
}
