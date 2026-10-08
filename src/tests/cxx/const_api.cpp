/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * A C++ program built against the Motif headers.  C++ does not convert
 * a string literal to char *, so the calls below compile only because
 * these functions take const strings (see doc/abi-policy.md); the file
 * is compiled with -Werror=write-strings to keep it that way.
 *
 * Exits 0 on success, 1 on failure and 77 when there is no display.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <Xm/XmAll.h>
#include <Xm/XmP.h>
#include <Xm/Ext.h>
#include <Xm/IconFile.h>
#include <Mrm/MrmPublic.h>

static int failures;

static void check(bool ok, const char *what)
{
	if (!ok) {
		std::fprintf(stderr, "cxx_const_api: %s failed\n", what);
		failures++;
	}
}

static bool text_is(XmString s, const char *expect)
{
	char *text = static_cast<char *>(XmStringUnparse(
		s, nullptr, XmCHARSET_TEXT, XmCHARSET_TEXT, nullptr, 0,
		XmOUTPUT_ALL));
	bool ok = text && !std::strcmp(text, expect);

	XtFree(text);
	return ok;
}

int main(int argc, char **argv)
{
	const char *display = std::getenv("DISPLAY");

	if (!display || !*display) {
		std::printf("cxx_const_api: SKIP: DISPLAY is not set\n");
		return 77;
	}

	XtAppContext app;
	Widget top = XtVaAppInitialize(&app, "CxxConstApi", nullptr, 0, &argc,
				       argv, nullptr, nullptr);
	Display *dpy = XtDisplay(top);
	Screen *screen = XtScreen(top);

	/* Widget names */
	Widget form = XmVaCreateManagedForm(top, "form", nullptr);
	Widget label = XmVaCreateManagedLabel(form, "label", nullptr);
	Widget text = XmVaCreateManagedText(form, "text", XmNvalue,
					    "find the needle", nullptr);
	check(!std::strcmp(XtName(label), "label"), "XmVaCreateManagedLabel");
	XtRealizeWidget(top);

	/* Compound strings and tags */
	XmString s = XmStringCreate("hello", "ISO8859-1");
	check(text_is(s, "hello"), "XmStringCreate");
	XmString g = XmStringGenerate("generated", XmFONTLIST_DEFAULT_TAG,
				      XmCHARSET_TEXT, "bold");
	check(text_is(g, "generated"), "XmStringGenerate");
	XmString r = XmStringPutRendition(s, "bold");
	check(text_is(r, "hello"), "XmStringPutRendition");
	XmString c = XmStringComponentCreate(XmSTRING_COMPONENT_TEXT, 4, "text");
	check(text_is(c, "text"), "XmStringComponentCreate");
	XmString ct = XmCvtCTToXmString("compound");
	check(text_is(ct, "compound"), "XmCvtCTToXmString");
	XtVaSetValues(label, XmNlabelString, s, nullptr);
	XmStringFree(ct);
	XmStringFree(c);
	XmStringFree(r);
	XmStringFree(g);
	XmStringFree(s);

	Arg args[2];
	XtSetArg(args[0], XmNfontName, "fixed");
	XtSetArg(args[1], XmNfontType, XmFONT_IS_FONT);
	XmRendition rend = XmRenditionCreate(top, "bold", args, 2);
	XmRenderTable table =
		XmRenderTableAddRenditions(nullptr, &rend, 1, XmMERGE_REPLACE);
	XmRenditionFree(rend);
	rend = XmRenderTableGetRendition(table, "bold");
	check(rend != nullptr, "XmRenderTableGetRendition");
	XmRenditionFree(rend);
	XmRenderTableFree(table);
	XmFontListEntry entry =
		XmFontListEntryLoad(dpy, "fixed", XmFONT_IS_FONT, "fixed");
	check(entry != nullptr, "XmFontListEntryLoad");
	XmFontListEntryFree(&entry);

	/* Names */
	check(XmInternAtom(dpy, "CXX_CONST_API", False) ==
		      XInternAtom(dpy, "CXX_CONST_API", False),
	      "XmInternAtom");
	check(XmCompareISOLatin1("Motif", "MOTIF") == 0, "XmCompareISOLatin1");
	check(XmeNamesAreEqual("XmMotif", "motif"), "XmeNamesAreEqual");
	XtEnum error;
	check(XmConvertStringToUnits(screen, "1in", XmHORIZONTAL,
				     Xm1000TH_INCHES, &error) == 1000,
	      "XmConvertStringToUnits");
	XmTextPosition pos;
	check(XmTextFindString(text, 0, "needle", XmTEXT_FORWARD, &pos) &&
		      pos == 9,
	      "XmTextFindString");
	check(XmTextFindStringWcs(text, 0, L"needle", XmTEXT_FORWARD, &pos) &&
		      pos == 9,
	      "XmTextFindStringWcs");
	check(XmGetPixmap(screen, "cxx_no_such_image", 0, 1) ==
		      XmUNSPECIFIED_PIXMAP,
	      "XmGetPixmap");
	String file = XmGetIconFileName(screen, nullptr, "cxx_no_such_icon",
					nullptr, XmUNSPECIFIED_ICON_SIZE);
	XtFree(file);
	check(XmClipboardRegisterFormat(dpy, "CXX_CONST_FORMAT", 8) ==
		      XmClipboardSuccess,
	      "XmClipboardRegisterFormat");

	/* Mrm: a buffer that is not a UID file is refused, not written */
	static const unsigned char junk[512] = {0};
	MrmHierarchy h = nullptr;
	MrmInitialize();
	check(MrmOpenHierarchyFromBufferWithSize(junk, sizeof junk, &h) !=
		      MrmSUCCESS,
	      "MrmOpenHierarchyFromBufferWithSize");
	if (h) {
		Widget w = nullptr;
		MrmType cls;
		MrmFetchWidget(h, "main", top, &w, &cls);
		MrmCloseHierarchy(h);
	}

	XtDestroyWidget(top);
	XtDestroyApplicationContext(app);
	return failures ? 1 : 0;
}
