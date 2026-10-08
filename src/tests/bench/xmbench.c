/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * xmbench: micro- and macro-benchmarks for libXm.
 *
 *   xmbench [-r REPEAT] [-s SCALE] [-j FILE] [-t] [-l] [CASE|GROUP ...]
 *
 * -t calls XtToolkitThreadInitialize first, so that Xt's (and Motif's)
 * locks are live, as in a multithreaded program.
 *
 * Each case reports, per operation:
 *   - ns       median wall time over REPEAT runs (default 5)
 *   - cpu      median CPU time of xmbench itself (not of the X server),
 *              which other load on the machine disturbs less
 *   - mallocs  calls to malloc/calloc/realloc
 *   - requests X requests (XNextRequest delta)
 *   - rtrips   round trips (waits for a reply in libxcb, which every Xlib
 *              call with a reply makes, XSync included)
 *   - icvalues calls to XSetICValues
 * The counters come from libxmbench_preload.so, which xmbench preloads by
 * re-executing itself; set XMBENCH_NO_PRELOAD=1 to run without it.
 *
 * The X requests and round trips of a case are counted before the final
 * XSync that each timed run ends with (the time includes it, so that the
 * server's share of the work is measured too).  The JSON output also
 * has their exact totals per timed run (the median over REPEAT runs),
 * which the per-op figures round away when they are rare.
 *
 * Cases that need an X server are skipped when DISPLAY is unset; xmbench
 * exits with 77 when every selected case was skipped.  Results are
 * printed as a table and, with -j, written as JSON.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Container.h>
#include <Xm/DrawP.h>
#include <Xm/Form.h>
#include <Xm/IconG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBarP.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/TraitP.h>
#include <Xm/AccTextT.h>
#include <Xm/ActivatableT.h>
#include <Xm/CareVisualT.h>
#include <Xm/SpecRenderT.h>
#include <Xm/XmIm.h>

#define EXIT_SKIP 77
#define MAX_REPEAT 64

struct bench_case {
	const char *name;
	const char *group;
	const char *desc;
	int needs_x;
	long n;                    /* operations at scale 1 */
	void (*init)(long n);      /* once, untimed */
	void (*setup)(long n);     /* before each run, untimed */
	long (*run)(long n);       /* timed, returns the operations done */
	void (*teardown)(void);    /* after each run, untimed */
	void (*fini)(void);        /* once, untimed */
};

struct counters {
	unsigned long mallocs, replies, icvalues, requests;
};

struct result {
	double ns, cpu, mallocs, requests, rtrips, icvalues;
	double run_requests, run_rtrips;   /* per timed run, not per op */
};

static XtAppContext app;
static Display *dpy;
static Widget top, root;
static volatile unsigned long sink;

static unsigned long *c_mallocs, *c_replies, *c_icvalues;

/* ------------------------------------------------------------------ */
/* Harness                                                             */
/* ------------------------------------------------------------------ */

static double clock_ns(clockid_t id)
{
	struct timespec ts;

	clock_gettime(id, &ts);
	return ts.tv_sec * 1e9 + ts.tv_nsec;
}

static double now_ns(void)
{
	return clock_ns(CLOCK_MONOTONIC);
}

static void snap(struct counters *c)
{
	c->mallocs = c_mallocs ? *c_mallocs : 0;
	c->replies = c_replies ? *c_replies : 0;
	c->icvalues = c_icvalues ? *c_icvalues : 0;
	c->requests = dpy ? XNextRequest(dpy) : 0;
}

/* Process everything that is pending, without blocking. */
static void drain(void)
{
	XtInputMask mask;

	if (!dpy)
		return;
	XSync(dpy, False);
	while ((mask = XtAppPending(app)) != 0)
		XtAppProcessEvent(app, mask);
}

static int cmp_double(const void *a, const void *b)
{
	double x = *(const double *)a, y = *(const double *)b;

	return (x > y) - (x < y);
}

static double median(double *v, int n)
{
	qsort(v, n, sizeof *v, cmp_double);
	return (n & 1) ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2;
}

static void run_case(const struct bench_case *bc, long n, int repeat,
		     struct result *res)
{
	double ns[MAX_REPEAT], cpu[MAX_REPEAT], ma[MAX_REPEAT], rq[MAX_REPEAT];
	double rt[MAX_REPEAT], ic[MAX_REPEAT], rq_run[MAX_REPEAT];
	double rt_run[MAX_REPEAT];
	int r;

	if (bc->init)
		bc->init(n);
	for (r = 0; r < repeat; r++) {
		struct counters before, after;
		double t0, t1, c0, c1;
		long ops;

		if (bc->setup)
			bc->setup(n);
		drain();
		snap(&before);
		c0 = clock_ns(CLOCK_PROCESS_CPUTIME_ID);
		t0 = now_ns();
		ops = bc->run(n);
		snap(&after);
		if (dpy)
			XSync(dpy, False);
		t1 = now_ns();
		c1 = clock_ns(CLOCK_PROCESS_CPUTIME_ID);
		if (ops < 1)
			ops = 1;
		ns[r] = (t1 - t0) / ops;
		cpu[r] = (c1 - c0) / ops;
		ma[r] = (double)(after.mallocs - before.mallocs) / ops;
		rq[r] = (double)(after.requests - before.requests) / ops;
		rt[r] = (double)(after.replies - before.replies) / ops;
		ic[r] = (double)(after.icvalues - before.icvalues) / ops;
		rq_run[r] = after.requests - before.requests;
		rt_run[r] = after.replies - before.replies;
		if (bc->teardown)
			bc->teardown();
		drain();
	}
	if (bc->fini)
		bc->fini();
	drain();
	res->ns = median(ns, repeat);
	res->cpu = median(cpu, repeat);
	res->mallocs = median(ma, repeat);
	res->requests = median(rq, repeat);
	res->rtrips = median(rt, repeat);
	res->icvalues = median(ic, repeat);
	res->run_requests = median(rq_run, repeat);
	res->run_rtrips = median(rt_run, repeat);
}

/* Re-execute with the counting library preloaded, unless already done. */
static void preload_self(char **argv)
{
#ifdef XMBENCH_PRELOAD
	const char *old = getenv("LD_PRELOAD");
	char *val;

	if (getenv("XMBENCH_NO_PRELOAD") || getenv("XMBENCH_PRELOADED"))
		return;
	if (access(XMBENCH_PRELOAD, R_OK) != 0)
		return;
	if (old && *old) {
		val = malloc(strlen(old) + strlen(XMBENCH_PRELOAD) + 2);
		if (!val)
			return;
		sprintf(val, "%s:%s", XMBENCH_PRELOAD, old);
	} else {
		val = strdup(XMBENCH_PRELOAD);
		if (!val)
			return;
	}
	setenv("LD_PRELOAD", val, 1);
	setenv("XMBENCH_PRELOADED", "1", 1);
	execv("/proc/self/exe", argv);
	/* Carry on without the counters. */
	unsetenv("XMBENCH_PRELOADED");
	if (old)
		setenv("LD_PRELOAD", old, 1);
	else
		unsetenv("LD_PRELOAD");
	free(val);
#else
	(void)argv;
#endif
}

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static Widget work;  /* per-case container, child of root */

static void destroy_work(void)
{
	if (work) {
		XtDestroyWidget(work);
		work = NULL;
	}
}

/* ------------------------------------------------------------------ */
/* Traits                                                              */
/* ------------------------------------------------------------------ */

static WidgetClass trait_classes[4];
static XrmQuark trait_names[4];

static void trait_init(long n)
{
	(void)n;
	trait_classes[0] = xmPushButtonWidgetClass;
	trait_classes[1] = xmLabelGadgetClass;
	trait_classes[2] = xmTextWidgetClass;
	trait_classes[3] = xmRowColumnWidgetClass;
	for (n = 0; n < 4; n++)
		XtInitializeWidgetClass(trait_classes[n]);
	trait_names[0] = XmQTactivatable;
	trait_names[1] = XmQTaccessTextual;
	trait_names[2] = XmQTcareParentVisual;
	trait_names[3] = XmQTspecifyRenderTable;
}

static long trait_run(long n)
{
	unsigned long hits = 0;
	long i;

	for (i = 0; i < n; i++)
		hits += XmeTraitGet((XtPointer)trait_classes[i & 3],
				    trait_names[(i >> 2) & 3]) != NULL;
	sink = hits;
	return n;
}

/* ------------------------------------------------------------------ */
/* Gadget Get/SetValues and the gadget cache                           */
/* ------------------------------------------------------------------ */

static Widget gadget;

static void gadget_new(WidgetClass wc)
{
	work = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, root,
				       NULL);
	gadget = XtVaCreateManagedWidget("gadget", wc, work, NULL);
	drain();
}

