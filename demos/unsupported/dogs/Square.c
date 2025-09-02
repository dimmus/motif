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
/*
 * HISTORY
*/
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Square.c /main/6 1998/03/25 18:18:13 csn $"
#endif
#endif

/*****************************************************************************
*
*  Square.c - widget source file
*
******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Xm/BulletinBP.h>
#include "SquareP.h"

#define Width(w) XmField((w),square_offsets,Core,width,Dimension)
#define Height(w) XmField((w),square_offsets,Core,height,Dimension)
#define BorderWidth(w) XmField((w),square_offsets,Core,border_width,Dimension)
#define MajorDimension(w) XmField((w),square_offsets,Square,major_dimension,int)

#define MakeSquare(w) XmConstraintField((w),square_constraint_offsets, \
			Square,make_square, Boolean)

static void ClassInitialize();
static void Initialize(SquareWidget req, SquareWidget new);
static Boolean SetValues(Widget current, Widget request, Widget new, ArgList args, Cardinal *num_args);
static void ConstraintInitialize(Widget req, Widget new);
static Boolean ConstraintSetValues(Widget old, Widget ref, Widget new, ArgList args, Cardinal *num_args);

static XmPartResource resources[] = {
    {
    SquareNmajorDimension, SquareCMajorDimension, XmRInt, sizeof(int),
    XmPartOffset(Square,major_dimension), XmRImmediate, (void *)SquareWIDTH
    }
};

static XmPartResource constraints[] = {
   {
    SquareNmakeSquare, SquareCMakeSquare, XmRBoolean, sizeof(Boolean),
    XmConstraintPartOffset(Square,make_square),
    XmRImmediate, (void *)False
   }
};


externaldef(squareclassrec) SquareClassRec squareClassRec =
{
   {                    /* core_class fields    */
      (WidgetClass) &xmBulletinBoardClassRec,   /* superclass   */
      "Square",                 /* class_name           */
      sizeof(SquarePart),       /* widget_size          */
      ClassInitialize,          /* class_initialize     */
      NULL,                     /* class init part proc */
      False,                    /* class_inited         */
      (XtInitProc)Initialize,   /* initialize           */
      NULL,                     /* initialize_notify    */
      XtInheritRealize,         /* realize              */
      NULL,                     /* actions              */
      0,                        /* num_actions          */
      (XtResourceList)resources,/* resources            */
      XtNumber(resources),      /* num_resources        */
      NULLQUARK,                /* xrm_class            */
      False,                    /* compress_motion      */
      XtExposeCompressMaximal,  /* compress_exposure    */
      False,                    /* compress_enterleave  */
      False,                    /* visible_interest     */
      NULL,                     /* destroy              */
      XtInheritResize,          /* resize               */
      XtInheritExpose,          /* expose               */
      SetValues,                /* set_values           */
      NULL,                     /* set_values_hook      */
      XtInheritSetValuesAlmost, /* set_values_almost    */
      NULL,                     /* get_values_hook      */
      NULL,                     /* accept_focus         */
      XtVersionDontCheck,       /* version              */
      NULL,                     /* callback_private     */
      XtInheritTranslations,    /* tm_table             */
      XtInheritQueryGeometry,   /* Query Geometry proc  */
      NULL,                     /* disp accelerator     */
      NULL,                     /* extension            */
   },

   {                    /* composite_class fields */
      XtInheritGeometryManager, /* geometry_manager       */
      XtInheritChangeManaged,   /* change_managed         */
      XtInheritInsertChild,     /* insert_child           */
      XtInheritDeleteChild,     /* delete_child           */
      NULL,                     /* extension              */
   },

   {                    /* constraint_class fields */
      (XtResourceList)constraints,  /* constraint resource     */
      XtNumber(constraints),        /* number of constraints   */
      sizeof(SquareConstraintRec), /* size of constraint      */
      (XtInitProc)ConstraintInitialize, /* initialization          */
      NULL,                         /* destroy proc            */
      ConstraintSetValues,          /* set_values proc         */
      NULL,                         /* extension               */
   },

   {                    /* manager_class fields   */
      XtInheritTranslations,        /* translations           */
      NULL,                         /* syn_resources          */
      0,                            /* num_syn_resources      */
      NULL,                         /* syn_cont_resources     */
      0,                            /* num_syn_cont_resources */
      XmInheritParentProcess,       /* parent_process         */
      NULL,                         /* extension              */
   },

   {                    /* bulletin_board_class fields */
      FALSE,                        /* always_install_accelerators */
      NULL,                         /* geo_matrix_create  */
      XmInheritFocusMovedProc,      /* focus_moved_proc   */
      NULL,                         /* extension          */
   },

   {                    /* square_class fields  */
      NULL,                         /* extension          */
   }
};

