/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Widget smoke test.  For every widget class declared in the public Xm
 * headers (widget_classes.h is generated from them at build time, see
 * widget_classes.cmake), create an instance, realize it, get every resource
 * and set it back, cycle the enumerated and Boolean resources through
 * their values, and destroy it, without X errors.  Each class runs in
 * its own process; run it under ASan, UBSan and LSan to catch memory
 * errors and leaks.
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/AccTextT.h>
#include <Xm/BulletinB.h>
#include <Xm/Command.h>
#include <Xm/Container.h>
#include <Xm/FontS.h>
#include <Xm/IconG.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/RepType.h>
#include <Xm/Scale.h>
#include <Xm/SelectioB.h>
#include <Xm/SlideC.h>
#include <Xm/SpinB.h>
#include <Xm/TextF.h>
#include <Xm/TraitP.h>
#include <check.h>

#include "leak.h"
#include "suites.h"
#include "widget_classes.h"

#define N_CLASSES (sizeof widget_classes / sizeof widget_classes[0])

/*
 * Classes that applications do not create themselves: abstract base
 * classes, per-display and per-screen objects, extension objects, and
 * the internals of drag and drop.
 */
static const char *const not_created[] = {
	"xmPrimitiveWidgetClass",
	"xmManagerWidgetClass",
	"xmGadgetClass",
	"xmDisplayClass",
	"xmScreenClass",
	"xmDragContextClass",
	"xmDragOverShellWidgetClass",
	"xmDropSiteManagerObjectClass",
	"xmDropTransferObjectClass",
	/* needs an Xprint server */
	"xmPrintShellWidgetClass",
	/* XmContainer's header row, created by it with XmNcontainerID */
	"xmIconHeaderClass",
};

/*
 * Classes that fail because of library bugs.  They are run by
 * Xm.Widgets.xfail, which passes while they still fail.  Those that
 * only leak are known bad only where LeakSanitizer is there to see it.
 */
static const struct {
	const char *name;
	int leak_only;
} known_bad[] = {
	/*
	 * GrabShell.c, Destroy: does not release the top and bottom shadow
	 * GCs (DropDown pops up its list in a GrabShell).
	 */
	{ "xmGrabShellWidgetClass", 1 },
	{ "xmDropDownWidgetClass", 1 },
	{ "xmCombinationBox2WidgetClass", 1 },
	/*
	 * Label.c and LabelG.c, SetValues: a new foreground, background or
	 * font releases the normal and insensitive GCs but not the shadow
	 * GC before SetNormalGC() allocates all three again; the color
	 * window of the ColorSelector changes its background.
	 */
	{ "xmColorSelectorWidgetClass", 1 },
	/*
	 * FontS.c, SetValues: passes every argument on to all descendants,
	 * so setting its own resources, even to their current values,
	 * reconfigures its children, until a later change loops forever in
	 * the XmButtonBox geometry manager (see
	 * font_selector_orientation_hang).
	 */
	{ "xmFontSelectorWidgetClass", 0 },
};

static int is_known_bad(const char *name)
{
	size_t i;

	for (i = 0; i < sizeof known_bad / sizeof known_bad[0]; i++) {
		if (strcmp(known_bad[i].name, name))
			continue;
#ifndef HAVE_LSAN
		if (known_bad[i].leak_only)
			return 0;
#endif
		return 1;
	}
	return 0;
}

static int x_errors;
static int xt_warnings;
static const char *current_class;
static int verbose;	/* MOTIF_TEST_VERBOSE: trace each resource */

static void trace(const char *what, XtResource *r)
{
	if (verbose)
		fprintf(stderr, "#   %s %s (%s)\n", what, r->resource_name,
			r->resource_type);
}

static int x_error_handler(Display *dpy, XErrorEvent *ev)
{
	char text[256];

	XGetErrorText(dpy, ev->error_code, text, sizeof text);
	fprintf(stderr, "# %s: X error: %s (request %d.%d, resource 0x%lx)\n",
		current_class, text, ev->request_code, ev->minor_code,
		ev->resourceid);
	x_errors++;
	return 0;
}

