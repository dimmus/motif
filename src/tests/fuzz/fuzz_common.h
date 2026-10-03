/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Helpers shared by the libFuzzer targets.
 */
#ifndef FUZZ_COMMON_H
#define FUZZ_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Intrinsic.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);
int LLVMFuzzerInitialize(int *argc, char ***argv);

/* A NUL-terminated copy of the input */
static inline char *fuzz_strdup(const uint8_t *data, size_t size)
{
	char *s = malloc(size + 1);

	if (s) {
		memcpy(s, data, size);
		s[size] = '\0';
	}
	return s;
}

/* A read-only stream over the input */
static inline FILE *fuzz_fmemopen(const uint8_t *data, size_t size)
{
	/* fmemopen() rejects empty buffers */
	static const uint8_t empty[1];

	return fmemopen((void *)(size ? data : empty), size ? size : 1, "rb");
}

/*
 * Write the input to a file of this process (in /dev/shm when it
 * exists) and return its name, for interfaces that take a file name.
 */
static inline const char *fuzz_write_file(const char *suffix,
					  const uint8_t *data, size_t size)
{
	static char path[256];
	FILE *f;

	if (!path[0])
		snprintf(path, sizeof path, "%s/motif-fuzz-%ld%s",
			 access("/dev/shm", W_OK) == 0 ? "/dev/shm" : "/tmp",
			 (long)getpid(), suffix);
	f = fopen(path, "wb");
	if (!f)
		abort();
	if (size && fwrite(data, 1, size, f) != size)
		abort();
	fclose(f);
	return path;
}

/*
 * The targets that need an X server share one application context and
 * shell; they are run under xvfb-run.  Without a display they cannot
 * do anything useful, so they stop.
 */
static XtAppContext fuzz_app;
static Widget fuzz_top;

static inline int fuzz_x_error(Display *dpy, XErrorEvent *ev)
{
	/* Bad values from the input end up in requests; that is fine. */
	(void)dpy;
	(void)ev;
	return 0;
}

static inline void fuzz_xt_warning(String msg)
{
	(void)msg;
}

static inline Widget fuzz_open_display(const char *name)
{
	int argc = 1;
	char *argv[] = { (char *)name, NULL };

	if (fuzz_top)
		return fuzz_top;
	if (!getenv("DISPLAY") || !*getenv("DISPLAY")) {
		fprintf(stderr, "%s: needs an X server, run it under xvfb-run\n",
			name);
		exit(1);
	}
	XtSetLanguageProc(NULL, NULL, NULL);
	fuzz_top = XtAppInitialize(&fuzz_app, "MotifFuzz", NULL, 0, &argc,
				   argv, NULL, NULL, 0);
	XtAppSetWarningHandler(fuzz_app, fuzz_xt_warning);
	XSetErrorHandler(fuzz_x_error);
	return fuzz_top;
}

/* Process what is pending */
static inline void fuzz_pump(void)
{
	Display *dpy = XtDisplay(fuzz_top);

	XSync(dpy, False);
	while (XtAppPending(fuzz_app))
		XtAppProcessEvent(fuzz_app, XtIMAll);
}

#endif /* FUZZ_COMMON_H */
