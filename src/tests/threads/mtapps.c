/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * mtapps: two threads, each with its own XtAppContext and Display,
 * build, realize, exercise and destroy Motif widget trees at the same
 * time.  Built normally it checks that this works at all; built with
 * WITH_TSAN it is the ThreadSanitizer test for the library's shared
 * (process-wide) state.
 *
 *   mtapps [iterations [threads [uid-file]]]
 *
 * With a uid file (hellomotif.uid), each iteration also opens it on its
 * display and fetches its widget tree with Mrm.
 *
 * Exits 0 on success, 1 on a failure, 77 without a display.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/ComboBox.h>
#include <Xm/Container.h>
#include <Xm/CutPaste.h>
#include <Xm/DragDrop.h>
#include <Xm/DrawingA.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/IconG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/Notebook.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/SelectioB.h>
#include <Xm/Separator.h>
#include <Xm/SpinB.h>
#include <Xm/TabList.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#define MAX_THREADS 8

/*
 * For AddressSanitizer builds.  libX11 looks a quark name up with
 * memcmp(name, stored, length of name) once the hash signatures match,
 * and a colliding stored name can be shorter: memcmp stops at the stored
 * NUL, which differs, but ASan's strict check covers the whole range.
 * The two threads intern names in an order where this happens.
 */
const char *__asan_default_options(void);
const char *__asan_default_options(void)
{
	return "strict_memcmp=0";
}

/*
 * For LeakSanitizer: leaks of Motif that this test reaches and that
 * happen on master as well, with one thread, so have nothing to do with
 * threads.  Remove these when they are fixed.
 * - The drop-down XmComboBox and the option menu leave references to
 *   shared GCs when they are destroyed.  XtCloseDisplay frees Xt's GC
 *   cache records but not the GCs, so the GC structures leak (reported
 *   from whichever widget created the shared GC first).
 * - XmListReplaceItemsPos after XmListSelectPos loses the list's array
 *   of selected items (allocated by UpdateSelection).
 */
const char *__lsan_default_suppressions(void);
const char *__lsan_default_suppressions(void)
{
	return "leak:XtAllocateGC\n"
	       "leak:UpdateSelection\n";
}

struct worker {
	pthread_t thread;
	int id;
	int iterations;
	int failures;
	char msg[256];
};

static int iterations = 3;
static char *uid_file = NULL;

/*
 * The clipboard and the PRIMARY selection are shared by the clients of
 * a server.  The clipboard protocol (a lock property on the root
 * window) is not atomic between clients, and a client that takes a
 * selection loses it when another one takes it right after (Xt then
 * finds the other owner and reports failure): the threads take turns
 * with them, so that the test checks the library and not the
 * protocol.  The rest of the threads' work is not serialized.
 */
static pthread_mutex_t clipboard_turn = PTHREAD_MUTEX_INITIALIZER;

