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
#ifndef _XmI18ListI_h
#  define _XmI18ListI_h
#  include <Xm/MultiListP.h>
#  ifdef __cplusplus
extern "C" {
#  endif
/* Defined in I18List.c, used by MultiList.c (and exported). */
extern XmMultiListRowInfo *XmI18ListFindRow(Widget, String, int *, Boolean, Boolean);
extern int *XmI18ListGetSelectedRowArray(XmI18ListWidget, int *);
extern void XmI18ListMakeRowVisible(XmI18ListWidget, int);
extern void XmI18ListDeselectRow(XmI18ListWidget, int);
extern void XmI18ListSelectRow(XmI18ListWidget, int, Boolean);
extern void XmI18ListSelectAllItems(XmI18ListWidget, Boolean);
extern void XmI18ListDeselectItems(XmI18ListWidget, XmString, int);
extern void XmI18ListSelectItems(XmI18ListWidget, XmString, int, Boolean);
#  ifdef __cplusplus
}
#  endif
#endif /* _XmI18ListI_h */
