/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * The string parameters that the API takes as pointers to const (see
 * doc/abi-policy.md).  This file is compiled with -Wwrite-strings and
 * -Werror=discarded-qualifiers, so it does not build if one of them
 * loses its const again; and every string it passes is a literal or a
 * const array, which lives in read-only memory, so a function that
 * writes to one crashes the test case.
 */
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/XmosP.h>
#include <Xm/AtomMgr.h>
#include <Xm/BulletinB.h>
#include <Xm/CutPaste.h>
#include <Xm/Ext.h>
#include <Xm/IconFile.h>
#include <Xm/IconFileP.h>
#include <Xm/Label.h>
#include <Xm/Picture.h>
#include <Xm/PushB.h>
#include <Xm/RepType.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <check.h>

#include "suites.h"

static Widget top, bb;

static void setup(void)
{
	top = init_xt("check_ConstApi");
	bb = XmVaCreateManagedBulletinBoard(top, "bb", NULL);
	XtRealizeWidget(top);
}

static void teardown(void)
{
	uninit_xt();
}

/* The text of the first segment of s, or NULL */
static char *text_of(XmString s)
{
	return (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       NULL, 0, XmOUTPUT_ALL);
}

static void assert_text(XmString s, const char *expect)
{
	char *text = text_of(s);

	ck_assert_ptr_nonnull(text);
	ck_assert_str_eq(text, expect);
	XtFree(text);
}

static char warning_seen[256];

static void warning_handler(String msg)
{
	strncpy(warning_seen, msg, sizeof warning_seen - 1);
	warning_seen[sizeof warning_seen - 1] = '\0';
}

START_TEST(const_names)
{
	Display *dpy = XtDisplay(top);
	static const unsigned char values[] = {3, 7};
	static char one[] = "const_one", two[] = "const_two";
	String names[] = {one, two};
	XmRepTypeId id;
	XmPicture picture;
	XtPointer method = NULL;
	XtEnum parse_error;
	int unit = -1;
	char lowered[8];
	XmString s;

	ck_assert_uint_eq(XmInternAtom(dpy, "_MOTIF_CONST_TEST", False),
			  XInternAtom(dpy, "_MOTIF_CONST_TEST", False));

	ck_assert_int_eq(XmCompareISOLatin1("Const\xc4", "cONST\xe4"), 0);
	ck_assert_int_ne(XmCompareISOLatin1("const", "consu"), 0);
	XmCopyISOLatin1Lowered(lowered, "CoNsT\xc4");
	ck_assert_str_eq(lowered, "const\xe4");

	/* test_str must already be in lower case */
	ck_assert(XmeNamesAreEqual("XmCONST", "const"));
	ck_assert(!XmeNamesAreEqual("const", "consts"));
	ck_assert_int_eq(XmeParseUnits("cm", &unit), XmPARSE_UNITS_OK);
	ck_assert_int_eq(unit, XmCENTIMETERS);
	ck_assert_int_eq(XmConvertStringToUnits(XtScreen(top), "2in",
						XmHORIZONTAL,
						Xm1000TH_INCHES,
						&parse_error), 2000);
	ck_assert(!parse_error);

	id = XmRepTypeRegister("ConstTestType", names, values, 2);
	ck_assert_int_ne(id, XmREP_TYPE_INVALID);
	ck_assert_int_eq(XmRepTypeGetId("ConstTestType"), id);
	ck_assert(XmRepTypeValidValue(id, 7, NULL));
	ck_assert(!XmRepTypeValidValue(id, 1, NULL));

	picture = XmParsePicture("###-##");
	ck_assert_ptr_nonnull(picture);
	XmPictureDelete(picture);

	ck_assert_int_ne(XmOSGetMethod(top, "CharDirection", &method, NULL),
			 XmOS_METHOD_NULL);
	ck_assert_ptr_nonnull(method);

	s = XmeGetLocalizedString(NULL, top, "labelString", "localized");
	assert_text(s, "localized");
	XmStringFree(s);

	XtAppSetWarningHandler(app, warning_handler);
	warning_seen[0] = '\0';
	XmeWarning(NULL, "const warning");
	ck_assert_str_eq(warning_seen, "const warning");
}
END_TEST

