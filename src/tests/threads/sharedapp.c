/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * sharedapp: two threads share one XtAppContext and Display, as Xt
 * allows, and create and destroy widgets in it at the same time.  One
 * thread uses the simple check and radio box functions, which fetch
 * subresources without holding the application lock themselves; the
 * other creates buttons, whose initialization takes the process lock
 * under the application lock.  Motif must take the two locks in the
 * same order (application, then process) everywhere, or the threads
 * deadlock; a watchdog turns a deadlock into a failure.
 *
 *   sharedapp [iterations]
 *
 * Exits 0 on success, 1 on a failure or a deadlock, 77 without a
 * display.
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>

#define WATCHDOG_SECONDS 300

static Widget form;
static int iterations = 3000;
static int failures;
static pthread_mutex_t state_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t state_cond = PTHREAD_COND_INITIALIZER;
static int boxes_done, threads_done;

static int boxes_finished(void)
{
	int done;

	pthread_mutex_lock(&state_lock);
	done = boxes_done;
	pthread_mutex_unlock(&state_lock);
	return done;
}

static void thread_finished(void)
{
	pthread_mutex_lock(&state_lock);
	threads_done++;
	pthread_cond_signal(&state_cond);
	pthread_mutex_unlock(&state_lock);
}

static int num_children(Widget w)
{
	Cardinal n = 0;

	XtVaGetValues(w, XmNnumChildren, &n, NULL);
	return (int)n;
}

/* XmCreateSimpleCheckBox and XmCreateSimpleRadioBox fetch their
 * subresources (XmNbuttonCount, ...) with no lock of their own. */
static void *boxes(void *arg)
{
	Arg args[1];
	Widget box;
	int i, bad = 0;

	(void)arg;
	XtSetArg(args[0], XmNbuttonCount, 3);
	for (i = 0; i < iterations; i++) {
		box = (i & 1) ? XmCreateSimpleRadioBox(form, "radio", args, 1)
			      : XmCreateSimpleCheckBox(form, "check", args, 1);
		if (num_children(box) != 3)
			bad++;
		XtDestroyWidget(box);
	}
	pthread_mutex_lock(&state_lock);
	failures += bad;
	boxes_done = 1;
	pthread_mutex_unlock(&state_lock);
	thread_finished();
	return NULL;
}

/* Buttons and labels: their initialization switches the class
 * translations under the process lock. */
static void *buttons(void *arg)
{
	int i;

	(void)arg;
	for (i = 0; !boxes_finished(); i++) {
		Widget w;

		switch (i % 3) {
		case 0:
			w = XmCreatePushButton(form, "push", NULL, 0);
			break;
		case 1:
			w = XmCreateToggleButton(form, "toggle", NULL, 0);
			break;
		default:
			w = XmCreateLabel(form, "label", NULL, 0);
			break;
		}
		XtDestroyWidget(w);
	}
	thread_finished();
	return NULL;
}

int main(int argc, char **argv)
{
	char *argv0[] = {"sharedapp", NULL};
	int xargc = 1;
	XtAppContext app;
	Display *dpy;
	Widget shell;
	pthread_t t1, t2;
	struct timespec deadline;
	int rc = 0;

	if (argc > 1)
		iterations = atoi(argv[1]);
	if (iterations < 1) {
		fprintf(stderr, "usage: sharedapp [iterations]\n");
		return 1;
	}
	if (!XInitThreads() || !XtToolkitThreadInitialize()) {
		fprintf(stderr, "sharedapp: Xlib or Xt has no thread support\n");
		return 77;
	}
	XtToolkitInitialize();
	app = XtCreateApplicationContext();
	dpy = XtOpenDisplay(app, NULL, "sharedapp", "SharedApp", NULL, 0, &xargc, argv0);
	if (!dpy) {
		fprintf(stderr, "sharedapp: no display, skipped\n");
		return 77;
	}
	shell = XtAppCreateShell("sharedapp", "SharedApp", applicationShellWidgetClass, dpy,
				 NULL, 0);
	form = XmCreateForm(shell, "form", NULL, 0);
	XtManageChild(form);
	XtRealizeWidget(shell);

	if (pthread_create(&t1, NULL, boxes, NULL) != 0 ||
	    pthread_create(&t2, NULL, buttons, NULL) != 0) {
		fprintf(stderr, "sharedapp: pthread_create failed\n");
		return 1;
	}
	clock_gettime(CLOCK_REALTIME, &deadline);
	deadline.tv_sec += WATCHDOG_SECONDS;
	pthread_mutex_lock(&state_lock);
	while (threads_done < 2 && rc != ETIMEDOUT)
		rc = pthread_cond_timedwait(&state_cond, &state_lock, &deadline);
	pthread_mutex_unlock(&state_lock);
	if (rc == ETIMEDOUT) {
		/* The threads hold the locks: do not try to clean up. */
		fprintf(stderr, "sharedapp: the threads did not finish in %d s: deadlock\n",
			WATCHDOG_SECONDS);
		_exit(1);
	}
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	XtDestroyWidget(shell);
	XtCloseDisplay(dpy);
	XtDestroyApplicationContext(app);
	printf("sharedapp: %d iterations: %s\n", iterations, failures ? "FAILED" : "ok");
	return failures ? 1 : 0;
}
