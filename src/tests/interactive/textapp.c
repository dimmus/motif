/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * textapp: a window with an XmTextField and an XmText for the tests
 * that drive real input with xdotool (text_xdotool.sh).
 *
 *   textapp [Xt options, e.g. -geometry +0+0 -title A]
 *
 * Prints "ready" once the window is mapped and, on SIGTERM, the
 * contents of both widgets ("tf=...", "text=...", newlines in the
 * Text as \n) before it exits.  Exits 77 without a display.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

static XtAppContext app;
static XtSignalId term_id;
static Widget tf, text;

static void on_term(int sig)
{
	(void)sig;
	XtNoticeSignal(term_id);
}

static void print_value(const char *name, char *value)
{
	char *p;

	printf("%s=", name);
	for (p = value; p && *p; p++) {
		if (*p == '\n')
			fputs("\\n", stdout);
		else
			putchar(*p);
	}
	putchar('\n');
	XtFree(value);
}

static void report_and_exit(XtPointer client, XtSignalId *id)
{
	(void)client;
	(void)id;
	print_value("tf", XmTextFieldGetString(tf));
	print_value("text", XmTextGetString(text));
	fflush(stdout);
	exit(0);
}

static void mapped(Widget w, XtPointer client, XEvent *ev, Boolean *cont)
{
	(void)w;
	(void)client;
	(void)cont;
	if (ev->type == MapNotify) {
		printf("ready\n");
		fflush(stdout);
	}
}

int main(int argc, char **argv)
{
	Widget top, form;
	Arg args[8];
	int n;

	if (!getenv("DISPLAY") || !*getenv("DISPLAY")) {
		printf("textapp: SKIP: DISPLAY is not set\n");
		return 77;
	}
	XtSetLanguageProc(NULL, NULL, NULL);
	top = XtAppInitialize(&app, "TextApp", NULL, 0, &argc, argv, NULL,
			      NULL, 0);
	term_id = XtAppAddSignal(app, report_and_exit, NULL);
	signal(SIGTERM, on_term);

	form = XmCreateForm(top, "form", NULL, 0);
	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNcolumns, 30); n++;
	tf = XmCreateTextField(form, "tf", args, n);
	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, tf); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNrows, 4); n++;
	text = XmCreateText(form, "text", args, n);
	XtManageChild(tf);
	XtManageChild(text);
	XtManageChild(form);
	XtAddEventHandler(top, StructureNotifyMask, False, mapped, NULL);
	XtRealizeWidget(top);
	XtAppMainLoop(app);
	return 0;
}
