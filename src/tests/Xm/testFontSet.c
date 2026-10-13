/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * The default font of a font set font list (XmeRenderTableGetDefaultFont,
 * XmFontListGetNextFont, _XmGetFirstFont).  Xlib can leave the fonts of
 * a font set unloaded until it draws with them: the generic XOM of the
 * UTF-8 locales does, and the XFontStructs that XFontsOfFontSet returns
 * then have fid 0 until text is drawn or measured with the font set.
 * Motif loads the first one itself, so that callers get a font they can
 * set in a GC (mwm's MakeAppearanceResources, Label, Scale).
 *
 * CTest runs the suite as Xm.FontSet in a UTF-8 locale if there is one,
 * as Xm.FontSet.C in the C locale, and, as Xm.FontSet.<locale>, in the
 * locales generated at build time (see i18n.cmake).
 */
#include <locale.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <check.h>

#include "XmStringI.h"
#include "i18n_util.h"
#include "suites.h"

/* The misc fonts, which the X servers of the CI have; "fixed" else */
#define FONTSET_13 "-misc-fixed-medium-r-normal--13-*-*-*-*-*-*-*,fixed"
#define FONTSET_20 "-misc-fixed-medium-r-normal--20-*-*-*-*-*-*-*,fixed"
/* FONTSET_13 as a font list resource value, as mwm's fontList: a font */
/* set ends in ':' and its base names are separated by ';' */
#define FONTLIST_13 "-misc-fixed-medium-r-normal--13-*-*-*-*-*-*-*;fixed:"

static Display *display;
static Widget top;
static int x_errors;

static int count_x_error(Display *dpy, XErrorEvent *ev)
{
	(void)dpy;
	(void)ev;
	x_errors++;
	return 0;
}

static void setup(void)
{
	static const char *const utf8[] = { "C.UTF-8", "en_US.UTF-8", NULL };

	ck_assert_ptr_nonnull(test_set_locale(utf8));
	top = init_xt("check_FontSet");
	display = XtDisplay(top);
	XSetErrorHandler(count_x_error);
	x_errors = 0;
}

static void teardown(void)
{
	XSetErrorHandler(NULL);
	uninit_xt();
}

static XFontSet create_font_set(const char *base)
{
	char **missing, *def;
	int n_missing;
	XFontSet fs = XCreateFontSet(display, base, &missing, &n_missing, &def);

	ck_assert_msg(fs != NULL, "no font set for %s in %s", base, setlocale(LC_CTYPE, NULL));
	if (missing)
		XFreeStringList(missing);
	return fs;
}

static XFontStruct *first_font_of(XFontSet fs)
{
	XFontStruct **fonts;
	char **names;

	ck_assert_int_gt(XFontsOfFontSet(fs, &fonts, &names), 0);
	return fonts[0];
}

/* The font set of the first entry of fl */
static XFontSet font_set_of(XmFontList fl)
{
	XmFontContext ctx;
	XmFontListEntry e;
	XmFontType type;
	XtPointer font;

	ck_assert(XmFontListInitFontContext(&ctx, fl));
	e = XmFontListNextEntry(ctx);
	ck_assert_ptr_nonnull(e);
	font = XmFontListEntryGetFont(e, &type);
	XmFontListFreeFontContext(ctx);
	ck_assert_int_eq(type, XmFONT_IS_FONTSET);
	return (XFontSet)font;
}

/*
 * f is a loaded font, usable in a GC, with the metrics of the first
 * font of fs.
 */
static void check_font(XFontStruct *f, XFontSet fs)
{
	XFontStruct *first = first_font_of(fs), *q;
	XGCValues v;
	GC gc;

	ck_assert_ptr_nonnull(f);
	ck_assert_msg(f->fid != 0, "the default font is not loaded (fid 0) in %s",
		      setlocale(LC_CTYPE, NULL));
	/* XOM's own font when it is loaded, else one of Motif's */
	if (first->fid != 0)
		ck_assert_ptr_eq(f, first);
	ck_assert_int_eq(f->ascent, first->ascent);
	ck_assert_int_eq(f->descent, first->descent);
	ck_assert_int_eq(f->max_bounds.width, first->max_bounds.width);
	q = XQueryFont(display, f->fid);
	ck_assert_ptr_nonnull(q);
	ck_assert_int_eq(q->ascent, f->ascent);
	XFreeFontInfo(NULL, q, 1);
	v.font = f->fid;
	gc = XCreateGC(display, DefaultRootWindow(display), GCFont, &v);
	XFreeGC(display, gc);
	XSync(display, False);
	ck_assert_int_eq(x_errors, 0);
}

static XmFontList convert(const char *spec)
{
	XmFontList fl = NULL;
	XrmValue from, to;

	from.addr = (XPointer)spec;
	from.size = strlen(spec) + 1;
	to.addr = (XPointer)&fl;
	to.size = sizeof fl;
	ck_assert(XtConvertAndStore(top, XmRString, &from, XmRFontList, &to));
	ck_assert_ptr_nonnull(fl);
	return fl;
}

/* A font set font list converted from a string */
START_TEST(converted_default_font)
{
	XmFontList fl = convert(FONTLIST_13);
	XFontStruct *f = NULL, *again = NULL;

	ck_assert_msg(XmeRenderTableGetDefaultFont(fl, &f),
		      "no default font for a font set in %s", setlocale(LC_CTYPE, NULL));
	check_font(f, font_set_of(fl));
	/* Loaded once */
	ck_assert(XmeRenderTableGetDefaultFont(fl, &again));
	ck_assert_ptr_eq(again, f);
}
END_TEST

/* Text measured with the font set loads its fonts in Xlib: the default */
/* font is then the font set's own, and the earlier one stays valid. */
START_TEST(measured_font_set)
{
	XmFontList fl = convert(FONTLIST_13);
	XFontSet fs = font_set_of(fl);
	XFontStruct *before = NULL, *after = NULL, *q;

	ck_assert(XmeRenderTableGetDefaultFont(fl, &before));
	check_font(before, fs);
	ck_assert_int_gt(XmbTextEscapement(fs, "x", 1), 0);
	ck_assert(XmeRenderTableGetDefaultFont(fl, &after));
	check_font(after, fs);
	q = XQueryFont(display, before->fid);
	ck_assert_ptr_nonnull(q);
	XFreeFontInfo(NULL, q, 1);
	XSync(display, False);
	ck_assert_int_eq(x_errors, 0);
}
END_TEST

START_TEST(get_next_font)
{
	XmFontList fl = convert(FONTLIST_13);
	XmFontContext ctx;
	XmStringCharSet tag = NULL;
	XFontStruct *f = NULL, *def = NULL;

	ck_assert(XmFontListInitFontContext(&ctx, fl));
	ck_assert(XmFontListGetNextFont(ctx, &tag, &f));
	XmFontListFreeFontContext(ctx);
	XtFree(tag);
	check_font(f, font_set_of(fl));
	ck_assert(XmeRenderTableGetDefaultFont(fl, &def));
	ck_assert_ptr_eq(def, f);
}
END_TEST

/* A font set of the application's own, in an entry without a display */
START_TEST(created_entry)
{
	XFontSet fs = create_font_set(FONTSET_13);
	XmFontListEntry e;
	XmFontList fl;
	XFontStruct *f = NULL;

	e = XmFontListEntryCreate(XmFONTLIST_DEFAULT_TAG, XmFONT_IS_FONTSET, (XtPointer)fs);
	ck_assert_ptr_nonnull(e);
	fl = XmFontListAppendEntry(NULL, e);
	XmFontListEntryFree(&e);
	ck_assert(XmeRenderTableGetDefaultFont(fl, &f));
	check_font(f, fs);
	XmFontListFree(fl);
	XFreeFontSet(display, fs);
	XSync(display, False);
	ck_assert_int_eq(x_errors, 0);
}
END_TEST

/* A rendition given another font set has the first font of that one */
START_TEST(rendition_update)
{
	XFontSet fs13 = create_font_set(FONTSET_13);
	XFontSet fs20 = create_font_set(FONTSET_20);
	XmRendition r;
	XmRenderTable rt, rt2;
	XFontStruct *f13, *f, *g = NULL;
	Arg args[2];
	Cardinal n = 0;

	XtSetArg(args[n], XmNfontType, XmFONT_IS_FONTSET), n++;
	XtSetArg(args[n], XmNfont, (XtPointer)fs13), n++;
	r = XmRenditionCreate(top, XmFONTLIST_DEFAULT_TAG, args, n);
	ck_assert_ptr_nonnull(r);

	/* Changed in place: r is not shared */
	f13 = _XmGetFirstFont(r);
	check_font(f13, fs13);
	n = 0;
	XtSetArg(args[n], XmNfont, (XtPointer)fs20), n++;
	XmRenditionUpdate(r, args, n);
	f = _XmGetFirstFont(r);
	check_font(f, fs20);

	/* Shared with a table: the table keeps the font it has */
	rt = XmRenderTableAddRenditions(NULL, &r, 1, XmMERGE_REPLACE);
	ck_assert(XmeRenderTableGetDefaultFont(rt, &g));
	ck_assert_ptr_eq(g, f);
	n = 0;
	XtSetArg(args[n], XmNfont, (XtPointer)fs13), n++;
	XmRenditionUpdate(r, args, n);
	check_font(_XmGetFirstFont(r), fs13);
	ck_assert(XmeRenderTableGetDefaultFont(rt, &g));
	ck_assert_ptr_eq(g, f);
	rt2 = XmRenderTableAddRenditions(NULL, &r, 1, XmMERGE_REPLACE);
	ck_assert(XmeRenderTableGetDefaultFont(rt2, &g));
	check_font(g, fs13);

	XmRenderTableFree(rt);
	XmRenderTableFree(rt2);
	XmRenditionFree(r);
	XFreeFontSet(display, fs13);
	XFreeFontSet(display, fs20);
	XSync(display, False);
	ck_assert_int_eq(x_errors, 0);
}
END_TEST

void fontset_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("FontSet");

	t = tcase_create("Default font of a font set");
	tcase_add_test(t, converted_default_font);
	tcase_add_test(t, measured_font_set);
	tcase_add_test(t, get_next_font);
	tcase_add_test(t, created_entry);
	tcase_add_test(t, rendition_update);
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_set_timeout(t, 10);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