static void gadget_init(long n)
{
	(void)n;
	gadget_new(xmLabelGadgetClass);
}

static void toggle_init(long n)
{
	(void)n;
	gadget_new(xmToggleButtonGadgetClass);
}

static void pushbg_init(long n)
{
	(void)n;
	gadget_new(xmPushButtonGadgetClass);
}

static void separatorg_init(long n)
{
	(void)n;
	gadget_new(xmSeparatorGadgetClass);
}

static void icong_init(long n)
{
	(void)n;
	gadget_new(xmIconGadgetClass);
}

/* A CascadeButtonGadget wants a menu for a parent. */
static void cascadebg_init(long n)
{
	(void)n;
	work = XmCreateMenuBar(root, "bar", NULL, 0);
	XtManageChild(work);
	gadget = XtVaCreateManagedWidget("gadget", xmCascadeButtonGadgetClass,
					 work, NULL);
	drain();
}

/* 1000 shells, each with its extension data, as in a big application. */
#define N_SHELLS 1000
static Widget shells[N_SHELLS];

static void shells_init(long n)
{
	int i;

	gadget_init(n);
	for (i = 0; i < N_SHELLS; i++)
		shells[i] = XtCreatePopupShell("shell", topLevelShellWidgetClass,
					       top, NULL, 0);
}

static void shells_fini(void)
{
	int i;

	for (i = 0; i < N_SHELLS; i++)
		XtDestroyWidget(shells[i]);
	destroy_work();
}

static long gadget_get_run(long n)
{
	Dimension mw = 0;
	unsigned char al = 0;
	long i;

	for (i = 0; i < n; i++)
		XtVaGetValues(gadget, XmNmarginWidth, &mw, XmNalignment, &al,
			      NULL);
	sink = mw + al;
	return n;
}

static long gadget_set_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XtVaSetValues(gadget, XmNmarginWidth, (Dimension)(2 + (i & 1)),
			      NULL);
	return n;
}

/* The Separator cache has no margin width or alignment. */
static long separatorg_get_run(long n)
{
	Dimension margin = 0;
	unsigned char type = 0;
	long i;

	for (i = 0; i < n; i++)
		XtVaGetValues(gadget, XmNmargin, &margin, XmNseparatorType,
			      &type, NULL);
	sink = margin + type;
	return n;
}

static long separatorg_set_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XtVaSetValues(gadget, XmNmargin, (Dimension)(2 + (i & 1)), NULL);
	return n;
}

/* A shell resource that its VendorShell extension object holds. */
static long shell_get_run(long n)
{
	unsigned char response = 0;
	long i;

	for (i = 0; i < n; i++)
		XtVaGetValues(top, XmNdeleteResponse, &response, NULL);
	sink = response;
	return n;
}

static void cache_setup(long n)
{
	(void)n;
	work = XtVaCreateWidget("rc", xmRowColumnWidgetClass, root, NULL);
}

/* Gadgets in runs of 10 with the same look, 200 different looks. */
static long cache_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XtVaCreateWidget("g", xmLabelGadgetClass, work,
				 XmNmarginWidth, (Dimension)((i / 10) % 200),
				 NULL);
	return n;
}

