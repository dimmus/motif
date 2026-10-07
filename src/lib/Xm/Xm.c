/* $XConsortium: Xm.c /main/6 1995/10/25 20:28:03 cde-sun $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * HISTORY
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "MessagesI.h"
#include "XmI.h"
#include <X11/Xlibint.h> /* for XESetCloseDisplay */
#include <X11/keysym.h>
#include <Xm/GadgetP.h>
#include <Xm/IconGP.h>
#include <Xm/LabelGP.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <stdatomic.h>
/**************************************************************************
 *   This is Xm.c
 *    It contains global API that:
 *      - it's not widget specific
 *      - it's already used by various widgets (you don't get useless
 *          code by linking with this module: all the functions
 *          are useful and used.
 *   For example, TrackingLocate or ResolvePartOffset do not belong
 *   here because they are not used by everybody.
 *************************************************************************/
Boolean _init_modifiers = TRUE;
unsigned int NumLockMask = 0;
unsigned int ScrollLockMask = 0;

/*************************************<->*************************************
 *
 *  _XmInitModifiers (void)
 *
 *   Description:
 *   -----------
 *     Sets the appropriate mask for NumLock and ScrollLock
 *
 *
 *   Inputs:
 *   ------
 *     None
 *
 *   Outputs:
 *   -------
 *     None
 *
 *   Procedures Called
 *   -----------------
 *     None
 *
 *************************************<->***********************************/
void _XmInitModifiers(void)
{
  XModifierKeymap *modmap;
  Display *dpy;
  KeySym *keymap;
  unsigned int keycode;
  int min_keycode;
  int max_keycode;
  int keysyms_per_keycode;
  int i;
  dpy = _XmGetDefaultDisplay();
  NumLockMask = 0;
  ScrollLockMask = 0;
  keysyms_per_keycode = 0;
  min_keycode = 0;
  max_keycode = 0;
  XDisplayKeycodes(dpy, &min_keycode, &max_keycode);
  modmap = XGetModifierMapping(dpy);
  keymap = XGetKeyboardMapping(
      dpy, min_keycode, max_keycode - min_keycode + 1, &keysyms_per_keycode);
  if (modmap && keymap) {
    for (i = 3 * modmap->max_keypermod; i < 8 * modmap->max_keypermod; i++) {
      keycode = modmap->modifiermap[i];
      if (((int)keycode >= min_keycode) && ((int)keycode <= max_keycode)) {
        int j;
        KeySym *syms = keymap + (keycode - min_keycode) * keysyms_per_keycode;
        for (j = 0; j < keysyms_per_keycode; j++)
          if (!NumLockMask && (syms[j] == XK_Num_Lock))
            NumLockMask = (1 << (i / modmap->max_keypermod));
          else if (!ScrollLockMask && (syms[j] == XK_Scroll_Lock))
            ScrollLockMask = (1 << (i / modmap->max_keypermod));
      }
    }
  }
  /* Cleanup memory */
  if (modmap)
    XFreeModifiermap(modmap);
  if (keymap)
    XFree(keymap);
}

/**************************************************************************
 *                                                                        *
 * _XmSocorro - Help dispatch function.  Start at the widget help was     *
 *   invoked on, find the first non-null help callback list, and call it. *
 *   -- Called by various widgets across Xm                               *
 *                                                                        *
 *************************************************************************/
/* ARGSUSED */
void _XmSocorro(Widget w,
                XEvent *event,
                String *params,       /* unused */
                Cardinal *num_params) /* unused */
{
  XmAnyCallbackStruct cb;
  if (w == NULL)
    return;
  cb.reason = XmCR_HELP;
  cb.event = event;
  do {
    if ((XtHasCallbacks(w, XmNhelpCallback) == XtCallbackHasSome)) {
      XtCallCallbacks(w, XmNhelpCallback, &cb);
      return;
    }
    else
      w = XtParent(w);
  } while (w != NULL);
}

/****************************************************************
 *
 * _XmParentProcess
 *    This is the entry point for parent processing.
 *   -- Called by various widgets across Xm
 *
 ****************************************************************/
