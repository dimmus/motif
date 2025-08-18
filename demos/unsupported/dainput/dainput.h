/* $XConsortium: dainput.h /main/5 1995/07/15 20:47:24 drk $ */
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
#ifndef _dainput_h_
#define _dainput_h_


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern void DaExit(Widget widget,
                 XtPointer client_data,
                 XtPointer calldata);

extern void DaOverView(Widget widget,
                     XtPointer client_data,
                     XtPointer calldata);

extern void DaRedrawText(Widget widget,
                       XtPointer client_data,
                       XtPointer calldata);

extern void DaInputText(Widget widget,
                      XtPointer client_data,
                      XtPointer calldata);

extern void DaResizedText(Widget widget,
                        XtPointer client_data,
                        XtPointer calldata);

extern void DaFocusHandler(Widget widget,
                         XtPointer client_data,
                         XEvent * event,
                         Boolean * cont);

extern Widget DaCreateGui(char * name,
			  char * aclass,
			  int argc,
			  char ** argv,
			  Display * display);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif



#endif
