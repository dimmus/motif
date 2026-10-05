/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Tests of xmbench-proxy: the request, reply, error, event and round
 * trip counts it reports, and the latency it adds.
 *
 * The suite starts its own Xvfb, without access control (so that a
 * client reaching it through the proxy over TCP needs no cookie) and
 * the proxy in front of it, and drives real Xlib clients through the
 * proxy.  It exits with 77 (skipped) when Xvfb was not found.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <check.h>

#define EXIT_SKIP 77
#define DELAY_MS 10

struct stats {
	long requests, replies, errors, events, round_trips;
};

static pid_t xvfb_pid, proxy_pid;
static char upstream[64], display[32], stats_path[] = "proxytest-XXXXXX";

static double now_s(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* Read a line from fd, unbuffered: the child keeps the pipe open. */
static int read_line(int fd, char *buf, size_t size)
{
	size_t n = 0;
	char c;

	while (n + 1 < size && read(fd, &c, 1) == 1 && c != '\n')
		buf[n++] = c;
	buf[n] = '\0';
	return n > 0;
}

static pid_t spawn(char *const argv[], int out_fd, int *pipe_rd)
{
	int fds[2];
	pid_t pid;

	if (pipe(fds) < 0)
		return -1;
	pid = fork();
	if (pid == 0) {
		close(fds[0]);
		if (out_fd >= 0) {
			dup2(fds[1], out_fd);
			if (fds[1] != out_fd)
				close(fds[1]);
		}
		execv(argv[0], argv);
		_exit(127);
	}
	close(fds[1]);
	*pipe_rd = fds[0];
	return pid;
}

static void stop(pid_t *pid)
{
	if (*pid > 0) {
		kill(*pid, SIGTERM);
		waitpid(*pid, NULL, 0);
	}
	*pid = 0;
}

static void start_servers(void)
{
	char fdarg[16], line[64], port[16];
	char *xvfb_argv[] = { XVFB, "-displayfd", fdarg, "-screen", "0",
			      "640x480x24", "-nolisten", "tcp", "-noreset",
			      NULL };
	struct stat st;
	int rd, n, tries;

	/* -displayfd: Xvfb picks a free display and writes its number. */
	snprintf(fdarg, sizeof fdarg, "%d", 3);
	xvfb_pid = spawn(xvfb_argv, 3, &rd);
	ck_assert_msg(xvfb_pid > 0, "cannot start %s", XVFB);
	ck_assert_msg(read_line(rd, line, sizeof line), "Xvfb did not start");
	close(rd);
	n = atoi(line);
	snprintf(upstream, sizeof upstream, "/tmp/.X11-unix/X%d", n);
	ck_assert_msg(stat(upstream, &st) == 0, "no socket %s", upstream);

	n = mkstemp(stats_path);
	ck_assert_int_ge(n, 0);
	close(n);

	/* Find a free port 6000 + N for the proxy. */
	for (tries = 0, n = 100 + getpid() % 400; tries < 50; tries++, n++) {
		char delay[16];
		char *proxy_argv[] = { XMBENCH_PROXY, "-d", delay, "-s",
				       stats_path, port, upstream, NULL };

		snprintf(delay, sizeof delay, "%d", DELAY_MS);
		snprintf(port, sizeof port, "%d", n);
		proxy_pid = spawn(proxy_argv, 1, &rd);
		ck_assert_int_gt(proxy_pid, 0);
		if (read_line(rd, line, sizeof line) &&
		    !strcmp(line, "ready")) {
			close(rd);
			snprintf(display, sizeof display, "127.0.0.1:%d", n);
			return;
		}
		close(rd);
		waitpid(proxy_pid, NULL, 0);
		proxy_pid = 0;
	}
	ck_abort_msg("no free port for the proxy");
}

static void stop_servers(void)
{
	stop(&proxy_pid);
	stop(&xvfb_pid);
	unlink(stats_path);
}

static long field(const char *line, const char *name)
{
	char key[64];
	const char *p;

	snprintf(key, sizeof key, "\"%s\": ", name);
	p = strstr(line, key);
	ck_assert_msg(p, "no %s in %s", name, line);
	return atol(p + strlen(key));
}

/* Wait for the stats line of the client that used this TCP port. */
static void get_stats(int port, struct stats *s)
{
	char line[512];
	double end = now_s() + 10;

	for (;;) {
		FILE *f = fopen(stats_path, "r");
		int found = 0;

		ck_assert_ptr_nonnull(f);
		while (!found && fgets(line, sizeof line, f))
			found = field(line, "client_port") == port;
		fclose(f);
		if (found)
			break;
		ck_assert_msg(now_s() < end, "no stats for port %d", port);
		usleep(10000);
	}
	s->requests = field(line, "requests");
	s->replies = field(line, "replies");
	s->errors = field(line, "errors");
	s->events = field(line, "events");
	s->round_trips = field(line, "round_trips");
}

static int local_port(Display *dpy)
{
	struct sockaddr_in sa;
	socklen_t len = sizeof sa;

	ck_assert_int_eq(getsockname(ConnectionNumber(dpy),
				     (struct sockaddr *)&sa, &len), 0);
	return ntohs(sa.sin_port);
}

static int ignore_error(Display *d, XErrorEvent *e)
{
	(void)d;
	(void)e;
	return 0;
}

static Display *open_proxied(void)
{
	Display *dpy;

	setenv("XAUTHORITY", "/dev/null", 1);
	dpy = XOpenDisplay(display);
	ck_assert_msg(dpy, "cannot open %s", display);
	return dpy;
}

#define N_SYNC 10
#define N_ATOMS 8
#define N_NOOP 100
#define N_SEND 5
#define BIG_SIZE (1 << 20)   /* more than a request without BIG-REQUESTS */

/*
 * The work whose counts are checked, after the connection setup.  The
 * expected deltas are given next to each step.
 */
static void work(Display *dpy)
{
	char *names[N_ATOMS], buf[N_ATOMS][32];
	Atom atoms[N_ATOMS];
	unsigned char *data;
	Window win;
	XEvent ev;
	int scr = DefaultScreen(dpy), i;

	/* N_SYNC requests, replies and round trips */
	for (i = 0; i < N_SYNC; i++)
		XSync(dpy, False);

	/* N_ATOMS requests and replies, pipelined: one round trip */
	for (i = 0; i < N_ATOMS; i++) {
		snprintf(buf[i], sizeof buf[i], "PROXYTEST_%d_%d", (int)getpid(),
			 i);
		names[i] = buf[i];
	}
	ck_assert(XInternAtoms(dpy, names, N_ATOMS, False, atoms));

	/* N_NOOP + 1 requests, one reply and round trip */
	for (i = 0; i < N_NOOP; i++)
		XNoOp(dpy);
	XSync(dpy, False);

	/* One request, an error and a round trip */
	XSetErrorHandler(ignore_error);
	ck_assert_ptr_null(XGetAtomName(dpy, (Atom)0x1fffffff));
	XSetErrorHandler(NULL);

	/* 3 requests (one of them a BIG-REQUEST), one reply and round
	 * trip.  (XPutImage would split the data instead.) */
	data = calloc(1, BIG_SIZE);
	ck_assert_ptr_nonnull(data);
	ck_assert_int_gt(XExtendedMaxRequestSize(dpy), BIG_SIZE / 4);
	ck_assert_int_lt(XMaxRequestSize(dpy), BIG_SIZE / 4);
	XChangeProperty(dpy, RootWindow(dpy, scr), atoms[1], XA_STRING, 8,
			PropModeReplace, data, BIG_SIZE);
	XDeleteProperty(dpy, RootWindow(dpy, scr), atoms[1]);
	XSync(dpy, False);
	free(data);

	/* N_SEND + 3 requests, N_SEND events, one reply and round trip:
	 * with an empty event mask, the event goes to the window's
	 * creator. */
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 0, 0, 10, 10, 0,
				  0, 0);
	memset(&ev, 0, sizeof ev);
	ev.xclient.type = ClientMessage;
	ev.xclient.window = win;
	ev.xclient.message_type = atoms[0];
	ev.xclient.format = 32;
	for (i = 0; i < N_SEND; i++)
		XSendEvent(dpy, win, False, 0, &ev);
	XDestroyWindow(dpy, win);
	XSync(dpy, False);
	ck_assert_int_eq(XPending(dpy), N_SEND);
}

