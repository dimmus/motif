/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Tests of the String to RenderTable conversion.
 *
 * The converter looks up the resources of each rendition in the resource
 * database under the widget's names and classes, so its result depends
 * on the widget.  Converted tables are cached under a key of everything
 * that the conversion reads; these tests check that a widget gets the
 * same table from the cache as it would get from a new conversion, and
 * that widgets whose lookups differ do not share tables.
 *
 * The first conversion of a string is always made by the converter
 * itself, so comparing the tables of a first and a later widget compares
 * a converted table with a cached one.  Whether two tables share their
 * data (the cache handed out the same table) is checked through the
 * handle: an XmRenderTable points to the shared table data.
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/Display.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <check.h>

#include "suites.h"

/* libXm's, not declared by an installed header. */
extern Display *_XmRenderTableDisplay(XmRenderTable table);

static Widget shell;

static int no_font_calls;

static void no_font(Widget w, XtPointer client_data, XtPointer call_data)
{
	no_font_calls++;
}

static void put(const char *line)
{
	XrmDatabase db = XtScreenDatabase(XtScreen(shell));

	XrmPutLineResource(&db, line);
}

/* A rendition resource of a core font: fontName and fontType. */
static void put_font(const char *rendition, const char *font)
{
	char line[256];

	snprintf(line, sizeof line, "%s.fontName: %s", rendition, font);
	put(line);
	snprintf(line, sizeof line, "%s.fontType: FONT_IS_FONT", rendition);
	put(line);
}

static void _init_xt(void)
{
	shell = init_xt("check_RenderTable");
}

static int warnings;

static void count_warning(String name, String type, String class, String defaultp,
			  String *params, Cardinal *num_params)
{
	warnings++;
}

/* Two tables share their data: the second is a copy of the first. */
static int shared(XmRenderTable a, XmRenderTable b)
{
	return a != NULL && b != NULL && *(XtPointer *)a == *(XtPointer *)b;
}

static XmRenderTable table_of(Widget w)
{
	XmRenderTable rt = NULL;

	XtVaGetValues(w, XmNrenderTable, &rt, NULL);
	return rt;
}

static Widget label(Widget parent, const char *name, const char *spec)
{
	return XtVaCreateWidget(name, xmLabelWidgetClass, parent,
				XtVaTypedArg, XmNrenderTable, XmRString,
				spec, (int)strlen(spec) + 1, NULL);
}

/* Everything a rendition's resources say, as text. */
static void describe_rendition(XmRendition r, char *buf, size_t size)
{
	XmStringTag tag = NULL;
	String font_name = NULL, style = NULL;
	XmFontType font_type = 0;
	unsigned char load_model = 0, underline = 0, strike = 0, fg_state = 0, bg_state = 0;
	XmTabList tabs = NULL;
	Pixel fg = 0, bg = 0;
	int font_size = 0;
	Arg a[12];
	Cardinal n = 0;

	XtSetArg(a[n], XmNtag, &tag), n++;
	XtSetArg(a[n], XmNfontName, &font_name), n++;
	XtSetArg(a[n], XmNfontType, &font_type), n++;
	XtSetArg(a[n], XmNloadModel, &load_model), n++;
	XtSetArg(a[n], XmNtabList, &tabs), n++;
	XtSetArg(a[n], XmNrenditionForeground, &fg), n++;
	XtSetArg(a[n], XmNrenditionBackground, &bg), n++;
	XtSetArg(a[n], XmNunderlineType, &underline), n++;
	XtSetArg(a[n], XmNstrikethruType, &strike), n++;
	XtSetArg(a[n], XmNforegroundState, &fg_state), n++;
	XtSetArg(a[n], XmNbackgroundState, &bg_state), n++;
	XtSetArg(a[n], XmNfontStyle, &style), n++;
	XmRenditionRetrieve(r, a, n);
	XtSetArg(a[0], XmNfontSize, &font_size);
	XmRenditionRetrieve(r, a, 1);
	snprintf(buf, size, "tag=%s name=%s type=%d load=%d tabs=%d fg=%lu bg=%lu ul=%d st=%d "
		 "fgs=%d bgs=%d style=%s size=%d",
		 tag ? tag : "(null)",
		 font_name == (String)XmAS_IS ? "AS_IS" : font_name ? font_name : "(null)",
		 (int)font_type, load_model,
		 tabs == (XmTabList)XmAS_IS ? -1 : tabs ? (int)XmTabListTabCount(tabs) : 0,
		 fg, bg, underline, strike, fg_state, bg_state, style ? style : "(null)", font_size);
}

