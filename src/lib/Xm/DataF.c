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
/*
 * XmDataField: an XmTextField with an input mask (XmNpicture,
 * XmNautoFill, XmNpictureErrorCallback) and a check of the value before
 * Tab moves the focus on (XmNvalidateCallback, the ValidateAndMove
 * action).  Everything else, XmNalignment included, is XmTextField's,
 * and the XmDataField* functions call the XmTextField* ones.
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "TextFI.h"
#include "TextFSelI.h"
#include "XmI.h"
#include <Xm/AccTextT.h>
#include <Xm/DataFP.h>
#include <Xm/DataFSelP.h>
#include <Xm/TraitP.h>
#include <Xm/TransferT.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

/********    Static Function Declarations    ********/
static void ClassPartInitialize(WidgetClass w_class);
static void Initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static void Destroy(Widget w);
static Boolean SetValues(
    Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args);
static void ValidateAndMove(Widget w, XEvent *ev, String *args, Cardinal *nargs);
static void PictureVerifyCallback(Widget w, XtPointer client_d, XtPointer call_d);
/********    End Static Function Declarations    ********/

static XtActionsRec actions[] = {
    {"ValidateAndMove", ValidateAndMove},
};

static XtResource resources[] = {
    {XmNpicture,
     XmCPicture,
     XmRString,
     sizeof(String),
     XtOffsetOf(XmDataFieldRec, data.picture_source),
     XmRImmediate,
     (XtPointer)NULL},
    {XmNautoFill,
     XmCAutoFill,
     XmRBoolean,
     sizeof(Boolean),
     XtOffsetOf(XmDataFieldRec, data.auto_fill),
     XmRImmediate,
     (XtPointer)True},
    {XmNpictureErrorCallback,
     XmCCallback,
     XmRCallback,
     sizeof(XtCallbackList),
     XtOffsetOf(XmDataFieldRec, data.picture_error_cb),
     XmRCallback,
     (XtPointer)NULL},
    {XmNvalidateCallback,
     XmCCallback,
     XmRCallback,
     sizeof(XtCallbackList),
     XtOffsetOf(XmDataFieldRec, data.validate_cb),
     XmRCallback,
     (XtPointer)NULL},
};

static XmPrimitiveClassExtRec primClassExtRec = {
    NULL,
    NULLQUARK,
    XmPrimitiveClassExtVersion,
    sizeof(XmPrimitiveClassExtRec),
    XmInheritBaselineProc,    /* widget_baseline        */
    XmInheritDisplayRectProc, /* widget_display_rect    */
    XmInheritMarginsProc,     /* get/set widget margins */
};

externaldef(xmdatafieldclassrec) XmDataFieldClassRec xmDataFieldClassRec = {
    {
        (WidgetClass)&xmTextFieldClassRec, /* superclass         */
        "XmDataField",                     /* class_name         */
        sizeof(XmDataFieldRec),            /* widget_size        */
        NULL,                              /* class_initialize   */
        ClassPartInitialize,               /* class_part_initiali*/
        FALSE,                             /* class_inited       */
        Initialize,                        /* initialize         */
        (XtArgsProc)NULL,                  /* initialize_hook    */
        XtInheritRealize,                  /* realize            */
        actions,                           /* actions            */
        XtNumber(actions),                 /* num_actions        */
        resources,                         /* resources          */
        XtNumber(resources),               /* num_resources      */
        NULLQUARK,                         /* xrm_class          */
        TRUE,                              /* compress_motion    */
        XtExposeCompressMaximal |          /* compress_exposure  */
            XtExposeNoRegion,
        TRUE,                     /* compress_enterleave*/
        FALSE,                    /* visible_interest   */
        Destroy,                  /* destroy            */
        XtInheritResize,          /* resize             */
        XtInheritExpose,          /* expose             */
        SetValues,                /* set_values         */
        (XtArgsFunc)NULL,         /* set_values_hook    */
        XtInheritSetValuesAlmost, /* set_values_almost  */
        (XtArgsProc)NULL,         /* get_values_hook    */
        (XtAcceptFocusProc)NULL,  /* accept_focus       */
        XtVersion,                /* version            */
        NULL,                     /* callback_private   */
        NULL,                     /* tm_table           */
        XtInheritQueryGeometry,   /* query_geometry     */
        (XtStringProc)NULL,       /* display accel      */
        NULL,                     /* extension          */
    },
    {
        /* Xmprimitive        */
        XmInheritBorderHighlight,     /* border_highlight   */
        XmInheritBorderUnhighlight,   /* border_unhighlight */
        NULL,                         /* translations       */
        (XtActionProc)NULL,           /* arm_and_activate   */
        NULL,                         /* syn resources      */
        0,                            /* num syn_resources  */
        (XtPointer)&primClassExtRec,  /* extension          */
    },
    {
        /* data class: in the place of XmTextField's text class part */
        NULL, /* extension          */
    }};