START_TEST(const_xmstrings)
{
	static const char tag[] = "ConstTag";
	static const char rendition[] = "ConstRendition";
	static const wchar_t wide[] = L"wide";
	XmString s, t, u;
	XtPointer *texts;
	XmStringTable table;
	unsigned char *stream;
	unsigned char *copy;
	unsigned int len;
	char *ct, *text = NULL;

	s = XmStringCreate("const text", tag);
	ck_assert(XmStringGetLtoR(s, tag, &text));
	ck_assert_str_eq(text, "const text");
	XtFree(text);
	/* The tag is cached: the string keeps its own copy */
	text = (char *)XmStringUnparse(s, tag, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       NULL, 0, XmOUTPUT_ALL);
	ck_assert_str_eq(text, "const text");
	XtFree(text);
	XmStringFree(s);

	s = XmStringLtoRCreate("first\nsecond", XmFONTLIST_DEFAULT_TAG);
	ck_assert_int_eq(XmStringLineCount(s), 2);
	XmStringFree(s);
	s = XmStringCreateLtoR("one\ntwo", tag);
	ck_assert_int_eq(XmStringLineCount(s), 2);
	XmStringFree(s);
	s = XmStringSegmentCreate("segment", tag, XmSTRING_DIRECTION_L_TO_R,
				  True);
	ck_assert_int_eq(XmStringLineCount(s), 2);
	XmStringFree(s);

	s = XmStringGenerate("generated", XmFONTLIST_DEFAULT_TAG,
			     XmCHARSET_TEXT, rendition);
	assert_text(s, "generated");
	t = XmStringPutRendition(s, rendition);
	assert_text(t, "generated");
	XmStringFree(t);
	XmStringFree(s);
	s = XmStringGenerate(wide, NULL, XmWIDECHAR_TEXT, NULL);
	ck_assert_ptr_nonnull(s);
	ck_assert(!XmStringEmpty(s));
	XmStringFree(s);

	s = XmStringComponentCreate(XmSTRING_COMPONENT_TEXT, 9, "component");
	assert_text(s, "component");
	t = XmStringComponentCreate(XmSTRING_COMPONENT_TAG, strlen(tag), tag);
	ck_assert_ptr_nonnull(t);
	XmStringFree(t);

	/* Byte streams, read back from a const copy */
	len = XmCvtXmStringToByteStream(s, &stream);
	ck_assert_uint_gt(len, 0);
	copy = (unsigned char *)XtMalloc(len);
	memcpy(copy, stream, len);
	XtFree((char *)stream);
	{
		const unsigned char *ro = copy;

		ck_assert_uint_eq(XmStringByteStreamLength(ro), len);
		u = XmCvtByteStreamToXmString(ro);
	}
	XtFree((char *)copy);
	ck_assert(XmStringCompare(s, u));
	XmStringFree(u);

	/* Compound text */
	ct = XmCvtXmStringToCT(s);
	ck_assert_ptr_nonnull(ct);
	{
		const char *ro = ct;

		u = XmCvtCTToXmString(ro);
	}
	XtFree(ct);
	assert_text(u, "component");
	XmStringFree(u);
	u = XmCvtCTToXmString("compound");
	assert_text(u, "compound");
	XmStringFree(u);

	table = (XmStringTable)XtMalloc(sizeof(XmString));
	table[0] = s;
	texts = XmStringTableUnparse(table, 1, NULL, XmCHARSET_TEXT,
				     XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
	ck_assert_str_eq((char *)texts[0], "component");
	XtFree((char *)texts[0]);
	XtFree((char *)texts);
	XtFree((char *)table);
	XmStringFree(s);

	ck_assert_ptr_null(XmRegisterSegmentEncoding("ConstSegTag",
						     "ISO8859-15"));
	text = XmMapSegmentEncoding("ConstSegTag");
	ck_assert_str_eq(text, "ISO8859-15");
	XtFree(text);
	text = XmRegisterSegmentEncoding("ConstSegTag", NULL);
	ck_assert_str_eq(text, "ISO8859-15");
	XtFree(text);
	ck_assert_ptr_null(XmMapSegmentEncoding("ConstSegTag"));
}
END_TEST

START_TEST(const_render_tables)
{
	Display *dpy = XtDisplay(top);
	static const char tag[] = "ConstRendition";
	XmRendition rend, got;
	XmRenderTable table, back;
	XmFontListEntry entry;
	XmFontList fl;
	XFontStruct *font;
	XmTab tab;
	char *prop, *decimal = NULL;
	unsigned int len;
	Arg args[2];
	Cardinal n = 0;

	XtSetArg(args[n], XmNfontName, "fixed"), n++;
	XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT), n++;
	rend = XmRenditionCreate(top, tag, args, n);
	ck_assert_ptr_nonnull(rend);
	table = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_REPLACE);
	XmRenditionFree(rend);
	got = XmRenderTableGetRendition(table, tag);
	ck_assert_ptr_nonnull(got);
	XmRenditionFree(got);
	ck_assert_ptr_null(XmRenderTableGetRendition(table, "NoSuchTag"));

	len = XmRenderTableCvtToProp(top, table, &prop);
	ck_assert_uint_gt(len, 0);
	{
		const char *ro = prop;

		back = XmRenderTableCvtFromProp(top, ro, len);
	}
	XtFree(prop);
	ck_assert_ptr_nonnull(back);
	got = XmRenderTableGetRendition(back, tag);
	ck_assert_ptr_nonnull(got);
	XmRenditionFree(got);
	XmRenderTableFree(back);
	XmRenderTableFree(table);

	entry = XmFontListEntryLoad(dpy, "fixed", XmFONT_IS_FONT, "ConstLoad");
	ck_assert_ptr_nonnull(entry);
	XmFontListEntryFree(&entry);
	font = XLoadQueryFont(dpy, "fixed");
	ck_assert_ptr_nonnull(font);
	entry = XmFontListEntryCreate("ConstEntry", XmFONT_IS_FONT, font);
	ck_assert_ptr_nonnull(entry);
	XmFontListEntryFree(&entry);
	entry = XmFontListEntryCreate_r("ConstEntry", XmFONT_IS_FONT, font, top);
	ck_assert_ptr_nonnull(entry);
	XmFontListEntryFree(&entry);
	fl = XmFontListCreate(font, "ConstList");
	ck_assert_ptr_nonnull(fl);
	fl = XmFontListAdd(fl, font, "ConstAdd");
	ck_assert_ptr_nonnull(fl);
	XmFontListFree(fl);
	fl = XmFontListCreate_r(font, "ConstList", top);
	ck_assert_ptr_nonnull(fl);
	XmFontListFree(fl);
	fl = XmStringCreateFontList(font, "ConstList");
	ck_assert_ptr_nonnull(fl);
	XmFontListFree(fl);
	fl = XmStringCreateFontList_r(font, "ConstList", top);
	ck_assert_ptr_nonnull(fl);
	XmFontListFree(fl);

	tab = XmTabCreate(1.5f, XmINCHES, XmABSOLUTE, XmALIGNMENT_BEGINNING,
			  ",");
	ck_assert_ptr_nonnull(tab);
	XmTabGetValues(tab, NULL, NULL, NULL, &decimal);
	ck_assert_str_eq(decimal, ",");
	XmTabFree(tab);
	XFreeFont(dpy, font);
}
END_TEST

