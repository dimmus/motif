/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Tests of the String to RenderTable conversion.
 *
 * The converter looks up the resources of each rendition in the resource
 * database under the widget's names and classes.
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <check.h>

#include "suites.h"

/* libXm's, not declared by an installed header. */
extern Display *_XmRenderTableDisplay(XmRenderTable table);

static Widget shell;

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

void rendertable_suite(SRunner *runner)
{
	Suite *s = suite_create("RenderTable");
	TCase *t = tcase_create("String to RenderTable");

	tcase_add_test(t, tab_list_resource);
	tcase_add_test(t, no_pointer_into_database);
	tcase_add_test(t, default_render_table_display_close);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
