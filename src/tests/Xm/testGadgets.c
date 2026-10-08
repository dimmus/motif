/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Gadget Get/SetValues and the extension data stacks (BaseClass.c).
 *
 * A gadget keeps part of its resources in records shared through the
 * gadget cache.  For every XtGetValues and XtSetValues it copies them
 * into scratch secondary objects and pushes extension data that point to
 * them on the gadget's stack, and pops it again at the end.  Shells keep
 * the extension data of their VendorShellExt object on such a stack for
 * their lifetime.  These tests check the stacks themselves, the values
 * that go through them for every gadget class, and (under LeakSanitizer)
 * that the recycled records do not leak.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <Xm/XmP.h>
#include <Xm/ExtObjectP.h>
#include <Xm/ArrowBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/IconG.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleBG.h>
#include <check.h>

#include "BaseClassI.h"
#include "leak.h"
#include "suites.h"

/* Extension types that no Motif class uses. */
#define EXT_A 100
#define EXT_B 101

static Widget top;

static void setup(void)
{
	top = init_xt("check_Gadgets");
}

static void teardown(void)
{
	uninit_xt();
	top = NULL;
}

static XmWidgetExtData pop(Widget w, unsigned char type)
{
	XmWidgetExtData data = (XmWidgetExtData)1;

	_XmPopWidgetExtData(w, &data, type);
	return data;
}

/* Stacks are per widget and per extension type, last in first out. */
START_TEST(ext_data_stack)
{
	XmWidgetExtDataRec rec[4];
	Widget rc, a, b;

	rc = XtCreateWidget("rc", xmRowColumnWidgetClass, top, NULL, 0);
	a = XtCreateWidget("a", xmLabelGadgetClass, rc, NULL, 0);
	b = XtCreateWidget("b", xmLabelGadgetClass, rc, NULL, 0);

	ck_assert_ptr_null(_XmGetWidgetExtData(a, EXT_A));
	ck_assert_ptr_null(pop(a, EXT_A));

	_XmPushWidgetExtData(a, &rec[0], EXT_A);
	_XmPushWidgetExtData(a, &rec[1], EXT_A);
	_XmPushWidgetExtData(a, &rec[2], EXT_B);
	_XmPushWidgetExtData(b, &rec[3], EXT_A);
	ck_assert_ptr_eq(_XmGetWidgetExtData(a, EXT_A), &rec[1]);
	ck_assert_ptr_eq(_XmGetWidgetExtData(a, EXT_B), &rec[2]);
	ck_assert_ptr_eq(_XmGetWidgetExtData(b, EXT_A), &rec[3]);
	ck_assert_ptr_null(_XmGetWidgetExtData(b, EXT_B));

	ck_assert_ptr_eq(pop(a, EXT_A), &rec[1]);
	ck_assert_ptr_eq(_XmGetWidgetExtData(a, EXT_A), &rec[0]);
	ck_assert_ptr_eq(pop(a, EXT_A), &rec[0]);
	ck_assert_ptr_null(_XmGetWidgetExtData(a, EXT_A));
	ck_assert_ptr_null(pop(a, EXT_A));

	/* The other stacks are untouched. */
	ck_assert_ptr_eq(_XmGetWidgetExtData(a, EXT_B), &rec[2]);
	ck_assert_ptr_eq(_XmGetWidgetExtData(b, EXT_A), &rec[3]);
	ck_assert_ptr_eq(pop(a, EXT_B), &rec[2]);
	ck_assert_ptr_eq(pop(b, EXT_A), &rec[3]);
	ck_assert_ptr_null(_XmGetWidgetExtData(a, EXT_B));
	ck_assert_ptr_null(_XmGetWidgetExtData(b, EXT_A));

	/* A NULL record is a record. */
	_XmPushWidgetExtData(a, NULL, EXT_A);
	_XmPushWidgetExtData(a, &rec[0], EXT_A);
	ck_assert_ptr_eq(pop(a, EXT_A), &rec[0]);
	ck_assert_ptr_null(_XmGetWidgetExtData(a, EXT_A));
	ck_assert_ptr_null(pop(a, EXT_A));
}
END_TEST

/* Enough stacks at once to make the table grow a few times. */
#define N_STACKS 600

