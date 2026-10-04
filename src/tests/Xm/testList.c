/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Tests of the XmList item and selection management: lookups by value,
 * duplicates, the selection lists after adds, deletes and replacements,
 * the item and selection resources, and the item extents.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <X11/Intrinsic.h>
#include <Xm/XmP.h>
#include <Xm/List.h>
#include <check.h>

#include "suites.h"

static Widget shell;
static int warnings;

static void count_warning(String msg)
{
	(void)msg;
	warnings++;
}

static void _init_xt(void)
{
	shell = init_xt("check_List");
	XtVaSetValues(shell, XmNallowShellResize, True, NULL);
	XtAppSetWarningHandler(app, count_warning);
	warnings = 0;
}

static XmString item(const char *fmt, int i)
{
	char text[64];

	snprintf(text, sizeof(text), fmt, i);
	return XmStringCreateLocalized(text);
}

/* A two-segment string that compares equal to item("%s", text). */
static XmString item2(const char *head, const char *tail)
{
	XmString a = XmStringCreateLocalized((char *)head);
	XmString b = XmStringCreateLocalized((char *)tail);
	XmString s = XmStringConcat(a, b);

	XmStringFree(a);
	XmStringFree(b);
	return s;
}

static Widget make_list(unsigned char policy, int count, const char *fmt, int modulo)
{
	XmString *items = (XmString *)XtMalloc((count + 1) * sizeof(XmString));
	Widget list;
	Arg args[8];
	Cardinal n = 0;
	int i;

	for (i = 0; i < count; i++)
		items[i] = item(fmt, modulo ? i % modulo : i);
	if (count) {
		XtSetArg(args[n], XmNitems, items), n++;
		XtSetArg(args[n], XmNitemCount, count), n++;
	}
	XtSetArg(args[n], XmNvisibleItemCount, 10), n++;
	XtSetArg(args[n], XmNselectionPolicy, policy), n++;
	XtSetArg(args[n], XmNlistSizePolicy, XmVARIABLE), n++;
	list = XmCreateScrolledList(shell, "list", args, n);
	for (i = 0; i < count; i++)
		XmStringFree(items[i]);
	XtFree((char *)items);
	XtManageChild(list);
	return list;
}

static int item_count(Widget list)
{
	int count = -1;

	XtVaGetValues(list, XmNitemCount, &count, NULL);
	return count;
}

/* XmListItemPos and XmListGetMatchPos against XmStringCompare. */
static void check_lookup(Widget list, XmString s)
{
	XmString *items = NULL;
	int count = 0, i, first = 0, n = 0, *pos = NULL, npos = 0;
	Boolean found;

	XtVaGetValues(list, XmNitems, &items, XmNitemCount, &count, NULL);
	found = XmListGetMatchPos(list, s, &pos, &npos);
	for (i = 0; i < count; i++) {
		if (!XmStringCompare(items[i], s))
			continue;
		if (!first)
			first = i + 1;
		ck_assert_int_lt(n, npos);
		ck_assert_int_eq(pos[n], i + 1);
		n++;
	}
	ck_assert_int_eq(n, npos);
	ck_assert_int_eq(found, n > 0);
	ck_assert_int_eq(XmListItemPos(list, s), first);
	ck_assert_int_eq(XmListItemExists(list, s), first > 0);
	XtFree((char *)pos);
}

/* The selection lists hold the selected elements, in order. */
static int check_selection(Widget list)
{
	XmString *items = NULL, *sel = NULL;
	int *pos = NULL, *pos2 = NULL;
	int count = 0, nsel = 0, npos = 0, npos2 = 0, i, k = 0;

	XtVaGetValues(list, XmNitems, &items, XmNitemCount, &count, XmNselectedItems, &sel,
		      XmNselectedItemCount, &nsel, XmNselectedPositions, &pos,
		      XmNselectedPositionCount, &npos, NULL);
	ck_assert_int_eq(nsel, npos);
	for (i = 1; i <= count; i++) {
		if (!XmListPosSelected(list, i))
			continue;
		ck_assert_int_lt(k, npos);
		ck_assert_int_eq(pos[k], i);
		ck_assert(XmStringCompare(sel[k], items[i - 1]));
		k++;
	}
	ck_assert_int_eq(k, npos);
	if (XmListGetSelectedPos(list, &pos2, &npos2)) {
		ck_assert_int_eq(npos2, npos);
		ck_assert(!memcmp(pos2, pos, npos * sizeof(int)));
		XtFree((char *)pos2);
	}
	else
		ck_assert_int_eq(npos, 0);
	return npos;
}

