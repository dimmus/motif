/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Text in a real locale: XmString, XmTextField, XmText, XmList and
 * XmLabel with text of the locale's language in the locale's codeset,
 * counted in characters, not bytes.
 *
 * CTest runs the suite as Xm.I18nLocale in the default locale and, as
 * Xm.I18nLocale.<locale>, in the locales generated at build time (see
 * i18n.cmake): ja_JP.UTF-8, ja_JP.EUC-JP, de_DE.UTF-8,
 * de_DE.ISO-8859-1 and he_IL.UTF-8.
 */
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <wchar.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <check.h>

#include "i18n_util.h"
#include "suites.h"

static Widget top, bb;
static const char *locale_name;
/* The sample text in the locale's codeset, and as wide characters */
static char *sample;
static wchar_t wtext[64];
static int nchars;

/* Samples by language, in UTF-8; the first the codeset has is used */
static const struct language_sample {
	const char *language;
	const char *utf8;
} samples[] = {
	/* 日本語のテキスト */
	{ "ja", "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e\xe3\x81\xae\xe3\x83\x86"
		"\xe3\x82\xad\xe3\x82\xb9\xe3\x83\x88" },
	/* Grüße aus Köln */
	{ "de", "Gr\xc3\xbc\xc3\x9f" "e aus K\xc3\xb6ln" },
	/* שלום עולם */
	{ "he", "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d \xd7\xa2\xd7\x95\xd7\x9c\xd7\x9d" },
	{ NULL, "Gr\xc3\xbc\xc3\x9f" "e aus K\xc3\xb6ln" },
	{ NULL, "plain text" },
};

static void choose_text(void)
{
	size_t i, n;

	for (i = 0; i < sizeof samples / sizeof samples[0]; i++) {
		if (samples[i].language &&
		    strncmp(locale_name, samples[i].language, 2) != 0)
			continue;
		if ((sample = test_from_utf8(samples[i].utf8)))
			break;
	}
	ck_assert_ptr_nonnull(sample);
	n = mbstowcs(wtext, sample, sizeof wtext / sizeof wtext[0]);
	ck_assert_uint_ne(n, (size_t)-1);
	nchars = (int)n;
}

static void setup(void)
{
	locale_name = test_set_locale(NULL);
	ck_assert_ptr_nonnull(locale_name);
	top = init_xt("check_I18nLocale");
	/* The language procedure has set the locale from LC_ALL */
	ck_assert_str_eq(setlocale(LC_CTYPE, NULL), locale_name);
	bb = XmCreateBulletinBoard(top, "bb", NULL, 0);
	XtManageChild(bb);
	choose_text();
}

static void teardown(void)
{
	free(sample);
	sample = NULL;
	uninit_xt();
}

/* Characters [from, to) of the sample in the locale's codeset */
static char *chars(int from, int to)
{
	wchar_t w[64];
	char *s = malloc(4 * 64 * MB_CUR_MAX + 1);
	size_t n;

	ck_assert_ptr_nonnull(s);
	ck_assert_int_le(to, nchars);
	wmemcpy(w, wtext + from, (size_t)(to - from));
	w[to - from] = L'\0';
	n = wcstombs(s, w, 4 * 64 * MB_CUR_MAX);
	ck_assert_uint_ne(n, (size_t)-1);
	return s;
}

/* a and b joined, freeing neither */
static char *concat(const char *a, const char *b)
{
	char *s = malloc(strlen(a) + strlen(b) + 1);

	ck_assert_ptr_nonnull(s);
	strcpy(s, a);
	strcat(s, b);
	return s;
}

static char *unparse(XmString s)
{
	return (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmMULTIBYTE_TEXT, NULL,
				       0, XmOUTPUT_ALL);
}

