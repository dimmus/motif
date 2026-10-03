/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Regression tests for the Form, List and Container layout code.
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <Xm/XmP.h>
#include <Xm/Container.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/IconG.h>
#include <Xm/List.h>
#include <Xm/ScrollBar.h>
#include <check.h>

#include "suites.h"

static Widget shell;

static void _init_xt(void)
{
	shell = init_xt("check_Layout");
	XtVaSetValues(shell, XmNallowShellResize, True, NULL);
}

/* Handle everything the X server has to say. */
static void settle(void)
{
	int i;

	for (i = 0; i < 3; i++) {
		XSync(XtDisplay(shell), False);
		while (XtAppPending(app) & XtIMXEvent)
			XtAppProcessEvent(app, XtIMXEvent);
	}
}

static Widget box(Widget parent, const char *name, int width, int height, ArgList args,
		  Cardinal n)
{
	Widget w = XmCreateDrawingArea(parent, (char *)name, args, n);

	XtVaSetValues(w, XmNwidth, width, XmNheight, height, XmNborderWidth, 0,
		      XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
	XtManageChild(w);
	return w;
}

/*
 * A column of children, each attached to the one above it and stretched
 * between the form sides: the form is as wide as the widest and as high
 * as all of them.
 */
START_TEST(form_column)
{
	enum { N = 300 };
	Widget form, kids[N];
	Dimension fw, fh;
	int i, y = 0, widest = 0;
	Arg args[8];
	Cardinal n;

	form = XmCreateForm(shell, "form", NULL, 0);
	for (i = 0; i < N; i++) {
		char name[16];
		int width = 10 + (i * 7) % 50;

		n = 0;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM), n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM), n++;
		if (i == 0) {
			XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM), n++;
		} else {
			XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
			XtSetArg(args[n], XmNtopWidget, kids[i - 1]), n++;
		}
		snprintf(name, sizeof(name), "k%d", i);
		kids[i] = box(form, name, width, 1 + i % 5, args, n);
		if (width > widest)
			widest = width;
	}
	XtManageChild(form);
	XtRealizeWidget(shell);
	settle();
	XtVaGetValues(form, XmNwidth, &fw, XmNheight, &fh, NULL);
	ck_assert_int_eq(fw, widest);
	for (i = 0; i < N; i++) {
		ck_assert_int_eq(XtX(kids[i]), 0);
		ck_assert_int_eq(XtY(kids[i]), y);
		ck_assert_int_eq(XtWidth(kids[i]), widest);
		y += 1 + i % 5;
	}
	ck_assert_int_eq(fh, y);

	/* Removing children one by one keeps the column together. */
	for (i = N - 1; i >= N / 2; i--)
		XtDestroyWidget(kids[i]);
	settle();
	XtVaGetValues(form, XmNheight, &fh, NULL);
	ck_assert_int_eq(XtY(kids[N / 2 - 1]) + XtHeight(kids[N / 2 - 1]), fh);
}
END_TEST

/* Children placed by position along a row. */
START_TEST(form_positions)
{
	Widget form, kids[4];
	int i;
	Arg args[8];
	Cardinal n;

	form = XmCreateForm(shell, "form", NULL, 0);
	XtVaSetValues(form, XmNfractionBase, 4, XmNwidth, 400, XmNheight, 50, NULL);
	for (i = 0; i < 4; i++) {
		n = 0;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION), n++;
		XtSetArg(args[n], XmNleftPosition, i), n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION), n++;
		XtSetArg(args[n], XmNrightPosition, i + 1), n++;
		XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM), n++;
		XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM), n++;
		kids[i] = box(form, "pos", 10, 10, args, n);
	}
	XtManageChild(form);
	XtRealizeWidget(shell);
	settle();
	for (i = 0; i < 4; i++) {
		ck_assert_int_eq(XtX(kids[i]), 100 * i);
		ck_assert_int_eq(XtWidth(kids[i]), 100);
		ck_assert_int_eq(XtHeight(kids[i]), 50);
	}
	XtVaSetValues(shell, XmNwidth, 800, NULL);
	settle();
	for (i = 0; i < 4; i++)
		ck_assert_int_eq(XtX(kids[i]), 200 * i);
}
END_TEST

/*
 * A sibling attached to a child destroyed while unmanaged used to keep
 * a pointer to the freed widget.  It is attached to the form instead.
 */
