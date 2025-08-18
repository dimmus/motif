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
/* $XConsortium: DragOverSI.h /main/6 1995/07/14 10:26:24 drk $ */
#ifndef _XmDragOverSI_h
#define _XmDragOverSI_h

#include <Xm/DragOverSP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for DragOverS.c    ********/

extern void _XmDragOverHide(
                        Widget w,
#if NeedWidePrototypes
                        int clipOriginX,
                        int clipOriginY,
#else
                        Position clipOriginX,
                        Position clipOriginY,
#endif /* NeedWidePrototypes */
                        XmRegion clipRegion) ;
extern void _XmDragOverShow(
                        Widget w,
#if NeedWidePrototypes
                        int clipOriginX,
                        int clipOriginY,
#else
                        Position clipOriginX,
                        Position clipOriginY,
#endif /* NeedWidePrototypes */
                        XmRegion clipRegion) ;
extern void _XmDragOverMove(
                        Widget w,
#if NeedWidePrototypes
                        int x,
                        int y) ;
#else
                        Position x,
                        Position y) ;
#endif /* NeedWidePrototypes */
extern void _XmDragOverChange(
                        Widget w,
#if NeedWidePrototypes
                        unsigned int dropSiteStatus) ;
#else
                        unsigned char dropSiteStatus) ;
#endif /* NeedWidePrototypes */
extern void _XmDragOverFinish(
                        Widget w,
#if NeedWidePrototypes
                        unsigned int completionStatus) ;
#else
                        unsigned char completionStatus) ;
#endif /* NeedWidePrototypes */

extern Cursor _XmDragOverGetActiveCursor(
			Widget w) ;
extern void _XmDragOverSetInitialPosition(
			Widget w,
#if NeedWidePrototypes
			int initialX,
			int initialY) ;
#else
			Position initialX,
			Position initialY) ;
#endif /* NeedWidePrototypes */

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDragOverSI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