/* The locale named by MOTIF_TEST_LOCALE is the one in effect */
START_TEST(locale_in_effect)
{
	const char *dot = strchr(locale_name, '.');
	const char *codeset = test_codeset();

	ck_assert(XSupportsLocale());
	if (dot && getenv("MOTIF_TEST_LOCALE")) {
		/* "EUC-JP", "UTF-8", "ISO-8859-1" */
		ck_assert_msg(!strcasecmp(dot + 1, codeset), "codeset %s in %s", codeset,
			      locale_name);
		if (!strcasecmp(codeset, "UTF-8"))
			ck_assert_int_ge(MB_CUR_MAX, 4);
		else if (!strcasecmp(codeset, "EUC-JP"))
			ck_assert_int_eq(MB_CUR_MAX, 3);
		else if (!strcasecmp(codeset, "ISO-8859-1"))
			ck_assert_int_eq(MB_CUR_MAX, 1);
	}
}
END_TEST

START_TEST(xmstring)
{
	XmString s, g, w, ct_back, stream_back;
	unsigned char *stream = NULL;
	char *back, *ct;
	Dimension width;
	XmRenderTable rt;
	Widget label;

	s = XmStringCreateLocalized(sample);
	back = unparse(s);
	ck_assert_str_eq(back, sample);
	XtFree(back);

	/* Multibyte and wide character locale text: the same text */
	g = XmStringGenerate(sample, NULL, XmMULTIBYTE_TEXT, NULL);
	back = unparse(g);
	ck_assert_str_eq(back, sample);
	XtFree(back);
	w = XmStringGenerate(wtext, NULL, XmWIDECHAR_TEXT, NULL);
	back = unparse(w);
	ck_assert_str_eq(back, sample);
	XtFree(back);

	ck_assert_uint_gt(XmCvtXmStringToByteStream(s, &stream), 0);
	stream_back = XmCvtByteStreamToXmString(stream);
	ck_assert(XmStringCompare(s, stream_back));
	XtFree((char *)stream);

	/* Compound text and back, through the locale's converters */
	ct = XmCvtXmStringToCT(s);
	ck_assert_ptr_nonnull(ct);
	ct_back = XmCvtCTToXmString(ct);
	back = unparse(ct_back);
	ck_assert_str_eq(back, sample);
	XtFree(back);
	XtFree(ct);

	label = XmCreateLabel(bb, "label", NULL, 0);
	XtVaGetValues(label, XmNrenderTable, &rt, NULL);
	width = XmStringWidth(rt, s);
	ck_assert_int_gt(width, 0);
	ck_assert_int_gt(XmStringHeight(rt, s), 0);

	XmStringFree(s);
	XmStringFree(g);
	XmStringFree(w);
	XmStringFree(ct_back);
	XmStringFree(stream_back);
}
END_TEST

/*
 * Compound text has no charset for Hebrew (and most of Unicode): Xlib
 * writes such characters as UTF-8 between ESC % G and ESC % @, and
 * XmCvtCTToXmString must read them.
 */
START_TEST(ct_utf8_segment)
{
	/* x, Hebrew shin, y */
	static const char ct[] = "x\033%G\327\251\033%@y";
	XmString s, back;
	char *text, *ct_out;

	back = XmCvtCTToXmString((char *)ct);
	ck_assert_ptr_nonnull(back);
	text = unparse(back);
	if (!strcasecmp(test_codeset(), "UTF-8")) {
		ck_assert_str_eq(text, "x\327\251y");

		/* The way back and forth, with charsets around the UTF-8 */
		XtFree(text);
		XmStringFree(back);
		/* aשb日c */
		s = XmStringCreateLocalized("a\327\251b\346\227\245c");
		ct_out = XmCvtXmStringToCT(s);
		ck_assert_ptr_nonnull(ct_out);
		back = XmCvtCTToXmString(ct_out);
		ck_assert_ptr_nonnull(back);
		text = unparse(back);
		ck_assert_str_eq(text, "a\327\251b\346\227\245c");
		XtFree(ct_out);
		XmStringFree(s);
	} else {
		/* The locale has no Hebrew: the text around it survives */
		ck_assert_int_eq(text[0], 'x');
		ck_assert_int_eq(text[strlen(text) - 1], 'y');
	}
	XtFree(text);
	XmStringFree(back);
}
END_TEST

