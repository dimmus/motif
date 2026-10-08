/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Tests for the gadget cache (Cache.c): the records of a class without a
 * hash proc, which are searched along their list, and those of the
 * gadget classes, which are indexed by a hash of their contents.  Either
 * way, gadgets with the same cached resources must share one record, and
 * a record must live exactly as long as the gadgets that use it.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <Xm/XmP.h>
#include <Xm/CascadeBGP.h>
#include <Xm/IconGP.h>
#include <Xm/LabelGP.h>
#include <Xm/PushBGP.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoGP.h>
#include <Xm/ToggleBGP.h>
#include <check.h>

#include "CacheI.h"
#include "suites.h"

/* ------------------------------------------------------------------ */
/* A cache class of the application's own, without a hash proc         */
/* ------------------------------------------------------------------ */

typedef struct {
	int key;
	int extra; /* not compared */
} TestPart;

static int test_compare(XtPointer a, XtPointer b)
{
	return ((TestPart *)a)->key == ((TestPart *)b)->key;
}

static XmCacheClassPart test_cache = {
	{ NULL, NULL, 0 },
	_XmCacheCopy,
	_XmCacheDelete,
	test_compare,
};

static TestPart *cache_key(int key, int extra)
{
	TestPart part;

	part.key = key;
	part.extra = extra;
	return (TestPart *)_XmCachePart(&test_cache, (XtPointer)&part, sizeof part);
}

static int ref_count(XtPointer data)
{
	return ((XmGadgetCachePtr)DataToGadgetCache(data))->ref_count;
}

/* The records along the list of <cp>, checking its links. */
static int list_length(XmCacheClassPartPtr cp)
{
	XmGadgetCachePtr head = &ClassCacheHead(cp), p;
	int n = 0;

	for (p = head->next; p; p = p->next) {
		ck_assert_ptr_eq(p->prev->next, p);
		ck_assert_int_gt(p->ref_count, 0);
		n++;
	}
	return n;
}

START_TEST(unhashed_class)
{
	TestPart *a, *b, *c, *a2;

	a = cache_key(1, 10);
	b = cache_key(2, 20);
	c = cache_key(3, 30);
	ck_assert_int_eq(list_length(&test_cache), 3);
	ck_assert_int_eq(c->extra, 30);

	/* A match shares the record, whatever the fields it does not
	 * compare, and moves to the front of the list. */
	a2 = cache_key(1, 99);
	ck_assert_ptr_eq(a2, a);
	ck_assert_int_eq(a->extra, 10);
	ck_assert_int_eq(ref_count(a), 2);
	ck_assert_ptr_eq(CacheDataPtr(ClassCacheHead(&test_cache).next), a);
	ck_assert_int_eq(list_length(&test_cache), 3);

	_XmCacheDelete((XtPointer)a2);
	ck_assert_int_eq(ref_count(a), 1);
	_XmCacheDelete((XtPointer)a);
	_XmCacheDelete((XtPointer)b);
	ck_assert_int_eq(list_length(&test_cache), 1);
	ck_assert_ptr_eq(cache_key(3, 0), c);
	_XmCacheDelete((XtPointer)c);
	_XmCacheDelete((XtPointer)c);
	ck_assert_int_eq(list_length(&test_cache), 0);
}
END_TEST

/* Many distinct records, then all of them again, then freed. */
START_TEST(unhashed_class_many)
{
	enum { N = 500 };
	TestPart *parts[N];
	int i;

	for (i = 0; i < N; i++)
		parts[i] = cache_key(i, i);
	for (i = N - 1; i >= 0; i--)
		ck_assert_ptr_eq(cache_key(i, -1), parts[i]);
	ck_assert_int_eq(list_length(&test_cache), N);
	for (i = 0; i < N; i++) {
		ck_assert_int_eq(parts[i]->key, i);
		ck_assert_int_eq(ref_count(parts[i]), 2);
		_XmCacheDelete((XtPointer)parts[i]);
		_XmCacheDelete((XtPointer)parts[i]);
	}
	ck_assert_int_eq(list_length(&test_cache), 0);
}
END_TEST

void cache_suite(SRunner *runner)
{
	Suite *s = suite_create("Cache");
	TCase *t = tcase_create("Records");

	tcase_add_test(t, unhashed_class);
	tcase_add_test(t, unhashed_class_many);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

/* ------------------------------------------------------------------ */
/* The gadget classes, whose records are indexed                       */
/* ------------------------------------------------------------------ */

static Widget shell, rc;

static void _init_xt(void)
{
	shell = init_xt("check_GadgetCache");
	rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, shell, NULL);
}

