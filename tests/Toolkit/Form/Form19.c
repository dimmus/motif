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
static char rcsid[] = "$XConsortium: Form19.c /main/4 1995/07/13 18:04:12 drk $"
#endif
#endif



#include <testlib.h>

#define MAXCASE 6


Widget Form[MAXCASE];
Widget Child1[MAXCASE];
Widget Child2[MAXCASE];
Widget  Child3[MAXCASE];
Widget  Child4[MAXCASE];
Widget  ChildA[MAXCASE];
Widget  ChildB[MAXCASE];
Widget  ChildC[MAXCASE];
Widget  ChildD[MAXCASE];

static void CreateForms();
static void DestroyForms();

void  main(argc, argv)
int argc;
char **argv;
{

  register int  n, i;
  Arg args[MAX_ARGS];

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg (args[n], XmNwidth,  50);                               n++;
  XtSetArg (args[n], XmNheight, 25);                               n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);
  CreateForms(Shell1);

  CommonPause();


  n = 0;
  XtSetArg(args[n], XmNwidth,  75);                      n++;
  XtSetArg(args[n], XmNheight, 75);                      n++;
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildA[i], args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNx, 75);                           n++;
  XtSetArg(args[n], XmNy, 75);                           n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_SELF);   n++;
  XtSetArg(args[n], XmNtopAttachment,  XmATTACH_SELF);   n++;
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildA[i], args, n);

  CommonPause();

  DestroyForms();

  CommonPause();

  CreateForms(Shell1);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNwidth,  75);                      n++;
  XtSetArg(args[n], XmNheight, 75);                      n++;
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildB[i], args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNx, 75);                           n++;
  XtSetArg(args[n], XmNy, 75);                           n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_SELF);   n++;
  XtSetArg(args[n], XmNtopAttachment,  XmATTACH_SELF);   n++;
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildB[i], args, n);

  CommonPause();


  DestroyForms();

  CommonPause();

  CreateForms(Shell1);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNwidth,  75);                      n++;
  XtSetArg(args[n], XmNheight, 75);                      n++;
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildA[i], args, n);
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildB[i], args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNx, 75);                           n++;
  XtSetArg(args[n], XmNy, 75);                           n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_SELF);   n++;
  XtSetArg(args[n], XmNtopAttachment,  XmATTACH_SELF);   n++;
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildA[i], args, n);


  n = 0;
  XtSetArg(args[n], XmNx, 200);                          n++;
  XtSetArg(args[n], XmNy,  50);                          n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_SELF);   n++;
  XtSetArg(args[n], XmNtopAttachment,  XmATTACH_SELF);   n++;
  for (i = 0; i < MAXCASE; i++) XtSetValues(ChildB[i], args, n);

  CommonPause();

  DestroyForms();

  CommonPause();

  XtAppMainLoop(app_context);
}

