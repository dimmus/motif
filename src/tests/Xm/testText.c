/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmText and XmTextField through their API: editing, positions,
 * search, the primary selection, cut/copy/paste through the clipboard,
 * and a 10 MB document.  Real keyboard and mouse input is driven by
 * text_xdotool.sh with xdotool.
 */
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/DataF.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
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

/* A server timestamp, for the calls that need one */
static Time server_time(Widget w)
{
	Display *dpy = XtDisplay(w);
	Atom prop = XInternAtom(dpy, "_MOTIF_TEST_TIME", False);
	XEvent ev;

	XSelectInput(dpy, XtWindow(w), PropertyChangeMask);
	XChangeProperty(dpy, XtWindow(w), prop, XA_STRING, 8, PropModeAppend,
			(unsigned char *)"", 0);
	XWindowEvent(dpy, XtWindow(w), PropertyChangeMask, &ev);
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
 * (the tests run in the C locale) sets an empty string.  The Wcs
 * functions used to replace their conversion buffer with "" on failure
 * and then XtFree() it.
 */
START_TEST(textfield_unconvertible_wcs)
{
	static wchar_t bad[] = { L'a', 0x263a, L'b', 0 };
	Widget tf = XmCreateTextField(bb, "tf", NULL, 0);
	Widget df = XmCreateDataField(bb, "df", NULL, 0);

	XtManageChild(tf);
	XtManageChild(df);
	XtRealizeWidget(top);
	XmTextFieldSetString(tf, "keep");
	XmTextFieldSetStringWcs(tf, bad);
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

void text_suite(SRunner *runner)
{
	Suite *s = suite_create("Text");
	TCase *t;

	t = tcase_create("TextField");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, textfield_editing);
	tcase_add_test(t, textfield_selection_and_clipboard);
	tcase_add_test(t, textfield_unconvertible_wcs);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Text");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, text_editing_and_search);
	tcase_add_test(t, text_substring_bad_size);
	tcase_add_test(t, text_selection_and_clipboard);
	tcase_add_test(t, copy_text_to_textfield);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Large document");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, large_document);
	tcase_set_timeout(t, 300);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
