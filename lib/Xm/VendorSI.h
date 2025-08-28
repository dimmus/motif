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
/* $XConsortium: VendorSI.h /main/5 1995/07/13 18:19:43 drk $ */
#ifndef _XmVendorSI_h
#define _XmVendorSI_h
#include <Xm/VendorSP.h>
#ifdef __cplusplus
extern "C" {
#endif
/* used in visual resources declaration and in _XmDefaultVisualResources.
   Cannot use 0 which is None = CopyFromParent, the Xt default */
#define INVALID_VISUAL ((Visual*)-1)
/********    Private Function Declarations    ********/
extern void _XmAddGrab(
                        Widget wid,
                        Boolean exclusive,
                        Boolean spring_loaded) ;
extern void _XmRemoveGrab(
                        Widget wid) ;
extern void _XmDefaultVisualResources(Widget widget) ;
/********    End Private Function Declarations    ********/
#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif
#endif /* _XmVendorSI_h */
