/* $XConsortium: Obso1_2.c /main/7 1996/06/14 23:10:17 pascale $ */
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
 *
 */
/*
 * HISTORY
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "BulletinBI.h"
#include "MessagesI.h"
#include "SelectioBI.h"
#include "SyntheticI.h"
#include "TraversalI.h"
#include "XmI.h"
#include "XmStringI.h" /* for _XmStringGetTextConcat() */
#include <Xm/BaseClassP.h>
#include <Xm/DesktopP.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawingAP.h>
#include <Xm/FileSBP.h>
#include <Xm/GadgetP.h>
#include <Xm/List.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuShellP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/PushBGP.h>
#include <Xm/PushBP.h>
#include <Xm/RowColumnP.h>
#include <Xm/ScaleP.h>
#include <Xm/ScreenP.h>
#include <Xm/ScrolledWP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/TransltnsP.h>
#include <Xm/VendorSEP.h>
#include <Xm/XmP.h>
#include <ctype.h>
#include <stdlib.h>

/********    Static Function Declarations    ********/
/********    End Static Function Declarations    ********/

/* Obsolete entry points, still exported for programs built against old
   releases.  They are deliberately not declared in any installed header. */
extern void SetMwmStuff(XmVendorShellExtObject ove, XmVendorShellExtObject nve);
extern int XmTextFieldGetBaseLine(Widget w);
extern int XmTextGetBaseLine(Widget w);
extern Boolean _XmChangeNavigationType(Widget current, XmNavigationType newNavType);
extern void _XmDrawShadow(Display *display, Drawable d, GC top_GC, GC bottom_GC, int size, int x,
                          int y, int width, int height);
extern void _XmHighlightBorder(Widget w);
extern void _XmTextFieldDestinationVisible(Widget w, Boolean turn_on);
extern void _XmUnhighlightBorder(Widget w);
externaldef(desktopobjectclass) WidgetClass xmDesktopObjectClass = (WidgetClass)&xmDesktopClassRec;
externaldef(displayobjectclass)
    WidgetClass xmDisplayObjectClass = (WidgetClass)(&xmDisplayClassRec);
externaldef(screenobjectclass) WidgetClass xmScreenObjectClass = (WidgetClass)(&xmScreenClassRec);
externaldef(worldobjectclass) WidgetClass xmWorldObjectClass = (WidgetClass)NULL;

/* WorldP.h was defuncted for 2.0 , no more ref to (&xmWorldClassRec) */
int XmTextFieldGetBaseLine(Widget w)
{
  return XmTextFieldGetBaseline(w);
}

int XmTextGetBaseLine(Widget w)
{
  return XmTextGetBaseline(w);
}

Boolean _XmChangeNavigationType(Widget current, XmNavigationType newNavType)
{
  /* This is a convenience routine for widgets wanting to change
   * their navigation type without using XtSetValues().
   */
  XmFocusData focusData;
  Widget new_wid = current->core.self;
  XmNavigationType curNavType = _XmGetNavigationType(current);
  XmTravGraph tgraph;
  if ((curNavType != newNavType) && (focusData = _XmGetFocusData(new_wid)) &&
      (tgraph = &(focusData->trav_graph))->num_entries)
  {
    _XmTravGraphUpdate(tgraph, new_wid);
    if ((focusData->focus_policy == XmEXPLICIT) && (focusData->focus_item == new_wid) &&
        !XmIsTraversable(new_wid))
    {
      Widget new_focus = _XmTraverseAway(
          tgraph, new_wid, (focusData->active_tab_group != new_wid));
      if (!new_focus) {
        new_focus = new_wid;
      }
      _XmMgrTraversal(new_focus, XmTRAVERSE_CURRENT);
    }
  }
  return TRUE;
}

/********************************************************************/
/* Following is the old code needed for subclasses already using it */
/* They can either define the macro and change nothing (but suffer
   the addition of code and the drawing performance) or adapt to the
   new interface (better) */
/************************************************************************
 *
 *  Primitive:_XmDrawShadow, become XmDrawShadow
 *
 *      Draw an n segment wide bordering shadow on the drawable
 *      d, using the provided GC's and rectangle.
 *
 ************************************************************************/
