/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmText, XmTextField and XmDataField through their API: editing,
 * positions, alignment, search, the primary selection, cut/copy/paste
 * through the clipboard, DataField pictures and validation, and a 10 MB
 * document; and through their actions, the quick transfers of the
 * primary and the secondary selection.  Real keyboard and mouse input is
 * driven by text_xdotool.sh with xdotool.
 *
 * Neither widget has an undo of edits (there is no undo action, and
 * osfUndo is not bound), so there is none to test; see
 * text_clipboard_undo_copy for XmClipboardUndoCopy().
 */
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/AccTextT.h>
#include <Xm/BulletinB.h>
#include <Xm/CutPaste.h>
#include <Xm/DataF.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/TraitP.h>
#include <Xm/TransferT.h>
#include <check.h>

#include "suites.h"

static Widget top, bb;

static void pump(void)
{
	Display *dpy = XtDisplay(top);
	int idle = 0;

	XSync(dpy, False);
	while (idle < 3) {
		if (XtAppPending(app)) {
			XtAppProcessEvent(app, XtIMAll);
			idle = 0;
		} else {
			XSync(dpy, False);
			idle++;
		}
	}
}

static void setup(void)
{
	Arg args[2];

	top = init_xt("check_Text");
	XtSetArg(args[0], XmNwidth, 400);
	XtSetArg(args[1], XmNheight, 300);
	bb = XmCreateBulletinBoard(top, "bb", args, 2);
	XtManageChild(bb);
}

static void teardown(void)
{
	uninit_xt();
}

/*
 * A server timestamp, for the calls that need one.  The event goes
 * through Xt too, so that XtLastTimestampProcessed() is as recent as it
 * would be after real input.
 */
static Time server_time(Widget w)
{
	Display *dpy = XtDisplay(w);
	Atom prop = XInternAtom(dpy, "_MOTIF_TEST_TIME", False);
	XEvent ev;

	XSelectInput(dpy, XtWindow(w), XtBuildEventMask(w) | PropertyChangeMask);
	XChangeProperty(dpy, XtWindow(w), prop, XA_STRING, 8, PropModeAppend,
			(unsigned char *)"", 0);
	XWindowEvent(dpy, XtWindow(w), PropertyChangeMask, &ev);
	XtDispatchEvent(&ev);
	return ev.xproperty.time;
}

static void assert_tf(Widget tf, const char *expect)
{
	char *s = XmTextFieldGetString(tf);

	ck_assert_str_eq(s, expect);
	XtFree(s);
}

static void assert_text(Widget t, const char *expect)
{
	char *s = XmTextGetString(t);

	ck_assert_str_eq(s, expect);
	XtFree(s);
}

START_TEST(textfield_editing)
{
	Widget tf = XmCreateTextField(bb, "tf", NULL, 0);
	char buf[16];

	XtManageChild(tf);
	XtRealizeWidget(top);
	XmTextFieldSetString(tf, "hello world");
	assert_tf(tf, "hello world");
	ck_assert_int_eq(XmTextFieldGetLastPosition(tf), 11);

	XmTextFieldInsert(tf, 5, ",");
	assert_tf(tf, "hello, world");
	XmTextFieldReplace(tf, 7, 12, "there");
	assert_tf(tf, "hello, there");
	ck_assert_int_eq(XmTextFieldGetSubstring(tf, 0, 5, sizeof buf, buf),
			 XmCOPY_SUCCEEDED);
	ck_assert_str_eq(buf, "hello");
	/* Past the end of the text: what there is */
	ck_assert_int_eq(XmTextFieldGetSubstring(tf, 7, 10, sizeof buf, buf),
			 XmCOPY_TRUNCATED);
	ck_assert_str_eq(buf, "there");
	/* The buffer must hold num_chars and the NUL */
	ck_assert_int_eq(XmTextFieldGetSubstring(tf, 0, 5, 5, buf),
			 XmCOPY_FAILED);

	XmTextFieldSetInsertionPosition(tf, 3);
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), 3);

	XmTextFieldSetString(tf, "");
	assert_tf(tf, "");
	ck_assert_int_eq(XmTextFieldGetLastPosition(tf), 0);
	pump();
}
END_TEST

/*
 * A wide string that does not convert to the locale's multibyte encoding
 * sets an empty string.  The Wcs functions used to replace their
 * conversion buffer with "" on failure and then XtFree() it.  A lone
 * surrogate converts in no locale, UTF-8 ones included, so the test does
 * not depend on the locale the suite runs in.
 */
