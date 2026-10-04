/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * xm_abtest: A/B harness for the Form, Container and List layout code.
 * Builds pseudo-random configurations, drives them through the API and
 * through real input (xdotool), and prints child geometry, selection
 * state, scroll state, callbacks and a hash of the window pixels after
 * every step.  Run the same seed against two builds of libXm and diff
 * the output; ab.sh does that.  See README.md in this directory.
 *
 *   xm_abtest MODE SEED [SIZE]
 *
 * MODE is form, formcyc, formgrid, formcolumn, formwide, container,
 * list or listscroll.  SIZE 0 (the default) lets the seed choose.
 * Setting ABT_DESTROY_UNMANAGED also destroys unmanaged Form children.
 */
#include <Xm/XmP.h>
#include <X11/CompositeP.h>
#include <Xm/Container.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/IconG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <X11/Xutil.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static XtAppContext app;
static Display *dpy;
static Widget top;
static unsigned int rng = 1;
static int step = 0;

static unsigned int rnd(void)
{
  rng ^= rng << 13;
  rng ^= rng >> 17;
  rng ^= rng << 5;
  return rng;
}
static int rn(int n) { return n > 0 ? (int)(rnd() % (unsigned)n) : 0; }
static int pick(int n, ...)
{
  va_list ap;
  int i, v = 0, k = rn(n);
  va_start(ap, n);
  for (i = 0; i <= k; i++)
    v = va_arg(ap, int);
  va_end(ap);
  return v;
}

static void settle(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    XSync(dpy, False);
    while (XtAppPending(app) & (XtIMXEvent | XtIMAlternateInput))
      XtAppProcessEvent(app, XtIMXEvent | XtIMAlternateInput);
    usleep(2000);
  }
}

static unsigned long long image_hash(Widget w)
{
  XImage *img;
  unsigned long long h = 1469598103934665603ULL;
  int x, y;
  if (!XtIsRealized(w) || XtWidth(w) == 0 || XtHeight(w) == 0)
    return 0;
  {
    XWindowAttributes wa;
    int rx, ry, ww, hh;
    Window ch;
    XGetWindowAttributes(dpy, XtWindow(w), &wa);
    XTranslateCoordinates(dpy, XtWindow(w), wa.root, 0, 0, &rx, &ry, &ch);
    ww = XtWidth(w);
    hh = XtHeight(w);
    if (rx + ww > WidthOfScreen(wa.screen))
      ww = WidthOfScreen(wa.screen) - rx;
    if (ry + hh > HeightOfScreen(wa.screen))
      hh = HeightOfScreen(wa.screen) - ry;
    if (ww <= 0 || hh <= 0)
      return 0;
    img = XGetImage(dpy, XtWindow(w), 0, 0, ww, hh, AllPlanes, ZPixmap);
  }
  if (!img)
    return 0;
  for (y = 0; y < img->height; y++)
    for (x = 0; x < img->width; x++) {
      h ^= (unsigned long long)XGetPixel(img, x, y);
      h *= 1099511628211ULL;
    }
  XDestroyImage(img);
  return h;
}

static void dump_geom(Widget w, int depth)
{
  Cardinal i;
  printf("%*s%s %d,%d %ux%u+%u %s\n", depth * 2, "", XtName(w), XtX(w), XtY(w), XtWidth(w),
         XtHeight(w), XtBorderWidth(w), XtIsManaged(w) ? "M" : "u");
  if (XtIsComposite(w) && !XmIsList(w) && !XmIsScrollBar(w)) {
    CompositeWidget cw = (CompositeWidget)w;
    for (i = 0; i < cw->composite.num_children; i++)
      dump_geom(cw->composite.children[i], depth + 1);
  }
}

static void checkpoint(const char *what, Widget root)
{
  settle();
  printf("=== step %d: %s\n", step++, what);
  dump_geom(root, 0);
  printf("pixels %016llx\n", image_hash(top));
  fflush(stdout);
}

static void xdo(const char *fmt, ...)
{
  char cmd[512];
  va_list ap;
  int n;
  va_start(ap, fmt);
  n = snprintf(cmd, sizeof(cmd), "xdotool ");
  vsnprintf(cmd + n, sizeof(cmd) - n, fmt, ap);
  va_end(ap);
  settle();
  usleep(250000); /* never a double click by accident */
  if (system(cmd) != 0)
    fprintf(stderr, "xdotool failed: %s\n", cmd);
  settle();
}

/* root coordinates of a point inside w */
static void root_xy(Widget w, int x, int y, int *rx, int *ry)
{
  Window child;
  XTranslateCoordinates(dpy, XtWindow(XtIsWidget(w) ? w : XtParent(w)),
                        DefaultRootWindow(dpy), x + (XtIsWidget(w) ? 0 : XtX(w)),
                        y + (XtIsWidget(w) ? 0 : XtY(w)), rx, ry, &child);
}

static void click(Widget w, int x, int y, const char *mods, int button)
{
  int rx, ry;
  /* xdo waits past the double click interval */
  root_xy(w, x, y, &rx, &ry);
  if (mods && *mods)
    xdo("mousemove %d %d keydown %s click %d keyup %s", rx, ry, mods, button, mods);
  else
    xdo("mousemove %d %d click %d", rx, ry, button);
}