Boolean _XmParentProcess(Widget widget, XmParentProcessData data)
{
  XmManagerWidgetClass manClass;
  manClass = (XmManagerWidgetClass)widget->core.widget_class;
  if (XmIsManager(widget) && manClass->manager_class.parent_process) {
    return ((*manClass->manager_class.parent_process)(widget, data));
  }
  return (FALSE);
}

/************************************************************************
 *
 *  _XmDestroyParentCallback
 *     Destroy parent. Used by various dialog subclasses
 *
 ************************************************************************/
/* ARGSUSED */
void _XmDestroyParentCallback(Widget w,
                              XtPointer client_data, /* unused */
                              XtPointer call_data)   /* unused */
{
  XtDestroyWidget(XtParent(w));
}

/************************************************************************
 *
 *  _XmClearShadowType
 *	Clear the right and bottom border area and save
 *	the old width, height and shadow type.
 *      Used by various subclasses for resize larger situation, where the
 *      inside shadow is not exposed.
 *   Maybe that should be moved in Draw.c, maybe not, since it's a widget API
 *
 ************************************************************************/
void _XmClearShadowType(Widget w,
                        Dimension old_width,
                        Dimension old_height,
                        Dimension old_shadow_thickness,
                        Dimension old_highlight_thickness)
{
  if (old_shadow_thickness == 0)
    return;
  if (XtIsRealized(w)) {
    if (old_width <= w->core.width)
      XClearArea(XtDisplay(w),
                 XtWindow(w),
                 old_width - old_shadow_thickness - old_highlight_thickness,
                 0,
                 old_shadow_thickness,
                 old_height - old_highlight_thickness,
                 False);
    if (old_height <= w->core.height)
      XClearArea(XtDisplay(w),
                 XtWindow(w),
                 0,
                 old_height - old_shadow_thickness - old_highlight_thickness,
                 old_width - old_highlight_thickness,
                 old_shadow_thickness,
                 False);
  }
}

/**********************************************************************
 *
 * _XmReOrderResourceList
 *   This procedure moves the given resource right after the
 *   insert_after name in this class resource list.
 *   (+ insert_after NULL means insert in front)
 *
 *   ----Replace by a call to an Xt function in R6.-----
 **********************************************************************/
void _XmReOrderResourceList(WidgetClass widget_class, String res_name, String insert_after)
{
  XrmResource **list;
  int len;
  XrmQuark res_nameQ = XrmPermStringToQuark(res_name);
  XrmResource *tmp;
  int n;
  _XmProcessLock();
  list = (XrmResource **)widget_class->core_class.resources;
  len = widget_class->core_class.num_resources;
  /* look for the named resource slot */
  n = 0;
  while ((n < len) && (list[n]->xrm_name != res_nameQ))
    n++;
  if (n < len) {
    int m, i;
    XrmQuark insert_afterQ;
    if (insert_after) {
      insert_afterQ = XrmPermStringToQuark(insert_after);
      /* now look for the insert_after resource slot */
      m = 0;
      while ((m < len) && (list[m]->xrm_name != insert_afterQ))
        m++;
    }
    else
      m = len;
    if (m == len)
      m = -1;
    /* now do the insertion/packing, both cases */
    tmp = list[n];
    if (n > m) {
      for (i = n; i > m + 1; i--)
        list[i] = list[i - 1];
      list[m + 1] = tmp;
    }
    else {
      for (i = n; i < m; i++)
        list[i] = list[i + 1];
      list[m] = tmp;
    }
  }
  _XmProcessUnlock();
}

/************************************************************************
 *
 *  _XmWarningMsg
 *	Add XME_WARNING to Message list so MotifWarningHandler will
 *      add Name: & Class:
 *
 ************************************************************************/
void _XmWarningMsg(Widget w, char *type, char *message, char **params, Cardinal num_params)
{
  char *new_params[11];
  Cardinal num_new_params = num_params + 1;
  int i;
  if (num_new_params > 11)
    num_new_params = 11;
  for (i = 0; (Cardinal)i < num_new_params - 1; i++)
    new_params[i] = params[i];
  new_params[num_new_params - 1] = XME_WARNING;
  if (w != NULL) {
    XtAppWarningMsg(XtWidgetToApplicationContext(w),
                    XrmQuarkToString(w->core.xrm_name),
                    type,
                    w->core.widget_class->core_class.class_name,
                    message,
                    new_params,
                    &num_new_params);
  }
  else
    XtWarning(message);
}

