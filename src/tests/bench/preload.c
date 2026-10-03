/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * LD_PRELOAD counters for xmbench.
 *
 * xmbench re-executes itself with this library preloaded.  It counts
 * calls to the allocator, to _XReply (every Xlib call that waits for a
 * reply from the server goes through it, XSync included, so this is the
 * number of round trips) and to XSetICValues (which can be synchronous
 * with an input method server).  xmbench finds the counters with
 * dlsym(RTLD_DEFAULT, ...), so it still runs, without these numbers,
 * when the library is not loaded.
 *
 * With XMBENCH_REPLY_BACKTRACE set, every _XReply prints a backtrace to
 * stderr, to find where the round trips of a case come from.
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
#include <stdlib.h>
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

/* Status _XReply(Display *, xReply *, int, Bool), from Xlibint.h */
typedef int (*xreply_fn)(Display *, void *, int, int);

int _XReply(Display *dpy, void *rep, int extra, int discard)
{
	static xreply_fn real;
	static int trace = -1;

	if (!real)
		real = (xreply_fn)dlsym(RTLD_NEXT, "_XReply");
	if (trace < 0)
		trace = getenv("XMBENCH_REPLY_BACKTRACE") != NULL;
	if (trace) {
		void *bt[32];

		backtrace_symbols_fd(bt, backtrace(bt, 32), 2);
		if (write(2, "--\n", 3) < 0)
			trace = 0;
	}
	xmbench_replies++;
	return real(dpy, rep, extra, discard);
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