/* A table as text, rendition by rendition. */
static void describe(XmRenderTable rt, char *buf, size_t size)
{
	XmStringTag *tags = NULL;
	int i, n;
	size_t len = 0;

	buf[0] = '\0';
	if (rt == NULL)
		return;
	n = XmRenderTableGetTags(rt, &tags);
	for (i = 0; i < n; i++) {
		XmRendition r = XmRenderTableGetRendition(rt, tags[i]);

		describe_rendition(r, buf + len, size - len);
		len = strlen(buf);
		if (len + 2 < size)
			buf[len++] = ';', buf[len] = '\0';
		XmRenditionFree(r);
		XtFree(tags[i]);
	}
	XtFree((char *)tags);
}

static char *font_name_of(XmRenderTable rt, const char *tag)
{
	static char name[128];
	XmRendition r;
	String font_name = NULL;
	Arg a[1];

	if (rt == NULL)
		return "(no table)";
	r = XmRenderTableGetRendition(rt, (XmStringTag)tag);
	name[0] = '\0';
	if (r == NULL)
		return name;
	XtSetArg(a[0], XmNfontName, &font_name);
	XmRenditionRetrieve(r, a, 1);
	if (font_name && font_name != (String)XmAS_IS)
		snprintf(name, sizeof name, "%s", font_name);
	XmRenditionFree(r);
	return name;
}

/* Widgets whose lookups find the same resources share one table, which */
/* is the table that the converter made for the first of them. */
START_TEST(same_string_same_table)
{
	char d1[1024], d2[1024];
	Widget rc, l1, l2, l3;

	put_font("*renderTable.core", "fixed");
	put_font("*renderTable.bold", "8x13bold");
	put("*renderTable.red.renditionForeground: red");
	put("*renderTable.red.underlineType: SINGLE_LINE");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	l1 = label(rc, "l", "core bold red");
	l2 = label(rc, "l", "core bold red");
	l3 = label(rc, "l", "core, bold red");
	describe(table_of(l1), d1, sizeof d1);
	describe(table_of(l2), d2, sizeof d2);
	ck_assert_str_eq(d1, d2);
	ck_assert_msg(strstr(d1, "tag=core name=fixed") != NULL, "%s", d1);
	ck_assert_msg(strstr(d1, "tag=bold name=8x13bold") != NULL, "%s", d1);
	ck_assert_msg(strstr(d1, "tag=red name=AS_IS") != NULL, "%s", d1);
	ck_assert_msg(shared(table_of(l1), table_of(l2)), "the second table was not cached");
	/* Another string with the same renditions is another key. */
	describe(table_of(l3), d2, sizeof d2);
	ck_assert_str_eq(d1, d2);
	ck_assert(!shared(table_of(l1), table_of(l3)));
	/* The widgets do not depend on each other. */
	XtDestroyWidget(l1);
	describe(table_of(l2), d2, sizeof d2);
	ck_assert_str_eq(d1, d2);
}
END_TEST