/************************************************************************
 *
 *  _XmMallocArray, _XmReallocArray
 *	Allocate room for num elements of size bytes each.
 *
 *	XtMalloc and XtRealloc take a Cardinal, so a size the caller
 *	multiplies itself silently wraps around on overflow, or is
 *	truncated to 32 bits on LP64, and the caller then writes past the
 *	end of a short buffer.  These check the multiplication, and fail
 *	the way XtMalloc does when it runs out of memory if the size does
 *	not fit.  A negative int count converts to a huge size_t, so it is
 *	caught as well.
 *
 ************************************************************************/
static void ArrayAllocError(String type)
{
  Cardinal num_params = 1;
  XtErrorMsg("allocError",
             type,
             "XtToolkitError",
             "Cannot perform %s: size overflow",
             &type,
             &num_params);
}

#define MAX_ALLOC_SIZE ((size_t)(Cardinal)~(Cardinal)0)

char *_XmMallocArray(size_t num, size_t size)
{
  if (size != 0 && num > MAX_ALLOC_SIZE / size) {
    ArrayAllocError("malloc");
    return NULL;
  }
  return XtMalloc((Cardinal)(num * size));
}

char *_XmReallocArray(char *ptr, size_t num, size_t size)
{
  if (size != 0 && num > MAX_ALLOC_SIZE / size) {
    ArrayAllocError("realloc");
    return NULL;
  }
  return XtRealloc(ptr, (Cardinal)(num * size));
}

/************************************************************************
 *
 *  _XmConcatStrings
 *	Concatenate the count strings of list into one string allocated
 *	with XtMalloc.  This is what a strcat() loop into a buffer of the
 *	summed lengths did, without the rescans and in size_t.
 *
 ************************************************************************/
char *_XmConcatStrings(char **list, int count)
{
  size_t size = 1, length = 0, n;
  char *s;
  int i;
  for (i = 0; i < count; i++)
    size += strlen(list[i]);
  s = _XmMallocArray(size, 1);
  for (i = 0; i < count; i++) {
    n = strlen(list[i]);
    memcpy(s + length, list[i], n);
    length += n;
  }
  s[length] = '\0';
  return s;
}

/************************************************************************
 *
 *  _XmGetWindowPropertyChecked
 *	Read the first long_length 32-bit units of a window property and
 *	check what came back.
 *
 *	Any client can write a property on any window, so its type,
 *	format and length must be checked before the data is used.  This
 *	returns True only when XGetWindowProperty succeeded, the property
 *	exists, its type is req_type (any type for AnyPropertyType), its
 *	format is format (8, 16 or 32 for 0) and it holds at least
 *	min_items items.  The data is then in *prop_return, to be freed
 *	with XFree.  Otherwise anything read is freed, *prop_return is
 *	NULL, *nitems_return is 0 and the other results are None or 0;
 *	unlike XGetWindowProperty, none of them is left unset when the
 *	request fails.  actual_type_return, actual_format_return and
 *	bytes_after_return may be NULL.
 *
 *	Remember that Xlib returns format 32 data as an array of long.
 *
 ************************************************************************/
Boolean _XmGetWindowPropertyChecked(Display *display,
                                    Window w,
                                    Atom property,
                                    long long_length,
                                    Atom req_type,
                                    int format,
                                    unsigned long min_items,
                                    Atom *actual_type_return,
                                    int *actual_format_return,
                                    unsigned long *nitems_return,
                                    unsigned long *bytes_after_return,
                                    unsigned char **prop_return)
{
  Atom type = None;
  int actual_format = 0;
  unsigned long nitems = 0, bytes_after = 0;
  unsigned char *data = NULL;
  Boolean ok;
  ok = XGetWindowProperty(display,
                          w,
                          property,
                          0L,
                          long_length,
                          False,
                          req_type,
                          &type,
                          &actual_format,
                          &nitems,
                          &bytes_after,
                          &data) == Success &&
       data != NULL && type != None && (req_type == AnyPropertyType || type == req_type) &&
       (format == 0 ? (actual_format == 8 || actual_format == 16 || actual_format == 32) :
                      actual_format == format) &&
       nitems >= min_items;
  if (!ok) {
    if (data != NULL)
      XFree(data);
    data = NULL;
    type = None;
    actual_format = 0;
    nitems = bytes_after = 0;
  }
  if (actual_type_return != NULL)
    *actual_type_return = type;
  if (actual_format_return != NULL)
    *actual_format_return = actual_format;
  if (bytes_after_return != NULL)
    *bytes_after_return = bytes_after;
  *nitems_return = nitems;
  *prop_return = data;
  return ok;
}

