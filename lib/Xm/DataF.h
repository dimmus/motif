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
#ifndef _XmDataF_h
#define _XmDataF_h

#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/Ext.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _XmDataFieldClassRec    *XmDataFieldWidgetClass;
typedef struct _XmDataFieldRec         *XmDataFieldWidget;

/*      Function Name: XmCreateDataField
 *      Description: Creation Routine for UIL and ADA.
 *      Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *      Returns: The Widget created.
 */

Widget XmCreateDataField(
#ifndef _NO_PROTO
Widget, String, ArgList, Cardinal
#endif
);

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateDataField(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedDataField(
                        Widget parent,
                        char *name,
                        ...);

Boolean _XmDataFieldReplaceText(
#ifndef _NO_PROTO
XmDataFieldWidget, XEvent*, XmTextPosition, XmTextPosition, char*, int, Boolean
#endif
);

void XmDataFieldSetString(
#ifndef _NO_PROTO
Widget, char*
#endif
);

extern char * XmDataFieldGetString(
#ifndef _NO_PROTO
Widget
#endif
);

extern wchar_t * XmDataFieldGetStringWcs(
#ifndef _NO_PROTO
Widget
#endif
);

void _XmDataFieldSetClipRect(
#ifndef _NO_PROTO
XmDataFieldWidget
#endif
);

void _XmDataFieldDrawInsertionPoint(
#ifndef _NO_PROTO
XmDataFieldWidget, Boolean
#endif
);

void XmDataFieldSetHighlight(
#ifndef _NO_PROTO
Widget, XmTextPosition, XmTextPosition, XmHighlightMode
#endif
);

void XmDataFieldSetAddMode(
#ifndef _NO_PROTO
Widget, Boolean
#endif
);

char * XmDataFieldGetSelection(
#ifndef _NO_PROTO
Widget
#endif
);

void XmDataFieldSetSelection(
#ifndef _NO_PROTO
Widget, XmTextPosition, XmTextPosition, Time
#endif
);

void _XmDataFieldSetSel2(
#ifndef _NO_PROTO
Widget, XmTextPosition, XmTextPosition, Boolean, Time
#endif
);

Boolean XmDataFieldGetSelectionPosition(
#ifndef _NO_PROTO
Widget, XmTextPosition *, XmTextPosition *
#endif
);

XmTextPosition XmDataFieldXYToPos(
#ifndef _NO_PROTO
Widget, Position, Position
#endif
);

void XmDataFieldShowPosition(
#ifndef _NO_PROTO
Widget, XmTextPosition
#endif
);

Boolean XmDataFieldCut(
#ifndef _NO_PROTO
Widget, Time
#endif
);

Boolean XmDataFieldCopy(
#ifndef _NO_PROTO
Widget, Time
#endif
);

Boolean XmDataFieldPaste(
#ifndef _NO_PROTO
Widget
#endif
);

void XmDataFieldSetEditable(
#ifndef _NO_PROTO
Widget, Boolean
#endif
);

void XmDataFieldSetInsertionPosition(
#ifndef _NO_PROTO
Widget, XmTextPosition
#endif
);

extern WidgetClass xmDataFieldWidgetClass;

typedef struct _XmDataFieldCallbackStruct {
    Widget   w;			/* The XmDataField */
    String   text;		/* Proposed string */
    Boolean  accept;		/* Accept return value, for validation */
} XmDataFieldCallbackStruct;

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* _XmDataF_h */