#define CHECK(w, cond)                                                     \
	do {                                                               \
		if (!(cond)) {                                             \
			(w)->failures++;                                   \
			snprintf((w)->msg, sizeof((w)->msg), "%s:%d: %s", \
				 __FILE__, __LINE__, #cond);               \
		}                                                          \
	} while (0)

/* Process the events that are there, and the ones a round trip brings. */
static void drain(XtAppContext app, Display *dpy)
{
	int round;

	for (round = 0; round < 3; round++) {
		XSync(dpy, False);
		while (XtAppPending(app) & (XtIMXEvent | XtIMTimer))
			XtAppProcessEvent(app, XtIMXEvent | XtIMTimer);
	}
}

static void exercise_strings(struct worker *w, Widget label)
{
	XmString a, b, c, d;
	XmStringTable table;
	XmRendition rend[2];
	XmRenderTable rt, copy;
	XmTabList tabs;
	XmTab tab;
	XmParseTable parse;
	XmFontList fl;
	XmFontListEntry entry;
	XmString *strs;
	char *text, *ct;
	Arg args[4];
	int n;

	a = XmStringCreateLocalized("Hello");
	b = XmStringGenerate("bold", NULL, XmCHARSET_TEXT, "B");
	c = XmStringConcat(a, b);
	d = XmStringCopy(c);
	CHECK(w, XmStringCompare(c, d));
	CHECK(w, XmStringLength(d) > 0);
	text = (char *)XmStringUnparse(d, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       NULL, 0, XmOUTPUT_ALL);
	CHECK(w, text && strcmp(text, "Hellobold") == 0);
	XtFree(text);

	/* Compound text both ways (ResEncod). */
	ct = XmCvtXmStringToCT(d);
	CHECK(w, ct != NULL);
	if (ct) {
		XmString back = XmCvtCTToXmString(ct);
		XmStringFree(back);
		XtFree(ct);
	}

	/* Render tables: create, merge, copy, use, free. */
	n = 0;
	XtSetArg(args[n], XmNfontName, "fixed"), n++;
	XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT), n++;
	XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE), n++;
	rend[0] = XmRenditionCreate(label, (XmStringTag) "B", args, n);
	tab = XmTabCreate(1.0, XmINCHES, XmABSOLUTE, XmALIGNMENT_BEGINNING, NULL);
	tabs = XmTabListInsertTabs(NULL, &tab, 1, 0);
	XmTabFree(tab);
	n = 0;
	XtSetArg(args[n], XmNtabList, tabs), n++;
	XtSetArg(args[n], XmNfontName, "fixed"), n++;
	XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT), n++;
	rend[1] = XmRenditionCreate(label, (XmStringTag) XmFONTLIST_DEFAULT_TAG, args, n);
	XmTabListFree(tabs);
	rt = XmRenderTableAddRenditions(NULL, rend, 2, XmMERGE_REPLACE);
	XmRenditionFree(rend[0]);
	XmRenditionFree(rend[1]);
	copy = XmRenderTableCopy(rt, NULL, 0);
	XtVaSetValues(label, XmNrenderTable, copy, XmNlabelString, d, NULL);
	XmRenderTableFree(copy);
	CHECK(w, XmStringWidth(rt, d) > 0);
	rend[0] = XmRenderTableGetRendition(rt, (XmStringTag) "B");
	CHECK(w, rend[0] != NULL);
	XmRenditionFree(rend[0]);
	XmRenderTableFree(rt);

	/* Old font lists still go through the same code. */
	entry = XmFontListEntryLoad(XtDisplay(label), "fixed", XmFONT_IS_FONT, "F");
	fl = XmFontListAppendEntry(NULL, entry);
	XmFontListEntryFree(&entry);
	XmFontListFree(fl);

	/* Parse tables and string tables. */
	parse = (XmParseTable)XtMalloc(sizeof(XmParseMapping));
	n = 0;
	XtSetArg(args[n], XmNincludeStatus, XmINSERT), n++;
	XtSetArg(args[n], XmNsubstitute, NULL), n++;
	XtSetArg(args[n], XmNpattern, "\t"), n++;
	XtSetArg(args[n], XmNpatternType, XmCHARSET_TEXT), n++;
	parse[0] = XmParseMappingCreate(args, n);
	table = XmStringTableParseStringArray(
	    (XtPointer *)(char *[]){"one\ta", "two\tb", "three"}, 3, NULL,
	    XmCHARSET_TEXT, parse, 1, NULL);
	strs = table;
	CHECK(w, strs != NULL);
	if (strs) {
		XmStringFree(strs[0]);
		XmStringFree(strs[1]);
		XmStringFree(strs[2]);
		XtFree((char *)strs);
	}
	XmParseTableFree(parse, 1);

	XmStringFree(a);
	XmStringFree(b);
	XmStringFree(c);
	XmStringFree(d);
}