START_TEST(textfield_unconvertible_wcs)
{
	static wchar_t bad[] = { L'a', 0xd800, L'b', 0 };
	Widget tf = XmCreateTextField(bb, "tf", NULL, 0);
	Widget df = XmCreateDataField(bb, "df", NULL, 0);

	XtManageChild(tf);
	XtManageChild(df);
	XtRealizeWidget(top);
	XmTextFieldSetString(tf, "keep");
	XmTextFieldSetStringWcs(tf, bad);
	assert_tf(tf, "");
	XmTextFieldSetString(tf, "keep");
	XmTextFieldReplaceWcs(tf, 0, 4, bad);
	assert_tf(tf, "");

	XmDataFieldSetString(df, "keep");
	XmDataFieldReplaceWcs(df, 0, 4, bad);
	{
		char *s = XmDataFieldGetString(df);

		ck_assert_str_eq(s, "");
		XtFree(s);
	}
	pump();
}
END_TEST

START_TEST(textfield_selection_and_clipboard)
{
	Widget tf = XmCreateTextField(bb, "tf", NULL, 0);
	XmTextPosition left, right;
	char *sel;
	Time t;

	XtManageChild(tf);
	XtRealizeWidget(top);
	pump();
	t = server_time(tf);

	XmTextFieldSetString(tf, "cut and paste");
	XmTextFieldSetSelection(tf, 4, 7, t);
	ck_assert(XmTextFieldGetSelectionPosition(tf, &left, &right));
	ck_assert_int_eq(left, 4);
	ck_assert_int_eq(right, 7);
	sel = XmTextFieldGetSelection(tf);
	ck_assert_str_eq(sel, "and");
	XtFree(sel);

	ck_assert(XmTextFieldCopy(tf, t));
	pump();
	XmTextFieldClearSelection(tf, t);
	ck_assert_ptr_null(XmTextFieldGetSelection(tf));

	XmTextFieldSetInsertionPosition(tf, 0);
	ck_assert(XmTextFieldPaste(tf));
	pump();
	assert_tf(tf, "andcut and paste");

	XmTextFieldSetSelection(tf, 0, 3, server_time(tf));
	ck_assert(XmTextFieldCut(tf, server_time(tf)));
	pump();
	assert_tf(tf, "cut and paste");
	XmTextFieldSetInsertionPosition(tf, XmTextFieldGetLastPosition(tf));
	ck_assert(XmTextFieldPaste(tf));
	pump();
	assert_tf(tf, "cut and pasteand");

	XmTextFieldSetSelection(tf, 0, 3, server_time(tf));
	ck_assert(XmTextFieldRemove(tf));
	assert_tf(tf, " and pasteand");
}
END_TEST

START_TEST(text_editing_and_search)
{
	Widget t;
	XmTextPosition pos;
	Position x, y;
	Arg args[2];

	XtSetArg(args[0], XmNeditMode, XmMULTI_LINE_EDIT);
	XtSetArg(args[1], XmNrows, 5);
	t = XmCreateText(bb, "text", args, 2);
	XtManageChild(t);
	XtRealizeWidget(top);

	XmTextSetString(t, "line one\nline two\nline three\n");
	ck_assert_int_eq(XmTextGetLastPosition(t), 29);
	XmTextInsert(t, 9, "inserted\n");
	assert_text(t, "line one\ninserted\nline two\nline three\n");
	XmTextReplace(t, 0, 4, "LINE");
	assert_text(t, "LINE one\ninserted\nline two\nline three\n");

	ck_assert(XmTextFindString(t, 0, "two", XmTEXT_FORWARD, &pos));
	ck_assert_int_eq(pos, 23);
	ck_assert(XmTextFindString(t, XmTextGetLastPosition(t), "line",
				   XmTEXT_BACKWARD, &pos));
	ck_assert_int_eq(pos, 27);
	ck_assert(!XmTextFindString(t, 0, "absent", XmTEXT_FORWARD, &pos));

	ck_assert(XmTextPosToXY(t, 0, &x, &y));
	ck_assert_int_eq(XmTextXYToPos(t, x, y), 0);
	XmTextShowPosition(t, XmTextGetLastPosition(t));
	XmTextSetTopCharacter(t, 9);
	pump();
	ck_assert_int_eq(XmTextGetTopCharacter(t), 9);
}
END_TEST

/*
 * XmTextGetSubstring compared the bytes it would copy with buf_size as
 * unsigned, so a negative buf_size let it copy without bound.
 */