/* Every gadget with a look of its own: n distinct cache parts. */
static long cache_distinct_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XtVaCreateWidget("g", xmLabelGadgetClass, work,
				 XmNmarginWidth, (Dimension)(i % 100),
				 XmNmarginHeight, (Dimension)(i / 100 % 100),
				 NULL);
	return n;
}

/*
 * The same with colors: each gadget its own foreground and select color.
 * Most of the time goes to Xt's GC cache, which is a list too.
 */
static long cache_colors_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XtVaCreateWidget("g", xmToggleButtonGadgetClass, work,
				 XmNforeground, (Pixel)i,
				 XmNselectColor, (Pixel)(n - i),
				 NULL);
	return n;
}

/* ------------------------------------------------------------------ */
/* XmText source                                                       */
/* ------------------------------------------------------------------ */

static Widget text;
static char chunk[1025];

static void text_new(const char *value)
{
	work = XtVaCreateWidget("text", xmTextWidgetClass, root,
				XmNeditMode, XmMULTI_LINE_EDIT,
				XmNvalue, value, NULL);
	text = work;
}

static char *make_lines(long len)
{
	char *s = malloc(len + 1);
	long i;

	if (!s)
		exit(1);
	for (i = 0; i < len; i++)
		s[i] = (i % 64 == 63) ? '\n' : 'a' + i % 26;
	s[len] = '\0';
	return s;
}

static void text_append_setup(long n)
{
	(void)n;
	memset(chunk, 'x', 1024);
	chunk[1023] = '\n';
	chunk[1024] = '\0';
	text_new("");
}

static long text_append_run(long n)
{
	XmTextPosition pos = 0;
	long i;

	for (i = 0; i < n; i++) {
		XmTextInsert(text, pos, chunk);
		pos += 1024;
	}
	return n;
}

static void text_type_setup(long n)
{
	char *s = make_lines(1 << 20);

	(void)n;
	text_new(s);
	free(s);
}

/* Type at the middle of a 1 MB text. */
static long text_type_run(long n)
{
	XmTextPosition pos = 1 << 19;
	long i;

	for (i = 0; i < n; i++)
		XmTextInsert(text, pos++, "k");
	return n;
}

static void text_insdel_setup(long n)
{
	char *s = make_lines((1 << 20) - 2);

	(void)n;
	text_new(s);
	free(s);
}

/* Insert and delete two characters at the middle, in turn. */
static long text_insdel_run(long n)
{
	XmTextPosition pos = 1 << 19;
	long i;

	for (i = 0; i < n; i++) {
		if (i & 1)
			XmTextReplace(text, pos, pos + 2, "");
		else
			XmTextInsert(text, pos, "kk");
	}
	return n;
}

static void text_teardown(void)
{
	destroy_work();
	text = NULL;
}

/* ------------------------------------------------------------------ */
/* XmIm spot location                                                  */
/* ------------------------------------------------------------------ */

static void spot_init(long n)
{
	char *s = make_lines(4000);

	(void)n;
	work = XtVaCreateManagedWidget("text", xmTextWidgetClass, root,
				       XmNeditMode, XmMULTI_LINE_EDIT,
				       XmNrows, 20, XmNcolumns, 70,
				       XmNvalue, s, NULL);
	text = work;
	free(s);
	drain();
	XmProcessTraversal(text, XmTRAVERSE_CURRENT);
	drain();
	if (!XmImGetXIC(text, XmINHERIT_POLICY, NULL, 0))
		fprintf(stderr, "xmbench: no input context, spot cases do not "
			"reach XSetICValues (set XMODIFIERS=@im=none)\n");
}

static long spot_same_run(long n)
{
	XPoint pt = { 10, 20 };
	long i;

	for (i = 0; i < n; i++)
		XmImVaSetValues(text, XmNspotLocation, &pt, NULL);
	return n;
}

/* Move the cursor along the first lines, as arrow keys do. */
static long text_cursor_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XmTextSetInsertionPosition(text, i % 500);
	return n;
}

static void spot_fini(void)
{
	text_teardown();
}

/* ------------------------------------------------------------------ */
/* ScrollBar autorepeat                                                */
/* ------------------------------------------------------------------ */

static Widget sbar;
static long sb_ticks;

static void sb_cb(Widget w, XtPointer cd, XtPointer call)
{
	(void)w; (void)cd; (void)call;
	sb_ticks++;
}

static void sb_init(long n)
{
	(void)n;
	work = XtVaCreateManagedWidget("sb", xmScrollBarWidgetClass, root,
				       XmNorientation, XmHORIZONTAL,
				       XmNwidth, 300, XmNheight, 20,
				       XmNminimum, 0, XmNmaximum, 100000000,
				       XmNsliderSize, 1, XmNincrement, 1,
				       XmNinitialDelay, 1, XmNrepeatDelay, 1,
				       NULL);
	sbar = work;
	XtAddCallback(sbar, XmNincrementCallback, sb_cb, NULL);
	drain();
}

static void sb_setup(long n)
{
	(void)n;
	XtVaSetValues(sbar, XmNvalue, 0, NULL);
}

static long sb_run(long n)
{
	XmScrollBarWidget sbw = (XmScrollBarWidget)sbar;
	XEvent ev;

	memset(&ev, 0, sizeof ev);
	ev.xbutton.type = ButtonPress;
	ev.xbutton.display = dpy;
	ev.xbutton.window = XtWindow(sbar);
	ev.xbutton.button = Button1;
	ev.xbutton.time = XtLastTimestampProcessed(dpy);
	ev.xbutton.x = sbw->scrollBar.arrow2_x + sbw->scrollBar.arrow_width / 2;
	ev.xbutton.y = sbw->scrollBar.arrow2_y + sbw->scrollBar.arrow_height / 2;
	sb_ticks = 0;
	XtCallActionProc(sbar, "Select", &ev, NULL, 0);
	while (sb_ticks < n) {
		XtInputMask mask = XtAppPending(app);

		XtAppProcessEvent(app, mask ? mask : XtIMAll);
	}
	ev.xbutton.type = ButtonRelease;
	XtCallActionProc(sbar, "Release", &ev, NULL, 0);
	return sb_ticks;
}

