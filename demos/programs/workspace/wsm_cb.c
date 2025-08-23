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
static char rcsid[] = "$TOG: wsm_cb.c /main/8 1997/05/02 10:05:18 dbl $"
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>

#include "wsm.h"
#include "wsmStruct.h"
#include "wsm_ui.h"
#include "wsmDebug.h"
#include "xrmLib.h"
#include "wsm_cb.h"
#include "command_ui.h"
#include "wsm_create.h"

extern Boolean wsm_shown;
extern Widget shell;
extern WSM_UI* wsm_ui;
extern Boolean connected;
extern Boolean mwm_gone;
extern Widget fromField;
extern Widget toField;
extern Space* all_space;
extern Space* current_space;
extern Space* space_list;
extern char *file_name;

Boolean copy_mode = True;

/**********************************************************************/
/*                        WSM Dialog Interface                       */
/**********************************************************************/
void
InitializeInterface(WSM_UI* wsm_ui_data)
{
  int num_rooms = GetNumberSpaces();
  int max_num_rooms = 20;

  wsm_ui_data->space_button = (Widget*) XtMalloc(max_num_rooms*sizeof(Widget));
  wsm_ui_data->num_space_buttons = num_rooms;
  wsm_ui_data->from_option_button = (Widget*) XtMalloc(max_num_rooms*sizeof(Widget));
  wsm_ui_data->to_option_button = (Widget*)XtMalloc((max_num_rooms +1)*sizeof(Widget));
  wsm_ui_data->from_space = NULL;
  wsm_ui_data->to_space = NULL;
}

void
HideWsm()
{
    wsm_shown = False;
    ShowWsmCommand();
/*  XtPopdown(XtParent(XtParent(wsm_ui->wsm_row_column)));*/
  XtPopdown(XtParent(wsm_ui->configure_form));
  XtPopdown(XtParent(wsm_ui->name_form));
  XtPopdown(XtParent(wsm_ui->delete_form));
  XtPopdown(XtParent(wsm_ui->save_as_form));
  XtUnmapWidget(shell);
}



void
ShowWsm()
{
    wsm_shown = True;
    ShowWsmCommand();
/*  XtPopup(XtParent(XtParent(wsm_ui->wsm_row_column)),XtGrabNone);*/
  XtMapWidget(shell);
}

/**********************************************************************/
/*                       Workspace Panel                              */
/**********************************************************************/

Widget
CreateWorkspacePanel(Widget shell_widget, WSM_UI* wsm_ui_data, Boolean show_menu)
{
  Widget parent;
  XmString xmstr;
  int i;
  Space *space;
  parent = CreateWorkspacePanelBX(shell,wsm_ui, show_menu);

  space = space_list;
  for (i = 0; space != NULL && i < wsm_ui->num_space_buttons; i++)
    {
      xmstr = XmStringLtoRCreate(space->name, XmSTRING_DEFAULT_CHARSET);
      XtVaSetValues(wsm_ui_data->space_button[i],XmNlabelString,xmstr,NULL);
      XmStringFree(xmstr);
      space = space->next;
    }
  return parent;
}

void
CreateNewSpaceButton(int i, char *name, WSM_UI *wsm_ui_data)
{
  int argcnt;
  Arg args[15];
  XmString xmstr;
  int user_data = i+1;
  argcnt = 0;
  XtSetArg(args[argcnt], XmNindicatorSize, 20); argcnt++;
  XtSetArg(args[argcnt], XmNspacing, 10); argcnt++;
  XtSetArg(args[argcnt], XmNuserData, user_data); argcnt++;
  XtSetArg(args[argcnt], XmNlabelString,
	   (xmstr=XmStringLtoRCreate(name,XmSTRING_DEFAULT_CHARSET))); argcnt++;
  XtSetArg(args[argcnt], XmNrecomputeSize, True); argcnt++;
  wsm_ui_data->space_button[i] = XtCreateWidget("pushButton",
					   xmToggleButtonWidgetClass,
					   wsm_ui->wsm_row_column,
					   args,
					   argcnt);
  XmStringFree( xmstr );

  XtAddCallback(wsm_ui_data->space_button[i], XmNvalueChangedCallback,
		SendLeaveRoomCB, (XtPointer)wsm_ui);
  XtManageChild(wsm_ui_data->space_button[i]);

}


