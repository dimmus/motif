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
 * a converted table with a cached one.  A widget gets a table of its own
 * either way, so whether its table came from the cache is seen through
 * the conversions of the rendition resources: the tests count those of
 * fontType and underlineType, which only a new conversion makes.
 *
 * The "Rendition handles" tests check that the render table functions
 * free what they hold when renditions are shared between tables.
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

#include "leak.h"
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

/* Conversions of rendition resources (fontType, underlineType). */
static int conversions, conversions_seen;

/* The conversions made since the last call. */
static int converted(void)
{
	int n = conversions - conversions_seen;

	conversions_seen = conversions;
	return n;
}

static Boolean store(XrmValue *to, XtPointer value, Cardinal size)
{
	if (to->addr == NULL)
		to->addr = (XPointer)value;
	else if (to->size < size) {
		to->size = size;
		return False;
	}
	else
		memcpy(to->addr, value, size);
	to->size = size;
	return True;
}

/* Index of the name (with or without "Xm") in names, or -1 after a */
/* warning. */
static int lookup(Display *dpy, XrmValue *from, const char *type,
		  const char *const *names, int n)
{
	const char *s = (const char *)from->addr;
	int i;

	conversions++;
	if (strncasecmp(s, "Xm", 2) == 0)
		s += 2;
	for (i = 0; i < n; i++)
		if (strcasecmp(s, names[i]) == 0)
			return i;
	XtDisplayStringConversionWarning(dpy, (char *)from->addr, (char *)type);
	return -1;
}

/* String to XmRFontType (an int) and XmRLineType (an unsigned char), */
/* as libXm converts them, counting the conversions.  Not cached by Xt. */
static Boolean cvt_font_type(Display *dpy, XrmValue *args, Cardinal *num_args,
			     XrmValue *from, XrmValue *to, XtPointer *data)
{
	static const char *const names[] = { "FONT_IS_FONT", "FONT_IS_FONTSET" };
	static int value;
	int i = lookup(dpy, from, XmRFontType, names, XtNumber(names));

	if (i < 0)
		return False;
	value = i == 0 ? XmFONT_IS_FONT : XmFONT_IS_FONTSET;
	return store(to, &value, sizeof value);
}

static Boolean cvt_line_type(Display *dpy, XrmValue *args, Cardinal *num_args,
			     XrmValue *from, XrmValue *to, XtPointer *data)
{
	static const char *const names[] = { "NO_LINE", "SINGLE_LINE", "DOUBLE_LINE" };
	static const unsigned char values[] = { XmNO_LINE, XmSINGLE_LINE, XmDOUBLE_LINE };
	static unsigned char value;
	int i = lookup(dpy, from, XmRLineType, names, XtNumber(names));

	if (i < 0)
		return False;
	value = values[i];
	return store(to, &value, sizeof value);
}

static void _init_xt(void)
{
	shell = init_xt("check_RenderTable");
	XtAppSetTypeConverter(app, XmRString, XmRFontType, cvt_font_type, NULL, 0,
			      XtCacheNone, NULL);
	XtAppSetTypeConverter(app, XmRString, XmRLineType, cvt_line_type, NULL, 0,
			      XtCacheNone, NULL);
	conversions = conversions_seen = 0;
}

static int warnings;