/* The selected positions are those of the selected elements. */
static void expect_positions(Widget list, int n, const int *expected)
{
	int *pos = NULL, npos = 0, i, k = 0, count = item_count(list);

	XtVaGetValues(list, XmNselectedPositions, &pos, XmNselectedPositionCount, &npos, NULL);
	ck_assert_int_eq(npos, n);
	for (i = 0; i < n; i++)
		ck_assert_int_eq(pos[i], expected[i]);
	for (i = 1; i <= count; i++)
		if (XmListPosSelected(list, i)) {
			ck_assert_int_lt(k, n);
			ck_assert_int_eq(expected[k], i);
			k++;
		}
	ck_assert_int_eq(k, n);
}

static void expect_selected(Widget list, int n, const int *expected)
{
	int *pos = NULL, npos = 0, i;

	ck_assert_int_eq(check_selection(list), n);
	XtVaGetValues(list, XmNselectedPositions, &pos, XmNselectedPositionCount, &npos, NULL);
	for (i = 0; i < n; i++)
		ck_assert_int_eq(pos[i], expected[i]);
}

/*
 * Lookups find the first of duplicate items, and all of them, in short
 * and in long lists, as items are added, deleted and replaced.
 */
static void check_duplicates(int filler)
{
	Widget list = make_list(XmMULTIPLE_SELECT, filler, "filler %d", 0);
	XmString a = item("dup %d", 1), b = item("dup %d", 2), c = item("other %d", 3);
	XmString a2 = item2("dup ", "1");
	int pos[2] = { 1, 3 };

	XmListAddItemUnselected(list, a, 1);
	XmListAddItemUnselected(list, b, 2);
	XmListAddItemUnselected(list, a, 3);
	XmListAddItemUnselected(list, a, 0);
	check_lookup(list, a);
	check_lookup(list, a2);
	check_lookup(list, b);
	check_lookup(list, c);
	ck_assert_int_eq(XmListItemPos(list, a2), 1);
	/* Deleting the first one finds the next. */
	XmListDeletePos(list, 1);
	check_lookup(list, a);
	ck_assert_int_eq(XmListItemPos(list, a), 2);
	XmListAddItemUnselected(list, a, 1);
	check_lookup(list, a);
	ck_assert_int_eq(XmListItemPos(list, a), 1);
	/* Replacing moves items between the buckets. */
	XmListReplaceItemsPosUnselected(list, &c, 1, 1);
	check_lookup(list, a);
	check_lookup(list, c);
	ck_assert_int_eq(XmListItemPos(list, c), 1);
	XmListReplaceItemsUnselected(list, &a, 1, &b);
	check_lookup(list, a);
	check_lookup(list, b);
	ck_assert_int_eq(XmListItemPos(list, a), 0);
	XmListDeletePositions(list, pos, 2);
	check_lookup(list, b);
	check_lookup(list, c);
	XmListDeleteItem(list, b);
	check_lookup(list, b);
	XmListDeleteItemsPos(list, 3, 1);
	check_lookup(list, b);
	check_lookup(list, c);
	ck_assert_int_eq(item_count(list), filler + 4 - 2 - 1 - 3);
	ck_assert_int_eq(warnings, 0);
	XmStringFree(a);
	XmStringFree(a2);
	XmStringFree(b);
	XmStringFree(c);
}

START_TEST(list_duplicates_short)
{
	check_duplicates(10);
}
END_TEST

START_TEST(list_duplicates_long)
{
	check_duplicates(500);
}
END_TEST