START_TEST(textfield)
{
	Widget tf = XmCreateTextField(bb, "tf", NULL, 0);
	wchar_t wbuf[64], *wvalue;
	char *s, *head, *tail, *expect, buf[256];
	Position x, y, last_x = -1;
	XmTextPosition left, right;
	int i;

	XtManageChild(tf);
	XtRealizeWidget(top);
	XmTextFieldSetString(tf, sample);
	s = XmTextFieldGetString(tf);
	ck_assert_str_eq(s, sample);
	XtFree(s);
	ck_assert_int_eq(XmTextFieldGetLastPosition(tf), nchars);

	wvalue = XmTextFieldGetStringWcs(tf);
	ck_assert(!wcscmp(wvalue, wtext));
	XtFree((char *)wvalue);

	/* Positions count characters */
	head = chars(1, 3);
	ck_assert_int_eq(XmTextFieldGetSubstring(tf, 1, 2, sizeof buf, buf),
			 XmCOPY_SUCCEEDED);
	ck_assert_str_eq(buf, head);
	free(head);
	ck_assert_int_eq(XmTextFieldGetSubstringWcs(tf, 1, 2, 64, wbuf),
			 XmCOPY_SUCCEEDED);
	ck_assert(!wmemcmp(wbuf, wtext + 1, 2));

	XmTextFieldInsert(tf, 2, "|");
	head = chars(0, 2);
	tail = chars(2, nchars);
	s = concat(head, "|");
	expect = concat(s, tail);
	free(s);
	s = XmTextFieldGetString(tf);
	ck_assert_str_eq(s, expect);
	XtFree(s);
	free(expect);
	ck_assert_int_eq(XmTextFieldGetLastPosition(tf), nchars + 1);
	XmTextFieldReplace(tf, 0, 3, "");
	s = XmTextFieldGetString(tf);
	ck_assert_str_eq(s, tail);
	XtFree(s);
	free(head);
	free(tail);

	XmTextFieldSetStringWcs(tf, wtext);
	s = XmTextFieldGetString(tf);
	ck_assert_str_eq(s, sample);
	XtFree(s);

	XmTextFieldSetSelection(tf, 1, 4, CurrentTime);
	ck_assert(XmTextFieldGetSelectionPosition(tf, &left, &right));
	ck_assert_int_eq(left, 1);
	ck_assert_int_eq(right, 4);
	s = XmTextFieldGetSelection(tf);
	head = chars(1, 4);
	ck_assert_str_eq(s, head);
	XtFree(s);
	free(head);

	/* Characters lie left to right, one after the other */
	for (i = 0; i <= nchars; i++) {
		ck_assert(XmTextFieldPosToXY(tf, i, &x, &y));
		ck_assert_int_ge(x, last_x);
		last_x = x;
		ck_assert_int_eq(XmTextFieldXYToPos(tf, x, y), i);
	}
	XmTextFieldSetInsertionPosition(tf, nchars);
	ck_assert_int_eq(XmTextFieldGetInsertionPosition(tf), nchars);
}
END_TEST