/* ------------------------------------------------------------------ */
/* Popup menu post/unpost                                              */
/* ------------------------------------------------------------------ */

static Widget popup;

static void menu_init(long n)
{
	int i;

	(void)n;
	work = XtVaCreateManagedWidget("bb", xmBulletinBoardWidgetClass, root,
				       XmNwidth, 200, XmNheight, 200, NULL);
	popup = XmCreatePopupMenu(work, "popup", NULL, 0);
	for (i = 0; i < 10; i++)
		XtVaCreateManagedWidget("item", xmPushButtonWidgetClass,
					popup, NULL);
	drain();
	/* Give the shell the focus, as a window manager would. */
	XSetInputFocus(dpy, XtWindow(top), RevertToParent, CurrentTime);
	XmProcessTraversal(work, XmTRAVERSE_CURRENT);
	drain();
}

static long menu_run(long n)
{
	XEvent ev;
	long i;

	memset(&ev, 0, sizeof ev);
	ev.xbutton.type = ButtonPress;
	ev.xbutton.display = dpy;
	ev.xbutton.window = XtWindow(work);
	ev.xbutton.button = Button3;
	ev.xbutton.x_root = 50;
	ev.xbutton.y_root = 50;
	for (i = 0; i < n; i++) {
		XmMenuPosition(popup, &ev.xbutton);
		XtManageChild(popup);
		drain();
		XtUnmanageChild(popup);
		drain();
	}
	return n;
}

/* ------------------------------------------------------------------ */
/* XmGetVisibility                                                     */
/* ------------------------------------------------------------------ */

static Widget vis_target;

static void vis_init(long n)
{
	int i;

	(void)n;
	work = XtVaCreateManagedWidget("bb", xmBulletinBoardWidgetClass, root,
				       XmNwidth, 400, XmNheight, 300,
				       XmNresizePolicy, XmRESIZE_NONE,
				       XmNmarginWidth, 0, XmNmarginHeight, 0,
				       NULL);
	vis_target = XtVaCreateManagedWidget("target",
					     xmPushButtonWidgetClass, work,
					     XmNx, 0, XmNy, 0,
					     XmNwidth, 200, XmNheight, 100,
					     XmNrecomputeSize, False, NULL);
	/* 50 siblings above the target, none of them covering it. */
	for (i = 0; i < 50; i++)
		XtVaCreateManagedWidget("sib", xmPushButtonWidgetClass, work,
					XmNx, 200 + (i % 10) * 18,
					XmNy, (i / 10) * 40,
					XmNwidth, 16, XmNheight, 30,
					XmNrecomputeSize, False, NULL);
	drain();
}

static long vis_run(long n)
{
	long i;
	int v = 0;

	for (i = 0; i < n; i++)
		v += XmGetVisibility(vis_target);
	sink = v;
	return n;
}

/* ------------------------------------------------------------------ */
/* Shadows                                                             */
/* ------------------------------------------------------------------ */

static Pixmap shadow_pm;
static GC shadow_gc[2];

static void shadow_init(long n)
{
	Screen *scr = DefaultScreenOfDisplay(dpy);

	(void)n;
	shadow_pm = XCreatePixmap(dpy, RootWindowOfScreen(scr), 400, 200,
				  DefaultDepthOfScreen(scr));
	shadow_gc[0] = XCreateGC(dpy, shadow_pm, 0, NULL);
	shadow_gc[1] = XCreateGC(dpy, shadow_pm, 0, NULL);
	XSetForeground(dpy, shadow_gc[0], WhitePixelOfScreen(scr));
	XSetForeground(dpy, shadow_gc[1], BlackPixelOfScreen(scr));
}

static long shadow_run(long n, int thick)
{
	long i;

	for (i = 0; i < n; i++)
		XmeDrawShadows(dpy, shadow_pm, shadow_gc[0], shadow_gc[1],
			       i & 15, 0, 300, 100, thick, XmSHADOW_OUT);
	return n;
}

static long shadow2_run(long n) { return shadow_run(n, 2); }
static long shadow8_run(long n) { return shadow_run(n, 8); }

static void shadow_fini(void)
{
	XFreeGC(dpy, shadow_gc[0]);
	XFreeGC(dpy, shadow_gc[1]);
	XFreePixmap(dpy, shadow_pm);
}

/* ------------------------------------------------------------------ */
/* XmString                                                            */
/* ------------------------------------------------------------------ */

static XmRenderTable str_rt, str_rt_xft, str_rt_fs;
static XmString str_one, str_multi;
static Widget str_label;

static long xs_create_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XmStringFree(XmStringCreateLocalized("The quick brown fox"));
	return n;
}

static long xs_concat_run(long n)
{
	XmString s = XmStringCreateLocalized("");
	long i;

	for (i = 0; i < n; i++)
		s = XmStringConcatAndFree(s,
			XmStringCreateLocalized("segment "));
	XmStringFree(s);
	return n;
}

static void xs_init(long n)
{
	XmRendition rend;
	Arg args[2];

	(void)n;
	work = XtVaCreateManagedWidget("label", xmLabelWidgetClass, root,
				       XmNwidth, 400, XmNheight, 40,
				       XmNrecomputeSize, False, NULL);
	str_label = work;
	XtVaGetValues(str_label, XmNrenderTable, &str_rt, NULL);
	str_rt = XmRenderTableCopy(str_rt, NULL, 0);
	XtSetArg(args[0], XmNfontName, "Sans-10");
	XtSetArg(args[1], XmNfontType, XmFONT_IS_XFT);
	rend = XmRenditionCreate(str_label, XmFONTLIST_DEFAULT_TAG, args, 2);
	str_rt_xft = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_NEW);
	XmRenditionFree(rend);
	XtSetArg(args[0], XmNfontName, "fixed");
	XtSetArg(args[1], XmNfontType, XmFONT_IS_FONTSET);
	rend = XmRenditionCreate(str_label, XmFONTLIST_DEFAULT_TAG, args, 2);
	str_rt_fs = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_NEW);
	XmRenditionFree(rend);
	str_one = XmStringCreateLocalized("The quick brown fox");
	str_multi = XmStringGenerate("The quick\tbrown fox\njumps over\n"
				     "the lazy dog", NULL, XmCHARSET_TEXT,
				     NULL);
	drain();
}