/* Resources given for a part of the widget tree only. */
START_TEST(resources_of_the_widget_path)
{
	Widget rc1, rc2, a, b, c, special, gadget;

	put_font("*renderTable.t", "fixed");
	put_font("*rc1*renderTable.t", "9x15");
	put_font("*rc2*renderTable.t", "8x13bold");
	put_font("*rc1.special.renderTable.t", "6x13");
	put_font("*rc1.XmLabelGadget.renderTable.t", "8x13");
	rc1 = XmCreateRowColumn(shell, "rc1", NULL, 0);
	rc2 = XmCreateRowColumn(shell, "rc2", NULL, 0);
	a = label(rc1, "l", "t");
	b = label(rc2, "l", "t");
	c = label(rc1, "l", "t");
	special = label(rc1, "special", "t");
	gadget = XtVaCreateWidget("g", xmLabelGadgetClass, rc1, XtVaTypedArg, XmNrenderTable,
				  XmRString, "t", 2, NULL);
	ck_assert_str_eq(font_name_of(table_of(a), "t"), "9x15");
	ck_assert_str_eq(font_name_of(table_of(b), "t"), "8x13bold");
	ck_assert_str_eq(font_name_of(table_of(c), "t"), "9x15");
	ck_assert_str_eq(font_name_of(table_of(special), "t"), "6x13");
	ck_assert_str_eq(font_name_of(table_of(gadget), "t"), "8x13");
	ck_assert(shared(table_of(a), table_of(c)));
	ck_assert(!shared(table_of(a), table_of(b)));
	ck_assert(!shared(table_of(a), table_of(special)));
	ck_assert(!shared(table_of(a), table_of(gadget)));
}
END_TEST

/* The converters of the render tables of BulletinBoard (and VendorShell) */
/* look up their own resource names; their children inherit them. */
START_TEST(resource_names_and_inheritance)
{
	Widget bb1, bb2, button, lab, text;
	XmRenderTable b1, b2;

	put_font("*renderTable.t", "fixed");
	put_font("*buttonRenderTable.t", "9x15");
	put_font("*labelRenderTable.t", "8x13bold");
	put_font("*textRenderTable.t", "6x13");
	put_font("*bb2.buttonRenderTable.t", "8x13");
	bb1 = XtVaCreateWidget("bb1", xmBulletinBoardWidgetClass, shell,
			       XtVaTypedArg, XmNbuttonRenderTable, XmRString, "t", 2,
			       XtVaTypedArg, XmNlabelRenderTable, XmRString, "t", 2,
			       XtVaTypedArg, XmNtextRenderTable, XmRString, "t", 2, NULL);
	bb2 = XtVaCreateWidget("bb2", xmBulletinBoardWidgetClass, shell,
			       XtVaTypedArg, XmNbuttonRenderTable, XmRString, "t", 2, NULL);
	XtVaGetValues(bb1, XmNbuttonRenderTable, &b1, NULL);
	XtVaGetValues(bb2, XmNbuttonRenderTable, &b2, NULL);
	ck_assert_str_eq(font_name_of(b1, "t"), "9x15");
	ck_assert_str_eq(font_name_of(b2, "t"), "8x13");
	button = XtVaCreateWidget("button", xmPushButtonWidgetClass, bb1, NULL);
	lab = XtVaCreateWidget("label", xmLabelWidgetClass, bb1, NULL);
	text = XtVaCreateWidget("text", xmTextFieldWidgetClass, bb1, NULL);
	ck_assert_str_eq(font_name_of(table_of(button), "t"), "9x15");
	ck_assert_str_eq(font_name_of(table_of(lab), "t"), "8x13bold");
	ck_assert_str_eq(font_name_of(table_of(text), "t"), "6x13");
	button = XtVaCreateWidget("button", xmPushButtonWidgetClass, bb2, NULL);
	ck_assert_str_eq(font_name_of(table_of(button), "t"), "8x13");
	/* XmNrenderTable on a child is its own conversion. */
	lab = label(bb1, "label", "t");
	ck_assert_str_eq(font_name_of(table_of(lab), "t"), "fixed");
}
END_TEST