/* ARGSUSED */
void
SendLeaveRoomCB(Widget w, XtPointer client, XtPointer call)
{
  int wsm_index;
  Space *space;
  XmToggleButtonCallbackStruct *toggle_struct = (XmToggleButtonCallbackStruct*)call;

  XtVaGetValues(w,XmNuserData, &wsm_index,NULL);
  if (connected && !mwm_gone)
    {
      if (toggle_struct->set)
	{
	  space = GetSpaceFromID(wsm_index -1);
	  if (space != NULL)
	    (void) SendLeaveRoom(space);
	}
    }
  if (mwm_gone && current_space != NULL)
      XmToggleButtonSetState(wsm_ui->space_button[GetSpaceID(current_space)],True,True);
}

/* ARGSUSED */
void
NewActivateCB(Widget w, XtPointer client, XtPointer call)
{
  static int i = 0;
  char str[20];
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  Space *space;

  if (i == 0)
    {
      for (i = 1, space = space_list; space != NULL; i++, space = space->next);
    }
  sprintf(str,"Room%d",i++);
  space = CreateSpace(XrmStringToQuark(str),str);
  CreateNewSpaceButton(wsm_ui->num_space_buttons,str, wsm_ui);
  CreateFromOptionButton(wsm_ui->num_space_buttons,str);
  CreateToOptionButton(wsm_ui->num_space_buttons,str);
#ifndef _NO_CLIENT_COMMAND
  AddSpaceCommand(space);
#endif
  wsm_ui->num_space_buttons++;
  UpdateSpaceList(wsm_ui->delete_list);
  UpdateSpaceList(wsm_ui->name_list);
  UpdateSpaceList(wsm_ui->background_list);
#ifndef _NO_OCCUPY_DIALOG
  UpdateSpaceList(wsm_ui->occupy_list);
#endif
}

/* ARGSUSED */
void
HideActivateCB(Widget w, XtPointer client, XtPointer call)
{

  HideWsm();

}

/* ARGSUSED */
void
SaveActivateCB(Widget w, XtPointer client, XtPointer call)
{

  SendSaveWsm(file_name);
}


/* ARGSUSED */
void
ExitCB(Widget w, XtPointer client, XtPointer call)
{

  ManageAllWindowsAndExit();

}


/**********************************************************************/
/*                        WSM Configure CBs                           */
/**********************************************************************/


void
CreateFromOptionButton(int i, char *name)
{
  int argcnt;
  Arg args[15];
  XmString xmstr;

  argcnt = 0;
  XtSetArg(args[argcnt], XmNuserData, i+1); argcnt++;
  XtSetArg(args[argcnt], XmNrecomputeSize, True); argcnt++;
  XtSetArg(args[argcnt], XmNlabelString,
	   (xmstr=XmStringLtoRCreate(name, XmSTRING_DEFAULT_CHARSET))); argcnt++;
  wsm_ui->from_option_button[i] = XtCreateWidget("fromWorkspace1Button",
						 xmPushButtonWidgetClass,
						 XtParent(wsm_ui->from_option_button[0]),
						 args,
						 argcnt);
  XmStringFree( xmstr );

  XtAddCallback(wsm_ui->from_option_button[i], XmNactivateCallback,
		FromWorkspaceCB, (XtPointer)wsm_ui);
  XtManageChild(wsm_ui->from_option_button[i]);
}



void
CreateToOptionButton(int i, char *name)
{
  int argcnt;
  Arg args[15];
  XmString xmstr[2];

  xmstr[0] = XmStringCreateLocalized(name);
  XtVaSetValues(wsm_ui->to_option_button[i],
		XmNuserData, i+1,
		XmNlabelString, xmstr[0],
		NULL);

  XmStringFree(xmstr[0]);
  argcnt = 0;
  XtSetArg(args[argcnt], XmNuserData, 0); argcnt++;
  XtSetArg(args[argcnt], XmNlabelString,
	   (xmstr[1]=XmStringLtoRCreate("All Workspaces", XmSTRING_DEFAULT_CHARSET))); argcnt++;
  wsm_ui->to_option_button[i+1] = XtCreateWidget("fromWorkspace1Button",
						 xmPushButtonWidgetClass,
						 XtParent(wsm_ui->to_option_button[0]),
						 args,
						 argcnt);
  XmStringFree( xmstr[1] );

  XtAddCallback(wsm_ui->to_option_button[i+1], XmNactivateCallback,
		ToWorkspaceCB, (XtPointer)wsm_ui);
  XtManageChild(wsm_ui->to_option_button[i+1]);
}