static long xs_extent(long n, XmRenderTable rt, XmString s)
{
	Dimension w = 0, h = 0;
	long i;

	for (i = 0; i < n; i++)
		XmStringExtent(rt, s, &w, &h);
	sink = w + h;
	return n;
}

static long xs_extent_run(long n) { return xs_extent(n, str_rt, str_one); }
static long xs_extent_multi_run(long n) { return xs_extent(n, str_rt, str_multi); }
static long xs_extent_xft_run(long n) { return xs_extent(n, str_rt_xft, str_one); }
static long xs_extent_fs_run(long n) { return xs_extent(n, str_rt_fs, str_one); }

static long xs_draw(long n, XmRenderTable rt)
{
	GC gc = XCreateGC(dpy, XtWindow(str_label), 0, NULL);
	long i;

	for (i = 0; i < n; i++)
		XmStringDraw(dpy, XtWindow(str_label), rt, str_one, gc, 0, 20,
			     400, XmALIGNMENT_BEGINNING,
			     XmSTRING_DIRECTION_L_TO_R, NULL);
	XFreeGC(dpy, gc);
	return n;
}

static long xs_draw_run(long n) { return xs_draw(n, str_rt); }
static long xs_draw_xft_run(long n) { return xs_draw(n, str_rt_xft); }
static long xs_draw_fs_run(long n) { return xs_draw(n, str_rt_fs); }

static void xs_fini(void)
{
	XmStringFree(str_one);
	XmStringFree(str_multi);
	XmRenderTableFree(str_rt);
	XmRenderTableFree(str_rt_xft);
	XmRenderTableFree(str_rt_fs);
	destroy_work();
}

/* Labels whose render table is converted from a resource string. */
static void rt_setup(long n)
{
	(void)n;
	work = XtVaCreateWidget("rc", xmRowColumnWidgetClass, root, NULL);
}

static long rt_run(long n)
{
	static const char spec[] = "-*-fixed-medium-r-normal--13-*-*-*-*-*-*-*";
	long i;

	for (i = 0; i < n; i++)
		XtVaCreateWidget("l", xmLabelWidgetClass, work,
				 XtVaTypedArg, XmNrenderTable, XmRString,
				 spec, (int)sizeof spec, NULL);
	return n;
}

/* ------------------------------------------------------------------ */
/* Big managers                                                        */
/* ------------------------------------------------------------------ */

static Widget *kids;

static void kids_alloc(long n)
{
	kids = realloc(kids, n * sizeof *kids);
	if (!kids)
		exit(1);
}

static void rc_setup(long n)
{
	kids_alloc(n);
	work = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, root,
				       XmNorientation, XmHORIZONTAL,
				       XmNpacking, XmPACK_COLUMN,
				       XmNnumColumns, 100, NULL);
	drain();
}

static long rc_fill(long n, WidgetClass wc)
{
	long i;

	for (i = 0; i < n; i++)
		kids[i] = XtCreateWidget("b", wc, work, NULL, 0);
	XtManageChildren(kids, n);
	drain();
	return n;
}

static long rc_buttons_run(long n) { return rc_fill(n, xmPushButtonWidgetClass); }
static long rc_gadgets_run(long n) { return rc_fill(n, xmPushButtonGadgetClass); }

static void rc_destroy_setup(long n)
{
	rc_setup(n);
	rc_fill(n, xmPushButtonGadgetClass);
}

static long destroy_work_run(long n)
{
	destroy_work();
	drain();
	return n;
}

static void form_setup(long n)
{
	kids_alloc(n);
	work = XtVaCreateManagedWidget("form", xmFormWidgetClass, root, NULL);
	drain();
}

static long form_fill(long n)
{
	long i;

	for (i = 0; i < n; i++)
		kids[i] = XtVaCreateWidget("s", xmSeparatorWidgetClass, work,
			XmNheight, 4, XmNwidth, 20, XmNshadowThickness, 0,
			XmNmargin, 0, XmNleftAttachment, XmATTACH_FORM,
			XmNtopAttachment, i ? XmATTACH_WIDGET : XmATTACH_FORM,
			XmNtopWidget, i ? kids[i - 1] : NULL, NULL);
	XtManageChildren(kids, n);
	drain();
	return n;
}

static long form_run(long n) { return form_fill(n); }

static void form_destroy_setup(long n)
{
	form_setup(n);
	form_fill(n);
}

static void container_setup(long n)
{
	kids_alloc(n);
	work = XtVaCreateManagedWidget("container", xmContainerWidgetClass,
				       root, XmNlayoutType, XmSPATIAL,
				       XmNspatialStyle, XmNONE,
				       XmNwidth, 700, XmNheight, 500, NULL);
	drain();
}

static long container_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		kids[i] = XtVaCreateWidget("icon", xmIconGadgetClass, work,
					   XmNx, (Position)((i % 100) * 60),
					   XmNy, (Position)((i / 100) * 40 % 30000),
					   NULL);
	XtManageChildren(kids, n);
	drain();
	return n;
}

/* ------------------------------------------------------------------ */
/* XmList                                                              */
/* ------------------------------------------------------------------ */

static Widget list;
static XmString *items;
static long n_items;

static void items_make(long n)
{
	char buf[32];
	long i;

	items = malloc(n * sizeof *items);
	if (!items)
		exit(1);
	for (i = 0; i < n; i++) {
		snprintf(buf, sizeof buf, "item %ld", i);
		items[i] = XmStringCreateLocalized(buf);
	}
	n_items = n;
}

static void items_free(void)
{
	long i;

	for (i = 0; i < n_items; i++)
		XmStringFree(items[i]);
	free(items);
	items = NULL;
	n_items = 0;
}

