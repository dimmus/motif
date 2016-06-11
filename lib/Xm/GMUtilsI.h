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
#ifndef _XmGMUtilsI_h
#define _XmGMUtilsI_h


/* Include files:
*/
#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern void _XmGMCalcSize( 
                        XmManagerWidget manager,
#if NeedWidePrototypes
                        int margin_width,
                        int margin_height,
#else
                        Dimension margin_width,
                        Dimension margin_height,
#endif /* NeedWidePrototypes */
                        Dimension *replyWidth,
                        Dimension *replyHeight) ;
extern Boolean _XmGMDoLayout( 
                        XmManagerWidget manager,
#if NeedWidePrototypes
                        int margin_width,
                        int margin_height,
#else
                        Dimension margin_width,
                        Dimension margin_height,
#endif /* NeedWidePrototypes */
                        int resize_policy,
#if NeedWidePrototypes
                        int queryonly) ;
#else
                        Boolean queryonly) ;
#endif /* NeedWidePrototypes */
extern void _XmGMEnforceMargin( 
                        XmManagerWidget manager,
#if NeedWidePrototypes
                        int margin_width,
                        int margin_height,
                        int setvalue) ;
#else
                        Dimension margin_width,
                        Dimension margin_height,
                        Boolean setvalue) ;
#endif /* NeedWidePrototypes */
extern XtGeometryResult _XmGMHandleQueryGeometry( 
                        Widget widget,
                        XtWidgetGeometry *intended,
                        XtWidgetGeometry *desired,
#if NeedWidePrototypes
                        int margin_width,
                        int margin_height,
#else
                        Dimension margin_width,
                        Dimension margin_height,
#endif /* NeedWidePrototypes */
                        int resize_policy) ;
extern Boolean _XmGMOverlap( 
                        XmManagerWidget manager,
                        Widget w) ;
extern XtGeometryResult _XmGMHandleGeometryManager( 
                        Widget parent,
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply,
#if NeedWidePrototypes
                        int margin_width,
                        int margin_height,
#else
                        Dimension margin_width,
                        Dimension margin_height,
#endif /* NeedWidePrototypes */
                        int resize_policy,
                        int allow_overlap) ;
/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmGMUtilsI_h */
 /* DON'T ADD STUFF AFTER THIS #endif */