/*
 * XmStringCompare leaves out tags and compares text with strncmp, so
 * that wide character strings with the same first character and length
 * compare equal: lookups match what it says.
 */
START_TEST(list_lookup_compare_semantics)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 300, "filler %d", 0);
	wchar_t w1[] = L"AB", w2[] = L"AC", w3[] = L"BB";
	XmString probes[6];
	int i, k;

	probes[0] = XmStringGenerate(w1, NULL, XmWIDECHAR_TEXT, NULL);
	probes[1] = XmStringGenerate(w2, NULL, XmWIDECHAR_TEXT, NULL);
	probes[2] = XmStringGenerate(w3, NULL, XmWIDECHAR_TEXT, NULL);
	probes[3] = XmStringCreate("tagged", "ISO8859-1");
	probes[4] = XmStringCreate("tagged", XmFONTLIST_DEFAULT_TAG);
	probes[5] = item2("tag", "ged");
	for (k = 0; k < 2; k++)
		for (i = 0; i < 6; i += 2)
			XmListAddItemUnselected(list, probes[i], 1 + (i * 37) % 300);
	for (i = 0; i < 6; i++)
		check_lookup(list, probes[i]);
	for (i = 0; i < 6; i++)
		XmStringFree(probes[i]);
}
END_TEST

/* A model of a list of numbered items with selection flags. */
#define MODEL_MAX 2048
static int model_item[MODEL_MAX];
static Boolean model_sel[MODEL_MAX];
static int model_count;
static unsigned int seed = 1;

static int rnd(int n)
{
	seed = seed * 1103515245u + 12345u;
	return n > 0 ? (int)((seed >> 8) % (unsigned int)n) : 0;
}

static void model_insert(int pos, int value)
{
	memmove(model_item + pos + 1, model_item + pos, (model_count - pos) * sizeof(int));
	memmove(model_sel + pos + 1, model_sel + pos, (model_count - pos) * sizeof(Boolean));
	model_item[pos] = value;
	model_sel[pos] = False;
	model_count++;
}

static void model_delete(int pos)
{
	model_count--;
	memmove(model_item + pos, model_item + pos + 1, (model_count - pos) * sizeof(int));
	memmove(model_sel + pos, model_sel + pos + 1, (model_count - pos) * sizeof(Boolean));
}

static int model_find(int value)
{
	int i;

	for (i = 0; i < model_count; i++)
		if (model_item[i] == value)
			return i;
	return -1;
}

static void model_check(Widget list, int range)
{
	XmString *items = NULL, *sel = NULL;
	int count = 0, nsel = 0, *pos = NULL, npos = 0, i, k = 0;

	XtVaGetValues(list, XmNitems, &items, XmNitemCount, &count, XmNselectedItems, &sel,
		      XmNselectedItemCount, &nsel, XmNselectedPositions, &pos,
		      XmNselectedPositionCount, &npos, NULL);
	ck_assert_int_eq(count, model_count);
	for (i = 0; i < count; i++) {
		XmString s = item("value %d", model_item[i]);

		ck_assert(XmStringCompare(items[i], s));
		XmStringFree(s);
		ck_assert_int_eq(XmListPosSelected(list, i + 1), model_sel[i]);
		if (model_sel[i]) {
			ck_assert_int_lt(k, npos);
			ck_assert_int_eq(pos[k], i + 1);
			ck_assert(XmStringCompare(sel[k], items[i]));
			k++;
		}
	}
	ck_assert_int_eq(k, npos);
	ck_assert_int_eq(nsel, npos);
	for (i = 0; i < 3; i++) {
		int v = rnd(range);
		XmString s = item("value %d", v);

		ck_assert_int_eq(XmListItemPos(list, s), model_find(v) + 1);
		XmStringFree(s);
	}
}

/*
 * Random adds, deletes and selections by position and by value in a
 * MULTIPLE_SELECT list with many duplicates, against a model: the
 * lookups and the selection lists must follow every change.
 */