void
UpdateList(Widget list,Space *s)
{

 char *str;
 XmString xmstr;
 WorkWindowList *w_list;

 w_list = s->w_list;


 XmListDeleteAllItems(list);
 while (w_list != NULL)
   {
     if (_WSMGetConfigFormatType(w_list->work_win->window) == WSM_WINDOW_FMT)
       {
	 str = (char*) XtMalloc((strlen(w_list->work_win->name) + 15) * sizeof(char));
	 sprintf(str,"0x%x %s",(unsigned)w_list->work_win->window, w_list->work_win->name);

	 xmstr = XmStringCreateLocalized(str);
	 XmListAddItemUnselected(list,xmstr,0);
	 XmStringFree(xmstr);
	 XtFree(str);
       }
     w_list = w_list->next;
   }

}

void
UpdateBothList(Space *s)
{
  if(connected)
    {
      if (wsm_ui->from_space == s)
	UpdateList(wsm_ui->from_list,s);
      if (wsm_ui->to_space == s)
	UpdateList(wsm_ui->to_list,s);
    }
}



void
UpdateButtons(WorkWindow *w_window)
{
  if (w_window == wsm_ui->w_window)
    {
      if (w_window->linked)
	XmToggleButtonSetState(wsm_ui->link_toggle,True,True);
      else
	XmToggleButtonSetState(wsm_ui->copy_toggle,True,True);
      if (w_window->s_list->next == NULL || w_window->window == 0 ||
	  _WSMGetConfigFormatType(w_window->window) == WSM_ICON_FMT)
	XtVaSetValues(wsm_ui->delete_button,XmNsensitive,False,NULL);
      else
	XtVaSetValues(wsm_ui->delete_button,XmNsensitive,True,NULL);
      if (w_window->window == 0 ||
	  _WSMGetConfigFormatType(w_window->window) == WSM_ICON_FMT ||
	  wsm_ui->to_space == all_space)
	XtVaSetValues(wsm_ui->move_button,XmNsensitive,False,NULL);
      else
	XtVaSetValues(wsm_ui->move_button,XmNsensitive,True,NULL);
    }
}

void
CreateConfigureCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int space_id = -1;
  if (connected )
    {
      if (wsm_ui->from_space == NULL && wsm_ui->to_space == NULL)
	{
	    wsm_ui->from_space = current_space;
	    wsm_ui->to_space = current_space;
	    UpdateBothList(current_space);
	    space_id = GetSpaceID(current_space);
	    if (space_id != -1)
	      {
		XtVaSetValues(wsm_ui->from_option_menu,
			      XmNmenuHistory, wsm_ui->from_option_button[space_id],
			      NULL);
		XtVaSetValues(wsm_ui->to_option_menu,
			      XmNmenuHistory, wsm_ui->to_option_button[space_id],
			      NULL);
	      }
	}
    }
  XtManageChild(wsm_ui->configure_form);
  XtPopup(XtParent(wsm_ui->configure_form), XtGrabNone);

}

/* ARGSUSED */
void
MoveCB(Widget w, XtPointer client, XtPointer call)
{
  int pos_count = 0;
  int *pos_list;
  int i;
			WorkWindow *w_window;
  WSM_UI *wsm_ui_data = (WSM_UI*)client;

  XtVaGetValues(wsm_ui->from_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      for (i = 0; i < pos_count; i++)
	{
	  w_window = GetWorkWindowID(wsm_ui->from_space,pos_list[i]-1);
	  if (w_window != NULL)
	    MoveWindow(w_window, wsm_ui->from_space, wsm_ui->to_space);
	}
    }
  if (pos_count > 0) XtFree((XtPointer)pos_list);
}


