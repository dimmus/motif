/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * xclients: the window manager workload of doc/profiling.md.
 *
 *   xclients [-n WINDOWS] [-t TITLES] [-c CONNECTIONS]
 *
 * Maps WINDOWS top-level windows (default 200), spread over CONNECTIONS
 * client connections (default 1; the window manager does not care which
 * connection a window belongs to, but its client list is per window),
 * waits until a window manager has reparented every one of them, then
 * changes the title of the windows TITLES times in total (default 0),
 * round robin, and unmaps and destroys them one by one, waiting each
 * time for the window manager to give the window back.  It prints the
 * time of each phase and exits with 1 if the window manager did not
 * react within 30 s.
 *
 * Only Xlib is used, so the profile of the window manager is not mixed
 * with that of a Motif client.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define TIMEOUT_MS 30000

static double now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* Wait for an event of TYPE on any of the N displays; 0 on timeout. */
static int wait_event(Display **dpys, int n, int type, XEvent *ev)
{
	double end = now() + TIMEOUT_MS / 1000.0;
	int i;

	for (;;) {
		struct pollfd pfd[64];

		for (i = 0; i < n; i++) {
			while (XPending(dpys[i])) {
				XNextEvent(dpys[i], ev);
				if (ev->type == type)
					return 1;
			}
		}
		for (i = 0; i < n; i++) {
			pfd[i].fd = ConnectionNumber(dpys[i]);
			pfd[i].events = POLLIN;
		}
		if (now() > end ||
		    poll(pfd, n, (int)((end - now()) * 1000) + 1) < 0)
			return 0;
	}
}

int main(int argc, char **argv)
{
	int nwin = 200, ntitles = 0, ndpy = 1, opt, i, done;
	Display *dpys[64];
	Window *wins;
	XEvent ev;
	double t0, t1, t2, t3;
	char title[64];

	while ((opt = getopt(argc, argv, "n:t:c:")) != -1) {
		switch (opt) {
		case 'n':
			nwin = atoi(optarg);
			break;
		case 't':
			ntitles = atoi(optarg);
			break;
		case 'c':
			ndpy = atoi(optarg);
			break;
		default:
			fprintf(stderr, "usage: xclients [-n WINDOWS] "
				"[-t TITLES] [-c CONNECTIONS]\n");
			return 2;
		}
	}
	if (nwin < 1 || ndpy < 1 || ndpy > 64 || ntitles < 0) {
		fprintf(stderr, "xclients: bad arguments\n");
		return 2;
	}
	for (i = 0; i < ndpy; i++) {
		if (!(dpys[i] = XOpenDisplay(NULL))) {
			fprintf(stderr, "xclients: cannot open display\n");
			return 2;
		}
	}
	wins = calloc(nwin, sizeof *wins);

	t0 = now();
	for (i = 0; i < nwin; i++) {
		Display *d = dpys[i % ndpy];
		XSizeHints hints;

		wins[i] = XCreateSimpleWindow(d, DefaultRootWindow(d),
					      (i * 7) % 600, (i * 5) % 400,
					      200, 100, 1,
					      BlackPixel(d, DefaultScreen(d)),
					      WhitePixel(d, DefaultScreen(d)));
		snprintf(title, sizeof title, "client %d", i);
		XStoreName(d, wins[i], title);
		/* A user-specified position, so that mwm does not wait for
		 * interactive placement. */
		hints.flags = USPosition | USSize;
		hints.x = (i * 7) % 600;
		hints.y = (i * 5) % 400;
		hints.width = 200;
		hints.height = 100;
		XSetWMNormalHints(d, wins[i], &hints);
		XSelectInput(d, wins[i], StructureNotifyMask);
		XMapWindow(d, wins[i]);
	}
	for (i = 0; i < ndpy; i++)
		XFlush(dpys[i]);
	for (done = 0; done < nwin; done++) {
		if (!wait_event(dpys, ndpy, ReparentNotify, &ev)) {
			fprintf(stderr, "xclients: %d of %d windows reparented "
				"in %d ms\n", done, nwin, TIMEOUT_MS);
			return 1;
		}
	}
	t1 = now();

	for (i = 0; i < ntitles; i++) {
		Display *d = dpys[(i % nwin) % ndpy];

		snprintf(title, sizeof title, "client %d, title %d",
			 i % nwin, i);
		XStoreName(d, wins[i % nwin], title);
		if (i % 100 == 99)
			XSync(d, False);
	}
	for (i = 0; i < ndpy; i++)
		XSync(dpys[i], False);
	t2 = now();

	/* Withdraw: the window manager reparents each window back to the
	 * root when it unmaps. */
	for (i = 0; i < nwin; i++) {
		Display *d = dpys[i % ndpy];

		XUnmapWindow(d, wins[i]);
		XFlush(d);
		do {
			if (!wait_event(dpys, ndpy, ReparentNotify, &ev)) {
				fprintf(stderr, "xclients: window %d not "
					"released\n", i);
				return 1;
			}
		} while (ev.xreparent.window != wins[i]);
		XDestroyWindow(d, wins[i]);
	}
	for (i = 0; i < ndpy; i++)
		XSync(dpys[i], False);
	t3 = now();

	printf("map+reparent %d windows: %.1f ms\n", nwin, (t1 - t0) * 1e3);
	printf("%d title changes: %.1f ms\n", ntitles, (t2 - t1) * 1e3);
	printf("withdraw %d windows: %.1f ms\n", nwin, (t3 - t2) * 1e3);
	for (i = 0; i < ndpy; i++)
		XCloseDisplay(dpys[i]);
	free(wins);
	return 0;
}
