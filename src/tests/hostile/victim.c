/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * victim: the Motif application the hostile-client test drives.  It is
 * built with ASan+UBSan and must survive every malformed thing a peer
 * (hostile_peer.c) can do through the drag-and-drop, selection and
 * clipboard protocols and through the root-window properties.
 *
 * It offers:
 *   - a drag source (the top DrawingArea): a Btn1 drag starts a real
 *     Motif drag, so sweeping the pointer across the hostile drop sites
 *     makes the toolkit read and parse their malformed
 *     _MOTIF_DRAG_RECEIVER_INFO (driven by xdotool);
 *   - a drop site (the same DrawingArea and the Text), so the hostile
 *     source's drag ClientMessages and drop are processed;
 *   - an XmText for clipboard paste.
 *
 * Actions bound for the driver to invoke with xdotool keys:
 *   key c  -> read the clipboard with the Inquire/Retrieve API (hits the
 *             record reader against the hostile owner);
 *   key p  -> XmTextPaste() from CLIPBOARD (hits the selection path);
 *   key d  -> re-read the root drag/bindings tables by starting a no-op
 *             drag programmatically.
 *
 * Prints "ready" when mapped, "clip-done", "paste-done", "drag-done"
 * after each action, and exits 0 on SIGTERM.  Exits 77 without a display.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/Text.h>
#include <Xm/DragDrop.h>
#include <Xm/CutPaste.h>
#include <Xm/AtomMgr.h>

static XtAppContext app;
static XtSignalId term_id;
static Widget top, drag_area, text;

static void on_term(int sig)
{
	(void)sig;
	XtNoticeSignal(term_id);
}

static void report_and_exit(XtPointer client, XtSignalId *id)
{
	(void)client;
	(void)id;
	printf("bye\n");
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

/* drop proc: accept nothing, just make sure we are a live drop site so
 * the hostile source's messages are routed through the drop site manager */
static void drop_proc(Widget w, XtPointer client, XtPointer call)
{
	XmDropProcCallbackStruct *cb = (XmDropProcCallbackStruct *)call;
	Arg args[4];
	int n = 0;

	(void)w;
	(void)client;
	/* decline the transfer cleanly */
	XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE);
	n++;
	XtSetArg(args[n], XmNnumDropTransfers, 0);
	n++;
	if (cb && cb->dragContext)
		XmDropTransferStart(cb->dragContext, args, n);
	printf("drop-proc\n");
	fflush(stdout);
}

/* Btn1 press on the drag source starts a real drag. */
static void StartDrag(Widget w, XEvent *ev, String *params, Cardinal *np)
{
	Arg args[6];
	int n = 0;
	Atom targets[1];

	(void)params;
	(void)np;
	targets[0] = XA_STRING;
	XtSetArg(args[n], XmNexportTargets, targets);
	n++;
	XtSetArg(args[n], XmNnumExportTargets, 1);
	n++;
	XtSetArg(args[n], XmNdragOperations, XmDROP_COPY | XmDROP_MOVE);
	n++;
	(void)XmDragStart(w, ev, args, n);
	printf("drag-start\n");
	fflush(stdout);
}

/* key c: exercise the clipboard record/inquire/retrieve API, which reads
 * the hostile owner's malformed replies and the malformed _MOTIF_CLIP_*
 * records on the root. */
static void DoClipboard(Widget w, XEvent *ev, String *params, Cardinal *np)
{
	Display *dpy = XtDisplay(w);
	Window win = XtWindow(w);
	int count = 0, i;
	unsigned long maxlen = 0, outlen = 0, len = 0;
	char fmt[256];
	char buf[256];
	long private_id = 0;

	(void)ev;
	(void)params;
	(void)np;
	XmClipboardInquireCount(dpy, win, &count, &maxlen);
	for (i = 1; i <= count && i <= 32; i++) {
		if (XmClipboardInquireFormat(dpy, win, i, fmt, sizeof fmt,
					     &outlen) == ClipboardSuccess) {
			if (outlen >= sizeof fmt)
				outlen = sizeof fmt - 1;
			fmt[outlen] = '\0';
			XmClipboardInquireLength(dpy, win, fmt, &len);
			XmClipboardRetrieve(dpy, win, fmt, buf, sizeof buf,
					    &outlen, &private_id);
		}
	}
	{
		XmClipboardPendingList list = NULL;
		unsigned long nitems = 0;

		if (XmClipboardInquirePendingItems(dpy, win, "STRING", &list,
						   &nitems) == ClipboardSuccess)
			XtFree((char *)list);
	}
	printf("clip-done count=%d\n", count);
	fflush(stdout);
}

