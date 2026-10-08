/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Right-to-left layout: every test builds the same widgets twice, under
 * a shell with XmNlayoutDirection XmLEFT_TO_RIGHT and under one with
 * XmRIGHT_TO_LEFT, and checks that the second is the mirror image of
 * the first: Label and PushButton text, pixmap and accelerator, Form
 * attachments, RowColumn rows and columns, ScrolledWindow scroll bars,
 * and a scrolled XmText.
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/XmP.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/LabelP.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <check.h>

#include "suites.h"

/* The two shells: [0] left to right, [1] right to left */
static Widget shells[2];

static const unsigned char directions[2] = { XmLEFT_TO_RIGHT, XmRIGHT_TO_LEFT };

static void setup(void)
{
	shells[0] = init_xt("check_Rtl");
	XtVaSetValues(shells[0], XmNlayoutDirection, XmLEFT_TO_RIGHT, NULL);
	shells[1] = XtVaAppCreateShell("rtl", "Check_Rtl", applicationShellWidgetClass,
				       XtDisplay(shells[0]), XmNlayoutDirection,
				       XmRIGHT_TO_LEFT, NULL);
}

static void teardown(void)
{
	XtDestroyWidget(shells[1]);
	uninit_xt();
}

static void settle(void)
{
	Display *dpy = XtDisplay(shells[0]);
	int i;

	for (i = 0; i < 3; i++) {
		XSync(dpy, False);
		while (XtAppPending(app) & XtIMXEvent)
			XtAppProcessEvent(app, XtIMXEvent);
	}
}

static void realize(void)
{
	XtRealizeWidget(shells[0]);
	XtRealizeWidget(shells[1]);
	settle();
}

/* The x the mirror image of a box at x, width wide, has in a parent */
static int mirror(int parent_width, int x, int width)
{
	return parent_width - x - width;
}

/* w[1] is where w[0] would be in a mirror, and as large */
static void assert_mirrored(Widget w[2])
{
	ck_assert_int_eq(XtWidth(w[1]), XtWidth(w[0]));
	ck_assert_int_eq(XtHeight(w[1]), XtHeight(w[0]));
	ck_assert_int_eq(XtWidth(XtParent(w[1])), XtWidth(XtParent(w[0])));
	ck_assert_int_eq(XtY(w[1]), XtY(w[0]));
	ck_assert_msg(XtX(w[1]) == mirror(XtWidth(XtParent(w[0])), XtX(w[0]),
					  XtWidth(w[0]) + 2 * XtBorderWidth(w[0])),
		      "%s: x %d in RTL, %d in LTR, parent %d wide", XtName(w[0]),
		      XtX(w[1]), XtX(w[0]), XtWidth(XtParent(w[0])));
}

/* The layout direction is inherited from the shell */
START_TEST(inherited)
{
	unsigned char dir;
	int i;

	for (i = 0; i < 2; i++) {
		Widget bb = XmCreateBulletinBoard(shells[i], "bb", NULL, 0);
		Widget b = XmCreatePushButton(bb, "b", NULL, 0);

		XtVaGetValues(b, XmNlayoutDirection, &dir, NULL);
		ck_assert_int_eq(dir, directions[i]);
	}
}
END_TEST

/* The text of a wide label starts at its leading edge */
START_TEST(label_alignment)
{
	static const unsigned char alignments[] = { XmALIGNMENT_BEGINNING,
						    XmALIGNMENT_CENTER, XmALIGNMENT_END };
	Widget l[2][3];
	int i, a;

	for (i = 0; i < 2; i++) {
		Widget bb = XmCreateBulletinBoard(shells[i], "bb", NULL, 0);

		XtManageChild(bb);
		for (a = 0; a < 3; a++) {
			l[i][a] = XmCreateLabel(bb, "label text", NULL, 0);
			XtVaSetValues(l[i][a], XmNalignment, alignments[a],
				      XmNrecomputeSize, False, XmNwidth, 200, XmNy, 30 * a,
				      NULL);
			XtManageChild(l[i][a]);
		}
	}
	realize();
	for (a = 0; a < 3; a++) {
		XmLabelPart *ltr = &((XmLabelWidget)l[0][a])->label;
		XmLabelPart *rtl = &((XmLabelWidget)l[1][a])->label;

		ck_assert_int_eq(XtWidth(l[0][a]), 200);
		ck_assert_int_eq(rtl->TextRect.width, ltr->TextRect.width);
		ck_assert_int_eq(rtl->TextRect.y, ltr->TextRect.y);
		ck_assert_int_eq(rtl->TextRect.x,
				 mirror(200, ltr->TextRect.x, ltr->TextRect.width));
	}
	/* BEGINNING is the left edge in LTR, the right one in RTL */
	ck_assert_int_lt(((XmLabelWidget)l[0][0])->label.TextRect.x, 50);
	ck_assert_int_gt(((XmLabelWidget)l[1][0])->label.TextRect.x, 100);
}
END_TEST