/* ARGSUSED */
void
DeleteCB(Widget w, XtPointer client, XtPointer call)
{
  int pos_count = 0;
  int *pos_list;
  WorkWindow *w_window;
  WSM_UI *wsm_ui_data = (WSM_UI*)client;

  XtVaGetValues(wsm_ui->from_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      w_window = GetWorkWindowID(wsm_ui->from_space,pos_list[0]-1);
      if (w_window != NULL)
	DeleteWindow(w_window, wsm_ui->from_space);
      XtFree((XtPointer)pos_list);
    }
}


/* ARGSUSED */
void
ToWorkspaceCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int room_num = 0;
  Space *space;

  XtVaGetValues(w,XmNuserData,&room_num, NULL);
  if (room_num != 0)
      space = GetSpaceFromID(room_num-1);
  else
      space = all_space;
  if (space != NULL)
    {
      wsm_ui->to_space = space;
      UpdateList(wsm_ui->to_list,space);
      if (space == all_space)
	XtVaSetValues(wsm_ui->move_button,XmNsensitive,False,NULL);
      else
	XtVaSetValues(wsm_ui->move_button,XmNsensitive,True,NULL);
    }
  else PRINT("not found %d\n", room_num);
}

/* ARGSUSED */
void
FromWorkspaceCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int room_num;
  Space *space;

  XtVaGetValues(w,XmNuserData,&room_num, NULL);
  space = GetSpaceFromID(room_num-1);
  if (space != NULL)
    {
      wsm_ui->from_space = space;
      UpdateList(wsm_ui->from_list,space);
    }
  else PRINT("Not Found %d\n", room_num);

}

void
Copy(WSM_UI* wsm_ui_data)
{
  int pos_count = 0;
  int *pos_list;
  int i;
  WorkWindow *w_window;
  XtVaGetValues(wsm_ui->from_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
#ifdef DEBUG
/*      PRINT("%d copy %d from %s to %s\n", pos_count, pos_list[0], from_space, to_space);*/
#endif
      for (i = 0; i < pos_count; i++)
	{
	  w_window = GetWorkWindowID(wsm_ui->from_space,pos_list[i] - 1);
	  if (w_window != NULL)
	    CopyWindow(w_window, wsm_ui->from_space,wsm_ui->to_space);
	}
      XtFree((XtPointer)pos_list);
    }
}

void
Link(WSM_UI* wsm_ui_data)
{
  int pos_count = 0;
  int *pos_list;
  WorkWindow *w_window;
  int i;
  XtVaGetValues(wsm_ui->from_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
#ifdef DEBUG
/*      PRINT("%d copy %d from %s to %s\n", pos_count, pos_list[0], from_space, to_space);*/
#endif
      for (i = 0; i < pos_count ; i++)
	{
	  w_window = GetWorkWindowID(wsm_ui->from_space, pos_list[i]-1);
	  if (w_window != NULL)
	    LinkWindow(w_window, wsm_ui->from_space, wsm_ui->to_space);
	}
      XtFree((XtPointer)pos_list);
    }
}

/* ARGSUSED */
void
OccupyCB(Widget w, XtPointer client,XtPointer  call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  Boolean copy_set;
  XtVaGetValues(wsm_ui->copy_toggle, XmNset,&copy_set,NULL);
  if (copy_set)
    Copy(wsm_ui);
  else Link(wsm_ui);
}




void
WindowModeCB(Widget w,XtPointer client,XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  XtVaSetValues(wsm_ui->from_list,XmNselectionPolicy,XmSINGLE_SELECT,NULL);
}



void
ClientModeCB(Widget w,XtPointer client,XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  XtVaSetValues(wsm_ui->from_list,XmNselectionPolicy,XmMULTIPLE_SELECT,NULL);
}


void
SelectFromListCB(Widget w,XtPointer client,XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int pos_count = 0;
  int *pos_list;
  WorkWindow *w_window;

  XtVaGetValues(wsm_ui->from_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      w_window = GetWorkWindowID(wsm_ui->from_space,pos_list[0]-1);
      if (w_window != NULL)
	{
	  wsm_ui->w_window = w_window;
	  UpdateButtons(w_window);
	}
      XtFree((XtPointer)pos_list);
    }
}