static void exercise_text(struct worker *w, Widget text, Widget tf)
{
	char buf[64];
	char *s;
	XmTextPosition left, right;

	XmTextSetString(text, "line one\nline two\nline three");
	XmTextInsert(text, 0, "start: ");
	XmTextReplace(text, 0, 5, "START");
	XmTextSetInsertionPosition(text, 3);
	XmTextShowPosition(text, XmTextGetLastPosition(text));
	CHECK(w, XmTextGetSubstring(text, 0, 5, sizeof(buf), buf) == XmCOPY_SUCCEEDED);
	CHECK(w, strncmp(buf, "START", 5) == 0);
	/* The primary selection, then the clipboard (TextSel, Transfer,
	 * CutPaste).  The copy is not checked: taking the CLIPBOARD
	 * selection fails, by the X protocol, when the other client took
	 * it at a later server time.  Paste only when this application
	 * owns CLIPBOARD: a paste from another worker goes unanswered when
	 * that worker closes its display first, and Xt keeps the request
	 * until its selection timeout, after this application context is
	 * gone (a leak of Xt's request records under LeakSanitizer). */
	pthread_mutex_lock(&clipboard_turn);
	XmTextSetSelection(text, 0, 5, CurrentTime);
	CHECK(w, XmTextGetSelectionPosition(text, &left, &right));
	s = XmTextGetSelection(text);
	CHECK(w, s && strcmp(s, "START") == 0);
	XtFree(s);
	(void)XmTextCopy(text, CurrentTime);
	if (XtWindowToWidget(XtDisplay(text),
			     XGetSelectionOwner(XtDisplay(text),
						XInternAtom(XtDisplay(text), "CLIPBOARD", False))))
		(void)XmTextPaste(text);
	XmTextClearSelection(text, CurrentTime);
	pthread_mutex_unlock(&clipboard_turn);
	(void)XmTextFindString(text, 0, "two", XmTEXT_FORWARD, &left);

	XmTextFieldSetString(tf, "field text");
	XmTextFieldInsert(tf, 0, ">");
	s = XmTextFieldGetString(tf);
	CHECK(w, s && strcmp(s, ">field text") == 0);
	XtFree(s);
	pthread_mutex_lock(&clipboard_turn);
	XmTextFieldSetSelection(tf, 1, 6, CurrentTime);
	XmTextFieldClearSelection(tf, CurrentTime);
	pthread_mutex_unlock(&clipboard_turn);
}

static void exercise_list(struct worker *w, Widget list)
{
	XmString items[8];
	char name[32];
	int i, *pos, count;

	for (i = 0; i < 8; i++) {
		snprintf(name, sizeof(name), "item %d", i);
		items[i] = XmStringCreateLocalized(name);
	}
	XmListAddItems(list, items, 8, 0);
	XmListSelectPos(list, 3, False);
	XmListSetBottomPos(list, 8);
	XtVaGetValues(list, XmNselectedPositions, &pos, XmNselectedPositionCount, &count, NULL);
	CHECK(w, count == 1 && pos[0] == 3);
	CHECK(w, XmListItemExists(list, items[5]));
	XmListDeletePos(list, 1);
	XmListReplaceItemsPos(list, &items[0], 1, 1);
	XmListDeselectAllItems(list);
	for (i = 0; i < 8; i++)
		XmStringFree(items[i]);
}

static void exercise_menus(Widget shell, Widget popup, Widget option)
{
	XButtonPressedEvent ev;
	WidgetList kids;
	Cardinal nkids;

	memset(&ev, 0, sizeof(ev));
	ev.type = ButtonPress;
	ev.display = XtDisplay(shell);
	ev.window = XtWindow(shell);
	ev.root = RootWindowOfScreen(XtScreen(shell));
	ev.x_root = 20;
	ev.y_root = 20;
	ev.button = Button3;
	XmMenuPosition(popup, &ev);
	XtManageChild(popup);
	XtUnmanageChild(popup);

	XtVaGetValues(XmOptionButtonGadget(option), XmNsubMenuId, &popup, NULL);
	if (popup) {
		XtVaGetValues(popup, XmNchildren, &kids, XmNnumChildren, &nkids, NULL);
		if (nkids > 1)
			XtVaSetValues(option, XmNmenuHistory, kids[1], NULL);
	}
}