static void list_new(void)
{
	Arg args[2];

	XtSetArg(args[0], XmNvisibleItemCount, 20);
	XtSetArg(args[1], XmNselectionPolicy, XmMULTIPLE_SELECT);
	list = XmCreateScrolledList(root, "list", args, 2);
	work = XtParent(list);
	XtManageChild(list);
	XtManageChild(work);
}

static void list_add_setup(long n)
{
	items_make(n);
	list_new();
	drain();
}

static long list_add_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XmListAddItemUnselected(list, items[i], 0);
	return n;
}

static void list_teardown(void)
{
	destroy_work();
	list = NULL;
	items_free();
}

/* A filled list shared by the select, delete and page-down cases. */
#define LIST_FILL 100000

static void list_fill_setup(long n)
{
	(void)n;
	items_make(LIST_FILL);
	list_new();
	XmListAddItems(list, items, LIST_FILL, 0);
	drain();
}

static long list_select_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XmListSelectItem(list, items[(i * 7919) % LIST_FILL], False);
	return n;
}

static long list_delete_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XmListDeleteItem(list, items[(i * 7919) % LIST_FILL]);
	return n;
}

static long list_pagedown_run(long n)
{
	long i;

	for (i = 0; i < n; i++) {
		XtCallActionProc(list, "ListNextPage", NULL, NULL, 0);
		drain();
	}
	return n;
}

/* ------------------------------------------------------------------ */
/* Xft drawing in many windows                                         */
/* ------------------------------------------------------------------ */

static void xft_setup(long n)
{
	XmRendition rend;
	XmRenderTable rt;
	Arg args[2];
	long i;

	kids_alloc(n);
	work = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, root,
				       XmNorientation, XmHORIZONTAL,
				       XmNpacking, XmPACK_COLUMN,
				       XmNnumColumns, 50, NULL);
	XtSetArg(args[0], XmNfontName, "Sans-8");
	XtSetArg(args[1], XmNfontType, XmFONT_IS_XFT);
	rend = XmRenditionCreate(work, XmFONTLIST_DEFAULT_TAG, args, 2);
	rt = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_NEW);
	XmRenditionFree(rend);
	for (i = 0; i < n; i++)
		kids[i] = XtVaCreateWidget("x", xmLabelWidgetClass, work,
					   XmNrenderTable, rt, NULL);
	XtManageChildren(kids, n);
	XmRenderTableFree(rt);
	drain();
}

/* Expose every label once. */
static long xft_run(long n)
{
	long i;

	for (i = 0; i < n; i++)
		XClearArea(dpy, XtWindow(kids[i]), 0, 0, 0, 0, True);
	drain();
	return n;
}

/* ------------------------------------------------------------------ */
/* Case table                                                          */
/* ------------------------------------------------------------------ */