externaldef(xmdatafieldwidgetclass)
    WidgetClass xmDataFieldWidgetClass = (WidgetClass)&xmDataFieldClassRec;

static void ClassPartInitialize(WidgetClass w_class)
{
  char *event_bindings;
  size_t size;
  _XmFastSubclassInit(w_class, XmDATAFIELD_BIT);
  /* XmTextField sets its traits on its own class only. */
  XmeTraitSet((XtPointer)w_class,
              XmQTtransfer,
              XmeTraitGet((XtPointer)xmTextFieldWidgetClass, XmQTtransfer));
  XmeTraitSet((XtPointer)w_class,
              XmQTaccessTextual,
              XmeTraitGet((XtPointer)xmTextFieldWidgetClass, XmQTaccessTextual));
  /* XmTextField's translations, with Tab bound to ValidateAndMove. */
  size = strlen(_XmDataF_EventBindings4) + strlen("\n") + strlen(_XmTextF_EventBindings1) +
         strlen(_XmTextF_EventBindings2) + strlen(_XmTextF_EventBindings3) + 1;
  event_bindings = XtMalloc(size);
  snprintf(event_bindings,
           size,
           "%s\n%s%s%s",
           _XmDataF_EventBindings4,
           _XmTextF_EventBindings1,
           _XmTextF_EventBindings2,
           _XmTextF_EventBindings3);
  _XmProcessLock();
  w_class->core_class.tm_table = (String)XtParseTranslationTable(event_bindings);
  _XmProcessUnlock();
  XtFree(event_bindings);
}

static void Initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
  XmDataFieldWidget df = (XmDataFieldWidget)new_w;
  XmDataField_picture(df) = NULL;
  if (XmDataField_picture_source(df)) {
    XmDataField_picture_source(df) = XtNewString(XmDataField_picture_source(df));
    XmDataField_picture(df) = XmParsePicture(XmDataField_picture_source(df));
    if (XmDataField_picture(df))
      XtAddCallback(new_w, XmNmodifyVerifyCallback, PictureVerifyCallback, NULL);
  }
}

static void Destroy(Widget w)
{
  XtFree((char *)XmDataField_picture_source(w));
  if (XmDataField_picture(w))
    XmPictureDelete(XmDataField_picture(w));
}

static Boolean SetValues(
    Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
  if (XmDataField_picture_source(old) != XmDataField_picture_source(new_w)) {
    XtFree((char *)XmDataField_picture_source(old));
    XmDataField_picture_source(new_w) = XtNewString(XmDataField_picture_source(new_w));
    if (XmDataField_picture(new_w)) {
      XmPictureDelete(XmDataField_picture(new_w));
      XmDataField_picture(new_w) = NULL;
    }
    if (XmDataField_picture_source(new_w))
      XmDataField_picture(new_w) = XmParsePicture(XmDataField_picture_source(new_w));
    /* Remove first, so that the callback is never registered twice. */
    XtRemoveCallback(new_w, XmNmodifyVerifyCallback, PictureVerifyCallback, NULL);
    if (XmDataField_picture(new_w))
      XtAddCallback(new_w, XmNmodifyVerifyCallback, PictureVerifyCallback, NULL);
  }
  return False;
}

static void ValidateAndMove(Widget w, XEvent *ev, String *args, Cardinal *nargs)
{
  XmDataFieldCallbackStruct cbs;
  /*
   * We are guaranteed that the picture will have accepted the string, so
   * just call the verify callbacks.
   */
  cbs.w = w;
  cbs.text = XmTextFieldGetString(w);
  cbs.accept = True;
  XtCallCallbackList(w, XmDataField_validate_cb(w), (XtPointer)&cbs);
  XtFree(cbs.text);
  if (cbs.accept == False) {
    XBell(XtDisplay(w), 0);
    return;
  }
  /* Otherwise give up the focus and process the traversal as normal. */
  if (*nargs > 0 && strncasecmp(args[0], "prev", 4) == 0)
    (void)XmProcessTraversal(w, XmTRAVERSE_PREV_TAB_GROUP);
  else
    (void)XmProcessTraversal(w, XmTRAVERSE_NEXT_TAB_GROUP);
}

