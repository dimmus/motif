#define _GNU_SOURCE 1
/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * xm_layoutbench: wall-clock timings of the Form, Container and List
 * layout code with many children or items, and of the creation of
 * Labels whose render table is converted from a resource.
 *
 *   xm_layoutbench MODE N
 *
 * MODE is form, formgrid, outline, spatial, detail, fillhead,
 * fillrandom, list, listops or rendertable.
 *
 * Prints the time of each phase (create, manage, resize, destroy, ...)
 * to stdout.  With PROF=FILE it also samples the stack on SIGPROF and
 * writes "module+offset" frames to FILE.  Run it against two builds of
 * libXm with LD_LIBRARY_PATH to compare them; see README.md.
 */
#include <Xm/XmP.h>
#include <X11/CompositeP.h>
#include <Xm/Container.h>
#include <Xm/Form.h>
#include <Xm/IconG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <signal.h>
#include <sys/time.h>

/* PROF=1: sample the stack on SIGPROF and dump "module+offset" frames */
#define MAXS 200000
static void *samples[MAXS][6];
static int nsamples;
static int prof_phase = -1;
static int sample_phase[MAXS];
static void on_prof(int sig)
{
  void *buf[10];
  int n, i;
  if (nsamples >= MAXS)
    return;
  n = backtrace(buf, 10);
  for (i = 0; i < 6; i++)
    samples[nsamples][i] = (i + 2 < n) ? buf[i + 2] : NULL;
  sample_phase[nsamples] = prof_phase;
  nsamples++;
}
static void prof_start(void)
{
  struct itimerval it = {{0, 1000}, {0, 1000}};
  void *dummy[2];
  if (!getenv("PROF"))
    return;
  backtrace(dummy, 2);
  signal(SIGPROF, on_prof);
  setitimer(ITIMER_PROF, &it, NULL);
}
static void prof_dump(void)
{
  int i, j;
  FILE *f;
  if (!getenv("PROF"))
    return;
  f = fopen(getenv("PROF"), "w");
  for (i = 0; i < nsamples; i++) {
    fprintf(f, "%d", sample_phase[i]);
    for (j = 0; j < 6; j++) {
      Dl_info info;
      if (samples[i][j] && dladdr(samples[i][j], &info) && info.dli_fname)
        fprintf(f, " %s+0x%lx", strrchr(info.dli_fname, '/') ? strrchr(info.dli_fname, '/') + 1 : info.dli_fname,
                (unsigned long)((char *)samples[i][j] - (char *)info.dli_fbase) - 1);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

static XtAppContext app;
static Display *dpy;
static Widget top;
static double t0;

static double now(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}
static void settle(void)
{
  XSync(dpy, False);
  while (XtAppPending(app) & XtIMXEvent)
    XtAppProcessEvent(app, XtIMXEvent);
  XSync(dpy, False);
  while (XtAppPending(app) & XtIMXEvent)
    XtAppProcessEvent(app, XtIMXEvent);
}
static void start(void) { t0 = now(); prof_phase++; }
static void lap(const char *what)
{
  settle();
  printf("  %-36s %9.3f s  [phase %d]\n", what, now() - t0, prof_phase);
  fflush(stdout);
  t0 = now();
  prof_phase++;
}

static void form_bench(int n, int grid)
{
  Widget form, *kids = calloc(n, sizeof(Widget));
  int i, cols = grid ? 20 : 1;
  form = XmCreateForm(top, "form", NULL, 0);
  printf("form %s n=%d\n", grid ? "grid" : "column", n);
  start();
  for (i = 0; i < n; i++) {
    Arg a[12];
    int m = 0;
    int r = i / cols, c = i % cols;
    if (c == 0) {
      XtSetArg(a[m], XmNleftAttachment, XmATTACH_FORM), m++;
    }
    else {
      XtSetArg(a[m], XmNleftAttachment, XmATTACH_WIDGET), m++;
      XtSetArg(a[m], XmNleftWidget, kids[i - 1]), m++;
    }
    if (c == cols - 1) {
      XtSetArg(a[m], XmNrightAttachment, XmATTACH_FORM), m++;
    }
    if (r == 0) {
      XtSetArg(a[m], XmNtopAttachment, XmATTACH_FORM), m++;
    }
    else {
      XtSetArg(a[m], XmNtopAttachment, XmATTACH_WIDGET), m++;
      XtSetArg(a[m], XmNtopWidget, kids[i - cols]), m++;
    }
    kids[i] = XmCreateLabelGadget(form, "label", a, m);
  }
  XtManageChildren(kids, n);
  XtManageChild(form);
  lap("create + manage children");
  XtRealizeWidget(top);
  lap("realize (initial layout)");
  for (i = 0; i < 5; i++)
    XtVaSetValues(top, XmNwidth, 400 + 50 * i, XmNheight, 300 + 40 * i, NULL);
  lap("5 shell resizes");
  for (i = 0; i < 5; i++)
    XtVaSetValues(kids[n / 2 + i], XmNleftOffset, 3 + i, NULL);
  lap("5 constraint changes");
  for (i = 0; i < 20; i++)
    XtDestroyWidget(kids[n - 1 - i]);
  lap("destroy 20 children (last first)");
  XtDestroyWidget(form);
  lap("destroy form");
}

static void container_bench(int n, unsigned char layout)
{
  Widget sw, c;
  Widget *items = calloc(n, sizeof(Widget));
  int i;
  sw = XmCreateScrolledWindow(top, "sw", NULL, 0);
  c = XtVaCreateWidget("c", xmContainerWidgetClass, sw, XmNlayoutType, layout, NULL);
  printf("container layout=%s n=%d\n", layout == XmOUTLINE ? "outline" : layout == XmDETAIL ? "detail" : "spatial", n);
  start();
  for (i = 0; i < n; i++)
    items[i] = XtVaCreateWidget("i", xmIconGadgetClass, c, NULL);
  lap("create children (append)");
  XtManageChildren(items, n);
  XtManageChild(c);
  XtManageChild(sw);
  lap("manage children");
  XtRealizeWidget(top);
  lap("realize");
  for (i = 0; i < 200; i++)
    XtVaCreateManagedWidget("j", xmIconGadgetClass, c, XmNpositionIndex, 0, NULL);
  lap("insert 200 at the front");
  for (i = 0; i < 200; i++)
    XtVaCreateManagedWidget("j", xmIconGadgetClass, c, NULL);
  lap("append 200 managed");
  XtDestroyWidget(sw);
  lap("destroy");
}

/* Fill a Container with n icons, each inserted at the front or at a
 * random place among those already in, then read their positions. */
static void container_fill_bench(int n, int random_places)
{
  Widget sw, c;
  Widget *items = calloc(n, sizeof(Widget));
  unsigned int r = 12345;
  long sum = 0;
  int i, pos;
  sw = XmCreateScrolledWindow(top, "sw", NULL, 0);
  c = XtVaCreateWidget("c", xmContainerWidgetClass, sw, XmNlayoutType, XmOUTLINE, NULL);
  printf("container fill %s n=%d\n", random_places ? "random" : "head", n);
  start();
  for (i = 0; i < n; i++) {
    r ^= r << 13;
    r ^= r >> 17;
    r ^= r << 5;
    items[i] = XtVaCreateWidget("i", xmIconGadgetClass, c, XmNpositionIndex,
                                random_places ? (int)(r % (unsigned)(i + 1)) : 0, NULL);
  }
  lap(random_places ? "create children (random places)" : "create children (at the front)");
  XtManageChildren(items, n);
  XtManageChild(c);
  XtManageChild(sw);
  XtRealizeWidget(top);
  lap("manage and realize");
  for (i = 0; i < n; i++) {
    XtVaGetValues(items[i], XmNpositionIndex, &pos, NULL);
    sum += pos;
  }
  lap("read every XmNpositionIndex");
  if (sum != (long)n * (n - 1) / 2)
    printf("bad positions: sum %ld\n", sum);
  XtDestroyWidget(sw);
  lap("destroy");
  free(items);
}

static void list_bench(int n)
{
  Widget list;
  XmString *t = calloc(n, sizeof(XmString));
  int i;
  char buf[64];
  list = XmCreateScrolledList(top, "list", NULL, 0);
  XtVaSetValues(list, XmNvisibleItemCount, 20, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
  XtManageChild(list);
  XtRealizeWidget(top);
  printf("list n=%d\n", n);
  for (i = 0; i < n; i++) {
    snprintf(buf, sizeof(buf), "item number %d", i);
    t[i] = XmStringCreateLocalized(buf);
  }
  start();
  for (i = 0; i < n; i++)
    XmListAddItemUnselected(list, t[i], 0);
  lap("add items one by one (append)");
  for (i = 0; i < 2000; i++)
    XmListSelectPos(list, 1 + (int)((long)i * 7919 % n), False);
  lap("select 2000 by position");
  for (i = 0; i < 200; i++)
    XmListSelectItem(list, t[n - 1 - i * 13], False);
  lap("select 200 by value (near the end)");
  for (i = 0; i < 200; i++)
    if (!XmListItemExists(list, t[(i * 7) % n]))
      printf("missing\n");
  lap("200 XmListItemExists");
  for (i = 0; i < 200; i++)
    (void)XmListItemPos(list, t[n - 1 - i]);
  lap("200 XmListItemPos (near the end)");
  for (i = 0; i < 1000; i++)
    XmListSetPos(list, 1 + (i * 20) % (n - 20));
  lap("1000 page scrolls (XmListSetPos)");
  for (i = 0; i < 1000; i++)
    XmListSetPos(list, 1 + (n / 2) + (i % 2));
  lap("1000 one-line scrolls");
  /* the same, waiting for the X server to draw each one */
  for (i = 0; i < 1000; i++) {
    XmListSetPos(list, 1 + (n / 2) + (i % 2));
    XSync(dpy, False);
  }
  lap("1000 one-line scrolls, synced");
  {
    /* the location cursor at the bottom row, then down and up a line */
    XKeyEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = KeyPress;
    ev.display = dpy;
    ev.window = None; /* as from an accelerator: without the focus */
    XmListSetPos(list, 1);
    XmListSetKbdItemPos(list, 20);
    settle();
    start();
    for (i = 0; i < 1000; i++) {
      ev.time += 100;
      XtCallActionProc(list, "ListNextItem", (XEvent *)&ev, NULL, 0);
      XSync(dpy, False);
    }
    lap("1000 keyboard line scrolls, synced");
  }
  {
    Widget vsb = NULL;
    int value, size, inc, page;
    XtVaGetValues(XtParent(list), XmNverticalScrollBar, &vsb, NULL);
    XmListSetPos(list, 1);
    settle();
    start();
    for (i = 0; i < 2000; i++) {
      XmScrollBarGetValues(vsb, &value, &size, &inc, &page);
      XmScrollBarSetValues(vsb, value + ((i / 500) % 2 ? -1 : 1), size, inc, page, True);
      XSync(dpy, False);
    }
    lap("2000 scrollbar line steps");
    for (i = 0; i < 1000; i++) {
      XmScrollBarGetValues(vsb, &value, &size, &inc, &page);
      XmScrollBarSetValues(vsb, value + ((i / 250) % 2 ? -page : page), size, inc, page, True);
      XSync(dpy, False);
    }
    lap("1000 scrollbar page steps");
  }
  for (i = 0; i < 1000; i++)
    XmListDeletePos(list, n / 2);
  lap("delete 1000 from the middle");
  XmListDeleteAllItems(list);
  lap("delete all");
  start();
  XtVaSetValues(list, XmNitems, t, XmNitemCount, n, NULL);
  lap("XmNitems set all");
  XmListDeleteAllItems(list);
  XmListAddItems(list, t, n, 0);
  lap("XmListAddItems all + delete all");
}

/* A pseudo-random sequence that does not depend on the C library. */
static unsigned int lcg_state = 12345;
static int rnd(int n)
{
  lcg_state = lcg_state * 1103515245u + 12345u;
  return (int)((lcg_state >> 8) % (unsigned int)n);
}

/* The XmList operations on many items: adds, lookups, selection,
 * deletes and replacements by value and by position. */
static void listops_bench(int n)
{
  Widget list;
  XmString *t = calloc(n, sizeof(XmString));
  XmString *u = calloc(n, sizeof(XmString));
  XmString *sel;
  int *pos = calloc(1000, sizeof(int));
  int i, k, count;
  char buf[64];
  XKeyEvent ev;
  list = XmCreateScrolledList(top, "list", NULL, 0);
  XtVaSetValues(list, XmNvisibleItemCount, 20, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
  XtManageChild(list);
  XtRealizeWidget(top);
  printf("listops n=%d\n", n);
  for (i = 0; i < n; i++) {
    snprintf(buf, sizeof(buf), "item number %d", i);
    t[i] = XmStringCreateLocalized(buf);
    snprintf(buf, sizeof(buf), "other item %d", i);
    u[i] = XmStringCreateLocalized(buf);
  }
  memset(&ev, 0, sizeof(ev));
  ev.type = KeyPress;
  ev.display = dpy;
  ev.window = XtWindow(list);
  start();
  for (i = 0; i < n; i++)
    XmListAddItemUnselected(list, t[i], 0);
  lap("append one by one");
  XmListDeleteAllItems(list);
  lap("delete all");
  for (i = 0; i < n; i++)
    XmListAddItemUnselected(list, t[n - 1 - i], 1);
  lap("add one by one at the top");
  for (i = 0; i < 10000; i++)
    if (XmListItemPos(list, t[rnd(n)]) == 0)
      printf("missing\n");
  lap("10000 XmListItemPos");
  for (i = 0; i < 10000; i++)
    XmListSelectItem(list, t[rnd(n)], False);
  lap("10000 select by value");
  for (i = 0; i < 10000; i++)
    XmListSelectPos(list, 1 + rnd(n), False);
  lap("10000 select by position");
  for (i = 0; i < 5000; i++)
    XmListDeselectPos(list, 1 + rnd(n));
  lap("5000 deselect by position");
  for (i = 0; i < 1000; i++)
    XtCallActionProc(list, "ListNextPage", (XEvent *)&ev, NULL, 0);
  lap("1000 page-downs");
  for (i = 0; i < 1000; i++)
    XtCallActionProc(list, "ListPrevPage", (XEvent *)&ev, NULL, 0);
  lap("1000 page-ups");
  XmListAddItems(list, u, n / 10, n / 2);
  lap("add n/10 checking the selection");
  for (i = 0; i < 1000; i++)
    XmListDeleteItem(list, t[rnd(n)]);
  lap("1000 delete by value");
  for (i = 0; i < 1000; i++) {
    XtVaGetValues(list, XmNitemCount, &count, NULL);
    XmListDeletePos(list, count / 2);
  }
  lap("1000 delete from the middle");
  for (i = 0; i < 1000; i++) {
    XtVaGetValues(list, XmNitemCount, &count, NULL);
    pos[i] = 1 + rnd(count);
  }
  XmListDeletePositions(list, pos, 1000);
  lap("XmListDeletePositions 1000");
  for (i = 0; i < 1000; i++) {
    k = rnd(n);
    XmListReplaceItems(list, &t[k], 1, &u[k]);
  }
  lap("1000 XmListReplaceItems");
  XtVaGetValues(list, XmNselectedItems, &sel, XmNselectedItemCount, &count, NULL);
  printf("  (%d selected)\n", count);
  start();
  XmListDeleteItems(list, sel, count < 2000 ? count : 2000);
  lap("XmListDeleteItems 2000 selected");
  XtVaSetValues(list, XmNselectedItems, t, XmNselectedItemCount, n / 10, NULL);
  lap("XmNselectedItems n/10");
  for (i = 0; i < 1000; i++)
    XmListDeselectItem(list, t[rnd(n / 10)]);
  lap("1000 deselect by value");
  {
    /* 300 ever wider items, deleted from the widest: each time the
     * list shrinks to the next one. */
    char wide[400];
    XmString s;
    for (i = 0; i < 300; i++) {
      memset(wide, '#', 40 + i);
      wide[40 + i] = '\0';
      s = XmStringCreateLocalized(wide);
      XmListAddItemUnselected(list, s, 1 + rnd(1000));
      XmStringFree(s);
    }
    start();
    for (i = 299; i >= 0; i--) {
      memset(wide, '#', 40 + i);
      wide[40 + i] = '\0';
      s = XmStringCreateLocalized(wide);
      XmListDeleteItem(list, s);
      XmStringFree(s);
    }
    lap("300 deletes of the widest by value");
  }
}

/*
 * N Labels whose XmNrenderTable comes from the same resource, for render
 * tables of a font list, of one and three renditions from the resource
 * database, and of an Xft rendition.
 */
static void rendertable_bench(int n)
{
  static const char *db[] = {
      "*core.fontName: -*-fixed-medium-r-normal--13-*-*-*-*-*-*-*",
      "*core.fontType: FONT_IS_FONT",
      "*bold.fontName: -*-fixed-bold-r-normal--13-*-*-*-*-*-*-*",
      "*bold.fontType: FONT_IS_FONT",
      "*red.renditionForeground: red",
      "*red.underlineType: SINGLE_LINE",
      "*xft.fontName: Sans",
      "*xft.fontType: FONT_IS_XFT",
      "*xft.fontSize: 12",
      "*fontlist*l.renderTable: -*-fixed-medium-r-normal--13-*-*-*-*-*-*-*",
      "*one*l.renderTable: core",
      "*three*l.renderTable: core bold red",
      "*xft*l.renderTable: xft",
  };
  static const char *cases[] = {"fontlist", "one", "three", "xft"};
  XrmDatabase rdb = XtScreenDatabase(XtScreen(top));
  unsigned int i, c;
  int j;
  for (i = 0; i < XtNumber(db); i++)
    XrmPutLineResource(&rdb, db[i]);
  printf("rendertable n=%d\n", n);
  for (c = 0; c < XtNumber(cases); c++) {
    char what[64];
    Widget rc = XtVaCreateWidget(cases[c], xmRowColumnWidgetClass, top, NULL), l = NULL;
    XmRenderTable rt = NULL;
    XmStringTag *tags = NULL;
    int ntags;
    start();
    for (j = 0; j < n; j++)
      l = XtVaCreateWidget("l", xmLabelWidgetClass, rc, NULL);
    XtVaGetValues(l, XmNrenderTable, &rt, NULL);
    ntags = XmRenderTableGetTags(rt, &tags);
    snprintf(what, sizeof(what), "create labels (%s, %d renditions)", cases[c], ntags);
    while (ntags > 0)
      XtFree(tags[--ntags]);
    XtFree((char *)tags);
    lap(what);
    XtDestroyWidget(rc);
    lap("destroy");
  }
}

int main(int argc, char **argv)
{
  const char *mode = argc > 1 ? argv[1] : "form";
  int n = argc > 2 ? atoi(argv[2]) : 1000;
  int fake_argc = 1;
  char *fake_argv[] = {"bench", NULL};
  top = XtVaOpenApplication(&app, "Bench", NULL, 0, &fake_argc, fake_argv, NULL,
                            applicationShellWidgetClass, XmNallowShellResize, True, NULL);
  dpy = XtDisplay(top);
  prof_start();
  atexit(prof_dump);
  if (!strcmp(mode, "form"))
    form_bench(n, 0);
  else if (!strcmp(mode, "formgrid"))
    form_bench(n, 1);
  else if (!strcmp(mode, "outline"))
    container_bench(n, XmOUTLINE);
  else if (!strcmp(mode, "spatial"))
    container_bench(n, XmSPATIAL);
  else if (!strcmp(mode, "detail"))
    container_bench(n, XmDETAIL);
  else if (!strcmp(mode, "fillhead"))
    container_fill_bench(n, 0);
  else if (!strcmp(mode, "fillrandom"))
    container_fill_bench(n, 1);
  else if (!strcmp(mode, "list"))
    list_bench(n);
  else if (!strcmp(mode, "listops"))
    listops_bench(n);
  else if (!strcmp(mode, "rendertable"))
    rendertable_bench(n);
  return 0;
}