static const struct bench_case cases[] = {
	{ "trait-get", "micro", "XmeTraitGet, hits and misses",
	  0, 10000000, trait_init, NULL, trait_run, NULL, NULL },
	{ "gadget-get", "micro", "XtGetValues on a LabelGadget",
	  1, 100000, gadget_init, NULL, gadget_get_run, NULL, destroy_work },
	{ "gadget-set", "micro", "XtSetValues of a cached LabelGadget resource",
	  1, 10000, gadget_init, NULL, gadget_set_run, NULL, destroy_work },
	{ "gadget-get-shells", "micro", "XtGetValues on a LabelGadget, 1000 shells",
	  1, 100000, shells_init, NULL, gadget_get_run, NULL, shells_fini },
	{ "toggle-get", "micro", "XtGetValues on a ToggleButtonGadget",
	  1, 100000, toggle_init, NULL, gadget_get_run, NULL, destroy_work },
	{ "toggle-set", "micro", "XtSetValues of a cached ToggleButtonGadget resource",
	  1, 10000, toggle_init, NULL, gadget_set_run, NULL, destroy_work },
	{ "pushbg-get", "micro", "XtGetValues on a PushButtonGadget",
	  1, 100000, pushbg_init, NULL, gadget_get_run, NULL, destroy_work },
	{ "pushbg-set", "micro", "XtSetValues of a cached PushButtonGadget resource",
	  1, 10000, pushbg_init, NULL, gadget_set_run, NULL, destroy_work },
	{ "cascadebg-get", "micro", "XtGetValues on a CascadeButtonGadget",
	  1, 100000, cascadebg_init, NULL, gadget_get_run, NULL, destroy_work },
	{ "cascadebg-set", "micro", "XtSetValues of a cached CascadeButtonGadget resource",
	  1, 10000, cascadebg_init, NULL, gadget_set_run, NULL, destroy_work },
	{ "separatorg-get", "micro", "XtGetValues on a SeparatorGadget",
	  1, 100000, separatorg_init, NULL, separatorg_get_run, NULL, destroy_work },
	{ "separatorg-set", "micro", "XtSetValues of a cached SeparatorGadget resource",
	  1, 10000, separatorg_init, NULL, separatorg_set_run, NULL, destroy_work },
	{ "icong-get", "micro", "XtGetValues on an IconGadget",
	  1, 100000, icong_init, NULL, gadget_get_run, NULL, destroy_work },
	{ "icong-set", "micro", "XtSetValues of a cached IconGadget resource",
	  1, 10000, icong_init, NULL, gadget_set_run, NULL, destroy_work },
	{ "shell-get", "micro", "XtGetValues of a VendorShell extension resource",
	  1, 100000, NULL, NULL, shell_get_run, NULL, NULL },
	{ "gadget-cache", "micro", "create LabelGadgets, 200 distinct cache parts",
	  1, 10000, NULL, cache_setup, cache_run, destroy_work, NULL },
	{ "gadget-cache-distinct", "micro", "create LabelGadgets, all distinct cache parts",
	  1, 10000, NULL, cache_setup, cache_distinct_run, destroy_work, NULL },
	{ "gadget-cache-colors", "micro", "create ToggleButtonGadgets, all distinct colors",
	  1, 2000, NULL, cache_setup, cache_colors_run, destroy_work, NULL },
	{ "xmstring-create", "micro", "XmStringCreateLocalized + XmStringFree",
	  0, 200000, NULL, NULL, xs_create_run, NULL, NULL },
	{ "xmstring-concat", "micro", "XmStringConcatAndFree, one segment at a time",
	  0, 5000, NULL, NULL, xs_concat_run, NULL, NULL },
	{ "xmstring-extent", "micro", "XmStringExtent, one segment, core font",
	  1, 200000, xs_init, NULL, xs_extent_run, NULL, xs_fini },
	{ "xmstring-extent-multi", "micro", "XmStringExtent, 3 lines with a tab",
	  1, 100000, xs_init, NULL, xs_extent_multi_run, NULL, xs_fini },
	{ "xmstring-extent-xft", "micro", "XmStringExtent, one segment, Xft",
	  1, 100000, xs_init, NULL, xs_extent_xft_run, NULL, xs_fini },
	{ "xmstring-extent-fontset", "micro", "XmStringExtent, one segment, font set",
	  1, 100000, xs_init, NULL, xs_extent_fs_run, NULL, xs_fini },
	{ "xmstring-draw", "micro", "XmStringDraw, core font",
	  1, 20000, xs_init, NULL, xs_draw_run, NULL, xs_fini },
	{ "xmstring-draw-xft", "micro", "XmStringDraw, Xft",
	  1, 20000, xs_init, NULL, xs_draw_xft_run, NULL, xs_fini },
	{ "xmstring-draw-fontset", "micro", "XmStringDraw, font set",
	  1, 20000, xs_init, NULL, xs_draw_fs_run, NULL, xs_fini },
	{ "rendertable-cvt", "micro", "create Labels with a String render table",
	  1, 1000, NULL, rt_setup, rt_run, destroy_work, NULL },
	{ "shadow-2", "micro", "XmeDrawShadows, thickness 2",
	  1, 200000, shadow_init, NULL, shadow2_run, NULL, shadow_fini },
	{ "shadow-8", "micro", "XmeDrawShadows, thickness 8",
	  1, 200000, shadow_init, NULL, shadow8_run, NULL, shadow_fini },
	{ "visibility", "micro", "XmGetVisibility with 50 siblings above",
	  1, 2000, vis_init, NULL, vis_run, NULL, destroy_work },
	{ "spot-same", "micro", "XmImVaSetValues(XmNspotLocation), same spot",
	  1, 100000, spot_init, NULL, spot_same_run, NULL, spot_fini },
	{ "text-cursor", "micro", "XmTextSetInsertionPosition along the text",
	  1, 20000, spot_init, NULL, text_cursor_run, NULL, spot_fini },
	{ "text-append", "macro", "XmTextInsert 1 KB at the end, 10 MB total",
	  1, 10000, NULL, text_append_setup, text_append_run, text_teardown, NULL },
	{ "text-type", "macro", "XmTextInsert 1 char at the middle of 1 MB",
	  1, 100000, NULL, text_type_setup, text_type_run, text_teardown, NULL },
	{ "text-insdel", "macro", "insert/delete 2 chars at a buffer boundary",
	  1, 20000, NULL, text_insdel_setup, text_insdel_run, text_teardown, NULL },
	{ "scrollbar-repeat", "macro", "ScrollBar arrow autorepeat ticks",
	  1, 200, sb_init, sb_setup, sb_run, NULL, destroy_work },
	{ "menu-post", "macro", "popup menu post + unpost",
	  1, 500, menu_init, NULL, menu_run, NULL, destroy_work },
	{ "rc-buttons", "macro", "RowColumn of PushButtons: create + manage",
	  1, 10000, NULL, rc_setup, rc_buttons_run, destroy_work, NULL },
	{ "rc-gadgets", "macro", "RowColumn of PushButtonGadgets: create + manage",
	  1, 10000, NULL, rc_setup, rc_gadgets_run, destroy_work, NULL },
	{ "rc-gadgets-destroy", "macro", "destroy a RowColumn of PushButtonGadgets",
	  1, 10000, NULL, rc_destroy_setup, destroy_work_run, NULL, NULL },
	{ "form-chain", "macro", "Form, chained children: create + manage",
	  1, 1000, NULL, form_setup, form_run, destroy_work, NULL },
	{ "form-chain-destroy", "macro", "destroy a Form with chained children",
	  1, 1000, NULL, form_destroy_setup, destroy_work_run, NULL, NULL },
	{ "container-icons", "macro", "Container of IconGadgets: create + manage",
	  1, 10000, NULL, container_setup, container_run, destroy_work, NULL },
	{ "list-add", "macro", "XmListAddItemUnselected at the end",
	  1, 100000, NULL, list_add_setup, list_add_run, list_teardown, NULL },
	{ "list-select", "macro", "XmListSelectItem in a 100k list",
	  1, 1000, NULL, list_fill_setup, list_select_run, list_teardown, NULL },
	{ "list-delete", "macro", "XmListDeleteItem in a 100k list",
	  1, 1000, NULL, list_fill_setup, list_delete_run, list_teardown, NULL },
	{ "list-pagedown", "macro", "ListNextPage in a 100k list",
	  1, 1000, NULL, list_fill_setup, list_pagedown_run, list_teardown, NULL },
	{ "xft-labels", "macro", "expose Xft Labels in their own windows",
	  1, 2000, NULL, xft_setup, xft_run, destroy_work, NULL },
};

#define N_CASES (sizeof cases / sizeof cases[0])

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

static void usage(FILE *f)
{
	fprintf(f, "usage: xmbench [-r REPEAT] [-s SCALE] [-j FILE] [-t] [-l] "
		   "[CASE|micro|macro|all ...]\n");
}

static int selected(const struct bench_case *bc, int argc, char **argv,
		    int first)
{
	int i;

	if (first >= argc)
		return 1;
	for (i = first; i < argc; i++)
		if (!strcmp(argv[i], "all") || !strcmp(argv[i], bc->name) ||
		    !strcmp(argv[i], bc->group))
			return 1;
	return 0;
}