void _XmDrawShadow(Display *display,
                   Drawable d,
                   GC top_GC,
                   GC bottom_GC,
                   int size,
                   int x,
                   int y,
                   int width,
                   int height)
{
  static XRectangle *rects = NULL;
  static int rect_count = 0;
  int i;
  int size2;
  int size3;
  if (size <= 0)
    return;
  if (size > width / 2)
    size = width / 2;
  if (size > height / 2)
    size = height / 2;
  if (size <= 0)
    return;
  /* rects is shared by every call */
  _XmProcessLock();
  if (rect_count == 0) {
    rects = (XRectangle *)_XmMallocArray(size, 4 * sizeof(XRectangle));
    rect_count = size;
  }
  if (rect_count < size) {
    rects = (XRectangle *)_XmReallocArray((char *)rects, size, 4 * sizeof(XRectangle));
    rect_count = size;
  }
  size2 = size + size;
  size3 = size2 + size;
  for (i = 0; i < size; i++) {
    /*  Top segments  */
    rects[i].x = x;
    rects[i].y = y + i;
    rects[i].width = width - i;
    rects[i].height = 1;
    /*  Left segments  */
    rects[i + size].x = x + i;
    rects[i + size].y = y;
    rects[i + size].width = 1;
    rects[i + size].height = height - i;
    /*  Bottom segments  */
    rects[i + size2].x = x + i + 1;
    rects[i + size2].y = y + height - i - 1;
    rects[i + size2].width = width - i - 1;
    rects[i + size2].height = 1;
    /*  Right segments  */
    rects[i + size3].x = x + width - i - 1;
    rects[i + size3].y = y + i + 1;
    rects[i + size3].width = 1;
    rects[i + size3].height = height - i - 1;
  }
  XFillRectangles(display, d, top_GC, &rects[0], size2);
  XFillRectangles(display, d, bottom_GC, &rects[size2], size2);
  _XmProcessUnlock();
}

void SetMwmStuff(XmVendorShellExtObject ove, /* unused */
                 XmVendorShellExtObject nve) /* unused */
{
  /* Sorry Charlie, this doesn't work any more.
   */
  return;
}

static Boolean _isISO(String charset)
{
  int i;
  if (strlen(charset) == 5) {
    for (i = 0; i < 5; i++) {
      if (!isdigit((unsigned char)charset[i]))
        return (False);
    }
    return (True);
  }
  else
    return (False);
}

char *_XmCharsetCanonicalize(String charset)
{
  String new_s;
  int len;
  /* ASCII -> ISO8859-1 */
  if (!strcmp(charset, "ASCII")) {
    new_s = XtNewString(XmSTRING_ISO8859_1);
  }
  else if (_isISO(charset)) {
    /* "ISO####-#" */
    len = 3 + 4 + 1 + 1 + 1;
    new_s = XtMalloc(len);
    snprintf(new_s, len, "ISO%s", charset);
    new_s[7] = '-';
    new_s[8] = charset[4];
    new_s[9] = '\0';
  }
  else
  /* Anything else is copied but not modified. */
  {
    len = strlen(charset) + 1;
    new_s = XtMalloc(len);
    strncpy(new_s, charset, len);
  }
  return (new_s);
}

XContext _XmMenuCursorContext = 0; /* This won't help much either. */

void _XmTextFieldDestinationVisible(Widget w,        /* unused */
                                    Boolean turn_on) /* unused */
{
  return;
}

_XmConst char *_XmTextEventBindings1 = _XmTextIn_XmTextEventBindings1;
_XmConst char *_XmTextEventBindings2 = _XmTextIn_XmTextEventBindings2;
_XmConst char *_XmTextEventBindings3 = _XmTextIn_XmTextEventBindings3;

/************************************************************************
 *
 *  The border highlighting and unhighlighting routines.
 *
 *  These routines were originally in Primitive.c but not used anywhere.
 *
 ************************************************************************/
void _XmHighlightBorder(Widget w)
{
  if (XmIsPrimitive(w)) {
    (*(xmPrimitiveClassRec.primitive_class.border_highlight))(w);
  }
  else {
    if (XmIsGadget(w)) {
      (*(xmGadgetClassRec.gadget_class.border_highlight))(w);
    }
  }
  return;
}

void _XmUnhighlightBorder(Widget w)
{
  if (XmIsPrimitive(w)) {
    (*(xmPrimitiveClassRec.primitive_class.border_unhighlight))(w);
  }
  else {
    if (XmIsGadget(w)) {
      (*(xmGadgetClassRec.gadget_class.border_unhighlight))(w);
    }
  }
  return;
}