START_TEST(text_substring_bad_size)
{
	Widget t = XmCreateText(bb, "text", NULL, 0);
	char buf[16];

	XtManageChild(t);
	XtRealizeWidget(top);
	XmTextSetString(t, "hello");
	ck_assert_int_eq(XmTextGetSubstring(t, 0, 5, sizeof buf, buf),
			 XmCOPY_SUCCEEDED);
	ck_assert_str_eq(buf, "hello");
	ck_assert_int_eq(XmTextGetSubstring(t, 0, 5, 5, buf), XmCOPY_FAILED);
	ck_assert_int_eq(XmTextGetSubstring(t, 0, 5, -1, buf), XmCOPY_FAILED);
	pump();
}
END_TEST

START_TEST(text_selection_and_clipboard)
{
	Widget t = XmCreateText(bb, "text", NULL, 0);
	char *sel;

	XtManageChild(t);
	XtRealizeWidget(top);
	pump();

	XmTextSetString(t, "primary selection");
	XmTextSetSelection(t, 0, 7, server_time(t));
	sel = XmTextGetSelection(t);
	ck_assert_str_eq(sel, "primary");
	XtFree(sel);

	ck_assert(XmTextCopy(t, server_time(t)));
	pump();
	XmTextClearSelection(t, server_time(t));
	XmTextSetInsertionPosition(t, XmTextGetLastPosition(t));
	ck_assert(XmTextPaste(t));
	pump();
	assert_text(t, "primary selectionprimary");
}
END_TEST

/*
 * XmText and XmTextField have no undo of edits; the only undo near them
 * is XmClipboardUndoCopy(), which takes back the last copy to the
 * clipboard made through a window, here the widget's own.  A second call
 * undoes the first.
 */
START_TEST(text_clipboard_undo_copy)
{
	Widget t = XmCreateText(bb, "text", NULL, 0);
	Display *dpy;

	XtManageChild(t);
	XtRealizeWidget(top);
	pump();
	dpy = XtDisplay(t);
	XmTextSetString(t, "one two");
	XmTextSetSelection(t, 0, 3, server_time(t));
	ck_assert(XmTextCopy(t, server_time(t)));
	XmTextSetSelection(t, 4, 7, server_time(t));
	ck_assert(XmTextCopy(t, server_time(t)));
	pump();

	ck_assert_int_eq(XmClipboardUndoCopy(dpy, XtWindow(t)),
			 ClipboardSuccess);
	XmTextClearSelection(t, server_time(t));
	XmTextSetInsertionPosition(t, XmTextGetLastPosition(t));
	ck_assert(XmTextPaste(t));
	pump();
	assert_text(t, "one twoone");

	ck_assert_int_eq(XmClipboardUndoCopy(dpy, XtWindow(t)),
			 ClipboardSuccess);
	XmTextSetInsertionPosition(t, XmTextGetLastPosition(t));
	ck_assert(XmTextPaste(t));
	pump();
	assert_text(t, "one twoonetwo");
}
END_TEST

/* Clipboard between two widgets of the same process */
START_TEST(copy_text_to_textfield)
{
	Widget t = XmCreateText(bb, "text", NULL, 0);
	Widget tf = XtVaCreateManagedWidget("tf", xmTextFieldWidgetClass, bb,
					    XmNy, 100, NULL);

	XtManageChild(t);
	XtRealizeWidget(top);
	pump();
	XmTextSetString(t, "from text");
	XmTextSetSelection(t, 5, 9, server_time(t));
	ck_assert(XmTextCopy(t, server_time(t)));
	pump();
	ck_assert(XmTextFieldPaste(tf));
	pump();
	assert_tf(tf, "text");
}
END_TEST

START_TEST(large_document)
{
	enum { SIZE = 10 * 1024 * 1024 };
	Widget t;
	char *doc, *got;
	XmTextPosition pos;
	Arg args[1];
	size_t i;

	XtSetArg(args[0], XmNeditMode, XmMULTI_LINE_EDIT);
	t = XmCreateScrolledText(bb, "text", args, 1);
	XtManageChild(t);
	XtRealizeWidget(top);

	doc = malloc(SIZE + 1);
	ck_assert_ptr_nonnull(doc);
	for (i = 0; i < SIZE; i++)
		doc[i] = (i % 64 == 63) ? '\n' : 'a' + (char)(i % 26);
	doc[SIZE] = '\0';

	XmTextSetString(t, doc);
	ck_assert_int_eq(XmTextGetLastPosition(t), SIZE);
	XmTextInsert(t, SIZE / 2, "MIDDLE");
	ck_assert_int_eq(XmTextGetLastPosition(t), SIZE + 6);
	ck_assert(XmTextFindString(t, 0, "MIDDLE", XmTEXT_FORWARD, &pos));
	ck_assert_int_eq(pos, SIZE / 2);
	XmTextShowPosition(t, SIZE);
	pump();
	XmTextReplace(t, SIZE / 2, SIZE / 2 + 6, "");
	got = XmTextGetString(t);
	ck_assert_int_eq(strlen(got), SIZE);
	ck_assert(!memcmp(got, doc, SIZE));
	XtFree(got);
	free(doc);
}
END_TEST

