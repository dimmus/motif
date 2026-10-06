# 5. The Graphics Pipeline: Images, Pixmaps, Text and GCs

**Scope.** Chapter 2 covered the geometric primitives.  This chapter
follows the other things a widget draws, from a resource file to the
screen: an image named in a resource (`*labelPixmap: folder.png`)
through the file readers, the image and pixmap caches, depth and size
conversion and the X server; anti-aliased text through Xft; and the
graphics contexts that both need.  The modules are
[`ImageCache.c`](../../src/lib/Xm/ImageCache.c),
[`PixConv.c`](../../src/lib/Xm/PixConv.c), the readers
[`ReadImage.c`](../../src/lib/Xm/ReadImage.c) (XBM),
[`Xpm*.c`](../../src/lib/Xm/Xpmcreate.c) (XPM),
[`Png.c`](../../src/lib/Xm/Png.c), [`Jpeg.c`](../../src/lib/Xm/Jpeg.c),
[`Svg.c`](../../src/lib/Xm/Svg.c), and the Xft part of
[`XmRenderT.c`](../../src/lib/Xm/XmRenderT.c).  The data structures
behind the caches are analysed separately in chapter 5.1.

---

## 5.1 From a name to a pixmap

The pipeline for `XmGetPixmap`, `XmGetPixmapByDepth`,
`XmGetScaledPixmap` and the `String → Pixmap` converter used by every
pixmap resource:

```
 name ──▶ pixmap_data_set? ──hit──▶ Pixmap (reference_count++)
            │ miss
            ▼
          image_set? ──hit──▶ XImage (built-in or installed)
            │ miss
            ▼
          XmGetIconFileName (search path, size suffixes)
            │
            ▼
          LoadImage: first byte of the file
            '<' ──▶ SVG (nanosvg)          not cached as an image
            0xff 0xd8 ──▶ JPEG (libjpeg)   not cached as an image
            0x89 'PNG' ──▶ PNG (libpng)    not cached as an image
            otherwise ──▶ XPM, then XBM    cached in image_set
            │
            ▼
          scale (print resolution or ratio) ──▶ XCreatePixmap, _XmPutScaledImage
            │
            ▼
          pixmap_data_set, pixmap_set entries (reference_count = 1)
```

Three hash tables hold the state (chapter 5.1 describes the table
implementation):

| Table | Key | Value | Purpose |
|-------|-----|-------|---------|
| `image_set` | image name (string) | `ImageData {hot_x, hot_y, XImage *, name, builtin_data}` | Images installed by `XmInstallImage` and the built-in bitmaps (`BitmapsI.h`: the 50 % stipple, the menu cascade arrow, ...); XBM and XPM files read from disk |
| `pixmap_data_set` | `PixmapData` (name, screen, depth, colours, resolution or ratio, print shell) | `PixmapData` | "Do I already have a pixmap for this name with these colours at this depth?" |
| `pixmap_set` | `PixmapData` (screen, pixmap id) | the same `PixmapData` | Reverse lookup for `XmeGetPixmapData` and `XmDestroyPixmap` |

The entry record:

```c
typedef struct _PixmapData {
  Screen *screen;
  char *image_name;
  XmAccessColorData acc_color;      /* foreground, background, top/bottom shadow, select, highlight */
  Pixmap pixmap;
  int depth;
  Dimension width, height;
  int reference_count;
  unsigned short print_resolution;  /* when printing */
  Widget print_shell;
  double scaling_ratio;
  Pixel *pixels;                    /* colours allocated by XPM, freed with the pixmap */
  int npixels;
} PixmapData;
```

**Why two levels.**  An `XImage` is client-side memory in the format
the file described; a `Pixmap` is a server-side resource at a given
screen, depth and colouring.  One image may become several pixmaps
(a bitmap drawn in a button's foreground and background colours, the
same bitmap at depth 1 for a mask or an insensitive stipple), so the
cache keys pixmaps by *everything that influenced the rendering*.
`ComparePixmapDatas` is therefore a long conjunction: same name, same
screen, same print shell, same resolution or ratio, depth equal (or a
negative "don't care" depth matching either the real depth or 1), and
each of the six symbolic colours equal *or unspecified on either side*.
The "unspecified" wildcard lets a request that names only foreground
and background reuse a pixmap built from an XPM that used no other
symbolic colour.