/*
 * Run a client in a child process, which must exit with 0; returns the
 * TCP port it connected from.
 */
static int run_client(int with_work, int clean_exit)
{
	int fds[2], port = 0, status;
	pid_t pid;

	ck_assert_int_eq(pipe(fds), 0);
	pid = fork();
	ck_assert_int_ge(pid, 0);
	if (pid == 0) {
		Display *dpy = open_proxied();

		port = local_port(dpy);
		if (write(fds[1], &port, sizeof port) != sizeof port)
			_exit(1);
		XSync(dpy, False);
		if (with_work)
			work(dpy);
		if (clean_exit)
			XCloseDisplay(dpy);
		_exit(0);
	}
	close(fds[1]);
	ck_assert_int_eq(read(fds[0], &port, sizeof port), sizeof port);
	close(fds[0]);
	ck_assert_int_eq(waitpid(pid, &status, 0), pid);
	ck_assert_msg(WIFEXITED(status) && WEXITSTATUS(status) == 0,
		      "client failed");
	return port;
}

START_TEST(test_counts)
{
	struct stats base, run;

	get_stats(run_client(0, 1), &base);
	get_stats(run_client(1, 1), &run);

	ck_assert_int_eq(run.requests - base.requests,
			 N_SYNC + N_ATOMS + N_NOOP + 1 + 1 + 3 + N_SEND + 3);
	ck_assert_int_eq(run.replies - base.replies, N_SYNC + N_ATOMS + 3);
	ck_assert_int_eq(run.errors - base.errors, 1);
	ck_assert_int_eq(run.events - base.events, N_SEND);
	ck_assert_int_eq(run.round_trips - base.round_trips, N_SYNC + 5);
	/* The connection setup itself waits for the server. */
	ck_assert_int_ge(base.round_trips, 1);
}
END_TEST

