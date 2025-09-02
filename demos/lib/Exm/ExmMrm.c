/* $XConsortium: ExmMrm.c /main/4 1995/07/15 20:40:36 drk $ */
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

#include <stdio.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>

#include <Exm/Simple.h>
#include <Exm/ExmString.h>
#include <Exm/StringTrans.h>
#include <Exm/CommandB.h>
#include <Exm/MenuB.h>
#include <Exm/Grid.h>
#include <Exm/TabB.h>
#include <Exm/Panner.h>

/**********************************************************************
 *
 * Wrapper functions for MrmRegisterClass compatibility
 *
 *********************************************************************/

static Widget ExmCreateSimpleWrapper()
{
    return ExmCreateSimple(NULL, NULL, NULL, 0);
}

static Widget ExmCreateStringWrapper()
{
    return ExmCreateString(NULL, NULL, NULL, 0);
}

static Widget ExmCreateStringTransferWrapper()
{
    return ExmCreateStringTransfer(NULL, NULL, NULL, 0);
}

static Widget ExmCreateGridWrapper()
{
    return ExmCreateGrid(NULL, NULL, NULL, 0);
}

static Widget ExmCreateCommandButtonWrapper()
{
    return ExmCreateCommandButton(NULL, NULL, NULL, 0);
}

static Widget ExmCreateMenuButtonWrapper()
{
    return ExmCreateMenuButton(NULL, NULL, NULL, 0);
}

static Widget ExmCreateTabButtonWrapper()
{
    return ExmCreateTabButton(NULL, NULL, NULL, 0);
}

static Widget ExmCreatePannerWrapper()
{
    return ExmCreatePanner(NULL, NULL, NULL, 0);
}

/**********************************************************************
 *
 * ExmMrmInitialize - register Exm widget classes with Mrm
 *
 *********************************************************************/

int ExmMrmInitialize()
{
    MrmRegisterClass (MrmwcUnknown, "ExmSimple",
			"ExmCreateSimple", ExmCreateSimpleWrapper,
			exmSimpleWidgetClass);
    MrmRegisterClass (MrmwcUnknown, "ExmString",
			"ExmCreateString", ExmCreateStringWrapper,
			exmStringWidgetClass);
    MrmRegisterClass (MrmwcUnknown, "ExmStringTransfer",
			"ExmCreateStringTransfer", ExmCreateStringTransferWrapper,
			exmStringTransferWidgetClass);
    MrmRegisterClass (MrmwcUnknown, "ExmGrid",
			"ExmCreateGrid", ExmCreateGridWrapper,
			exmGridWidgetClass);
    MrmRegisterClass (MrmwcUnknown, "ExmCommandButton",
			"ExmCreateCommandButton", ExmCreateCommandButtonWrapper,
			exmCommandButtonWidgetClass);
    MrmRegisterClass (MrmwcUnknown, "ExmMenuButton",
			"ExmCreateMenuButton", ExmCreateMenuButtonWrapper,
			exmMenuButtonWidgetClass);
    MrmRegisterClass (MrmwcUnknown, "ExmTabButton",
			"ExmCreateTabButton", ExmCreateTabButtonWrapper,
			exmTabButtonWidgetClass);
    MrmRegisterClass (MrmwcUnknown, "ExmPanner",
			"ExmCreatePanner", ExmCreatePannerWrapper,
			exmPannerWidgetClass);
    return (0);
}