static void xt_warning_handler(String name, String type, String klass,
			       String defaultp, String *params,
			       Cardinal *num_params)
{
	char buf[1024];
	Cardinal n = num_params ? *num_params : 0;
	String p[10] = { 0 };
	Cardinal i;

	(void)type;
	(void)klass;
	for (i = 0; i < n && i < 10; i++)
		p[i] = params[i];
	snprintf(buf, sizeof buf, defaultp ? defaultp : name,
		 p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9]);
	fprintf(stderr, "# %s: Xt warning: %s\n", current_class, buf);
	xt_warnings++;
}

static int in_list(const char *name, const char *const *list, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		if (list[i] && !strcmp(list[i], name))
			return 1;
	return 0;
}

static int class_is(WidgetClass wc, WidgetClass super)
{
	for (; wc; wc = wc->core_class.superclass)
		if (wc == super)
			return 1;
	return 0;
}

/* Process events until the queue stays empty */
static void pump(Widget w)
{
	Display *dpy = XtDisplay(w);
	int idle = 0;

	XSync(dpy, False);
	while (idle < 3) {
		if (XtAppPending(app)) {
			XtAppProcessEvent(app, XtIMAll);
			idle = 0;
		} else {
			XSync(dpy, False);
			idle++;
		}
	}
}

/* XtGetValues writes resource_size bytes; widen them to an XtArgVal */
static XtArgVal widen(const void *buf, Cardinal size)
{
	switch (size) {
	case 1: return (XtArgVal)*(const unsigned char *)buf;
	case 2: return (XtArgVal)*(const unsigned short *)buf;
	case 4: return (XtArgVal)*(const unsigned int *)buf;
	default: return *(const XtArgVal *)buf;
	}
}

static XtArgVal get_value(Widget w, XtResource *r)
{
	union { XtArgVal v; char c[2 * sizeof(XtArgVal)]; } buf;
	Arg arg;

	memset(&buf, 0, sizeof buf);
	XtSetArg(arg, r->resource_name, &buf);
	XtGetValues(w, &arg, 1);
	return widen(&buf, r->resource_size);
}

static void set_value(Widget w, XtResource *r, XtArgVal v)
{
	Arg arg;

	XtSetArg(arg, r->resource_name, v);
	XtSetValues(w, &arg, 1);
}

/*
 * Resources XtGetValues returns a copy of, which the caller frees.
 * Most XmString resources are synthetic and copied; string tables are
 * not.  Text, TextField and DataField copy their value.
 */
static int value_is_copy(Widget w, XtResource *r)
{
	if (!strcmp(r->resource_type, XmRXmString))
		return 1;
	if ((!strcmp(r->resource_name, XmNvalue) &&
	     !strcmp(r->resource_type, XmRString)) ||
	    (!strcmp(r->resource_name, XmNvalueWcs) &&
	     !strcmp(r->resource_type, XmRValueWcs)))
		return 1;
	/* Label and LabelG build it on the fly */
	if (!strcmp(r->resource_name, XmNmnemonicCharSet))
		return 1;
	(void)w;
	return 0;
}

static void free_copy(XtResource *r, XtArgVal v)
{
	if (!v)
		return;
	if (!strcmp(r->resource_type, XmRXmString))
		XmStringFree((XmString)v);
	else
		XtFree((char *)v);
}

/* Resources that must not be set, whatever the value */
static int skip_resource(XtResource *r)
{
	return !strcmp(r->resource_type, XtRCallback) ||
	       !strcmp(r->resource_type, XtRWidgetList) ||
	       !strcmp(r->resource_name, XtNchildren) ||
	       !strcmp(r->resource_name, XtNnumChildren) ||
	       r->resource_size == 0 ||
	       r->resource_size > sizeof(XtArgVal);
}