static void focus(Widget w)
{
  XSetInputFocus(dpy, XtWindow(top), RevertToParent, CurrentTime);
  settle();
  XmProcessTraversal(w, XmTRAVERSE_CURRENT);
  settle();
}

/* ---------------------------------------------------------------- Form */

static const unsigned char att_types[] = {XmATTACH_NONE,
                                          XmATTACH_FORM,
                                          XmATTACH_OPPOSITE_FORM,
                                          XmATTACH_WIDGET,
                                          XmATTACH_OPPOSITE_WIDGET,
                                          XmATTACH_POSITION,
                                          XmATTACH_SELF};

static Widget *kids;
static int nkids;
static int *perm; /* acyclic attachments go to kids earlier in perm */
static int *rank;
static int cyclic;
static int fraction_base;

static Widget pick_target(int i)
{
  int tries;
  if (cyclic)
    return kids[rn(nkids)];
  for (tries = 0; tries < 8; tries++) {
    int j = rn(nkids);
    if (kids[j] && rank[j] < rank[i])
      return kids[j];
  }
  return NULL;
}

static void random_side(Arg *args, int *n, int i, int side)
{
  static String type_res[] = {XmNleftAttachment, XmNrightAttachment, XmNtopAttachment,
                                   XmNbottomAttachment};
  static String w_res[] = {XmNleftWidget, XmNrightWidget, XmNtopWidget, XmNbottomWidget};
  static String off_res[] = {XmNleftOffset, XmNrightOffset, XmNtopOffset, XmNbottomOffset};
  static String pos_res[] = {XmNleftPosition, XmNrightPosition, XmNtopPosition,
                                  XmNbottomPosition};
  unsigned char t = att_types[rn(7)];
  Widget target = NULL;
  if (t == XmATTACH_WIDGET || t == XmATTACH_OPPOSITE_WIDGET) {
    target = pick_target(i);
    if (!target)
      t = rn(2) ? XmATTACH_FORM : XmATTACH_NONE;
  }
  XtSetArg(args[*n], type_res[side], t), (*n)++;
  if (target)
    XtSetArg(args[*n], w_res[side], target), (*n)++;
  if (t == XmATTACH_POSITION)
    XtSetArg(args[*n], pos_res[side], rn(fraction_base + 1)), (*n)++;
  if (rn(3))
    XtSetArg(args[*n], off_res[side], rn(30) - 6), (*n)++;
}

static Widget make_kid(Widget form, int i, Boolean managed)
{
  Arg args[32];
  int n = 0, s;
  char name[32];
  Widget w;
  XmString str;
  for (s = 0; s < 4; s++)
    if (rn(5))
      random_side(args, &n, i, s);
  XtSetArg(args[n], XmNx, rn(200)), n++;
  XtSetArg(args[n], XmNy, rn(200)), n++;
  snprintf(name, sizeof(name), "k%d", i);
  switch (rn(3)) {
    case 0: {
      char text[40];
      int len = 1 + rn(20);
      memset(text, 'a' + rn(26), len);
      text[len] = 0;
      str = XmStringCreateLocalized(text);
      XtSetArg(args[n], XmNlabelString, str), n++;
      w = XmCreatePushButton(form, name, args, n);
      XmStringFree(str);
      break;
    }
    case 1: {
      char text[16];
      snprintf(text, sizeof(text), "g%d", i * 7);
      str = XmStringCreateLocalized(text);
      XtSetArg(args[n], XmNlabelString, str), n++;
      w = XmCreateLabelGadget(form, name, args, n);
      XmStringFree(str);
      break;
    }
    default:
      XtSetArg(args[n], XmNwidth, 5 + rn(80)), n++;
      XtSetArg(args[n], XmNheight, 5 + rn(60)), n++;
      XtSetArg(args[n], XmNborderWidth, rn(3)), n++;
      w = XmCreateDrawingArea(form, name, args, n);
      break;
  }
  if (managed)
    XtManageChild(w);
  return w;
}