START_TEST(ext_data_many_stacks)
{
	static XmWidgetExtDataRec rec[N_STACKS][2];
	static Widget w[N_STACKS];
	Widget rc;
	int i;

	rc = XtCreateWidget("rc", xmRowColumnWidgetClass, top, NULL, 0);
	for (i = 0; i < N_STACKS; i++) {
		w[i] = XtCreateWidget("g", xmLabelGadgetClass, rc, NULL, 0);
		_XmPushWidgetExtData(w[i], &rec[i][0], EXT_A);
		if (i % 3 == 0)
			_XmPushWidgetExtData(w[i], &rec[i][1], EXT_A);
	}
	for (i = 0; i < N_STACKS; i++)
		ck_assert_ptr_eq(_XmGetWidgetExtData(w[i], EXT_A),
				 &rec[i][i % 3 == 0]);

	/* Empty every other stack, then check and empty the rest. */
	for (i = 0; i < N_STACKS; i += 2) {
		if (i % 3 == 0)
			ck_assert_ptr_eq(pop(w[i], EXT_A), &rec[i][1]);
		ck_assert_ptr_eq(pop(w[i], EXT_A), &rec[i][0]);
		ck_assert_ptr_null(_XmGetWidgetExtData(w[i], EXT_A));
	}
	for (i = N_STACKS - 1; i > 0; i -= 2) {
		ck_assert_ptr_eq(_XmGetWidgetExtData(w[i], EXT_A),
				 &rec[i][i % 3 == 0]);
		if (i % 3 == 0)
			ck_assert_ptr_eq(pop(w[i], EXT_A), &rec[i][1]);
		ck_assert_ptr_eq(pop(w[i], EXT_A), &rec[i][0]);
	}
	for (i = 0; i < N_STACKS; i++)
		ck_assert_ptr_null(_XmGetWidgetExtData(w[i], EXT_A));
}
END_TEST

/*
 * The gadget classes, each with a resource that lives in its cache part
 * (or, for the ArrowButtonGadget, which has none, in the instance).
 */
static const struct gadget_class {
	const char *name;
	WidgetClass *wc;
	const char *resource;
	int in_menu; /* needs a menu parent */
} gadget_classes[] = {
	{ "LabelGadget", &xmLabelGadgetClass, XmNmarginWidth, 0 },
	{ "PushButtonGadget", &xmPushButtonGadgetClass, XmNmarginWidth, 0 },
	{ "ToggleButtonGadget", &xmToggleButtonGadgetClass, XmNmarginWidth, 0 },
	{ "CascadeButtonGadget", &xmCascadeButtonGadgetClass, XmNmarginWidth, 1 },
	{ "SeparatorGadget", &xmSeparatorGadgetClass, XmNmargin, 0 },
	{ "IconGadget", &xmIconGadgetClass, XmNmarginWidth, 0 },
	{ "ArrowButtonGadget", &xmArrowButtonGadgetClass, XmNshadowThickness, 0 },
};

#define N_GADGET_CLASSES (sizeof gadget_classes / sizeof gadget_classes[0])

static Widget gadget_parent(const struct gadget_class *gc)
{
	Widget parent;

	if (gc->in_menu)
		parent = XmCreateMenuBar(top, "bar", NULL, 0);
	else
		parent = XmCreateRowColumn(top, "rc", NULL, 0);
	XtManageChild(parent);
	return parent;
}

static Dimension get_dimension(Widget w, const char *resource)
{
	Dimension value = 0;

	XtVaGetValues(w, resource, &value, NULL);
	return value;
}

/*
 * Values go through the scratch records both ways, and the gadget hooks
 * push and pop only their own extension data: a record pushed on the
 * gadget's cache extension stack before is still on top afterwards.
 */
START_TEST(gadget_values)
{
	const struct gadget_class *gc = &gadget_classes[_i];
	XmWidgetExtDataRec mine;
	XmWidgetExtData got;
	Widget parent, w;
	Dimension d;

	parent = gadget_parent(gc);
	w = XtVaCreateManagedWidget("gadget", *gc->wc, parent, gc->resource, 5,
				    NULL);
	XtRealizeWidget(top);
	ck_assert_msg(get_dimension(w, gc->resource) == 5, "%s: %s", gc->name,
		      gc->resource);
	ck_assert_ptr_null(_XmGetWidgetExtData(w, XmCACHE_EXTENSION));

	_XmPushWidgetExtData(w, &mine, XmCACHE_EXTENSION);
	XtVaSetValues(w, gc->resource, 9, NULL);
	ck_assert_ptr_eq(_XmGetWidgetExtData(w, XmCACHE_EXTENSION), &mine);
	d = get_dimension(w, gc->resource);
	ck_assert_ptr_eq(_XmGetWidgetExtData(w, XmCACHE_EXTENSION), &mine);
	_XmPopWidgetExtData(w, &got, XmCACHE_EXTENSION);
	ck_assert_ptr_eq(got, &mine);
	ck_assert_msg(d == 9, "%s: %s is %u, not 9", gc->name, gc->resource, d);

	/* And back again, with nothing left on the stack. */
	XtVaSetValues(w, gc->resource, 5, NULL);
	ck_assert_uint_eq(get_dimension(w, gc->resource), 5);
	ck_assert_ptr_null(_XmGetWidgetExtData(w, XmCACHE_EXTENSION));
	XtDestroyWidget(parent);
}
END_TEST