/*
 * Resources that can only be set at creation; changing them later is
 * refused with a warning, so they are not cycled.
 */
static const char *const create_only[] = {
	XmNlayoutDirection,
	XmNstringDirection,
	XmNdialogType,
	XmNdialogStyle,
	XmNrowColumnType,
	XmNcomboBoxType,
	XmNscrollingPolicy,
	XmNvisualPolicy,
	XmNlistSizePolicy,
	XmNpositionType,
	XmNpositionMode,
};

/* Get every resource and set it back unchanged */
static void get_set_all(Widget w, XtResourceList res, Cardinal n)
{
	Cardinal i;

	for (i = 0; i < n; i++) {
		XtResource *r = &res[i];
		XtArgVal v;

		if (skip_resource(r))
			continue;
		trace("get/set", r);
		v = get_value(w, r);
		set_value(w, r, v);
		if (value_is_copy(w, r))
			free_copy(r, v);
	}
}

/*
 * XmRepTypeGetRecord() allocates the record's name, value names and
 * values separately from the record (RepType.c, CopyRecord), so a
 * single XtFree() as its man page suggests leaks them.
 */
static void free_rep_record(XmRepTypeEntry rec)
{
	unsigned int i;

	for (i = 0; i < rec->num_values; i++)
		XtFree(rec->value_names[i]);
	XtFree((char *)rec->value_names);
	XtFree((char *)rec->values);
	XtFree(rec->rep_type_name);
	XtFree((char *)rec);
}

/*
 * Set enumerated resources to each of their legal values, and Boolean
 * ones to the opposite value, then restore them.
 */
static void cycle_values(Widget w, XtResourceList res, Cardinal n)
{
	Cardinal i, j;

	for (i = 0; i < n; i++) {
		XtResource *r = &res[i];
		XmRepTypeId id;
		XmRepTypeEntry rec;
		XtArgVal orig;

		if (skip_resource(r) ||
		    in_list(r->resource_name, create_only,
			    sizeof create_only / sizeof create_only[0]))
			continue;
		if (!strcmp(r->resource_type, XmRBoolean) ||
		    !strcmp(r->resource_type, XtRBoolean)) {
			trace("toggle", r);
			orig = get_value(w, r);
			set_value(w, r, !orig);
			pump(w);
			set_value(w, r, orig);
			continue;
		}
		id = XmRepTypeGetId(r->resource_type);
		if (id == XmREP_TYPE_INVALID)
			continue;
		rec = XmRepTypeGetRecord(id);
		if (!rec)
			continue;
		trace("cycle", r);
		orig = get_value(w, r);
		for (j = 0; j < rec->num_values; j++) {
			XtArgVal v = rec->values ? rec->values[j] : j;

			set_value(w, r, v);
			pump(w);
		}
		set_value(w, r, orig);
		free_rep_record(rec);
	}
}

static void exercise(Widget w)
{
	XtResourceList res;
	Cardinal n;

	XtGetResourceList(XtClass(w), &res, &n);
	get_set_all(w, res, n);
	pump(w);
	cycle_values(w, res, n);
	pump(w);
	XtFree((char *)res);

	if (XtParent(w) && XtIsConstraint(XtParent(w))) {
		XtGetConstraintResourceList(XtClass(XtParent(w)), &res, &n);
		if (res) {
			get_set_all(w, res, n);
			pump(w);
			XtFree((char *)res);
		}
	}
}

