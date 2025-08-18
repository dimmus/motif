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

#ifndef _XmDataFSelP_h
#define _XmDataFSelP_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    External (DataF.c) function declaration *******/
#ifdef _NO_PROTO
extern Widget _XmDataFieldGetDropReciever() ;
#else
extern Widget _XmDataFieldGetDropReciever( Widget w ) ;
#endif /* _NO_PROTO */


/********    Private Function Declarations    ********/
#ifdef _NO_PROTO

extern Boolean _XmDataFieldConvert() ;
extern void _XmDataFieldLoseSelection() ;

#else

extern Boolean _XmDataFieldConvert(
                        Widget w,
                        Atom *selection,
                        Atom *target,
                        Atom *type,
                        XtPointer *value,
                        unsigned long *length,
                        int *format) ;
extern void _XmDataFieldLoseSelection(
                        Widget w,
                        Atom *selection) ;

#endif /* _NO_PROTO */
/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDataFSelP_h */
/* DON't ADD STUFF AFTER THIS #endif */
