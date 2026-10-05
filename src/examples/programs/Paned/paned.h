/*
 * paned.h - functions shared between paned.c and creation-c.c.
 */
#ifndef PANED_H
#define PANED_H

#include <Xm/Xm.h>

/* creation-c.c */
extern Widget Createpaned(Widget parent);
extern Widget Createform(Widget parent);

/* paned.c (callbacks) */
extern void CreateLabel(Widget w, XtPointer client, XtPointer call);
extern void OtherResCB(Widget w, XtPointer client, XtPointer call);
extern void ConstraintResCB(Widget w, XtPointer client, XtPointer call);
extern void OrientChValCB(Widget w, XtPointer client, XtPointer call);
extern void SepValChCB(Widget w, XtPointer client, XtPointer call);
extern void QuitCB(Widget w, XtPointer client, XtPointer call);
extern void SashValChCB(Widget w, XtPointer client, XtPointer call);

#endif /* PANED_H */