void
MultSelectFromListCB(Widget w,XtPointer client,XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  XmListCallbackStruct *list_call = (XmListCallbackStruct *)call;
  int pos_count = 0;
  int *pos_list;
  WorkWindow *w_window;
  int *select_ids;
  int num_select;
  int i;
  int item_pos = list_call->item_position;
  Boolean doit = False;
  PRINT("item pos %d\n",item_pos);

  w_window = GetWorkWindowID(wsm_ui->from_space,item_pos-1);
  if (w_window != NULL)
    {
      wsm_ui->w_window = w_window;
      UpdateButtons(w_window);
      GetWorkWindowClientIDs(item_pos-1, wsm_ui->from_space,&select_ids,&num_select);

      XtVaGetValues(wsm_ui->from_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
      if (pos_count > 0)
	{
	  for (i = 0; i < pos_count; i++)
	    {
	      if (pos_list[i] == item_pos)
		doit = True;
	    }
	  if (doit)
	    for (i = 0; i < pos_count; i++)
	      {
		if (pos_list[i] != item_pos)
		  XmListDeselectPos(wsm_ui->from_list,pos_list[i]);
	      }
	  XtFree((XtPointer)pos_list);
	}
      if (num_select > 0)
	{
	  for (i = 0; i < num_select; i++)
	    {
	      if (select_ids[i] != item_pos -1)
		{
		  XmListSelectPos(w,select_ids[i] +1,False);
		}
	    }
	}
      if (pos_count > 0) XtFree((XtPointer)pos_list);
    }
}



void
SelectToListCB(Widget w, XtPointer client,XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int pos_count = 0;
  int *pos_list;

/*  XmListDeselectAllItems(wsm_ui->to_list);*/

  XtVaGetValues(wsm_ui->to_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      XmListDeselectPos(wsm_ui->to_list,pos_list[0]);
      XtFree((XtPointer)pos_list);
    }
}



/* ARGSUSED */
void
DismissConfigureCB(Widget w,XtPointer  client, XtPointer call)
{

  WSM_UI *wsm_ui_data = (WSM_UI*)client;

   XtPopdown(XtParent(wsm_ui->configure_form));
}

/**********************************************************************/
/*                        WSM Name Workspace CBs                      */
/**********************************************************************/

/* ARGSUSED */
void
CreateNameCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wsm_ui_data = (WSM_UI*)client;

  if (connected )
    {
      UpdateSpaceList(wsm_ui->name_list);
    }

  XtManageChild(wsm_ui->name_form);
  XmListSelectPos(wsm_ui->name_list,1,True);

  XtPopup(XtParent(wsm_ui->name_form), XtGrabNone);

}


void
UpdateSpaceList(Widget list)
{
  XmString xmstr;
  Space *s = space_list;

  XmListDeleteAllItems(list);
  while (s != NULL)
   {
     xmstr = XmStringLtoRCreate(s->name, XmSTRING_DEFAULT_CHARSET);
     XmListAddItem(list,xmstr,0);
     XmStringFree(xmstr);
     s = s->next;
   }

}


void
SelectNameSpaceCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int pos_count = 0;
  int *pos_list;
  Space *space;

  XtVaGetValues(wsm_ui->name_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      space = GetSpaceFromID(pos_list[0]-1);
      if (space != NULL)
	{
	  XmTextSetString(wsm_ui->name_text,space->name);
	  XmTextSetString(wsm_ui->pixmap_text,space->pixmap_name);
	}
      XtFree((XtPointer)pos_list);
    }
}


