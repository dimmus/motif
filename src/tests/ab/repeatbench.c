/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * xm_repeatbench: the ScrollBar arrow autorepeat, timed.
 *
 *   xm_repeatbench [HOLD_MS [WORK [RUNS]]]
 *
 * Holds the increment arrow of a ScrollBar down for HOLD_MS (default
 * 3000) RUNS times (default 3).  The button is pressed and released
 * with XTest through a second connection to $DRIVER_DISPLAY (default:
 * $DISPLAY), so that the application's connection can go through a slow
 * link (see latency.sh) while the "user" sits at the server.  Each
 * repeat copies an 800x600 area WORK times (default 0) in a
 * DrawingArea, as a scrolled view would redraw.
 *
 * For each run it prints the repeats while the button was down, the
 * steady repeat rate (from 400 ms after the press), and, relative to
 * the release: when the application saw it, when the server had drawn
 * everything (an XSync from the application), and the last repeat.
 * The last line has the medians.
 */
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/ScrollBar.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_TICKS 4096
#define MAX_RUNS 64

static XtAppContext app;
static Widget sb, da;
static GC gc;
static int work;
static double ticks[MAX_TICKS];
static int nticks;
static double t_seen, t_settled;

static double now(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
}

static int by_value(const void *a, const void *b)
{
  double x = *(const double *)a, y = *(const double *)b;
  return (x > y) - (x < y);
}

static double median(double *v, int n)
{
  qsort(v, n, sizeof(*v), by_value);
  return n ? (n % 2 ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2) : 0;
}

static void repeat(Widget w, XtPointer client, XtPointer call)
{
  int i;
  if (nticks < MAX_TICKS)
    ticks[nticks++] = now();
  for (i = 0; i < work; i++)
    XCopyArea(XtDisplay(da), XtWindow(da), XtWindow(da), gc, i & 1, 0, 799, 600, !(i & 1), 0);
}

static void settled(XtPointer client, XtIntervalId *id)
{
  XSync(XtDisplay(sb), False);
  t_settled = now();
}

static void released(Widget w, XtPointer client, XEvent *event, Boolean *cont)
{
  if (event->type == ButtonRelease && t_seen == 0) {
    t_seen = now();
    XtAppAddTimeOut(app, 0, settled, NULL);
  }
}

/* The user: press, hold, release; report the times through fd. */
static void drive(int x, int y, int hold, int fd)
{
  const char *name = getenv("DRIVER_DISPLAY");
  Display *d = XOpenDisplay(name ? name : getenv("DISPLAY"));
  double t[2];
  if (!d)
    _exit(1);
  XTestFakeMotionEvent(d, -1, x, y, 0);
  XSync(d, False);
  usleep(50000);
  t[0] = now();
  XTestFakeButtonEvent(d, 1, True, 0);
  XSync(d, False);
  usleep(hold * 1000);
  t[1] = now();
  XTestFakeButtonEvent(d, 1, False, 0);
  XSync(d, False);
  if (write(fd, t, sizeof(t)) != sizeof(t))
    _exit(1);
  _exit(0);
}

static void run_for(double ms)
{
  double end = now() + ms;
  while (now() < end) {
    if (XtAppPending(app))
      XtAppProcessEvent(app, XtIMAll);
    else
      usleep(1000);
  }
}

int main(int argc, char **argv)
{
  Widget top, form;
  int hold = argc > 1 ? atoi(argv[1]) : 3000;
  int runs = argc > 3 ? atoi(argv[3]) : 3;
  int r, one = 1;
  double rate[MAX_RUNS], seen[MAX_RUNS], settle[MAX_RUNS];
  Position x, y;
  XGCValues values;

  work = argc > 2 ? atoi(argv[2]) : 0;
  if (runs > MAX_RUNS)
    runs = MAX_RUNS;
  top = XtVaAppInitialize(&app, "RepeatBench", NULL, 0, &one, argv, NULL, XmNx, 0, XmNy, 0, NULL);
  form = XtVaCreateManagedWidget("form", xmFormWidgetClass, top, NULL);
  sb = XtVaCreateManagedWidget("sb", xmScrollBarWidgetClass, form, XmNorientation, XmVERTICAL,
                               XmNmaximum, 1000000, XmNsliderSize, 10, XmNwidth, 20, XmNheight,
                               600, XmNtopAttachment, XmATTACH_FORM, XmNleftAttachment,
                               XmATTACH_FORM, NULL);
  da = XtVaCreateManagedWidget("da", xmDrawingAreaWidgetClass, form, XmNwidth, 800, XmNheight,
                               600, XmNtopAttachment, XmATTACH_FORM, XmNleftAttachment,
                               XmATTACH_WIDGET, XmNleftWidget, sb, NULL);
  XtAddCallback(sb, XmNincrementCallback, repeat, NULL);
  XtAddEventHandler(sb, ButtonReleaseMask, False, released, NULL);
  XtRealizeWidget(top);
  values.graphics_exposures = False;
  gc = XCreateGC(XtDisplay(da), XtWindow(da), GCGraphicsExposures, &values);
  run_for(500);
  XtTranslateCoords(sb, 10, 600 - 8, &x, &y); /* the increment arrow */

  for (r = 0; r < runs; r++) {
    int fds[2], i, down = 0, after = 0, steady = 0, got = 0;
    double t[2], last;
    pid_t pid;
    nticks = 0;
    t_seen = t_settled = 0;
    XmScrollBarSetValues(sb, 0, 10, 1, 10, False);
    XSync(XtDisplay(sb), False);
    run_for(200);
    if (pipe(fds))
      return 1;
    if ((pid = fork()) == 0) {
      close(fds[0]);
      drive(x, y, hold, fds[1]);
    }
    close(fds[1]);
    while (!got) {
      struct timeval tv = {0, 1000};
      fd_set fs;
      FD_ZERO(&fs);
      FD_SET(fds[0], &fs);
      if (select(fds[0] + 1, &fs, NULL, NULL, &tv) > 0) {
        if (read(fds[0], t, sizeof(t)) != sizeof(t))
          return 1;
        got = 1;
      }
      while (XtAppPending(app))
        XtAppProcessEvent(app, XtIMAll);
    }
    waitpid(pid, NULL, 0);
    close(fds[0]);
    while (!t_settled)
      XtAppProcessEvent(app, XtIMAll);
    run_for(300);
    last = nticks ? ticks[nticks - 1] : t[0];
    for (i = 0; i < nticks; i++) {
      if (ticks[i] > t[1]) {
        after++;
        continue;
      }
      down++;
      if (ticks[i] >= t[0] + 400)
        steady++;
    }
    rate[r] = steady * 1000.0 / (t[1] - t[0] - 400);
    seen[r] = t_seen - t[1];
    settle[r] = t_settled - t[1];
    printf("run %d: %d repeats down, %d after, %.1f/s; release seen %+.1f ms, "
           "drawn %+.1f ms, last repeat %+.1f ms\n",
           r, down, after, rate[r], seen[r], settle[r], last - t[1]);
    fflush(stdout);
  }
  printf("median: %.1f/s; release seen %+.1f ms, drawn %+.1f ms\n", median(rate, runs),
         median(seen, runs), median(settle, runs));
  return 0;
}