static void exercise_clipboard(struct worker *w, Widget shell)
{
	Display *dpy = XtDisplay(shell);
	Window win = XtWindow(shell);
	XmString label;
	long item = 0;
	char buf[64];
	unsigned long len = 0;
	long id;
	int tries, status;

	/*
	 * Hold the clipboard lock over the whole copy, as a client sharing
	 * the clipboard has to, and take turns with the other thread.
	 */
	pthread_mutex_lock(&clipboard_turn);
	for (tries = 0; tries < 200; tries++) {
		status = XmClipboardLock(dpy, win);
		if (status == XmClipboardSuccess)
			break;
		drain(XtWidgetToApplicationContext(shell), dpy);
	}
	CHECK(w, status == XmClipboardSuccess);
	if (status != XmClipboardSuccess) {
		pthread_mutex_unlock(&clipboard_turn);
		return;
	}
	label = XmStringCreateLocalized("mtapps");
	status = XmClipboardStartCopy(dpy, win, label, CurrentTime, NULL, NULL, &item);
	XmStringFree(label);
	CHECK(w, status == XmClipboardSuccess);
	snprintf(buf, sizeof(buf), "from thread %d", w->id);
	status = XmClipboardCopy(dpy, win, item, "STRING", buf, (long)strlen(buf) + 1, 0, &id);
	CHECK(w, status == XmClipboardSuccess);
	status = XmClipboardEndCopy(dpy, win, item);
	CHECK(w, status == XmClipboardSuccess);
	memset(buf, 0, sizeof(buf));
	status = XmClipboardRetrieve(dpy, win, "STRING", buf, sizeof(buf), &len, &id);
	CHECK(w, status == XmClipboardSuccess);
	CHECK(w, strncmp(buf, "from thread ", 12) == 0);
	XmClipboardUnlock(dpy, win, True);
	pthread_mutex_unlock(&clipboard_turn);
}

static void drop_proc(Widget w, XtPointer client, XtPointer call)
{
	(void)w;
	(void)client;
	(void)call;
}

static void exercise_dnd(struct worker *w, Widget site)
{
	Atom targets[2];
	Atom *got = NULL;
	Cardinal ngot = 0;
	Arg args[4];
	int n;
	Widget dc;

	targets[0] = XInternAtom(XtDisplay(site), "STRING", False);
	targets[1] = XInternAtom(XtDisplay(site), "COMPOUND_TEXT", False);
	n = 0;
	XtSetArg(args[n], XmNimportTargets, targets), n++;
	XtSetArg(args[n], XmNnumImportTargets, 2), n++;
	XtSetArg(args[n], XmNdropProc, drop_proc), n++;
	XtSetArg(args[n], XmNdropSiteOperations, XmDROP_COPY), n++;
	XmDropSiteRegister(site, args, n);
	n = 0;
	XtSetArg(args[n], XmNdropSiteActivity, XmDROP_SITE_ACTIVE), n++;
	XmDropSiteUpdate(site, args, n);
	n = 0;
	XtSetArg(args[n], XmNimportTargets, &got), n++;
	XtSetArg(args[n], XmNnumImportTargets, &ngot), n++;
	XmDropSiteRetrieve(site, args, n);
	CHECK(w, ngot == 2);
	XmDropSiteStartUpdate(site);
	XmDropSiteEndUpdate(site);
	XmDropSiteUnregister(site);

	/* The drag context class and the atoms tables behind it. */
	dc = XmGetDragContext(site, CurrentTime);
	(void)dc;
	(void)XmTargetsAreCompatible(XtDisplay(site), targets, 2, targets, 1);
}

static void hello_activate(Widget w, XtPointer client, XtPointer call)
{
	(void)w;
	(void)client;
	(void)call;
}