static void check_model(int initial, int range, int ops)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 0, "", 0);
	int i, k, pos, v;
	XmString s;

	model_count = 0;
	for (i = 0; i < initial; i++) {
		v = rnd(range);
		s = item("value %d", v);
		XmListAddItemUnselected(list, s, 0);
		XmStringFree(s);
		model_insert(model_count, v);
	}
	for (i = 0; i < ops; i++) {
		int what = rnd(12);

		if (model_count < 2 && what >= 3)
			what = 0;
		if (model_count > MODEL_MAX - 10 && what < 2)
			what = 3;
		switch (what) {
		case 0:
		case 1:
			/* insert anywhere, or append */
			v = rnd(range);
			pos = rnd(model_count + 2);
			s = item("value %d", v);
			XmListAddItemUnselected(list, s, pos);
			XmStringFree(s);
			model_insert((pos == 0 || pos > model_count) ? model_count : pos - 1, v);
			break;
		case 2: {
			/* several at once */
			XmString t[3];
			pos = rnd(model_count + 1);
			for (k = 0; k < 3; k++) {
				v = rnd(range);
				t[k] = item("value %d", v);
				model_insert((pos == 0 ? model_count : pos - 1 + k), v);
			}
			XmListAddItemsUnselected(list, t, 3, pos);
			for (k = 0; k < 3; k++)
				XmStringFree(t[k]);
			break;
		}
		case 3:
			pos = rnd(model_count + 1);
			XmListDeletePos(list, pos);
			model_delete(pos ? pos - 1 : model_count - 1);
			break;
		case 4: {
			/* unsorted, with repeats */
			int p[4], n = 1 + rnd(4);
			Boolean del[MODEL_MAX] = { False };
			for (k = 0; k < n; k++) {
				p[k] = 1 + rnd(model_count);
				del[p[k] - 1] = True;
			}
			XmListDeletePositions(list, p, n);
			for (k = model_count - 1; k >= 0; k--)
				if (del[k])
					model_delete(k);
			break;
		}
		case 5: {
			int n = 1 + rnd(3);
			pos = 1 + rnd(model_count);
			XmListDeleteItemsPos(list, n, pos);
			for (k = 0; k < n && pos - 1 < model_count; k++)
				model_delete(pos - 1);
			break;
		}
		case 6:
			v = model_item[rnd(model_count)];
			s = item("value %d", v);
			XmListDeleteItem(list, s);
			XmStringFree(s);
			model_delete(model_find(v));
			break;
		case 7:
		case 8:
			pos = rnd(model_count + 1);
			XmListSelectPos(list, pos, False);
			pos = pos ? pos - 1 : model_count - 1;
			model_sel[pos] = !model_sel[pos];
			break;
		case 9:
			v = model_item[rnd(model_count)];
			s = item("value %d", v);
			XmListSelectItem(list, s, False);
			XmStringFree(s);
			pos = model_find(v);
			model_sel[pos] = !model_sel[pos];
			break;
		case 10:
			pos = rnd(model_count + 1);
			XmListDeselectPos(list, pos);
			model_sel[pos ? pos - 1 : model_count - 1] = False;
			break;
		case 11:
			v = model_item[rnd(model_count)];
			s = item("value %d", v);
			XmListDeselectItem(list, s);
			XmStringFree(s);
			model_sel[model_find(v)] = False;
			break;
		}
		model_check(list, range);
	}
	ck_assert_int_eq(warnings, 0);
}

START_TEST(list_model_short)
{
	seed = 7;
	check_model(20, 15, 1500);
}
END_TEST

START_TEST(list_model_long)
{
	seed = 11;
	check_model(400, 300, 1500);
}
END_TEST

START_TEST(list_model_duplicates)
{
	seed = 13;
	check_model(300, 4, 1000);
}
END_TEST

/*
 * Deleting selected items, by position, by positions in any order and
 * with repeats, by range and by value, keeps the others selected.
 */
static Boolean was_selected[201];