/* Each round trip costs twice the delay, a pipelined batch only once. */
START_TEST(test_latency)
{
	char *names[50], buf[50][32];
	Atom atoms[50];
	Display *dpy = open_proxied();
	double t;
	int i;

	XSync(dpy, False);
	t = now_s();
	for (i = 0; i < N_SYNC; i++)
		XSync(dpy, False);
	t = now_s() - t;
	ck_assert_msg(t >= N_SYNC * 2 * DELAY_MS / 1e3,
		      "%d round trips took %.3f s", N_SYNC, t);

	for (i = 0; i < 50; i++) {
		snprintf(buf[i], sizeof buf[i], "PROXYLAT_%d_%d", (int)getpid(),
			 i);
		names[i] = buf[i];
	}
	t = now_s();
	ck_assert(XInternAtoms(dpy, names, 50, False, atoms));
	t = now_s() - t;
	ck_assert_msg(t >= 2 * DELAY_MS / 1e3, "no latency: %.3f s", t);
	ck_assert_msg(t < 25 * 2 * DELAY_MS / 1e3,
		      "50 pipelined requests took %.3f s", t);
	XCloseDisplay(dpy);
}
END_TEST

/* A client that dies without closing still gets its stats line, and the
 * proxy keeps serving. */
START_TEST(test_abrupt_close)
{
	struct stats s;

	get_stats(run_client(1, 0), &s);
	ck_assert_int_ge(s.round_trips, N_SYNC + 5);
	get_stats(run_client(0, 1), &s);
	ck_assert_int_ge(s.replies, 1);
}
END_TEST

int main(void)
{
	Suite *s;
	TCase *tc;
	SRunner *sr;
	int failed;

	if (!*XVFB) {
		printf("# SKIP Proxy: Xvfb was not found\n");
		return EXIT_SKIP;
	}
	s = suite_create("Proxy");
	tc = tcase_create("proxy");
	tcase_add_unchecked_fixture(tc, start_servers, stop_servers);
	tcase_set_timeout(tc, 60);
	tcase_add_test(tc, test_counts);
	tcase_add_test(tc, test_latency);
	tcase_add_test(tc, test_abrupt_close);
	suite_add_tcase(s, tc);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