static void exercise_mrm(struct worker *w, Widget parent)
{
	MrmRegisterArg names[] = {{"helloworld_button_activate", (XtPointer)hello_activate}};
	MrmHierarchy hierarchy;
	MrmType type;
	Widget tree = NULL;

	if (!uid_file)
		return;
	if (MrmOpenHierarchyPerDisplay(XtDisplay(parent), 1, &uid_file, NULL, &hierarchy) !=
	    MrmSUCCESS) {
		CHECK(w, !"MrmOpenHierarchyPerDisplay failed");
		return;
	}
	CHECK(w, MrmRegisterNamesInHierarchy(hierarchy, names, 1) == MrmSUCCESS);
	CHECK(w, MrmFetchWidget(hierarchy, "helloworld_main", parent, &tree, &type) ==
		     MrmSUCCESS);
	MrmCloseHierarchy(hierarchy);
	if (tree)
		XtDestroyWidget(tree);
}

static void exercise_classes(struct worker *w, Widget shell)
{
	WidgetClass classes[] = {xmTextWidgetClass, xmLabelGadgetClass,
				 xmPushButtonGadgetClass};
	XmSecondaryResourceData *data;
	Pixel fg, ts, bs, sel;
	Cardinal i, j, n;

	for (i = 0; i < XtNumber(classes); i++) {
		n = XmGetSecondaryResourceData(classes[i], &data);
		for (j = 0; j < n; j++) {
			XtFree((char *)data[j]->resources);
			XtFree((char *)data[j]);
		}
		if (n)
			XtFree((char *)data);
	}
	XmGetColors(XtScreen(shell), DefaultColormapOfScreen(XtScreen(shell)),
		    WhitePixelOfScreen(XtScreen(shell)), &fg, &ts, &bs, &sel);
	CHECK(w, fg != ts);
}

static void exercise_dialogs(Widget shell)
{
	Widget fsb, msg, prompt;
	XmString dir = XmStringCreateLocalized(".");

	fsb = XmCreateFileSelectionDialog(shell, "fsb", NULL, 0);
	XtVaSetValues(fsb, XmNdirectory, dir, NULL);
	XmStringFree(dir);
	XmFileSelectionDoSearch(fsb, NULL);
	XtManageChild(fsb);
	msg = XmCreateWarningDialog(shell, "warn", NULL, 0);
	XtManageChild(msg);
	prompt = XmCreatePromptDialog(shell, "prompt", NULL, 0);
	XtManageChild(prompt);
	XtUnmanageChild(fsb);
	XtUnmanageChild(msg);
	XtUnmanageChild(prompt);
	XtDestroyWidget(XtParent(fsb));
	XtDestroyWidget(XtParent(msg));
	XtDestroyWidget(XtParent(prompt));
}

