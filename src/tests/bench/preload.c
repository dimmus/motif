/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * LD_PRELOAD counters for xmbench.
 *
 * xmbench re-executes itself with this library preloaded.  It counts
 * calls to the allocator, waits for a reply from the server (round
 * trips) and calls to XSetICValues (which can be synchronous with an
 * input method server).  xmbench finds the counters with
 * dlsym(RTLD_DEFAULT, ...), so it still runs, without these numbers,
 * when the library is not loaded.
 *
 * The round trips are counted in libxcb: every Xlib call that waits for
 * a reply, XSync included, goes through xcb_wait_for_reply64 (or
 * xcb_wait_for_reply), from _XReply, after it sent its requests with
 * xcb_writev.  A wait counts as a round trip when requests were sent
 * since the last one: the waits that _XReply makes for the replies of
 * earlier requests with asynchronous handlers, which XInternAtoms or
 * XGetWindowAttributes use to get several replies in one round trip, do
 * not.  This is what counting the calls to _XReply gives, without having
 * to interpose a function that libX11 calls internally, which does not
 * work where it is linked with -Bsymbolic-functions (Debian, Ubuntu).
 *
 * With XMBENCH_REPLY_BACKTRACE set, every round trip prints a backtrace
 * to stderr, to find where the round trips of a case come from
 * (tools/dev/profile/rtrips.py summarises them).
 *
 * With XMBENCH_REPORT set, the counters are printed to stderr when the
 * process exits, so that the library also counts for programs other
 * than xmbench (tools/dev/profile preloads it into hello_motif and
 * mwm).
 *
 * The allocator wrappers forward to the glibc __libc_* entry points
 * rather than to dlsym(RTLD_NEXT, ...), since dlsym itself allocates.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <execinfo.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>

#include <X11/Xlib.h>

extern void *__libc_malloc(size_t);
extern void *__libc_calloc(size_t, size_t);
extern void *__libc_realloc(void *, size_t);
extern void __libc_free(void *);

unsigned long xmbench_mallocs;
unsigned long xmbench_frees;
unsigned long xmbench_replies;
unsigned long xmbench_icvalues;

void *malloc(size_t size)
{
	xmbench_mallocs++;
	return __libc_malloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
	xmbench_mallocs++;
	return __libc_calloc(nmemb, size);
}

void *realloc(void *ptr, size_t size)
{
	/* Count only the reallocs that may move the block. */
	if (size)
		xmbench_mallocs++;
	return __libc_realloc(ptr, size);
}

void free(void *ptr)
{
	if (ptr)
		xmbench_frees++;
	__libc_free(ptr);
}

static void report(void) __attribute__((destructor));

static void report(void)
{
	if (getenv("XMBENCH_REPORT"))
		fprintf(stderr, "xmbench_preload: mallocs %lu frees %lu "
			"rtrips %lu icvalues %lu\n", xmbench_mallocs,
			xmbench_frees, xmbench_replies, xmbench_icvalues);
}

/* Requests were sent since the last round trip. */
static int sent;

static void count_reply(void)
{
	static int trace = -1;

	if (!sent)
		return;
	sent = 0;

	if (trace < 0)
		trace = getenv("XMBENCH_REPLY_BACKTRACE") != NULL;
	if (trace) {
		void *bt[32];

		backtrace_symbols_fd(bt, backtrace(bt, 32), 2);
		if (write(2, "--\n", 3) < 0)
			trace = 0;
	}
	xmbench_replies++;
}

/*
 * int xcb_writev(xcb_connection_t *, struct iovec *, int, uint64_t),
 * void *xcb_wait_for_reply(xcb_connection_t *, unsigned int,
 *                          xcb_generic_error_t **)
 * and its version with a 64-bit sequence number, from <xcb/xcbext.h>
 * and <xcb/xcb.h>.
 */
typedef int (*writev_fn)(void *, struct iovec *, int, unsigned long long);
typedef void *(*wait_fn)(void *, unsigned int, void **);
typedef void *(*wait64_fn)(void *, unsigned long long, void **);

int xcb_writev(void *c, struct iovec *vector, int count,
	       unsigned long long requests);
void *xcb_wait_for_reply(void *c, unsigned int request, void **e);
void *xcb_wait_for_reply64(void *c, unsigned long long request, void **e);

int xcb_writev(void *c, struct iovec *vector, int count,
	       unsigned long long requests)
{
	static writev_fn real;

	if (!real)
		real = (writev_fn)dlsym(RTLD_NEXT, "xcb_writev");
	if (requests)
		sent = 1;
	return real(c, vector, count, requests);
}

void *xcb_wait_for_reply(void *c, unsigned int request, void **e)
{
	static wait_fn real;

	if (!real)
		real = (wait_fn)dlsym(RTLD_NEXT, "xcb_wait_for_reply");
	count_reply();
	return real(c, request, e);
}

void *xcb_wait_for_reply64(void *c, unsigned long long request, void **e)
{
	static wait64_fn real;

	if (!real)
		real = (wait64_fn)dlsym(RTLD_NEXT, "xcb_wait_for_reply64");
	count_reply();
	return real(c, request, e);
}

/*
 * XSetICValues takes a NULL terminated list of name/value pairs.
 * Forward up to 15 pairs, which is more than any caller in Motif passes
 * (it uses XNVaNestedList for anything longer than a few).
 */
#define MAX_IC_ARGS 31

typedef char *(*seticvalues_fn)(XIC, ...);

char *XSetICValues(XIC ic, ...)
{
	static seticvalues_fn real;
	void *a[MAX_IC_ARGS] = { NULL };
	va_list ap;
	int n;

	if (!real)
		real = (seticvalues_fn)dlsym(RTLD_NEXT, "XSetICValues");
	va_start(ap, ic);
	for (n = 0; n < MAX_IC_ARGS - 1; n += 2) {
		if (!(a[n] = va_arg(ap, void *)))
			break;
		a[n + 1] = va_arg(ap, void *);
	}
	va_end(ap);
	xmbench_icvalues++;
	return real(ic, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7],
		    a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15],
		    a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23],
		    a[24], a[25], a[26], a[27], a[28], a[29], a[30]);
}