/*
 * Create, change, read and destroy gadgets of every class many times: the
 * recycled scratch objects and extension data records must not leak.
 */
START_TEST(gadget_life_cycle)
{
	Widget parent[N_GADGET_CLASSES], w;
	Dimension d;
	size_t i;
	int n;

	for (i = 0; i < N_GADGET_CLASSES; i++)
		parent[i] = gadget_parent(&gadget_classes[i]);
	XtRealizeWidget(top);
	for (n = 0; n < 100; n++)
		for (i = 0; i < N_GADGET_CLASSES; i++) {
			const struct gadget_class *gc = &gadget_classes[i];

			w = XtVaCreateManagedWidget("gadget", *gc->wc, parent[i],
						    NULL);
			XtVaSetValues(w, gc->resource, 2 + n % 7,
				      XmNsensitive, n & 1, NULL);
			d = get_dimension(w, gc->resource);
			ck_assert_uint_eq(d, 2 + n % 7);
			XtDestroyWidget(w);
		}
	for (i = 0; i < N_GADGET_CLASSES; i++)
		XtDestroyWidget(parent[i]);
	XSync(XtDisplay(top), False);
	while (XtAppPending(app))
		XtAppProcessEvent(app, XtIMAll);
#ifdef HAVE_LSAN
	ck_assert_msg(!leaks_found(), "gadget Get/SetValues leaked");
#endif
}
END_TEST

/*
 * A shell's extension object stays on its stack across Get and
 * SetValues, which push and pop records of their own above it.
 */
START_TEST(shell_ext_data)
{
	XmWidgetExtData ext;
	unsigned char response = 0;

	ext = _XmGetWidgetExtData(top, XmSHELL_EXTENSION);
	ck_assert_ptr_nonnull(ext);
	ck_assert_ptr_nonnull(ext->widget);
	ck_assert_ptr_eq(((XmExtObject)ext->widget)->ext.logicalParent, top);

	XtVaSetValues(top, XmNdeleteResponse, XmUNMAP, NULL);
	XtVaGetValues(top, XmNdeleteResponse, &response, NULL);
	ck_assert_uint_eq(response, XmUNMAP);
	XtVaSetValues(top, XmNdeleteResponse, XmDO_NOTHING, NULL);
	XtVaGetValues(top, XmNdeleteResponse, &response, NULL);
	ck_assert_uint_eq(response, XmDO_NOTHING);
	ck_assert_ptr_eq(_XmGetWidgetExtData(top, XmSHELL_EXTENSION), ext);
}
END_TEST

/* Write a 2x2 bitmap file NAME in the current directory, its path to PATH. */
static void write_xbm(const char *name, char *path, size_t size)
{
	char dir[1024];
	FILE *fp;

	ck_assert_ptr_nonnull(getcwd(dir, sizeof dir));
	ck_assert_int_lt(snprintf(path, size, "%s/%s", dir, name), (int)size);
	ck_assert_ptr_nonnull(fp = fopen(path, "w"));
	fputs("#define t_width 2\n#define t_height 2\n"
	      "static unsigned char t_bits[] = {\n   0x01, 0x02};\n", fp);
	fclose(fp);
}

#define N_TRIES 64

/*
 * IconG.c: an IconGadget whose icon pixmap comes from the converter
 * fetches the mask that goes with it, and records in an XContext keyed
 * by the gadget's address that it must free that mask.  Destroy freed
 * the mask but left the record, so a later IconGadget at the same
 * address took a mask that it was given for its own and released it
 * when it was destroyed.  Look for a gadget at the address of the first
 * (malloc usually hands the block straight back; under ASan it does not,
 * and then there is nothing to check).
 */