static void smoke(const struct widget_class *wc)
{
	WidgetClass cls = *wc->cls;
	Widget top, parent, w;
	Arg args[4];
	Cardinal n;

	current_class = wc->name;
	fprintf(stderr, "# %s\n", wc->name);
	top = init_xt("WidgetSmoke");
	XSetErrorHandler(x_error_handler);
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	x_errors = xt_warnings = 0;

	n = 0;
	XtSetArg(args[n], XmNwidth, 400); n++;
	XtSetArg(args[n], XmNheight, 300); n++;
	parent = XmCreateBulletinBoard(top, "parent", args, n);
	XtManageChild(parent);
	XtRealizeWidget(top);
	pump(top);

	n = 0;
	if (class_is(cls, shellWidgetClass)) {
		XtSetArg(args[n], XmNwidth, 100); n++;
		XtSetArg(args[n], XmNheight, 100); n++;
		w = XtCreatePopupShell("widget", cls, top, args, n);
		XtRealizeWidget(w);
	} else if (!strcmp(wc->name, "xmSlideContextWidgetClass")) {
		/* XmSlideContext moves an existing widget, and destroys
		 * itself when it is done; keep it from finishing. */
		XtSetArg(args[n], XmNslideWidget, parent); n++;
		XtSetArg(args[n], XmNslideInterval, 1000000); n++;
		w = XtCreateWidget("widget", cls, parent, args, n);
	} else if (!class_is(cls, rectObjClass)) {
		/* XmDragIcon and other plain objects */
		w = XtCreateWidget("widget", cls, parent, args, n);
	} else {
		w = XtCreateManagedWidget("widget", cls, parent, args, n);
	}
	ck_assert_msg(w != NULL, "%s: creation failed", wc->name);
	pump(top);

	exercise(w);

	XtDestroyWidget(w);
	pump(top);
	ck_assert_msg(x_errors == 0, "%s: %d X errors", wc->name, x_errors);
	uninit_xt();
}

/* Indices into widget_classes of the classes each test case runs */
static int normal_idx[N_CLASSES], n_normal;
static int xfail_idx[N_CLASSES], n_xfail;

START_TEST(widget_smoke)
{
	smoke(&widget_classes[normal_idx[_i]]);
}
END_TEST

START_TEST(widget_smoke_known_bad)
{
	smoke(&widget_classes[xfail_idx[_i]]);
}
END_TEST

/*
 * Known library bug: XmFontSelector's SetValues passes every argument
 * on to all its descendants (FontS.c, _XmSetValuesOnChildren), so
 * setting XmNorientation, even to its current value, turns its button
 * boxes around; changing a label afterwards then never returns from
 * the XmButtonBox geometry manager.
 */
START_TEST(font_selector_orientation_hang)
{
	Widget top, fs;
	unsigned char orientation;
	XmString s;

	current_class = "xmFontSelectorWidgetClass";
	top = init_xt("WidgetSmoke");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	fs = XtCreateManagedWidget("fs", xmFontSelectorWidgetClass, top,
				   NULL, 0);
	XtRealizeWidget(top);
	XtVaGetValues(fs, XmNorientation, &orientation, NULL);
	XtVaSetValues(fs, XmNorientation, orientation, NULL);
	s = XmStringCreateLocalized("Bold");
	XtVaSetValues(fs, XmNboldString, s, NULL);
	XmStringFree(s);
	uninit_xt();
}
END_TEST

/*
 * Known library bug: XmFontSelector's get_values_hook returns
 * XmNbothString for XmNboldString and the other way round (FontS.c,
 * GetValuesHook).
 */
START_TEST(font_selector_bold_string)
{
	Widget top, fs;
	XmString set, got = NULL;

	current_class = "xmFontSelectorWidgetClass";
	top = init_xt("WidgetSmoke");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	set = XmStringCreateLocalized("Bold");
	fs = XtVaCreateManagedWidget("fs", xmFontSelectorWidgetClass, top,
				     XmNboldString, set, NULL);
	XtVaGetValues(fs, XmNboldString, &got, NULL);
	ck_assert_msg(XmStringCompare(set, got),
		      "XmNboldString does not read back");
	XmStringFree(got);
	XmStringFree(set);
	uninit_xt();
}
END_TEST

/*
 * Known library bug: XmSlideContext created without XmNslideWidget
 * warns, but its destroy method then calls XtRemoveCallback() on the
 * NULL slide widget (SlideC.c, destroy).
 */
