/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * XmeTraitSet and XmeTraitGet: setting, replacing and removing traits,
 * enough of them that the table grows and removals move other traits,
 * and lookups from several threads while another thread sets traits
 * (XmeTraitGet does not take the process lock; build with WITH_TSAN to
 * have ThreadSanitizer check the stress case).  No X server is needed.
 */
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/TraitP.h>
#include <Xm/CareVisualT.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ArrowB.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <check.h>

#include "suites.h"

/* Distinct objects to install traits on, and trait values: never NULL. */
#define N_OBJS 6000
static char objs[N_OBJS];
#define OBJ(i) ((XtPointer)&objs[i])
#define VALUE(i, k) ((XtPointer)(uintptr_t)(((uintptr_t)(i) << 4) | ((k) << 1) | 1))

static XrmQuark qa, qb;

static void init_quarks(void)
{
	qa = XrmPermStringToQuark("XmTtestTraitA");
	qb = XrmPermStringToQuark("XmTtestTraitB");
}

START_TEST(set_replace_remove)
{
	init_quarks();
	ck_assert_ptr_null(XmeTraitGet(OBJ(0), qa));
	/* Removing what is not there is harmless. */
	ck_assert(XmeTraitSet(OBJ(0), qa, NULL));
	ck_assert_ptr_null(XmeTraitGet(OBJ(0), qa));

	ck_assert(XmeTraitSet(OBJ(0), qa, VALUE(0, 0)));
	ck_assert_ptr_eq(XmeTraitGet(OBJ(0), qa), VALUE(0, 0));
	/* Same object, other trait; same trait, other object. */
	ck_assert_ptr_null(XmeTraitGet(OBJ(0), qb));
	ck_assert_ptr_null(XmeTraitGet(OBJ(1), qa));
	ck_assert(XmeTraitSet(OBJ(0), qb, VALUE(0, 1)));
	ck_assert_ptr_eq(XmeTraitGet(OBJ(0), qa), VALUE(0, 0));
	ck_assert_ptr_eq(XmeTraitGet(OBJ(0), qb), VALUE(0, 1));

	/* Setting again replaces, removing removes (the old value does not
	 * come back). */
	ck_assert(XmeTraitSet(OBJ(0), qa, VALUE(0, 2)));
	ck_assert_ptr_eq(XmeTraitGet(OBJ(0), qa), VALUE(0, 2));
	ck_assert(XmeTraitSet(OBJ(0), qa, NULL));
	ck_assert_ptr_null(XmeTraitGet(OBJ(0), qa));
	ck_assert_ptr_eq(XmeTraitGet(OBJ(0), qb), VALUE(0, 1));
	ck_assert(XmeTraitSet(OBJ(0), qb, NULL));
	ck_assert_ptr_null(XmeTraitGet(OBJ(0), qb));
}
END_TEST

static void check_all(int n, int removed_odd, int k)
{
	int i;

	for (i = 0; i < n; i++) {
		XtPointer want = (removed_odd && (i & 1)) ? NULL : VALUE(i, k);
		ck_assert_msg(XmeTraitGet(OBJ(i), qa) == want,
			      "object %d: wrong trait", i);
		ck_assert_ptr_null(XmeTraitGet(OBJ(i), qb));
	}
}

/* Thousands of traits: the table grows several times, and removals
 * shift the traits that follow back, wrapping around the end. */
START_TEST(many_traits)
{
	int i, round;

	init_quarks();
	for (i = 0; i < N_OBJS; i++) {
		XmeTraitSet(OBJ(i), qa, VALUE(i, 0));
		if ((i & 255) == 0)
			check_all(i + 1, 0, 0);
	}
	check_all(N_OBJS, 0, 0);
	for (round = 0; round < 3; round++) {
		for (i = 1; i < N_OBJS; i += 2)
			XmeTraitSet(OBJ(i), qa, NULL);
		check_all(N_OBJS, 1, round);
		for (i = 0; i < N_OBJS; i++)
			XmeTraitSet(OBJ(i), qa, VALUE(i, round + 1));
		check_all(N_OBJS, 0, round + 1);
	}
	/* Remove them all, in an order unrelated to the slots. */
	for (i = 0; i < N_OBJS; i++)
		XmeTraitSet(OBJ((i * 7919) % N_OBJS), qa, NULL);
	for (i = 0; i < N_OBJS; i++)
		ck_assert_ptr_null(XmeTraitGet(OBJ(i), qa));
}
END_TEST