/* The left and right edges of the text area of a text field */
static void text_edges(Widget w, Position *left, Position *right)
{
	Dimension width, margin, shadow, highlight;

	XtVaGetValues(w, XmNwidth, &width, XmNmarginWidth, &margin,
		      XmNshadowThickness, &shadow,
		      XmNhighlightThickness, &highlight, NULL);
	*left = margin + shadow + highlight;
	*right = width - *left;
}

static Position pos_x(Widget w, XmTextPosition pos)
{
	Position x, y;

	ck_assert(XmTextFieldPosToXY(w, pos, &x, &y));
	return x;
}

/*
 * XmNalignment: XmALIGNMENT_END keeps the end of the text at the right
 * margin, while the text fits and after it was scrolled; XmTextField has
 * it since XmDataField became its subclass.
 */
static void check_alignment(Widget w)
{
	Position left, right;
	XmTextPosition last;
	char long_text[201];

	XtManageChild(w);
	XtRealizeWidget(top);
	pump();
	text_edges(w, &left, &right);

	XmTextFieldSetString(w, "abc");
	ck_assert_int_eq(pos_x(w, 3), right);
	ck_assert_int_gt(pos_x(w, 0), left);
	XmTextFieldInsert(w, 3, "def");
	ck_assert_int_eq(pos_x(w, 6), right);
	XmTextFieldReplace(w, 0, 4, "");
	ck_assert_int_eq(pos_x(w, 2), right);

	memset(long_text, 'm', 200);
	long_text[200] = '\0';
	XmTextFieldSetString(w, long_text);
	last = XmTextFieldGetLastPosition(w);
	XmTextFieldShowPosition(w, last);
	ck_assert_int_eq(pos_x(w, last), right);
	XmTextFieldShowPosition(w, 0);
	ck_assert_int_eq(pos_x(w, 0), left);
	XmTextFieldReplace(w, 0, last - 2, "");
	ck_assert_int_eq(pos_x(w, 2), right);

	XtVaSetValues(w, XmNalignment, XmALIGNMENT_BEGINNING, NULL);
	pump();
	ck_assert_int_eq(pos_x(w, 0), left);
	XtVaSetValues(w, XmNalignment, XmALIGNMENT_END, NULL);
	pump();
	ck_assert_int_eq(pos_x(w, 2), right);
}

START_TEST(textfield_alignment)
{
	Arg args[1];

	XtSetArg(args[0], XmNalignment, XmALIGNMENT_END);
	check_alignment(XmCreateTextField(bb, "tf", args, 1));
}
END_TEST

START_TEST(datafield_alignment)
{
	Arg args[1];

	XtSetArg(args[0], XmNalignment, XmALIGNMENT_END);
	check_alignment(XmCreateDataField(bb, "df", args, 1));
}
END_TEST

/* XmDataField is an XmTextField, so the XmTextField API applies */
START_TEST(datafield_is_textfield)
{
	Widget df = XmCreateDataField(bb, "df", NULL, 0);
	char *s;

	XtManageChild(df);
	XtRealizeWidget(top);
	ck_assert(XtIsSubclass(df, xmTextFieldWidgetClass));
	ck_assert(XmIsTextField(df));
	ck_assert_ptr_eq(
	    XmeTraitGet((XtPointer)xmDataFieldWidgetClass, XmQTaccessTextual),
	    XmeTraitGet((XtPointer)xmTextFieldWidgetClass, XmQTaccessTextual));
	ck_assert_ptr_eq(
	    XmeTraitGet((XtPointer)xmDataFieldWidgetClass, XmQTtransfer),
	    XmeTraitGet((XtPointer)xmTextFieldWidgetClass, XmQTtransfer));

	XmDataFieldSetString(df, "data");
	assert_tf(df, "data");
	s = XmTextGetString(df);
	ck_assert_str_eq(s, "data");
	XtFree(s);
	XmTextFieldInsert(df, 4, "field");
	s = XmDataFieldGetString(df);
	ck_assert_str_eq(s, "datafield");
	XtFree(s);
	pump();
}
END_TEST