/* XmPIXMAP_AND_STRING: the pixmap is on the leading side of the text */
START_TEST(label_pixmap_and_string)
{
	Widget l[2];
	Pixmap pix;
	int i;

	pix = XCreatePixmap(XtDisplay(shells[0]), DefaultRootWindow(XtDisplay(shells[0])),
			    16, 16, DefaultDepthOfScreen(XtScreen(shells[0])));
	for (i = 0; i < 2; i++) {
		Widget bb = XmCreateBulletinBoard(shells[i], "bb", NULL, 0);

		XtManageChild(bb);
		l[i] = XmCreateLabel(bb, "text", NULL, 0);
		XtVaSetValues(l[i], XmNlabelType, XmPIXMAP_AND_STRING, XmNlabelPixmap,
			      pix, XmNpixmapPlacement, XmPIXMAP_LEFT, NULL);
		XtManageChild(l[i]);
	}
	realize();
	{
		XmLabelPart *ltr = &((XmLabelWidget)l[0])->label;
		XmLabelPart *rtl = &((XmLabelWidget)l[1])->label;

		ck_assert_int_eq(XtWidth(l[1]), XtWidth(l[0]));
		/* Pixmap and string rectangles are relative to TextRect */
		ck_assert_int_lt(ltr->PixmapRect.x, ltr->StringRect.x);
		ck_assert_int_gt(rtl->PixmapRect.x, rtl->StringRect.x);
		ck_assert_int_eq(rtl->PixmapRect.x,
				 mirror(ltr->TextRect.width, ltr->PixmapRect.x,
					ltr->PixmapRect.width));
		ck_assert_int_eq(rtl->StringRect.x,
				 mirror(ltr->TextRect.width, ltr->StringRect.x,
					ltr->StringRect.width));
	}
	XFreePixmap(XtDisplay(shells[0]), pix);
}
END_TEST

/* A menu PushButton: label at the leading edge, accelerator at the other */
START_TEST(pushbutton_accelerator)
{
	Widget b[2], menu[2];
	int i;

	for (i = 0; i < 2; i++) {
		Widget bar = XmCreateMenuBar(shells[i], "bar", NULL, 0);
		XmString acc = XmStringCreateLocalized("Ctrl+Q");

		menu[i] = XmCreatePulldownMenu(bar, "menu", NULL, 0);
		XtManageChild(bar);
		b[i] = XmCreatePushButton(menu[i], "Quit", NULL, 0);
		XtVaSetValues(b[i], XmNacceleratorText, acc, XmNaccelerator, "Ctrl<Key>q",
			      NULL);
		XtManageChild(b[i]);
		XmStringFree(acc);
	}
	realize();
	/* The menu shells are popups: realized on their own */
	for (i = 0; i < 2; i++)
		XtRealizeWidget(XtParent(menu[i]));
	settle();
	{
		XmLabelPart *ltr = &((XmLabelWidget)b[0])->label;
		XmLabelPart *rtl = &((XmLabelWidget)b[1])->label;
		int w = XtWidth(b[0]);

		ck_assert_int_eq(XtWidth(b[1]), w);
		ck_assert_int_gt(ltr->acc_TextRect.x, ltr->TextRect.x);
		ck_assert_int_lt(rtl->acc_TextRect.x, rtl->TextRect.x);
		ck_assert_int_eq(rtl->TextRect.x,
				 mirror(w, ltr->TextRect.x, ltr->TextRect.width));
		ck_assert_int_eq(rtl->acc_TextRect.x,
				 mirror(w, ltr->acc_TextRect.x, ltr->acc_TextRect.width));
	}
}
END_TEST