static void count_warning(String name, String type, String class, String defaultp,
			  String *params, Cardinal *num_params)
{
	warnings++;
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
	converted();
	l1 = label(rc, "l", "core bold red");
	ck_assert_int_gt(converted(), 0);
	l2 = label(rc, "l", "core bold red");
	ck_assert_msg(converted() == 0, "the second table was not cached");
	/* Another string with the same renditions is another key. */
	l3 = label(rc, "l", "core, bold red");
	ck_assert_int_gt(converted(), 0);
	describe(table_of(l1), d1, sizeof d1);
	describe(table_of(l2), d2, sizeof d2);
	ck_assert_str_eq(d1, d2);
	ck_assert_msg(strstr(d1, "tag=core name=fixed") != NULL, "%s", d1);
	ck_assert_msg(strstr(d1, "tag=bold name=8x13bold") != NULL, "%s", d1);
	ck_assert_msg(strstr(d1, "tag=red name=AS_IS") != NULL, "%s", d1);
	describe(table_of(l3), d2, sizeof d2);
	ck_assert_str_eq(d1, d2);
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
	converted();
	a = label(rc1, "l", "t");
	ck_assert_int_gt(converted(), 0);
	b = label(rc2, "l", "t");
	ck_assert_int_gt(converted(), 0);
	c = label(rc1, "l", "t");
	ck_assert_int_eq(converted(), 0);
	special = label(rc1, "special", "t");
	ck_assert_int_gt(converted(), 0);
	gadget = XtVaCreateWidget("g", xmLabelGadgetClass, rc1, XtVaTypedArg, XmNrenderTable,
				  XmRString, "t", 2, NULL);
	ck_assert_int_gt(converted(), 0);
	ck_assert_str_eq(font_name_of(table_of(a), "t"), "9x15");
	ck_assert_str_eq(font_name_of(table_of(b), "t"), "8x13bold");
	ck_assert_str_eq(font_name_of(table_of(c), "t"), "9x15");
	ck_assert_str_eq(font_name_of(table_of(special), "t"), "6x13");
	ck_assert_str_eq(font_name_of(table_of(gadget), "t"), "8x13");
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

static Pixel foreground_of(XmRenderTable rt, const char *tag)
{
	XmRendition r = XmRenderTableGetRendition(rt, (XmStringTag)tag);
	Pixel fg = 0;
	Arg a[1];

	ck_assert(r != NULL);
	XtSetArg(a[0], XmNrenditionForeground, &fg);
	XmRenditionRetrieve(r, a, 1);
	XmRenditionFree(r);
	return fg;
}

/* The colors of renditions are allocated in the colormap of the */
/* widget: a widget in another colormap does not get its table. */
START_TEST(colormap)
{
	Display *dpy = XtDisplay(shell);
	Screen *screen = XtScreen(shell);
	Colormap cmap = XCreateColormap(dpy, RootWindowOfScreen(screen),
					DefaultVisualOfScreen(screen), AllocNone);
	Widget other, rc1, rc2, a, b, c;
	XColor color, exact;

	put_font("*renderTable.t", "fixed");
	put("*renderTable.t.renditionForeground: red");
	/* From the database, so that the colormap of the widget is set */
	/* when its render table is converted. */
	put("*l.renderTable: t");
	other = XtVaCreatePopupShell("other", topLevelShellWidgetClass, shell, NULL);
	rc1 = XmCreateRowColumn(shell, "rc", NULL, 0);
	rc2 = XtVaCreateWidget("rc", xmRowColumnWidgetClass, other, XmNcolormap, cmap, NULL);
	converted();
	a = XtVaCreateWidget("l", xmLabelWidgetClass, rc1, NULL);
	ck_assert_int_gt(converted(), 0);
	b = XtVaCreateWidget("l", xmLabelWidgetClass, rc2, NULL);
	ck_assert_int_gt(converted(), 0);
	c = XtVaCreateWidget("l", xmLabelGadgetClass, rc2, NULL);
	ck_assert_int_eq(converted(), 0);
	ck_assert(XAllocNamedColor(dpy, cmap, "red", &color, &exact));
	ck_assert_uint_eq(foreground_of(table_of(b), "t"), color.pixel);
	ck_assert_uint_eq(foreground_of(table_of(c), "t"), color.pixel);
	ck_assert(XAllocNamedColor(dpy, DefaultColormapOfScreen(screen), "red", &color, &exact));
	ck_assert_uint_eq(foreground_of(table_of(a), "t"), color.pixel);
	/* The colormap goes with the display: Xt frees the colors */
	/* allocated in it when the application context goes. */
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
	converted();
	l[1] = label(rc, "l", "t");
	ck_assert_int_eq(converted(), 0);
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
	converted();
	b = label(rc, "l", "t");
	ck_assert_int_gt(converted(), 0);
	/* The database finds what it found for a again. */
	put_font("*renderTable.t", "fixed");
	c = label(rc, "l", "t");
	ck_assert_int_eq(converted(), 0);
	ck_assert_str_eq(font_name_of(table_of(a), "t"), "fixed");
	ck_assert_str_eq(font_name_of(table_of(b), "t"), "9x15");
	ck_assert_str_eq(font_name_of(table_of(c), "t"), "fixed");
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
	Widget rc;
	int i;

	put("*renderTable.t.fontName:");
	put("*renderTable.t.fontType: FONT_IS_FONTSET");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	XtAppSetWarningMsgHandler(app, count_warning);
	warnings = 0;
	for (i = 0; i < 3; i++) {
		converted();
		label(rc, "l", "t");
		ck_assert_int_gt(converted(), 0);
	}
	ck_assert_int_eq(warnings, 3);
	XtAddCallback(XmGetXmDisplay(XtDisplay(shell)), XmNnoFontCallback, no_font, NULL);
	no_font_calls = 0;
	warnings = 0;
	for (i = 0; i < 3; i++)
		label(rc, "l", "t");
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
	for (i = 0; i < 3; i++) {
		converted();
		l[i] = label(rc, "l", "t");
		ck_assert_int_gt(converted(), 0);
	}
	ck_assert_int_eq(warnings, 3);
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
	converted();
	b = XtVaCreateWidget("l", xmLabelWidgetClass, rc, XtVaTypedArg, XmNrenderTable,
			     XmRString, "t", 2, XmNlabelString, NULL, NULL);
	ck_assert_int_gt(converted(), 0);
	ck_assert_str_eq(font_name_of(table_of(a), "t"), "fixed");
	ck_assert_str_eq(font_name_of(table_of(b), "t"), "fixed");
}
END_TEST

/* XmNnoFontCallback that gives the rendition a font, as documented. */
static void give_font(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmDisplayCallbackStruct *cb = (XmDisplayCallbackStruct *)call_data;
	Arg a[3];

	no_font_calls++;
	XtSetArg(a[0], XmNfontName, "fixed");
	XtSetArg(a[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(a[2], XmNloadModel, XmLOAD_IMMEDIATE);
	XmRenditionUpdate(cb->rendition, a, 3);
}

/* A label whose table has no font calls XmNnoFontCallback with the */
/* first rendition of its table when it measures its string, and a font */
/* given to that rendition is for its table only: each label calls it, */
/* and the table cached for the others keeps no font. */
START_TEST(no_font_callback_when_drawn)
{
	static const char tag[] = "FONTLIST_DEFAULT_TAG_STRING";
	Widget rc, l[3], a;
	XmString empty;
	int i;

	put("*renderTable.FONTLIST_DEFAULT_TAG_STRING.renditionForeground: red");
	put("*renderTable.FONTLIST_DEFAULT_TAG_STRING.underlineType: SINGLE_LINE");
	XtAddCallback(XmGetXmDisplay(XtDisplay(shell)), XmNnoFontCallback, give_font, NULL);
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	converted();
	for (i = 0; i < 3; i++) {
		no_font_calls = 0;
		l[i] = label(rc, "l", tag);
		ck_assert_int_eq(converted(), i == 0);
		ck_assert_int_eq(no_font_calls, 1);
		ck_assert_str_eq(font_name_of(table_of(l[i]), tag), "fixed");
	}
	/* A label with nothing to measure. */
	empty = XmStringCreateLocalized("");
	no_font_calls = 0;
	a = XtVaCreateWidget("l", xmLabelWidgetClass, rc, XmNlabelString, empty,
			     XtVaTypedArg, XmNrenderTable, XmRString, tag, (int)sizeof tag, NULL);
	ck_assert_int_eq(converted(), 0);
	ck_assert_int_eq(no_font_calls, 0);
	ck_assert_str_eq(font_name_of(table_of(a), tag), "");
	XmStringFree(empty);
}
END_TEST

/* A table used again stays cached while many other strings are */
/* converted once, and those are dropped. */
START_TEST(table_used_again_stays_cached)
{
	Widget rc;
	char spec[32];
	int i;

	put_font("*renderTable.t", "fixed");
	rc = XmCreateRowColumn(shell, "rc", NULL, 0);
	XtDestroyWidget(label(rc, "l", "t"));
	XtDestroyWidget(label(rc, "l", "t once"));
	for (i = 0; i < 200; i++) {
		snprintf(spec, sizeof spec, "t u%d", i);
		XtDestroyWidget(label(rc, "l", spec));
		if (i % 8 == 0) {
			converted();
			XtDestroyWidget(label(rc, "l", "t"));
			ck_assert_int_eq(converted(), 0);
		}
	}
	converted();
	XtDestroyWidget(label(rc, "l", "t once"));
	ck_assert_int_gt(converted(), 0);
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

/*
 * Rendition handles.
 *
 * A table holds a handle of its own to each of its renditions, whose
 * data it may share with other tables and with the renditions the
 * program holds.  XmRenderTableFree, a merge that replaces a rendition
 * and XmRenderTableRemoveRenditions free the table's handle whoever
 * else still holds the rendition.  Tables copied with XmRenderTableCopy
 * share one record, and with it the handles: those are freed with the
 * last of the tables, and a table that changes gets handles of its own.
 */

static XmRendition font_rendition(const char *tag, const char *font)
{
	Arg a[2];

	XtSetArg(a[0], XmNfontName, (XtArgVal)font);
	XtSetArg(a[1], XmNfontType, XmFONT_IS_FONT);
	return XmRenditionCreate(shell, tag, a, 2);
}

static XmRenderTable table_with(XmRendition r)
{
	return XmRenderTableAddRenditions(NULL, &r, 1, XmMERGE_NEW);
}

static int tag_count(XmRenderTable rt)
{
	XmStringTag *tags = NULL;
	int i, n = XmRenderTableGetTags(rt, &tags);

	for (i = 0; i < n; i++)
		XtFree(tags[i]);
	XtFree((char *)tags);
	return n;
}

/* A table freed while another table and the program hold its rendition. */
START_TEST(free_table_of_held_rendition)
{
	XmRendition a = font_rendition("A", "fixed");
	XmRendition b = font_rendition("A", "8x13");
	XmRenderTable rt1 = table_with(a), rt2 = table_with(b);
	XmRendition r = XmRenderTableGetRendition(rt1, "A");

	rt2 = XmRenderTableAddRenditions(rt2, &r, 1, XmMERGE_REPLACE);
	XmRenditionFree(r);
	XmRenderTableFree(rt1);
	ck_assert_str_eq(font_name_of(rt2, "A"), "fixed");
	XmRenderTableFree(rt2);
	XmRenditionFree(a);
	XmRenditionFree(b);
#ifdef HAVE_LSAN
	ck_assert_msg(!leaks_found(), "XmRenderTableFree leaked a rendition handle");
#endif
}
END_TEST

/*
 * Each merge mode, on a rendition that another table holds too.  The
 * program's renditions are freed first, so that the tables free the
 * renditions that only they hold.
 */
static const struct {
	XmMergeMode mode;
	const char *font;
	int count;
} merges[] = {
	{ XmMERGE_REPLACE, "8x13", 1 },
	{ XmMERGE_NEW, "8x13", 1 },
	{ XmMERGE_OLD, "fixed", 1 },
	{ XmSKIP, "fixed", 1 },
	{ XmDUPLICATE, "fixed", 2 },
};

START_TEST(merge_held_rendition)
{
	XmRendition a = font_rendition("A", "fixed");
	XmRendition b = font_rendition("A", "8x13");
	XmRenderTable other = table_with(a), rt = table_with(a);

	rt = XmRenderTableAddRenditions(rt, &b, 1, merges[_i].mode);
	ck_assert_int_eq(tag_count(rt), merges[_i].count);
	ck_assert_str_eq(font_name_of(rt, "A"), merges[_i].font);
	ck_assert_str_eq(font_name_of(other, "A"), "fixed");
	XmRenditionFree(a);
	XmRenditionFree(b);
	XmRenderTableFree(rt);
	ck_assert_str_eq(font_name_of(other, "A"), "fixed");
	XmRenderTableFree(other);
#ifdef HAVE_LSAN
	ck_assert_msg(!leaks_found(), "XmRenderTableAddRenditions leaked (merge mode %d)",
		      (int)merges[_i].mode);
#endif
}
END_TEST

/* A rendition removed from a table while another table holds it. */
START_TEST(remove_held_rendition)
{
	XmRendition r[2];
	XmRenderTable other, rt;
	XmStringTag tag = "A";

	r[0] = font_rendition("A", "fixed");
	r[1] = font_rendition("B", "8x13");
	other = table_with(r[0]);
	rt = XmRenderTableAddRenditions(NULL, r, 2, XmMERGE_NEW);
	rt = XmRenderTableRemoveRenditions(rt, &tag, 1);
	ck_assert_int_eq(tag_count(rt), 1);
	ck_assert_str_eq(font_name_of(rt, "B"), "8x13");
	XmRenderTableFree(rt);
	ck_assert_str_eq(font_name_of(other, "A"), "fixed");
	XmRenderTableFree(other);
	XmRenditionFree(r[0]);
	XmRenditionFree(r[1]);
#ifdef HAVE_LSAN
	ck_assert_msg(!leaks_found(), "XmRenderTableRemoveRenditions leaked a rendition handle");
#endif
}
END_TEST

/*
 * Copies of one table changed in every way while the others live; the
 * original is freed first, so a handle that a changed copy still shared
 * with it would be used after it is freed.
 */
START_TEST(change_shared_table)
{
	XmRendition r[2], c, b2;
	XmRenderTable rt, added, replaced, removed, some, same;
	XmStringTag tag = "A";

	r[0] = font_rendition("A", "fixed");
	r[1] = font_rendition("B", "8x13");
	c = font_rendition("C", "fixed");
	b2 = font_rendition("B", "fixed");
	rt = XmRenderTableAddRenditions(NULL, r, 2, XmMERGE_NEW);
	added = XmRenderTableCopy(rt, NULL, 0);
	replaced = XmRenderTableCopy(rt, NULL, 0);
	removed = XmRenderTableCopy(rt, NULL, 0);
	same = XmRenderTableCopy(rt, NULL, 0);
	some = XmRenderTableCopy(rt, &tag, 1);
	added = XmRenderTableAddRenditions(added, &c, 1, XmMERGE_NEW);
	replaced = XmRenderTableAddRenditions(replaced, &b2, 1, XmMERGE_REPLACE);
	removed = XmRenderTableRemoveRenditions(removed, &tag, 1);
	XmRenditionFree(r[0]);
	XmRenditionFree(r[1]);
	XmRenditionFree(c);
	XmRenditionFree(b2);
	XmRenderTableFree(rt);

	ck_assert_int_eq(tag_count(added), 3);
	ck_assert_str_eq(font_name_of(added, "B"), "8x13");
	ck_assert_str_eq(font_name_of(added, "C"), "fixed");
	ck_assert_int_eq(tag_count(replaced), 2);
	ck_assert_str_eq(font_name_of(replaced, "A"), "fixed");
	ck_assert_str_eq(font_name_of(replaced, "B"), "fixed");
	ck_assert_int_eq(tag_count(removed), 1);
	ck_assert_str_eq(font_name_of(removed, "B"), "8x13");
	ck_assert_int_eq(tag_count(some), 1);
	ck_assert_str_eq(font_name_of(some, "A"), "fixed");
	ck_assert_int_eq(tag_count(same), 2);
	ck_assert_str_eq(font_name_of(same, "B"), "8x13");
	XmRenderTableFree(added);
	XmRenderTableFree(replaced);
	XmRenderTableFree(removed);
	XmRenderTableFree(some);
	ck_assert_str_eq(font_name_of(same, "A"), "fixed");
	XmRenderTableFree(same);
#ifdef HAVE_LSAN
	ck_assert_msg(!leaks_found(), "copies of a render table leaked");
#endif
}
END_TEST

static XmRendition substitute;
static int no_rendition_calls;

/* Adds substitute to the table, or leaves it alone if there is none. */
static void no_rendition(Widget w, XtPointer client_data, XtPointer call_data)
{
	XmDisplayCallbackStruct *cb = (XmDisplayCallbackStruct *)call_data;

	no_rendition_calls++;
	if (substitute != NULL)
		cb->render_table = XmRenderTableAddRenditions(cb->render_table,
							      &substitute, 1, XmMERGE_NEW);
}

/*
 * The XmNnoRenditionCallback is given a copy of the table being
 * searched; a table it returns instead takes the searched table's
 * place, and the copy is freed if it returns that.  Loop 0 adds the
 * missing rendition, loop 1 leaves the copy alone.
 */
START_TEST(no_rendition_callback_table)
{
	Widget dsp = XmGetXmDisplay(XtDisplay(shell));
	XmRendition a = font_rendition("A", "fixed");
	XmRenderTable rt = table_with(a);
	XmString s = XmStringCreate("x", "Z");
	Dimension w = 0, h = 0;

	substitute = _i == 0 ? font_rendition("Z", "8x13") : NULL;
	no_rendition_calls = 0;
	XtAddCallback(dsp, XmNnoRenditionCallback, no_rendition, NULL);
	XmStringExtent(rt, s, &w, &h);
	XtRemoveCallback(dsp, XmNnoRenditionCallback, no_rendition, NULL);
	ck_assert_int_eq(no_rendition_calls, 1);
	ck_assert_int_eq(tag_count(rt), _i == 0 ? 2 : 1);
	if (_i == 0) {
		ck_assert_uint_gt(w, 0);
		ck_assert_str_eq(font_name_of(rt, "Z"), "8x13");
		XmRenditionFree(substitute);
	}
	XmRenderTableFree(rt);
	XmRenditionFree(a);
	XmStringFree(s);
#ifdef HAVE_LSAN
	ck_assert_msg(!leaks_found(), "the XmNnoRenditionCallback table leaked");
#endif
}
END_TEST

/* The lookups of a widget nested deeper than the arrays of names and */
/* classes on the stack, of 100 entries, allow for. */
START_TEST(deeply_nested_widget)
{
	Widget parent = shell, l;
	XmRendition r;
	String font_name = NULL;
	Arg a[1];
	int i;

	put_font("*renderTable.t", "fixed");
	/* XmRenditionCreate looks up the resource class only. */
	put_font("*RenderTable.u", "8x13");
	for (i = 0; i < 150; i++)
		parent = XmCreateRowColumn(parent, "rc", NULL, 0);
	converted();
	l = label(parent, "l", "t");
	ck_assert_int_gt(converted(), 0);
	ck_assert_str_eq(font_name_of(table_of(l), "t"), "fixed");
	/* A cached table, and a rendition made for the widget. */
	(void)label(parent, "l", "t");
	ck_assert_msg(converted() == 0, "the second table was not cached");
	r = XmRenditionCreate(l, "u", NULL, 0);
	XtSetArg(a[0], XmNfontName, &font_name);
	XmRenditionRetrieve(r, a, 1);
	ck_assert(font_name != NULL && font_name != (String)XmAS_IS);
	ck_assert_str_eq(font_name, "8x13");
	XmRenditionFree(r);
}
END_TEST

void rendertable_suite(SRunner *runner)
{
	Suite *s = suite_create("RenderTable");
	TCase *t = tcase_create("String to RenderTable");

	tcase_add_test(t, same_string_same_table);
	tcase_add_test(t, resources_of_the_widget_path);
	tcase_add_test(t, resource_names_and_inheritance);
	tcase_add_test(t, colormap);
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
	tcase_add_test(t, no_font_callback_when_drawn);
	tcase_add_test(t, table_used_again_stays_cached);
	tcase_add_test(t, display_close);
	tcase_add_test(t, default_render_table_display_close);
	tcase_add_test(t, deeply_nested_widget);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);

	t = tcase_create("Rendition handles");
	tcase_add_test(t, free_table_of_held_rendition);
	tcase_add_loop_test(t, merge_held_rendition, 0, (int)XtNumber(merges));
	tcase_add_test(t, remove_held_rendition);
	tcase_add_test(t, change_shared_table);
	tcase_add_loop_test(t, no_rendition_callback_table, 0, 2);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