/************************************************************************
 *
 *  _XmGetSubresources
 *	XtGetSubresources, under the process lock.
 *
 *	Xt compiles a resource list in place the first time it is used,
 *	holding only the lock of the application context of the widget.
 *	Motif's static lists are shared by all application contexts, so
 *	two threads could compile one at the same time.
 *
 *	The application lock is taken first, as Xt does: some callers
 *	(XmCreateSimpleCheckBox, ...) do not hold it, and taking the
 *	process lock first deadlocks with a thread of the same
 *	application context that holds the application lock and waits
 *	for the process lock.
 *
 ************************************************************************/
void _XmGetSubresources(Widget w,
                        XtPointer base,
                        _Xconst char *name,
                        _Xconst char *class_name,
                        XtResourceList resources,
                        Cardinal num_resources,
                        ArgList args,
                        Cardinal num_args)
{
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  _XmProcessLock();
  XtGetSubresources(w, base, name, class_name, resources, num_resources, args, num_args);
  _XmProcessUnlock();
  _XmAppUnlock(app);
}

/************************************************************************
 *
 *  _XmStartErrorTrap, _XmEndErrorTrap
 *	Catch the X errors that the requests made on a display between
 *	the two calls cause, instead of letting them reach the
 *	application's error handler (which by default exits).
 *
 *	Xlib has one error handler for the whole process, so code that
 *	installs its own around a request and then puts the old one back
 *	loses or misroutes errors when two threads do so at once.  Here
 *	TrapErrorHandler stays installed while any thread has a trap, and
 *	keeps an error only when the thread that reads it from the
 *	connection has a trap that matches it; any other error goes to the
 *	handler that was installed before.  With Xt's locking that thread
 *	is the one holding the application lock, which made the requests.
 *
 *	error_code and resource restrict the errors trapped (0: any).
 *	_XmEndErrorTrap first makes a round trip when sync is True (pass
 *	False when the last request was itself a round trip) and returns
 *	the error code of the first error trapped, or 0.  Traps nest.
 *
 ************************************************************************/
static _Thread_local XmErrorTrap threadErrorTraps = NULL;
static _Atomic(XErrorHandler) trapPreviousHandler = NULL;
static int numErrorTraps = 0; /* traps of all threads; process lock */

static int TrapErrorHandler(Display *display, XErrorEvent *event)
{
  XmErrorTrap trap;
  XErrorHandler previous;
  for (trap = threadErrorTraps; trap != NULL; trap = trap->prev) {
    if (trap->display == display && event->serial >= trap->first_request &&
        (trap->error_code == 0 || event->error_code == trap->error_code) &&
        (trap->resource == 0 || event->resourceid == trap->resource))
    {
      if (trap->error == 0)
        trap->error = event->error_code;
      return 0;
    }
  }
  previous = atomic_load(&trapPreviousHandler);
  return previous ? (*previous)(display, event) : 0;
}

void _XmStartErrorTrap(XmErrorTrap trap, Display *display, int error_code, XID resource)
{
  XErrorHandler previous;
  trap->display = display;
  trap->error_code = (unsigned char)error_code;
  trap->resource = resource;
  trap->error = 0;
  /* Install the handler, and remember the one it replaces, which gets
     the errors no trap keeps.  It is installed again for every trap:
     another thread may have put its own in while traps were on, and
     that one is the application's handler now.  Never remember
     TrapErrorHandler itself (an application may put back what it got
     from XSetErrorHandler while a trap was on), or it would call
     itself. */
  _XmProcessLock();
  previous = XSetErrorHandler(TrapErrorHandler);
  if (previous != TrapErrorHandler)
    atomic_store(&trapPreviousHandler, previous);
  numErrorTraps++;
  _XmProcessUnlock();
  trap->first_request = NextRequest(display);
  trap->prev = threadErrorTraps;
  threadErrorTraps = trap;
}

