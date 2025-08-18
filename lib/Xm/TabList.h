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
#ifndef _Xm_TabList_h_
#define _Xm_TabList_h_

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XmCOLOR_DYNAMIC ((Pixel)-1)
#define XmPIXMAP_DYNAMIC ((Pixmap) 3)
#define XmTAB_LAST_POSITION -1
#define XmTAB_NOT_FOUND -1

#define XmTAB_LABEL_STRING      (1L<<0)
#define XmTAB_LABEL_PIXMAP      (1L<<1)
#define XmTAB_PIXMAP_PLACEMENT  (1L<<2)
#define XmTAB_BACKGROUND        (1L<<3)
#define XmTAB_FOREGROUND        (1L<<4)
#define XmTAB_VALUE_MODE        (1L<<5)
#define XmTAB_LABEL_ALIGNMENT   (1L<<6)
#define XmTAB_STRING_DIRECTION  (1L<<7)
#define XmTAB_BACKGROUND_PIXMAP	(1L<<8)
#define XmTAB_SENSITIVE         (1L<<9)
#define XmTAB_ALL_FLAGS         (XmTAB_LABEL_STRING|XmTAB_LABEL_PIXMAP|\
				 XmTAB_PIXMAP_PLACEMENT|XmTAB_BACKGROUND|\
				 XmTAB_FOREGROUND|XmTAB_VALUE_MODE|\
				 XmTAB_LABEL_ALIGNMENT|XmTAB_STRING_DIRECTION|\
				 XmTAB_BACKGROUND_PIXMAP|XmTAB_SENSITIVE)

typedef struct _XmTabAttributeRec {
    XmString          label_string;	/* default: NULL                     */
    XmStringDirection string_direction; /* default: XmSTRING_DIRECTION_L_TO_R*/
    Pixmap            label_pixmap;	/* default: XmUNSPECIFIED_PIXMAP     */
    int               label_alignment;  /* default: XmALIGNEMENT_CENTER      */
    XmPixmapPlacement pixmap_placement; /* default: XmPIXMAP_RIGHT           */
    Pixel             foreground;       /* default: XmCOLOR_DYNAMIC          */
    Pixel             background;       /* default: XmCOLOR_DYNAMIC          */
    Pixmap            background_pixmap;/* default: XmPIXMAP_DYNAMIC         */
    Boolean	      sensitive;        /* default: True                     */
    XmTabValueMode    value_mode;       /* default: XmTAB_VALUE_COPY         */
} XmTabAttributeRec, * XmTabAttributes;

typedef struct _XmTabbedStackListRec *XmTabbedStackList;

XmTabbedStackList XmTabbedStackListCreate(
#ifndef _NO_PROTO
void
#endif
);

XmTabbedStackList XmTabbedStackListCopy(
#ifndef _NO_PROTO
XmTabbedStackList
#endif
);

void XmTabbedStackListFree(
#ifndef _NO_PROTO
XmTabbedStackList
#endif
);

void XmTabbedStackListRemove(
#ifndef _NO_PROTO
XmTabbedStackList, int
#endif
);

int XmTabbedStackListInsert(
#ifndef _NO_PROTO
XmTabbedStackList, int, XtValueMask, XmTabAttributes
#endif
);

int XmTabbedStackListAppend(
#ifndef _NO_PROTO
XmTabbedStackList, XtValueMask, XmTabAttributes
#endif
);

void XmTabbedStackListModify(
#ifndef _NO_PROTO
XmTabbedStackList, int, XtValueMask, XmTabAttributes
#endif
);

void XmTabbedStackListQuery(
#ifndef _NO_PROTO
XmTabbedStackList, int, XmTabAttributes
#endif
);

int XmTabbedStackListFind(
#ifndef _NO_PROTO
XmTabbedStackList, XmString
#endif
);

void XmTabbedStackListSimpleRemove(
#ifndef _NO_PROTO
XmTabbedStackList, XmString
#endif
);

int XmTabbedStackListSimpleInsert(
#ifndef _NO_PROTO
XmTabbedStackList, int, XmString
#endif
);

int XmTabbedStackListSimpleAppend(
#ifndef _NO_PROTO
XmTabbedStackList, XmString
#endif
);

void XmTabbedStackListSimpleModify(
#ifndef _NO_PROTO
XmTabbedStackList, int, XmString
#endif
);

XmString XmTabbedStackListSimpleQuery(
#ifndef _NO_PROTO
XmTabbedStackList, int
#endif
);

XmTabResult XmTabbedStackListCompare(
#ifndef _NO_PROTO
XmTabbedStackList, XmTabbedStackList
#endif
);

void XmTabAttibutesFree(
#ifndef _NO_PROTO
XmTabAttributes
#endif
);

#ifdef __cplusplus
}	/* Closes scope of 'extern "C"' declaration */
#endif


#endif