static void run_once(struct worker *w, int iter)
{
	char *argv0[] = {"mtapps", NULL};
	int argc = 1;
	char **argv = argv0;
	XtAppContext app;
	Display *dpy;
	Widget shell, main_w, menubar, pulldown, cascade, form, label, text, tf;
	Widget list, combo, spin, spin_text, scale, popup, option, option_menu;
	Widget site, container, notebook, frame;
	XmString s;
	Pixmap pixmap;
	Arg args[8];
	int n, i;
	char name[32];

	app = XtCreateApplicationContext();
	dpy = XtOpenDisplay(app, NULL, "mtapps", "MtApps", NULL, 0, &argc, argv);
	if (!dpy) {
		w->failures++;
		snprintf(w->msg, sizeof(w->msg), "cannot open the display");
		XtDestroyApplicationContext(app);
		return;
	}
	n = 0;
	XtSetArg(args[n], XmNgeometry, "400x400"), n++;
	XtSetArg(args[n], XmNx, 20 + 420 * w->id), n++;
	shell = XtAppCreateShell("mtapps", "MtApps", applicationShellWidgetClass, dpy, args, n);

	main_w = XmCreateMainWindow(shell, "main", NULL, 0);
	XtManageChild(main_w);

	/* A menu bar with widget and gadget buttons and a nested cascade. */
	menubar = XmCreateMenuBar(main_w, "menubar", NULL, 0);
	pulldown = XmCreatePulldownMenu(menubar, "filePane", NULL, 0);
	for (i = 0; i < 3; i++) {
		snprintf(name, sizeof(name), "push%d", i);
		XtManageChild(XmCreatePushButton(pulldown, name, NULL, 0));
		snprintf(name, sizeof(name), "pushG%d", i);
		XtManageChild(XmCreatePushButtonGadget(pulldown, name, NULL, 0));
	}
	XtManageChild(XmCreateSeparator(pulldown, "sep", NULL, 0));
	XtManageChild(XmCreateToggleButtonGadget(pulldown, "toggleG", NULL, 0));
	n = 0;
	XtSetArg(args[n], XmNsubMenuId, pulldown), n++;
	cascade = XmCreateCascadeButton(menubar, "File", args, n);
	XtManageChild(cascade);
	pulldown = XmCreatePulldownMenu(menubar, "editPane", NULL, 0);
	XtManageChild(XmCreateToggleButton(pulldown, "toggle", NULL, 0));
	n = 0;
	XtSetArg(args[n], XmNsubMenuId, pulldown), n++;
	XtManageChild(XmCreateCascadeButtonGadget(menubar, "Edit", args, n));
	XtManageChild(menubar);

	form = XmCreateForm(main_w, "form", NULL, 0);
	XtManageChild(form);

	s = XmStringCreateLocalized("Label");
	n = 0;
	XtSetArg(args[n], XmNlabelString, s), n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM), n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM), n++;
	label = XmCreateLabel(form, "label", args, n);
	XtManageChild(label);
	XtManageChild(XmCreateLabelGadget(form, "labelG", args, n));
	XmStringFree(s);

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT), n++;
	XtSetArg(args[n], XmNrows, 4), n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, label), n++;
	text = XmCreateScrolledText(form, "text", args, n);
	XtManageChild(text);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, XtParent(text)), n++;
	tf = XmCreateTextField(form, "tf", args, n);
	XtManageChild(tf);

	n = 0;
	XtSetArg(args[n], XmNvisibleItemCount, 4), n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, tf), n++;
	list = XmCreateScrolledList(form, "list", args, n);
	XtManageChild(list);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, XtParent(list)), n++;
	combo = XmCreateDropDownComboBox(form, "combo", args, n);
	XtManageChild(combo);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, combo), n++;
	spin = XmCreateSpinBox(form, "spin", args, n);
	n = 0;
	XtSetArg(args[n], XmNspinBoxChildType, XmNUMERIC), n++;
	XtSetArg(args[n], XmNmaximumValue, 10), n++;
	spin_text = XmCreateTextField(spin, "spinText", args, n);
	XtManageChild(spin_text);
	XtManageChild(spin);

	n = 0;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL), n++;
	XtSetArg(args[n], XmNshowValue, True), n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, spin), n++;
	scale = XmCreateScale(form, "scale", args, n);
	XtManageChild(scale);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, scale), n++;
	option_menu = XmCreatePulldownMenu(form, "optionPane", NULL, 0);
	for (i = 0; i < 3; i++) {
		snprintf(name, sizeof(name), "choice%d", i);
		XtManageChild(XmCreatePushButtonGadget(option_menu, name, NULL, 0));
	}
	XtSetArg(args[n], XmNsubMenuId, option_menu), n++;
	option = XmCreateOptionMenu(form, "option", args, n);
	XtManageChild(option);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, option), n++;
	XtSetArg(args[n], XmNwidth, 50), n++;
	XtSetArg(args[n], XmNheight, 30), n++;
	site = XmCreateDrawingArea(form, "site", args, n);
	XtManageChild(site);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, site), n++;
	frame = XmCreateFrame(form, "frame", args, n);
	XtManageChild(frame);
	container = XmCreateContainer(frame, "container", NULL, 0);
	for (i = 0; i < 3; i++) {
		snprintf(name, sizeof(name), "icon%d", i);
		XtManageChild(XmCreateIconGadget(container, name, NULL, 0));
	}
	XtManageChild(container);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
	XtSetArg(args[n], XmNtopWidget, frame), n++;
	notebook = XmCreateNotebook(form, "notebook", args, n);
	for (i = 0; i < 2; i++) {
		snprintf(name, sizeof(name), "page%d", i);
		XtManageChild(XmCreateLabel(notebook, name, NULL, 0));
	}
	XtManageChild(notebook);

	popup = XmCreatePopupMenu(form, "popup", NULL, 0);
	XtManageChild(XmCreatePushButton(popup, "popPush", NULL, 0));
	XtManageChild(XmCreateCascadeButtonGadget(popup, "popCascade", NULL, 0));

	XtRealizeWidget(shell);
	drain(app, dpy);

	exercise_strings(w, label);
	exercise_text(w, text, tf);
	exercise_list(w, list);
	XmComboBoxAddItem(combo, s = XmStringCreateLocalized("combo"), 0, False);
	XmStringFree(s);
	XmScaleSetValue(scale, 30 + iter);
	XmProcessTraversal(text, XmTRAVERSE_CURRENT);
	drain(app, dpy);
	exercise_menus(shell, popup, option);
	exercise_clipboard(w, shell);
	exercise_dnd(w, site);
	exercise_classes(w, shell);
	exercise_mrm(w, form);
	exercise_dialogs(shell);
	pixmap = XmGetPixmap(XtScreen(shell), "xm_warning", BlackPixelOfScreen(XtScreen(shell)),
			     WhitePixelOfScreen(XtScreen(shell)));
	if (pixmap != XmUNSPECIFIED_PIXMAP)
		XmDestroyPixmap(XtScreen(shell), pixmap);
	drain(app, dpy);

	XtDestroyWidget(shell);
	drain(app, dpy);
	XtCloseDisplay(dpy);
	XtDestroyApplicationContext(app);
}