static void check_selected_survivors(Widget list)
{
	XmString *items = NULL;
	int count = 0, i, n = 0, k;

	XtVaGetValues(list, XmNitems, &items, XmNitemCount, &count, NULL);
	for (i = 0; i < count; i++) {
		for (k = 0; k <= 200; k++) {
			XmString s = item("item %d", k);
			Boolean same = XmStringCompare(items[i], s);

			XmStringFree(s);
			if (same)
				break;
		}
		ck_assert_int_le(k, 200);
		ck_assert_int_eq(XmListPosSelected(list, i + 1), was_selected[k]);
		n += was_selected[k];
	}
	ck_assert_int_eq(check_selection(list), n);
}

START_TEST(list_delete_selected)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 200, "item %d", 0);
	XmString s, t[2];
	int p[] = { 50, 10, 50, 300, 11 };
	int i;

	memset(was_selected, 0, sizeof(was_selected));
	for (i = 10; i <= 200; i += 10) {
		XmListSelectPos(list, i, False);
		was_selected[i - 1] = True;
	}
	XmListSelectPos(list, 12, False);
	was_selected[11] = True;
	XmListSelectPos(list, 50, False); /* toggles it off again */
	was_selected[49] = False;
	check_selected_survivors(list);
	XmListDeletePos(list, 1);
	check_selected_survivors(list);
	XmListDeletePositions(list, p, 5);
	ck_assert_int_eq(warnings, 1); /* position 300 */
	check_selected_survivors(list);
	XmListDeleteItemsPos(list, 47, 150);
	check_selected_survivors(list);
	XmListDeleteItemsPos(list, 1, 20);
	check_selected_survivors(list);
	s = item("item %d", 39);
	XmListDeleteItem(list, s);
	XmStringFree(s);
	check_selected_survivors(list);
	t[0] = item("item %d", 59);
	t[1] = item("item %d", 60);
	XmListDeleteItems(list, t, 2);
	XmStringFree(t[0]);
	XmStringFree(t[1]);
	check_selected_survivors(list);
	XmListDeletePos(list, 0);
	check_selected_survivors(list);
	XmListDeleteAllItems(list);
	ck_assert_int_eq(check_selection(list), 0);
	ck_assert_int_eq(warnings, 1);
}
END_TEST

/* Inserting before selected items moves their positions. */
START_TEST(list_add_moves_selection)
{
	Widget list = make_list(XmEXTENDED_SELECT, 100, "item %d", 0);
	XmString t[3];
	int e1[] = { 54 };
	int e2[] = { 7 };
	int i;

	for (i = 0; i < 3; i++)
		t[i] = item("new %d", i);
	XmListSelectPos(list, 50, False);
	XmListAddItemsUnselected(list, t, 3, 1);
	XmListAddItemUnselected(list, t[0], 30);
	XmListAddItemUnselected(list, t[0], 80);
	XmListAddItemUnselected(list, t[0], 0);
	expect_selected(list, 1, e1);
	ck_assert_int_eq(XmListGetKbdItemPos(list), 54);
	/* Extended selection by the API replaces the selection. */
	XmListSelectPos(list, 7, False);
	expect_selected(list, 1, e2);
	ck_assert_int_eq(XmListGetKbdItemPos(list), 7);
	for (i = 0; i < 3; i++)
		XmStringFree(t[i]);
}
END_TEST

/*
 * Items added with the selection checked are selected when they match a
 * selected item: they join the selected positions but not the selected
 * items, which name each selected value once.
 */
START_TEST(list_add_matching_selected)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 100, "item %d", 0);
	XmString s = item("item %d", 40), *sel = NULL;
	int e1[] = { 1, 42, 102 };
	int nsel = 0, npos = 0;

	XmListSelectPos(list, 41, False);
	XmListAddItem(list, s, 1);
	XmListAddItem(list, s, 0);
	XtVaGetValues(list, XmNselectedItems, &sel, XmNselectedItemCount, &nsel,
		      XmNselectedPositionCount, &npos, NULL);
	ck_assert_int_eq(nsel, 1);
	ck_assert_int_eq(npos, 3);
	ck_assert(XmStringCompare(sel[0], s));
	ck_assert(XmListPosSelected(list, 1));
	ck_assert(XmListPosSelected(list, 42));
	ck_assert(XmListPosSelected(list, 102));
	/* The selection is rebuilt from the elements. */
	XmListUpdateSelectedList(list);
	expect_selected(list, 3, e1);
	XmStringFree(s);
}
END_TEST