/* A plain PushButton is a Label: mirrored like one */
START_TEST(pushbutton)
{
	Widget b[2];
	int i;

	for (i = 0; i < 2; i++) {
		Widget bb = XmCreateBulletinBoard(shells[i], "bb", NULL, 0);

		XtManageChild(bb);
		b[i] = XmCreatePushButton(bb, "push", NULL, 0);
		XtVaSetValues(b[i], XmNrecomputeSize, False, XmNwidth, 150, XmNalignment,
			      XmALIGNMENT_BEGINNING, XmNmarginLeft, 4, XmNmarginRight, 12,
			      NULL);
		XtManageChild(b[i]);
	}
	realize();
	{
		XmLabelPart *ltr = &((XmLabelWidget)b[0])->label;
		XmLabelPart *rtl = &((XmLabelWidget)b[1])->label;

		/* Margins are given left and right, not leading and trailing */
		ck_assert_int_eq(rtl->margin_left, 4);
		ck_assert_int_eq(rtl->margin_right, 12);
		ck_assert_int_eq(rtl->TextRect.x + rtl->TextRect.width,
				 150 - ((XmPrimitiveWidget)b[1])->primitive.highlight_thickness -
				     ((XmPrimitiveWidget)b[1])->primitive.shadow_thickness -
				     rtl->margin_width - rtl->margin_right);
		ck_assert_int_eq(ltr->TextRect.x,
				 ((XmPrimitiveWidget)b[0])->primitive.highlight_thickness +
				     ((XmPrimitiveWidget)b[0])->primitive.shadow_thickness +
				     ltr->margin_width + ltr->margin_left);
	}
}
END_TEST

