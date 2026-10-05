/*
 * Copyright 1994, Integrated Computer Solutions, Inc.
 *
 * All Rights Reserved.
 *
 * Author: Rick Umali
 *
 * extlist.h
 *
 */

#ifndef _extlist_h
#define _extlist_h
typedef enum { PPORSCHE, PCLOWN, PSTOP, PCRAB } PlayerPix;

#define NUM_PLAYERS 14

/**************************************************************
 *		DATA STRUCTURES
 **************************************************************/
typedef struct _DemoInfo {
  Widget extlist;
  Widget rem_pb;
  Widget player[14];
} DemoStruct, *DemoInfo;

typedef struct _PlayerData {
  PlayerPix picture;
    char *name;
    int at_bats;
    int runs;
    int hits;
    int rbi;
    int average; /* entered as an integer...displayed as a "float" */
    int home_runs;
} PlayerData;

#define NUM_COLUMNS 8
#define MAX_ARGS 15
#define LINEUP_LIMIT 9

/**************************************************************
 *		FUNCTION PROTOTYPES
 **************************************************************/
/* creation.c */
extern Widget Createform(Widget parent, DemoInfo demo_info);

/* callbacks-c.c */
extern Widget CreateExtListCB(Widget parent);
extern void RemCB(Widget w, XtPointer client, XtPointer call);
extern void QuitCB(Widget w, XtPointer client, XtPointer call);
extern void UnselCB(Widget w, XtPointer client, XtPointer call);
extern void ToggleFindArea(Widget w, XtPointer client, XtPointer call);
extern void FirstRowCol(Widget w, XtPointer client, XtPointer call);
extern void ChoosePlayerCB(Widget w, XtPointer client, XtPointer call);
extern void UpdateRemLabelStr(Widget w, XtPointer client, XtPointer call);
extern void CreateLabel(Widget w, XtPointer client, XtPointer call);

/* util-c.c */
extern XtPointer CONVERT(Widget w, char *from_string, char *to_type,
			 int to_size, Boolean *success);
extern void MENU_POST(Widget p, XtPointer mw, XEvent *ev, Boolean *dispatch);
extern Pixmap XPM_PIXMAP(Widget w, char **pixmapName);
#endif
