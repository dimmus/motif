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
#ifndef _XmDataFI_h
#  define _XmDataFI_h
#  include <Xm/DataFP.h>
#  ifdef __cplusplus
extern "C" {
#  endif
/* Defined in DataF.c, used by DataFSel.c. */
extern int _XmDataFieldCountBytes(XmDataFieldWidget tf, wchar_t *wc_value, int num_chars);
extern void _XmDataFielddf_SetCursorPosition(XmDataFieldWidget tf,
                                             XEvent *event,
                                             XmTextPosition position,
                                             Boolean adjust_flag,
                                             Boolean call_cb);
extern void _XmDataFieldDeselectSelection(Widget w, Boolean disown, Time sel_time);
extern Boolean _XmDataFielddf_SetDestination(Widget w, XmTextPosition position, Time set_time);
extern void _XmDataFieldStartSelection(XmDataFieldWidget tf,
                                       XmTextPosition left,
                                       XmTextPosition right,
                                       Time sel_time);
#  ifdef __cplusplus
}
#  endif
#endif /* _XmDataFI_h */