static void open_display(int *argc, char **argv)
{
	XtSetLanguageProc(NULL, NULL, NULL);
	top = XtVaOpenApplication(&app, "XmBench", NULL, 0, argc, argv, NULL,
				  applicationShellWidgetClass,
				  XmNallowShellResize, False,
				  XmNwidth, 800, XmNheight, 600, NULL);
	dpy = XtDisplay(top);
	root = XtVaCreateManagedWidget("root", xmBulletinBoardWidgetClass, top,
				       XmNresizePolicy, XmRESIZE_NONE,
				       XmNwidth, 800, XmNheight, 600, NULL);
	XtRealizeWidget(top);
	drain();
}

int main(int argc, char **argv)
{
	int repeat = 5, opt, first, ran = 0, skipped = 0, want_x = 0;
	int threads = 0;
	double scale = 1.0;
	const char *json = NULL;
	FILE *jf = NULL;
	size_t i;

	preload_self(argv);
	while ((opt = getopt(argc, argv, "r:s:j:tlh")) != -1) {
		switch (opt) {
		case 'r':
			repeat = atoi(optarg);
			if (repeat < 1 || repeat > MAX_REPEAT) {
				fprintf(stderr, "xmbench: bad repeat count\n");
				return 1;
			}
			break;
		case 's':
			scale = atof(optarg);
			if (!(scale > 0)) {
				fprintf(stderr, "xmbench: bad scale\n");
				return 1;
			}
			break;
		case 'j':
			json = optarg;
			break;
		case 't':
			threads = 1;
			break;
		case 'l':
			for (i = 0; i < N_CASES; i++)
				printf("%-24s %-6s %s\n", cases[i].name,
				       cases[i].group, cases[i].desc);
			return 0;
		case 'h':
			usage(stdout);
			return 0;
		default:
			usage(stderr);
			return 1;
		}
	}
	first = optind;
	for (opt = first; opt < argc; opt++) {
		int known = !strcmp(argv[opt], "all") ||
			    !strcmp(argv[opt], "micro") ||
			    !strcmp(argv[opt], "macro");

		for (i = 0; i < N_CASES && !known; i++)
			known = !strcmp(argv[opt], cases[i].name);
		if (!known) {
			fprintf(stderr, "xmbench: unknown case %s\n",
				argv[opt]);
			return 1;
		}
	}

	c_mallocs = dlsym(RTLD_DEFAULT, "xmbench_mallocs");
	c_replies = dlsym(RTLD_DEFAULT, "xmbench_replies");
	c_icvalues = dlsym(RTLD_DEFAULT, "xmbench_icvalues");
	if (!c_mallocs)
		fprintf(stderr, "xmbench: counting library not loaded, "
			"mallocs/rtrips/icvalues are not measured\n");

	for (i = 0; i < N_CASES; i++)
		if (cases[i].needs_x && selected(&cases[i], argc, argv, first))
			want_x = 1;
	if (threads && !XtToolkitThreadInitialize()) {
		fprintf(stderr, "xmbench: Xt has no thread support\n");
		return 1;
	}
	XtToolkitInitialize();
	if (want_x && getenv("DISPLAY") && *getenv("DISPLAY")) {
		int xargc = 1;

		open_display(&xargc, argv);
	}

	if (json) {
		jf = strcmp(json, "-") ? fopen(json, "w") : stdout;
		if (!jf) {
			fprintf(stderr, "xmbench: %s: %s\n", json,
				strerror(errno));
			return 1;
		}
		fprintf(jf, "{\n  \"bench\": \"xmbench\",\n  \"version\": 1,\n"
			"  \"repeat\": %d,\n  \"scale\": %g,\n"
			"  \"threads\": %s,\n"
			"  \"counters\": %s,\n  \"display\": %s,\n"
			"  \"cases\": [", repeat, scale,
			threads ? "true" : "false",
			c_mallocs ? "true" : "false",
			dpy ? "true" : "false");
	}
	printf("%-24s %9s %12s %12s %9s %9s %8s %8s\n", "case", "n", "ns/op",
	       "cpu-ns/op", "mallocs", "requests", "rtrips", "icvalues");
	for (i = 0; i < N_CASES; i++) {
		const struct bench_case *bc = &cases[i];
		struct result res;
		long n;

		if (!selected(bc, argc, argv, first))
			continue;
		n = (long)(bc->n * scale);
		if (n < 1)
			n = 1;
		if (bc->needs_x && !dpy) {
			printf("%-24s %9s\n", bc->name, "skipped (no display)");
			skipped++;
			continue;
		}
		run_case(bc, n, repeat, &res);
		printf("%-24s %9ld %12.1f %12.1f %9.3f %9.3f %8.3f %8.3f\n",
		       bc->name, n, res.ns, res.cpu, res.mallocs, res.requests,
		       res.rtrips, res.icvalues);
		fflush(stdout);
		if (jf)
			fprintf(jf, "%s\n    {\"name\": \"%s\", \"group\": \"%s\", "
				"\"n\": %ld, \"ns_per_op\": %.2f, "
				"\"cpu_ns_per_op\": %.2f, "
				"\"mallocs_per_op\": %.4f, "
				"\"requests_per_op\": %.4f, "
				"\"round_trips_per_op\": %.4f, "
				"\"icvalues_per_op\": %.4f, "
				"\"requests_per_run\": %g, "
				"\"round_trips_per_run\": %g}",
				ran ? "," : "", bc->name, bc->group, n, res.ns,
				res.cpu,
				res.mallocs, res.requests, res.rtrips,
				res.icvalues, res.run_requests, res.run_rtrips);
		ran++;
	}
	if (jf) {
		fprintf(jf, "\n  ]\n}\n");
		if (jf != stdout)
			fclose(jf);
	}
	free(kids);
	if (!ran && skipped)
		return EXIT_SKIP;
	return 0;
}