START_TEST(const_images)
{
	Screen *screen = XtScreen(top);
	Display *dpy = XtDisplay(top);
	static char bits[8 * 2];
	static const char name[] = "const_test_image";
	Pixel fg = BlackPixelOfScreen(screen), bg = WhitePixelOfScreen(screen);
	XImage *image;
	Pixmap pix;
	String file;

	image = XCreateImage(dpy, DefaultVisualOfScreen(screen), 1, XYBitmap,
			     0, bits, 16, 8, 8, 2);
	ck_assert_ptr_nonnull(image);
	ck_assert(XmInstallImage(image, name));
	ck_assert(!XmInstallImage(image, name));

	pix = XmGetPixmap(screen, name, fg, bg);
	ck_assert(pix != XmUNSPECIFIED_PIXMAP);
	ck_assert(XmDestroyPixmap(screen, pix));
	pix = XmGetPixmapByDepth(screen, name, 1, 0, 1);
	ck_assert(pix != XmUNSPECIFIED_PIXMAP);
	ck_assert(XmDestroyPixmap(screen, pix));
	pix = XmGetSizedPixmap(top, name, fg, bg, DefaultDepthOfScreen(screen),
			       32, 16);
	ck_assert(pix != XmUNSPECIFIED_PIXMAP);
	ck_assert(XmDestroyPixmap(screen, pix));
	/* No "const_test_image_m" mask was installed */
	ck_assert(XmeGetMask(screen, name) == XmUNSPECIFIED_PIXMAP);

	/* An installed image is its own file name */
	file = XmGetIconFileName(screen, NULL, name, NULL,
				 XmUNSPECIFIED_ICON_SIZE);
	ck_assert_ptr_nonnull(file);
	ck_assert_str_eq(file, name);
	XtFree(file);
	file = XmGetIconFileName(screen, "const_no_such_icon", NULL, "",
				 XmLARGE_ICON_SIZE);
	ck_assert_ptr_null(file);
	XmeFlushIconFileCache("/no/such/dir");

	ck_assert(XmUninstallImage(image));
	image->data = NULL;
	XDestroyImage(image);
}
END_TEST

static void checked(Widget w, XtPointer client, XtPointer call)
{
	(void)w;
	(void)client;
	(void)call;
}

