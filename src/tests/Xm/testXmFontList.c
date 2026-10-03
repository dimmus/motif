/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <check.h>

#include "suites.h"

/* LeakSanitizer is part of ASan with GCC (__SANITIZE_ADDRESS__) and Clang. */
#if defined(__SANITIZE_ADDRESS__)
#define HAVE_LSAN 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define HAVE_LSAN 1
#endif
#endif
#ifdef HAVE_LSAN
#include <sanitizer/lsan_interface.h>
#define KNOWN_LEAK_BEGIN() __lsan_disable()
#define KNOWN_LEAK_END() __lsan_enable()
#else
#define KNOWN_LEAK_BEGIN() ((void)0)
#define KNOWN_LEAK_END() ((void)0)
#endif

static Display *display;

static void _init_xt(void)
{
	display = XtDisplay(init_xt("check_XmFontList"));
}

/* Compare the tags of two entries; XmFontListEntryGetTag returns copies. */
static int same_tag(XmFontListEntry a, XmFontListEntry b)
{
	char *ta, *tb;
	int same;

	if (!a || !b)
		return 0;
	ta = XmFontListEntryGetTag(a);
	tb = XmFontListEntryGetTag(b);
	same = ta && tb && !strcmp(ta, tb);
	XtFree(ta);
	XtFree(tb);
	return same;
}

/*
 * The entries are freed before the font lists that hold them.
 * XmRenderTableFree and XmFontListRemoveEntry free a rendition's handle
 * only when they drop the last reference to the rendition, so freeing a
 * list, or removing an entry from it, while the entry it was built from
 * is still alive leaks the list's 8-byte handle.  This is a library bug:
 * a table slot cannot tell whether its handle is its own (CopyRendition)
 * or shared with a copy of the table (DuplicateRendition).
 * XmRenditionFree always frees its handle, so this order frees
 * everything; where the entry has to outlive the list, the leaking
 * allocation is excluded from leak checking with KNOWN_LEAK_BEGIN/END.
 */

START_TEST(create_from_invalid_entry)
{
	XmFontList fl;

	ck_assert_msg(!(fl = XmFontListAppendEntry(NULL, NULL)),
	              "Adding a NULL entry should fail");
	if (fl) XmFontListFree(fl);
}
END_TEST

START_TEST(create_from_valid_entry)
{
	XmFontList fl;
	XmFontListEntry e;

	e = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	ck_assert_msg((e && (fl = XmFontListAppendEntry(NULL, e))),
	              "Failed to create a list from a valid entry");

	XmFontListEntryFree(&e);
	if (fl) XmFontListFree(fl);
}
END_TEST

START_TEST(add_invalid_entry)
{
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);

	ck_assert_msg(fl, "Failed to create the font list");
	ck_assert_msg((XmFontListAppendEntry(fl, NULL) == fl),
	              "Shouldn't be able to add a NULL entry");
	XmFontListEntryFree(&e);
	XmFontListFree(fl);
}
END_TEST

START_TEST(add_valid_entry)
{
	XmFontList fl, fx;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);

	ck_assert_msg(fl, "Failed to create the font list");
	ck_assert_msg(e,  "Failed to create font list entry");
	ck_assert_msg((fl && e2 && (fx = XmFontListAppendEntry(fl, e2))),
	              "Failed to add a valid entry");
	ck_assert(fx && fx != fl);

	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
	if (fx) XmFontListFree(fx);
	else XmFontListFree(fl);
}
END_TEST

START_TEST(remove_entry_invalid_list)
{
	XmFontListEntry e;

	e = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	ck_assert_msg(!XmFontListRemoveEntry(NULL, e), "Unexpected result");
	XmFontListEntryFree(&e);
}
END_TEST

START_TEST(remove_entry_invalid_entry)
{
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(XmFontListRemoveEntry(fl, NULL) == fl, "Unexpected result");
	XmFontListEntryFree(&e);
	XmFontListFree(fl);
}
END_TEST

START_TEST(remove_entry_not_in_list)
{
	XmFontList fl, fx;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	ck_assert_msg((fx = XmFontListRemoveEntry(fl, e2)) == fl,
	              "Unexpected return value");
	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
	XmFontListFree(fl);
}
END_TEST

START_TEST(remove_entry)
{
	XmFontList fl, fx;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	KNOWN_LEAK_BEGIN();
	fl = XmFontListAppendEntry(fl, e2);
	KNOWN_LEAK_END();
	ck_assert_msg((fx = XmFontListRemoveEntry(fl, e2)) != fl,
	              "Unexpected return value");
	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
	XmFontListFree(fx);
}
END_TEST

START_TEST(remove_sole_entry)
{
	XmFontList fl, fx;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	KNOWN_LEAK_BEGIN();
	fl = XmFontListAppendEntry(NULL, e);
	KNOWN_LEAK_END();
	ck_assert_msg((fx = XmFontListRemoveEntry(fl, e)) != fl,
	              "Unexpected return value");
	XmFontListEntryFree(&e);
	if (fx) XmFontListFree(fx);
}
END_TEST

