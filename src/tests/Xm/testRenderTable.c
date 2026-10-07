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
#include <Xm/RowColumn.h>
#include <check.h>

#include "suites.h"

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

void rendertable_suite(SRunner *runner)
{
	Suite *s = suite_create("RenderTable");
	TCase *t = tcase_create("String to RenderTable");

	tcase_add_test(t, no_pointer_into_database);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