void
ChangeSpaceName(WSM_UI* wsm_ui_data,Space *space,int wsm_index)
{
  XmString xmstr;

  xmstr = XmStringLtoRCreate(space->name, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(wsm_ui_data->space_button[wsm_index],XmNlabelString,xmstr,NULL);
  XtVaSetValues(wsm_ui->from_option_button[wsm_index],XmNlabelString,xmstr,NULL);
  XtVaSetValues(wsm_ui->to_option_button[wsm_index],XmNlabelString,xmstr,NULL);
  XmStringFree(xmstr);
  UpdateSpaceList(wsm_ui->name_list);
  UpdateSpaceList(wsm_ui->delete_list);
  UpdateSpaceList(wsm_ui->background_list);
#ifndef _NO_OCCUPY_DIALOG
  UpdateSpaceList(wsm_ui->occupy_list);
#endif

}

/* ARGSUSED */
void
NameActivateCB(Widget w, XtPointer client, XtPointer call)
{
  XmProcessTraversal(wsm_ui->pixmap_text,XmTRAVERSE_CURRENT);

}


/* ARGSUSED */
void
NameOkActivateCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  Space *space;
  int pos_count = 0;
  int *pos_list;
  char *old_name;
  char *str;
  XtVaGetValues(wsm_ui->name_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      space = GetSpaceFromID(pos_list[0]-1);
      if (space != NULL)
	{
	  PRINT("change name for space %s\n",space->name);
	  old_name = (char*) XtMalloc(strlen((space->name) +1)*sizeof(char));
	  strcpy(old_name,space->name);
	  str = XmTextGetString(wsm_ui->name_text);
	  if (strlen(str) < MAX_LENGTH)
	    {
	      strcpy(space->name,str);
	      if (strcmp(old_name,space->name) != 0)
		{
		  PRINT("Change name to %s\n",space->name);
		  ChangeSpaceName(wsm_ui,space,pos_list[0]-1);
#ifndef _NO_CLIENT_COMMAND
		  ChangeSpaceCommandName(space);
#endif
		}
	    }
	  XtFree(str);
	  XtFree(old_name);
#ifndef _NO_PIXMAP
	  str = XmTextGetString(wsm_ui->pixmap_text);
	  if (strcmp(str,"None")!=0 && strcmp(str,"none") != 0 &&
	      strcmp(str,space->pixmap_name) != 0)
	    if (SetSpaceLabelPixmap(space,str))
	      {
		XtVaSetValues(wsm_ui_data->space_button[GetSpaceID(space)],
			      XmNlabelPixmap,space->pixmap_label,
			      XmNlabelType, XmPIXMAP,
			      NULL);
	      }
	  XtFree(str);
	  XmListSelectPos(wsm_ui->name_list,pos_list[0],True);
/*	  XtPopdown(XtParent(wsm_ui->name_form));*/
#endif
	}
      XtFree((XtPointer)pos_list);
    }

}




/* ARGSUSED */
void
DismissNameCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wsm_ui_data = (WSM_UI*)client;
   XtPopdown(XtParent(wsm_ui->name_form));
}




/**********************************************************************/
/*                        WSM Background Workspace CBs                      */
/**********************************************************************/

/* ARGSUSED */
void
CreateBackgroundCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wsm_ui_data = (WSM_UI*)client;

  if (connected )
    {
      UpdateSpaceList(wsm_ui->background_list);
    }
   XtManageChild(wsm_ui->background_form);
  XmListSelectPos(wsm_ui->background_list,1,True);
  XtPopup(XtParent(wsm_ui->background_form), XtGrabNone);

}

void
SelectBackgroundSpaceCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int pos_count = 0;
  int *pos_list;
  Space *space;

  XtVaGetValues(wsm_ui->background_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      space = GetSpaceFromID(pos_list[0]-1);
      if (space != NULL)
	{
	  XmTextSetString(wsm_ui->background_text,space->background);
	}
      XtFree((XtPointer)pos_list);
    }
}


/* ARGSUSED */
void
BackgroundActivateCB(Widget w, XtPointer client, XtPointer call)
{
  Space *space;
  int pos_count = 0;
  int *pos_list;
  char *str;
  XtVaGetValues(wsm_ui->background_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      space = GetSpaceFromID(pos_list[0]-1);
      if (space != NULL)
	{
	  str = XmTextGetString(wsm_ui->background_text);
/*	  SendChangeSpaceBackground(space);*/
	  if (str[0] != '"')
	    {
	      if (SetSpacePixel(space,str))
		{
		  XtVaSetValues(wsm_ui->space_button[GetSpaceID(space)],
				XmNbackground,space->pixel,
				XmNunselectColor,space->pixel,
				NULL);
		  if (space == current_space)
		    SetSpaceBackground(space);
		}
	    }
	  else
	    {
	      if (SetSpacePixmap(space,str+1))
		{
		  if (space == current_space)
		    SetSpaceBackground(space);
		}
	    }
	  XtFree(str);
	  XmListSelectPos(wsm_ui->background_list,pos_list[0],True);
/*	  XtPopdown(XtParent(wsm_ui->background_form));*/
	}
      XtFree((XtPointer)pos_list);
    }

}