START_TEST(form_destroy_unmanaged_target)
{
	Widget form, a, b;
	unsigned char type;
	Widget target;
	int offset;
	Arg args[8];
	Cardinal n = 0;

	form = XmCreateForm(shell, "form", NULL, 0);
	a = box(form, "a", 30, 20, NULL, 0);
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNleftWidget, a), n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM), n++;
	b = box(form, "b", 30, 20, args, n);
	XtManageChild(form);
	XtRealizeWidget(shell);
	settle();
	ck_assert_int_eq(XtX(b), 30);
	XtUnmanageChild(a);
	settle();
	XtDestroyWidget(a);
	settle();
	XtVaGetValues(b, XmNleftAttachment, &type, XmNleftWidget, &target, XmNleftOffset, &offset,
		      NULL);
	ck_assert_int_eq(type, XmATTACH_FORM);
	ck_assert_ptr_null(target);
	ck_assert_int_eq(offset, XtX(b));
	/* lay out again */
	XtVaSetValues(shell, XmNwidth, 300, NULL);
	settle();
	ck_assert_int_eq(XtX(b), offset);
}
END_TEST

static XImage *grab(Widget w)
{
	return XGetImage(XtDisplay(w), XtWindow(w), 0, 0, XtWidth(w), XtHeight(w), AllPlanes,
			 ZPixmap);
}

static int same_image(XImage *a, XImage *b)
{
	int x, y;

	if (a->width != b->width || a->height != b->height)
		return 0;
	for (y = 0; y < a->height; y++)
		for (x = 0; x < a->width; x++)
			if (XGetPixel(a, x, y) != XGetPixel(b, x, y))
				return 0;
	return 1;
}

static XmString make_item(int i)
{
	char text[32];

	snprintf(text, sizeof(text), "item %d%.*s", i, i % 7, "-------");
	return XmStringCreateLocalized(text);
}

static Widget make_list(int count, int spacing, int highlight)
{
	XmString *items = (XmString *)XtMalloc(count * sizeof(XmString));
	Widget list;
	Arg args[8];
	Cardinal n = 0;
	int i;

	for (i = 0; i < count; i++)
		items[i] = make_item(i);
	XtSetArg(args[n], XmNitems, items), n++;
	XtSetArg(args[n], XmNitemCount, count), n++;
	XtSetArg(args[n], XmNvisibleItemCount, 10), n++;
	XtSetArg(args[n], XmNlistSpacing, spacing), n++;
	XtSetArg(args[n], XmNhighlightThickness, highlight), n++;
	XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT), n++;
	list = XmCreateScrolledList(shell, "list", args, n);
	for (i = 0; i < count; i++)
		XmStringFree(items[i]);
	XtFree((char *)items);
	XtManageChild(list);
	return list;
}

/*
 * Scrolling with the scrollbar copies the rows that stay visible: the
 * result must be what a full redraw gives.
 */
static void check_scrolling(int spacing, int highlight)
{
	static const int moves[] = { 1, 1, 3, -2, 9, -1, -9, 5, 2, -4, 30, -12 };
	Widget list, vsb = NULL;
	int value, size, inc, page, i;
	XImage *scrolled, *redrawn;

	list = make_list(200, spacing, highlight);
	XtRealizeWidget(shell);
	settle();
	for (i = 1; i <= 200; i += 13)
		XmListSelectPos(list, i, False);
	XtVaGetValues(XtParent(list), XmNverticalScrollBar, &vsb, NULL);
	ck_assert_ptr_nonnull(vsb);
	settle();
	for (i = 0; i < (int)(sizeof(moves) / sizeof(moves[0])); i++) {
		XmScrollBarGetValues(vsb, &value, &size, &inc, &page);
		XmScrollBarSetValues(vsb, value + moves[i], size, inc, page, True);
		settle();
		scrolled = grab(list);
		XClearArea(XtDisplay(list), XtWindow(list), 0, 0, 0, 0, True);
		settle();
		redrawn = grab(list);
		ck_assert_msg(same_image(scrolled, redrawn), "scroll %d differs from a redraw", i);
		XDestroyImage(scrolled);
		XDestroyImage(redrawn);
	}
}

START_TEST(list_scroll_by_copy)
{
	check_scrolling(0, 2);
}
END_TEST

START_TEST(list_scroll_by_copy_spaced)
{
	check_scrolling(3, 1);
}
END_TEST

START_TEST(list_scroll_no_spacing)
{
	check_scrolling(0, 0);
}
END_TEST

/*
 * XmNselectedItems naming an item the list does not have used to leave
 * an uninitialised selected position behind after a deletion.
 */