START_TEST(const_widget_names)
{
	XmString item = XmStringCreateLocalized("item");
	Cardinal n = 0;
	Widget w, dialog;

	w = XmVaCreateManagedLabel(bb, "const_label", XmNx, 5, NULL);
	ck_assert_str_eq(XtName(w), "const_label");
	w = XmVaCreatePushButton(bb, "const_push", NULL);
	ck_assert_str_eq(XtName(w), "const_push");
	ck_assert(!XtIsManaged(w));
	w = XmVaCreateManagedRowColumn(bb, "const_rc", NULL);
	ck_assert_str_eq(XtName(w), "const_rc");
	w = XmVaCreateSimpleCheckBox(bb, "const_check", checked,
				     XmVaCHECKBUTTON, item, (KeySym)0, NULL,
				     NULL, NULL);
	ck_assert_str_eq(XtName(w), "const_check");
	XtVaGetValues(w, XmNnumChildren, &n, NULL);
	ck_assert_uint_eq(n, 1);
	w = XmVaCreateSimpleRadioBox(bb, "const_radio", 0, checked,
				     XmVaRADIOBUTTON, item, (KeySym)0, NULL,
				     NULL, NULL);
	ck_assert_str_eq(XtName(w), "const_radio");
	XmStringFree(item);

	dialog = XmeCreateClassDialog(xmBulletinBoardWidgetClass, top,
				      "const_dialog", NULL, 0);
	ck_assert_str_eq(XtName(dialog), "const_dialog");
	ck_assert_str_eq(XtName(XtParent(dialog)), "const_dialog_popup");
}
END_TEST

START_TEST(const_text_search)
{
	static const wchar_t wneedle[] = L"needle";
	static char content[] = "hay needle hay needle";
	XmTextPosition pos = -1;
	Widget text;

	text = XmVaCreateManagedText(bb, "text", NULL);
	XmTextSetString(text, content);
	ck_assert(XmTextFindString(text, 0, "needle", XmTEXT_FORWARD, &pos));
	ck_assert_int_eq(pos, 4);
	ck_assert(XmTextFindString(text, 21, "needle", XmTEXT_BACKWARD, &pos));
	ck_assert_int_eq(pos, 15);
	ck_assert(!XmTextFindString(text, 0, "straw", XmTEXT_FORWARD, &pos));
	ck_assert(XmTextFindStringWcs(text, 5, wneedle, XmTEXT_FORWARD, &pos));
	ck_assert_int_eq(pos, 15);
}
END_TEST

START_TEST(const_clipboard)
{
	static const char format[] = "CONST_TEST_FORMAT";
	static const char data[] = "clipboard data";
	Display *dpy = XtDisplay(top);
	Window win = XtWindow(top);
	XmString label = XmStringCreateLocalized("const");
	XmClipboardPendingList list = NULL;
	unsigned long length = 0, count = 0, got = 0;
	long item_id = 0, data_id = 0, private_id = 0;
	char buf[64];

	ck_assert_int_eq(XmClipboardRegisterFormat(dpy, format, 8),
			 XmClipboardSuccess);
	/* Registering it again with the same length is fine */
	ck_assert_int_eq(XmClipboardRegisterFormat(dpy, format, 8),
			 XmClipboardSuccess);

	ck_assert_int_eq(XmClipboardStartCopy(dpy, win, label, CurrentTime,
					      NULL, NULL, &item_id),
			 XmClipboardSuccess);
	ck_assert_int_eq(XmClipboardCopy(dpy, win, item_id, format,
					 data, sizeof data, 0,
					 &data_id),
			 XmClipboardSuccess);
	ck_assert_int_eq(XmClipboardEndCopy(dpy, win, item_id),
			 XmClipboardSuccess);
	XmStringFree(label);

	ck_assert_int_eq(XmClipboardInquireLength(dpy, win, format, &length),
			 XmClipboardSuccess);
	ck_assert_uint_eq(length, sizeof data);
	ck_assert_int_eq(XmClipboardInquirePendingItems(dpy, win, format,
							&list, &count),
			 XmClipboardSuccess);
	XtFree((char *)list);
	ck_assert_int_eq(XmClipboardRetrieve(dpy, win, format, buf, sizeof buf,
					     &got, &private_id),
			 XmClipboardSuccess);
	ck_assert_uint_eq(got, sizeof data);
	ck_assert_str_eq(buf, data);
}
END_TEST

void const_api_suite(SRunner *runner)
{
	Suite *s = suite_create("ConstApi");
	TCase *t;

	t = tcase_create("ConstApi");
	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, const_names);
	tcase_add_test(t, const_xmstrings);
	tcase_add_test(t, const_render_tables);
	tcase_add_test(t, const_images);
	tcase_add_test(t, const_widget_names);
	tcase_add_test(t, const_text_search);
	tcase_add_test(t, const_clipboard);
	tcase_set_timeout(t, 60);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
