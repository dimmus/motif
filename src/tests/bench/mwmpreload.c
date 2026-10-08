/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * LD_PRELOAD probe for mwmbench, loaded into the mwm under test.
 *
 * mwmbench passes a memfd in MWMBENCH_SHM_FD; this library maps it and
 * keeps there (see struct mwmbench_shm in mwmbench.h):
 *   - the calls to the allocator and to _XReply (round trips), as the
 *     xmbench preload does;
 *   - the number of X requests mwm had issued when it last went idle;
 *   - a sequence number bumped, with a futex wake, every time mwm blocks
 *     in the Xt event loop with nothing left to do, so that mwmbench can
 *     tell when mwm has finished processing what it was sent;
 *   - the position the last XQueryPointer returned, also with a futex
 *     wake: mwm queries the pointer once for every motion hint during an
 *     interactive move or resize, which is how mwmbench follows a drag
 *     step by step while mwm holds its own modal event loop;
 *   - a sequence number bumped, with a futex wake, by every successful
 *     XGrabPointer: mwm grabs the pointer when a drag passes its move
 *     threshold, and only then reports motion as hints.
 * Without MWMBENCH_SHM_FD the counters go to a private copy, so mwm runs
 * normally.
 *
 * With MWMBENCH_REPLY_BACKTRACE set, every _XReply prints a backtrace to
 * stderr (mwm's log), to find where the round trips of a case come from.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <execinfo.h>
#include <limits.h>
#include <poll.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/futex.h>

#include <X11/Xlib.h>

#include "mwmbench.h"

extern void *__libc_malloc(size_t);
extern void *__libc_calloc(size_t, size_t);
extern void *__libc_realloc(void *, size_t);
extern void __libc_free(void *);

static struct mwmbench_shm private_shm;
static struct mwmbench_shm *shm = &private_shm;

#define MAX_DISPLAYS 4
static Display *displays[MAX_DISPLAYS];

__attribute__((constructor))
static void mwmbench_preload_init(void)
{
	const char *s = getenv("MWMBENCH_SHM_FD");
	void *p;
	int fd;

	if (!s || !*s)
		return;
	fd = atoi(s);
	p = mmap(NULL, sizeof *shm, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	unsetenv("MWMBENCH_SHM_FD");
	if (p != MAP_FAILED)
		shm = p;
}

static void wake(unsigned int *word)
{
	syscall(SYS_futex, word, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
}

void *malloc(size_t size)
{
	shm->mallocs++;
	return __libc_malloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
	shm->mallocs++;
	return __libc_calloc(nmemb, size);
}

void *realloc(void *ptr, size_t size)
{
	/* Count only the reallocs that may move the block. */
	if (size)
		shm->mallocs++;
	return __libc_realloc(ptr, size);
}

void free(void *ptr)
{
	__libc_free(ptr);
}

/* Status _XReply(Display *, xReply *, int, Bool), from Xlibint.h */
typedef int (*xreply_fn)(Display *, void *, int, int);

int _XReply(Display *dpy, void *rep, int extra, int discard);

int _XReply(Display *dpy, void *rep, int extra, int discard)
{
	static xreply_fn real;
	static int trace = -1;

	if (!real)
		real = (xreply_fn)dlsym(RTLD_NEXT, "_XReply");
	if (trace < 0)
		trace = getenv("MWMBENCH_REPLY_BACKTRACE") != NULL;
	if (trace) {
		void *bt[32];

		backtrace_symbols_fd(bt, backtrace(bt, 32), 2);
		if (write(2, "--\n", 3) < 0)
			trace = 0;
	}
	shm->replies++;
	return real(dpy, rep, extra, discard);
}

typedef Display *(*opendisplay_fn)(const char *);

Display *XOpenDisplay(const char *name)
{
	static opendisplay_fn real;
	Display *dpy;
	int i;

	if (!real)
		real = (opendisplay_fn)dlsym(RTLD_NEXT, "XOpenDisplay");
	dpy = real(name);
	for (i = 0; dpy && i < MAX_DISPLAYS; i++)
		if (!displays[i]) {
			displays[i] = dpy;
			break;
		}
	return dpy;
}

typedef Bool (*querypointer_fn)(Display *, Window, Window *, Window *,
				int *, int *, int *, int *, unsigned int *);

Bool XQueryPointer(Display *dpy, Window w, Window *root, Window *child,
		   int *root_x, int *root_y, int *win_x, int *win_y,
		   unsigned int *mask)
{
	static querypointer_fn real;
	Bool ret;

	if (!real)
		real = (querypointer_fn)dlsym(RTLD_NEXT, "XQueryPointer");
	ret = real(dpy, w, root, child, root_x, root_y, win_x, win_y, mask);
	if (ret) {
		__atomic_store_n(&shm->query_x, *root_x, __ATOMIC_RELAXED);
		__atomic_store_n(&shm->query_y, *root_y, __ATOMIC_RELAXED);
	}
	__atomic_add_fetch(&shm->query_seq, 1, __ATOMIC_RELEASE);
	wake(&shm->query_seq);
	return ret;
}

/*
 * Whether a poll() call comes from libXt (its event loop) rather than
 * from xcb waiting for a reply or from mwm itself.  The answer for each
 * call site is cached.
 */
static int from_xt(void *caller)
{
	static void *xt_base;
	static struct { void *addr; int xt; } cache[8];
	static int ncache;
	Dl_info info;
	int i, xt;

	for (i = 0; i < ncache; i++)
		if (cache[i].addr == caller)
			return cache[i].xt;
	if (!xt_base) {
		void *sym = dlsym(RTLD_DEFAULT, "XtAppNextEvent");

		if (sym && dladdr(sym, &info))
			xt_base = info.dli_fbase;
	}
	xt = xt_base && dladdr(caller, &info) && info.dli_fbase == xt_base;
	if (ncache < 8) {
		cache[ncache].addr = caller;
		cache[ncache].xt = xt;
		ncache++;
	}
	return xt;
}

typedef int (*poll_fn)(struct pollfd *, nfds_t, int);

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	static poll_fn real;
	int idle, ret, i;

	if (!real)
		real = (poll_fn)dlsym(RTLD_NEXT, "poll");
	idle = timeout != 0 && from_xt(__builtin_return_address(0));
	if (idle) {
		unsigned long requests = 0;

		for (i = 0; i < MAX_DISPLAYS && displays[i]; i++)
			requests += XNextRequest(displays[i]) - 1;
		shm->requests = requests;
		__atomic_store_n(&shm->idle, 1, __ATOMIC_RELAXED);
		__atomic_add_fetch(&shm->idle_seq, 1, __ATOMIC_RELEASE);
		wake(&shm->idle_seq);
	}
	ret = real(fds, nfds, timeout);
	if (idle)
		__atomic_store_n(&shm->idle, 0, __ATOMIC_RELAXED);
	return ret;
}

typedef int (*grabpointer_fn)(Display *, Window, Bool, unsigned int, int,
			      int, Window, Cursor, Time);

int XGrabPointer(Display *dpy, Window w, Bool owner_events,
		 unsigned int event_mask, int pointer_mode, int keyboard_mode,
		 Window confine_to, Cursor cursor, Time time)
{
	static grabpointer_fn real;
	int ret;

	if (!real)
		real = (grabpointer_fn)dlsym(RTLD_NEXT, "XGrabPointer");
	ret = real(dpy, w, owner_events, event_mask, pointer_mode,
		   keyboard_mode, confine_to, cursor, time);
	if (ret == GrabSuccess) {
		__atomic_add_fetch(&shm->grab_seq, 1, __ATOMIC_RELEASE);
		wake(&shm->grab_seq);
	}
	return ret;
}