/*
 * Readers look traits up without stopping while a writer changes the
 * table.  Objects [0, N_STABLE) have trait A all along; the N_CHURN
 * objects after them have trait B removed and set again over and over,
 * which moves the slots that follow back and forth; one object has
 * trait A replaced over and over.  In the growth case the writer also
 * installs trait A on new objects, so that the table grows several
 * times, and initializes real widget classes, which install their own
 * traits.  A reader must never miss a trait that is there all along,
 * nor see a value that was never set.
 *
 * The churn case keeps the table small and well filled (about 360
 * traits in the initial 512 slots), so that removals move many slots:
 * without the sequence check in XmeTraitGet its readers see wrong
 * values, and with plain loads of the slots ThreadSanitizer reports
 * the races.
 */
#define N_STABLE 120
#define N_CHURN 230
#define FIRST_NEW (N_STABLE + N_CHURN)
#define N_NEW (N_OBJS - FIRST_NEW - 1)
#define REPLACED (N_OBJS - 1)
#define N_READERS 3
#define N_ROUNDS 400000

static atomic_int readers_started, writer_done;
static atomic_long errors, overlapped;

static WidgetClass *stress_classes[] = {
	&xmPushButtonWidgetClass, &xmPushButtonGadgetClass,
	&xmArrowButtonWidgetClass, &xmToggleButtonWidgetClass,
	&xmTextWidgetClass, &xmTextFieldWidgetClass,
};
#define N_CLASSES (sizeof stress_classes / sizeof stress_classes[0])
static XtPointer class_seen[N_READERS][N_CLASSES];

static void wait_for_readers(void)
{
	while (atomic_load(&readers_started) < N_READERS)
		sched_yield();
}

/* Trait B of churn object i after round r of churn_writer. */
static XtPointer churn_value(int i, int r)
{
	return (r / N_CHURN) & 1 ? VALUE(N_STABLE + i, 0) : NULL;
}

static void *churn_writer(void *arg)
{
	int r, i;

	(void)arg;
	wait_for_readers();
	for (r = 0; r < N_ROUNDS; r++) {
		i = r % N_CHURN;
		XmeTraitSet(OBJ(N_STABLE + i), qb, churn_value(i, r));
		XmeTraitSet(OBJ(REPLACED), qa, VALUE(REPLACED, r & 1));
	}
	atomic_store(&writer_done, 1);
	return NULL;
}

static void *growth_writer(void *arg)
{
	int i, per_class = N_NEW / N_CLASSES;

	(void)arg;
	wait_for_readers();
	for (i = 0; i < N_NEW; i++) {
		XmeTraitSet(OBJ(FIRST_NEW + i), qa, VALUE(FIRST_NEW + i, 0));
		XmeTraitSet(OBJ(REPLACED), qa, VALUE(REPLACED, i & 1));
		if (i % per_class == 0 && i / per_class < (int)N_CLASSES)
			XtInitializeWidgetClass(*stress_classes[i / per_class]);
	}
	atomic_store(&writer_done, 1);
	return NULL;
}