static int picture_errors;

static void picture_error(Widget w, XtPointer client, XtPointer call)
{
	picture_errors++;
}

START_TEST(datafield_picture)
{
	Widget df, fill, nofill;
	Arg args[2];

	XtSetArg(args[0], XmNpicture, "###");
	df = XmCreateDataField(bb, "df", args, 1);
	XtAddCallback(df, XmNpictureErrorCallback, picture_error, NULL);
	XtSetArg(args[0], XmNpicture, "##-##");
	fill = XmCreateDataField(bb, "fill", args, 1);
	XtSetArg(args[1], XmNautoFill, False);
	nofill = XmCreateDataField(bb, "nofill", args, 2);
	XtManageChild(df);
	XtManageChild(fill);
	XtManageChild(nofill);
	XtRealizeWidget(top);

	picture_errors = 0;
	XmDataFieldInsert(df, 0, "12");
	assert_tf(df, "12");
	XmDataFieldSetInsertionPosition(df, 2);
	XmDataFieldInsert(df, 2, "x");
	assert_tf(df, "12");
	ck_assert_int_eq(picture_errors, 1);
	XmDataFieldInsert(df, 2, "3");
	assert_tf(df, "123");

	/* A new picture replaces the old one, and is checked only once. */
	XtVaSetValues(df, XmNpicture, "####", NULL);
	XmDataFieldSetInsertionPosition(df, 3);
	XmDataFieldInsert(df, 3, "x");
	ck_assert_int_eq(picture_errors, 2);
	XmDataFieldInsert(df, 3, "4");
	assert_tf(df, "1234");

	/* No picture, no checks */
	XtVaSetValues(df, XmNpicture, NULL, NULL);
	XmDataFieldInsert(df, 0, "x");
	assert_tf(df, "x1234");
	ck_assert_int_eq(picture_errors, 2);

	XmDataFieldInsert(fill, 0, "12");
	assert_tf(fill, "12-");
	XmDataFieldInsert(nofill, 0, "12");
	assert_tf(nofill, "12");
	pump();
}
END_TEST

static char *validated;

static void reject(Widget w, XtPointer client, XtPointer call)
{
	XmDataFieldCallbackStruct *cbs = call;

	ck_assert_ptr_eq(cbs->w, w);
	validated = XtNewString(cbs->text);
	cbs->accept = False;
}

START_TEST(datafield_validate)
{
	Widget df = XmCreateDataField(bb, "df", NULL, 0);
	String params[1] = { "next" };

	XtAddCallback(df, XmNvalidateCallback, reject, NULL);
	XtManageChild(df);
	XtRealizeWidget(top);
	XmDataFieldSetString(df, "value");
	validated = NULL;
	XtCallActionProc(df, "ValidateAndMove", NULL, params, 1);
	ck_assert_ptr_nonnull(validated);
	ck_assert_str_eq(validated, "value");
	XtFree(validated);

	/* Tab is bound to ValidateAndMove ahead of TextField's binding. */
	{
		XKeyEvent key;

		memset(&key, 0, sizeof(key));
		key.type = KeyPress;
		key.display = XtDisplay(df);
		key.window = XtWindow(df);
		key.root = RootWindowOfScreen(XtScreen(df));
		key.time = server_time(df);
		key.keycode = XKeysymToKeycode(key.display, XK_Tab);
		key.same_screen = True;
		XmDataFieldSetString(df, "tab");
		validated = NULL;
		XtDispatchEvent((XEvent *)&key);
		ck_assert_ptr_nonnull(validated);
		ck_assert_str_eq(validated, "tab");
		XtFree(validated);
	}
	pump();
}
END_TEST

/*
 * The secondary selection ("quick transfer"): a Button2 drag with Alt
 * or Meta in one text widget selects text there, and on release that
 * text is copied (no Shift) or moved (Shift) to the insertion point of
 * the widget that has the destination cursor (_MOTIF_DESTINATION), in
 * this or another application.  These tests call the actions that the
 * default translations bind to those events (secondary-start,
 * secondary-adjust, copy-to, move-to) with synthetic events;
 * text_xdotool.sh drives the same with real input, also between two
 * processes.
 */

static Widget create_text_widget(const char *name, Boolean field, Position y)
{
	return XtVaCreateManagedWidget(name,
				       field ? xmTextFieldWidgetClass
					     : xmTextWidgetClass,
				       bb, XmNy, y, XmNcolumns, 30, NULL);
}