static Widget label(WidgetClass wc, int look)
{
	return XtVaCreateWidget("g", wc, rc,
				XmNmarginWidth, (Dimension)(look % 50),
				XmNmarginHeight, (Dimension)(look / 50 % 50),
				NULL);
}

/*
 * Check the cache of the parts of <n> gadgets that <get> returns: parts
 * that <compare> finds equal are one record, used by as many gadgets as
 * its reference count says.
 */
static void check_shared(Widget *w, int n, XtPointer (*get)(Widget),
			 XmCacheClassPartPtr cp)
{
	XmCacheCompareProc compare = ClassCacheCompare(cp);
	int i, j, users;

	for (i = 0; i < n; i++) {
		XtPointer pi = get(w[i]);

		users = 0;
		for (j = 0; j < n; j++) {
			XtPointer pj = get(w[j]);

			if (pj == pi)
				users++;
			else
				ck_assert_msg(!compare(pi, pj),
					      "gadgets %d and %d have equal parts "
					      "in two records", i, j);
		}
		ck_assert_int_eq(ref_count(pi), users);
	}
}

static XtPointer label_part(Widget w)
{
	return (XtPointer)LabG_Cache(w);
}

/* Gadgets with the same look share their part; others do not. */
START_TEST(label_sharing)
{
	enum { LOOKS = 2000 };
	static Widget first[LOOKS], second[LOOKS];
	int i;

	for (i = 0; i < LOOKS; i++)
		first[i] = label(xmLabelGadgetClass, i);
	for (i = LOOKS - 1; i >= 0; i--)
		second[i] = label(xmLabelGadgetClass, i);
	for (i = 0; i < LOOKS; i++) {
		ck_assert_ptr_eq(LabG_Cache(second[i]), LabG_Cache(first[i]));
		ck_assert_int_eq(ref_count(LabG_Cache(first[i])), 2);
		if (i)
			ck_assert_ptr_ne(LabG_Cache(first[i]),
					 LabG_Cache(first[i - 1]));
		ck_assert_int_eq(LabG_MarginWidth(first[i]), i % 50);
	}

	/* Destroying the first set leaves the parts to the second. */
	for (i = 0; i < LOOKS; i++)
		XtDestroyWidget(first[i]);
	for (i = 0; i < LOOKS; i++) {
		ck_assert_int_eq(ref_count(LabG_Cache(second[i])), 1);
		ck_assert_int_eq(LabG_MarginHeight(second[i]), i / 50 % 50);
	}
	for (i = 0; i < LOOKS; i += 2)
		XtDestroyWidget(second[i]);
	/* ... and destroying the last user frees a part: a new gadget of
	 * that look gets a new one (which ASan would catch otherwise). */
	for (i = 0; i < LOOKS; i += 2) {
		first[i] = label(xmLabelGadgetClass, i);
		ck_assert_int_eq(ref_count(LabG_Cache(first[i])), 1);
		ck_assert_int_eq(LabG_MarginWidth(first[i]), i % 50);
	}
}
END_TEST

/*
 * Random creations, changes and destructions of gadgets of a few looks
 * keep the records shared and counted.
 */
START_TEST(label_random)
{
	enum { N = 60, STEPS = 3000 };
	Widget w[N];
	unsigned int seed = 12345;
	int i, step;

	for (i = 0; i < N; i++)
		w[i] = label(xmLabelGadgetClass, i % 7);
	for (step = 0; step < STEPS; step++) {
		int k = rand_r(&seed) % N, look = rand_r(&seed) % 9;

		switch (rand_r(&seed) % 3) {
		case 0:
			XtDestroyWidget(w[k]);
			w[k] = label(xmLabelGadgetClass, look);
			break;
		case 1:
			XtVaSetValues(w[k], XmNmarginWidth, (Dimension)look, NULL);
			break;
		default:
			XtVaSetValues(w[k], XmNforeground, (Pixel)(look & 3),
				      NULL);
			break;
		}
		if (step % 100 == 0)
			check_shared(w, N, label_part, LabG_ClassCachePart(NULL));
	}
	check_shared(w, N, label_part, LabG_ClassCachePart(NULL));
}
END_TEST