START_TEST(list_selected_items_missing)
{
	Widget list = make_list(5, 0, 2);
	XmString missing = XmStringCreateLocalized("not in the list");
	int *positions = NULL, count = -1;

	XtVaSetValues(list, XmNselectedItems, &missing, XmNselectedItemCount, 1, NULL);
	XmStringFree(missing);
	XmListDeletePos(list, 1);
	XtVaGetValues(list, XmNselectedPositions, &positions, XmNselectedPositionCount, &count,
		      NULL);
	ck_assert_int_eq(count, 0);
	ck_assert_ptr_null(positions);
}
END_TEST

/* Selecting by position and by value, and looking items up. */
START_TEST(list_select_and_find)
{
	Widget list = make_list(300, 0, 2);
	XmString item = make_item(250);
	int *positions = NULL, count = 0;

	XmListSelectPos(list, 3, False);
	XmListSelectPos(list, 100, False);
	XmListSelectItem(list, item, False);
	XtVaGetValues(list, XmNselectedPositions, &positions, XmNselectedPositionCount, &count,
		      NULL);
	ck_assert_int_eq(count, 3);
	ck_assert_int_eq(positions[0], 3);
	ck_assert_int_eq(positions[1], 100);
	ck_assert_int_eq(positions[2], 251);
	ck_assert_int_eq(XmListItemPos(list, item), 251);
	ck_assert(XmListItemExists(list, item));
	XmListDeselectPos(list, 100);
	XtVaGetValues(list, XmNselectedPositions, &positions, XmNselectedPositionCount, &count,
		      NULL);
	ck_assert_int_eq(count, 2);
	ck_assert_int_eq(positions[1], 251);
	XmListDeletePos(list, 1);
	XtVaGetValues(list, XmNselectedPositions, &positions, XmNselectedPositionCount, &count,
		      NULL);
	ck_assert_int_eq(count, 2);
	ck_assert_int_eq(positions[0], 2);
	ck_assert_int_eq(positions[1], 250);
	XmStringFree(item);
	item = XmStringCreateLocalized("item 999");
	ck_assert(!XmListItemExists(list, item));
	ck_assert_int_eq(XmListItemPos(list, item), 0);
	XmStringFree(item);
}
END_TEST

static int position_of(Widget w)
{
	int pos = -100;

	XtVaGetValues(w, XmNpositionIndex, &pos, NULL);
	return pos;
}

/* XmNpositionIndex stays 0, 1, 2... when children are appended. */
START_TEST(container_positions)
{
	enum { N = 60 };
	Widget c, kids[N], extra;
	int i;

	c = XtVaCreateManagedWidget("c", xmContainerWidgetClass, shell, XmNlayoutType, XmOUTLINE,
				    NULL);
	for (i = 0; i < N; i++)
		kids[i] = XtVaCreateManagedWidget("i", xmIconGadgetClass, c, NULL);
	for (i = 0; i < N; i++)
		ck_assert_int_eq(position_of(kids[i]), i);
	extra = XtVaCreateManagedWidget("x", xmIconGadgetClass, c, XmNpositionIndex, 10, NULL);
	ck_assert_int_eq(position_of(extra), 10);
	ck_assert_int_eq(position_of(kids[10]), 11);
	ck_assert_int_eq(position_of(kids[N - 1]), N);
	/* a lone entry keeps the index it is given, until one is added */
	extra = XtVaCreateManagedWidget("p", xmIconGadgetClass, c, XmNentryParent, kids[0], NULL);
	ck_assert_int_eq(position_of(extra), 0);
	XtVaSetValues(extra, XmNpositionIndex, 7, NULL);
	ck_assert_int_eq(position_of(extra), 7);
	kids[1] = XtVaCreateManagedWidget("q", xmIconGadgetClass, c, XmNentryParent, kids[0], NULL);
	ck_assert_int_eq(position_of(extra), 0);
	ck_assert_int_eq(position_of(kids[1]), 1);
	XtRealizeWidget(shell);
	settle();
	XtDestroyWidget(c);
	settle();
}
END_TEST

void layout_suite(SRunner *runner)
{
	Suite *s = suite_create("Layout");
	TCase *t;

	t = tcase_create("Form");
	tcase_add_test(t, form_column);
	tcase_add_test(t, form_positions);
	tcase_add_test(t, form_destroy_unmanaged_target);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);

	t = tcase_create("List");
	tcase_add_test(t, list_scroll_by_copy);
	tcase_add_test(t, list_scroll_by_copy_spaced);
	tcase_add_test(t, list_scroll_no_spacing);
	tcase_add_test(t, list_selected_items_missing);
	tcase_add_test(t, list_select_and_find);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);

	t = tcase_create("Container");
	tcase_add_test(t, container_positions);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