/* Call action on w with a synthetic event at x, y */
static void event_action(Widget w, const char *action, int type, int x, int y,
			 unsigned int state, Time t)
{
	XEvent ev;

	memset(&ev, 0, sizeof ev);
	ev.xbutton.type = type;
	ev.xbutton.display = XtDisplay(w);
	ev.xbutton.window = XtWindow(w);
	ev.xbutton.root = RootWindowOfScreen(XtScreen(w));
	ev.xbutton.time = t;
	ev.xbutton.x = x;
	ev.xbutton.y = y;
	ev.xbutton.same_screen = True;
	ev.xbutton.state = state;
	if (type == ButtonPress || type == ButtonRelease)
		ev.xbutton.button = (state & Button1Mask) ? Button1 : Button2;
	else if (type == KeyPress)
		ev.xkey.keycode = XKeysymToKeycode(XtDisplay(w), XK_Escape);
	XtCallActionProc(w, action, &ev, NULL, 0);
}

/* Call action on w with a synthetic pointer event over position pos */
static void pointer_action(Widget w, const char *action, int type,
			   XmTextPosition pos, unsigned int state, Time t)
{
	Position x, y;

	/* XmTextPosToXY gives the baseline at the left of the character */
	ck_assert(XmTextPosToXY(w, pos, &x, &y));
	event_action(w, action, type, x + 1, y - 2, state, t);
}

/* Click Button1 at pos in w: w gets the focus and the destination cursor */
static void click(Widget w, XmTextPosition pos)
{
	Time t = server_time(w);

	pointer_action(w, "grab-focus", ButtonPress, pos, 0, t);
	pointer_action(w, "extend-end", ButtonRelease, pos, Button1Mask, t);
	pump();
	ck_assert_ptr_eq(XmGetDestination(XtDisplay(w)), w);
}

/*
 * Drag Button2 with Alt in w from left to right and release it there,
 * calling release ("copy-to" or "move-to").
 */
static void secondary_drag(Widget w, XmTextPosition left, XmTextPosition right,
			   const char *release)
{
	/* One timestamp for all: the selections are owned at the time of
	 * the events, which must not be later than the server's time. */
	Time t = server_time(w);

	/* What the press and the motion cause (the focus events of the
	 * keyboard grab) is handled before the release, as with real input. */
	pointer_action(w, "secondary-start", ButtonPress, left, Mod1Mask, t);
	pump();
	pointer_action(w, "secondary-adjust", MotionNotify, (left + right) / 2,
		       Mod1Mask | Button2Mask, t);
	pointer_action(w, "secondary-adjust", MotionNotify, right,
		       Mod1Mask | Button2Mask, t);
	pump();
	pointer_action(w, release, ButtonRelease, right,
		       Mod1Mask | Button2Mask, t);
	pump();
}

/* One quick transfer from a Text or TextField to a Text or TextField */
static void check_secondary(Boolean src_field, Boolean dst_field,
			    const char *release, const char *src_expect,
			    const char *dst_expect)
{
	Widget src = create_text_widget("src", src_field, 0);
	Widget dst = create_text_widget("dst", dst_field, 100);
	Display *dpy;

	XtRealizeWidget(top);
	pump();
	dpy = XtDisplay(top);
	XmTextSetString(src, "abc def ghi");
	XmTextSetString(dst, "dest");
	click(dst, 4);

	secondary_drag(src, 4, 7, release);
	assert_text(src, src_expect);
	assert_text(dst, dst_expect);
	/* The transfer is over: nobody owns SECONDARY any more, and the
	 * destination cursor has not moved to the source. */
	ck_assert_int_eq(XGetSelectionOwner(dpy, XA_SECONDARY), None);
	ck_assert_ptr_eq(XmGetDestination(dpy), dst);
}

/* _i: bit 0, the source is a TextField; bit 1, the destination is one */
START_TEST(secondary_copy)
{
	check_secondary(_i & 1, (_i & 2) != 0, "copy-to", "abc def ghi",
			"destdef");
}
END_TEST

START_TEST(secondary_move)
{
	check_secondary(_i & 1, (_i & 2) != 0, "move-to", "abc  ghi",
			"destdef");
}
END_TEST

/*
 * Within one widget, with the destination cursor before, after and in
 * the secondary selection.  Moving text into itself does nothing.
 * _i: bit 0, a TextField; the rest, the case.
 */
