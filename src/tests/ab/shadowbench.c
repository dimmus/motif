/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * xm_shadowbench: compare XmeDrawShadows, which draws a shadow as one
 * line segment per pixel row or column, with other ways of drawing the
 * same pixels, for exactness and for speed.
 *
 *   xm_shadowbench check
 *   xm_shadowbench time [N]
 *
 * The other ways, which live only here:
 *   rects  XFillRectangles with the same rows and columns (1 pixel wide)
 *   poly   one 6-point XFillPolygon (Nonconvex) per GC, whose slanted
 *          edges are placed so that the X polygon fill rule picks
 *          exactly the pixels of the segments
 *   quads  the same as two convex 4-point polygons per GC
 *
 * "check" draws every shadow type with thicknesses 0 to 10 and several
 * sizes (some too small for the thickness) with each way, reads the
 * pixels back with XGetImage and counts the ones that differ from
 * XmeDrawShadows, with a solid GC, a stippled one, an XOR one, and GCs
 * with a line width of 1 and 2.  "time" draws N shadows of each
 * thickness with each way, and prints the time per shadow (including
 * the server's, as each run ends with XSync) and the request bytes.
 */
#include <Xm/XmP.h>
#include <Xm/DrawP.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static Display *dpy;
static Window root;
static int depth;

enum { W_SEGS, W_RECTS, W_POLY, W_QUADS, N_WAYS };
static const char *way_name[N_WAYS] = { "segments", "rects", "poly", "quads" };
static long req_bytes;

/* ---- the other ways ------------------------------------------------ */

/* The segments of DrawSimpleShadow, as rectangles of the same pixels. */
static void rect_shadow(Drawable d, GC top, GC bot, Position x, Position y, Dimension width,
                        Dimension height, Dimension t, Dimension cor)
{
  XSegment s[64];
  XRectangle r[64];
  int i, size2, size3;
  if (t > (width >> 1))
    t = width >> 1;
  if (t > (height >> 1))
    t = height >> 1;
  if (t <= 0)
    return;
  size2 = t << 1;
  size3 = size2 + t;
  for (i = 0; i < t; i++) {
    s[i].x1 = x;
    s[i].y2 = s[i].y1 = y + i;
    s[i].x2 = x + width - i - 1;
    s[i + t].x2 = s[i + t].x1 = x + i;
    s[i + t].y1 = y + t;
    s[i + t].y2 = y + height - i - 1;
    s[i + size2].x1 = x + i + ((cor) ? 0 : 1);
    s[i + size2].y2 = s[i + size2].y1 = y + height - i - 1;
    s[i + size2].x2 = x + width - 1;
    s[i + size3].x2 = s[i + size3].x1 = x + width - i - 1;
    s[i + size3].y1 = y + i + 1 - cor;
    s[i + size3].y2 = y + height - 1;
  }
  for (i = 0; i < 2 * size2; i++) {
    r[i].x = s[i].x1 < s[i].x2 ? s[i].x1 : s[i].x2;
    r[i].y = s[i].y1 < s[i].y2 ? s[i].y1 : s[i].y2;
    r[i].width = abs(s[i].x2 - s[i].x1) + 1;
    r[i].height = abs(s[i].y2 - s[i].y1) + 1;
  }
  XFillRectangles(dpy, d, top, r, size2);
  XFillRectangles(dpy, d, bot, r + size2, size2);
  req_bytes += 2 * (12 + 8 * size2);
}

static void poly_shadow(Drawable d, GC top, GC bot, Position x, Position y, Dimension w,
                        Dimension h, Dimension t, Dimension cor, int quads)
{
  XPoint p[6], q[4];
  if (t > w / 2)
    t = w / 2;
  if (t > h / 2)
    t = h / 2;
  if (t <= 0)
    return;
  /* The top and left: the slanted edges run one pixel outside the
   * centres of the last pixels of the rows and columns, which a point
   * on an edge with the inside to its left (or above) excludes. */
  p[0].x = x, p[0].y = y;
  p[1].x = x + w + 1, p[1].y = y;
  p[2].x = x + w + 1 - t, p[2].y = y + t;
  p[3].x = x + t, p[3].y = y + t;
  p[4].x = x + t, p[4].y = y + h + 1 - t;
  p[5].x = x, p[5].y = y + h + 1;
  if (quads) {
    q[0] = p[0], q[1] = p[1], q[2] = p[2];
    q[3].x = x, q[3].y = y + t;
    XFillPolygon(dpy, d, top, q, 4, Convex, CoordModeOrigin);
    q[0].x = x, q[0].y = y + t;
    q[1] = p[3], q[2] = p[4], q[3] = p[5];
    XFillPolygon(dpy, d, top, q, 4, Convex, CoordModeOrigin);
    req_bytes += 2 * (16 + 4 * 4);
  }
  else {
    XFillPolygon(dpy, d, top, p, 6, Nonconvex, CoordModeOrigin);
    req_bytes += 16 + 4 * 6;
  }
  /* The bottom and right: the slanted edges run through the centres of
   * the first pixels, which a point on an edge with the inside to its
   * right (or below) includes. */
  p[0].x = x + w, p[0].y = y + 1 - cor;
  p[1].x = x + w, p[1].y = y + h;
  p[2].x = x + 1 - cor, p[2].y = y + h;
  p[3].x = x + 1 - cor + t, p[3].y = y + h - t;
  p[4].x = x + w - t, p[4].y = y + h - t;
  p[5].x = x + w - t, p[5].y = y + 1 - cor + t;
  if (quads) {
    q[0] = p[0], q[1].x = x + w, q[1].y = y + h - t, q[2] = p[4], q[3] = p[5];
    XFillPolygon(dpy, d, bot, q, 4, Convex, CoordModeOrigin);
    q[0].x = x + w, q[0].y = y + h - t;
    q[1] = p[1], q[2] = p[2], q[3] = p[3];
    XFillPolygon(dpy, d, bot, q, 4, Convex, CoordModeOrigin);
    req_bytes += 2 * (16 + 4 * 4);
  }
  else {
    XFillPolygon(dpy, d, bot, p, 6, Nonconvex, CoordModeOrigin);
    req_bytes += 16 + 4 * 6;
  }
}

static void other_simple(int way, Drawable d, GC top, GC bot, Position x, Position y, Dimension w,
                         Dimension h, Dimension t, Dimension cor)
{
  if (way == W_SEGS) {
    /* Only count the bytes of what XmeDrawShadows sent. */
    if (t > w / 2)
      t = w / 2;
    if (t > h / 2)
      t = h / 2;
    if (t > 0)
      req_bytes += 2 * (12 + 8 * 2 * t);
  }
  else if (way == W_RECTS)
    rect_shadow(d, top, bot, x, y, w, h, t, cor);
  else
    poly_shadow(d, top, bot, x, y, w, h, t, cor, way == W_QUADS);
}

/* XmeDrawShadows with DrawSimpleShadow replaced by the other way. */
static void draw_shadow(int way, Drawable d, GC top, GC bot, Position x, Position y, Dimension w,
                        Dimension h, Dimension t, int type)
{
  GC tmp;
  if (way == W_SEGS)
    XmeDrawShadows(dpy, d, top, bot, x, y, w, h, t, type);
  if (type == XmSHADOW_IN || type == XmSHADOW_ETCHED_IN)
    tmp = top, top = bot, bot = tmp;
  if ((type == XmSHADOW_ETCHED_IN || type == XmSHADOW_ETCHED_OUT) && t != 1) {
    other_simple(way, d, top, bot, x, y, w, h, t / 2, 1);
    other_simple(way, d, bot, top, x + t / 2, y + t / 2, w - (t / 2) * 2, h - (t / 2) * 2, t / 2,
                 1);
  }
  else
    other_simple(way, d, top, bot, x, y, w, h, t, 0);
}

/* ---- check --------------------------------------------------------- */

#define PW 64
#define PH 48

static void fill_and_draw(int way, Pixmap pm, GC clear, GC top, GC bot, int w, int h, int t,
                          int type, XImage **img)
{
  XFillRectangle(dpy, pm, clear, 0, 0, PW, PH);
  draw_shadow(way, pm, top, bot, 3, 2, w, h, t, type);
  *img = XGetImage(dpy, pm, 0, 0, PW, PH, AllPlanes, ZPixmap);
}

static int check(void)
{
  static const int types[] = { XmSHADOW_IN, XmSHADOW_OUT, XmSHADOW_ETCHED_IN,
                               XmSHADOW_ETCHED_OUT };
  static const int sizes[][2] = { { 40, 30 }, { 21, 21 }, { 7, 30 }, { 30, 6 }, { 3, 3 },
                                  { 1, 9 }, { 20, 2 }, { 0, 5 } };
  static const char *gc_names[] = { "solid", "stippled", "xor", "line-width-1",
                                    "line-width-2" };
  static char stipple_bits[] = { 0x05, 0x0a, 0x05, 0x0a };
  Pixmap pm = XCreatePixmap(dpy, root, PW, PH, depth);
  Pixmap stipple = XCreateBitmapFromData(dpy, root, stipple_bits, 4, 4);
  GC clear = XCreateGC(dpy, pm, 0, NULL);
  int g, way, ti, si, t, x, y, failures = 0;
  XSetForeground(dpy, clear, 0x101010);
  for (g = 0; g < 5; g++) {
    XGCValues v;
    unsigned long mask = GCForeground | GCBackground;
    GC top, bot;
    long diff[N_WAYS] = { 0 };
    v.background = 0x0000ff;
    if (g == 1) {
      v.fill_style = FillOpaqueStippled, v.stipple = stipple;
      mask |= GCFillStyle | GCStipple;
    }
    if (g == 2)
      v.function = GXxor, mask |= GCFunction;
    if (g >= 3)
      v.line_width = g - 2, mask |= GCLineWidth;
    v.foreground = 0xe0e0e0;
    top = XCreateGC(dpy, pm, mask, &v);
    v.foreground = 0x606060;
    bot = XCreateGC(dpy, pm, mask, &v);
    for (ti = 0; ti < 4; ti++)
      for (si = 0; si < (int)(sizeof sizes / sizeof sizes[0]); si++)
        for (t = 0; t <= 10; t++) {
          XImage *ref, *img;
          fill_and_draw(W_SEGS, pm, clear, top, bot, sizes[si][0], sizes[si][1], t, types[ti],
                        &ref);
          for (way = W_RECTS; way < N_WAYS; way++) {
            fill_and_draw(way, pm, clear, top, bot, sizes[si][0], sizes[si][1], t, types[ti],
                          &img);
            for (y = 0; y < PH; y++)
              for (x = 0; x < PW; x++)
                if (XGetPixel(ref, x, y) != XGetPixel(img, x, y)) {
                  diff[way]++;
                  if (getenv("VERBOSE"))
                    printf("%s %s type %d size %dx%d t %d: %d,%d\n", gc_names[g], way_name[way],
                           types[ti], sizes[si][0], sizes[si][1], t, x, y);
                }
            XDestroyImage(img);
          }
          XDestroyImage(ref);
        }
    printf("%-13s", gc_names[g]);
    for (way = W_RECTS; way < N_WAYS; way++) {
      printf("  %s: %ld pixels differ", way_name[way], diff[way]);
      if (g < 3 && diff[way])
        failures++;
    }
    printf("\n");
    XFreeGC(dpy, top);
    XFreeGC(dpy, bot);
  }
  XFreeGC(dpy, clear);
  XFreePixmap(dpy, stipple);
  XFreePixmap(dpy, pm);
  return failures != 0;
}

/* ---- time ---------------------------------------------------------- */

static double now(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static int cmp_double(const void *a, const void *b)
{
  double x = *(const double *)a, y = *(const double *)b;
  return (x > y) - (x < y);
}

static void time_ways(long n)
{
  static const int thick[] = { 1, 2, 4, 8, 10 };
  Pixmap pm = XCreatePixmap(dpy, root, 400, 200, depth);
  GC top = XCreateGC(dpy, pm, 0, NULL), bot = XCreateGC(dpy, pm, 0, NULL);
  int ti, way, r;
  XSetForeground(dpy, top, 0xe0e0e0);
  XSetForeground(dpy, bot, 0x606060);
  printf("%-6s %-9s %10s %10s\n", "thick", "way", "ns/shadow", "bytes");
  for (ti = 0; ti < 5; ti++)
    for (way = 0; way < N_WAYS; way++) {
      double t[7];
      long i;
      for (r = 0; r < 7; r++) {
        double t0;
        XSync(dpy, False);
        req_bytes = 0;
        t0 = now();
        for (i = 0; i < n; i++)
          draw_shadow(way, pm, top, bot, i & 15, 0, 300, 100, thick[ti], XmSHADOW_OUT);
        XSync(dpy, False);
        t[r] = (now() - t0) / n * 1e9;
      }
      qsort(t, 7, sizeof t[0], cmp_double);
      printf("%-6d %-9s %10.0f %10ld\n", thick[ti], way_name[way], t[3], req_bytes / n);
    }
  XFreeGC(dpy, top);
  XFreeGC(dpy, bot);
  XFreePixmap(dpy, pm);
}

int main(int argc, char **argv)
{
  int status = 0;
  if (argc < 2 || (strcmp(argv[1], "check") && strcmp(argv[1], "time"))) {
    fprintf(stderr, "usage: %s check | time [N]\n", argv[0]);
    return 2;
  }
  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot open display\n", argv[0]);
    return 77;
  }
  root = DefaultRootWindow(dpy);
  depth = DefaultDepth(dpy, DefaultScreen(dpy));
  /* XmeDrawShadows takes the application context lock of the display. */
  XtToolkitInitialize();
  XtDisplayInitialize(XtCreateApplicationContext(), dpy, "shadowbench", "Shadowbench", NULL, 0,
                      &argc, argv);
  if (!strcmp(argv[1], "check"))
    status = check();
  else
    time_ways(argc > 2 ? atol(argv[2]) : 100000);
  XCloseDisplay(dpy);
  return status;
}