START_TEST(icon_gadget_mask_owner)
{
	char icon[1100], icon_mask[1100];
	Widget rc, w[N_TRIES];
	Pixmap mask = XmUNSPECIFIED_PIXMAP, shared;
	uintptr_t first;
	int i, n, reused = 0;

	write_xbm("check_gadgets_icon.xbm", icon, sizeof icon);
	write_xbm("check_gadgets_icon_m.xbm", icon_mask, sizeof icon_mask);
	rc = XmCreateRowColumn(top, "rc", NULL, 0);
	w[0] = XtVaCreateWidget("icon", xmIconGadgetClass, rc,
				XtVaTypedArg, XmNlargeIconPixmap, XmRString,
				icon, (int)strlen(icon) + 1, NULL);
	XtVaGetValues(w[0], XmNlargeIconMask, &mask, NULL);
	unlink(icon);
	first = (uintptr_t)w[0];
	XtDestroyWidget(w[0]);
	ck_assert_msg(mask != XmUNSPECIFIED_PIXMAP, "no mask was fetched");

	/* A mask from the pixmap cache, which the test holds a reference to. */
	shared = XmGetPixmapByDepth(XtScreen(top), icon_mask, 1, 0, 1);
	unlink(icon_mask);
	ck_assert(shared != XmUNSPECIFIED_PIXMAP);
	for (n = 0; n < N_TRIES && !reused; n++) {
		w[n] = XtVaCreateWidget("icon", xmIconGadgetClass, rc,
					XmNlargeIconMask, shared, NULL);
		reused = (uintptr_t)w[n] == first;
	}
	for (i = 0; i < n; i++)
		XtDestroyWidget(w[i]);

	/* The gadgets did not own it, so the test's reference is still there. */
	if (reused)
		ck_assert_msg(XmDestroyPixmap(XtScreen(top), shared),
			      "an IconGadget released a mask it did not own");
	else
		XmDestroyPixmap(XtScreen(top), shared);
}
END_TEST

/*
 * IconG.c SetValues: replacing a mask that the gadget fetched for itself
 * releases that mask, and the gadget does not own the one it is given.
 * SetValues looked for the ownership record of the scratch copy of the
 * gadget that Xt passes as the current widget instead of the gadget's,
 * so it never released the fetched mask, and Destroy then released the
 * caller's mask in its place.
 */
START_TEST(icon_gadget_mask_set_values)
{
	char icon[1100], icon_mask[1100], other[1100];
	Widget rc, w;
	Pixmap fetched = XmUNSPECIFIED_PIXMAP, shared, got = None;

	write_xbm("check_gadgets_sv.xbm", icon, sizeof icon);
	write_xbm("check_gadgets_sv_m.xbm", icon_mask, sizeof icon_mask);
	write_xbm("check_gadgets_sv_other.xbm", other, sizeof other);
	rc = XmCreateRowColumn(top, "rc", NULL, 0);
	w = XtVaCreateWidget("icon", xmIconGadgetClass, rc,
			     XtVaTypedArg, XmNlargeIconPixmap, XmRString,
			     icon, (int)strlen(icon) + 1, NULL);
	XtVaGetValues(w, XmNlargeIconMask, &fetched, NULL);
	unlink(icon);
	unlink(icon_mask);
	ck_assert_msg(fetched != XmUNSPECIFIED_PIXMAP, "no mask was fetched");

	/* A mask from the pixmap cache, which the test holds a reference to. */
	shared = XmGetPixmapByDepth(XtScreen(top), other, 1, 0, 1);
	unlink(other);
	ck_assert(shared != XmUNSPECIFIED_PIXMAP);
	ck_assert(shared != fetched);
	XtVaSetValues(w, XmNlargeIconMask, shared, NULL);
	XtVaGetValues(w, XmNlargeIconMask, &got, NULL);
	ck_assert(got == shared);

	/* The fetched mask was released (and the cache forgot it). */
	ck_assert_msg(!XmDestroyPixmap(XtScreen(top), fetched),
		      "SetValues did not release the mask the gadget owned");

	XtDestroyWidget(w);
	ck_assert_msg(XmDestroyPixmap(XtScreen(top), shared),
		      "an IconGadget released a mask it was given");
}
END_TEST

void gadgets_suite(SRunner *runner)
{
	Suite *s = suite_create("Gadgets");
	TCase *t;

	t = tcase_create("Extension data");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, ext_data_stack);
	tcase_add_test(t, ext_data_many_stacks);
	tcase_add_test(t, shell_ext_data);
	suite_add_tcase(s, t);

	t = tcase_create("Get and SetValues");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_loop_test(t, gadget_values, 0, N_GADGET_CLASSES);
	tcase_add_test(t, gadget_life_cycle);
	tcase_add_test(t, icon_gadget_mask_owner);
	tcase_add_test(t, icon_gadget_mask_set_values);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
