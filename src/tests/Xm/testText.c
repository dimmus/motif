/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmText, XmTextField and XmDataField through their API: editing,
 * positions, alignment, search, the primary selection, cut/copy/paste
 * through the clipboard, DataField pictures and validation, and a 10 MB
 * document.  Real keyboard and mouse input is driven by
 * text_xdotool.sh with xdotool.
 */
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/AccTextT.h>
#include <Xm/BulletinB.h>
#include <Xm/DataF.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/TraitP.h>
#include <Xm/TransferT.h>
#include <Xm/TextP.h>
#include <check.h>

#include "suites.h"
#include "TextI.h"

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

/*
 * The line table lookup as it was before it bisected: a walk from
 * table_index, forward or backward.
 */
static unsigned int walk_table_index(XmTextWidget tw, XmTextPosition pos)
{
	XmTextLineTable line_table = tw->text.line_table;
	unsigned int cur_index = tw->text.table_index;
	unsigned int max_index = tw->text.total_lines - 1;
	unsigned int position = (unsigned int)pos;

	if (line_table[cur_index].start_pos < position) {
		while (cur_index < max_index &&
		       line_table[cur_index].start_pos < position)
			cur_index++;
		if (position < line_table[cur_index].start_pos)
			cur_index--;
	} else {
		while (cur_index && line_table[cur_index].start_pos > position)
			cur_index--;
	}
	return cur_index;
}

static unsigned long lcg(unsigned long *state)
{
	*state = *state * 6364136223846793005UL + 1442695040888963407UL;
	return *state >> 33;
}

/* Check _XmTextGetTableIndex against the walk, from several cursors. */
static void check_line_table(Widget w, unsigned long *rnd)
{
	XmTextWidget tw = (XmTextWidget)w;
	XmTextLineTable lt = tw->text.line_table;
	unsigned int total = tw->text.total_lines, saved = tw->text.table_index;
	XmTextPosition last = XmTextGetLastPosition(w);
	unsigned int cursors[4], i, c, k;

	ck_assert_uint_ge(total, 1);
	/* The bisection relies on this. */
	for (i = 1; i < total; i++)
		ck_assert_uint_le(lt[i - 1].start_pos, lt[i].start_pos);

	cursors[0] = 0;
	cursors[1] = total / 2;
	cursors[2] = total - 1;
	cursors[3] = lcg(rnd) % total;
	for (c = 0; c < 4; c++) {
		tw->text.table_index = cursors[c];
		for (k = 0; k < 64; k++) {
			XmTextPosition pos;

			if (k < 2)
				pos = k ? last : 0;
			else if (k < 32)
				pos = lcg(rnd) % (last + 1);
			else
				pos = lt[lcg(rnd) % total].start_pos +
				      (XmTextPosition)(k % 3) - 1;
			if (pos < 0)
				pos = 0;
			ck_assert_uint_eq(_XmTextGetTableIndex(tw, pos),
					  walk_table_index(tw, pos));
		}
	}
	tw->text.table_index = saved;
}

/*
 * The line table: lookups find the line the old walk found, while the
 * text is edited, with and without word wrap (whose continuation lines
 * are in the table too).
 */
START_TEST(text_line_table_lookup)
{
	static const char words[] = "lorem ipsum dolor sit amet consectetur "
				    "adipiscing elit sed do eiusmod tempor ";
	unsigned long rnd = 12345;
	Arg args[4];
	char *doc, ins[64];
	int wrap, i, j, len;
	Widget t;

	for (wrap = 0; wrap < 2; wrap++) {
		XtSetArg(args[0], XmNeditMode, XmMULTI_LINE_EDIT);
		XtSetArg(args[1], XmNwordWrap, wrap);
		XtSetArg(args[2], XmNscrollHorizontal, False);
		XtSetArg(args[3], XmNcolumns, 20);
		t = XmCreateScrolledText(bb, "text", args, 4);
		XtManageChild(t);
		XtRealizeWidget(top);

		/* 400 lines of 0 to 79 characters. */
		doc = malloc(400 * 81 + 1);
		ck_assert_ptr_nonnull(doc);
		for (i = len = 0; i < 400; i++) {
			int n = lcg(&rnd) % 80;

			for (j = 0; j < n; j++)
				doc[len++] = words[(i * 7 + j) % (sizeof words - 1)];
			doc[len++] = '\n';
		}
		doc[len] = '\0';
		XmTextSetString(t, doc);
		free(doc);
		pump();
		/* Word wrap adds continuation lines to the 400 lines. */
		if (wrap)
			ck_assert_int_gt(((XmTextWidget)t)->text.total_lines, 600);
		else
			ck_assert_int_eq(((XmTextWidget)t)->text.total_lines, 401);
		check_line_table(t, &rnd);

		for (i = 0; i < 200; i++) {
			XmTextPosition last = XmTextGetLastPosition(t);
			XmTextPosition pos = lcg(&rnd) % (last + 1);

			if (lcg(&rnd) % 3) {
				int n = 1 + lcg(&rnd) % 40;

				for (j = 0; j < n; j++)
					ins[j] = (lcg(&rnd) % 8) ? words[lcg(&rnd) % (sizeof words - 1)]
								 : '\n';
				ins[n] = '\0';
				XmTextInsert(t, pos, ins);
			} else {
				XmTextPosition end = pos + lcg(&rnd) % 100;

				XmTextReplace(t, pos, end > last ? last : end, "");
			}
			if (i % 10 == 0)
				XmTextShowPosition(t, lcg(&rnd) % (XmTextGetLastPosition(t) + 1));
			check_line_table(t, &rnd);
		}
		XtDestroyWidget(XtParent(t));
	}
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
	tcase_add_test(t, text_line_table_lookup);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Large document");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, large_document);
	tcase_set_timeout(t, 300);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
