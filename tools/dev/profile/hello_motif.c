/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * hello_motif: the startup probe of doc/profiling.md.
 *
 *   hello_motif [-n COUNT] [Xt options]
 *
 * Opens the display, creates an application shell with a RowColumn, a
 * Label, a PushButton and a Text, realizes it, handles the events until
 * the window is drawn, and exits.  Timing it from the shell (perf stat,
 * LD_DEBUG=statistics) gives the cost of starting a small Motif
 * program: loading and relocating the libraries, class initialization,
 * resource conversion and the round trips to the server.
 *
 * With -n COUNT it instead creates and destroys the widgets COUNT times
 * in one process, which leaves out the dynamic loader and the class
 * initialization, for comparison.
 *
 * It uses only the Motif 2.x API, so it builds against older releases
 * (2.3.8, 2.4.1) for before/after comparisons.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>

static void drain(XtAppContext app, Display *dpy)
{
	/* An XSync answers only once the server has handled every request,
	 * so after it the events of the realize and map are all queued. */
	for (;;) {
		XSync(dpy, False);
		if (!XtAppPending(app))
			break;
		while (XtAppPending(app))
			XtAppProcessEvent(app, XtIMAll);
	}
}

static Widget create(Widget top)
{
	Widget rc;
	XmString s;

	rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, top, NULL);
	s = XmStringCreateLocalized("Hello, Motif");
	XtVaCreateManagedWidget("label", xmLabelWidgetClass, rc,
				XmNlabelString, s, NULL);
	XmStringFree(s);
	s = XmStringCreateLocalized("Quit");
	XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, rc,
				XmNlabelString, s, NULL);
	XmStringFree(s);
	XtVaCreateManagedWidget("text", xmTextWidgetClass, rc,
				XmNvalue, "Some text", XmNcolumns, 20, NULL);
	return rc;
}

int main(int argc, char **argv)
{
	XtAppContext app;
	Widget top, rc;
	long i, count = 0;

	XtSetLanguageProc(NULL, NULL, NULL);
	top = XtVaOpenApplication(&app, "HelloMotif", NULL, 0, &argc, argv,
				  NULL, applicationShellWidgetClass, NULL);
	if (argc == 3 && !strcmp(argv[1], "-n"))
		count = atol(argv[2]);
	else if (argc != 1) {
		fprintf(stderr, "usage: hello_motif [-n COUNT]\n");
		return 1;
	}

	rc = create(top);
	XtRealizeWidget(top);
	drain(app, XtDisplay(top));
	for (i = 0; i < count; i++) {
		XtDestroyWidget(rc);
		rc = create(top);
		drain(app, XtDisplay(top));
	}
	return 0;
}