START_TEST(slide_context_without_widget)
{
	Widget top, w;

	current_class = "xmSlideContextWidgetClass";
	top = init_xt("WidgetSmoke");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	w = XtCreateWidget("slide", xmSlideContextWidgetClass, top, NULL, 0);
	ck_assert_ptr_nonnull(w);
	XtDestroyWidget(w);
	uninit_xt();
}
END_TEST

/*
 * Scale.c, GetValueString: XmNdecimalPoints is not bounded, and the
 * value string was formatted into fixed stack buffers.
 */
START_TEST(scale_many_decimal_points)
{
	Widget top, w;

	current_class = "xmScaleWidgetClass";
	top = init_xt("WidgetRegress");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	w = XtVaCreateManagedWidget("scale", xmScaleWidgetClass, top,
				    XmNshowValue, True,
				    XmNdecimalPoints, 400,
				    XmNvalue, 42, NULL);
	XtRealizeWidget(top);
	pump(top);
	XmScaleSetValue(w, 43);
	pump(top);
	uninit_xt();
}
END_TEST

/*
 * SpinB.c, NumToString: abs(INT_MIN) counted no digits, so the value
 * string of a numeric spin box was printed past its buffer.
 */
START_TEST(spin_box_int_min)
{
	Widget top, sb, tf;

	current_class = "xmSpinBoxWidgetClass";
	top = init_xt("WidgetRegress");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	sb = XtCreateManagedWidget("spin", xmSpinBoxWidgetClass, top, NULL, 0);
	tf = XtVaCreateManagedWidget("text", xmTextFieldWidgetClass, sb,
				     XmNspinBoxChildType, XmNUMERIC,
				     XmNminimumValue, INT_MIN,
				     XmNmaximumValue, INT_MIN + 10,
				     XmNposition, INT_MIN, NULL);
	XtRealizeWidget(top);
	pump(top);
	XtVaSetValues(tf, XmNposition, INT_MIN + 1, NULL);
	pump(top);
	uninit_xt();
}
END_TEST

/*
 * IconG.c: a 0 in XmNdetailOrder (which is 1-based) indexed the detail
 * table at (Cardinal)-1, and the detail loop read past the reordered
 * table when XmNdetailOrderCount was less than XmNdetailCount.
 */
START_TEST(icon_gadget_detail_order)
{
	static Cardinal order[] = { 0 };
	Widget top, c;
	XmString details[3];
	int i;

	current_class = "xmIconGadgetClass";
	top = init_xt("WidgetRegress");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	c = XtVaCreateManagedWidget("container", xmContainerWidgetClass, top,
				    XmNlayoutType, XmDETAIL,
				    XmNdetailOrder, order,
				    XmNdetailOrderCount, 1,
				    XmNwidth, 300, XmNheight, 200, NULL);
	for (i = 0; i < 3; i++)
		details[i] = XmStringCreateLocalized("detail");
	XtVaCreateManagedWidget("icon", xmIconGadgetClass, c,
				XmNdetail, details,
				XmNdetailCount, 3, NULL);
	for (i = 0; i < 3; i++)
		XmStringFree(details[i]);
	XtRealizeWidget(top);
	pump(top);
	uninit_xt();
}
END_TEST

/*
 * Label.c, the XmQTaccessTextual setValue method: an XmFORMAT_WCS value
 * was converted into a buffer without room for the terminating NUL.
 * The method frees the value it is given.
 */
START_TEST(label_set_wide_value)
{
	static const wchar_t *values[] = { L"", L"wide" };
	XmAccessTextualTrait trait;
	Widget top, w;
	size_t i;

	current_class = "xmLabelWidgetClass";
	top = init_xt("WidgetRegress");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	w = XtCreateManagedWidget("label", xmLabelWidgetClass, top, NULL, 0);
	trait = (XmAccessTextualTrait)XmeTraitGet((XtPointer)xmLabelWidgetClass,
						   XmQTaccessTextual);
	ck_assert_ptr_nonnull(trait);
	for (i = 0; i < sizeof values / sizeof values[0]; i++) {
		size_t n = wcslen(values[i]) + 1;
		wchar_t *v = (wchar_t *)XtMalloc(n * sizeof(wchar_t));

		wmemcpy(v, values[i], n);
		trait->setValue(w, (XtPointer)v, XmFORMAT_WCS);
	}
	XtRealizeWidget(top);
	pump(top);
	uninit_xt();
}
END_TEST