/* key p: paste CLIPBOARD into the Text through the normal text path, which
 * goes through XtGetSelectionValue to the hostile owner. */
static void DoPaste(Widget w, XEvent *ev, String *params, Cardinal *np)
{
	(void)ev;
	(void)params;
	(void)np;
	(void)XmTextPaste(text);
	printf("paste-done\n");
	fflush(stdout);
}

/* key d: start and immediately cancel a drag, forcing the toolkit to read
 * the (hostile) root drag window / targets / atoms tables. */
static void DoDrag(Widget w, XEvent *ev, String *params, Cardinal *np)
{
	Arg args[4];
	int n = 0;
	Atom targets[1];
	Widget dc;
	XButtonEvent be;

	(void)params;
	(void)np;
	memset(&be, 0, sizeof be);
	be.type = ButtonPress;
	be.display = XtDisplay(w);
	be.window = XtWindow(w);
	be.button = Button2;
	be.time = XtLastTimestampProcessed(XtDisplay(w));
	targets[0] = XA_STRING;
	XtSetArg(args[n], XmNexportTargets, targets);
	n++;
	XtSetArg(args[n], XmNnumExportTargets, 1);
	n++;
	dc = XmDragStart(w, ev ? ev : (XEvent *)&be, args, n);
	if (dc)
		XmDragCancel(dc);
	printf("drag-done\n");
	fflush(stdout);
}

static XtActionsRec actions[] = {
	{ "startDrag", StartDrag },
	{ "doClipboard", DoClipboard },
	{ "doPaste", DoPaste },
	{ "doDrag", DoDrag },
};

int main(int argc, char **argv)
{
	Widget form;
	Arg args[12];
	int n;
	Atom import_targets[1];
	static const char *drag_translations =
		"<Btn1Down>: startDrag()\n"
		"<Key>c: doClipboard()\n"
		"<Key>p: doPaste()\n"
		"<Key>d: doDrag()";
	static const char *key_translations =
		"<Key>c: doClipboard()\n"
		"<Key>p: doPaste()\n"
		"<Key>d: doDrag()";

	if (!getenv("DISPLAY") || !*getenv("DISPLAY")) {
		printf("victim: SKIP: no DISPLAY\n");
		return 77;
	}

	XtSetLanguageProc(NULL, NULL, NULL);
	top = XtAppInitialize(&app, "HostileVictim", NULL, 0, &argc, argv,
			      NULL, NULL, 0);
	XtAppAddActions(app, actions, XtNumber(actions));

	n = 0;
	XtSetArg(args[n], XmNwidth, 400);
	n++;
	XtSetArg(args[n], XmNheight, 300);
	n++;
	form = XmCreateForm(top, "form", args, n);
	XtManageChild(form);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);
	n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);
	n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
	n++;
	XtSetArg(args[n], XmNheight, 120);
	n++;
	drag_area = XmCreateDrawingArea(form, "dragArea", args, n);
	XtManageChild(drag_area);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);
	n++;
	XtSetArg(args[n], XmNtopWidget, drag_area);
	n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);
	n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);
	n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);
	n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);
	n++;
	text = XmCreateScrolledText(form, "text", args, n);
	XtManageChild(text);

	/* register the drawing area as a drop site so the hostile source's
	 * drag messages are handled */
	import_targets[0] = XA_STRING;
	n = 0;
	XtSetArg(args[n], XmNimportTargets, import_targets);
	n++;
	XtSetArg(args[n], XmNnumImportTargets, 1);
	n++;
	XtSetArg(args[n], XmNdropSiteOperations, XmDROP_COPY | XmDROP_MOVE);
	n++;
	XtSetArg(args[n], XmNdropProc, drop_proc);
	n++;
	XmDropSiteRegister(drag_area, args, n);

	XtOverrideTranslations(drag_area,
			       XtParseTranslationTable(drag_translations));
	XtOverrideTranslations(text,
			       XtParseTranslationTable(key_translations));

	term_id = XtAppAddSignal(app, report_and_exit, NULL);
	signal(SIGTERM, on_term);
	signal(SIGINT, on_term);

	XtAddEventHandler(top, StructureNotifyMask, False, mapped, NULL);
	XtRealizeWidget(top);

	XtAppMainLoop(app);
	return 0;
}