static void *run(void *arg)
{
	struct worker *w = arg;
	int i;

	for (i = 0; i < w->iterations; i++)
		run_once(w, i);
	return NULL;
}

int main(int argc, char **argv)
{
	struct worker workers[MAX_THREADS];
	int nthreads = 2, i, failures = 0;
	Display *probe;

	if (argc > 1)
		iterations = atoi(argv[1]);
	if (argc > 2)
		nthreads = atoi(argv[2]);
	if (argc > 3)
		uid_file = argv[3];
	if (iterations < 1 || nthreads < 1 || nthreads > MAX_THREADS) {
		fprintf(stderr, "usage: mtapps [iterations [threads [uid-file]]]\n");
		return 1;
	}

	if (!XInitThreads() || !XtToolkitThreadInitialize()) {
		fprintf(stderr, "mtapps: Xlib or Xt has no thread support\n");
		return 77;
	}
	probe = XOpenDisplay(NULL);
	if (!probe) {
		fprintf(stderr, "mtapps: no display, skipped\n");
		return 77;
	}
	XCloseDisplay(probe);
	XtToolkitInitialize();
	XtSetLanguageProc(NULL, NULL, NULL);
	MrmInitialize();

	for (i = 0; i < nthreads; i++) {
		memset(&workers[i], 0, sizeof(workers[i]));
		workers[i].id = i;
		workers[i].iterations = iterations;
		if (pthread_create(&workers[i].thread, NULL, run, &workers[i]) != 0) {
			fprintf(stderr, "mtapps: pthread_create failed\n");
			return 1;
		}
	}
	for (i = 0; i < nthreads; i++) {
		pthread_join(workers[i].thread, NULL);
		if (workers[i].failures) {
			fprintf(stderr, "mtapps: thread %d: %d failures, last: %s\n", i,
				workers[i].failures, workers[i].msg);
			failures++;
		}
	}
	printf("mtapps: %d threads x %d iterations: %s\n", nthreads, iterations,
	       failures ? "FAILED" : "ok");
	return failures ? 1 : 0;
}