START_TEST(copy_list_invalid)
{
	XmFontList fl;
	ck_assert_msg(!(fl = XmFontListCopy(NULL)), "Expected result to be NULL");
	if (fl) XmFontListFree(fl);
}
END_TEST

START_TEST(copy_list)
{
	XmFontList fl, fx;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(fl != (fx = XmFontListCopy(fl)) && fx, "Unexpected result");
	XmFontListEntryFree(&e);
	if (fx) XmFontListFree(fx);
	XmFontListFree(fl);
}
END_TEST

START_TEST(font_context_null_return)
{
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(!XmFontListInitFontContext(NULL, fl),
	              "Should fail if fc is NULL");
	XmFontListEntryFree(&e);
	XmFontListFree(fl);
}
END_TEST

START_TEST(font_context_empty_list)
{
	XmFontContext fc = NULL;

	ck_assert_msg(!XmFontListInitFontContext(&fc, NULL) && !fc,
	              "Should fail if the list is empty");
	if (fc) XmFontListFreeFontContext(fc);
}
END_TEST

START_TEST(enum_null_context)
{
	ck_assert_msg(!XmFontListNextEntry(NULL), "Should fail if no context");
}
END_TEST

START_TEST(enum_first_entry)
{
	XmFontContext fc;
	XmFontList fl;
	XmFontListEntry e, e2;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, XmSTRING_DEFAULT_CHARSET);
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(XmFontListInitFontContext(&fc, fl) && fc,
	              "Should be able to create a font context from a font list");

	e2 = XmFontListNextEntry(fc);
	ck_assert_msg(same_tag(e2, e),
	              "Failed to get first entry");
	XmFontListEntryFree(&e);
	XmFontListFreeFontContext(fc);
	XmFontListFree(fl);
}
END_TEST

START_TEST(enum_second_entry)
{
	XmFontContext fc;
	XmFontList fl;
	XmFontListEntry e, e2, e3;

	e  = XmFontListEntryLoad(display, "fixed",    XmFONT_IS_FONT, "FIRST-ENTRY");
	e2 = XmFontListEntryLoad(display, "8x13bold", XmFONT_IS_FONT, "SECOND-ENTRY");
	fl = XmFontListAppendEntry(XmFontListAppendEntry(NULL, e), e2);
	ck_assert_msg(XmFontListInitFontContext(&fc, fl) && fc,
	              "Should be able to create a font context from a font list");
	e3 = XmFontListNextEntry(fc);
	ck_assert_msg(same_tag(e3, e),
	              "Failed to get first entry");
	e3 = XmFontListNextEntry(fc);
	ck_assert_msg(same_tag(e3, e2),
	              "Failed to get second entry");

	XmFontListEntryFree(&e);
	XmFontListEntryFree(&e2);
	XmFontListFreeFontContext(fc);
	XmFontListFree(fl);
}
END_TEST

START_TEST(enum_beyond_the_end)
{
	XmFontContext fc;
	XmFontList fl;
	XmFontListEntry e;

	e  = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT, "FIRST-ENTRY");
	fl = XmFontListAppendEntry(NULL, e);
	ck_assert_msg(XmFontListInitFontContext(&fc, fl) && fc,
	              "Should be able to create a font context from a font list");
	ck_assert_msg(same_tag(XmFontListNextEntry(fc), e),
	              "Failed to get first entry");
	ck_assert_msg(!XmFontListNextEntry(fc),
	              "Should get NULL beyond the end of the list");
	XmFontListEntryFree(&e);
	XmFontListFreeFontContext(fc);
	XmFontListFree(fl);
}
END_TEST

void xmfontlist_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmFontList");

	t = tcase_create("Create font list");
	tcase_add_test(t, create_from_invalid_entry);
	tcase_add_test(t, create_from_valid_entry);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Add an entry to a font list");
	tcase_add_test(t, add_invalid_entry);
	tcase_add_test(t, add_valid_entry);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Remove an entry from a font list");
	tcase_add_test(t, remove_entry_invalid_list);
	tcase_add_test(t, remove_entry_invalid_entry);
	tcase_add_test(t, remove_entry_not_in_list);
	tcase_add_test(t, remove_entry);
	tcase_add_test(t, remove_sole_entry);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Copy a font list");
	tcase_add_test(t, copy_list_invalid);
	tcase_add_test(t, copy_list);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Enumerate a font list");
	tcase_add_test(t, font_context_null_return);
	tcase_add_test(t, font_context_empty_list);
	tcase_add_test(t, enum_null_context);
	tcase_add_test(t, enum_first_entry);
	tcase_add_test(t, enum_second_entry);
	tcase_add_test(t, enum_beyond_the_end);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