**Symbolic colours.**  Motif XPM files may name `background`,
`foreground`, `top_shadow_color`, `bottom_shadow_color`,
`select_color`, `highlight_color` and `none` as colours
(`SYMB_*` indices in the source), and `GetOverrideColors` substitutes
the widget's actual pixels at load time so that one icon follows the
widget's colour scheme.  The resulting pixmap depends on those pixels,
which is exactly why they are part of the key.

**Reference counting.**  Every hit increments `reference_count`;
`XmDestroyPixmap` decrements and, at zero, removes both entries, frees
the server pixmap (unless it was registered by the application as
`DIRECT_PIXMAP_CACHED`), frees the XPM-allocated colours through the
colour cache, and frees the record.  The same pixmap requested a
thousand times by a thousand buttons costs one server resource and one
hash lookup per button; the 2026 review found and fixed a
use-after-free and a lock imbalance in this code, both in the
error paths (`CHANGELOG`, "Images").

**Printing.**  Motif can render to an X Print Server at a resolution
other than the screen's.  When `scaling_ratio` is 0 the pixmap is
scaled by `print_resolution / pixmap_resolution`, where the design
resolution of the image is the print shell's
`XmNdefaultPixmapResolution` (100 dpi without a print shell); the
reader has a placeholder for taking it from the file instead ("in
the future: look in the file") that was never filled in.  The ratio or
the resolution is in the key so that screen and printer pixmaps
coexist.

## 5.2 Format detection and the readers

`LoadImage` sniffs the first four bytes rather than trusting an
extension: `<` for SVG, the JPEG SOI marker, the PNG signature,
anything else goes to the XPM reader and, if that fails, to XBM.
PNG, JPEG and SVG images are *not* installed in `image_set` ("destroy
it once it's used"): they are decoded straight into a pixmap and the
`XImage` freed, because a decoded photograph is large and the pixmap
is what gets reused.  Each reader checks sizes and overflow before
allocating (the `CHANGELOG` lists "the PNG, JPEG and SVG loaders check
sizes and overflow"), and the bundled XPM code carries the fixes for
CVE-2022-44617, CVE-2022-46285, CVE-2023-43788 and CVE-2023-43789 and
never runs external decompressors (CVE-2022-4883).  The search path
and the `_s`/`_m`/`_l`/`_t` size suffixes are `XmGetIconFileName`'s
(`IconFile.c`), following the CDE icon conventions.

### SVG

[`Svg.c`](../../src/lib/Xm/Svg.c) bundles nanosvg, a single-header
parser and rasterizer.  `_XmSvgGetImage` parses the file at 96 dpi and
returns an `XImage` whose `obdata` is the parsed `NSVGimage` and whose
`f.sub_image` method is a rasterizer, so that the rest of the pipeline
can ask for the image *at the size it needs*:

```c
static XImage *rasterize(XImage *src, int x, int y, unsigned int w, unsigned int h)
{
  ...
  /* nanosvgrast indexes the bitmap with int (y * stride), so w * h * 4 must fit */
  if (!w || !h || w > SVG_MAX_DIM || h > SVG_MAX_DIM || (size_t)w * h > INT_MAX / 4 ||
      !(rast = nsvgCreateRasterizer()))
    return NULL;
  if (!(data = Xcalloc((size_t)w * h, 4))) { ... }
  x_scale = w / (float)src->width;
  y_scale = h / (float)src->height;
  scale = (x_scale < y_scale) ? x_scale : y_scale;
  nsvgRasterize(rast, (NSVGimage *)src->obdata, x * scale, y * scale, scale, data, w, h, w * 4);
  nsvgDeleteRasterizer(rast);
  ...                      /* a 32-bit ZPixmap XImage with 8-bit R, G, B masks and alpha in the top byte */
  img->depth = 32;
  if (!XInitImage(img)) { ... }
  return img;
}
```

The scale is the *smaller* of the two ratios, so the drawing keeps its
aspect ratio inside the requested box.  Vector icons are the one place
where Motif can produce a crisp image at any size, and
`_XmPutScaledImage` rasterizes them at the destination size instead of
scaling pixels.

## 5.3 Getting pixels onto the server: `_XmPutScaledImage` and `render_image`

```c
void _XmPutScaledImage(Screen *screen, Display *display, Drawable d, int depth, GC gc, XImage *src,
                       int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
  int free_src = 0;
  Visual *vis = DefaultVisualOfScreen(screen);
  /* svg: Rasterize to the given size */
  if (XImageIsSVG(src)) { ... src = src->f.sub_image(src, sx, sy, dw, dh); ... }
  /* Same depth, size, and format */
  if (src->depth == depth && dw == sw && dh == sh && src->red_mask == vis->red_mask &&
      src->green_mask == vis->green_mask && src->blue_mask == vis->blue_mask)
  {
    XPutImage(display, d, gc, src, sx, sy, dx, dy, dw, dh);
    ...
    return;
  }
  render_image(screen, display, d, depth, gc, src, sx, sy, sw, sh, dx, dy, dw, dh);
  ...
}
```

The fast path is a single `XPutImage` when the image already matches
the destination's depth, size and channel layout, which is the case
for bitmaps and for XPM images on the default visual.  Everything else
goes through `render_image`, a software scaler and format converter:

```c
static void render_image(...)
{
  ...
  psz = depth > 16 ? 32 : depth < 16 ? 8 : 16;
  /* Let Xlib pick the server's bits per pixel for this depth ... XPutPixel() computes
   * y * bytes_per_line in int, so stay below INT_MAX. */
  if (!(dest_image = XCreateImage(display, vis, depth, ZPixmap, 0, NULL, dw, dh, psz, 0)) ||
      dest_image->bytes_per_line <= 0 || dest_image->bytes_per_line > INT_MAX / dh ||
      !(data = Xmalloc((size_t)dest_image->bytes_per_line * dh))) { ... }
  dest_image->data = data;
  ...
  bg.pixel = gcv.background;
  XQueryColor(display, screen->cmap, &bg);
  for (y = 0; y < dh; y++) {
    for (x = 0; x < dw; x++) {
      ox = sx + (int)(x / (double)dw * sw);        /* nearest-neighbour sampling */
      oy = sy + (int)(y / (double)dh * sh);
      ...
      switch (src->depth) {
        case 1:  pixel = (bit set) ? gcv.foreground : gcv.background; break;
        case 8:  xc.pixel = XGetPixel(src, ox, oy); XQueryColor(display, screen->cmap, &xc); r = xc.red / 257; ... break;
        default:
          pixel = XGetPixel(src, ox, oy);
          r = (pixel & src->red_mask) >> ctz(src->red_mask); ...
          if (src->depth == 32) {
            if (!(a = (pixel >> 24) & 0xff)) { pixel = gcv.background; break; }
            else if (a != 0xff) {
              r = BLEND(bg.red / 65535.0, r / 255.0, a / 255.0); ...   /* alpha over the GC background */
            }
          }
          ...
          pixel = (((r << ctz(vis->red_mask)) & vis->red_mask) | ...);   /* TrueColor/DirectColor compose */
      }
      if (src->depth == 1 || vis->class == TrueColor || vis->class == DirectColor) { XPutPixel(dest_image, x, y, pixel); continue; }
      if (depth == 1 || vis->map_entries <= 2) { XPutPixel(dest_image, x, y, (r | g | b) ? gcv.foreground : gcv.background); continue; }
      /* Colormap time */
      xc.red = r * 257; xc.green = g * 257; xc.blue = b * 257;
      pixel = gcv.foreground;
      if (XAllocColor(display, screen->cmap, &xc))
        pixel = xc.pixel;
      XPutPixel(dest_image, x, y, pixel);
    }
  }
  XPutImage(display, d, gc, dest_image, 0, 0, dx, dy, dw, dh);
  XDestroyImage(dest_image);
}
```

Analysis:

- **Sampling** is nearest-neighbour: O(dw × dh) with no filtering.
  Fine for icons scaled by integer factors and for print resolution
  changes; a photograph scaled down by 3 will alias.  Vector images
  bypass this (they are rasterized at size), bitmaps are exact.
- **Channel extraction** uses the image's masks and `ctz` (count
  trailing zeros, a compiler builtin with a de Bruijn-style fallback
  table), so any `TrueColor` layout is handled, and the 5:5:5 versus
  5:6:5 16-bit cases are detected with a popcount of the visual's red
  mask.
- **Alpha** (depth-32 sources, which is what PNG and SVG produce) is
  composited over the GC's background colour with a plain
  `bg·(1−a) + c·a` lerp, after one `XQueryColor` for the background.
  There is no destination-read, so an image with a soft edge blends
  against the widget's background colour, not against whatever is
  already drawn.  This is the limit of core-protocol compositing;
  true blending would need the RENDER extension, which Motif uses only
  through Xft.
- **PseudoColor** destinations are the slow path: one `XAllocColor`
  round trip *per pixel* (and for 8-bit sources one `XQueryColor` per
  pixel on the way in).  On an 8-bit visual a 64 × 64 icon is 4 096
  round trips, which is why the comment says "Slowly interrogate the
  colormap".  In 2026 this path exists for Xvfb runs and legacy
  servers; the design accepts its cost rather than carrying a colour
  quantiser.
- **Safety.**  The destination size is validated against `INT_MAX`
  because `XPutPixel` indexes with `int`; the width and height come
  from a file or a resource and are untrusted.

## 5.4 Graphics contexts for images

`GetGCForPutImage` keeps a fourth hash table, `gc_set`, keyed by
(screen, print shell, destination depth, image depth, foreground,
background).  The comment explains why it does not use the per-screen
`XmScreen` object's GC cache: pixmap conversion can run before the
`XmScreen` exists (the conversion for the first VendorShell), and the
cache must be per depth.  The GCs are created with `XCreateGC` on the
target pixmap and never freed until the screen or print shell closes
(`_XmCleanPixmapCache` walks the tables with `_XmMapHashTable`).  As
the comment also notes, there are few distinct entries: depth-1 to
depth-1, depth-N to depth-N (colours irrelevant), and depth-1 to
depth-N for each colour pair used by background pixmaps.

`_XmGetPixmapBasedGC` in `PixConv.c`, used by widgets whose background
is a pixmap, builds a GC with `FillOpaqueStippled` for a depth-1 pixmap or
`FillTiled` for a colour one, through `XtGetGC` so that the GC is
shared.

## 5.5 Text: core fonts, font sets and Xft

A `XmString` is drawn by `XmStringDraw` through a *render table*
(`XmRenderTable`), a list of *renditions* (`XmRendition`) each naming a
font (or a font set, or an Xft font), colours, underline and
strikethrough style, and tabs.  The rendition record shows the three
font back ends side by side:

```c
/* src/lib/Xm/XmRenderTI.h */
typedef struct __XmRenditionRec {
  unsigned int fontOnly : REND_OPTIMIZED_BITS;
  unsigned int refcount : REND_REFCOUNT_BITS;
  unsigned char loadModel;
  XmStringTag tag;
  String fontName;
  XmFontType fontType;          /* XmFONT_IS_FONT, XmFONT_IS_FONTSET, XmFONT_IS_XFT */
  XtPointer font;               /* XFontStruct *, XFontSet, or XftFont * */
  Display *display;
  GC gc;
  XmStringTag *tags; unsigned int count; Boolean hadEnds;
  XmTabList tabs;
  Pixel background, foreground;
  unsigned char underlineType, strikethruType, backgroundState, foregroundState;
#  if USE_XFT
  char *fontStyle, *fontFoundry, *fontEncoding;
  int fontSize, pixelSize, fontSlant, fontSpacing, fontWeight;
  XftPattern *pattern;
  XftFont *xftFont;
  XftColor xftForeground, xftBackground;
#  endif
} _XmRenditionRec, *_XmRendition;
```

Core fonts (`XLoadQueryFont`, server-side bitmaps) and font sets
(`XCreateFontSet`, several core fonts covering a locale's charsets) are
drawn with `XDrawString`/`XmbDrawString` through a GC.  Xft fonts are
client-side FreeType glyphs composited by the RENDER extension; they
are drawn through an `XftDraw` per window with an `XftColor`, not a
GC.  Since Motif 2.3 the choice is per rendition (`XmNfontType`), so a
single compound string can mix a core font for one segment and an Xft
font for another.

### The Xft caches

Three things are expensive on the Xft path and are cached per display
in this tree, in a record found by walking a short move-to-front list
of displays:

```c
/* src/lib/Xm/XmRenderT.c */
  rec->draws = _XmAllocHashTable(64, NULL, NULL);                        /* Window -> XftDraw * */
  rec->colors = _XmAllocHashTable(64, CompareXftColor, HashXftColor);    /* (colormap, pixel) -> XftColor */
  rec->fonts = _XmAllocHashTable(16, CompareXftFont, HashXftFont);       /* font description -> XftFont * */
```

- **Draws.**  `XftDrawCreate` allocates a RENDER *Picture* for a
  window; creating one per `XmStringDraw` call would be a server
  resource per string.  `_XmXftDrawCreate` looks the window up in
  `draws`, creates on a miss, and registers an `XtNdestroyCallback` on
  the window's widget so that the `XftDraw` is destroyed *before* the
  window is, "so that the table does not keep every window ever drawn
  into, nor hand a stale XftDraw to a new window that gets the same
  XID".  The last clause is the subtle one: X recycles window ids, so
  a stale cache entry would not merely leak, it would draw into the
  wrong window.
- **Colours.**  An `XftColor` needs 16-bit RGB, which for an arbitrary
  pixel means `XQueryColor`, a round trip.  `GetXftColor` caches the
  answer per (colormap, pixel) *only when the colormap is static*
  (`TrueColor`, `StaticColor`, `StaticGray` default colormaps,
  `ColormapIsStatic`); on a dynamic colormap the pixel's colour can
  change, so it is queried every time.
- **Fonts.**  Opening an Xft font means building a fontconfig pattern,
  matching it against the installed fonts and loading the face, tens
  of milliseconds.  `LookupXftFont`/`CacheXftFont` key a copy of the
  opened `XftFont` by the full description (name, foundry, encoding,
  style, size, pixel size, slant, weight, spacing), hashed with the
  classic multiplicative string hash `h = h·31 + c`.
- **Lifetime.**  The record registers an extension close hook with
  `XESetCloseDisplay`, so that when the display is closed the fonts are
  closed with `XftFontClose`, the colours and draws tables freed, and
  the record unlinked; the draws themselves are not destroyed because
  the server has already freed the windows' pictures.
- **Growth.**  `AddHashEntry` doubles a table when its count exceeds
  its bucket count, keeping the load factor at most 1 (chapter 5.1).

`xmbench` has `xmstring-draw`, `xmstring-draw-xft`,
`xmstring-draw-fontset`, the matching `-extent` cases, and
`xft-labels` ("expose Xft Labels in their own windows") to compare the
three back ends; the [CHANGELOG](../../CHANGELOG.md) records that "the
Xft fonts, colours and draws are cached per display".

### Drawing a segment

```c
void _XmXftDrawString(Display *display, Window window, XmRendition rend, int bpc,
                      Position x, Position y, char *s, int len, Boolean image)
{
  XftDraw *draw = _XmXftDrawCreate(display, window);
  XftColor fg_color = _XmRendXftFG(rend);
  if (image) {                         /* "image" text: paint the background box first */
    XftColor bg_color = _XmRendXftBG(rend);
    XGlyphInfo ext;
    switch (bpc) { case 1: XftTextExtentsUtf8(...); break; case 2: XftTextExtents16(...); break; case 4: XftTextExtents32(...); break; }
    if (_XmRendBG(rend) == XmUNSPECIFIED_PIXEL) { ... bg_color = GetDrawXftColor(display, window, gc_val.background); }
    XftDrawRect(draw, &bg_color, x, y - font->ascent, ext.xOff, font->ascent + font->descent);
  }
  if (_XmRendFG(rend) == XmUNSPECIFIED_PIXEL) { ... fg_color = GetDrawXftColor(display, window, gc_val.foreground); }
  switch (bpc) {
    case 1: XftDrawStringUtf8(draw, &fg_color, _XmRendXftFont(rend), x, y, (XftChar8 *)s, len); break;
    case 2: XftDrawString16(...); break;
    case 4: XftDrawString32(...); break;
  }
}
```

`bpc` is bytes per character of the segment's text: 1 for UTF-8
(`XM_UTF8`) and locale multibyte text, 2 and 4 for the `XChar2b` and
`wchar_t` forms a compound string may hold.  When the rendition does
not specify a colour, the colour comes from the GC that the core-font
path would have used (`XGetGCValues` reads Xlib's client-side copy, no
round trip), through the colour cache.  `XftDrawSetClipRectangles`
carries the widget's clip rectangle over to the RENDER picture, since
the GC's clip does not apply to it.

## 5.6 The pipeline in one table

| Stage | Cost | Caches that remove it |
|-------|------|-----------------------|
| Find the file | path search, `stat` per candidate | `image_set` for XBM/XPM; `pixmap_data_set` for everything |
| Decode | O(pixels), libpng/libjpeg/nanosvg/XPM | `image_set` (bitmaps, XPM); pixmaps for the rest |
| Allocate colours (XPM, PseudoColor) | round trips | the colour cache in `ImageCache.c` (`GetCacheColor`, reference-counted by name and RGB) |
| Convert and scale | O(dest pixels), plus round trips on PseudoColor | avoided entirely by the `XPutImage` fast path |
| Server pixmap | one `CreatePixmap`, one `PutImage` | `pixmap_data_set` with reference counting |
| GC for the transfer | `CreateGC` | `gc_set` |
| Xft draw / colour / font | Picture per window, round trip per colour, fontconfig match per font | the three per-display tables |

## 5.7 Assessment

| Decision | For | Against |
|----------|-----|---------|
| Two-level image/pixmap cache keyed by colours | Correct sharing with Motif's symbolic colours; printing coexists with the screen | A 14-term comparison per probe; the key is a struct of eleven fields; entries never expire except by reference count |
| Content sniffing | Works with any file name | Four bytes decide; a mislabelled file is tried as XPM |
| Large formats not cached as images | Bounded client memory | A PNG used twice is decoded twice |
| Nearest-neighbour software scaler | Any visual, any depth, no dependencies | Aliasing; per-pixel round trips on PseudoColor |
| Alpha over GC background only | No destination read | Soft edges blend against the colour, not the content |
| Xft per rendition | Anti-aliasing where wanted; legacy fonts still work | Two drawing paths in every text widget; GC clips do not apply to pictures |
| Per-display Xft tables with destroy and close hooks | No stale XIDs; bounded resources | More code than the array cache it replaced; correctness depends on every window going through `_XmXftDrawCreate` |

## References

- Xlib, "Images" and "Graphics Context Functions":
  <https://www.x.org/releases/current/doc/libX11/libX11/libX11.html>
- Xft: <https://www.x.org/releases/current/doc/man/man3/Xft.3.xhtml>;
  fontconfig: <https://www.freedesktop.org/wiki/Software/fontconfig/>;
  the RENDER extension: <https://www.x.org/releases/current/doc/renderproto/renderproto.txt>
- nanosvg: <https://github.com/memononen/nanosvg>
- libXpm advisories: <https://lists.x.org/archives/xorg-announce/2023-January/003312.html>,
  <https://lists.x.org/archives/xorg-announce/2023-October/003424.html>
- [`src/lib/Xm/ImageCache.c`](../../src/lib/Xm/ImageCache.c),
  [`ImageCachI.h`](../../src/lib/Xm/ImageCachI.h),
  [`PixConv.c`](../../src/lib/Xm/PixConv.c),
  [`IconFile.c`](../../src/lib/Xm/IconFile.c),
  [`Svg.c`](../../src/lib/Xm/Svg.c), [`Png.c`](../../src/lib/Xm/Png.c),
  [`Jpeg.c`](../../src/lib/Xm/Jpeg.c),
  [`XmRenderT.c`](../../src/lib/Xm/XmRenderT.c),
  [`XmRenderTI.h`](../../src/lib/Xm/XmRenderTI.h),
  [`Color.c`](../../src/lib/Xm/Color.c)
- Manual pages `XmGetPixmap(3)`, `XmGetPixmapByDepth(3)`,
  `XmGetScaledPixmap(3)`, `XmInstallImage(3)`, `XmDestroyPixmap(3)`,
  `XmRenderTable(3)`, `XmRendition(3)`, `XmeXpm(3)` in
  [`doc/man/man3`](../man/man3)
- [SECURITY.md](../../SECURITY.md), "Files the user did not write"
