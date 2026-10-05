/*
 * Copyright 1994, Integrated Computer Solutions, Inc.
 *
 * All Rights Reserved.
 *
 * Author: Rick Umali
 *
 * fontsel.h
 *
 */
/**************************************************************
 *		DEFINES
 **************************************************************/
#define EXPLAIN_CURFONT 1
#define EXPLAIN_SHOWFONT 2

#include <Xm/Xm.h>

/**************************************************************
 *		SHARED FUNCTIONS
 **************************************************************/
extern Widget CreateDemoForm(Widget parent);
extern void CreateHypeLabel(Widget w, XtPointer client, XtPointer call);
extern void ShowFontValChCB(Widget w, XtPointer client, XtPointer call);
extern void ExplainCB(Widget w, XtPointer client, XtPointer call);
extern void ShowCurFont(Widget w, XtPointer client, XtPointer call);
extern void ShowOtherCB(Widget w, XtPointer client, XtPointer call);
extern void QuitCB(Widget w, XtPointer client, XtPointer call);