static void form_test(int n_kids, int is_cyclic)
{
  Arg args[16];
  int n = 0, i, round;
  Widget form;
  XtWidgetGeometry pref;
  cyclic = is_cyclic;
  nkids = n_kids;
  kids = calloc(nkids, sizeof(Widget));
  perm = calloc(nkids, sizeof(int));
  rank = calloc(nkids, sizeof(int));
  for (i = 0; i < nkids; i++)
    perm[i] = i;
  for (i = nkids - 1; i > 0; i--) {
    int j = rn(i + 1), t = perm[i];
    perm[i] = perm[j];
    perm[j] = t;
  }
  for (i = 0; i < nkids; i++)
    rank[perm[i]] = i;
  fraction_base = (int[]){100, 7, 1000, 3}[rn(4)];
  XtSetArg(args[n], XmNfractionBase, fraction_base), n++;
  XtSetArg(args[n], XmNhorizontalSpacing, rn(10)), n++;
  XtSetArg(args[n], XmNverticalSpacing, rn(10)), n++;
  if (rn(2))
    XtSetArg(args[n], XmNmarginWidth, rn(10)), n++;
  if (rn(2))
    XtSetArg(args[n], XmNmarginHeight, rn(10)), n++;
  XtSetArg(args[n], XmNrubberPositioning, rn(2)), n++;
  XtSetArg(args[n], XmNresizePolicy, pick(3, XmRESIZE_ANY, XmRESIZE_GROW, XmRESIZE_NONE)),
      n++;
  XtSetArg(args[n], XmNlayoutDirection, rn(2) ? XmLEFT_TO_RIGHT : XmRIGHT_TO_LEFT), n++;
  XtSetArg(args[n], XmNshadowThickness, rn(3)), n++;
  form = XmCreateForm(top, "form", args, n);
  printf("form fb=%d\n", fraction_base);
  /* create in perm order so that some attach to later children */
  for (i = 0; i < nkids; i++) {
    int k = perm[i];
    kids[k] = NULL;
  }
  for (i = 0; i < nkids; i++) {
    int k = cyclic ? i : perm[i];
    kids[k] = make_kid(form, k, rn(6) != 0);
  }
  if (cyclic) {
    /* now that all exist, add attachments to anyone */
    for (i = 0; i < nkids; i++) {
      Arg a[16];
      int m = 0;
      random_side(a, &m, i, rn(4));
      XtSetValues(kids[i], a, m);
    }
  }
  XtManageChild(form);
  XtQueryGeometry(form, NULL, &pref);
  printf("prefer %ux%u\n", pref.width, pref.height);
  XtRealizeWidget(top);
  checkpoint("realize", top);
  for (round = 0; round < 6; round++) {
    int what = rn(7);
    switch (what) {
      case 0:
        XtVaSetValues(top, XmNwidth, 50 + rn(700), XmNheight, 50 + rn(600), NULL);
        checkpoint("resize shell", top);
        break;
      case 1:
        for (i = 0; i < 1 + nkids / 4; i++) {
          int k = rn(nkids);
          Arg a[16];
          int m = 0;
          if (!kids[k])
            continue;
          random_side(a, &m, k, rn(4));
          XtSetValues(kids[k], a, m);
        }
        checkpoint("constraints", top);
        break;
      case 2:
        for (i = 0; i < 1 + nkids / 4; i++) {
          int k = rn(nkids);
          if (kids[k] && XtIsWidget(kids[k]))
            XtVaSetValues(kids[k], XmNwidth, 3 + rn(120), XmNheight, 3 + rn(90), NULL);
        }
        checkpoint("child resize", top);
        break;
      case 3:
        for (i = 0; i < 1 + nkids / 4; i++) {
          int k = rn(nkids);
          if (!kids[k])
            continue;
          if (XtIsManaged(kids[k]))
            XtUnmanageChild(kids[k]);
          else
            XtManageChild(kids[k]);
        }
        checkpoint("manage", top);
        break;
      case 4:
        for (i = 0; i < 1 + nkids / 6; i++) {
          int k = rn(nkids);
          /* destroying an unmanaged child that others attach to leaves
           * dangling attachments in the old code */
          if (!kids[k] || (!XtIsManaged(kids[k]) && !getenv("ABT_DESTROY_UNMANAGED")))
            continue;
          XtDestroyWidget(kids[k]);
          kids[k] = NULL;
        }
        checkpoint("destroy", top);
        break;
      case 5:
        for (i = 0; i < 1 + nkids / 6; i++) {
          int k = rn(nkids);
          if (kids[k])
            continue;
          kids[k] = make_kid(form, k, True);
        }
        checkpoint("create", top);
        break;
      case 6: {
        XtWidgetGeometry want;
        want.request_mode = rn(2) ? CWWidth : (CWWidth | CWHeight);
        want.width = 10 + rn(500);
        want.height = 10 + rn(500);
        XtQueryGeometry(form, &want, &pref);
        printf("query %ux%u\n", pref.width, pref.height);
        break;
      }
    }
  }
  XtDestroyWidget(form);
  settle();
  printf("destroyed\n");
}