/* A string that is a font list: XmNrenderTable and XmNfontList agree. */
START_TEST(font_list_string)
{
	char d1[1024], d2[1024], d3[1024];
	static const char spec[] = "fixed,8x13bold=bold";
	Widget rc, l1, l2, l3;

	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	l1 = label(rc, "l", spec);
	l2 = label(rc, "l", spec);
	l3 = XtVaCreateWidget("l", xmLabelWidgetClass, rc, XtVaTypedArg, XmNfontList,
			      XmRString, spec, (int)sizeof spec, NULL);
	describe(table_of(l1), d1, sizeof d1);
	describe(table_of(l2), d2, sizeof d2);
	describe(table_of(l3), d3, sizeof d3);
	ck_assert_msg(strstr(d1, "name=fixed") && strstr(d1, "tag=bold name=8x13bold"), "%s", d1);
	ck_assert_str_eq(d1, d2);
	ck_assert_str_eq(d1, d3);
	ck_assert(shared(table_of(l1), table_of(l2)));
}
END_TEST

static char *font_style_of(XmRenderTable rt, const char *tag)
{
	static char style[64];
	XmRendition r = XmRenderTableGetRendition(rt, (XmStringTag)tag);
	String s = NULL;
	Arg a[1];

	XtSetArg(a[0], XmNfontStyle, &s);
	XmRenditionRetrieve(r, a, 1);
	snprintf(style, sizeof style, "%s", s ? s : "(null)");
	XmRenditionFree(r);
	return style;
}

/* The entries of a font list look up their resources for the display, */
/* under the font list class: a change to those is seen too. */
START_TEST(font_list_display_resources)
{
	Widget rc, a, b, c;

	put("?.?.fontStyle: Bold");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	a = label(rc, "l", "fixed");
	ck_assert_str_eq(font_style_of(table_of(a), XmFONTLIST_DEFAULT_TAG), "Bold");
	put("?.?.fontStyle: Thin");
	b = label(rc, "l", "fixed");
	c = label(rc, "l", "fixed");
	ck_assert_str_eq(font_style_of(table_of(a), XmFONTLIST_DEFAULT_TAG), "Bold");
	ck_assert_str_eq(font_style_of(table_of(b), XmFONTLIST_DEFAULT_TAG), "Thin");
	ck_assert_str_eq(font_style_of(table_of(c), XmFONTLIST_DEFAULT_TAG), "Thin");
	ck_assert(!shared(table_of(a), table_of(b)));
	ck_assert(shared(table_of(b), table_of(c)));
}
END_TEST

/* Resources that no one sets keep their unspecified values. */
START_TEST(unspecified_resources)
{
	Widget rc, l[2];
	int i;

	put_font("*renderTable.t", "fixed");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	l[0] = label(rc, "l", "t");
	l[1] = label(rc, "l", "t");
	for (i = 0; i < 2; i++) {
		XmRendition r = XmRenderTableGetRendition(table_of(l[i]), "t");
		Pixel fg = 0, bg = 0;
		unsigned char ul = 0, st = 0, fgs = 0, bgs = 0, load = 0;
		XmTabList tabs = NULL;
		XmFontType type = 0;
		String style = (String)"x";
		Arg a[10];
		Cardinal n = 0;

		ck_assert(r != NULL);
		XtSetArg(a[n], XmNrenditionForeground, &fg), n++;
		XtSetArg(a[n], XmNrenditionBackground, &bg), n++;
		XtSetArg(a[n], XmNunderlineType, &ul), n++;
		XtSetArg(a[n], XmNstrikethruType, &st), n++;
		XtSetArg(a[n], XmNforegroundState, &fgs), n++;
		XtSetArg(a[n], XmNbackgroundState, &bgs), n++;
		XtSetArg(a[n], XmNloadModel, &load), n++;
		XtSetArg(a[n], XmNtabList, &tabs), n++;
		XtSetArg(a[n], XmNfontType, &type), n++;
		XtSetArg(a[n], XmNfontStyle, &style), n++;
		XmRenditionRetrieve(r, a, n);
		ck_assert_uint_eq(fg, XmUNSPECIFIED_PIXEL);
		ck_assert_uint_eq(bg, XmUNSPECIFIED_PIXEL);
		ck_assert_int_eq(ul, XmAS_IS);
		ck_assert_int_eq(st, XmAS_IS);
		ck_assert_int_eq(fgs, XmAS_IS);
		ck_assert_int_eq(bgs, XmAS_IS);
		ck_assert_int_eq(load, XmAS_IS);
		ck_assert(tabs == (XmTabList)XmAS_IS);
		ck_assert_int_eq(type, XmFONT_IS_FONT);
		ck_assert(style == NULL);
		XmRenditionFree(r);
	}
	ck_assert(shared(table_of(l[0]), table_of(l[1])));
}
END_TEST