static void PictureVerifyCallback(Widget w, XtPointer client_d, XtPointer call_d)
{
  XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_d;
  char *curr, *newptr, *changed = NULL;
  int src, dst, i;
  XmPictureState ps;
  Boolean done = False;
  /*
   * If we're just backspacing, allow the change irregarless
   */
  if (cbs->startPos < cbs->currInsert || cbs->text->length == 0)
    return;
  /*
   * Get the current string, and splice in the intended changes
   */
  curr = XmTextFieldGetString(w);
  newptr = _XmMallocArray(cbs->text->length + strlen(curr) + 2, sizeof(char));
  dst = 0;
  /* Copy in the stuff before the modification */
  for (src = 0; src < cbs->startPos; src++, dst++)
    newptr[dst] = curr[src];
  /* Then the modification text */
  if (cbs->text->ptr) {
    for (src = 0; src < cbs->text->length; src++, dst++)
      newptr[dst] = cbs->text->ptr[src];
  }
  /* Then the last bit */
  if (cbs->endPos > cbs->startPos) {
    for (dst = cbs->endPos + cbs->text->length; src < cbs->endPos; src++, dst++)
      newptr[dst] = curr[src];
  }
  /* And stick a null in for good measure and sanity in debugging */
  newptr[dst] = '\0';
  XtFree(curr);
  /*
   * Run it through the picture, and bail if it isn't accepted
   */
  ps = XmGetNewPictureState(XmDataField_picture(w));
  for (i = 0; (size_t)i < strlen(newptr); i++) {
    changed = XmPictureProcessCharacter(ps, newptr[i], &done);
    if (changed == NULL || done)
      break;
  }
  XtFree(newptr);
  if (changed == NULL) {
    cbs->doit = False;
    XmPictureDeleteState(ps);
    XtCallCallbackList(w, XmDataField_picture_error_cb(w), NULL);
    return;
  }
  /*
   * And now try autofilling
   */
  if (XmDataField_auto_fill(w))
    changed = XmPictureDoAutoFill(ps);
  else
    changed = XmPictureGetCurrentString(ps);
  /*
   * Now the hard part:  we may have been auto-filled, so we have to
   * massage the callback struct to reflect what's happened.  XmTextField
   * frees the text that the callbacks leave in cbs->text.
   */
  cbs->startPos = 0;
  /* CR03686 cbs->endPos = strlen(newptr); */
  cbs->text->ptr = XtNewString(changed);
  cbs->text->length = strlen(changed);
  XmPictureDeleteState(ps);
}

/*
 * The API.  An XmDataField is an XmTextField, so these are the
 * XmTextField functions.
 */
Widget XmCreateDataField(Widget parent, char *name, ArgList arglist, Cardinal argcount)
{
  return XtCreateWidget(name, xmDataFieldWidgetClass, parent, arglist, argcount);
}

Widget XmVaCreateDataField(Widget parent, char *name, ...)
{
  Widget w;
  va_list var;
  int count;
  Va_start(var, name);
  count = XmeCountVaListSimple(var);
  va_end(var);
  Va_start(var, name);
  w = XmeVLCreateWidget(name, xmDataFieldWidgetClass, parent, False, var, count);
  va_end(var);
  return w;
}

Widget XmVaCreateManagedDataField(Widget parent, char *name, ...)
{
  Widget w;
  va_list var;
  int count;
  Va_start(var, name);
  count = XmeCountVaListSimple(var);
  va_end(var);
  Va_start(var, name);
  w = XmeVLCreateWidget(name, xmDataFieldWidgetClass, parent, True, var, count);
  va_end(var);
  return w;
}

void XmDataFieldSetString(Widget w, char *value)
{
  XmTextFieldSetString(w, value);
}

char *XmDataFieldGetString(Widget w)
{
  return XmTextFieldGetString(w);
}

wchar_t *XmDataFieldGetStringWcs(Widget w)
{
  return XmTextFieldGetStringWcs(w);
}

void XmDataFieldSetHighlight(Widget w,
                             XmTextPosition left,
                             XmTextPosition right,
                             XmHighlightMode mode)
{
  XmTextFieldSetHighlight(w, left, right, mode);
}

void XmDataFieldSetAddMode(Widget w, Boolean state)
{
  XmTextFieldSetAddMode(w, state);
}

char *XmDataFieldGetSelection(Widget w)
{
  return XmTextFieldGetSelection(w);
}

void XmDataFieldSetSelection(Widget w, XmTextPosition first, XmTextPosition last, Time sel_time)
{
  XmTextFieldSetSelection(w, first, last, sel_time);
}

Boolean XmDataFieldGetSelectionPosition(Widget w, XmTextPosition *left, XmTextPosition *right)
{
  return XmTextFieldGetSelectionPosition(w, left, right);
}

XmTextPosition XmDataFieldXYToPos(Widget w, Position x, Position y)
{
  return XmTextFieldXYToPos(w, x, y);
}

void XmDataFieldShowPosition(Widget w, XmTextPosition position)
{
  XmTextFieldShowPosition(w, position);
}

Boolean XmDataFieldCut(Widget w, Time clip_time)
{
  return XmTextFieldCut(w, clip_time);
}

Boolean XmDataFieldCopy(Widget w, Time clip_time)
{
  return XmTextFieldCopy(w, clip_time);
}