static void form_grid_test(int rows, int cols)
{
  Arg args[32];
  int n = 0, r, c, round, i;
  Widget form;
  Widget *grid;
  XtWidgetGeometry pref;
  int fb = pick(3, 100, cols, 1000);
  int rtol = rn(2);
  grid = calloc(rows * cols, sizeof(Widget));
  XtSetArg(args[n], XmNfractionBase, fb), n++;
  XtSetArg(args[n], XmNhorizontalSpacing, rn(6)), n++;
  XtSetArg(args[n], XmNverticalSpacing, rn(6)), n++;
  XtSetArg(args[n], XmNresizePolicy, pick(3, XmRESIZE_ANY, XmRESIZE_GROW, XmRESIZE_NONE)), n++;
  XtSetArg(args[n], XmNlayoutDirection, rtol ? XmRIGHT_TO_LEFT : XmLEFT_TO_RIGHT), n++;
  form = XmCreateForm(top, "form", args, n);
  printf("grid %dx%d fb=%d\n", rows, cols, fb);
  for (r = 0; r < rows; r++)
    for (c = 0; c < cols; c++) {
      char name[32];
      Widget w;
      n = 0;
      if (c == 0 || rn(5) == 0) {
        XtSetArg(args[n], XmNleftAttachment, rn(3) ? XmATTACH_FORM : XmATTACH_POSITION), n++;
        XtSetArg(args[n], XmNleftPosition, c * fb / cols), n++;
      }
      else {
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET), n++;
        XtSetArg(args[n], XmNleftWidget, grid[r * cols + c - 1]), n++;
      }
      switch (rn(4)) {
        case 0:
          XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION), n++;
          XtSetArg(args[n], XmNrightPosition, (c + 1) * fb / cols), n++;
          break;
        case 1:
          if (c == cols - 1)
            XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM), n++;
          break;
        case 2:
          if (r > 0) {
            XtSetArg(args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET), n++;
            XtSetArg(args[n], XmNrightWidget, grid[(r - 1) * cols + c]), n++;
          }
          break;
      }
      if (r == 0) {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM), n++;
      }
      else if (rn(6) == 0) {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION), n++;
        XtSetArg(args[n], XmNtopPosition, r * 100 / rows), n++;
      }
      else {
        XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET), n++;
        XtSetArg(args[n], XmNtopWidget, grid[(r - 1) * cols + c]), n++;
      }
      if (r == rows - 1 && rn(2))
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM), n++;
      if (rn(3) == 0)
        XtSetArg(args[n], XmNleftOffset, rn(8)), n++;
      if (rn(3) == 0)
        XtSetArg(args[n], XmNtopOffset, rn(8)), n++;
      snprintf(name, sizeof(name), "g%d_%d", r, c);
      if (rn(2)) {
        XmString s = XmStringCreateLocalized(name);
        XtSetArg(args[n], XmNlabelString, s), n++;
        w = rn(2) ? XmCreatePushButton(form, name, args, n) : XmCreateLabelGadget(form, name, args, n);
        XmStringFree(s);
      }
      else {
        XtSetArg(args[n], XmNwidth, 10 + rn(50)), n++;
        XtSetArg(args[n], XmNheight, 10 + rn(30)), n++;
        w = XmCreateDrawingArea(form, name, args, n);
      }
      grid[r * cols + c] = w;
    }
  XtManageChildren(grid, rows * cols);
  XtManageChild(form);
  XtQueryGeometry(form, NULL, &pref);
  printf("prefer %ux%u\n", pref.width, pref.height);
  XtRealizeWidget(top);
  checkpoint("realize", top);
  for (round = 0; round < 8; round++) {
    int k = rn(rows * cols);
    switch (rn(5)) {
      case 0:
        XtVaSetValues(top, XmNwidth, 100 + rn(900), XmNheight, 100 + rn(800), NULL);
        checkpoint("resize shell", top);
        break;
      case 1:
        if (XtIsWidget(grid[k]))
          XtVaSetValues(grid[k], XmNwidth, 5 + rn(150), XmNheight, 5 + rn(80), NULL);
        checkpoint("child resize", top);
        break;
      case 2:
        if (XtIsManaged(grid[k])) {
          XtUnmanageChild(grid[k]);
          checkpoint("unmanage", top);
          XtManageChild(grid[k]);
          checkpoint("manage", top);
        }
        break;
      case 3:
        XtVaSetValues(grid[k], XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, rn(fb), NULL);
        checkpoint("constraint", top);
        break;
      case 4:
        for (i = 0; i < rows * cols; i++)
          if (XtIsManaged(grid[i]) && rn(8) == 0) {
            XtDestroyWidget(grid[i]);
            grid[i] = XmCreateLabelGadget(form, "late", NULL, 0);
          }
        checkpoint("destroy", top);
        break;
    }
  }
  XtDestroyWidget(form);
  settle();
  printf("destroyed\n");
}

/* ----------------------------------------------------------- Container */

static void dump_container(Widget c)
{
  CompositeWidget cw = (CompositeWidget)c;
  WidgetList sel = NULL;
  int nsel = 0;
  Cardinal i;
  for (i = 0; i < cw->composite.num_children; i++) {
    Widget k = cw->composite.children[i];
    int pos = -2;
    unsigned char state = 0, vis = 0;
    if (!XmIsIconGadget(k))
      continue;
    XtVaGetValues(k, XmNpositionIndex, &pos, XmNoutlineState, &state, XmNvisualEmphasis, &vis,
                  NULL);
    printf("  %s pos=%d outline=%d emph=%d\n", XtName(k), pos, state, vis);
  }
  XtVaGetValues(c, XmNselectedObjects, &sel, XmNselectedObjectCount, &nsel, NULL);
  printf("  selected:");
  for (i = 0; i < (Cardinal)nsel; i++)
    printf(" %s", XtName(sel[i]));
  printf("\n");
}

