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
static void check_scrolling(int spacing, int highlight, Boolean tiled)
{
	static const int moves[] = { 1, 1, 3, -2, 9, -1, -9, 5, 2, -4, 30, -12 };
	Widget list, vsb = NULL;
	int value, size, inc, page, i;
	XImage *scrolled, *redrawn;

	list = make_list(200, spacing, highlight);
	if (tiled) {
		/* the gaps between the rows show the tile, which stays put */
		static char bits[] = { 0x05, 0x02, 0x07, 0x01, 0x06 };
		Display *dpy = XtDisplay(shell);
		int scr = DefaultScreen(dpy);
		Pixmap tile = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, scr), bits, 3, 5,
							  BlackPixel(dpy, scr),
							  WhitePixel(dpy, scr),
							  DefaultDepth(dpy, scr));

		XtVaSetValues(list, XmNbackgroundPixmap, tile, NULL);
	}
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
	check_scrolling(0, 2, False);
}
END_TEST

START_TEST(list_scroll_by_copy_spaced)
{
	check_scrolling(3, 1, False);
}
END_TEST

START_TEST(list_scroll_tiled_background)
{
	check_scrolling(3, 1, True);
}
END_TEST

START_TEST(list_scroll_no_spacing)
{
	check_scrolling(0, 0, False);
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

static Widget icon(Widget c, const char *name, Widget parent, int position)
{
	Arg args[4];
	Cardinal n = 0;

	XtSetArg(args[n], XmNoutlineState, XmEXPANDED), n++;
	if (parent)
		XtSetArg(args[n], XmNentryParent, parent), n++;
	if (position != -2)
		XtSetArg(args[n], XmNpositionIndex, position), n++;
	return XtCreateManagedWidget(name, xmIconGadgetClass, c, args, n);
}

/* The entries of parent (NULL: the top level) are exactly want[0..n-1]. */
static void check_level(Widget c, Widget parent, Widget *want, int n)
{
	WidgetList kids = NULL;
	int i, count = XmContainerGetItemChildren(c, parent, &kids);

	ck_assert_int_eq(count, n);
	for (i = 0; i < n; i++) {
		ck_assert_ptr_eq(kids[i], want[i]);
		ck_assert_int_eq(position_of(want[i]), i);
	}
	XtFree((char *)kids);
}

/* Inserting at the head numbers the others after it. */
START_TEST(container_positions_head)
{
	enum { N = 500 };
	Widget c, kids[N], order[N];
	int i;

	c = XtVaCreateManagedWidget("c", xmContainerWidgetClass, shell, XmNlayoutType, XmOUTLINE,
				    NULL);
	for (i = 0; i < N; i++) {
		kids[i] = icon(c, "i", NULL, 0);
		/* reading one brings all up to date */
		if (i % 97 == 0)
			ck_assert_int_eq(position_of(kids[0]), i);
	}
	for (i = 0; i < N; i++)
		order[i] = kids[N - 1 - i];
	check_level(c, NULL, order, N);
	/* removing from the head moves the others up */
	for (i = 0; i < 10; i++)
		XtDestroyWidget(order[i]);
	ck_assert_int_eq(position_of(order[10]), 0);
	ck_assert_int_eq(position_of(order[N - 1]), N - 11);
	check_level(c, NULL, order + 10, N - 10);
	XtRealizeWidget(shell);
	settle();
	for (i = 11; i < N; i++)
		ck_assert_int_gt(XtY(order[i]), XtY(order[i - 1]));
	XtDestroyWidget(c);
}
END_TEST

/* XmContainerReorder after inserts that left the indices to renumber. */
START_TEST(container_positions_reorder)
{
	Widget c, a, b, d, e, f, list[2];

	c = XtVaCreateManagedWidget("c", xmContainerWidgetClass, shell, XmNlayoutType, XmOUTLINE,
				    NULL);
	f = icon(c, "f", NULL, -2);
	e = icon(c, "e", NULL, 0);
	d = icon(c, "d", NULL, 0);
	b = icon(c, "b", NULL, 0);
	a = icon(c, "a", NULL, 0);
	icon(c, "x", NULL, 2);
	XtDestroyWidget(XtNameToWidget(c, "x"));
	/* a b d e f: d and b get positions 1 and 2, in that order */
	list[0] = d;
	list[1] = b;
	XmContainerReorder(c, list, 2);
	{
		Widget want[] = {a, d, b, e, f};

		check_level(c, NULL, want, 5);
	}
	XtDestroyWidget(c);
}
END_TEST

/* The entries of a destroyed entry go to the top of the top level. */
START_TEST(container_positions_destroy_parent)
{
	Widget c, p, q, k[3];
	int i;

	c = XtVaCreateManagedWidget("c", xmContainerWidgetClass, shell, XmNlayoutType, XmOUTLINE,
				    NULL);
	q = icon(c, "q", NULL, -2);
	p = icon(c, "p", NULL, 0);
	for (i = 0; i < 3; i++)
		k[i] = icon(c, "k", p, 0);
	XtDestroyWidget(p);
	{
		Widget want[] = {k[0], k[1], k[2], q};

		check_level(c, NULL, want, 4);
	}
	XtDestroyWidget(c);
}
END_TEST

/*
 * Random inserts, removals, moves and reorders, checked against a
 * model of the levels: the entries of each item, in order.
 */
enum { MAXI = 160 };
static Widget m_item[MAXI];	/* NULL once destroyed */
static int m_parent[MAXI];	/* index of the entry parent, -1 for none */
static Widget m_level[MAXI + 1][MAXI]; /* [parent + 1] */
static int m_count[MAXI + 1];
static int m_items;
static unsigned int m_rng;

static int m_rand(int n)
{
	m_rng ^= m_rng << 13;
	m_rng ^= m_rng >> 17;
	m_rng ^= m_rng << 5;
	return n > 0 ? (int)(m_rng % (unsigned int)n) : 0;
}

static int m_index(Widget w)
{
	int i;

	for (i = 0; i < m_items; i++)
		if (m_item[i] == w)
			return i;
	ck_abort_msg("no such item");
	return -1;
}

static int m_remove(int level, Widget w)
{
	int i, j;

	for (i = 0; i < m_count[level]; i++)
		if (m_level[level][i] == w)
			break;
	ck_assert_int_lt(i, m_count[level]);
	for (j = i; j + 1 < m_count[level]; j++)
		m_level[level][j] = m_level[level][j + 1];
	m_count[level]--;
	return i;
}

static void m_insert(int level, Widget w, int position)
{
	int j;

	if (position == XmLAST_POSITION || position > m_count[level])
		position = m_count[level];
	for (j = m_count[level]; j > position; j--)
		m_level[level][j] = m_level[level][j - 1];
	m_level[level][position] = w;
	m_count[level]++;
}

/* A live item, or -1. */
static int m_live(void)
{
	int i, k = m_rand(m_items);

	for (i = 0; i < m_items; i++)
		if (m_item[(k + i) % m_items])
			return (k + i) % m_items;
	return -1;
}

static Boolean m_descends(int i, int from)
{
	for (; i >= 0; i = m_parent[i])
		if (i == from)
			return True;
	return False;
}

static void m_check(Widget c)
{
	int l;

	for (l = 0; l <= m_items; l++)
		if (l == 0 || m_item[l - 1])
			check_level(c, l ? m_item[l - 1] : NULL, m_level[l], m_count[l]);
}

static void m_step(Widget c)
{
	int i, j, l, n, position;

	switch (m_rand(9)) {
	case 0: /* insert anywhere, in any level */
	case 1:
		if (m_items == MAXI)
			break;
		j = m_rand(3) ? m_live() : -1;
		l = j + 1;
		position = m_rand(4) ? m_rand(m_count[l] + 2) : (m_rand(2) ? XmLAST_POSITION : -2);
		i = m_items++;
		m_item[i] = icon(c, "i", j >= 0 ? m_item[j] : NULL, position);
		m_parent[i] = j;
		m_count[i + 1] = 0;
		m_insert(l, m_item[i], position == -2 ? XmLAST_POSITION : position);
		break;
	case 2: /* remove an entry without entries */
		i = m_live();
		if (i < 0 || m_count[i + 1] > 0)
			break;
		m_remove(m_parent[i] + 1, m_item[i]);
		XtDestroyWidget(m_item[i]);
		m_item[i] = NULL;
		break;
	case 3: /* move within the level: not alone, which keeps any index */
		i = m_live();
		if (i < 0 || m_count[m_parent[i] + 1] < 2)
			break;
		position = m_rand(4) ? m_rand(m_count[m_parent[i] + 1] + 1) : XmLAST_POSITION;
		XtVaSetValues(m_item[i], XmNpositionIndex, position, NULL);
		m_remove(m_parent[i] + 1, m_item[i]);
		m_insert(m_parent[i] + 1, m_item[i], position);
		break;
	case 4: /* move to another level, keeping the index */
	case 5: /* or to a given one */
		i = m_live();
		j = m_rand(3) ? m_live() : -1;
		if (i < 0 || (j >= 0 && m_descends(j, i)) || j == m_parent[i])
			break;
		position = m_remove(m_parent[i] + 1, m_item[i]);
		if (m_rand(2)) {
			position = m_rand(m_count[j + 1] + 2);
			XtVaSetValues(m_item[i], XmNentryParent, j >= 0 ? m_item[j] : NULL,
				      XmNpositionIndex, position, NULL);
		} else {
			XtVaSetValues(m_item[i], XmNentryParent, j >= 0 ? m_item[j] : NULL, NULL);
		}
		m_parent[i] = j;
		m_insert(j + 1, m_item[i], position);
		break;
	case 6: { /* reorder a few entries of a level */
		Widget list[6];
		int pos[6], k;

		i = m_live();
		if (i < 0)
			break;
		l = m_parent[i] + 1;
		if (m_count[l] < 2)
			break;
		n = 2 + m_rand(m_count[l] < 6 ? m_count[l] - 1 : 5);
		for (k = 0; k < n; k++) {
			int p;

			do {
				p = m_rand(m_count[l]);
				for (j = 0; j < k && list[j] != m_level[l][p]; j++)
					;
			} while (j < k);
			list[k] = m_level[l][p];
			pos[k] = p;
		}
		XmContainerReorder(c, list, n);
		/* the positions they had, sorted, given out in list order */
		for (k = 1; k < n; k++)
			for (j = k; j > 0 && pos[j - 1] > pos[j]; j--) {
				int t = pos[j];

				pos[j] = pos[j - 1];
				pos[j - 1] = t;
			}
		for (k = 0; k < n; k++) {
			m_remove(l, list[k]);
			m_insert(l, list[k], pos[k]);
		}
		break;
	}
	case 7: /* remove an entry with entries: they go to the top level */
		i = m_live();
		if (i < 0 || m_count[i + 1] == 0)
			break;
		XtDestroyWidget(m_item[i]);
		while (m_count[i + 1] > 0) {
			Widget w = m_level[i + 1][0];

			m_remove(i + 1, w);
			m_insert(0, w, 0);
			m_parent[m_index(w)] = -1;
		}
		m_remove(m_parent[i] + 1, m_item[i]);
		m_item[i] = NULL;
		break;
	default: /* read one index */
		i = m_live();
		if (i < 0)
			break;
		l = m_parent[i] + 1;
		for (j = 0; m_level[l][j] != m_item[i]; j++)
			;
		ck_assert_int_eq(position_of(m_item[i]), j);
		break;
	}
}

/* Outline and detail entries are laid out in depth-first order. */
static void m_check_layout(int level, int *y)
{
	int i;

	for (i = 0; i < m_count[level]; i++) {
		Widget w = m_level[level][i];

		ck_assert_int_gt(XtY(w), *y);
		*y = XtY(w);
		m_check_layout(m_index(w) + 1, y);
	}
}

START_TEST(container_positions_random)
{
	static const unsigned char layouts[] = {XmOUTLINE, XmDETAIL, XmSPATIAL};
	Widget c;
	int step, y = -1;

	c = XtVaCreateManagedWidget("c", xmContainerWidgetClass, shell, XmNlayoutType,
				    layouts[_i % 3], NULL);
	memset(m_count, 0, sizeof(m_count));
	m_items = 0;
	m_rng = 0x9e3779b9u * (unsigned int)(_i + 1);
	for (step = 0; step < 600; step++) {
		m_step(c);
		if (step % 50 == 49)
			m_check(c);
		if (step == 300) {
			XtRealizeWidget(shell);
			settle();
		}
	}
	m_check(c);
	settle();
	if (layouts[_i % 3] != XmSPATIAL)
		m_check_layout(0, &y);
	XtDestroyWidget(c);
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
	tcase_add_test(t, list_scroll_tiled_background);
	tcase_add_test(t, list_scroll_no_spacing);
	tcase_add_test(t, list_selected_items_missing);
	tcase_add_test(t, list_select_and_find);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);

	t = tcase_create("Container");
	tcase_add_test(t, container_positions);
	tcase_add_test(t, container_positions_head);
	tcase_add_test(t, container_positions_reorder);
	tcase_add_test(t, container_positions_destroy_parent);
	tcase_add_loop_test(t, container_positions_random, 0, 6);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 30);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
