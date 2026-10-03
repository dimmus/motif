/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Headless i18n tests: locale setup and the locale-dependent XmString
 * conversions that do not need an X server (multibyte <-> wide char,
 * UTF-8, locale text).  A case that needs a locale which is not
 * installed skips (does not fail) so the suite runs anywhere.  Compound
 * text, which Xlib converts through the display, is in the XmStringCT
 * suite.
 */
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmStringI.h>
#include <check.h>

#include "suites.h"

/* Try to set one of several locale names; return the one that worked. */
static const char *set_locale(const char *const *names)
{
	const char *r;
	int i;

	for (i = 0; names[i]; i++)
		if ((r = setlocale(LC_CTYPE, names[i])) != NULL)
			return names[i];
	return NULL;
}

/* The string survives a byte-stream round trip with the same text */
static void assert_byte_stream_ok(XmString s)
{
	unsigned char *stream = NULL;
	unsigned int len;
	XmString back;
	char *a, *b;

	len = XmCvtXmStringToByteStream(s, &stream);
	ck_assert_uint_gt(len, 0);
	back = XmCvtByteStreamToXmString(stream);
	ck_assert_ptr_nonnull(back);
	a = (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				    NULL, 0, XmOUTPUT_ALL);
	b = (char *)XmStringUnparse(back, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				    NULL, 0, XmOUTPUT_ALL);
	ck_assert_pstr_eq(a, b);
	XtFree(a);
	XtFree(b);
	XmStringFree(back);
	XtFree((char *)stream);
}

static char *unparse(XmString s, XmTextType type)
{
	return (char *)XmStringUnparse(s, NULL, type, type, NULL, 0,
				       XmOUTPUT_ALL);
}

/* UTF-8 round trip through XmString in a UTF-8 locale */
START_TEST(utf8_locale_roundtrip)
{
	static const char *const utf8[] = { "en_US.UTF-8", "C.UTF-8",
					    "C.utf8", NULL };
	/* "grüße" and a euro sign */
	static const char text[] = "gr\xc3\xbc\xc3\x9f" "e \xe2\x82\xac";
	XmString s;
	char *back;

	if (!set_locale(utf8)) {
		printf("# SKIP utf8_locale_roundtrip: no UTF-8 locale\n");
		return;
	}
	s = XmStringCreateLocalized((char *)text);
	ck_assert_ptr_nonnull(s);
	back = unparse(s, XmCHARSET_TEXT);
	ck_assert_ptr_nonnull(back);
	ck_assert_str_eq(back, text);
	XtFree(back);
	ck_assert(!XmStringEmpty(s));
	assert_byte_stream_ok(s);
	XmStringFree(s);

#if XM_UTF8
	s = XmStringGenerate((XtPointer)text, "UTF-8", XmCHARSET_TEXT, NULL);
	back = XmCvtXmStringToUTF8String(s);
	ck_assert_ptr_nonnull(back);
	ck_assert_str_eq(back, text);
	XtFree(back);
	XmStringFree(s);
#endif
	setlocale(LC_CTYPE, "C");
}
END_TEST

/* Multibyte text in the C locale is plain ASCII and round-trips */
START_TEST(c_locale_text)
{
	XmString s;
	char *back;

	setlocale(LC_CTYPE, "C");
	s = XmStringCreateLocalized("plain ascii 123");
	back = unparse(s, XmCHARSET_TEXT);
	ck_assert_str_eq(back, "plain ascii 123");
	XtFree(back);
	XmStringFree(s);
}
END_TEST

/* Wide-char text components, converted with the locale's wcstombs */
START_TEST(widechar_component)
{
	static const char *const utf8[] = { "en_US.UTF-8", "C.UTF-8",
					    "C.utf8", NULL };
	wchar_t wide[16];
	XmString s;
	char *back;
	size_t n;

	if (!set_locale(utf8)) {
		printf("# SKIP widechar_component: no UTF-8 locale\n");
		return;
	}
	n = mbstowcs(wide, "caf\xc3\xa9", 16);
	ck_assert_uint_ne(n, (size_t)-1);

	s = XmStringComponentCreate(XmSTRING_COMPONENT_WIDECHAR_TEXT,
				    (unsigned)(n * sizeof(wchar_t)),
				    (char *)wide);
	ck_assert_ptr_nonnull(s);
	back = unparse(s, XmCHARSET_TEXT);
	ck_assert_ptr_nonnull(back);
	ck_assert_str_eq(back, "caf\xc3\xa9");
	XtFree(back);
	assert_byte_stream_ok(s);
	XmStringFree(s);
	setlocale(LC_CTYPE, "C");
}
END_TEST

/* XmStringCreateLocalized must set XmNlanguage-independent defaults that
 * XmGetXmDisplay is not needed for (no display). */
START_TEST(direction_default)
{
	XmString s = XmStringCreateLocalized("abc");
	XmStringDirection dir;
	XmStringContext ctx;
	unsigned int len;
	XtPointer val;
	XmStringComponentType t;

	setlocale(LC_CTYPE, "C");
	dir = XmSTRING_DIRECTION_UNSET;
	ck_assert(XmStringInitContext(&ctx, s));
	while ((t = XmStringGetNextTriple(ctx, &len, &val)) !=
	       XmSTRING_COMPONENT_END) {
		if (t == XmSTRING_COMPONENT_DIRECTION)
			dir = (XmStringDirection)(long)val;
		XtFree((char *)val);
	}
	XmStringFreeContext(ctx);
	/* Left-to-right or unset for a Latin string */
	ck_assert(dir == XmSTRING_DIRECTION_L_TO_R ||
		  dir == XmSTRING_DIRECTION_UNSET);
	XmStringFree(s);
}
END_TEST

void i18n_suite(SRunner *runner)
{
	Suite *s = suite_create("I18n");
	TCase *t = tcase_create("Locale conversions");

	tcase_add_test(t, utf8_locale_roundtrip);
	tcase_add_test(t, c_locale_text);
	tcase_add_test(t, widechar_component);
	tcase_add_test(t, direction_default);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