START_TEST(secondary_same_widget)
{
	static const struct {
		XmTextPosition dest;
		const char *release, *expect;
	} cases[] = {
		{ 0, "copy-to", "defabc def ghi" },
		{ 11, "copy-to", "abc def ghidef" },
		{ 5, "copy-to", "abc ddefef ghi" },
		{ 0, "move-to", "defabc  ghi" },
		{ 11, "move-to", "abc  ghidef" },
		{ 5, "move-to", "abc def ghi" },
	};
	Widget w = create_text_widget("text", _i & 1, 0);

	XtRealizeWidget(top);
	pump();
	XmTextSetString(w, "abc def ghi");
	click(w, cases[_i >> 1].dest);
	secondary_drag(w, 4, 7, cases[_i >> 1].release);
	assert_text(w, cases[_i >> 1].expect);
	ck_assert_int_eq(XGetSelectionOwner(XtDisplay(w), XA_SECONDARY), None);
}
END_TEST

/*
 * A secondary selection is dropped, and nothing transferred, when Escape
 * (process-cancel) is pressed during the drag (_i bit 1 clear) or the
 * button is released outside the widget (bit 1 set).  _i bit 0: the
 * source is a TextField.
 */
START_TEST(secondary_cancel)
{
	Widget src = create_text_widget("src", _i & 1, 0);
	Widget dst = create_text_widget("dst", False, 100);
	Display *dpy;
	Time t;

	XtRealizeWidget(top);
	pump();
	dpy = XtDisplay(top);
	XmTextSetString(src, "abc def ghi");
	XmTextSetString(dst, "dest");
	click(dst, 4);

	t = server_time(src);
	pointer_action(src, "secondary-start", ButtonPress, 4, Mod1Mask, t);
	pointer_action(src, "secondary-adjust", MotionNotify, 7,
		       Mod1Mask | Button2Mask, t);
	if (_i & 2) {
		event_action(src, "move-to", ButtonRelease,
			     src->core.width + 10, 5,
			     Mod1Mask | Button2Mask, t);
	} else {
		event_action(src, "process-cancel", KeyPress, 0, 0,
			     Mod1Mask | Button2Mask, t);
		pointer_action(src, "move-to", ButtonRelease, 7,
			       Mod1Mask | Button2Mask, t);
	}
	pump();
	assert_text(src, "abc def ghi");
	assert_text(dst, "dest");
	ck_assert_int_eq(XGetSelectionOwner(dpy, XA_SECONDARY), None);

	/* The next quick transfer works */
	secondary_drag(src, 0, 3, "copy-to");
	assert_text(dst, "destabc");
}
END_TEST

/*
 * The same actions without a secondary selection: Button2 clicked (not
 * dragged) in a widget copies the primary selection to the pointer
 * (copy-to), or moves it there with Shift (move-to).
 */
static void quick_primary(Widget src, Widget dst, const char *release)
{
	Time t = server_time(src);

	XmTextSetSelection(src, 4, 7, t);
	pointer_action(dst, "process-bdrag", ButtonPress, 4, 0, t);
	pointer_action(dst, release, ButtonRelease, 4, Button2Mask, t);
	pump();
}

/* _i: bit 0, the source is a TextField; bit 1, the destination is one */
START_TEST(primary_quick_copy_move)
{
	Widget src = create_text_widget("src", _i & 1, 0);
	Widget dst = create_text_widget("dst", (_i & 2) != 0, 100);

	XtRealizeWidget(top);
	pump();
	XmTextSetString(src, "abc def ghi");
	XmTextSetString(dst, "dest");
	quick_primary(src, dst, "copy-to");
	assert_text(src, "abc def ghi");
	assert_text(dst, "destdef");
	quick_primary(src, dst, "move-to");
	assert_text(src, "abc  ghi");
	assert_text(dst, "destdefdef");
}
END_TEST

/* A modifyVerifyCallback that refuses every change */
static void refuse_change(Widget w, XtPointer client, XtPointer call)
{
	((XmTextVerifyCallbackStruct *)call)->doit = False;
}

/* Make dst refuse to change: _i bit 2 set, by XmNmodifyVerifyCallback,
 * otherwise by not being editable. */
static void refuse_changes(Widget dst, int how)
{
	if (how)
		XtAddCallback(dst, XmNmodifyVerifyCallback, refuse_change, NULL);
	else
		XtVaSetValues(dst, XmNeditable, False, NULL);
}