static void *reader(void *arg)
{
	int id = (int)(intptr_t)arg;
	long n = 0, bad = 0;
	unsigned i = (unsigned)id * 7919u;
	XtPointer v;
	size_t c;

	atomic_fetch_add(&readers_started, 1);
	while (!atomic_load(&writer_done)) {
		int s = i % N_STABLE, k = N_STABLE + i % N_CHURN;
		int j = FIRST_NEW + (int)(i % N_NEW);

		if (XmeTraitGet(OBJ(s), qa) != VALUE(s, 0))
			bad++;
		v = XmeTraitGet(OBJ(k), qb);
		if (v != NULL && v != VALUE(k, 0))
			bad++;
		v = XmeTraitGet(OBJ(j), qa);
		if (v != NULL && v != VALUE(j, 0))
			bad++;
		v = XmeTraitGet(OBJ(REPLACED), qa);
		if (v != VALUE(REPLACED, 0) && v != VALUE(REPLACED, 1))
			bad++;
		c = i % N_CLASSES;
		if ((v = XmeTraitGet(*stress_classes[c], XmQTcareParentVisual)))
			class_seen[id][c] = v;
		i = i * 1103515245u + 12345u;
		n++;
	}
	atomic_fetch_add(&errors, bad);
	atomic_fetch_add(&overlapped, n);
	return NULL;
}

static void run_threads(void *(*writer)(void *))
{
	pthread_t readers[N_READERS], w;
	int i;

	ck_assert(XtToolkitThreadInitialize());
	XtToolkitInitialize();
	/* Initializes the trait names, before the readers use them. */
	XtInitializeWidgetClass(xmPrimitiveWidgetClass);
	init_quarks();
	for (i = 0; i < N_STABLE; i++)
		XmeTraitSet(OBJ(i), qa, VALUE(i, 0));
	for (i = N_STABLE; i < FIRST_NEW; i++)
		XmeTraitSet(OBJ(i), qb, VALUE(i, 0));
	XmeTraitSet(OBJ(REPLACED), qa, VALUE(REPLACED, 0));

	for (i = 0; i < N_READERS; i++)
		ck_assert_int_eq(pthread_create(&readers[i], NULL, reader,
						(void *)(intptr_t)i), 0);
	ck_assert_int_eq(pthread_create(&w, NULL, writer, NULL), 0);
	pthread_join(w, NULL);
	for (i = 0; i < N_READERS; i++)
		pthread_join(readers[i], NULL);

	ck_assert_msg(atomic_load(&errors) == 0, "%ld wrong lookups", (long)atomic_load(&errors));
	/* The readers did run while the writer did. */
	ck_assert(atomic_load(&overlapped) > 0);
	for (i = 0; i < N_STABLE; i++)
		ck_assert_ptr_eq(XmeTraitGet(OBJ(i), qa), VALUE(i, 0));
}

START_TEST(threaded_churn)
{
	int i;

	run_threads(churn_writer);
	for (i = 0; i < N_CHURN; i++) {
		int last = (N_ROUNDS - 1 - i) / N_CHURN * N_CHURN + i;

		ck_assert_ptr_eq(XmeTraitGet(OBJ(N_STABLE + i), qb),
				 churn_value(i, last));
	}
}
END_TEST

START_TEST(threaded_growth)
{
	int i, r;
	size_t c;

	run_threads(growth_writer);
	for (i = 0; i < N_NEW; i++)
		ck_assert_ptr_eq(XmeTraitGet(OBJ(FIRST_NEW + i), qa),
				 VALUE(FIRST_NEW + i, 0));
	/* A class trait seen while the class was being initialized is the
	 * one it ends up with. */
	for (c = 0; c < N_CLASSES; c++) {
		XtPointer v = XmeTraitGet(*stress_classes[c], XmQTcareParentVisual);

		ck_assert_ptr_nonnull(v);
		for (r = 0; r < N_READERS; r++)
			ck_assert(class_seen[r][c] == NULL || class_seen[r][c] == v);
	}
}
END_TEST

void traits_suite(SRunner *runner)
{
	Suite *s = suite_create("Traits");
	TCase *t = tcase_create("Set and get");
	TCase *mt = tcase_create("Threads");

	tcase_add_test(t, set_replace_remove);
	tcase_add_test(t, many_traits);
	suite_add_tcase(s, t);
	/* ThreadSanitizer builds are several times slower. */
	tcase_set_timeout(mt, 120);
	tcase_add_test(mt, threaded_churn);
	tcase_add_test(mt, threaded_growth);
	suite_add_tcase(s, mt);
	srunner_add_suite(runner, s);
}