static void container_test(int nitems)
{
  Arg args[16];
  int n = 0, i, round;
  Widget sw, c;
  Widget *items = calloc(nitems + 64, sizeof(Widget));
  int nmade = 0;
  unsigned char layout = pick(3, XmOUTLINE, XmSPATIAL, XmDETAIL);
  sw = XmCreateScrolledWindow(top, "sw", NULL, 0);
  XtSetArg(args[n], XmNlayoutType, layout), n++;
  XtSetArg(args[n], XmNselectionPolicy,
           pick(4, XmSINGLE_SELECT, XmBROWSE_SELECT, XmMULTIPLE_SELECT, XmEXTENDED_SELECT)),
      n++;
  if (layout == XmSPATIAL) {
    XtSetArg(args[n], XmNspatialStyle, pick(3, XmGRID, XmCELLS, XmNONE)), n++;
    XtSetArg(args[n], XmNspatialIncludeModel,
             pick(3, XmAPPEND, XmCLOSEST, XmFIRST_FIT)),
        n++;
  }
  XtSetArg(args[n], XmNentryViewType, rn(2) ? XmSMALL_ICON : XmLARGE_ICON), n++;
  XtSetArg(args[n], XmNautomaticSelection, rn(2) ? XmAUTO_SELECT : XmNO_AUTO_SELECT), n++;
  c = XmCreateContainer(sw, "cont", args, n);
  printf("container layout=%d\n", layout);
  for (i = 0; i < nitems; i++) {
    char name[32], text[32];
    XmString s;
    Arg a[8];
    int m = 0;
    snprintf(name, sizeof(name), "i%d", i);
    snprintf(text, sizeof(text), "item %d %.*s", i, rn(8), "xxxxxxxx");
    s = XmStringCreateLocalized(text);
    XtSetArg(a[m], XmNlabelString, s), m++;
    if (nmade > 0 && rn(3) == 0)
      XtSetArg(a[m], XmNentryParent, items[rn(nmade)]), m++;
    if (rn(4) == 0)
      XtSetArg(a[m], XmNpositionIndex, rn(4) == 0 ? XmLAST_POSITION : rn(nmade + 2)), m++;
    if (rn(3) == 0)
      XtSetArg(a[m], XmNoutlineState, XmEXPANDED), m++;
    items[nmade++] = XmCreateIconGadget(c, name, a, m);
    XtManageChild(items[nmade - 1]);
  }
  XtManageChild(c);
  XtManageChild(sw);
  XtVaSetValues(top, XmNwidth, 300 + rn(300), XmNheight, 200 + rn(300), NULL);
  XtRealizeWidget(top);
  checkpoint("realize", top);
  dump_container(c);
  for (round = 0; round < 8; round++) {
    int what = rn(7);
    switch (what) {
      case 0: {
        char name[32];
        XmString s;
        Arg a[8];
        int m = 0;
        snprintf(name, sizeof(name), "n%d", nmade);
        s = XmStringCreateLocalized(name);
        XtSetArg(a[m], XmNlabelString, s), m++;
        if (rn(2))
          XtSetArg(a[m], XmNentryParent, items[rn(nmade)]), m++;
        if (rn(2))
          XtSetArg(a[m], XmNpositionIndex, rn(3) == 0 ? XmLAST_POSITION : rn(nmade)), m++;
        items[nmade] = XmCreateIconGadget(c, name, a, m);
        if (items[nmade])
          XtManageChild(items[nmade]);
        nmade++;
        checkpoint("add", top);
        break;
      }
      case 1: {
        int k = rn(nmade);
        if (items[k] && !items[k]->core.being_destroyed) {
          XtDestroyWidget(items[k]);
          /* children of k die too: forget them */
          items[k] = NULL;
          settle();
          for (i = 0; i < nmade; i++) {
            CompositeWidget cw = (CompositeWidget)c;
            Cardinal j;
            Boolean found = False;
            for (j = 0; j < cw->composite.num_children; j++)
              if (cw->composite.children[j] == items[i])
                found = True;
            if (!found)
              items[i] = NULL;
          }
        }
        checkpoint("destroy", top);
        break;
      }
      case 2: {
        int k = rn(nmade);
        if (items[k])
          XtVaSetValues(items[k], XmNpositionIndex, rn(nmade), NULL);
        checkpoint("move", top);
        break;
      }
      case 3: {
        int k = rn(nmade);
        if (items[k])
          XtVaSetValues(items[k], XmNoutlineState, rn(2) ? XmEXPANDED : XmCOLLAPSED, NULL);
        checkpoint("outline", top);
        break;
      }
      case 4:
        XtVaSetValues(top, XmNwidth, 150 + rn(500), XmNheight, 150 + rn(400), NULL);
        checkpoint("resize", top);
        break;
      case 5:
      case 6: {
        int k = rn(nmade), j;
        if (!items[k] || !XtIsManaged(items[k]) || XtWidth(items[k]) == 0)
          break;
        focus(c);
        for (j = 0; j < 2; j++) {
          click(items[k], XtWidth(items[k]) / 2, XtHeight(items[k]) / 2,
                (const char *[]){"", "shift", "ctrl"}[rn(3)], 1);
          k = rn(nmade);
          if (!items[k] || !XtIsManaged(items[k]) || XtWidth(items[k]) == 0)
            break;
        }
        if (rn(2))
          xdo("key %s", (const char *[]){"Down", "Up", "space", "shift+Down", "ctrl+slash",
                                         "Next", "Home"}[rn(7)]);
        checkpoint("input", top);
        break;
      }
    }
    dump_container(c);
  }
  XtDestroyWidget(sw);
  settle();
  printf("destroyed\n");
}