/*
 * Many selected items: the items added with the selection checked are
 * looked up in a set of them.
 */
START_TEST(list_add_checks_many_selected)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 300, "item %d", 0);
	XmString t[40];
	int i, npos = 0;

	for (i = 1; i <= 300; i += 3)
		XmListSelectPos(list, i, False);
	for (i = 0; i < 40; i++)
		t[i] = item("item %d", i);
	XmListAddItems(list, t, 40, 0);
	for (i = 0; i < 40; i++) {
		ck_assert_int_eq(XmListPosSelected(list, 301 + i), i % 3 == 0);
		XmStringFree(t[i]);
	}
	XtVaGetValues(list, XmNselectedPositionCount, &npos, NULL);
	ck_assert_int_eq(npos, 100 + 14);
}
END_TEST

/*
 * Replacing items by value replaces every match, in the order of the
 * old items: the second old item matches what the first was replaced
 * with.
 */
START_TEST(list_replace_all_matches)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 200, "item %d", 5);
	XmString o[2], r[2], *items = NULL;
	int i;

	o[0] = item("item %d", 1);
	o[1] = item("item %d", 2);
	r[0] = item("item %d", 2);
	r[1] = item("new %d", 9);
	check_lookup(list, o[0]);
	XmListReplaceItemsUnselected(list, o, 2, r);
	XtVaGetValues(list, XmNitems, &items, NULL);
	for (i = 0; i < 200; i++) {
		XmString s = (i % 5 == 1 || i % 5 == 2) ? item("new %d", 9) : item("item %d", i % 5);

		ck_assert(XmStringCompare(items[i], s));
		XmStringFree(s);
	}
	for (i = 0; i < 2; i++) {
		check_lookup(list, o[i]);
		check_lookup(list, r[i]);
	}
	ck_assert_int_eq(check_selection(list), 0);
	for (i = 0; i < 2; i++) {
		XmStringFree(o[i]);
		XmStringFree(r[i]);
	}
}
END_TEST

/*
 * Replacing a selected item keeps it selected, under its new name; the
 * unselected variants deselect it.  A replaced item that matches a
 * selected one is selected.
 */
START_TEST(list_replace_selected)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 300, "item %d", 0);
	XmString o = item("item %d", 20), r = item("new %d", 1), u = item("item %d", 40);
	XmString *sel = NULL;
	int e1[] = { 10, 21, 41 };
	int e2[] = { 10, 41 };
	int e3[] = { 10, 31, 41 };
	int e4[] = { 10, 31 };
	int p, nsel = 0;

	XmListSelectPos(list, 10, False);
	XmListSelectPos(list, 21, False);
	XmListSelectPos(list, 41, False);
	XmListReplaceItems(list, &o, 1, &r);
	expect_selected(list, 3, e1);
	XtVaGetValues(list, XmNselectedItems, &sel, XmNselectedItemCount, &nsel, NULL);
	ck_assert(XmStringCompare(sel[1], r));
	check_lookup(list, r);
	/* The selected items keep the deselected one until rebuilt. */
	XmListReplaceItemsUnselected(list, &r, 1, &o);
	expect_positions(list, 2, e2);
	XmListUpdateSelectedList(list);
	expect_selected(list, 2, e2);
	check_lookup(list, o);
	/* item 30 becomes item 40, which is selected */
	p = 31;
	XmListReplacePositions(list, &p, &u, 1);
	expect_positions(list, 3, e3);
	XmListUpdateSelectedList(list);
	expect_selected(list, 3, e3);
	XmListReplaceItemsPosUnselected(list, &u, 1, 41);
	expect_positions(list, 2, e4);
	XmListUpdateSelectedList(list);
	XmListReplaceItemsPos(list, &r, 1, 10);
	expect_selected(list, 2, e4);
	XtVaGetValues(list, XmNselectedItems, &sel, NULL);
	ck_assert(XmStringCompare(sel[0], r));
	check_lookup(list, u);
	XmStringFree(o);
	XmStringFree(r);
	XmStringFree(u);
}
END_TEST