/*
 * The dialog convenience functions copied the caller's argument list
 * with memcpy() even when it was NULL with a count of 0, which is
 * undefined behaviour (UBSan: "null pointer passed as argument 2").
 */
START_TEST(dialogs_without_args)
{
	Widget top, w;

	current_class = "dialogs";
	top = init_xt("WidgetRegress");
	XtAppSetWarningMsgHandler(app, xt_warning_handler);
	w = XmCreateMessageDialog(top, "message", NULL, 0);
	XtManageChild(w);
	w = XmCreatePromptDialog(top, "prompt", NULL, 0);
	XtManageChild(w);
	w = XmCreateSelectionDialog(top, "selection", NULL, 0);
	XtManageChild(w);
	w = XmCreateCommandDialog(top, "command", NULL, 0);
	XtManageChild(w);
	w = XmCreateBulletinBoardDialog(top, "bulletin", NULL, 0);
	XtManageChild(w);
	XtRealizeWidget(top);
	pump(top);
	uninit_xt();
}
END_TEST

/* The generated table must have found the headers */
START_TEST(class_table)
{
	ck_assert_int_ge((int)N_CLASSES, 60);
}
END_TEST

void widgets_suite(SRunner *runner)
{
	Suite *s = suite_create("Widgets");
	const char *only;
	TCase *t;
	size_t i;

	/* MOTIF_TEST_WIDGET_CLASS=xmFooWidgetClass runs just that class */
	only = getenv("MOTIF_TEST_WIDGET_CLASS");
	verbose = getenv("MOTIF_TEST_VERBOSE") != NULL;
	n_normal = n_xfail = 0;
	for (i = 0; i < N_CLASSES; i++) {
		const char *name = widget_classes[i].name;

		if (only && *only && strcmp(only, name))
			continue;
		if (in_list(name, not_created,
			    sizeof not_created / sizeof not_created[0]))
			continue;
		if (is_known_bad(name))
			xfail_idx[n_xfail++] = (int)i;
		else
			normal_idx[n_normal++] = (int)i;
	}

	t = tcase_create("Class table");
	tcase_add_test(t, class_table);
	suite_add_tcase(s, t);

	t = tcase_create("Smoke");
	tcase_add_loop_test(t, widget_smoke, 0, n_normal);
	tcase_set_timeout(t, 120);
	suite_add_tcase(s, t);

	if (!only || !*only) {
		t = tcase_create("Regressions");
		tcase_add_test(t, scale_many_decimal_points);
		tcase_add_test(t, spin_box_int_min);
		tcase_add_test(t, icon_gadget_detail_order);
		tcase_add_test(t, label_set_wide_value);
		tcase_add_test(t, dialogs_without_args);
		suite_add_tcase(s, t);
	}

	/* Some of them hang: give up early */
	t = tcase_create("Known bugs");
	if (n_xfail)
		tcase_add_loop_test(t, widget_smoke_known_bad, 0, n_xfail);
	if (!only || !*only) {
		tcase_add_test(t, slide_context_without_widget);
		tcase_add_test(t, font_selector_bold_string);
	}
	tcase_set_timeout(t, 30);
	tcase_set_tags(t, "xfail");
	suite_add_tcase(s, t);

	/* It hangs: give up early */
	if (!only || !*only) {
		t = tcase_create("Known bugs, hangs");
		tcase_add_test(t, font_selector_orientation_hang);
		tcase_set_timeout(t, 15);
		tcase_set_tags(t, "xfail");
		suite_add_tcase(s, t);
	}

	srunner_add_suite(runner, s);
}
