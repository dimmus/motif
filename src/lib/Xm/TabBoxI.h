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
#ifndef _XmTabBoxI_h
#  define _XmTabBoxI_h
#  include <Xm/TabBoxP.h>
#  include <Xm/TabList.h>
#  ifdef __cplusplus
extern "C" {
#  endif
/* TabBox.c */
extern void _XmTabBoxSelectTab(Widget widget, int idx);
extern void _XmTabBoxStackedGeometry(XmTabBoxWidget tab, Dimension size, XRectangle *rect);
extern void _XmTabBoxGetNumRowsColumns(Widget widget, int size, int *num_rows, int *num_cols);
extern int _XiGetTabIndex(Widget tab, int row, int column);
extern int _XmTabBoxGetTabWidth(Widget widget, int idx);
extern int _XmTabBoxGetTabHeight(Widget widget, int idx);
extern Widget _XmTabBoxCanvas(Widget widget);
extern int _XmTabBoxGetMaxTabWidth(Widget widget);
extern int _XmTabBoxGetMaxTabHeight(Widget widget);
/* TabList.c */
extern int _XmTabbedStackListCount(XmTabbedStackList tab_list);
extern XmTabAttributes _XmTabbedStackListGet(XmTabbedStackList tab_list, int position);
extern XmTabAttributes _XmTabbedStackListArray(XmTabbedStackList tab_list);
#  ifdef __cplusplus
}
#  endif
#endif /* _XmTabBoxI_h */