Boolean XmDataFieldPaste(Widget w)
{
  return XmTextFieldPaste(w);
}

void XmDataFieldSetEditable(Widget w, Boolean editable)
{
  XmTextFieldSetEditable(w, editable);
}

void XmDataFieldSetInsertionPosition(Widget w, XmTextPosition position)
{
  XmTextFieldSetInsertionPosition(w, position);
}

Boolean XmDataFieldGetAddMode(Widget w)
{
  return XmTextFieldGetAddMode(w);
}

Boolean XmDataFieldGetEditable(Widget w)
{
  return XmTextFieldGetEditable(w);
}

int XmDataFieldGetMaxLength(Widget w)
{
  return XmTextFieldGetMaxLength(w);
}

void XmDataFieldSetMaxLength(Widget w, int max_length)
{
  XmTextFieldSetMaxLength(w, max_length);
}

XmTextPosition XmDataFieldGetCursorPosition(Widget w)
{
  return XmTextFieldGetCursorPosition(w);
}

XmTextPosition XmDataFieldGetInsertionPosition(Widget w)
{
  return XmTextFieldGetInsertionPosition(w);
}

XmTextPosition XmDataFieldGetLastPosition(Widget w)
{
  return XmTextFieldGetLastPosition(w);
}

int XmDataFieldGetSubstring(
    Widget w, XmTextPosition start, int num_chars, int buf_size, char *buffer)
{
  return XmTextFieldGetSubstring(w, start, num_chars, buf_size, buffer);
}

int XmDataFieldGetSubstringWcs(
    Widget w, XmTextPosition start, int num_chars, int buf_size, wchar_t *buffer)
{
  return XmTextFieldGetSubstringWcs(w, start, num_chars, buf_size, buffer);
}

wchar_t *XmDataFieldGetSelectionWcs(Widget w)
{
  return XmTextFieldGetSelectionWcs(w);
}

void XmDataFieldReplace(Widget w, XmTextPosition from_pos, XmTextPosition to_pos, char *value)
{
  XmTextFieldReplace(w, from_pos, to_pos, value);
}

void XmDataFieldReplaceWcs(Widget w,
                           XmTextPosition from_pos,
                           XmTextPosition to_pos,
                           wchar_t *wc_value)
{
  XmTextFieldReplaceWcs(w, from_pos, to_pos, wc_value);
}

void XmDataFieldInsert(Widget w, XmTextPosition position, char *value)
{
  XmTextFieldInsert(w, position, value);
}

void XmDataFieldInsertWcs(Widget w, XmTextPosition position, wchar_t *wcstring)
{
  XmTextFieldInsertWcs(w, position, wcstring);
}

Boolean XmDataFieldRemove(Widget w)
{
  return XmTextFieldRemove(w);
}

Boolean XmDataFieldPosToXY(Widget w, XmTextPosition position, Position *x, Position *y)
{
  return XmTextFieldPosToXY(w, position, x, y);
}

int XmDataFieldGetBaseline(Widget w)
{
  return XmTextFieldGetBaseline(w);
}

/*
 * Internal functions that <Xm/DataF.h> and <Xm/DataFSelP.h> declare and
 * libXm has always exported.
 */
Boolean _XmDataFieldReplaceText(XmDataFieldWidget df,
                                XEvent *event,
                                XmTextPosition replace_prev,
                                XmTextPosition replace_next,
                                char *insert,
                                int insert_length,
                                Boolean move_cursor)
{
  return _XmTextFieldReplaceText(
      (XmTextFieldWidget)df, event, replace_prev, replace_next, insert, insert_length, move_cursor);
}

void _XmDataFieldSetClipRect(XmDataFieldWidget df)
{
  _XmTextFieldSetClipRect((XmTextFieldWidget)df);
}

void _XmDataFieldDrawInsertionPoint(XmDataFieldWidget df, Boolean turn_on)
{
  _XmTextFieldDrawInsertionPoint((XmTextFieldWidget)df, turn_on);
}

void _XmDataFieldSetSel2(
    Widget w, XmTextPosition left, XmTextPosition right, Boolean disown, Time sel_time)
{
  _XmTextFieldSetSel2(w, left, right, disown, sel_time);
}

Boolean _XmDataFieldConvert(Widget w,
                            Atom *selection,
                            Atom *target,
                            Atom *type,
                            XtPointer *value,
                            unsigned long *length,
                            int *format)
{
  return _XmTextFieldConvert(w, selection, target, type, value, length, format, NULL, NULL);
}

void _XmDataFieldLoseSelection(Widget w, Atom *selection)
{
  _XmTextFieldLoseSelection(w, selection);
}

Widget _XmDataFieldGetDropReciever(Widget w)
{
  return _XmTextFieldGetDropReciever(w);
}