/* XmNitems and XmNitemCount through XtSetValues, including the list's own. */
START_TEST(list_items_resource)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 300, "item %d", 0);
	XmString *items = NULL, s = item("item %d", 250), t = item("item %d", 120);
	XmString fresh[2];
	int *p = NULL, n = 0;

	check_lookup(list, s);
	XmListSelectPos(list, 3, False);
	XtVaGetValues(list, XmNitems, &items, NULL);
	XtVaSetValues(list, XmNitems, items + 100, XmNitemCount, 150, NULL);
	ck_assert_int_eq(item_count(list), 150);
	check_lookup(list, s);
	check_lookup(list, t);
	ck_assert_int_eq(XmListItemPos(list, t), 21);
	ck_assert_int_eq(XmListItemPos(list, s), 0);
	/* The selected item is gone; XmNselectedItems still names it. */
	ck_assert(!XmListGetSelectedPos(list, &p, &n));
	XmListSelectItem(list, t, False);
	ck_assert_int_eq(check_selection(list), 1);
	fresh[0] = s;
	fresh[1] = s;
	XtVaSetValues(list, XmNitems, fresh, XmNitemCount, 2, NULL);
	check_lookup(list, s);
	check_lookup(list, t);
	XtVaSetValues(list, XmNitemCount, 0, NULL);
	ck_assert_int_eq(item_count(list), 0);
	ck_assert_int_eq(XmListItemPos(list, s), 0);
	XmListAddItemUnselected(list, t, 0);
	check_lookup(list, t);
	XmStringFree(s);
	XmStringFree(t);
}
END_TEST

/* XmNselectedItems with many items, and XmNselectedPositions. */
START_TEST(list_selection_resources)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 300, "item %d", 100);
	XmString sel[20];
	int p[] = { 7, 3, 250 };
	int e[] = { 3, 7, 250 };
	int i, npos = 0;

	for (i = 0; i < 20; i++)
		sel[i] = item("item %d", 5 * i);
	XtVaSetValues(list, XmNselectedItems, sel, XmNselectedItemCount, 20, NULL);
	XtVaGetValues(list, XmNselectedPositionCount, &npos, NULL);
	ck_assert_int_eq(npos, 60);
	for (i = 1; i <= 300; i++)
		ck_assert_int_eq(XmListPosSelected(list, i), (i - 1) % 100 % 5 == 0);
	for (i = 0; i < 20; i++)
		XmStringFree(sel[i]);
	XmListDeselectAllItems(list);
	XtVaSetValues(list, XmNselectedPositions, p, XmNselectedPositionCount, 3, NULL);
	expect_selected(list, 3, e);
}
END_TEST

/*
 * XmNselectedPositions replacing a selection used to leave the widget
 * with the selected items it had just freed.
 */
START_TEST(list_selected_positions_replace_selection)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 50, "item %d", 0);
	XmString *sel = NULL, s = item("item %d", 9);
	int p[] = { 10, 20 };
	int nsel = 0;

	XmListSelectPos(list, 1, False);
	XmListSelectPos(list, 2, False);
	XtVaSetValues(list, XmNselectedPositions, p, XmNselectedPositionCount, 2, NULL);
	ck_assert_int_eq(check_selection(list), 2);
	XtVaGetValues(list, XmNselectedItems, &sel, XmNselectedItemCount, &nsel, NULL);
	ck_assert_int_eq(nsel, 2);
	ck_assert(XmStringCompare(sel[0], s));
	XmStringFree(s);
}
END_TEST

/*
 * More elements selected than the selected items name, then replaced
 * without selecting: the selected positions used to be counted below
 * zero, and the list ran out of memory.
 */