/* ---------------------------------------------------------------- List */

static void list_cb(Widget w, XtPointer cd, XtPointer call)
{
  XmListCallbackStruct *cb = (XmListCallbackStruct *)call;
  int i;
  printf("  cb reason=%d pos=%d sel=%d [", cb->reason, cb->item_position,
         cb->selected_item_count);
  for (i = 0; i < cb->selected_item_count && cb->selected_item_positions; i++)
    printf(" %d", cb->selected_item_positions[i]);
  printf(" ] type=%d\n", cb->reason == XmCR_EXTENDED_SELECT ? cb->selection_type : -1);
}

static XmString mkitem(int i)
{
  char text[64];
  /* duplicates on purpose */
  int v = rn(4) == 0 ? rn(20) : i;
  snprintf(text, sizeof(text), "item %d%.*s", v, rn(30), "-----------------------------------");
  return XmStringCreateLocalized(text);
}

static void dump_sb(const char *name, Widget sb)
{
  int value, max, min, slider, inc, page;
  if (!sb || !XtIsManaged(sb)) {
    printf("  %s: none\n", name);
    return;
  }
  XtVaGetValues(sb, XmNvalue, &value, XmNmaximum, &max, XmNminimum, &min, XmNsliderSize, &slider,
                XmNincrement, &inc, XmNpageIncrement, &page, NULL);
  printf("  %s: v=%d [%d,%d] s=%d i=%d p=%d\n", name, value, min, max, slider, inc, page);
}

static void dump_list(Widget list)
{
  int count, top_pos, vis, nsel, *pos = NULL, npos = 0, i;
  XmStringTable sel;
  Widget hsb = NULL, vsb = NULL;
  XtVaGetValues(list, XmNitemCount, &count, XmNtopItemPosition, &top_pos,
                XmNvisibleItemCount, &vis, XmNselectedItemCount, &nsel, XmNselectedItems, &sel,
                NULL);
  XtVaGetValues(XtParent(list), XmNhorizontalScrollBar, &hsb, XmNverticalScrollBar, &vsb, NULL);
  printf("  count=%d top=%d vis=%d nsel=%d sel=[", count, top_pos, vis, nsel);
  if ((XtVaGetValues(list, XmNselectedPositions, &pos, XmNselectedPositionCount, &npos, NULL), npos > 0)) {
    for (i = 0; i < npos; i++)
      printf(" %d", pos[i]);
  }
  printf(" ] selitems=[");
  for (i = 0; i < nsel; i++) {
    char *t = NULL;
    t = (char *)XmStringUnparse(sel[i], NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0,
                                XmOUTPUT_ALL);
    printf(" '%s'", t ? t : "?");
    XtFree(t);
  }
  printf(" ]\n");
  dump_sb("hsb", hsb);
  dump_sb("vsb", vsb);
}

static int scroll_mode;