static int destroy_calls;

static void destroyed(Widget w, XtPointer client_data, XtPointer call_data)
{
	destroy_calls++;
}

/* A rendition resource that Xt converts with a reference count (a tab */
/* list) leaves the destroy callbacks of the widget alone. */
START_TEST(tab_list_resource)
{
	XtCallbackRec callbacks[] = { { destroyed, NULL }, { NULL, NULL } };
	Widget rc, a, b;
	XmRendition r;
	XmTabList tabs = NULL;
	Arg arg[1];

	put_font("*renderTable.t", "fixed");
	put("*renderTable.t.tabList: 1in, +2in");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	destroy_calls = 0;
	a = XtVaCreateWidget("l", xmLabelWidgetClass, rc, XtVaTypedArg, XmNrenderTable,
			     XmRString, "t", 2, XmNdestroyCallback, callbacks, NULL);
	b = XtVaCreateWidget("g", xmLabelGadgetClass, rc, XtVaTypedArg, XmNrenderTable,
			     XmRString, "t", 2, XmNdestroyCallback, callbacks, NULL);
	r = XmRenderTableGetRendition(table_of(b), "t");
	XtSetArg(arg[0], XmNtabList, &tabs);
	XmRenditionRetrieve(r, arg, 1);
	ck_assert_int_eq(XmTabListTabCount(tabs), 2);
	XmRenditionFree(r);
	XtDestroyWidget(a);
	XtDestroyWidget(b);
	ck_assert_int_eq(destroy_calls, 2);
}
END_TEST

/* A change to the database is seen by the next conversion. */
START_TEST(database_change)
{
	Widget rc, a, b, c;

	put_font("*renderTable.t", "fixed");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	a = label(rc, "l", "t");
	put_font("*renderTable.t", "9x15");
	b = label(rc, "l", "t");
	put_font("*renderTable.t", "fixed");
	c = label(rc, "l", "t");
	ck_assert_str_eq(font_name_of(table_of(a), "t"), "fixed");
	ck_assert_str_eq(font_name_of(table_of(b), "t"), "9x15");
	ck_assert_str_eq(font_name_of(table_of(c), "t"), "fixed");
	ck_assert(!shared(table_of(a), table_of(b)));
}
END_TEST

/* A rendition keeps no pointer into the database, whose values can */
/* change while the rendition is in use. */
START_TEST(no_pointer_into_database)
{
	Widget rc, a;
	XmRendition r;
	String style = NULL;
	Arg arg[1];

	put_font("*renderTable.t", "fixed");
	put("*renderTable.t.fontStyle: Bold");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	a = label(rc, "l", "t");
	put("*renderTable.t.fontStyle: Thin");
	r = XmRenderTableGetRendition(table_of(a), "t");
	XtSetArg(arg[0], XmNfontStyle, &style);
	XmRenditionRetrieve(r, arg, 1);
	ck_assert_str_eq(style, "Bold");
	XmRenditionFree(r);
}
END_TEST