/* ARGSUSED */
void
DismissBackgroundCB(Widget w, XtPointer client, XtPointer call)
{

  WSM_UI *wsm_ui_data = (WSM_UI*)client;
   XtPopdown(XtParent(wsm_ui->background_form));
}


/**********************************************************************/
/*                        WSM Delete Workspace CBs                    */
/**********************************************************************/
/* ARGSUSED */
void
CreateDeleteCB(Widget w, XtPointer client, XtPointer call)
{
 WSM_UI *wsm_ui_data = (WSM_UI*)client;

  if (connected )
    {
      UpdateSpaceList(wsm_ui->delete_list);
    }

  XtManageChild(wsm_ui->delete_form);
  XtPopup(XtParent(wsm_ui->delete_form), XtGrabNone);
}


/* ARGSUSED */
void
DismissDeleteCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
   XtPopdown(XtParent(wsm_ui->delete_form));

}


/* ARGSUSED */
void
DeleteActivateCB(Widget w, XtPointer client, XtPointer call)
{
  Space *space;
  int pos_count = 0;
  int *pos_list;
  int i;
  WSM_UI *wsm_ui_data = (WSM_UI*)client;

  XtVaGetValues(wsm_ui->delete_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      space = GetSpaceFromID(pos_list[0]-1);
      if (space != NULL)
	{
#ifndef _NO_CLIENT_COMMAND
	  RemoveSpaceCommand(pos_list[0]-1);
#endif
	  RemoveSpace(space);
	  XtDestroyWidget(wsm_ui_data->space_button[pos_list[0]-1]);
	  XtDestroyWidget(wsm_ui->from_option_button[pos_list[0]-1]);
	  XtDestroyWidget(wsm_ui->to_option_button[pos_list[0]-1]);
	  for (i = pos_list[0]-1; i < wsm_ui->num_space_buttons-1; i++)
	    {
	      wsm_ui_data->space_button[i] = wsm_ui_data->space_button[i+1];
	      XtVaSetValues(wsm_ui_data->space_button[i], XmNuserData,i+1,NULL);
	      wsm_ui->from_option_button[i] = wsm_ui->from_option_button[i+1];
	      XtVaSetValues(wsm_ui->from_option_button[i],XmNuserData,i+1,NULL);
	      wsm_ui->to_option_button[i] = wsm_ui->to_option_button[i+1];
	      XtVaSetValues(wsm_ui->to_option_button[i],XmNuserData,i+1,NULL);
	    }
	  wsm_ui->to_option_button[i] = wsm_ui->to_option_button[i+1];
	  wsm_ui->num_space_buttons--;
	  UpdateSpaceList(wsm_ui->delete_list);
	  UpdateSpaceList(wsm_ui->name_list);
	  UpdateSpaceList(wsm_ui->background_list);
#ifndef _NO_OCCUPY_DIALOG
	  UpdateSpaceList(wsm_ui->occupy_list);
#endif
	}
      XtFree((XtPointer)pos_list);
    }
}






void
SelectDeleteCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int pos_count = 0;
  int *pos_list;
  Space *space;
  WorkWindowList *w_list;

  XtVaGetValues(wsm_ui->delete_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      space = GetSpaceFromID(pos_list[0]-1);
      if (space != NULL)
	{
	  w_list = space->w_list;
	  while (w_list != NULL)
	    {
	      if (w_list->work_win->s_list->next == NULL)
		{
		  XtVaSetValues(wsm_ui->ok_button,XmNsensitive,False,NULL);
		  XtFree((XtPointer)pos_list);
		  return;
		}
	      w_list = w_list->next;
	    }
	  XtVaSetValues(wsm_ui->ok_button,XmNsensitive,True,NULL);
	}
      XtFree((XtPointer)pos_list);
    }
}


/**********************************************************************/
/*                        Save As CBs                      */
/**********************************************************************/


void
SaveAsCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;

   XtManageChild(wsm_ui->save_as_form);
  XtPopup(XtParent(wsm_ui->save_as_form),XtGrabNone);
}