/* Each record of <cp> is used by as many of the <n> gadgets as it says. */
static void check_counts(Widget *w, int n, XtPointer (*get)(Widget), XmCacheClassPartPtr cp)
{
	XmGadgetCachePtr p;
	int i, users, records = 0;

	for (p = ClassCacheHead(cp).next; p; p = p->next) {
		users = 0;
		for (i = 0; i < n; i++)
			if (get(w[i]) == CacheDataPtr(p))
				users++;
		ck_assert_int_eq(p->ref_count, users);
		records++;
	}
	ck_assert_int_eq(records, list_length(cp));
}

static XtPointer toggle_part(Widget w)
{
	return (XtPointer)TBG_Cache(w);
}

/*
 * ToggleButtonGadget's SetValues writes label resources into the cached
 * label part in place, which leaves that record under the hash of its
 * old contents.  Random changes and destructions must still keep every
 * record counted, and free it (ASan) with its last gadget.
 */
START_TEST(toggle_random)
{
	enum { N = 40, STEPS = 2000 };
	Widget w[N];
	unsigned int seed = 54321;
	int i, step;

	for (i = 0; i < N; i++)
		w[i] = XtVaCreateWidget("t", xmToggleButtonGadgetClass, rc,
					XmNspacing, (Dimension)(i % 5), NULL);
	for (step = 0; step < STEPS; step++) {
		int k = rand_r(&seed) % N, look = rand_r(&seed) % 6;

		switch (rand_r(&seed) % 4) {
		case 0:
			XtDestroyWidget(w[k]);
			w[k] = XtVaCreateWidget("t", xmToggleButtonGadgetClass, rc,
						XmNspacing, (Dimension)look, NULL);
			break;
		case 1:
			XtVaSetValues(w[k], XmNmarginWidth, (Dimension)look, NULL);
			break;
		case 2:
			XtVaSetValues(w[k], XmNspacing, (Dimension)look, NULL);
			break;
		default:
			XtVaSetValues(w[k], XmNforeground, (Pixel)(look & 3),
				      XmNmarginHeight, (Dimension)(look >> 1), NULL);
			break;
		}
		if (step % 100 == 0) {
			check_counts(w, N, label_part, LabG_ClassCachePart(NULL));
			check_counts(w, N, toggle_part, TBG_ClassCachePart(NULL));
		}
	}
	check_counts(w, N, label_part, LabG_ClassCachePart(NULL));
	check_shared(w, N, toggle_part, TBG_ClassCachePart(NULL));
	for (i = 0; i < N; i++)
		XtDestroyWidget(w[i]);
	ck_assert_int_eq(list_length(LabG_ClassCachePart(NULL)), 0);
	ck_assert_int_eq(list_length(TBG_ClassCachePart(NULL)), 0);
}
END_TEST

static XtPointer push_part(Widget w)
{
	return (XtPointer)PBG_Cache(w);
}

static void pump(int ms)
{
	int i;

	for (i = 0; i < ms; i++) {
		while (XtAppPending(app))
			XtAppProcessEvent(app, XtIMAll);
		usleep(1000);
	}
}

/*
 * PushButtonGadget keeps its activation timer in its cached part, and
 * changes it there: while it runs, a new gadget of the same look gets a
 * part of its own, and once it has run, the look's part is shared again.
 */
START_TEST(push_timer)
{
	Widget a, b, c, d;
	XmGadgetClass gc = (XmGadgetClass)xmPushButtonGadgetClass;
	XButtonEvent ev;

	a = XtVaCreateManagedWidget("a", xmPushButtonGadgetClass, rc, NULL);
	b = XtVaCreateManagedWidget("b", xmPushButtonGadgetClass, rc, NULL);
	ck_assert_ptr_eq(PBG_Cache(a), PBG_Cache(b));
	XtRealizeWidget(shell);
	pump(20);

	memset(&ev, 0, sizeof ev);
	ev.type = ButtonPress;
	ev.display = XtDisplay(shell);
	ev.window = XtWindow(rc);
	ev.time = CurrentTime;
	gc->gadget_class.arm_and_activate(a, (XEvent *)&ev, NULL, NULL);
	ck_assert(PBG_Timer(a) != 0);
	ck_assert(PBG_Timer(b) != 0); /* the part is shared */

	c = XtVaCreateManagedWidget("c", xmPushButtonGadgetClass, rc, NULL);
	ck_assert_ptr_ne(PBG_Cache(c), PBG_Cache(a));
	ck_assert(PBG_Timer(c) == 0);

	pump(400);
	ck_assert(PBG_Timer(a) == 0);
	d = XtVaCreateManagedWidget("d", xmPushButtonGadgetClass, rc, NULL);
	/* a's part and c's are equal again: d shares one of them (two
	 * records of one look are allowed here, as before the index). */
	ck_assert(PBG_Cache(d) == PBG_Cache(a) || PBG_Cache(d) == PBG_Cache(c));
	ck_assert_ptr_ne(PBG_Cache(a), PBG_Cache(c));
	ck_assert_int_eq(ref_count(PBG_Cache(a)) + ref_count(PBG_Cache(c)), 4);
	XtDestroyWidget(a);
	XtDestroyWidget(b);
	XtDestroyWidget(c);
	XtDestroyWidget(d);
}
END_TEST