START_TEST(text_widget)
{
	Widget t = XmCreateText(bb, "text", NULL, 0);
	char *value, *s, *sub, buf[256];
	XmTextPosition pos;
	wchar_t *wvalue;

	XtVaSetValues(t, XmNeditMode, XmMULTI_LINE_EDIT, XmNrows, 3, NULL);
	XtManageChild(t);
	XtRealizeWidget(top);
	s = concat(sample, "\n");
	value = concat(s, sample);
	free(s);
	XmTextSetString(t, value);
	ck_assert_int_eq(XmTextGetLastPosition(t), 2 * nchars + 1);

	/* Search forward and backward, in characters */
	sub = chars(2, 4);
	ck_assert(XmTextFindString(t, 0, sub, XmTEXT_FORWARD, &pos));
	ck_assert_int_eq(pos, 2);
	ck_assert(XmTextFindString(t, 2 * nchars + 1, sub, XmTEXT_BACKWARD, &pos));
	ck_assert_int_eq(pos, nchars + 1 + 2);
	ck_assert_int_eq(XmTextGetSubstring(t, nchars + 1 + 2, 2, sizeof buf, buf),
			 XmCOPY_SUCCEEDED);
	ck_assert_str_eq(buf, sub);
	free(sub);

	XmTextInsertWcs(t, nchars, L"!");
	wvalue = XmTextGetStringWcs(t);
	ck_assert_int_eq((int)wcslen(wvalue), 2 * nchars + 2);
	ck_assert(wvalue[nchars] == L'!');
	ck_assert(!wmemcmp(wvalue, wtext, (size_t)nchars));
	XtFree((char *)wvalue);
	XmTextReplace(t, nchars, nchars + 1, "");
	s = XmTextGetString(t);
	ck_assert_str_eq(s, value);
	XtFree(s);

	XmTextSetSelection(t, 1, nchars + 3, CurrentTime);
	s = XmTextGetSelection(t);
	ck_assert_int_eq(test_mbslen(s), nchars + 2);
	XtFree(s);
	free(value);
}
END_TEST

START_TEST(list_widget)
{
	Widget lw = XmCreateList(bb, "list", NULL, 0);
	XmString items[3], *selected = NULL;
	char *t2 = concat(sample, " 2"), *t3 = concat(sample, " 3");
	int *pos = NULL, npos = 0, nsel = 0, i;
	XmRenderTable rt;
	Dimension width;

	items[0] = XmStringCreateLocalized(sample);
	items[1] = XmStringCreateLocalized(t2);
	items[2] = XmStringCreateLocalized(t3);
	XmListAddItems(lw, items, 3, 0);
	XtManageChild(lw);
	XtRealizeWidget(top);

	ck_assert(XmListItemExists(lw, items[1]));
	ck_assert_int_eq(XmListItemPos(lw, items[2]), 3);
	ck_assert(XmListGetMatchPos(lw, items[0], &pos, &npos));
	ck_assert_int_eq(npos, 1);
	ck_assert_int_eq(pos[0], 1);
	XtFree((char *)pos);

	XmListSelectPos(lw, 2, False);
	XtVaGetValues(lw, XmNselectedItems, &selected, XmNselectedItemCount, &nsel,
		      NULL);
	ck_assert_int_eq(nsel, 1);
	ck_assert(XmStringCompare(selected[0], items[1]));

	/* The list is as wide as its widest item, at least */
	XtVaGetValues(lw, XmNrenderTable, &rt, XmNwidth, &width, NULL);
	ck_assert_int_ge(width, XmStringWidth(rt, items[2]));

	for (i = 0; i < 3; i++)
		XmStringFree(items[i]);
	free(t2);
	free(t3);
}
END_TEST

START_TEST(label_widget)
{
	XmString s = XmStringCreateLocalized(sample), got = NULL;
	Widget label = XmCreateLabel(bb, "label", NULL, 0);
	XmRenderTable rt;
	Dimension width;

	XtVaSetValues(label, XmNlabelString, s, NULL);
	XtManageChild(label);
	XtRealizeWidget(top);
	XtVaGetValues(label, XmNlabelString, &got, XmNrenderTable, &rt, XmNwidth, &width,
		      NULL);
	ck_assert(XmStringCompare(s, got));
	ck_assert_int_ge(width, XmStringWidth(rt, s));
	XmStringFree(got);
	XmStringFree(s);
}
END_TEST

void i18n_locale_suite(SRunner *runner)
{
	Suite *s = suite_create("I18nLocale");
	TCase *t = tcase_create("Text in the locale");

	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, locale_in_effect);
	tcase_add_test(t, xmstring);
	tcase_add_test(t, ct_utf8_segment);
	tcase_add_test(t, textfield);
	tcase_add_test(t, text_widget);
	tcase_add_test(t, list_widget);
	tcase_add_test(t, label_widget);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