static void CreateForms(Shell)
Widget Shell;
{
  register int  n;
  register int  case_number;
  char          label[80];
  Arg           args[MAX_ARGS];

  for (case_number = 0; case_number < MAXCASE; case_number++) {

    sprintf(label, "Case%d", case_number);

    n = 0;
      XtSetArg(args[n], XmNx,  0);                                 n++;
      XtSetArg(args[n],
	       XmNy,
	       50 + (case_number * 100));        n++;

    XtSetArg(args[n], XmNallowOverlap, True);                      n++;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY);              n++;
    XtSetArg(args[n], XmNdialogStyle,  XmDIALOG_MODELESS);         n++;
    XtSetArg(args[n], XmNautoUnmanage, False);                     n++;
    XtSetArg(args[n], XmNdefaultPosition, False);                  n++;
    XtSetArg(args[n], XmNallowShellResize, True);                  n++;
    Form[case_number] = XmCreateFormDialog(Shell, label, args, n);

    Child1[case_number] = XmCreatePushButton(Form[case_number], "", NULL, 0);
    Child2[case_number] = XmCreatePushButton(Form[case_number], "", NULL, 0);
    Child3[case_number] = XmCreatePushButton(Form[case_number], "", NULL, 0);
    Child4[case_number] = XmCreatePushButton(Form[case_number], "", NULL, 0);


    n = 0;
    XtSetArg(args[n], XmNresizable, True);                        n++;
    XtSetValues(Child1[case_number], args, n);
    XtSetValues(Child2[case_number], args, n);
    XtSetValues(Child3[case_number], args, n);
    XtSetValues(Child4[case_number], args, n);

    switch ( case_number ) {

    case 0:

      sprintf(label, "Child: A 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
      XtSetValues(Child1[case_number], args, n);

      ChildA[case_number] = Child1[case_number];

      sprintf(label, "Child: B 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child1[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildB[case_number] = Child2[case_number];

      sprintf(label, "Child: C 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildC[case_number] = Child3[case_number];

      sprintf(label, "Child: D 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);        n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child4[case_number], args, n);

      ChildD[case_number] = Child4[case_number];

      break;

    case 1:

      sprintf(label, "Child: A 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetValues(Child1[case_number], args, n);

      ChildA[case_number] = Child1[case_number];

      sprintf(label, "Child: B 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child1[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildB[case_number] = Child2[case_number];

      sprintf(label, "Child: C 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildC[case_number] = Child3[case_number];

      sprintf(label, "Child: D 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);        n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child4[case_number], args, n);

      ChildD[case_number] = Child4[case_number];

      break;

    case 2:

      sprintf(label, "Child: A 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
      XtSetValues(Child1[case_number], args, n);

      ChildA[case_number] = Child1[case_number];

      sprintf(label, "Child: B 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child1[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildB[case_number] = Child2[case_number];

      sprintf(label, "Child: C 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildC[case_number] = Child3[case_number];

      sprintf(label, "Child: D 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child4[case_number], args, n);

      ChildD[case_number] = Child4[case_number];

      break;

    case 3:

      sprintf(label, "Child: A 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetValues(Child1[case_number], args, n);

      ChildA[case_number] = Child1[case_number];

      sprintf(label, "Child: B 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child1[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildB[case_number] = Child2[case_number];

      sprintf(label, "Child: C 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildC[case_number] = Child3[case_number];

      sprintf(label, "Child: D 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child4[case_number], args, n);

      ChildD[case_number] = Child4[case_number];

      break;

    case 4:

      sprintf(label, "Child: D 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);        n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child1[case_number], args, n);

      ChildD[case_number] = Child1[case_number];

      sprintf(label, "Child: C 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildC[case_number] = Child2[case_number];

      sprintf(label, "Child: B 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child4[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildB[case_number] = Child3[case_number];

      sprintf(label, "Child: A 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
      XtSetValues(Child4[case_number], args, n);

      ChildA[case_number] = Child4[case_number];

      break;

    case 5:

      sprintf(label, "Child: D 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);        n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child1[case_number], args, n);

      ChildD[case_number] = Child1[case_number];

      sprintf(label, "Child: C 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildC[case_number] = Child2[case_number];

      sprintf(label, "Child: B 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child4[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildB[case_number] = Child3[case_number];

      sprintf(label, "Child: A 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetValues(Child4[case_number], args, n);

      ChildA[case_number] = Child4[case_number];

      break;

    case 6:

      sprintf(label, "Child: D 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child1[case_number], args, n);

      ChildD[case_number] = Child1[case_number];

      sprintf(label, "Child: C 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildC[case_number] = Child2[case_number];

      sprintf(label, "Child: B 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child4[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildB[case_number] = Child3[case_number];

      sprintf(label, "Child: A 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
      XtSetValues(Child4[case_number], args, n);

      ChildA[case_number] = Child4[case_number];

      break;

    case 7:

      sprintf(label, "Child: D 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child2[case_number]);       n++;
      XtSetValues(Child1[case_number], args, n);

      ChildD[case_number] = Child1[case_number];

      sprintf(label, "Child: C 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child3[case_number]);       n++;
      XtSetValues(Child2[case_number], args, n);

      ChildC[case_number] = Child2[case_number];

      sprintf(label, "Child: B 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNleftWidget, Child4[case_number]);       n++;
      XtSetValues(Child3[case_number], args, n);

      ChildB[case_number] = Child3[case_number];

      sprintf(label, "Child: A 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));   n++;
      XtSetValues(Child4[case_number], args, n);

      ChildA[case_number] = Child4[case_number];

      break;

    case 8:

      sprintf(label, "Child: A 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);          n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child2[case_number]);       n++;
      XtSetValues(Child1[case_number], args, n);

      ChildA[case_number] = Child1[case_number];

      sprintf(label, "Child: B 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child3[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child2[case_number], args, n);

      ChildB[case_number] = Child2[case_number];

      sprintf(label, "Child: C 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child4[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child3[case_number], args, n);

      ChildC[case_number] = Child3[case_number];

      sprintf(label, "Child: D 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);         n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child4[case_number], args, n);

      ChildD[case_number] = Child4[case_number];

      break;

    case 9:

      sprintf(label, "Child: A 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child2[case_number]);       n++;
      XtSetValues(Child1[case_number], args, n);

      ChildA[case_number] = Child1[case_number];

      sprintf(label, "Child: B 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child3[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child2[case_number], args, n);

      ChildB[case_number] = Child2[case_number];

      sprintf(label, "Child: C 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child4[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child3[case_number], args, n);

      ChildC[case_number] = Child3[case_number];

      sprintf(label, "Child: D 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);         n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child4[case_number], args, n);

      ChildD[case_number] = Child4[case_number];

      break;

    case 10:

      sprintf(label, "Child: D 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);         n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child1[case_number], args, n);

      ChildD[case_number] = Child1[case_number];

      sprintf(label, "Child: C 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child1[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child2[case_number], args, n);

      ChildC[case_number] = Child2[case_number];

      sprintf(label, "Child: B 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child2[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child3[case_number], args, n);

      ChildB[case_number] = Child3[case_number];

      sprintf(label, "Child: A 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);          n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child3[case_number]);       n++;
      XtSetValues(Child4[case_number], args, n);

      ChildA[case_number] = Child4[case_number];

      break;

    case 11:

      sprintf(label, "Child: D 1");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);         n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child1[case_number], args, n);

      ChildD[case_number] = Child1[case_number];

      sprintf(label, "Child: C 2");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child1[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child2[case_number], args, n);

      ChildC[case_number] = Child2[case_number];

      sprintf(label, "Child: B 3");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child2[case_number]);       n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);          n++;
      XtSetValues(Child3[case_number], args, n);

      ChildB[case_number] = Child3[case_number];

      sprintf(label, "Child: A 4");

      n = 0;
      XtSetArg(args[n],
               XmNlabelString,
               XmStringCreate(label, XmSTRING_DEFAULT_CHARSET));    n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNrightWidget, Child3[case_number]);       n++;
      XtSetValues(Child4[case_number], args, n);

      ChildA[case_number] = Child4[case_number];

      break;

    }


    XtManageChild(Child1[case_number]);
    XtManageChild(Child2[case_number]);
    XtManageChild(Child3[case_number]);
    XtManageChild(Child4[case_number]);


    XtManageChild(Form[case_number]);

  }
}

static void DestroyForms()
{
  register int  case_number;


  for (case_number = 0; case_number < MAXCASE; case_number++) {

    XtDestroyWidget(XtParent(Form[case_number]));
  }
}