externaldef(squarewidgetclass) WidgetClass
	squareWidgetClass = (WidgetClass) &squareClassRec;

static XmOffsetPtr square_offsets;		/* Part offsets table */
static XmOffsetPtr square_constraint_offsets;	/* Constraint offsets table */

/**********************************************************************
 *
 * SquareCreate - Convenience routine, used by Uil/Mrm.
 *
 *********************************************************************/

Widget SquareCreate(Widget parent, char *name, ArgList arglist, Cardinal nargs)
{
    return(XtCreateWidget (name, squareWidgetClass, parent, arglist, nargs));
}

/**********************************************************************
 *
 * SquareMrmInitialize - register Square widget class with Mrm
 *
 *********************************************************************/

int SquareMrmInitialize()
{
    return(MrmRegisterClass (MrmwcUnknown, "Square", "SquareCreate",
			     (Widget (*)(void))SquareCreate, (WidgetClass)&squareClassRec));
}

/**********************************************************************
 *
 * Class methods
 *
 *********************************************************************/

static void ClassInitialize()
{
    XmeResolvePartOffsets(squareWidgetClass, &square_offsets,
				&square_constraint_offsets);
}

static void Initialize(SquareWidget req, SquareWidget new)
{
    if (MajorDimension(new) != SquareWIDTH &&
	MajorDimension(new) != SquareHEIGHT) {
	XtWarning("Square: invalid majorDimension");
	MajorDimension(new) = SquareWIDTH;
    }
}

static Boolean SetValues(Widget current, Widget request, Widget new, ArgList args, Cardinal *num_args)
{
    SquareWidget curr = (SquareWidget)current;
    SquareWidget req = (SquareWidget)request;
    SquareWidget nw = (SquareWidget)new;
    
    if (MajorDimension(nw) != SquareWIDTH &&
	MajorDimension(nw) != SquareHEIGHT) {
	XtWarning("Square: invalid majorDimension");
	MajorDimension(nw) = MajorDimension(curr);
    }
    return (False);
}

static void ConstraintInitialize(Widget req, Widget new)
{
    Dimension m;

    if(MakeSquare(new) == True) {
        if (MajorDimension(XtParent(new))==SquareWIDTH)
            m = Width(new);
        else
            m = Height(new);

	XtResizeWidget(new, m, m, BorderWidth(new));
    }
}

static Boolean ConstraintSetValues(Widget old, Widget ref, Widget new, ArgList args, Cardinal *num_args)
{
    Boolean redraw = False;

    if (MakeSquare(new) != MakeSquare(old)) {
	if(MakeSquare(new)==True) {
            if (MajorDimension(XtParent(new))==SquareWIDTH)
        	Height(new) = Width(new);
            else
        	Width(new) = Height(new);
	}
	else {
	    XtWidgetGeometry gi;
	    XtWidgetGeometry gp;

            if (MajorDimension(XtParent(new))==SquareWIDTH)
        	Height(new) = Height(new)/2;
            else
        	Width(new) = Width(new)/2;

	    gi.request_mode = CWWidth | CWHeight;
	    gi.width = Width(new);
	    gi.height = Height(new);
	    if (XtQueryGeometry((Widget)new, &gi, &gp) == XtGeometryAlmost) {
		if (gp.request_mode && CWWidth != 0) Width(new) = gp.width;
		if (gp.request_mode && CWHeight != 0) Height(new) = gp.height;
	    }
	}
	redraw = True;
    }
    return (redraw);
}