int _XmEndErrorTrap(XmErrorTrap trap, Boolean sync)
{
  XmErrorTrap *link;
  if (sync)
    XSync(trap->display, False);
  for (link = &threadErrorTraps; *link != NULL; link = &(*link)->prev) {
    if (*link == trap) {
      *link = trap->prev;
      break;
    }
  }
  _XmProcessLock();
  if (--numErrorTraps == 0) {
    XErrorHandler current = XSetErrorHandler(atomic_load(&trapPreviousHandler));
    /* A handler another thread installed meanwhile stays */
    if (current != TrapErrorHandler)
      (void)XSetErrorHandler(current);
  }
  _XmProcessUnlock();
  return trap->error;
}

/*
 * The atoms _XmIsISO10646 compares a font's CHARSET_REGISTRY property
 * with, interned once per display.  Comparing atoms is the same as
 * comparing their names, and avoids fetching the name of the property
 * value (a malloc, and a round trip when Xlib's atom cache misses) every
 * time a segment is drawn.  A record is dropped when its display is
 * closed, so that a new display at the same address starts afresh.
 */
typedef struct _XmISO10646AtomsRec {
  struct _XmISO10646AtomsRec *next;
  Display *display;
  int extension;
  Atom registry;
  Atom upper;
  Atom lower;
} XmISO10646AtomsRec;

static XmISO10646AtomsRec *iso10646_atoms = NULL;

static int ISO10646CloseDisplay(Display *dpy, XExtCodes *codes)
{
  XmISO10646AtomsRec **prev, *rec;
  _XmProcessLock();
  for (prev = &iso10646_atoms; (rec = *prev) != NULL; prev = &rec->next)
    if (rec->display == dpy && rec->extension == codes->extension) {
      *prev = rec->next;
      XtFree((char *)rec);
      break;
    }
  _XmProcessUnlock();
  return 0;
}

static Boolean GetISO10646Atoms(Display *dpy, XmISO10646AtomsRec *atoms)
{
  static char *names[] = {"CHARSET_REGISTRY", "ISO10646", "iso10646"};
  Atom values[XtNumber(names)];
  XmISO10646AtomsRec *rec;
  XExtCodes *codes;
  _XmProcessLock();
  for (rec = iso10646_atoms; rec != NULL; rec = rec->next)
    if (rec->display == dpy) {
      *atoms = *rec;
      _XmProcessUnlock();
      return True;
    }
  _XmProcessUnlock();
  /* The names must exist for a later font to be matched, so create them. */
  if (!XInternAtoms(dpy, names, XtNumber(names), False, values))
    return False;
  atoms->registry = values[0];
  atoms->upper = values[1];
  atoms->lower = values[2];
  /* Without a close hook the atoms cannot be cached; still use them. */
  if ((codes = XAddExtension(dpy)) == NULL)
    return True;
  XESetCloseDisplay(dpy, codes->extension, ISO10646CloseDisplay);
  rec = XtNew(XmISO10646AtomsRec);
  *rec = *atoms;
  rec->display = dpy;
  rec->extension = codes->extension;
  _XmProcessLock();
  rec->next = iso10646_atoms;
  iso10646_atoms = rec;
  _XmProcessUnlock();
  return True;
}

/* ARGSUSED */
Boolean _XmIsISO10646(Display *dpy, XFontStruct *font)
{
  XmISO10646AtomsRec atoms;
  XFontProp *xfp;
  int i;
  if (font == NULL || font->n_properties <= 0 || !GetISO10646Atoms(dpy, &atoms))
    return False;
  for (i = 0, xfp = font->properties; i < font->n_properties; xfp++, i++) {
    if (xfp->name == atoms.registry &&
        ((Atom)xfp->card32 == atoms.upper || (Atom)xfp->card32 == atoms.lower))
      return True;
  }
  return False;
}

/*
 * Convert seg_len bytes of UTF-8 to UCS-2 in buf, which has room for
 * seg_len characters, and return the number of characters.  A sequence
 * that is not 1 to 3 bytes long, or is cut short by the end of the text,
 * becomes '?' and consumes one byte.
 */