static void list_test(int nitems)
{
  Arg args[24];
  int n = 0, i, round;
  Widget list;
  XmString *tab;
  unsigned char policy =
      pick(4, XmSINGLE_SELECT, XmBROWSE_SELECT, XmMULTIPLE_SELECT, XmEXTENDED_SELECT);
  XtSetArg(args[n], XmNselectionPolicy, policy), n++;
  XtSetArg(args[n], XmNvisibleItemCount, 3 + rn(15)), n++;
  XtSetArg(args[n], XmNdoubleClickInterval, 150), n++;
  if (rn(3) == 0)
    XtSetArg(args[n], XmNhighlightThickness, rn(4)), n++;
  if (rn(3) == 0)
    XtSetArg(args[n], XmNshadowThickness, rn(4)), n++;
  XtSetArg(args[n], XmNlistSizePolicy, pick(3, XmCONSTANT, XmVARIABLE, XmRESIZE_IF_POSSIBLE)),
      n++;
  XtSetArg(args[n], XmNscrollBarDisplayPolicy, rn(2) ? XmSTATIC : XmAS_NEEDED), n++;
  if (rn(2))
    XtSetArg(args[n], XmNlistSpacing, rn(4)), n++;
  if (rn(2))
    XtSetArg(args[n], XmNlayoutDirection, XmRIGHT_TO_LEFT), n++;
  if (rn(3) == 0)
    XtSetArg(args[n], XmNselectionMode, XmADD_MODE), n++;
  if (rn(3) == 0)
    XtSetArg(args[n], XmNmatchBehavior, XmNONE), n++;
  tab = calloc(nitems, sizeof(XmString));
  for (i = 0; i < nitems; i++)
    tab[i] = mkitem(i);
  XtSetArg(args[n], XmNitems, tab), n++;
  XtSetArg(args[n], XmNitemCount, nitems), n++;
  list = XmCreateScrolledList(top, "list", args, n);
  for (i = 0; i < nitems; i++)
    XmStringFree(tab[i]);
  free(tab);
  XtAddCallback(list, XmNsingleSelectionCallback, list_cb, NULL);
  XtAddCallback(list, XmNbrowseSelectionCallback, list_cb, NULL);
  XtAddCallback(list, XmNmultipleSelectionCallback, list_cb, NULL);
  XtAddCallback(list, XmNextendedSelectionCallback, list_cb, NULL);
  XtAddCallback(list, XmNdefaultActionCallback, list_cb, NULL);
  XtManageChild(list);
  printf("list policy=%d\n", policy);
  XtRealizeWidget(top);
  checkpoint("realize", top);
  dump_list(list);
  for (round = 0; round < 40; round++) {
    int count, what = rn(24), k;
    if (scroll_mode && rn(3))
      what = pick(6, 22, 23, 22, 21, 11, 12); /* mostly scrolling */
    if (scroll_mode && rn(25) == 0) {
      /* partly off the screen: the copies then miss pixels */
      XtVaSetValues(top, XmNx, -rn(80), XmNy, -rn(80), NULL);
    }
    XmString s;
    XtVaGetValues(list, XmNitemCount, &count, NULL);
    switch (what) {
      case 0:
        s = mkitem(count);
        XmListAddItem(list, s, rn(count + 2));
        XmStringFree(s);
        break;
      case 1: {
        int m = 1 + rn(20);
        XmString *t = calloc(m, sizeof(XmString));
        for (k = 0; k < m; k++)
          t[k] = mkitem(count + k);
        if (rn(2))
          XmListAddItems(list, t, m, rn(count + 2));
        else
          XmListAddItemsUnselected(list, t, m, rn(count + 2));
        for (k = 0; k < m; k++)
          XmStringFree(t[k]);
        free(t);
        break;
      }
      case 2:
        s = mkitem(count);
        XmListAddItemUnselected(list, s, rn(count + 2));
        XmStringFree(s);
        break;
      case 3:
        if (count)
          XmListDeletePos(list, rn(count + 1));
        break;
      case 4:
        s = mkitem(rn(count + 1));
        XmListDeleteItem(list, s);
        XmStringFree(s);
        break;
      case 5: {
        int m = 1 + rn(5);
        int *p = calloc(m, sizeof(int));
        for (k = 0; k < m; k++)
          p[k] = 1 + rn(count + 1);
        XmListDeletePositions(list, p, m);
        free(p);
        break;
      }
      case 6:
        if (count)
          XmListDeleteItemsPos(list, 1 + rn(4), 1 + rn(count));
        break;
      case 7:
        XmListSelectPos(list, rn(count + 1), rn(2));
        break;
      case 8:
        s = mkitem(rn(count + 1));
        XmListSelectItem(list, s, rn(2));
        XmStringFree(s);
        break;
      case 9:
        XmListDeselectPos(list, rn(count + 1));
        break;
      case 10:
        if (rn(4) == 0)
          XmListDeselectAllItems(list);
        else
          XmListUpdateSelectedList(list);
        break;
      case 11:
        XmListSetPos(list, rn(count + 1));
        break;
      case 12:
        XmListSetBottomPos(list, rn(count + 1));
        break;
      case 13:
        XmListSetHorizPos(list, rn(200));
        break;
      case 14: {
        int m = 1 + rn(3);
        XmString *t = calloc(m, sizeof(XmString));
        for (k = 0; k < m; k++)
          t[k] = mkitem(rn(count + 10));
        XmListReplaceItemsPos(list, t, m, 1 + rn(count + 1));
        for (k = 0; k < m; k++)
          XmStringFree(t[k]);
        free(t);
        break;
      }
      case 15: {
        int pos;
        s = mkitem(rn(count + 1));
        printf("  exists=%d pos=%d\n", XmListItemExists(list, s), (pos = XmListItemPos(list, s)));
        {
          int *mp = NULL, mc = 0;
          if (XmListGetMatchPos(list, s, &mp, &mc)) {
            printf("  match:");
            for (k = 0; k < mc; k++)
              printf(" %d", mp[k]);
            printf("\n");
            XtFree((char *)mp);
          }
        }
        printf("  possel=%d\n", XmListPosSelected(list, 1 + rn(count + 1)));
        XmStringFree(s);
        break;
      }
      case 16:
        XtVaSetValues(top, XmNwidth, 60 + rn(500), XmNheight, 60 + rn(400), NULL);
        break;
      case 17: {
        /* only items the list has: with others, the old code leaves
         * uninitialised selected positions behind */
        int m = count ? rn(4) : 0;
        XmString *t = calloc(m + 1, sizeof(XmString)), *all = NULL;
        XtVaGetValues(list, XmNitems, &all, NULL);
        for (k = 0; k < m; k++)
          t[k] = XmStringCopy(all[rn(count)]);
        XtVaSetValues(list, XmNselectedItems, t, XmNselectedItemCount, m, NULL);
        for (k = 0; k < m; k++)
          XmStringFree(t[k]);
        free(t);
        break;
      }
      case 18:
      case 19:
      case 20: {
        /* real clicks on rows */
        Position hl;
        Dimension ht;
        int vis, row;
        XtVaGetValues(list, XmNvisibleItemCount, &vis, NULL);
        ht = XtHeight(list);
        row = rn(vis + 1);
        focus(list);
        click(list, 10 + rn(XtWidth(list) > 20 ? XtWidth(list) - 20 : 1),
              vis ? (int)(ht * row / (vis + 1)) + 3 : 5,
              (const char *[]){"", "", "shift", "ctrl"}[rn(4)], 1);
        if (rn(6) == 0) {
          int rx, ry;
          root_xy(list, 15, 5, &rx, &ry);
          xdo("mousemove %d %d click --repeat 2 --delay 30 1", rx, ry);
          usleep(300000);
        }
        else if (rn(4) == 0) {
          int rx, ry, rx2, ry2;
          root_xy(list, 15, 5, &rx, &ry);
          root_xy(list, 15, ht - 5, &rx2, &ry2);
          xdo("mousemove %d %d mousedown 1 mousemove %d %d mousemove %d %d mouseup 1", rx, ry, rx,
              (ry + ry2) / 2, rx2, ry2);
        }
        (void)hl;
        break;
      }
      case 21:
        focus(list);
        xdo("key %s", (const char *[]){"Down", "Up", "Next", "Prior", "ctrl+Home", "ctrl+End",
                                       "space", "shift+Down", "shift+Next", "Right", "Left",
                                       "ctrl+slash", "shift+F8", "Return"}[rn(14)]);
        break;
      case 22: {
        /* wheel and scrollbar */
        Widget vsb = NULL, hsb = NULL;
        XtVaGetValues(XtParent(list), XmNverticalScrollBar, &vsb, XmNhorizontalScrollBar, &hsb,
                      NULL);
        if (rn(2)) {
          int rx, ry;
          root_xy(list, XtWidth(list) / 2, XtHeight(list) / 2, &rx, &ry);
          xdo("mousemove %d %d click --repeat %d %d", rx, ry, 1 + rn(3), rn(2) ? 4 : 5);
        }
        else if (vsb && XtIsManaged(vsb) && XtIsRealized(vsb)) {
          int y = rn(XtHeight(vsb));
          click(vsb, XtWidth(vsb) / 2, y, "", 1);
        }
        else if (hsb && XtIsManaged(hsb) && XtIsRealized(hsb)) {
          click(hsb, rn(XtWidth(hsb)), XtHeight(hsb) / 2, "", 1);
        }
        break;
      }
      case 23: {
        int rx, ry;
        Widget vsb = NULL;
        XtVaGetValues(XtParent(list), XmNverticalScrollBar, &vsb, NULL);
        if (vsb && XtIsManaged(vsb) && XtIsRealized(vsb)) {
          /* drag the slider */
          int v, sz, max;
          XtVaGetValues(vsb, XmNvalue, &v, XmNsliderSize, &sz, XmNmaximum, &max, NULL);
          root_xy(vsb, XtWidth(vsb) / 2, 20 + (int)((long)XtHeight(vsb) * v / (max ? max : 1)), &rx,
                  &ry);
          xdo("mousemove %d %d mousedown 1 mousemove %d %d mousemove %d %d mouseup 1", rx, ry, rx,
              ry + 7, rx, ry + rn(60) - 20);
        }
        break;
      }
    }
    {
      char what_s[32];
      snprintf(what_s, sizeof(what_s), "op %d", what);
      checkpoint(what_s, top);
    }
    dump_list(list);
  }
}