static XtPointer icon_part(Widget w)
{
	return (XtPointer)IG_Cache(w);
}

/* The other gadget classes with a cache part of their own. */
START_TEST(other_classes)
{
	enum { N = 300 };
	static Widget w[2][N], all[2 * N];
	Widget menu;
	int i, k;

	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			w[k][i] = XtVaCreateWidget("t", xmToggleButtonGadgetClass, rc,
						   XmNspacing, (Dimension)(i % 100),
						   XmNindicatorSize, (Dimension)(8 + i / 100),
						   NULL);
	for (i = 0; i < N; i++) {
		/* (Their label parts are not shared: ToggleButtonGadget
		 * changes them in place once they are cached.) */
		ck_assert_ptr_eq(TBG_Cache(w[0][i]), TBG_Cache(w[1][i]));
		ck_assert_int_eq(TBG_Spacing(w[1][i]), i % 100);
		if (i)
			ck_assert_ptr_ne(TBG_Cache(w[0][i]), TBG_Cache(w[0][i - 1]));
	}
	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			XtDestroyWidget(w[k][i]);

	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			w[k][i] = XtVaCreateWidget("s", xmSeparatorGadgetClass, rc,
						   XmNmargin, (Dimension)i, NULL);
	for (i = 0; i < N; i++) {
		ck_assert_ptr_eq(SEPG_Cache(w[0][i]), SEPG_Cache(w[1][i]));
		ck_assert_int_eq(SEPG_Margin(w[1][i]), i);
	}
	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			XtDestroyWidget(w[k][i]);

	menu = XmCreatePulldownMenu(rc, "menu", NULL, 0);
	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			w[k][i] = XtVaCreateWidget("c", xmCascadeButtonGadgetClass, menu,
						   XmNmappingDelay, i, NULL);
	for (i = 0; i < N; i++) {
		ck_assert_ptr_eq(CBG_Cache(w[0][i]), CBG_Cache(w[1][i]));
		ck_assert_int_eq(CBG_MapDelay(w[1][i]), i);
	}
	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			XtDestroyWidget(w[k][i]);

	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			w[k][i] = XtVaCreateWidget("i", xmIconGadgetClass, rc,
						   XmNspacing, (Dimension)i, NULL);
	/* (Equal looks need not share a part here: each gadget copies
	 * its render table, and the parts compare that by address.) */
	for (i = 0; i < N; i++)
		ck_assert_int_eq(IG_Spacing(w[1][i]), i);
	memcpy(all, w, sizeof w);
	check_shared(all, 2 * N, icon_part, IG_ClassCachePart(NULL));
	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			XtDestroyWidget(w[k][i]);

	for (k = 0; k < 2; k++)
		for (i = 0; i < N; i++)
			w[k][i] = XtVaCreateWidget("p", xmPushButtonGadgetClass, rc,
						   XmNdefaultButtonShadowThickness,
						   (Dimension)i, NULL);
	for (i = 0; i < N; i++)
		ck_assert_ptr_eq(PBG_Cache(w[0][i]), PBG_Cache(w[1][i]));
	memcpy(all, w, sizeof w);
	check_shared(all, 2 * N, push_part, PBG_ClassCachePart(NULL));
}
END_TEST

void gadget_cache_suite(SRunner *runner)
{
	Suite *s = suite_create("GadgetCache");
	TCase *t = tcase_create("Gadgets");

	tcase_add_test(t, label_sharing);
	tcase_add_test(t, label_random);
	tcase_add_test(t, toggle_random);
	tcase_add_test(t, push_timer);
	tcase_add_test(t, other_classes);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