size_t _XmUtf8ToUcs2Buf(char *draw_text, size_t seg_len, XChar2b *buf)
{
  char *ep;
  unsigned short codepoint;
  XChar2b *ptr;
  ep = draw_text + seg_len;
  for (ptr = buf; draw_text < ep; ptr++) {
    if ((draw_text[0] & 0x80) == 0) {
      codepoint = draw_text[0];
      draw_text++;
    }
    else if ((draw_text[0] & 0x20) == 0 && ep - draw_text >= 2) {
      codepoint = (draw_text[0] & 0x1F) << 6 | (draw_text[1] & 0x3F);
      draw_text += 2;
    }
    else if ((draw_text[0] & 0x30) == 0x20 && ep - draw_text >= 3) {
      codepoint = (draw_text[0] & 0x0F) << 12 | (draw_text[1] & 0x3F) << 6 | (draw_text[2] & 0x3F);
      draw_text += 3;
    }
    else { /* wrong UTF-8 */
      codepoint = (unsigned)'?';
      draw_text++;
    }
    ptr->byte1 = (codepoint >> 8) & 0xff;
    ptr->byte2 = codepoint & 0xff;
  }
  return ptr - buf;
}

XChar2b *_XmUtf8ToUcs2(char *draw_text, size_t seg_len, size_t *ret_str_len)
{
  XChar2b *buf2b;
  /*
   * Convert to UCS2 string on the fly.
   */
  buf2b = (XChar2b *)_XmMallocArray(seg_len, sizeof(XChar2b));
  *ret_str_len = _XmUtf8ToUcs2Buf(draw_text, seg_len, buf2b);
  return buf2b;
}

/***************************************/
/********---- PUBLIC API ----**********/
/*************************************/
/************************************************************************
 *
 *  XmeWarning
 *	Build up a warning message and call Xt to get it displayed.
 *
 ************************************************************************/
void XmeWarning(Widget w, char *message)
{
  char *params[1];
  Cardinal num_params = 0;
  if (w != NULL) {
    /* the MotifWarningHandler installed in VendorS.c knows about
       this convention */
    params[0] = XME_WARNING;
    num_params++;
    XtAppWarningMsg(XtWidgetToApplicationContext(w),
                    XrmQuarkToString(w->core.xrm_name),
                    "XmeWarning",
                    w->core.widget_class->core_class.class_name,
                    message,
                    params,
                    &num_params);
  }
  else
    XtWarning(message);
}

/************************************************************************
 *
 *  XmObjectAtPoint
 *	new implementation that ask a manager class method
 *   -- Called by various widgets across Xm
 *
 ************************************************************************/
Widget XmObjectAtPoint(Widget wid, Position x, Position y)
{
  XmManagerWidgetClass mw = (XmManagerWidgetClass)XtClass(wid);
  XmManagerClassExt *mext;
  Widget return_wid = NULL;
  _XmWidgetToAppContext(wid);
  _XmAppLock(app);
  if (!XmIsManager(wid)) {
    _XmAppUnlock(app);
    return NULL;
  }
  mext = (XmManagerClassExt *)_XmGetClassExtensionPtr(
      (XmGenericClassExt *)&(mw->manager_class.extension), NULLQUARK);
  if (!*mext) {
    _XmAppUnlock(app);
    return NULL;
  }
  if ((*mext)->object_at_point)
    return_wid = ((*mext)->object_at_point)(wid, x, y);
  _XmAppUnlock(app);
  return return_wid;
}

/************************************************************************
 *
 *  _XmAssignInsensitiveColor
 *  Allocate the Gray color for display widget like insensitive.
 *
 *
 ************************************************************************/
Pixel _XmAssignInsensitiveColor(Widget w)
{
  Pixel p = 0;
  if (XmIsPrimitive(w)) {
    XmPrimitiveWidget pw = (XmPrimitiveWidget)w;
    p = pw->primitive.bottom_shadow_color;
  }
  else if (XmIsGadget(w)) {
    if (XmIsLabelGadget(w)) {
      XmLabelGadget lg = (XmLabelGadget)w;
      p = LabG_BottomShadowColor(lg);
    }
    if (XmIsIconGadget(w)) {
      XmIconGadget ig = (XmIconGadget)w;
      p = IG_BottomShadowColor(ig);
    }
  }
  else {
    p = 0;
  }
  return p;
}