/* ARGSUSED */
void
SaveAsOkCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int new_length;
  char *new_file_name;
  char *home_name;
  new_file_name = XmTextGetString(wsm_ui->save_as_text);
  if (strcmp(new_file_name,"")!= 0)
    {
      if (new_file_name[0] != '/')
	{
	  home_name = getenv("HOME");
	  new_length = strlen(home_name) + 1 + strlen(new_file_name) + 2;
	  file_name = XtRealloc(file_name, new_length * sizeof(char));
	  strcpy(file_name,home_name);
	  strcat(file_name, "/");
	  strcat(file_name,new_file_name);
	}
      else
	{
	  new_length = strlen(new_file_name) + 1;
	  file_name = XtRealloc(file_name, new_length * sizeof(char));
	  strcpy(file_name,new_file_name);
	}

      SendSaveWsm(file_name);
      XtPopdown(XtParent(wsm_ui->save_as_form));
      XtFree((XtPointer)new_file_name);
    }
}


/* ARGSUSED */
void
DismissSaveAsCB(Widget w, XtPointer  client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;

   XtPopdown(XtParent(wsm_ui->save_as_form));
}




/**********************************************************************/
/*                        WSM Occupy Workspace CBs                    */
/**********************************************************************/


void
UpdateOccupySpaceList(Widget list)
{
  XmString xmstr;
  Space *s = space_list;

  XmListDeleteAllItems(list);
  while (s != NULL)
   {
     xmstr = XmStringLtoRCreate(s->name, XmSTRING_DEFAULT_CHARSET);
     XmListAddItem(list,xmstr,0);
     XmStringFree(xmstr);
     s = s->next;
   }

}

/* ARGSUSED */

void
CreateOccupy(WorkWindow *w_window)
{
  wsm_ui->occupy_window = w_window;
  if (connected )
    {
      UpdateSpaceList(wsm_ui->occupy_list);
    }

  if (w_window->linked)
    XmToggleButtonSetState(wsm_ui->link_occupy_toggle,True,True);
  else
    XmToggleButtonSetState(wsm_ui->copy_occupy_toggle,True,True);
  XtManageChild(wsm_ui->occupy_form);
  XtPopup(XtParent(wsm_ui->occupy_form), XtGrabNone);
}


/* ARGSUSED */
void
DismissOccupyCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
   XtPopdown(XtParent(wsm_ui->occupy_form));

}


/* ARGSUSED */
void
OccupyActivateCB(Widget w, XtPointer client, XtPointer call)
{
  Space *space;
  int pos_count = 0;
  int *pos_list;
  int i;
  WSM_UI *wsm_ui_data = (WSM_UI*)client;

  XtVaGetValues(wsm_ui->occupy_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      for (i = 0; i < pos_count; i++)
	{
	  space = GetSpaceFromID(pos_list[i] - 1);
	  if (space != NULL)
	    {
		if (copy_mode)
		{
		    PRINT("Copy to space: %s\n", space->name);
		    CopyWindow(wsm_ui->occupy_window,current_space,space);
		}
		else
		{
		    PRINT("Link to space: %s\n", space->name);
		    LinkWindow(wsm_ui->occupy_window,current_space,space);
		}
	    }
	  else PRINT("Space == NULL\n");
	}
      XtFree((XtPointer)pos_list);
    }
  XtPopdown(XtParent(wsm_ui->occupy_form));
}






void
SelectOccupyCB(Widget w, XtPointer client, XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  int pos_count = 0;
  int *pos_list;
  Space *space;
  WorkWindowList *w_list;

  XtVaGetValues(wsm_ui->occupy_list, XmNselectedPositions, &pos_list, XmNselectedPositionCount, &pos_count, NULL);
  if (pos_count > 0)
    {
      space = GetSpaceFromID(pos_list[0]-1);
      if (space != NULL)
	{

	}
      XtFree((XtPointer)pos_list);
    }
}



void
CopyModeCB(Widget w,XtPointer client,XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  copy_mode = XmToggleButtonGetState(w);
}



void
LinkModeCB(Widget w,XtPointer client,XtPointer call)
{
  WSM_UI *wsm_ui_data = (WSM_UI*)client;
  copy_mode = XmToggleButtonGetState(w);
}