/*
 * Moving text to a destination that refuses it leaves the source alone:
 * the destination used to answer the request as if it had inserted the
 * text, and the source then deleted it.  _i: bit 0, the source is a
 * TextField; bit 1, the destination is one; bit 2, see refuse_changes().
 */
START_TEST(secondary_move_refused)
{
	Widget src = create_text_widget("src", _i & 1, 0);
	Widget dst = create_text_widget("dst", (_i & 2) != 0, 100);

	XtRealizeWidget(top);
	pump();
	XmTextSetString(src, "abc def ghi");
	XmTextSetString(dst, "dest");
	click(dst, 4);
	refuse_changes(dst, _i & 4);

	secondary_drag(src, 4, 7, "move-to");
	assert_text(src, "abc def ghi");
	assert_text(dst, "dest");
	ck_assert_int_eq(XGetSelectionOwner(XtDisplay(top), XA_SECONDARY),
			 None);
}
END_TEST

/* As secondary_move_refused, for the primary selection */
START_TEST(primary_quick_move_refused)
{
	Widget src = create_text_widget("src", _i & 1, 0);
	Widget dst = create_text_widget("dst", (_i & 2) != 0, 100);

	XtRealizeWidget(top);
	pump();
	XmTextSetString(src, "abc def ghi");
	XmTextSetString(dst, "dest");
	refuse_changes(dst, _i & 4);
	quick_primary(src, dst, "move-to");
	assert_text(src, "abc def ghi");
	assert_text(dst, "dest");
}
END_TEST

/*
 * The quick transfer actions called without an event, as through
 * XtCallActionProc(): the insertion cursor stands for the pointer.  They
 * used to dereference the event and crash.  _i: bit 0, the source is a
 * TextField; bit 1, the destination is one.
 */
START_TEST(secondary_no_event)
{
	static const char *actions[] = {
		"secondary-start", "secondary-adjust", "copy-to",
		"secondary-start", "secondary-adjust", "move-to",
		"secondary-start", "secondary-adjust", "link-to",
		"secondary-start", "process-cancel", "copy-to",
		"process-bdrag", "process-cancel", "move-to",
		"copy-to", "move-to", "link-to",
	};
	Widget src = create_text_widget("src", _i & 1, 0);
	Widget dst = create_text_widget("dst", (_i & 2) != 0, 100);
	size_t i;

	XtRealizeWidget(top);
	pump();
	XmTextSetString(src, "abc def ghi");
	XmTextSetString(dst, "dest");
	for (i = 0; i < XtNumber(actions); i++) {
		XtCallActionProc(src, actions[i], NULL, NULL, 0);
		pump();
	}
	/* Nothing selected, nothing transferred */
	assert_text(src, "abc def ghi");
	assert_text(dst, "dest");

	/* A Button2 click without a drag pastes the primary selection
	 * at the cursor. */
	click(dst, 4);
	XmTextSetSelection(src, 4, 7, server_time(src));
	XtCallActionProc(dst, "process-bdrag", NULL, NULL, 0);
	XtCallActionProc(dst, "copy-to", NULL, NULL, 0);
	pump();
	assert_text(dst, "destdef");
}
END_TEST

void text_suite(SRunner *runner)
{
	Suite *s = suite_create("Text");
	TCase *t;

	t = tcase_create("TextField");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, textfield_editing);
	tcase_add_test(t, textfield_selection_and_clipboard);
	tcase_add_test(t, textfield_unconvertible_wcs);
	tcase_add_test(t, textfield_alignment);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("DataField");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, datafield_is_textfield);
	tcase_add_test(t, datafield_alignment);
	tcase_add_test(t, datafield_picture);
	tcase_add_test(t, datafield_validate);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Text");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, text_editing_and_search);
	tcase_add_test(t, text_substring_bad_size);
	tcase_add_test(t, text_selection_and_clipboard);
	tcase_add_test(t, copy_text_to_textfield);
	tcase_add_test(t, text_clipboard_undo_copy);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Secondary selection");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_loop_test(t, secondary_copy, 0, 4);
	tcase_add_loop_test(t, secondary_move, 0, 4);
	tcase_add_loop_test(t, secondary_same_widget, 0, 12);
	tcase_add_loop_test(t, secondary_cancel, 0, 4);
	tcase_add_loop_test(t, primary_quick_copy_move, 0, 4);
	tcase_add_loop_test(t, secondary_move_refused, 0, 8);
	tcase_add_loop_test(t, primary_quick_move_refused, 0, 8);
	tcase_add_loop_test(t, secondary_no_event, 0, 4);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Large document");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, large_document);
	tcase_set_timeout(t, 300);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