static void quiet(String msg) { printf("  warning: %s\n", msg); }
static int xerr(Display *d, XErrorEvent *e)
{
  printf("  X error %d req %d.%d\n", e->error_code, e->request_code, e->minor_code);
  return 0;
}

int main(int argc, char **argv)
{
  const char *mode = argc > 1 ? argv[1] : "form";
  int seed = argc > 2 ? atoi(argv[2]) : 1;
  int size = argc > 3 ? atoi(argv[3]) : 0;
  int fake_argc = 1;
  char *fake_argv[] = {"abtest", NULL};
  rng = 2463534242u ^ (unsigned)(seed * 2654435761u);
  if (rng == 0)
    rng = 1;
  rnd();
  top = XtVaOpenApplication(&app, "Abtest", NULL, 0, &fake_argc, fake_argv, NULL,
                            applicationShellWidgetClass, XmNallowShellResize, True, NULL);
  dpy = XtDisplay(top);
  XtAppSetWarningHandler(app, quiet);
  XSetErrorHandler(xerr);
  if (!strcmp(mode, "form"))
    form_test(size ? size : 2 + rn(40), 0);
  else if (!strcmp(mode, "formcyc"))
    form_test(size ? size : 2 + rn(10), 1);
  else if (!strcmp(mode, "formgrid"))
    form_grid_test(1 + rn(12), 1 + rn(8));
  else if (!strcmp(mode, "formcolumn"))
    form_grid_test(size, 1);
  else if (!strcmp(mode, "formwide"))
    form_grid_test(size / 20, 20);
  else if (!strcmp(mode, "container"))
    container_test(size ? size : 1 + rn(40));
  else if (!strcmp(mode, "list"))
    list_test(size ? size : rn(4) == 0 ? rn(3) : rn(300));
  else if (!strcmp(mode, "listscroll")) {
    scroll_mode = 1;
    list_test(size ? size : 20 + rn(400));
  }
  printf("done\n");
  return 0;
}
