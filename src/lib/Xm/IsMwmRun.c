/* $XConsortium: IsMwmRun.c /main/7 1996/05/21 12:02:11 pascale $ */
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
#include "XmI.h"
#include <Xm/MwmUtil.h>
#include <Xm/XmP.h>

/************************************************************************
 *
 *  XmIsMotifWMRunning
 *
 ************************************************************************/
Boolean XmIsMotifWMRunning(Widget shell)
{
  Atom motif_wm_info_atom;
  unsigned long num_items;
  PropMotifWmInfo *prop = 0;
  Boolean found;
  Window root = RootWindowOfScreen(XtScreen(shell));
  _XmWidgetToAppContext(shell);
  _XmAppLock(app);
  motif_wm_info_atom = XInternAtom(XtDisplay(shell), _XA_MOTIF_WM_INFO, FALSE);
  _XmProcessLock();
  found = _XmGetWindowPropertyChecked(XtDisplay(shell),
                                      root,
                                      motif_wm_info_atom,
                                      (long)PROP_MOTIF_WM_INFO_ELEMENTS,
                                      motif_wm_info_atom,
                                      32,
                                      PROP_MOTIF_WM_INFO_ELEMENTS,
                                      NULL,
                                      NULL,
                                      &num_items,
                                      NULL,
                                      (unsigned char **)&prop);
  _XmProcessUnlock();
  if (!found) {
    _XmAppUnlock(app);
    return (FALSE);
  }
  else {
    Window wm_window = (Window)prop->wmWindow;
    Window top, parent, *children;
    unsigned int num_children;
    Boolean returnVal;
    Cardinal i;
    if (XQueryTree(XtDisplay(shell), root, &top, &parent, &children, &num_children)) {
      i = 0;
      while ((i < num_children) && (children[i] != wm_window))
        i++;
      returnVal = (i == num_children) ? FALSE : TRUE;
    }
    else
      returnVal = FALSE;
    if (prop)
      XFree((char *)prop);
    if (children)
      XFree((char *)children);
    _XmAppUnlock(app);
    return (returnVal);
  }
}