/* A font that fails to load gives the warning or XmNnoFontCallback */
/* call of a new table to each widget.  (An empty font set name fails */
/* without Xt falling back to a default font.) */
START_TEST(font_failure_each_time)
{
	Widget rc, l[3];
	int i;

	put("*renderTable.t.fontName:");
	put("*renderTable.t.fontType: FONT_IS_FONTSET");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	XtAppSetWarningMsgHandler(app, count_warning);
	warnings = 0;
	for (i = 0; i < 3; i++)
		l[i] = label(rc, "l", "t");
	ck_assert_int_eq(warnings, 3);
	ck_assert(!shared(table_of(l[0]), table_of(l[1])));
	XtAddCallback(XmGetXmDisplay(XtDisplay(shell)), XmNnoFontCallback, no_font, NULL);
	no_font_calls = 0;
	warnings = 0;
	for (i = 0; i < 3; i++)
		l[i] = label(rc, "l", "t");
	ck_assert_int_eq(no_font_calls, 3);
	ck_assert_int_eq(warnings, 0);
}
END_TEST

/* A font that Xt replaces with a default one warns once, as Xt */
/* caches the conversion. */
START_TEST(font_fallback)
{
	char d1[1024], d2[1024];
	Widget rc, l[3];
	int i;

	put_font("*renderTable.t", "-no-such-font-*");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	XtAppSetWarningMsgHandler(app, count_warning);
	warnings = 0;
	for (i = 0; i < 3; i++)
		l[i] = label(rc, "l", "t");
	ck_assert_int_eq(warnings, 1);
	describe(table_of(l[0]), d1, sizeof d1);
	describe(table_of(l[2]), d2, sizeof d2);
	ck_assert_str_eq(d1, d2);
}
END_TEST

/* A resource that fails to convert warns for each widget. */
START_TEST(conversion_failure_each_time)
{
	Widget rc, l[3];
	int i;

	put_font("*renderTable.t", "fixed");
	put("*renderTable.t.underlineType: NO_SUCH_LINE");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	XtAppSetWarningMsgHandler(app, count_warning);
	warnings = 0;
	for (i = 0; i < 3; i++)
		l[i] = label(rc, "l", "t");
	ck_assert_int_eq(warnings, 3);
	ck_assert(!shared(table_of(l[0]), table_of(l[1])));
	ck_assert_str_eq(font_name_of(table_of(l[2]), "t"), "fixed");
}
END_TEST

/* A font that is loaded on first use is loaded for each widget. */
START_TEST(deferred_font)
{
	Widget rc, a, b;

	put_font("*renderTable.t", "fixed");
	put("*renderTable.t.loadModel: LOAD_DEFERRED");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	a = XtVaCreateWidget("l", xmLabelWidgetClass, rc, XtVaTypedArg, XmNrenderTable,
			     XmRString, "t", 2, XmNlabelString, NULL, NULL);
	b = XtVaCreateWidget("l", xmLabelWidgetClass, rc, XtVaTypedArg, XmNrenderTable,
			     XmRString, "t", 2, XmNlabelString, NULL, NULL);
	ck_assert(!shared(table_of(a), table_of(b)));
}
END_TEST

/* A table in use stays cached while many other strings are converted. */
START_TEST(table_in_use_stays_cached)
{
	Widget rc, a, b;
	char spec[32];
	int i;

	put_font("*renderTable.t", "fixed");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	a = label(rc, "l", "t");
	for (i = 0; i < 200; i++) {
		snprintf(spec, sizeof spec, "t u%d", i);
		XtDestroyWidget(label(rc, "l", spec));
	}
	b = label(rc, "l", "t");
	ck_assert(shared(table_of(a), table_of(b)));
}
END_TEST