START_TEST(list_replace_unselected_duplicates)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 0, "", 0);
	XmString a = item("dup %d", 0), b = item("other %d", 0);
	int i;

	for (i = 0; i < 100; i++)
		XmListAddItemUnselected(list, b, 0);
	XmListSelectPos(list, 1, False);
	XmListReplaceItemsPos(list, &a, 1, 1);
	for (i = 0; i < 3; i++)
		XmListAddItem(list, a, 0); /* selected: they match */
	/* This counts the selected positions from the selected items. */
	XmListDeletePos(list, 50);
	XmListReplaceItemsUnselected(list, &a, 1, &b);
	ck_assert_int_eq(XmListItemPos(list, a), 0);
	for (i = 1; i <= 102; i++)
		ck_assert(!XmListPosSelected(list, i));
	XmStringFree(a);
	XmStringFree(b);
}
END_TEST

static Dimension list_width(Widget list)
{
	Dimension w = 0;

	XtVaGetValues(list, XmNwidth, &w, NULL);
	return w;
}

/* Items replaced by position are measured again without a selection. */
START_TEST(list_replace_pos_measures)
{
	Widget list = make_list(XmBROWSE_SELECT, 20, "item %d", 0);
	XmString wide = item("a much, much wider item %d", 1);
	Dimension base;

	XtRealizeWidget(shell);
	base = list_width(list);
	XmListReplaceItemsPos(list, &wide, 1, 5);
	ck_assert_int_gt(list_width(list), base);
	XmListSelectPos(list, 3, False);
	XmListReplaceItemsPos(list, &wide, 1, 3);
	ck_assert_int_eq(check_selection(list), 1);
	XmStringFree(wide);
}
END_TEST

/* Many items added and deleted one at a time, at both ends. */
START_TEST(list_grow_and_shrink)
{
	Widget list = make_list(XmMULTIPLE_SELECT, 0, "", 0);
	XmString *items = NULL, s;
	int i;

	for (i = 0; i < 3000; i++) {
		s = item("item %d", i);
		XmListAddItemUnselected(list, s, (i % 2) ? 0 : 1);
		XmStringFree(s);
	}
	ck_assert_int_eq(item_count(list), 3000);
	XtVaGetValues(list, XmNitems, &items, NULL);
	for (i = 0; i < 1500; i++) {
		XmString even = item("item %d", 2998 - 2 * i);
		XmString odd = item("item %d", 1 + 2 * i);

		ck_assert(XmStringCompare(items[i], even));
		ck_assert(XmStringCompare(items[1500 + i], odd));
		XmStringFree(even);
		XmStringFree(odd);
	}
	for (i = 0; i < 2990; i++)
		XmListDeletePos(list, (i % 2) ? 1 : 0);
	ck_assert_int_eq(item_count(list), 10);
	XtVaGetValues(list, XmNitems, &items, NULL);
	for (i = 0; i < 10; i++) {
		s = item("item %d", (i < 5) ? 8 - 2 * i : 1 + 2 * (i - 5));
		ck_assert(XmStringCompare(items[i], s));
		XmStringFree(s);
	}
	s = item("item %d", 8);
	check_lookup(list, s);
	XmStringFree(s);
}
END_TEST

void list_suite(SRunner *runner)
{
	Suite *s = suite_create("List");
	TCase *t;

	t = tcase_create("Items");
	tcase_add_test(t, list_duplicates_short);
	tcase_add_test(t, list_duplicates_long);
	tcase_add_test(t, list_lookup_compare_semantics);
	tcase_add_test(t, list_items_resource);
	tcase_add_test(t, list_replace_pos_measures);
	tcase_add_test(t, list_grow_and_shrink);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	t = tcase_create("Selection");
	tcase_add_test(t, list_model_short);
	tcase_add_test(t, list_model_long);
	tcase_add_test(t, list_model_duplicates);
	tcase_add_test(t, list_delete_selected);
	tcase_add_test(t, list_add_moves_selection);
	tcase_add_test(t, list_add_matching_selected);
	tcase_add_test(t, list_add_checks_many_selected);
	tcase_add_test(t, list_replace_all_matches);
	tcase_add_test(t, list_replace_selected);
	tcase_add_test(t, list_selection_resources);
	tcase_add_test(t, list_selected_positions_replace_selection);
	tcase_add_test(t, list_replace_unselected_duplicates);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