static Widget box(Widget parent, const char *name, int width, int height)
{
	Widget w = XmCreateDrawingArea(parent, (char *)name, NULL, 0);

	XtVaSetValues(w, XmNwidth, width, XmNheight, height, XmNborderWidth, 0,
		      XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
	return w;
}

/*
 * Form: XmNleftAttachment is the leading edge, so every kind of
 * attachment comes out mirrored.
 */
START_TEST(form_attachments)
{
	Widget k[2][5];
	int i, j;

	for (i = 0; i < 2; i++) {
		Widget form = XmCreateForm(shells[i], "form", NULL, 0);

		XtVaSetValues(form, XmNwidth, 300, XmNheight, 120, XmNresizePolicy,
			      XmRESIZE_NONE, NULL);
		k[i][0] = box(form, "form_left", 50, 20);
		XtVaSetValues(k[i][0], XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
			      XmNtopAttachment, XmATTACH_FORM, NULL);
		k[i][1] = box(form, "widget_left", 60, 20);
		XtVaSetValues(k[i][1], XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget,
			      k[i][0], XmNleftOffset, 5, XmNtopAttachment, XmATTACH_FORM,
			      NULL);
		k[i][2] = box(form, "positions", 10, 20);
		XtVaSetValues(k[i][2], XmNleftAttachment, XmATTACH_POSITION,
			      XmNleftPosition, 20, XmNrightAttachment, XmATTACH_POSITION,
			      XmNrightPosition, 70, XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, k[i][0], NULL);
		k[i][3] = box(form, "form_right", 40, 20);
		XtVaSetValues(k[i][3], XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 7,
			      XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, k[i][2],
			      XmNtopOffset, 3, NULL);
		k[i][4] = box(form, "opposite", 30, 20);
		XtVaSetValues(k[i][4], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			      XmNleftWidget, k[i][1], XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, k[i][3], NULL);
		for (j = 0; j < 5; j++)
			XtManageChild(k[i][j]);
		XtManageChild(form);
	}
	realize();
	/* The left to right layout is the one asked for */
	ck_assert_int_eq(XtX(k[0][0]), 10);
	ck_assert_int_eq(XtX(k[0][1]), 65);
	ck_assert_int_eq(XtX(k[0][2]), 60);
	ck_assert_int_eq(XtWidth(k[0][2]), 150);
	ck_assert_int_eq(XtX(k[0][3]), 300 - 7 - 40);
	ck_assert_int_eq(XtX(k[0][4]), 65);
	for (j = 0; j < 5; j++) {
		Widget pair[2] = { k[0][j], k[1][j] };

		assert_mirrored(pair);
	}
}
END_TEST

/* RowColumn: horizontal rows, and columns, run from the leading edge */
START_TEST(rowcolumn)
{
	Widget kids[2][2][5];
	int i, j, layout;

	for (i = 0; i < 2; i++) {
		Widget bb = XmCreateBulletinBoard(shells[i], "bb", NULL, 0);
		Widget row = XmCreateRowColumn(bb, "row", NULL, 0);
		Widget cols = XmCreateRowColumn(bb, "columns", NULL, 0);

		XtVaSetValues(bb, XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
		XtVaSetValues(row, XmNorientation, XmHORIZONTAL, XmNpacking, XmPACK_TIGHT,
			      NULL);
		XtVaSetValues(cols, XmNorientation, XmVERTICAL, XmNpacking, XmPACK_COLUMN,
			      XmNnumColumns, 2, XmNy, 100, NULL);
		for (j = 0; j < 5; j++) {
			char name[16];

			snprintf(name, sizeof name, "r%d", j);
			kids[i][0][j] = box(row, name, 20 + 10 * j, 15);
			snprintf(name, sizeof name, "c%d", j);
			kids[i][1][j] = box(cols, name, 20 + 10 * j, 15);
			XtManageChild(kids[i][0][j]);
			XtManageChild(kids[i][1][j]);
		}
		XtManageChild(row);
		XtManageChild(cols);
		XtManageChild(bb);
	}
	realize();
	/* In LTR the row goes left to right */
	for (j = 1; j < 5; j++)
		ck_assert_int_gt(XtX(kids[0][0][j]), XtX(kids[0][0][j - 1]));
	for (layout = 0; layout < 2; layout++)
		for (j = 0; j < 5; j++) {
			Widget pair[2] = { kids[0][layout][j], kids[1][layout][j] };

			assert_mirrored(pair);
		}
}
END_TEST

/* ScrolledWindow: the vertical scroll bar goes to the leading side */
START_TEST(scrolledwindow)
{
	Widget sw[2], vsb[2], hsb[2], area[2];
	unsigned char placement;
	int i;

	for (i = 0; i < 2; i++) {
		Arg args[1];

		/* The scrolling policy can only be set at creation */
		XtSetArg(args[0], XmNscrollingPolicy, XmAUTOMATIC);
		sw[i] = XmCreateScrolledWindow(shells[i], "sw", args, 1);
		XtVaSetValues(sw[i], XmNwidth, 200, XmNheight, 150, NULL);
		area[i] = box(sw[i], "area", 400, 400);
		XtManageChild(area[i]);
		XtManageChild(sw[i]);
	}
	realize();
	XtVaGetValues(sw[0], XmNscrollBarPlacement, &placement, NULL);
	ck_assert_int_eq(placement, XmBOTTOM_RIGHT);
	XtVaGetValues(sw[1], XmNscrollBarPlacement, &placement, NULL);
	ck_assert_int_eq(placement, XmBOTTOM_LEFT);
	for (i = 0; i < 2; i++)
		XtVaGetValues(sw[i], XmNverticalScrollBar, &vsb[i], XmNhorizontalScrollBar,
			      &hsb[i], NULL);
	ck_assert(XtIsManaged(vsb[0]) && XtIsManaged(vsb[1]));
	ck_assert_int_gt(XtX(vsb[0]), XtWidth(sw[0]) / 2);
	ck_assert_int_lt(XtX(vsb[1]), XtWidth(sw[1]) / 2);
	assert_mirrored(vsb);
	assert_mirrored(hsb);
}
END_TEST

/*
 * A scrolled XmText is mirrored as a whole: its scroll bar is on the
 * leading side and the text beside it.  The text itself is not laid out
 * right to left: XmText has no bidirectional layout (nor had Motif
 * 2.3), so position 0 is at the left in both.
 */
START_TEST(scrolled_text)
{
	Widget text[2], sw[2], vsb[2];
	Position x0[2], y0[2], x1[2], y1[2];
	int i;

	for (i = 0; i < 2; i++) {
		Arg args[3];

		XtSetArg(args[0], XmNeditMode, XmMULTI_LINE_EDIT);
		XtSetArg(args[1], XmNrows, 5);
		XtSetArg(args[2], XmNcolumns, 20);
		text[i] = XmCreateScrolledText(shells[i], "text", args, 3);
		XmTextSetString(text[i], "one\ntwo\nthree\nfour\nfive\nsix\nseven\n");
		XtManageChild(text[i]);
		sw[i] = XtParent(text[i]);
		XtVaGetValues(sw[i], XmNverticalScrollBar, &vsb[i], NULL);
	}
	realize();
	ck_assert(XtIsManaged(vsb[1]));
	assert_mirrored(vsb);
	assert_mirrored(text);
	ck_assert_int_gt(XtX(text[1]), XtX(vsb[1]));
	for (i = 0; i < 2; i++) {
		ck_assert(XmTextPosToXY(text[i], 0, &x0[i], &y0[i]));
		ck_assert(XmTextPosToXY(text[i], 3, &x1[i], &y1[i]));
		ck_assert_int_lt(x0[i], x1[i]);
	}
	ck_assert_int_eq(x0[1], x0[0]);
	ck_assert_int_eq(x1[1], x1[0]);
}
END_TEST

/*
 * Vertical text is asked for with XmTOP_TO_BOTTOM_RIGHT_TO_LEFT: lines
 * run down, from the right.  XmRIGHT_TO_LEFT must not select it (it did:
 * XmDirectionMatch() takes XmRIGHT_TO_LEFT for any precedence).
 */
START_TEST(vertical_text)
{
	static const unsigned char dirs[] = { XmRIGHT_TO_LEFT, XmTOP_TO_BOTTOM_RIGHT_TO_LEFT };
	Position x0, y0, x1, y1, x2, y2;
	Dimension width[2];
	Widget text[2];
	int i;

	for (i = 0; i < 2; i++) {
		Widget bb = XmCreateBulletinBoard(shells[i], "bb", NULL, 0);
		Arg args[4];

		XtSetArg(args[0], XmNeditMode, XmMULTI_LINE_EDIT);
		XtSetArg(args[1], XmNrows, 5);
		XtSetArg(args[2], XmNcolumns, 20);
		XtSetArg(args[3], XmNlayoutDirection, dirs[i]);
		text[i] = XmCreateText(bb, "text", args, 4);
		XmTextSetString(text[i], "one\ntwo");
		XtManageChild(text[i]);
		XtManageChild(bb);
	}
	realize();
	for (i = 0; i < 2; i++)
		XtVaGetValues(text[i], XmNwidth, &width[i], NULL);

	/* XmRIGHT_TO_LEFT: horizontal, 20 columns wide like left to right */
	ck_assert(XmTextPosToXY(text[0], 0, &x0, &y0));
	ck_assert(XmTextPosToXY(text[0], 3, &x1, &y1));
	ck_assert(XmTextPosToXY(text[0], 4, &x2, &y2));
	ck_assert_int_gt(x1, x0);
	ck_assert_int_eq(y1, y0);
	ck_assert_int_gt(y2, y0);
	ck_assert_int_eq(x2, x0);

	/* Vertical: a line goes down, the next one is to its left */
	ck_assert(XmTextPosToXY(text[1], 0, &x0, &y0));
	ck_assert(XmTextPosToXY(text[1], 3, &x1, &y1));
	ck_assert(XmTextPosToXY(text[1], 4, &x2, &y2));
	ck_assert_int_eq(x1, x0);
	ck_assert_int_gt(y1, y0);
	ck_assert_int_lt(x2, x0);
	ck_assert_int_eq(y2, y0);
	ck_assert_int_gt(x0, width[1] / 2);
}
END_TEST

/* The same for a scrolled XmList */
START_TEST(scrolled_list)
{
	Widget list[2], vsb[2];
	XmString items[8];
	int i;

	for (i = 0; i < 8; i++) {
		char name[16];

		snprintf(name, sizeof name, "item %d", i);
		items[i] = XmStringCreateLocalized(name);
	}
	for (i = 0; i < 2; i++) {
		list[i] = XmCreateScrolledList(shells[i], "list", NULL, 0);
		XtVaSetValues(list[i], XmNitems, items, XmNitemCount, 8,
			      XmNvisibleItemCount, 4, NULL);
		XtManageChild(list[i]);
		XtVaGetValues(XtParent(list[i]), XmNverticalScrollBar, &vsb[i], NULL);
	}
	realize();
	assert_mirrored(vsb);
	assert_mirrored(list);
	for (i = 0; i < 8; i++)
		XmStringFree(items[i]);
}
END_TEST

void rtl_suite(SRunner *runner)
{
	Suite *s = suite_create("Rtl");
	TCase *t = tcase_create("Right to left");

	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, inherited);
	tcase_add_test(t, label_alignment);
	tcase_add_test(t, label_pixmap_and_string);
	tcase_add_test(t, pushbutton_accelerator);
	tcase_add_test(t, pushbutton);
	tcase_add_test(t, form_attachments);
	tcase_add_test(t, rowcolumn);
	tcase_add_test(t, scrolledwindow);
	tcase_add_test(t, scrolled_text);
	tcase_add_test(t, vertical_text);
	tcase_add_test(t, scrolled_list);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