/* The font of the rendition tagged tag of rt. */
static XFontStruct *font_of(XmRenderTable rt, const char *tag)
{
	XmRendition r = XmRenderTableGetRendition(rt, (XmStringTag)tag);
	XFontStruct *font = NULL;
	Arg a[1];

	ck_assert(r != NULL);
	XtSetArg(a[0], XmNfont, &font);
	XmRenditionRetrieve(r, a, 1);
	XmRenditionFree(r);
	return font;
}

/* The font that display has for name now. */
static XFontStruct *loaded_font(Display *dpy, const char *name)
{
	XmFontListEntry e = XmFontListEntryLoad(dpy, (char *)name, XmFONT_IS_FONT, "x");
	XmFontType type;
	XFontStruct *font = (XFontStruct *)XmFontListEntryGetFont(e, &type);

	XmFontListEntryFree(&e);
	return font;
}

/* The default render table (XmeGetDefaultRenderTable) of a display goes */
/* away with it: a display opened later, even at the same address, gets */
/* one of its own, with its fonts. */
START_TEST(default_render_table_display_close)
{
	int i;

	for (i = 0; i < 3; i++) {
		int argc = 0;
		Display *dpy = XtOpenDisplay(app, NULL, "second", "Second", NULL, 0, &argc, NULL);
		Widget top, rc, l;

		ck_assert(dpy != NULL);
		top = XtVaAppCreateShell("second", "Second", applicationShellWidgetClass, dpy, NULL);
		rc = XmCreateRowColumn(top, "rc", NULL, 0);
		l = XtVaCreateWidget("l", xmLabelWidgetClass, rc, NULL);
		ck_assert(_XmRenderTableDisplay(table_of(l)) == dpy);
		/* XmDEFAULT_FONT is "fixed". */
		ck_assert(font_of(table_of(l), XmFONTLIST_DEFAULT_TAG) == loaded_font(dpy, "fixed"));
		XtDestroyWidget(top);
		XtCloseDisplay(dpy);
	}
}
END_TEST

/* The tables of a display go away with it: a display opened later, */
/* even at the same address, gets tables of its own, with its fonts. */
START_TEST(display_close)
{
	int i;

	for (i = 0; i < 3; i++) {
		int argc = 0;
		Display *dpy = XtOpenDisplay(app, NULL, "second", "Second", NULL, 0, &argc, NULL);
		Widget top, rc, l;
		XFontStruct *font;

		ck_assert(dpy != NULL);
		top = XtVaAppCreateShell("second", "Second", applicationShellWidgetClass, dpy, NULL);
		rc = XmCreateRowColumn(top, "rc", NULL, 0);
		l = label(rc, "l", "fixed");
		ck_assert(_XmRenderTableDisplay(table_of(l)) == dpy);
		font = font_of(table_of(l), XmFONTLIST_DEFAULT_TAG);
		ck_assert(font == loaded_font(dpy, "fixed"));
		ck_assert_int_gt(XTextWidth(font, "x", 1), 0);
		XtDestroyWidget(top);
		XtCloseDisplay(dpy);
	}
}
END_TEST

void rendertable_suite(SRunner *runner)
{
	Suite *s = suite_create("RenderTable");
	TCase *t = tcase_create("String to RenderTable");

	tcase_add_test(t, same_string_same_table);
	tcase_add_test(t, resources_of_the_widget_path);
	tcase_add_test(t, resource_names_and_inheritance);
	tcase_add_test(t, font_list_string);
	tcase_add_test(t, font_list_display_resources);
	tcase_add_test(t, unspecified_resources);
	tcase_add_test(t, tab_list_resource);
	tcase_add_test(t, database_change);
	tcase_add_test(t, no_pointer_into_database);
	tcase_add_test(t, font_failure_each_time);
	tcase_add_test(t, font_fallback);
	tcase_add_test(t, conversion_failure_each_time);
	tcase_add_test(t, deferred_font);
	tcase_add_test(t, table_in_use_stays_cached);
	tcase_add_test(t, display_close);
	tcase_add_test(t, default_render_table_display_close);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
