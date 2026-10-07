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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#ifdef REV_INFO
#  ifndef lint
static char rcsid[] = "$TOG: XmRenderT.c /main/14 1998/10/26 20:14:42 samborn $"
#  endif
#endif
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlocale.h>
#include "HashI.h"
#include "MessagesI.h"
#include "XmI.h"
#include "XmRenderTI.h"
#include "XmStringI.h"
#include "XmTabListI.h"
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <X11/Xlibint.h> /* for XESetCloseDisplay */
#include <X11/Xresource.h>
#include <Xm/Display.h>  /* For XmGetXmDisplay */
#include <Xm/DisplayP.h> /* For direct access to callback fields */
#if USE_XFT
#  include <X11/Xft/Xft.h>
#endif
/* Warning Messages */
#define NO_NULL_TAG_MSG _XmMMsgXmRenderT_0000
#define NULL_DISPLAY_MSG _XmMMsgXmRenderT_0001
#define INVALID_TYPE_MSG _XmMMsgXmRenderT_0002
#define CONVERSION_FAILED_MSG _XmMMsgXmRenderT_0003
#define NULL_FONT_TYPE_MSG _XmMMsgXmRenderT_0004
#define NULL_LOAD_IMMEDIATE_MSG _XmMMsgXmRenderT_0005
/* local macros */
#define GetHandle(type) (type *)XtMalloc(sizeof(type))
#define FreeHandle(handle) XtFree((char *)handle)
#define SetPtr(handle, ptr) *(handle) = ptr
#define GetPtr(handle) *(handle)
#define NameIsString(fontname) (((fontname) != NULL) && ((fontname) != (String)XmAS_IS))
#define ListIsList(tablist) \
  (((tablist) != NULL) && ((unsigned int)(unsigned long)(tablist) != XmAS_IS))
/**********************************************************************
 *	      IMPORTANT NOTE: IMPLEMENTATION OF SHARING
 *
 *	Instances of XmRenderTable and XmRendition are shared via a
 *	reference counting mechanism. This comment provides a general
 *	overview of how this is done.
 *
 *	First, both rendertable and renditions are indirectly
 *	referenced via a handle mechanism.  See the GetHandle,
 *	FreeHandle, SetPtr and GetPtr macros above.  This allows
 *	either the handle to change without the underlying data
 * 	structure changing, or the underlying data structure to change
 *	without the handle changing. I will indicate below where this
 *	happens.
 *
 *	Second, the real data structure for rendertable and rendition
 *	contain a reference count.  This count is incremented on copy
 *	and decremented on free.  If a decrement produces a zero
 *	refcount, the actual memory is freed.  If an increment
 *	produces a zero refcount, then the refcount has overflowed.  The
 *	refcount is decremented, and new memory is allocated for a new
 *	copy.
 *
 *	Finally, I have defined a terminology for the different types
 *	of "copying" that can done based on allocating a new handle or
 *	not and allocating a new data structure or not.  This probably
 *	conflicts with some other existing terminology, probably in
 *	object oriented programming.  Sorry about that.
 *
 *	Function:	Clone	Copy	Renew	Duplicate
 *			(Mutate)	(Update)
 *
 *	handle		new	new	old	old
 *	structure	new	inc	new	inc
 *			(changed)	(changed)
 *
 *	(changed) indicates that the data in the new structure has
 *	been changed from the data in the old structure.
 *
 *	I will use these terms as a short hand in describing the
 *	functions below.
 **********************************************************************/
/********    Static Function Declarations    ********/
static void CopyInto(XmRendition toRend, XmRendition fromRend);
static void MergeInto(XmRendition toRend, XmRendition fromRend);
static XmRendition CloneRendition(XmRendition rend);
static XmRendition CopyRendition(XmRendition rend);
static XmRendition RenewRendition(XmRendition rend);
static XmRendition DuplicateRendition(XmRendition rend);
static Boolean FreeRendition(XmRendition rend);
static void RenditionWarning(char *tag, char *type, char *message, Display *dpy);
static void CleanupResources(XmRendition rend, Boolean copy);
static void ValidateTag(XmRendition rend, XmStringTag dflt);
static void ValidateAndLoadFont(XmRendition rend, Display *display);
static void SetRend(XmRendition to, XmRendition from);
static Boolean RendComplete(XmRendition rend);
static void CopyFromArg(XtArgVal src, char *dst, unsigned int size);
static void CopyToArg(char *src, XtArgVal *dst, unsigned int size);
static Cardinal GetNamesAndClasses(Widget w, XrmNameList names, XrmClassList classes);
static XrmResourceList CompileResourceTable(XtResourceList resources, Cardinal num_resources);
static Boolean GetResources(XmRendition rend,
                            Display *dsp,
                            Widget wid,
                            String resname,
                            String resclass,
                            XmStringTag tag,
                            ArgList arglist,
                            Cardinal argcount);
static void SetDefault(XmRendition rend);
#if USE_XFT
static XftColor GetCachedXftColor(Display *display, Pixel color);
static Boolean LookupXftFont(Display *display, XmRendition rend, XftFont **font);
static void CacheXftFont(Display *display, XmRendition rend);
#endif
/********    End Static Function Declarations    ********/
/* The warnings, failed conversions and XmNnoFontCallback calls made */
/* so far while creating renditions.  A converted render table whose */
/* creation made any is not cached (see _XmRenderTableCvtCachePut). */
static unsigned long rendition_failures = 0;
/* Resource List. */
/************************************************************************/
/* N.B.:  The SetDefault procedure has a hardcoded list of all the	*/
/*	common resources.  Be sure to update it when adding resources.	*/
/************************************************************************/
#define DEFAULT_loadModel XmAS_IS
#define DEFAULT_tag XmS
#define DEFAULT_fontName (String) XmAS_IS
#define DEFAULT_fontType (XmFontType) XmAS_IS
#define DEFAULT_font (XtPointer) XmAS_IS
#define DEFAULT_tabs (XmTabList) XmAS_IS
#define DEFAULT_background XmUNSPECIFIED_PIXEL
#define DEFAULT_foreground XmUNSPECIFIED_PIXEL
#define DEFAULT_underlineType XmAS_IS
#define DEFAULT_strikethruType XmAS_IS
#define DEFAULT_backgroundState XmAS_IS
#define DEFAULT_foregroundState XmAS_IS
#if USE_XFT
#  define DEFAULT_xftFont (XtPointer) XmAS_IS
#  define DEFAULT_fontStyle (String) NULL
#  define DEFAULT_fontFoundry (String) NULL
#  define DEFAULT_fontEncoding (String) NULL
#  define DEFAULT_fontSize 0
#endif
static XtResource _XmRenditionResources[] = {
    {XmNtag,
     XmCTag,
     XmRString,
     sizeof(XmStringTag),
     XtOffsetOf(_XmRenditionRec, tag),
     XmRImmediate,
     (XtPointer)DEFAULT_tag},
    {XmNfontName,
     XmCFontName,
     XmRString,
     sizeof(String),
     XtOffsetOf(_XmRenditionRec, fontName),
     XmRImmediate,
     (XtPointer)DEFAULT_fontName},
    {XmNfontType,
     XmCFontType,
     XmRFontType,
     sizeof(XmFontType),
     XtOffsetOf(_XmRenditionRec, fontType),
     XmRImmediate,
     (XtPointer)DEFAULT_fontType},
    {XmNfont,
     XmCFont,
     XmRFontStruct,
     sizeof(XtPointer),
     XtOffsetOf(_XmRenditionRec, font),
     XmRImmediate,
     (XtPointer)DEFAULT_font},
    {XmNloadModel,
     XmCLoadModel,
     XmRLoadModel,
     sizeof(unsigned char),
     XtOffsetOf(_XmRenditionRec, loadModel),
     XmRImmediate,
     (XtPointer)DEFAULT_loadModel},
    {XmNtabList,
     XmCTabList,
     XmRTabList,
     sizeof(XmTabList),
     XtOffsetOf(_XmRenditionRec, tabs),
     XmRImmediate,
     (XtPointer)DEFAULT_tabs},
#if USE_XFT
    {XmNxftFont,
     XmCXftFont,
     XmRPointer,
     sizeof(XtPointer),
     XtOffsetOf(_XmRenditionRec, xftFont),
     XmRImmediate,
     (XtPointer)DEFAULT_xftFont},
    {XmNrenditionBackground,
     XmCRenditionBackground,
     XmRRenditionPixel,
     sizeof(Pixel),
     XtOffsetOf(_XmRenditionRec, xftBackground.pixel),
     XmRImmediate,
     (XtPointer)DEFAULT_background},
    {XmNrenditionForeground,
     XmCRenditionForeground,
     XmRRenditionPixel,
     sizeof(Pixel),
     XtOffsetOf(_XmRenditionRec, xftForeground.pixel),
     XmRImmediate,
     (XtPointer)DEFAULT_foreground},
    {XmNfontStyle,
     XmCFontStyle,
     XmRString,
     sizeof(String),
     XtOffsetOf(_XmRenditionRec, fontStyle),
     XmRImmediate,
     (XtPointer)DEFAULT_fontStyle},
    {XmNfontFoundry,
     XmCFontFoundry,
     XmRString,
     sizeof(String),
     XtOffsetOf(_XmRenditionRec, fontFoundry),
     XmRImmediate,
     (XtPointer)DEFAULT_fontFoundry},
    {XmNfontEncoding,
     XmCFontEncoding,
     XmRString,
     sizeof(String),
     XtOffsetOf(_XmRenditionRec, fontEncoding),
     XmRImmediate,
     (XtPointer)DEFAULT_fontEncoding},
    {XmNfontSize,
     XmCFontSize,
     XmRInt,
     sizeof(int),
     XtOffsetOf(_XmRenditionRec, fontSize),
     XmRImmediate,
     (XtPointer)DEFAULT_fontSize},
#else
    {XmNrenditionBackground,
     XmCRenditionBackground,
     XmRRenditionPixel,
     sizeof(Pixel),
     XtOffsetOf(_XmRenditionRec, background),
     XmRImmediate,
     (XtPointer)DEFAULT_background},
    {XmNrenditionForeground,
     XmCRenditionForeground,
     XmRRenditionPixel,
     sizeof(Pixel),
     XtOffsetOf(_XmRenditionRec, foreground),
     XmRImmediate,
     (XtPointer)DEFAULT_foreground},
#endif
    {XmNunderlineType,
     XmCUnderlineType,
     XmRLineType,
     sizeof(unsigned char),
     XtOffsetOf(_XmRenditionRec, underlineType),
     XmRImmediate,
     (XtPointer)DEFAULT_underlineType},
    {XmNstrikethruType,
     XmCStrikethruType,
     XmRLineType,
     sizeof(unsigned char),
     XtOffsetOf(_XmRenditionRec, strikethruType),
     XmRImmediate,
     (XtPointer)DEFAULT_strikethruType},
    {XmNforegroundState,
     XmCGroundState,
     XmRGroundState,
     sizeof(unsigned char),
     XtOffsetOf(_XmRenditionRec, foregroundState),
     XmRImmediate,
     (XtPointer)DEFAULT_foregroundState},
    {XmNbackgroundState,
     XmCGroundState,
     XmRGroundState,
     sizeof(unsigned char),
     XtOffsetOf(_XmRenditionRec, backgroundState),
     XmRImmediate,
     (XtPointer)DEFAULT_backgroundState},
};
static XmConst Cardinal _XmNumRenditionResources = XtNumber(_XmRenditionResources);

/* Searches up widget hierarchy, quarkifying ancestor names and */
/* classes. */
static Cardinal GetNamesAndClasses(Widget w, XrmNameList names, XrmClassList classes)
{
  Cardinal length, j;
  XrmQuark t;
  WidgetClass wc;
  /* Return null-terminated quark arrays, with length the number of
     quarks (not including NULL) */
  for (length = 0; w != NULL; w = (Widget)w->core.parent) {
    names[length] = w->core.xrm_name;
    wc = XtClass(w);
    /* KLUDGE KLUDGE KLUDGE KLUDGE */
    if (w->core.parent == NULL && XtIsApplicationShell(w)) {
      classes[length] = ((ApplicationShellWidget)w)->application.xrm_class;
    }
    else
      classes[length] = wc->core_class.xrm_class;
    length++;
  }
  /* They're in backwards order, flop them around */
  for (j = 0; j < length / 2; j++) {
    t = names[j];
    names[j] = names[length - j - 1];
    names[length - j - 1] = t;
    t = classes[j];
    classes[j] = classes[length - j - 1];
    classes[length - j - 1] = t;
  }
  names[length] = NULLQUARK;
  classes[length] = NULLQUARK;
  return length;
} /* GetNamesAndClasses */

/* Converts resource list to quarkified list. */
static XrmResourceList CompileResourceTable(XtResourceList resources, Cardinal num_resources)
{
  Cardinal count;
  XrmResourceList table, tPtr;
  XtResourceList rPtr;
  tPtr = table = (XrmResourceList)_XmMallocArray(num_resources, sizeof(XrmResource));
  rPtr = resources;
  for (count = 0; count < num_resources; count++, tPtr++, rPtr++) {
    tPtr->xrm_name = XrmPermStringToQuark(rPtr->resource_name);
    tPtr->xrm_class = XrmPermStringToQuark(rPtr->resource_class);
    tPtr->xrm_type = XrmPermStringToQuark(rPtr->resource_type);
    tPtr->xrm_size = rPtr->resource_size;
    tPtr->xrm_offset = rPtr->resource_offset;
    tPtr->xrm_default_type = XrmPermStringToQuark(rPtr->default_type);
    tPtr->xrm_default_addr = rPtr->default_addr;
  }
  return (table);
}

/* The rendition resources, compiled on first use.  Call with the */
/* process lock held. */
static XrmResourceList RenditionResourceTable(void)
{
  static XrmResourceList table = NULL;
  if (table == NULL)
    table = CompileResourceTable(_XmRenditionResources, _XmNumRenditionResources);
  return (table);
}

#define SEARCH_LIST_SIZE 100

/* The database that the resources of renditions for wid, or for */
/* display dsp when wid is NULL, come from. */
static XrmDatabase RenditionDatabase(Display *dsp, Widget wid)
{
  if (wid != NULL)
    return (XtScreenDatabase(XtScreenOfObject(wid)));
  if (dsp != NULL)
    return (XtScreenDatabase(DefaultScreenOfDisplay(dsp)));
  return (NULL);
}

/* Set names and classes to those of wid and its ancestors, if wid is */
/* not NULL, followed by resname and resclass.  Returns their number; */
/* the arrays, of 100 entries, have room for two more. */
static Cardinal RenditionNames(
    Widget wid, String resname, String resclass, XrmName *names, XrmClass *classes)
{
  Cardinal length = 0;
  if (wid != NULL)
    length = GetNamesAndClasses(wid, names, classes);
  names[length] = XrmStringToQuark(resname);
  classes[length] = XrmStringToQuark(resclass);
  return (length + 1);
}

/* Get the search list of db for the resources of rendition tag (NULL */
/* for the default rendition), under the length names and classes */
/* from RenditionNames.  *searchList is stackSearchList, of */
/* SEARCH_LIST_SIZE entries, or an array for the caller to free. */
static void RenditionSearchList(XrmDatabase db,
                                XrmName *names,
                                XrmClass *classes,
                                Cardinal length,
                                XmStringTag tag,
                                XrmHashTable *stackSearchList,
                                XrmHashTable **searchList)
{
  static XrmClass QRendition = NULLQUARK;
  unsigned int searchListSize = SEARCH_LIST_SIZE;
  if (QRendition == NULLQUARK)
    QRendition = XrmPermStringToQuark(XmCRendition);
  if (tag != NULL) {
    names[length] = XrmStringToQuark(tag);
    classes[length] = QRendition;
    length++;
  }
  names[length] = NULLQUARK;
  classes[length] = NULLQUARK;
  *searchList = stackSearchList;
  while (!XrmQGetSearchList(db, names, classes, *searchList, searchListSize)) {
    if (*searchList == stackSearchList)
      *searchList = NULL;
    *searchList = (XrmHashTable *)_XmReallocArray(
        (char *)*searchList, searchListSize *= 2, sizeof(XrmHashTable));
  }
}

/*
 * Convert value, of type from_type, to to_type for a rendition of wid,
 * like XtConvertAndStore.  But XtConvertAndStore holds a reference to a
 * value whose converter counts them (that of XmRTabList) until wid is
 * destroyed, in a destroy callback of wid, and wid can be a widget whose
 * resources Xt is still fetching: Xt then takes its destroy callbacks
 * for a list given as a resource, and corrupts it.  A rendition copies
 * such a value (CleanupResources), so it takes no reference.
 */
static Boolean ConvertResource(
    Widget wid, XrmQuark from_type, XrmValue *value, String to_type, XrmValue *to)
{
  XrmValue result;
  XtConvert(wid, XrmQuarkToString(from_type), value, to_type, &result);
  if ((result.addr == NULL) || (result.size > to->size))
    return (False);
  memcpy(to->addr, result.addr, result.size);
  to->size = result.size;
  return (True);
}

/* Does resource database lookup for arglist, filling in defaults from */
/* resource list as necessary. */
static Boolean GetResources(XmRendition rend,
                            Display *dsp,
                            Widget wid,
                            String resname,
                            String resclass,
                            XmStringTag tag,
                            ArgList arglist,
                            Cardinal argcount)
{
  static XrmQuarkList quarks = NULL;
  static Cardinal num_quarks = 0;
  static Boolean *found = NULL;
  int i, j;
  XrmResourceList table;
  static XrmQuark QString = NULLQUARK;
  static XrmQuark Qfont;
  Arg *arg;
  XrmName argName;
  XrmResource *res;
  XrmName names[100];
  XrmClass classes[100];
  XrmDatabase db;
  XrmHashTable stackSearchList[SEARCH_LIST_SIZE];
  XrmHashTable *searchList = stackSearchList;
  Boolean got_one = False;
  XrmValue value;
  XrmQuark rawType;
  XrmValue convValue;
  Boolean have_value, copied;
  XtAppContext app = NULL;
  if (wid)
    app = XtWidgetToApplicationContext(wid);
  else if (dsp)
    app = XtDisplayToApplicationContext(dsp);
  if (app) {
    _XmAppLock(app);
  }
  _XmProcessLock();
  /* Initialize quark cache */
  if (quarks == NULL) {
    quarks = (XrmQuark *)_XmMallocArray(_XmNumRenditionResources, sizeof(XrmQuark));
    num_quarks = _XmNumRenditionResources;
  }
  /* Initialize found */
  if (found == NULL)
    found = (Boolean *)_XmMallocArray(_XmNumRenditionResources, sizeof(Boolean));
  bzero(found, _XmNumRenditionResources * sizeof(Boolean));
  /* Cache arglist */
  if (num_quarks < argcount) {
    quarks = (XrmQuark *)_XmReallocArray((char *)quarks, argcount, sizeof(XrmQuark));
    num_quarks = argcount;
  }
  for (i = 0; (Cardinal)i < argcount; i++)
    quarks[i] = XrmStringToQuark(arglist[i].name);
  table = RenditionResourceTable();
  if (QString == NULLQUARK) {
    QString = XrmPermStringToQuark(XtCString);
    Qfont = XrmPermStringToQuark(XmNfont);
  }
  /* Set resources from arglist. */
  for (arg = arglist, i = 0; (Cardinal)i < argcount; arg++, i++) {
    argName = quarks[i];
    for (j = 0, res = table; (Cardinal)j < _XmNumRenditionResources; j++, res++) {
      if (res->xrm_name == argName) {
        CopyFromArg((arg->value), ((char *)GetPtr(rend) + res->xrm_offset), res->xrm_size);
        found[j] = TRUE;
        break;
      }
    }
  }
  /* DB query */
  if ((db = RenditionDatabase(dsp, wid)) != NULL)
    RenditionSearchList(db,
                        names,
                        classes,
                        RenditionNames(wid, resname, resclass, names, classes),
                        tag,
                        stackSearchList,
                        &searchList);
  /* Loop over table */
  for (j = 0, res = table; (Cardinal)j < _XmNumRenditionResources; j++, res++) {
    if (!found[j]) {
      copied = False;
      have_value = False;
      if ((db != NULL) &&
          (XrmQGetSearchResource(searchList, res->xrm_name, res->xrm_class, &rawType, &value)))
      {
        /* convert if necessary */
        if (rawType != res->xrm_type) {
          if (wid != NULL) {
            convValue.size = res->xrm_size;
            convValue.addr = (char *)GetPtr(rend) + res->xrm_offset;
            /*
             * Check for special font case.
             * Depending upon the fontType resource, try to convert
             * to a FontSet, else to a FontStruct.
             */
            if ((res->xrm_name == Qfont) && (_XmRendFontType(rend) == XmFONT_IS_FONTSET))
              copied = have_value = ConvertResource(wid, rawType, &value, "FontSet", &convValue);
            else
              copied = have_value = ConvertResource(
                  wid, rawType, &value, XrmQuarkToString(res->xrm_type), &convValue);
            if (!have_value)
              rendition_failures++;
          }
          else
            have_value = False;
        }
        else
          have_value = True;
        /* Check for special font case */
        if (have_value) {
          if (res->xrm_name == Qfont) {
            _XmRendFontName(rend) = value.addr;
            copied = True;
          }
        }
      }
      if (!got_one && have_value)
        got_one = True;
      /* Set defaults */
      if (!have_value) {
        CopyFromArg((XtArgVal)(res->xrm_default_addr),
                    ((char *)GetPtr(rend) + res->xrm_offset),
                    res->xrm_size);
        copied = True;
      }
      /* Copy if needed */
      if (!copied) {
        /* Keep no pointer into the database, which can change or */
        /* go away while the rendition is still in use. */
        if (res->xrm_type == QString)
          *((String *)((char *)GetPtr(rend) + res->xrm_offset)) =
              (value.addr != NULL) ? XrmQuarkToString(XrmStringToQuark(value.addr)) : NULL;
        else if (value.addr != NULL)
          memcpy(((char *)GetPtr(rend) + res->xrm_offset), value.addr, res->xrm_size);
        else
          bzero(((char *)GetPtr(rend) + res->xrm_offset), res->xrm_size);
      }
    }
  }
  if (searchList != stackSearchList)
    XtFree((char *)searchList);
  _XmProcessUnlock();
  if (app) {
    _XmAppUnlock(app);
  }
  return (got_one);
}

/* Sets all resources to defaults from resource list. */
static void SetDefault(XmRendition rend)
{
  /* A more robust implementation of this routine would to to loop
   * over _XmRenditionResources and use CopyFromArg to reset values
   * in rend, but to improve performance we use direct assignments.
   */
  if (rend == NULL)
    return;
#if USE_XFT
  memset(&(_XmRendXftFG(rend)), 0, sizeof(XftColor));
  memset(&(_XmRendXftBG(rend)), 0, sizeof(XftColor));
#endif
  /* Leave _XmRendFontOnly unchanged.	 */
  /* Leave _XmRendRefcount unchanged.	 */
  _XmRendLoadModel(rend) = DEFAULT_loadModel;
  _XmRendTag(rend) = DEFAULT_tag;
  _XmRendFontName(rend) = DEFAULT_fontName;
  _XmRendFontType(rend) = DEFAULT_fontType;
  _XmRendFont(rend) = DEFAULT_font;
  /* Leave _XmRendDisplay unchanged.	 */
  /* Leave _XmRendGC unchanged.		 */
  /* Leave _XmRendTags unchanged.	 */
  /* Leave _XmRendCount unchanged.	 */
  /* Leave _XmRendHadEnds unchanged.	 */
  _XmRendTabs(rend) = DEFAULT_tabs;
  _XmRendBG(rend) = DEFAULT_background;
  _XmRendFG(rend) = DEFAULT_foreground;
  _XmRendUnderlineType(rend) = DEFAULT_underlineType;
  _XmRendStrikethruType(rend) = DEFAULT_strikethruType;
  _XmRendBGState(rend) = DEFAULT_backgroundState;
  _XmRendFGState(rend) = DEFAULT_foregroundState;
#if USE_XFT
  /* Opaque: Xft draws with the alpha, and text drawn with 0 is invisible. */
  _XmRendXftFG(rend).color.alpha = 0xFFFF;
  _XmRendXftBG(rend).color.alpha = 0xFFFF;
  _XmRendXftFont(rend) = DEFAULT_xftFont;
  _XmRendPattern(rend) = NULL;
  _XmRendFontStyle(rend) = DEFAULT_fontStyle;
  _XmRendFontFoundry(rend) = DEFAULT_fontFoundry;
  _XmRendFontEncoding(rend) = DEFAULT_fontEncoding;
  _XmRendFontSize(rend) = DEFAULT_fontSize;
  _XmRendPixelSize(rend) = 0;
  _XmRendFontSlant(rend) = 0;
  _XmRendFontSpacing(rend) = 0;
  _XmRendFontWeight(rend) = 0;
#endif
}

/* Extern function to pick out display from rendertable. */
Display *_XmRenderTableDisplay(XmRenderTable table)
{
  return (_XmRTDisplay(table));
}

/* Find a rendition in table with matching tag.  Call callback if not */
/* found and callback available.  Fail if need_font is true and */
/* rendition found does not provide font. */
XmRendition _XmRenderTableFindRendition(XmRenderTable table,
                                        XmStringTag tag,
                                        Boolean cached_tag,
                                        Boolean need_font,
                                        Boolean call,
                                        short *index)
{
  int i, j;
  XmRendition rend;
  Boolean hit = FALSE;
  XmDisplayCallbackStruct cb;
  XmDisplay dsp;
  XmRenderTable copy;
  if ((table == NULL) || (tag == NULL))
    return (NULL);
  for (;;) /* May have to try twice */ {
    for (i = 0; i < _XmRTCount(table); i++) {
      rend = _XmRTRenditions(table)[i];
      if ((cached_tag) ? (_XmRendTag(rend) == tag) : (strcmp(_XmRendTag(rend), tag) == 0)) {
        hit = TRUE;
        if ((_XmRendFont(rend) == NULL) && (_XmRendXftFont(rend) == NULL) &&
            NameIsString(_XmRendFontName(rend)))
        {
          if (_XmRendLoadModel(rend) == XmLOAD_DEFERRED)
            _XmRendLoadModel(rend) = XmLOAD_IMMEDIATE;
          ValidateAndLoadFont(rend, _XmRendDisplay(rend));
          if (need_font && (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL))
            break;
        }
        if (index != NULL)
          *index = i;
        return (rend);
      }
    }
    /* Are we done? */
    if (hit || !call)
      break;
    call = FALSE;
    /* Call callback */
    if (_XmRTDisplay(table) != NULL) {
      dsp = (XmDisplay)XmGetXmDisplay(_XmRTDisplay(table));
      /* CR 7964: XtHasCallbacks is surprisingly expensive, */
      /*	so we use a conservative approximation here. */
      if (dsp && dsp->display.noRenditionCallback) {
        copy = XmRenderTableCopy(table, NULL, 0);
        cb.reason = XmCR_NO_RENDITION;
        cb.event = NULL;
        cb.render_table = copy;
        cb.tag = tag;
        XtCallCallbackList((Widget)dsp, dsp->display.noRenditionCallback, &cb);
        if (cb.render_table != copy) {
          /* Callback mutated table.  Update table with */
          /* substitution and search again. */
          for (j = 0; j < _XmRTCount(table); j++)
            if (FreeRendition(_XmRTRenditions(table)[j]))
              FreeHandle(_XmRTRenditions(table)[j]);
          if (_XmRTRefcountDec(table) == 0)
            XtFree((char *)GetPtr(table));
          SetPtr(table, GetPtr(cb.render_table));
          FreeHandle(cb.render_table);
        }
        else
          break;
      }
      else
        break;
    }
    else
      break;
  }
  /* Didn't find it. */
  if (index != NULL)
    *index = -1;
  return (NULL);
}

/* If to has resource unset and from has it set, set in to. */
static void SetRend(XmRendition to, XmRendition from)
{
  if (NameIsString(_XmRendFontName(from)) && !NameIsString(_XmRendFontName(to)))
    _XmRendFontName(to) = _XmRendFontName(from);
  if ((_XmRendFontType(from) != XmAS_IS) && (_XmRendFontType(to) == XmAS_IS))
    _XmRendFontType(to) = _XmRendFontType(from);
  if ((_XmRendLoadModel(from) != XmAS_IS) && (_XmRendLoadModel(to) == XmAS_IS))
    _XmRendLoadModel(to) = _XmRendLoadModel(from);
  if ((_XmRendFont(from) != NULL) && ((unsigned int)(unsigned long)_XmRendFont(to) == XmAS_IS))
    _XmRendFont(to) = _XmRendFont(from);
  if (ListIsList(_XmRendTabs(from)) && !ListIsList(_XmRendTabs(to)))
    _XmRendTabs(to) = _XmRendTabs(from);
#if USE_XFT
  if ((_XmRendFG(from) != XmUNSPECIFIED_PIXEL) && (_XmRendFG(to) == XmUNSPECIFIED_PIXEL)) {
    _XmRendFG(to) = _XmRendFG(from);
    _XmRendXftFG(to) = GetCachedXftColor(_XmRendDisplay(to), _XmRendFG(to));
  }
  if ((_XmRendBG(from) != XmUNSPECIFIED_PIXEL) && (_XmRendBG(to) == XmUNSPECIFIED_PIXEL)) {
    _XmRendBG(to) = _XmRendBG(from);
    _XmRendXftBG(to) = GetCachedXftColor(_XmRendDisplay(to), _XmRendBG(to));
  }
  if ((_XmRendXftFont(from) != NULL) &&
      ((unsigned int)(unsigned long)_XmRendXftFont(to) == XmAS_IS))
    _XmRendXftFont(to) = _XmRendXftFont(from);
  if ((_XmRendFontStyle(from) != NULL) && _XmRendFontStyle(to) == NULL)
    _XmRendFontStyle(to) = _XmRendFontStyle(from);
  if ((_XmRendFontFoundry(from) != NULL) && _XmRendFontFoundry(to) == NULL)
    _XmRendFontFoundry(to) = _XmRendFontFoundry(from);
  if ((_XmRendFontEncoding(from) != NULL) && _XmRendFontEncoding(to) == NULL)
    _XmRendFontEncoding(to) = _XmRendFontEncoding(from);
  if (_XmRendFontSize(to) == 0)
    _XmRendFontSize(to) = _XmRendFontSize(from);
#else
  if ((_XmRendFG(from) != XmUNSPECIFIED_PIXEL) && (_XmRendFG(to) == XmUNSPECIFIED_PIXEL))
    _XmRendFG(to) = _XmRendFG(from);
  if ((_XmRendBG(from) != XmUNSPECIFIED_PIXEL) && (_XmRendBG(to) == XmUNSPECIFIED_PIXEL))
    _XmRendBG(to) = _XmRendBG(from);
#endif
  if ((_XmRendUnderlineType(from) != XmAS_IS) && (_XmRendUnderlineType(to) == XmAS_IS))
    _XmRendUnderlineType(to) = _XmRendUnderlineType(from);
  if ((_XmRendStrikethruType(from) != XmAS_IS) && (_XmRendStrikethruType(to) == XmAS_IS))
    _XmRendStrikethruType(to) = _XmRendStrikethruType(from);
}

/* Check that all resources are not default values. */
static Boolean RendComplete(XmRendition rend)
{
  return (((unsigned int)(unsigned long)_XmRendFontName(rend) != XmAS_IS) &&
          (_XmRendFontType(rend) != XmAS_IS) && (_XmRendLoadModel(rend) != XmAS_IS) &&
          (((unsigned int)(unsigned long)_XmRendFont(rend) != XmAS_IS)
#if USE_XFT
           || ((unsigned int)(unsigned long)_XmRendXftFont(rend) != XmAS_IS)
#endif
               ) &&
          ((unsigned int)(unsigned long)_XmRendTabs(rend) != XmAS_IS) &&
          (_XmRendFG(rend) != XmUNSPECIFIED_PIXEL) && (_XmRendBG(rend) != XmUNSPECIFIED_PIXEL) &&
          (_XmRendUnderlineType(rend) != XmAS_IS) && (_XmRendStrikethruType(rend) != XmAS_IS));
}

/* Search rt for all renditions matching tags, successively merging */
/* resource values in scr rendition. */
XmRendition _XmRenditionMerge(Display *d, /* unused */
                              XmRendition *scr,
                              XmRendition base_rend,
                              XmRenderTable rt,
                              XmStringTag base_tag,
                              XmStringTag *tags,
                              unsigned short tag_count,
                              Boolean copy)
{
  XmRendition rend, tmp;
  int i;
  if (scr == NULL) {
    rend = XmRenditionCreate(NULL, XmS, NULL, 0); /* Create new */
  }
  else {
    rend = *scr;
    if (copy) {
      if (NameIsString(_XmRendFontName(rend)))
        XtFree(_XmRendFontName(rend));
      if (ListIsList(_XmRendTabs(rend)))
        XmTabListFree(_XmRendTabs(rend));
    }
    SetDefault(rend); /* Reset state */
  }
  for (i = (tag_count - 1); i >= 0; i--) {
    tmp = _XmRenderTableFindRendition(rt, tags[i], TRUE, FALSE, TRUE, NULL);
    if (tmp == NULL)
      continue;
    SetRend(rend, tmp);
    if (RendComplete(rend))
      break;
  }
  if (!RendComplete(rend)) {
    short index;
    _XmRenderTableFindFallback(rt, base_tag, TRUE, &index, &tmp);
    if (tmp != NULL)
      SetRend(rend, tmp);
  }
  if (base_rend != NULL) {
    SetRend(rend, base_rend);
    if (_XmRendFGState(base_rend) == XmFORCE_COLOR)
#if USE_XFT
      _XmRendXftFG(rend) = _XmRendXftFG(base_rend);
#else
      _XmRendFG(rend) = _XmRendFG(base_rend);
#endif
    if (_XmRendBGState(base_rend) == XmFORCE_COLOR)
      _XmRendBG(rend) = _XmRendBG(base_rend);
  }
  CleanupResources(rend, copy);
  return (rend);
}

/****************
 * If the cached_tag flag is true, _XmRenderTableFindFallback assumes that the
 *   tag pointer is a pointer out of the (local) tag cache.
 *   Since XmRenditionCreate also uses tag pointers out of this cache,
 *   a string compare is avoided by simply comparing pointer values.
 ****************/
extern Boolean _XmRenderTableFindFallback(XmRenderTable rendertable,
                                          XmStringTag tag,
                                          Boolean cached_tag,
                                          short *indx,
                                          XmRendition *rend_ptr)
{
  XmStringTag search_cset = NULL;
  *indx = -1;
  if ((rendertable != NULL) && (_XmRTCount(rendertable) == 0)) {
    *rend_ptr = NULL;
    return (FALSE);
  }
  if (rendertable != NULL) {
    if (tag != NULL) {
      if (cached_tag) /* No XmSTRING_DEFAULT_CHARSET */ {
        *rend_ptr = (XmRendition)_XmRenderTableFindRendition(
            rendertable, tag, TRUE, TRUE, FALSE, indx);
        if (*rend_ptr != NULL)
          return (TRUE);
      }
      else {
        XmStringTag curtag;
        if ((strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
          curtag = _XmStringGetCurrentCharset();
        else
          curtag = tag;
        *rend_ptr = (XmRendition)_XmRenderTableFindRendition(
            rendertable, curtag, FALSE, TRUE, FALSE, indx);
        if (*rend_ptr != NULL)
          return (TRUE);
      }
      /* Didn't find a match.  See if tag is one of the defaults
             and search for the other. */
      if (_XmStringIsCurrentCharset(tag)) {
        search_cset = XmFONTLIST_DEFAULT_TAG;
        *rend_ptr = (XmRendition)_XmRenderTableFindRendition(
            rendertable, search_cset, TRUE, TRUE, FALSE, indx);
        if (*rend_ptr != NULL)
          return (TRUE);
      }
      else if ((tag == XmFONTLIST_DEFAULT_TAG) || (strcmp(tag, XmFONTLIST_DEFAULT_TAG) == 0)) {
        search_cset = _XmStringGetCurrentCharset();
        *rend_ptr = (XmRendition)_XmRenderTableFindRendition(
            rendertable, search_cset, FALSE, TRUE, FALSE, indx);
        if (*rend_ptr != NULL)
          return (TRUE);
      }
    }
    /* Otherwise pick up first font(set) if tag a default value. */
    if ((tag == NULL) || (tag == XmFONTLIST_DEFAULT_TAG) ||
        (strcmp(tag, XmFONTLIST_DEFAULT_TAG) == 0) || _XmStringIsCurrentCharset(tag))
      return (_XmRenderTableFindFirstFont(rendertable, indx, rend_ptr));
  }
  *rend_ptr = NULL;
  *indx = -1;
  return (FALSE);
}

extern Boolean _XmRenderTableFindFirstFont(XmRenderTable rendertable,
                                           short *indx,
                                           XmRendition *rend_ptr)
{
  int i, f_idx = -1, fs_idx = -1;
#if USE_XFT
  int xft_idx = -1;
#endif
  for (i = _XmRTCount(rendertable) - 1; i >= 0; i--) {
    *rend_ptr = _XmRTRenditions(rendertable)[i];
    if (_XmRendFont(*rend_ptr) != NULL) {
      if (_XmRendFontType(*rend_ptr) == XmFONT_IS_FONT)
        f_idx = i;
      else if (_XmRendFontType(*rend_ptr) == XmFONT_IS_FONTSET)
        fs_idx = i;
#if USE_XFT
    }
    else if (_XmRendXftFont(*rend_ptr) != NULL) {
      if (_XmRendFontType(*rend_ptr) == XmFONT_IS_XFT)
        xft_idx = i;
#endif
    }
  }
#if USE_XFT
  if (xft_idx >= 0) {
    *rend_ptr = _XmRTRenditions(rendertable)[xft_idx];
    *indx = xft_idx;
  }
  else
#endif
      if (fs_idx >= 0)
  {
    *rend_ptr = _XmRTRenditions(rendertable)[fs_idx];
    *indx = fs_idx;
  }
  else if (f_idx >= 0) {
    *rend_ptr = _XmRTRenditions(rendertable)[f_idx];
    *indx = f_idx;
  }
  else {
    *rend_ptr = NULL;
    *indx = -1;
    return (FALSE);
  }
  return (TRUE);
}

/* Put value of every resource in fromRend into toRend, copying where */
/* necessary. */
static void CopyInto(XmRendition toRend, XmRendition fromRend)
{
  _XmRendTag(toRend) = _XmStringCacheTag(_XmRendTag(fromRend), XmSTRING_TAG_STRLEN);
  /* CR 7890 - the fontName might be XmAS_IS here - if so, we
   ** obviously don't want to do an XtNewString (implicit strcpy)
   */
  if (!NameIsString(_XmRendFontName(fromRend)))
    _XmRendFontName(toRend) = NULL;
  else
    _XmRendFontName(toRend) = XtNewString(_XmRendFontName(fromRend));
  _XmRendFontType(toRend) = _XmRendFontType(fromRend);
  _XmRendLoadModel(toRend) = _XmRendLoadModel(fromRend);
  _XmRendFont(toRend) = _XmRendFont(fromRend);
  _XmRendDisplay(toRend) = _XmRendDisplay(fromRend);
  if (!ListIsList(_XmRendTabs(fromRend)))
    _XmRendTabs(toRend) = NULL;
  else
    _XmRendTabs(toRend) = XmTabListCopy(_XmRendTabs(fromRend), 0, 0);
#if USE_XFT
  _XmRendXftFont(toRend) = _XmRendXftFont(fromRend);
  _XmRendXftBG(toRend) = _XmRendXftBG(fromRend);
  _XmRendXftFG(toRend) = _XmRendXftFG(fromRend);
  _XmRendFontStyle(toRend) = _XmRendFontStyle(fromRend);
  _XmRendFontFoundry(toRend) = _XmRendFontFoundry(fromRend);
  _XmRendFontEncoding(toRend) = _XmRendFontEncoding(fromRend);
  _XmRendFontSize(toRend) = _XmRendFontSize(fromRend);
#else
  _XmRendBG(toRend) = _XmRendBG(fromRend);
  _XmRendFG(toRend) = _XmRendFG(fromRend);
#endif
  _XmRendUnderlineType(toRend) = _XmRendUnderlineType(fromRend);
  _XmRendStrikethruType(toRend) = _XmRendStrikethruType(fromRend);
}

/* As above, except only change resources in toRend that are default. */
static void MergeInto(XmRendition toRend, XmRendition fromRend)
{
  _XmRendTag(toRend) = _XmStringCacheTag(_XmRendTag(fromRend), XmSTRING_TAG_STRLEN);
  if ((_XmRendFontName(toRend) == NULL) && NameIsString(_XmRendFontName(fromRend)))
    _XmRendFontName(toRend) = XtNewString(_XmRendFontName(fromRend));
  if (_XmRendFontType(toRend) == XmAS_IS)
    _XmRendFontType(toRend) = _XmRendFontType(fromRend);
  if (_XmRendLoadModel(toRend) == XmAS_IS)
    _XmRendLoadModel(toRend) = _XmRendLoadModel(fromRend);
  if (_XmRendFont(toRend) == NULL)
    _XmRendFont(toRend) = _XmRendFont(fromRend);
  if (!ListIsList(_XmRendTabs(toRend)) && ListIsList(_XmRendTabs(fromRend)))
    _XmRendTabs(toRend) = XmTabListCopy(_XmRendTabs(fromRend), 0, 0);
#if USE_XFT
  if (_XmRendXftFont(toRend) == NULL)
    _XmRendXftFont(toRend) = _XmRendXftFont(fromRend);
  if (_XmRendBG(toRend) == XmUNSPECIFIED_PIXEL)
    _XmRendXftBG(toRend) = _XmRendXftBG(fromRend);
  if (_XmRendFG(toRend) == XmUNSPECIFIED_PIXEL)
    _XmRendXftFG(toRend) = _XmRendXftFG(fromRend);
  if (_XmRendFontStyle(toRend) == NULL)
    _XmRendFontStyle(toRend) = _XmRendFontStyle(fromRend);
  if (_XmRendFontFoundry(toRend) == NULL)
    _XmRendFontFoundry(toRend) = _XmRendFontFoundry(fromRend);
  if (_XmRendFontEncoding(toRend) == NULL)
    _XmRendFontEncoding(toRend) = _XmRendFontEncoding(fromRend);
  if (_XmRendFontSize(toRend) == 0)
    _XmRendFontSize(toRend) = _XmRendFontSize(fromRend);
#else
  if (_XmRendBG(toRend) == XmUNSPECIFIED_PIXEL)
    _XmRendBG(toRend) = _XmRendBG(fromRend);
  if (_XmRendFG(toRend) == XmUNSPECIFIED_PIXEL)
    _XmRendFG(toRend) = _XmRendFG(fromRend);
#endif
  if (_XmRendUnderlineType(toRend) == XmAS_IS)
    _XmRendUnderlineType(toRend) = _XmRendUnderlineType(fromRend);
  if (_XmRendUnderlineType(toRend) == XmAS_IS)
    _XmRendStrikethruType(toRend) = _XmRendStrikethruType(fromRend);
}

/* Make a Clone--new handle and new data structure--of a rendition. */
static XmRendition CloneRendition(XmRendition rend)
{
  _XmRendition copy;
  XmRendition copy_handle;
  if (rend == NULL)
    return (NULL);
  copy = (_XmRendition)XtMalloc(sizeof(_XmRenditionRec));
  bzero((char *)copy, sizeof(_XmRenditionRec));
  copy_handle = GetHandle(_XmRendition);
  SetPtr(copy_handle, copy);
  _XmRendFontOnly(copy_handle) = FALSE;
  _XmRendRefcount(copy_handle) = 1;
  CopyInto(copy_handle, rend);
  return (copy_handle);
}

/* Set the old handle to point to a new data structure. */
static XmRendition RenewRendition(XmRendition rend)
{
  _XmRendition copy;
  if (rend == NULL)
    return (NULL);
  copy = (_XmRendition)XtMalloc(sizeof(_XmRenditionRec));
  memcpy((char *)copy, (char *)GetPtr(rend), sizeof(_XmRenditionRec));
  SetPtr(rend, copy);
  _XmRendFontOnly(rend) = FALSE;
  _XmRendRefcount(rend) = 1;
  return (rend);
}

/* Allocate a new handle which points to the old data structure with */
/* an incremented refcount. */
static XmRendition CopyRendition(XmRendition rend)
{
  XmRendition copy;
  if (rend == NULL)
    return (NULL);
  if (_XmRendRefcountInc(rend) == 0) {
    _XmRendRefcountDec(rend);
    return (CloneRendition(rend));
  }
  else {
    copy = GetHandle(_XmRendition);
    SetPtr(copy, GetPtr(rend));
    return (copy);
  }
}

/* Increment the refcount.  Clone if overflow. */
static XmRendition DuplicateRendition(XmRendition rend)
{
  if (rend == NULL)
    return (NULL);
  if (_XmRendRefcountInc(rend) == 0) {
    _XmRendRefcountDec(rend);
    return (CloneRendition(rend));
  }
  else {
    return (rend);
  }
}

/* Make a copy of a rendition, *including* the "scratch" info (tags,
 * GC, hadEnds).
 * Shared indicates whether or not this is a shared copy.
 */
XmRendition _XmRenditionCopy(XmRendition rend, Boolean shared)
{
  XmRendition toRend;
  int i;
  if (rend == NULL)
    return (NULL);
  if (shared)
    toRend = CopyRendition(rend);
  else
    toRend = CloneRendition(rend);
  /* If we had to clone, copy the 'scratch' info. */
  if (*toRend != *rend) {
    _XmRendGC(toRend) = _XmRendGC(rend);
    _XmRendTagCount(toRend) = _XmRendTagCount(rend);
    _XmRendHadEnds(toRend) = _XmRendHadEnds(rend);
    _XmRendTags(toRend) = (XmStringTag *)_XmMallocArray(_XmRendTagCount(rend), sizeof(XmStringTag));
    for (i = 0; (unsigned int)i < _XmRendTagCount(rend); i++)
      _XmRendTags(toRend)[i] = _XmRendTags(rend)[i];
  }
  return (toRend);
}

/* Creates new rendertable, adding any new renditions. */
/* Mutate rendertable.  Copy renditions. */
XmRenderTable XmRenderTableAddRenditions(XmRenderTable oldtable,
                                         XmRendition *renditions,
                                         Cardinal rendition_count,
                                         XmMergeMode merge_mode)
{
  int i, next;
  int count = rendition_count;
  XmRendition rend, match;
  _XmRenderTable table;
  XmRenderTable newtable, tmptable = NULL;
  Boolean *matches;
  short idx;
  XtAppContext app = NULL;
  if ((renditions == NULL) || (rendition_count == 0))
    return (oldtable);
  if (_XmRendDisplay(renditions[0]))
    app = XtDisplayToApplicationContext(_XmRendDisplay(renditions[0]));
  if (app) {
    _XmAppLock(app);
  }
  else {
    _XmProcessLock();
  }
  if (oldtable == NULL) {
    /* Malloc new table */
    table = (_XmRenderTable)XtMalloc(
        sizeof(_XmRenderTableRec) +
        (sizeof(XmRendition) * (rendition_count - RENDITIONS_IN_STRUCT)));
    oldtable = GetHandle(_XmRenderTable);
    SetPtr(oldtable, table);
    _XmRTCount(oldtable) = rendition_count;
    _XmRTDisplay(oldtable) = NULL;
    _XmRTRefcount(oldtable) = 1;
    /* Copy renditions */
    for (i = 0; (Cardinal)i < rendition_count; i++) {
      _XmRTRenditions(oldtable)[i] = CopyRendition(renditions[i]);
      if (_XmRTDisplay(oldtable) == NULL)
        _XmRTDisplay(oldtable) = _XmRendDisplay(renditions[i]);
    }
  }
  else {
    matches = (Boolean *)_XmMallocArray(rendition_count, sizeof(Boolean));
    bzero(matches, rendition_count * sizeof(Boolean));
    /* May have to copy table if shared. */
    if (_XmRTRefcount(oldtable) > 1) {
      /* Allocate new table */
      table = (_XmRenderTable)XtMalloc(
          sizeof(_XmRenderTableRec) +
          (sizeof(XmRendition) * (_XmRTCount(oldtable) - RENDITIONS_IN_STRUCT)));
      newtable = GetHandle(_XmRenderTable);
      SetPtr(newtable, table);
      _XmRTDisplay(newtable) = _XmRTDisplay(oldtable);
      _XmRTRefcount(newtable) = 1;
      /* Move old Renditions. */
      for (i = 0; i < _XmRTCount(oldtable); i++)
        _XmRTRenditions(newtable)[i] = _XmRTRenditions(oldtable)[i];
      _XmRTCount(newtable) = _XmRTCount(oldtable);
      _XmRTRefcountDec(oldtable);
      /* Free at end so we don't get same memory from malloc. */
      tmptable = oldtable;
      oldtable = newtable;
    }
    /* Merge matching renditions */
    for (i = 0; (Cardinal)i < rendition_count; i++) {
      rend = renditions[i];
      match = _XmRenderTableFindRendition(oldtable, _XmRendTag(rend), TRUE, FALSE, FALSE, &idx);
      if ((match != NULL) && (merge_mode != XmDUPLICATE)) {
        /* Merge renditions. */
        switch (merge_mode) {
          case XmMERGE_REPLACE:
            if (FreeRendition(match))
              FreeHandle(match);
            _XmRTRenditions(oldtable)[idx] = CopyRendition(rend);
            break;
          case XmSKIP:
            break;
          case XmMERGE_OLD:
            if (_XmRendRefcount(match) > 1) {
              match = CloneRendition(match);
              _XmRTRenditions(oldtable)[idx] = match;
            }
            MergeInto(match, rend);
            break;
          case XmMERGE_NEW:
            rend = CloneRendition(rend);
            MergeInto(rend, match);
            _XmRTRenditions(oldtable)[idx] = rend;
            if (FreeRendition(match))
              FreeHandle(match);
            break;
          default:
            printf("NYI");
            break;
        }
        matches[i] = TRUE;
        --count;
      }
    }
    if (count > 0) /* Allocate new table */ {
      table = (_XmRenderTable)XtMalloc(
          sizeof(_XmRenderTableRec) +
          (sizeof(XmRendition) * (_XmRTCount(oldtable) + count - RENDITIONS_IN_STRUCT)));
      newtable = GetHandle(_XmRenderTable);
      SetPtr(newtable, table);
      _XmRTDisplay(newtable) = _XmRTDisplay(oldtable);
      _XmRTRefcount(newtable) = 1;
      /* Move old Renditions. */
      for (i = 0; i < _XmRTCount(oldtable); i++)
        _XmRTRenditions(newtable)[i] = _XmRTRenditions(oldtable)[i];
      /* Copy new renditions. */
      next = _XmRTCount(oldtable);
      for (i = 0; (Cardinal)i < rendition_count; i++) {
        if (!matches[i]) {
          _XmRTRenditions(newtable)[next] = CopyRendition(renditions[i]);
          if (_XmRTDisplay(newtable) == NULL)
            _XmRTDisplay(newtable) = _XmRTDisplay(oldtable);
          ++next;
        }
      }
      _XmRTCount(newtable) = _XmRTCount(oldtable) + count;
      /* Deallocate oldtable */
      XtFree((char *)GetPtr(oldtable));
      FreeHandle(oldtable);
    }
    /* Otherwise just return newhandle to oldtable */
    else {
      table = GetPtr(oldtable);
      newtable = GetHandle(_XmRenderTable);
      SetPtr(newtable, table);
      FreeHandle(oldtable);
    }
    XtFree((char *)matches);
    oldtable = newtable;
  }
  if (tmptable != NULL)
    FreeHandle(tmptable);
  if (app) {
    _XmAppUnlock(app);
  }
  else {
    _XmProcessUnlock();
  }
  return (oldtable);
}

/* Remove matching renditions. */
/* Mutates oldtable, decrements removed renditions. */
XmRenderTable XmRenderTableRemoveRenditions(XmRenderTable oldtable,
                                            XmStringTag *tags,
                                            int tag_count)
{
  XmRenderTable ret_val;
  XtAppContext app = NULL;
  if (_XmRTDisplay(oldtable))
    app = XtDisplayToApplicationContext(_XmRTDisplay(oldtable));
  if (app) {
    _XmAppLock(app);
  }
  else {
    _XmProcessLock();
  }
  ret_val = _XmRenderTableRemoveRenditions(oldtable, tags, tag_count, FALSE, XmFONT_IS_FONT, NULL);
  if (app) {
    _XmAppUnlock(app);
  }
  else {
    _XmProcessUnlock();
  }
  return ret_val;
}

/* Remove matching renditions. */
/* Mutates oldtable, decrements removed renditions. */
/* If chk_font TRUE, checks that font and type also match. */
XmRenderTable _XmRenderTableRemoveRenditions(XmRenderTable oldtable,
                                             XmStringTag *tags,
                                             int tag_count,
                                             Boolean chk_font,
                                             XmFontType type,
                                             XtPointer font)
{
  int i, j;
  int count;
  _XmRenderTable table;
  XmRenderTable newtable = NULL;
  if ((oldtable == NULL) || (tags == NULL) || (tag_count == 0))
    return (oldtable);
  count = 0;
  if (_XmRTRefcount(oldtable) > 1) {
    /* Allocate new table */
    table = (_XmRenderTable)XtMalloc(
        sizeof(_XmRenderTableRec) +
        (sizeof(XmRendition) * (_XmRTCount(oldtable) - RENDITIONS_IN_STRUCT)));
    newtable = GetHandle(_XmRenderTable);
    SetPtr(newtable, table);
    _XmRTDisplay(newtable) = _XmRTDisplay(oldtable);
    _XmRTRefcount(newtable) = 1;
    /* Move old Renditions. */
    for (i = 0; i < _XmRTCount(oldtable); i++)
      _XmRTRenditions(newtable)[i] = _XmRTRenditions(oldtable)[i];
    _XmRTCount(newtable) = _XmRTCount(oldtable);
    if (_XmRTRefcountDec(oldtable) == 0)
      XtFree((char *)GetPtr(oldtable));
    FreeHandle(oldtable);
    oldtable = newtable;
  }
  /* Iterate over renditions */
  for (i = 0; i < _XmRTCount(oldtable); i++) {
    /* Match against tags */
    for (j = 0; j < tag_count; j++) {
      if ((strcmp(_XmRendTag(_XmRTRenditions(oldtable)[i]), tags[j]) == 0) &&
          (!chk_font || ((font == _XmRendFont(_XmRTRenditions(oldtable)[i])) &&
                         (type == _XmRendFontType(_XmRTRenditions(oldtable)[i])))))
      {
        if (FreeRendition(_XmRTRenditions(oldtable)[i]))
          FreeHandle(_XmRTRenditions(oldtable)[i]);
        _XmRTRenditions(oldtable)[i] = NULL;
        break;
      }
    }
    if (_XmRTRenditions(oldtable)[i] != NULL) {
      if (count != i)
        _XmRTRenditions(oldtable)[count] = _XmRTRenditions(oldtable)[i];
      count++;
    }
  }
  if (count == 0)
  /* No renditions left. Return NULL. */
  {
    XmRenderTableFree(oldtable);
    return (NULL);
  }
  else if (count < _XmRTCount(oldtable)) {
    /* Realloc table */
    table = (_XmRenderTable)XtRealloc((char *)*oldtable,
                                      sizeof(_XmRenderTableRec) +
                                          (sizeof(XmRendition) * (count - RENDITIONS_IN_STRUCT)));
    if (newtable == NULL) {
      newtable = GetHandle(_XmRenderTable);
      FreeHandle(oldtable);
    }
    SetPtr(newtable, table);
    _XmRTCount(newtable) = count;
    return (newtable);
  }
  return (oldtable);
}

static void CopyFromArg(XtArgVal src, char *dst, unsigned int size)
{
  if (size > sizeof(XtArgVal))
    memcpy((char *)dst, (char *)src, (size_t)size);
  else {
    union {
      long longval;
      int intval;
      short shortval;
      char charval;
      char *charptr;
      XtPointer ptr;
    } u;

    char *p = (char *)&u;
    if (size == sizeof(long))
      u.longval = (long)src;
    else if (size == sizeof(int))
      u.intval = (int)src;
    else if (size == sizeof(short))
      u.shortval = (short)src;
    else if (size == sizeof(char))
      u.charval = (char)src;
    else if (size == sizeof(XtPointer))
      u.ptr = (XtPointer)src;
    else if (size == sizeof(char *))
      u.charptr = (char *)src;
    else
      p = (char *)&src;
    memcpy((char *)dst, p, (size_t)size);
  }
} /* CopyFromArg */

static void CopyToArg(char *src, XtArgVal *dst, unsigned int size)
{
  if ((void *)(*dst) == NULL) {
    /* old GetValues semantics (storing directly into arglists) are bad,
     * but preserve for compatibility as long as arglist contains NULL.
     */
    if (size == sizeof(long))
      *dst = (XtArgVal) * (long *)src;
    else if (size == sizeof(int))
      *dst = (XtArgVal) * (int *)src;
    else if (size == sizeof(short))
      *dst = (XtArgVal) * (short *)src;
    else if (size == sizeof(char))
      *dst = (XtArgVal) * (char *)src;
    else if (size == sizeof(XtPointer))
      *dst = (XtArgVal) * (XtPointer *)src;
    else if (size == sizeof(char *))
      *dst = (XtArgVal) * (char **)src;
    else if (size == sizeof(XtArgVal))
      *dst = *(XtArgVal *)src;
    else
      memcpy((char *)dst, (char *)src, (size_t)size);
  }
  else {
    /* proper GetValues semantics: argval is pointer to destination */
    if (size == sizeof(long))
      *((long *)*dst) = *(long *)src;
    else if (size == sizeof(int))
      *((int *)*dst) = *(int *)src;
    else if (size == sizeof(short))
      *((short *)*dst) = *(short *)src;
    else if (size == sizeof(char))
      *((char *)*dst) = *(char *)src;
    else if (size == sizeof(XtPointer))
      *((XtPointer *)*dst) = *(XtPointer *)src;
    else if (size == sizeof(char *))
      *((char **)*dst) = *(char **)src;
    else if (size == sizeof(XtArgVal))
      *((XtArgVal *)*dst) = *(XtArgVal *)src;
    else
      memcpy((char *)*dst, (char *)src, (size_t)size);
  }
} /* CopyToArg */

/* Copies renditions matching tags to a new table. */
/* If all renditions copied then duplicate rendertable, duplicate */
/* renditions.  Otherwise, mutate rendertable, duplicate renditions. */
XmRenderTable XmRenderTableCopy(XmRenderTable table, XmStringTag *tags, int tag_count)
{
  XmRenderTable rt = NULL;
  _XmRenderTable t = NULL;
  int i, j, count;
  int size;
  XmRendition rend = NULL;
  XtAppContext app = NULL;
  if (table == NULL)
    return ((XmRenderTable)NULL);
  if (_XmRTDisplay(table))
    app = XtDisplayToApplicationContext(_XmRTDisplay(table));
  if (app) {
    _XmAppLock(app);
  }
  else {
    _XmProcessLock();
  }
  count = 0;
  if ((_XmRTRefcountInc(table) == 0) || (tags != NULL)) {
    /* Malloc new table */
    _XmRTRefcountDec(table);
    if ((tags != NULL) && (tag_count > 0))
      size = (sizeof(_XmRendition) * (tag_count - RENDITIONS_IN_STRUCT));
    else
      size = (sizeof(_XmRendition) * (_XmRTCount(table) - RENDITIONS_IN_STRUCT));
    size = (size < 0) ? 0 : size;
    t = (_XmRenderTable)XtMalloc(sizeof(_XmRenderTableRec) + size);
    rt = GetHandle(_XmRenderTable);
    SetPtr(rt, t);
    _XmRTRefcount(rt) = 1;
  }
  if (tags == NULL) {
    /* Increment renditions. */
    for (i = 0; i < _XmRTCount(table); i++) {
      rend = DuplicateRendition(_XmRTRenditions(table)[i]);
      /* Check for overflow. */
      if (rend != _XmRTRenditions(table)[i])
        break;
    }
    if ((i < _XmRTCount(table)) || (rt != NULL)) /* Overflow! */ {
      /* Either a rendition or the table refcount overflowed. */
      if (rt == NULL) {
        /* Malloc new table, giving back the reference taken above. */
        _XmRTRefcountDec(table);
        t = (_XmRenderTable)XtMalloc(
            sizeof(_XmRenderTableRec) +
            (sizeof(_XmRendition) * (_XmRTCount(table) - RENDITIONS_IN_STRUCT)));
        rt = GetHandle(_XmRenderTable);
        SetPtr(rt, t);
        _XmRTRefcount(rt) = 1;
      }
      _XmRTCount(rt) = _XmRTCount(table);
      /* Move renditions done already. */
      for (j = 0; j < i; j++)
        _XmRTRenditions(rt)[j] = _XmRTRenditions(table)[j];
      if (i < _XmRTCount(rt)) {
        _XmRTRenditions(rt)[i] = rend;
        /* Copy rest */
        for (j = i + 1; j < _XmRTCount(rt); j++)
          _XmRTRenditions(rt)[j] = DuplicateRendition(_XmRTRenditions(table)[j]);
      }
    }
    else {
      rt = GetHandle(_XmRenderTable);
      SetPtr(rt, GetPtr(table));
    }
  }
  else {
    /* Copy matching renditions. */
    for (i = 0; i < tag_count; i++) {
      XmRendition match;
      match = XmRenderTableGetRendition(table, tags[i]);
      if (match != NULL)
        _XmRTRenditions(rt)[count++] = match;
    }
    /* Realloc table */
    t = (_XmRenderTable)XtRealloc((char *)t,
                                  sizeof(_XmRenderTableRec) +
                                      (sizeof(XmRendition) *
                                       (MAX(count, RENDITIONS_IN_STRUCT) - RENDITIONS_IN_STRUCT)));
    SetPtr(rt, t);
    _XmRTCount(rt) = count;
  }
  _XmRTDisplay(rt) = _XmRTDisplay(table);
  if (app) {
    _XmAppUnlock(app);
  }
  else {
    _XmProcessUnlock();
  }
  return (rt);
}

/* Decrement rendertable, free if refcount is zero.  XmRenditionFree */
/* renditions. */
void XmRenderTableFree(XmRenderTable table)
{
  int i;
  _XmProcessLock();
  for (i = 0; i < _XmRTCount(table); i++)
    if (FreeRendition(_XmRTRenditions(table)[i]))
      FreeHandle(_XmRTRenditions(table)[i]);
  if (_XmRTRefcountDec(table) == 0)
    XtFree((char *)GetPtr(table));
  FreeHandle(table);
  _XmProcessUnlock();
}

/* Get list of tags of all renditions in table. */
int XmRenderTableGetTags(XmRenderTable table, XmStringTag **tag_list)
{
  int i, ret_val;
  XtAppContext app = NULL;
  if (table == NULL) {
    *tag_list = NULL;
    return (0);
  }
  app = XtDisplayToApplicationContext(_XmRTDisplay(table));
  (void)app; /* unused but required for _XmAppLock */
  _XmAppLock(app);
  *tag_list = (XmStringTag *)_XmMallocArray(_XmRTCount(table), sizeof(XmStringTag));
  for (i = 0; i < _XmRTCount(table); i++)
    (*tag_list)[i] = XtNewString(_XmRendTag(_XmRTRenditions(table)[i]));
  ret_val = _XmRTCount(table);
  _XmAppUnlock(app);
  return ret_val;
}

/* Returns copy of matching rendition. */
XmRendition XmRenderTableGetRendition(XmRenderTable table, XmStringTag tag)
{
  XmRendition ret_val;
  _XmDisplayToAppContext(_XmRTDisplay(table));
  _XmAppLock(app);
  ret_val = CopyRendition(_XmRenderTableFindRendition(table, tag, FALSE, FALSE, FALSE, NULL));
  _XmAppUnlock(app);
  return ret_val;
}

/* Returns array of copies of matching renditions. */
XmRendition *XmRenderTableGetRenditions(XmRenderTable table, char **tags, Cardinal tag_count)
{
  XmRendition rend, *rends;
  int i, count;
  XtAppContext app = NULL;
  if ((table == NULL) || (tags == NULL) || (tag_count == 0))
    return (NULL);
  if (_XmRTDisplay(table)) {
    app = XtDisplayToApplicationContext(_XmRTDisplay(table));
    _XmAppLock(app);
  }
  rends = (XmRendition *)_XmMallocArray(tag_count, sizeof(XmRendition));
  count = 0;
  for (i = 0; (Cardinal)i < tag_count; i++) {
    rend = _XmRenderTableFindRendition(table, tags[i], FALSE, FALSE, FALSE, NULL);
    if (rend != NULL) {
      rends[count] = CopyRendition(rend);
      count++;
    }
  }
  if ((Cardinal)count < tag_count)
    rends = (XmRendition *)_XmReallocArray((char *)rends, count, sizeof(XmRendition));
  if (app) {
    _XmAppUnlock(app);
  }
  return (rends);
}

/* Wrapper for calling XtWarning functions. */
static void RenditionWarning(char *tag, char *type, char *message, Display *dpy)
{
  char *params[1];
  Cardinal num_params = 1;
  Display *d;
  _XmProcessLock();
  rendition_failures++;
  _XmProcessUnlock();
  /* the MotifWarningHandler installed in VendorS.c knows about
     this convention */
  params[0] = XME_WARNING;
  if (dpy)
    d = dpy;
  else
    d = _XmGetDefaultDisplay();
  if (d)
    XtAppWarningMsg(
        XtDisplayToApplicationContext(d), tag, type, "XmRendition", message, params, &num_params);
  else
    XtWarning(message);
}

/* Replace XmAS_IS and copy as necessary. */
static void CleanupResources(XmRendition rend, Boolean copy)
{
  if ((unsigned int)(unsigned long)_XmRendFont(rend) == XmAS_IS)
    _XmRendFont(rend) = NULL;
  else if (_XmRendFontType(rend) == XmAS_IS)
    _XmRendFontType(rend) = XmFONT_IS_FONT;
#if USE_XFT
  if ((unsigned int)(unsigned long)_XmRendXftFont(rend) == XmAS_IS)
    _XmRendXftFont(rend) = NULL;
#endif
  if (((unsigned int)(unsigned long)_XmRendFontName(rend) == XmAS_IS) ||
      (strcmp(_XmRendFontName(rend), XmSXmAS_IS) == 0))
    _XmRendFontName(rend) = NULL;
  else if (copy)
    _XmRendFontName(rend) = XtNewString(_XmRendFontName(rend));
  if ((unsigned int)(unsigned long)_XmRendTabs(rend) == XmAS_IS)
    _XmRendTabs(rend) = NULL;
  else if (copy)
    _XmRendTabs(rend) = XmTabListCopy(_XmRendTabs(rend), 0, 0);
}

/* Emit warning and set default if tag is NULL. */
static void ValidateTag(XmRendition rend, XmStringTag dflt)
{
  if (_XmRendTag(rend) == NULL) {
    RenditionWarning(_XmRendTag(rend), "NO_NULL_TAG", NO_NULL_TAG_MSG, _XmRendDisplay(rend));
    _XmRendTag(rend) = _XmStringCacheTag(dflt, XmSTRING_TAG_STRLEN);
  }
}
/* Make sure all the font related resources make sense together and */
/* then load the font specified by fontName if necessary. */
static void ValidateAndLoadFont(XmRendition rend, Display *display)
{
  XrmString locale;
  XtPointer font;
  XrmValue args[2];
  Cardinal num_args = 0;
  XrmValue fromVal;
  XrmValue toVal;
  Boolean result = False;
  _XmRendDisplay(rend) = display;
  if (_XmRendLoadModel(rend) != XmLOAD_DEFERRED) {
    XmDisplay dsp = NULL;
    XmDisplayCallbackStruct cb;
    if ((_XmRendFont(rend) == NULL) &&
#if USE_XFT
        (_XmRendXftFont(rend) == NULL) &&
#endif
        (_XmRendFontName(rend) != NULL))
    {
      if (_XmRendFontType(rend) != XmAS_IS) {
        if (display == NULL) {
          RenditionWarning(_XmRendTag(rend), "NULL_DISPLAY", NULL_DISPLAY_MSG, NULL);
          return;
        }
        args[0].addr = (XPointer)&display;
        args[0].size = sizeof(Display *);
        num_args++;
        fromVal.addr = _XmRendFontName(rend);
        fromVal.size = strlen(_XmRendFontName(rend));
        toVal.addr = (XPointer)&font;
        toVal.size = sizeof(XtPointer);
        switch (_XmRendFontType(rend)) {
          case XmFONT_IS_FONT:
            result = XtCallConverter(
                display, XtCvtStringToFontStruct, args, num_args, &fromVal, &toVal, NULL);
            break;
          case XmFONT_IS_FONTSET:
            /* libX11's XCreateFontSet() frees an empty base font name */
            /* list that it does not own; never hand it one. */
            if (*_XmRendFontName(rend) == '\0')
              break;
            locale = XrmQuarkToString(XrmStringToQuark(setlocale(LC_ALL, NULL)));
            args[1].addr = (XPointer)&locale;
            args[1].size = sizeof(XrmString);
            num_args++;
            result = XtCallConverter(
                display, XtCvtStringToFontSet, args, num_args, &fromVal, &toVal, NULL);
            break;
#if USE_XFT
          case XmFONT_IS_XFT: {
            FcResult res;
            FcPattern *p;
            XftFont *cached;
            if (LookupXftFont(display, rend, &cached)) {
              /* FreeRendition closes the font, so take a reference. */
              _XmRendXftFont(rend) = cached ? XftFontCopy(display, cached) : NULL;
            }
            else {
              _XmRendPattern(rend) = FcPatternCreate();
              if (_XmRendFontName(rend))
                FcPatternAddString(
                    _XmRendPattern(rend), FC_FAMILY, (XftChar8 *)_XmRendFontName(rend));
              if (_XmRendFontFoundry(rend))
                FcPatternAddString(
                    _XmRendPattern(rend), FC_FOUNDRY, (XftChar8 *)_XmRendFontFoundry(rend));
              if (_XmRendFontEncoding(rend))
                FcPatternAddString(
                    _XmRendPattern(rend), XFT_ENCODING, (XftChar8 *)_XmRendFontEncoding(rend));
              if (_XmRendFontStyle(rend))
                FcPatternAddString(
                    _XmRendPattern(rend), FC_STYLE, (XftChar8 *)_XmRendFontStyle(rend));
              if (_XmRendFontSize(rend))
                FcPatternAddInteger(_XmRendPattern(rend), FC_SIZE, _XmRendFontSize(rend));
              if (_XmRendPixelSize(rend))
                FcPatternAddInteger(_XmRendPattern(rend), FC_PIXEL_SIZE, _XmRendPixelSize(rend));
              if (_XmRendFontSlant(rend))
                FcPatternAddInteger(_XmRendPattern(rend), FC_SLANT, _XmRendFontSlant(rend));
              if (_XmRendFontWeight(rend))
                FcPatternAddInteger(_XmRendPattern(rend), FC_WEIGHT, _XmRendFontWeight(rend));
              if (_XmRendFontSpacing(rend))
                FcPatternAddInteger(_XmRendPattern(rend), FC_SPACING, _XmRendFontSpacing(rend));
              p = XftFontMatch(display, 0, _XmRendPattern(rend), &res);
              _XmRendXftFont(rend) = p ? XftFontOpenPattern(display, p) : NULL;
              /* The font owns the pattern only once it is open. */
              if (p != NULL && _XmRendXftFont(rend) == NULL)
                FcPatternDestroy(p);
              CacheXftFont(display, rend);
            }
          }
            result = _XmRendXftFont(rend) != NULL;
            break;
#endif
          default:
            RenditionWarning(
                _XmRendTag(rend), "INVALID_TYPE", INVALID_TYPE_MSG, _XmRendDisplay(rend));
            break;
        }
        /* NoFontCallback. */
        if (!result) {
          if (display != NULL) {
            dsp = (XmDisplay)XmGetXmDisplay(display);
            cb.reason = XmCR_NO_FONT;
            cb.event = NULL;
            cb.rendition = rend;
            cb.font_name = _XmRendFontName(rend);
            /* We must know for sure whether there are any */
            /* callbacks, so we have to use XtHasCallbacks. */
            if (XtHasCallbacks((Widget)dsp, XmNnoFontCallback) == XtCallbackHasSome) {
              _XmProcessLock();
              rendition_failures++;
              _XmProcessUnlock();
              XtCallCallbackList((Widget)dsp, dsp->display.noFontCallback, &cb);
              return;
            }
          }
          RenditionWarning(
              _XmRendTag(rend), "CONVERSION_FAILED", CONVERSION_FAILED_MSG, _XmRendDisplay(rend));
        }
        else {
#if USE_XFT
          if (_XmRendFontType(rend) != XmFONT_IS_XFT)
#endif
            _XmRendFont(rend) = font;
        }
      }
      else {
        RenditionWarning(
            _XmRendTag(rend), "NULL_FONT_TYPE", NULL_FONT_TYPE_MSG, _XmRendDisplay(rend));
      }
    }
    else if ((_XmRendLoadModel(rend) == XmLOAD_IMMEDIATE) && (_XmRendFont(rend) == NULL) &&
#if USE_XFT
             (_XmRendXftFont(rend) == NULL) &&
#endif
             (_XmRendFontName(rend) == NULL))
    {
      RenditionWarning(
          _XmRendTag(rend), "NULL_LOAD_IMMEDIATE", NULL_LOAD_IMMEDIATE_MSG, _XmRendDisplay(rend));
    }
  }
}

/* Create new rendition. */
XmRendition XmRenditionCreate(Widget widget, XmStringTag tag, ArgList arglist, Cardinal argcount)
{
  XmRendition ret_val;
  XtAppContext app = NULL;
  /* If cannot applock, assume its an internal call (from
   *	fontlist code, etc.) and already process locked.
   */
  if (widget)
    app = XtWidgetToApplicationContext(widget);
  if (app) {
    _XmAppLock(app);
  }
  else {
    _XmProcessLock();
  }
  ret_val = _XmRenditionCreate(NULL, widget, XmS, XmCRenderTable, tag, arglist, argcount, NULL);
  if (app) {
    _XmAppUnlock(app);
  }
  else {
    _XmProcessUnlock();
  }
  return ret_val;
}

/* Internal function.  Called from XmRenditionCreate, resource */
/* converter, and Mrm create function. */
XmRendition _XmRenditionCreate(Display *display,
                               Widget widget,
                               String resname,
                               String resclass,
                               XmStringTag tag,
                               ArgList arglist,
                               Cardinal argcount,
                               Boolean *in_db)
{
  XmRendition rend;
  _XmRendition rend_int;
  Boolean result;
  if ((display == NULL) && (widget != NULL))
    display = XtDisplayOfObject(widget);
  if ((tag != NULL) && (tag != XmFONTLIST_DEFAULT_TAG) &&
      (strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
    tag = _XmStringGetCurrentCharset();
  /* Allocate rendition. */
  rend_int = (_XmRendition)XtMalloc(sizeof(_XmRenditionRec));
  bzero((char *)rend_int, sizeof(_XmRenditionRec));
  rend = GetHandle(_XmRendition);
  SetPtr(rend, rend_int);
  _XmRendRefcount(rend) = 1;
  /* For now, FontOnly renditions aren't implemented. */
  _XmRendFontOnly(rend) = FALSE;
  /* X resource DB query */
  result = GetResources(rend, display, widget, resname, resclass, tag, arglist, argcount);
  if (in_db != NULL)
    *in_db = result;
  if (tag == NULL) {
    if (result == FALSE) {
      XtFree((char *)rend_int);
      FreeHandle(rend);
      return (NULL);
    }
    else
      tag = _MOTIF_DEFAULT_LOCALE;
  }
  _XmRendTag(rend) = _XmStringCacheTag(tag, XmSTRING_TAG_STRLEN);
  /* Cleanup and validate resources. */
  CleanupResources(rend, TRUE);
  ValidateTag(rend, XmS);
  ValidateAndLoadFont(rend, display);
  return (rend);
}

/* Mrm create function for rendertables. */
Widget _XmCreateRenderTable(Widget parent,
                            String name,       /* unused */
                            ArgList arglist,   /* unused */
                            Cardinal argcount) /* unused */
{
  XmRenderTable newtable;
  _XmRenderTable table;
  /* Malloc new table */
  table = (_XmRenderTable)XtMalloc(sizeof(_XmRenderTableRec));
  newtable = GetHandle(_XmRenderTable);
  SetPtr(newtable, table);
  _XmRTCount(newtable) = 0;
  _XmRTRefcount(newtable) = 1;
  _XmRTDisplay(newtable) = XtDisplay(parent);
  return ((Widget)newtable);
}

/* Mrm create function for renditions. */
Widget _XmCreateRendition(Widget parent, String name, ArgList arglist, Cardinal argcount)
{
  XmRenderTable rt = (XmRenderTable)parent;
  _XmRenderTable table;
  XmRendition rend;
  table = GetPtr(rt);
  rend = _XmRenditionCreate(
      _XmRTDisplay(rt), NULL, XmS, XmCRenderTable, name, arglist, argcount, NULL);
  /* Ignore repeats */
  if (_XmRenderTableFindRendition(rt, _XmRendTag(rend), TRUE, FALSE, FALSE, NULL) != NULL) {
    if (FreeRendition(rend))
      FreeHandle(rend);
    return ((Widget)NULL);
  }
  table = (_XmRenderTable)XtRealloc(
      (char *)table,
      sizeof(_XmRenderTableRec) +
          (sizeof(XmRendition) * ((_XmRTCount(rt) + 1) - RENDITIONS_IN_STRUCT)));
  SetPtr(rt, table);
  /* Copy new rendition. */
  _XmRTRenditions(rt)[_XmRTCount(rt)] = CopyRendition(rend);
  _XmRTCount(rt)++;
  return ((Widget)rend);
}

/* Free data structure and any copied values. */
/* TRUE return -> freed.  FALSE -> just decremented. */
static Boolean FreeRendition(XmRendition rendition)
{
  if (rendition == NULL)
    return (FALSE);
  if (_XmRendRefcountDec(rendition) == 0) {
    /* CR 7890 - the fontName might be XmAS_IS here */
    if (NameIsString(_XmRendFontName(rendition)))
      XtFree(_XmRendFontName(rendition));
    if (ListIsList(_XmRendTabs(rendition)))
      XmTabListFree(_XmRendTabs(rendition));
    if (_XmRendTagCount(rendition) != 0)
      XtFree((char *)_XmRendTags(rendition));
#if USE_XFT
    if (_XmRendXftFont(rendition)) {
      XftFontClose(_XmRendDisplay(rendition), _XmRendXftFont(rendition));
      _XmRendXftFont(rendition) = NULL;
    }
    if (_XmRendPattern(rendition)) {
      FcPatternDestroy(_XmRendPattern(rendition));
      _XmRendPattern(rendition) = NULL;
    }
#endif
    XtFree((char *)GetPtr(rendition));
    return (TRUE);
  }
  return (FALSE);
}

void XmRenditionFree(XmRendition rendition)
{
  if (rendition == NULL)
    return;
  _XmProcessLock();
  FreeRendition(rendition);
  FreeHandle(rendition);
  _XmProcessUnlock();
}

/*
 * Render tables converted from resource strings.
 *
 * The String to RenderTable converters look up the resources of each
 * rendition in the resource database under the names and classes of the
 * widget and its ancestors ("*form*renderTable.bold.fontName"), so one
 * string can make different tables for different widgets, and Xt does
 * not cache them.  But a table depends on its widget only through the
 * values that those lookups find, and through the screen and colormap
 * that the values are converted for (the converters of the rendition
 * resources take no other argument from the widget).  A key holds all
 * of that, with the string, the resource name and the locale; a table
 * converted for one widget is kept under its key, and a widget whose key
 * is the same gets a copy of it (CopyConvertedTable) instead of a new
 * table.
 *
 * The copy is a table of the widget's own, as a new one would be, not a
 * table sharing the kept one's data or renditions: the renditions of a
 * table are handed out (XmString calls the XmNnoFontCallback of a widget
 * with no font with the first rendition of its table, and the callback
 * may give that rendition a font with XmRenditionUpdate), and what is
 * done to them must not reach the tables of other widgets.  The copy
 * shares only what a new conversion shares too: the fonts, which Xt and
 * the Xft font cache keep per display, and the quark strings.
 *
 * A string that names no rendition is a font list, whose entries look
 * up their resources for the display rather than for the widget: the
 * entry of such a table also holds what those lookups found, and is used
 * only while they still find the same.
 *
 * A table is only kept when making it gave no warning, no failed
 * conversion and no XmNnoFontCallback call, and when all its fonts are
 * loaded, so that a copy behaves as a new table would.  The tables are
 * kept per display: a table that was not used since the cache last grew
 * is dropped when it grows again, and all of them when the display is
 * closed.
 */
typedef struct {
  char *data;
  Cardinal size, max;
  char *stack; /* the initial buffer, on the caller's stack */
} XmRTKeyRec, *XmRTKey;

typedef struct _XmRTCacheEntryRec {
  XmHashValue hash;
  XmRTKeyRec key;         /* widget lookups */
  XmRTKeyRec check;       /* display lookups of a font list, by tag */
  XmRenderTable table;    /* NULL until the entry is cached */
  unsigned long failures; /* rendition_failures when the key was made */
  Boolean used;           /* since the last sweep */
} XmRTCacheEntryRec, *XmRTCacheEntry;

typedef struct _XmRTCacheRec {
  struct _XmRTCacheRec *next;
  Display *display;
  XmHashTable entries; /* XmRTCacheEntry -> XmRTCacheEntry */
  Cardinal sweep;      /* look for unused tables at this many entries */
} XmRTCacheRec;

static XmRTCacheRec *_XmRTCaches = NULL;

/* A key starts in a buffer of this size on the stack. */
#define KEY_BUF_SIZE 512

static void KeyInit(XmRTKey key, char *stack, Cardinal size)
{
  key->data = key->stack = stack;
  key->size = 0;
  key->max = size;
}

static void KeyGrow(XmRTKey key, Cardinal size)
{
  key->max = 2 * (key->size + size);
  if (key->data == key->stack) {
    key->data = XtMalloc(key->max);
    memcpy(key->data, key->stack, key->size);
  }
  else
    key->data = XtRealloc(key->data, key->max);
}

static inline void KeyAdd(XmRTKey key, const void *data, Cardinal size)
{
  if (key->size + size > key->max)
    KeyGrow(key, size);
  memcpy(key->data + key->size, data, size);
  key->size += size;
}

/* A hash of all of key, a word at a time: the keys of one string */
/* differ only in the values found for the widgets. */
static XmHashValue KeyHash(XmRTKey key)
{
  unsigned long h = key->size, w;
  Cardinal i;
  for (i = 0; i + sizeof(w) <= key->size; i += sizeof(w)) {
    memcpy(&w, key->data + i, sizeof(w));
    h = (h ^ w) * 0x9E3779B1UL;
    h ^= h >> 15;
  }
  if (i < key->size) {
    w = 0;
    memcpy(&w, key->data + i, key->size - i);
    h = (h ^ w) * 0x9E3779B1UL;
  }
  h ^= h >> (sizeof(h) * 4);
  return ((XmHashValue)(h & 0x7FFFFFFF));
}

/* Free a key, or move it from the stack to the heap. */
static void KeyFree(XmRTKey key)
{
  if (key->data != key->stack)
    XtFree(key->data);
  key->data = NULL;
}

static void KeyKeep(XmRTKey to, XmRTKey key)
{
  to->size = to->max = key->size;
  to->stack = NULL;
  to->data = XtMalloc(MAX(key->size, 1));
  memcpy(to->data, key->data, key->size);
  KeyFree(key);
}

/* Add to key what GetResources finds in db for rendition tag (NULL */
/* for the default rendition) under the length names and classes from */
/* RenditionNames: which resources are found, and the type and value */
/* of each. */
static void KeyAddRendition(XmRTKey key,
                            XrmDatabase db,
                            XrmName *names,
                            XrmClass *classes,
                            Cardinal length,
                            XmStringTag tag)
{
  XrmHashTable stackSearchList[SEARCH_LIST_SIZE];
  XrmHashTable *searchList = stackSearchList;
  XrmResourceList res = RenditionResourceTable();
  XrmRepresentation type;
  XrmValue value;
  unsigned long found = 0; /* a bit for each of the 17 resources */
  Cardinal j, found_at;
  /* As in _XmRenditionCreate. */
  if ((tag != NULL) && (strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
    tag = _XmStringGetCurrentCharset();
  found_at = key->size;
  KeyAdd(key, &found, sizeof(found));
  if (db == NULL)
    return;
  RenditionSearchList(db, names, classes, length, tag, stackSearchList, &searchList);
  for (j = 0; j < _XmNumRenditionResources; j++, res++)
    if (XrmQGetSearchResource(searchList, res->xrm_name, res->xrm_class, &type, &value)) {
      found |= 1UL << j;
      KeyAdd(key, &type, sizeof(type));
      KeyAdd(key, &value.size, sizeof(value.size));
      if (value.addr != NULL)
        KeyAdd(key, value.addr, value.size);
    }
  memcpy(key->data + found_at, &found, sizeof(found));
  if (searchList != stackSearchList)
    XtFree((char *)searchList);
}

/* Add to check what XmFontListEntryLoad finds for each of the */
/* renditions tagged in tags, from a font list entry, of display. */
static void KeyAddFontListRenditions(XmRTKey check, Display *display, XmRTKey tags)
{
  XrmName names[100];
  XrmClass classes[100];
  XrmDatabase db = RenditionDatabase(display, NULL);
  Cardinal length = RenditionNames(NULL, XmS, XmCFontList, names, classes);
  char *tag;
  for (tag = tags->data; tag < tags->data + tags->size; tag += strlen(tag) + 1) {
    KeyAdd(check, tag, strlen(tag) + 1);
    KeyAddRendition(check, db, names, classes, length, tag);
  }
}

static Boolean CompareRTCacheEntry(XmHashKey k1, XmHashKey k2)
{
  XmRTCacheEntry e1 = (XmRTCacheEntry)k1, e2 = (XmRTCacheEntry)k2;
  return (e1->hash == e2->hash && e1->key.size == e2->key.size &&
          memcmp(e1->key.data, e2->key.data, e1->key.size) == 0);
}

static XmHashValue HashRTCacheEntry(XmHashKey k)
{
  return (((XmRTCacheEntry)k)->hash);
}

/* Whether the display lookups of the font list of a cached entry */
/* still find the same. */
static Boolean RTCacheEntryValid(XmRTCacheEntry entry, Display *display)
{
  XmRTKeyRec tags, now;
  char tags_buf[KEY_BUF_SIZE], now_buf[KEY_BUF_SIZE];
  char *p;
  Boolean valid;
  if (entry->check.size == 0)
    return (True);
  /* The tags are the strings at the start of each part of check. */
  KeyInit(&tags, tags_buf, sizeof(tags_buf));
  for (p = entry->check.data; p < entry->check.data + entry->check.size;) {
    unsigned long found;
    Cardinal j;
    KeyAdd(&tags, p, strlen(p) + 1);
    p += strlen(p) + 1;
    memcpy(&found, p, sizeof(found));
    p += sizeof(found);
    for (j = 0; j < _XmNumRenditionResources; j++)
      if (found & (1UL << j)) {
        unsigned int size;
        p += sizeof(XrmRepresentation);
        memcpy(&size, p, sizeof(size));
        p += sizeof(size) + size;
      }
  }
  KeyInit(&now, now_buf, sizeof(now_buf));
  KeyAddFontListRenditions(&now, display, &tags);
  valid = (now.size == entry->check.size && memcmp(now.data, entry->check.data, now.size) == 0);
  KeyFree(&tags);
  KeyFree(&now);
  return (valid);
}

static void FreeRTCacheEntry(XmRTCacheEntry entry)
{
  if (entry->table != NULL)
    XmRenderTableFree(entry->table);
  XtFree(entry->key.data);
  XtFree(entry->check.data);
  XtFree((char *)entry);
}

static Boolean FreeRTCacheMapProc(XmHashKey k, XtPointer value, XtPointer data)
{
  FreeRTCacheEntry((XmRTCacheEntry)value);
  return False;
}

/* A copy of rend, a rendition of a converted table, that shares none */
/* of its data: the same rendition as a new conversion makes when the */
/* fonts are loaded already. */
static XmRendition CopyConvertedRendition(XmRendition rend)
{
  XmRendition copy;
  _XmRendition data;
  data = (_XmRendition)XtMalloc(sizeof(_XmRenditionRec));
  memcpy((char *)data, (char *)GetPtr(rend), sizeof(_XmRenditionRec));
  copy = GetHandle(_XmRendition);
  SetPtr(copy, data);
  _XmRendRefcount(copy) = 1;
  if (NameIsString(_XmRendFontName(rend)))
    _XmRendFontName(copy) = XtNewString(_XmRendFontName(rend));
  if (ListIsList(_XmRendTabs(rend)))
    _XmRendTabs(copy) = XmTabListCopy(_XmRendTabs(rend), 0, 0);
  _XmRendGC(copy) = NULL;
  _XmRendTags(copy) = NULL;
  _XmRendTagCount(copy) = 0;
  _XmRendHadEnds(copy) = FALSE;
#if USE_XFT
  /* FreeRendition closes the font: take a reference.  The pattern is */
  /* only kept by the rendition that opened the font. */
  if (_XmRendXftFont(rend) != NULL)
    _XmRendXftFont(copy) = XftFontCopy(_XmRendDisplay(rend), _XmRendXftFont(rend));
  _XmRendPattern(copy) = NULL;
#endif
  return (copy);
}

/* A copy of table, a converted table, that shares none of its data. */
static XmRenderTable CopyConvertedTable(XmRenderTable table)
{
  XmRenderTable rt;
  _XmRenderTable t;
  int i;
  t = (_XmRenderTable)XtMalloc(
      sizeof(_XmRenderTableRec) +
      (sizeof(XmRendition) * (MAX(_XmRTCount(table), RENDITIONS_IN_STRUCT) - RENDITIONS_IN_STRUCT)));
  rt = GetHandle(_XmRenderTable);
  SetPtr(rt, t);
  _XmRTMark(rt) = 0;
  _XmRTRefcount(rt) = 1;
  _XmRTCount(rt) = _XmRTCount(table);
  _XmRTDisplay(rt) = _XmRTDisplay(table);
  for (i = 0; i < _XmRTCount(table); i++)
    _XmRTRenditions(rt)[i] = CopyConvertedRendition(_XmRTRenditions(table)[i]);
  return (rt);
}

/* Drop a table that was not used since the last sweep. */
static Boolean SweepRTCacheMapProc(XmHashKey k, XtPointer value, XtPointer data)
{
  XmRTCacheEntry entry = (XmRTCacheEntry)value;
  if (entry->used)
    entry->used = False;
  else {
    (void)_XmRemoveHashEntry((XmHashTable)data, k);
    FreeRTCacheEntry(entry);
  }
  return False;
}

/* Destroy callback of the hook object of a display, which */
/* XtCloseDisplay destroys while the display is still open. */
static void RTCacheDisplayClosed(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmRTCacheRec **prev, *rec;
  _XmProcessLock();
  for (prev = &_XmRTCaches; (rec = *prev) != NULL; prev = &rec->next)
    if (rec->display == (Display *)client_data) {
      *prev = rec->next;
      _XmMapHashTable(rec->entries, FreeRTCacheMapProc, NULL);
      _XmFreeHashTable(rec->entries);
      XtFree((char *)rec);
      break;
    }
  _XmProcessUnlock();
}

/* Call with the process lock held. */
static XmRTCacheRec *FindRTCache(Display *display, Boolean create)
{
  XmRTCacheRec *rec;
  for (rec = _XmRTCaches; rec != NULL; rec = rec->next)
    if (rec->display == display)
      return (rec);
  if (!create)
    return (NULL);
  rec = XtNew(XmRTCacheRec);
  rec->display = display;
  rec->entries = _XmAllocHashTable(16, CompareRTCacheEntry, HashRTCacheEntry);
  rec->sweep = 16;
  rec->next = _XmRTCaches;
  _XmRTCaches = rec;
  XtAddCallback(
      XtHooksOfDisplay(display), XtNdestroyCallback, RTCacheDisplayClosed, (XtPointer)display);
  return (rec);
}

/*
 * The String to RenderTable converter for resource resname of wid calls
 * this before converting spec.  It returns a copy of the table cached
 * for the same key, or NULL with a pending key in *pending, which the
 * converter passes to _XmRenderTableCvtCachePut with the table it makes.
 */
XmRenderTable _XmRenderTableCvtCacheGet(
    Widget wid, String resname, String resclass, char *spec, XtPointer *pending)
{
  XmRTCacheEntryRec key;
  XmRTCacheEntry entry = NULL, cached;
  XmRTCacheRec *rec;
  XmRenderTable table = NULL;
  char key_buf[KEY_BUF_SIZE], spec_buf[256];
  XrmName names[100];
  XrmClass classes[100];
  Cardinal length;
  XrmDatabase db;
  Screen *screen;
  Widget w;
  char *s, *tag, *strtok_buf;
  const char *locale;
  size_t spec_len = strlen(spec);
  *pending = NULL;
  if (wid == NULL)
    return (NULL);
  _XmProcessLock();
  KeyInit(&key.key, key_buf, sizeof(key_buf));
  key.check.data = NULL;
  key.check.size = 0;
  key.table = NULL;
  key.failures = rendition_failures;
  key.used = False;
  /* The screen and the colormap that the values are converted for. */
  screen = XtScreenOfObject(wid);
  KeyAdd(&key.key, &screen, sizeof(screen));
  for (w = wid; !XtIsWidget(w); w = XtParent(w))
    ;
  KeyAdd(&key.key, &w->core.colormap, sizeof(Colormap));
  /* The locale of font sets. */
  locale = setlocale(LC_ALL, NULL);
  if (locale == NULL)
    locale = "";
  KeyAdd(&key.key, locale, strlen(locale) + 1);
  KeyAdd(&key.key, resname, strlen(resname) + 1);
  KeyAdd(&key.key, spec, spec_len + 1);
  /* The values found for each rendition that the converter makes. */
  db = RenditionDatabase(NULL, wid);
  length = RenditionNames(wid, resname, resclass, names, classes);
  KeyAddRendition(&key.key, db, names, classes, length, NULL);
  s = (spec_len < sizeof(spec_buf)) ? memcpy(spec_buf, spec, spec_len + 1) : XtNewString(spec);
  for (tag = strtok_r(s, _XmRENDITION_TAG_DELIMITERS, &strtok_buf); tag != NULL;
       tag = strtok_r(NULL, _XmRENDITION_TAG_DELIMITERS, &strtok_buf))
    KeyAddRendition(&key.key, db, names, classes, length, tag);
  if (s != spec_buf)
    XtFree(s);
  key.hash = KeyHash(&key.key);
  if ((rec = FindRTCache(XtDisplayOfObject(wid), False)) != NULL &&
      (cached = (XmRTCacheEntry)_XmGetHashEntry(rec->entries, (XmHashKey)&key)) != NULL &&
      RTCacheEntryValid(cached, XtDisplayOfObject(wid))) {
    cached->used = True;
    table = CopyConvertedTable(cached->table);
  }
  if (table == NULL) {
    entry = XtNew(XmRTCacheEntryRec);
    *entry = key;
    KeyKeep(&entry->key, &key.key);
  }
  else
    KeyFree(&key.key);
  _XmProcessUnlock();
  *pending = (XtPointer)entry;
  return (table);
}

/* Whether the fonts of all the renditions of table are loaded. */
static Boolean RenderTableFontsLoaded(XmRenderTable table)
{
  XmRendition rend;
  int i;
  for (i = 0; i < _XmRTCount(table); i++) {
    rend = _XmRTRenditions(table)[i];
    if ((_XmRendFont(rend) == NULL) &&
#if USE_XFT
        (_XmRendXftFont(rend) == NULL) &&
#endif
        NameIsString(_XmRendFontName(rend)))
      return (False);
  }
  return (True);
}

/*
 * Cache table, which the converter made for the pending key from
 * _XmRenderTableCvtCacheGet, or free the key if table is NULL or must
 * not be shared.  wid is the widget of the conversion, and font_list
 * whether the string was converted as a font list.
 */
void _XmRenderTableCvtCachePut(Widget wid,
                               XtPointer pending,
                               XmRenderTable table,
                               Boolean font_list)
{
  XmRTCacheEntry entry = (XmRTCacheEntry)pending, stale;
  XmRTCacheRec *rec;
  Display *display = XtDisplayOfObject(wid);
  if (entry == NULL)
    return;
  _XmProcessLock();
  if ((table != NULL) && (entry->failures == rendition_failures) &&
      RenderTableFontsLoaded(table) && (rec = FindRTCache(display, True)) != NULL)
  {
    if (font_list) {
      XmRTKeyRec tags, check;
      char tags_buf[KEY_BUF_SIZE], check_buf[KEY_BUF_SIZE];
      int i;
      KeyInit(&tags, tags_buf, sizeof(tags_buf));
      for (i = 0; i < _XmRTCount(table); i++) {
        XmStringTag tag = _XmRendTag(_XmRTRenditions(table)[i]);
        KeyAdd(&tags, tag, strlen(tag) + 1);
      }
      KeyInit(&check, check_buf, sizeof(check_buf));
      KeyAddFontListRenditions(&check, display, &tags);
      KeyFree(&tags);
      KeyKeep(&entry->check, &check);
    }
    /* An entry whose font list lookups found something else. */
    if ((stale = (XmRTCacheEntry)_XmRemoveHashEntry(rec->entries, (XmHashKey)entry)) != NULL)
      FreeRTCacheEntry(stale);
    if (_XmHashTableCount(rec->entries) >= rec->sweep) {
      _XmMapHashTable(rec->entries, SweepRTCacheMapProc, (XtPointer)rec->entries);
      rec->sweep = MAX(16, 2 * _XmHashTableCount(rec->entries));
    }
    entry->table = CopyConvertedTable(table);
    entry->used = True;
    _XmAddHashEntry(rec->entries, (XmHashKey)entry, (XtPointer)entry);
    if (_XmHashTableCount(rec->entries) > _XmHashTableSize(rec->entries))
      _XmResizeHashTable(rec->entries, 2 * _XmHashTableSize(rec->entries));
    entry = NULL;
  }
  if (entry != NULL)
    FreeRTCacheEntry(entry);
  _XmProcessUnlock();
}

/* Get resource values from rendition. */
void XmRenditionRetrieve(XmRendition rendition, ArgList arglist, Cardinal argcount)
{
  int i, j;
  Arg *arg;
  XtResource *res;
  char *as_is = (char *)XmAS_IS;
  if (rendition == NULL)
    return;
  _XmProcessLock();
  /* Get resources */
  for (i = 0; (Cardinal)i < argcount; i++) {
    arg = &(arglist[i]);
    for (j = 0; (Cardinal)j < _XmNumRenditionResources; j++) {
      res = &(_XmRenditionResources[j]);
      if (strcmp(res->resource_name, arg->name) == 0) {
        /* CR 7890: Font hook - if there's a fontName but the
         ** font hasn't been fetched yet, now's a good time to
         ** get it - if the caller wants to use the font to, say,
         ** compute font metrics for layout (as CSText does), it won't
         ** like to get NULL back
         */
        if (strcmp(res->resource_name, XmNfont) == 0) {
          if ((_XmRendFont(rendition) == NULL) &&
#if USE_XFT
              (_XmRendXftFont(rendition) == NULL) &&
#endif
              (_XmRendFontName(rendition) != NULL))
          {
            if (_XmRendLoadModel(rendition) == XmLOAD_DEFERRED)
              _XmRendLoadModel(rendition) = XmLOAD_IMMEDIATE;
            ValidateAndLoadFont(rendition, _XmRendDisplay(rendition));
          }
          if (_XmRendFont(rendition) == NULL
#if USE_XFT
              && _XmRendXftFont(rendition) == NULL
#endif
          )
            CopyToArg((char *)&as_is, &(arg->value), sizeof(char *));
          else
            CopyToArg(((char *)GetPtr(rendition) + res->resource_offset),
                      &(arg->value),
                      res->resource_size);
        }
        else if (((strcmp(res->resource_name, XmNfontName) == 0) &&
                  (_XmRendFontName(rendition) == NULL)) ||
                 ((strcmp(res->resource_name, XmNtabList) == 0) &&
                  (_XmRendTabs(rendition) == NULL)))
          CopyToArg((char *)&as_is, &(arg->value), sizeof(char *));
        else
          CopyToArg(((char *)GetPtr(rendition) + res->resource_offset),
                    &(arg->value),
                    res->resource_size);
        break;
      }
    }
  }
  _XmProcessUnlock();
}

/* Set resources in rendition. */
/* Renew rendition if necessary, then update resources. */
void XmRenditionUpdate(XmRendition rendition, ArgList arglist, Cardinal argcount)
{
  XmStringTag oldtag;
  char *oldname;
  XtPointer oldfont;
  XmTabList oldtabs;
  int i, j;
  XtResource *res;
  Arg *arg;
  Display *display = _XmGetDefaultDisplay();
  Boolean can_free;
  XtAppContext app = NULL;
  if (rendition == NULL)
    return;
  if (_XmRendDisplay(rendition)) {
    app = XtDisplayToApplicationContext(_XmRendDisplay(rendition));
    _XmAppLock(app);
  }
  if (_XmRendDisplay(rendition) && (_XmRendDisplay(rendition) != display))
    display = _XmRendDisplay(rendition);
  /* Save old values to check for dependencies and free memory. */
  oldtag = _XmRendTag(rendition);
  oldname = _XmRendFontName(rendition);
  oldfont = _XmRendFont(rendition);
  oldtabs = _XmRendTabs(rendition);
  can_free = TRUE;
  /* New memory if needed. */
  if (_XmRendRefcount(rendition) > 1) {
    _XmRendRefcountDec(rendition);
    RenewRendition(rendition);
    can_free = FALSE;
  }
  for (i = 0; (Cardinal)i < argcount; i++) {
    arg = &(arglist[i]);
    for (j = 0; (Cardinal)j < _XmNumRenditionResources; j++) {
      res = &(_XmRenditionResources[j]);
      if (strcmp(res->resource_name, arg->name) == 0) {
        CopyFromArg(
            (arg->value), ((char *)GetPtr(rendition) + res->resource_offset), res->resource_size);
        break;
      }
    }
  }
  CopyInto(rendition, rendition);
  /** Validate resources **/
  /* CR 7890 - handle cases of fontName == NULL and fontName == XmAS_IS */
  /* If fontName changed but not font, NULL font so it's updated.
   ** (first make sure we won't crash on the strcmp) */
  if (NameIsString(oldname) && NameIsString(_XmRendFontName(rendition))) {
    if (strcmp(oldname, _XmRendFontName(rendition)) != 0) {
      if (oldfont == _XmRendFont(rendition))
        _XmRendFont(rendition) = NULL;
    }
    if (can_free)
      XtFree(oldname);
  }
  /* Also handle the case where we started with a NULL fontName and
   ** had a real fontName specified */
  else if ((oldname == NULL) && NameIsString(_XmRendFontName(rendition))) {
    if (oldfont == _XmRendFontName(rendition))
      _XmRendFont(rendition) = NULL;
  }
  if (_XmRendFont(rendition) == (XtPointer)XmAS_IS)
    _XmRendFont(rendition) = NULL;
  if ((oldtabs != _XmRendTabs(rendition)) && can_free)
    XmTabListFree(oldtabs);
  ValidateTag(rendition, oldtag);
  ValidateAndLoadFont(rendition, display);
  if (app) {
    _XmAppUnlock(app);
  }
}

/*****************************************************************************/
/* XmRenderTableCvtToProp takes a rendertable and converts it to             */
/* an ascii string in the following format:				     */
/* tag : char*								     */
/* font : either fontid (integer) or [ fontid, fontid ... fontid ] or -1     */
/* tablist : [ tab1, ... tabn ] or -1					     */
/* background : pixel or -1						     */
/* foreground : pixel or -1						     */
/* underlineType : integer (from enum in Xm.h ) or -1			     */
/* strikethruType : integer (from enum in Xm.h ) or -1			     */
/* 									     */
/* example:								     */
/* "tag, font, tablist, background, foreground, underlineType, 		     */
/*  strikethruType\n							     */
/* bold, 10000031, -1, -1, -1, -1, -1\n					     */
/* underline, 10000029, -1, -1, -1, -1, -1\n				     */
/* default, 10000029, [ 1.234 1 0 0, 2.43 2 0 2], 1, 2, 0, 0\n		     */
/* japanese, [10000029, 10000030], -1, -1, -1, -1, -1"			     */
/* 									     */
/* The first line gives a complete list of the attributes by name.	     */
/* on the destination side,  attributes which are not understood	     */
/* or are outdated can be ignored.  The conversion of each rendition	     */
/* passes a single "line" which contains the fields in order.		     */
/*****************************************************************************/
/* Note that this MUST be in the same order as the output conversion
   below!! */
static XmConst char *CVTproperties[] = {
    XmNtag,
    XmNfont,
    XmNtabList,
    XmNbackground,
    XmNforeground,
    XmNunderlineType,
    XmNstrikethruType,
    NULL,
};
/* Must be big enough to take all the above strings concatenated with
   commas separating them */
static char CVTtransfervector[256];
static int CVTtvinited = 0;
/* Use this macro to encapsulate the code that extends the output
   buffer as needed.  srcsize is strlen(src); the buffer always has
   room for it and the terminating NUL. */
#define CVTaddString(dest, src, srcsize) \
  { \
    if ((size_t)(chars_used + (srcsize)) >= (size_t)allocated_size) { \
      while ((size_t)(chars_used + (srcsize)) >= (size_t)allocated_size) \
        allocated_size *= 2; \
      dest = XtRealloc(dest, allocated_size); \
    } \
    memcpy((dest) + chars_used, src, srcsize); \
    chars_used += (srcsize); \
    (dest)[chars_used] = '\0'; \
  }

unsigned int XmRenderTableCvtToProp(Widget widget, /* unused */
                                    XmRenderTable table,
                                    char **prop_return)
{
  int i;
  int allocated_size = 256;
  int chars_used = 0, size;
  char *buffer;
  char *str;
  char temp[256];
  XmRendition rendition;
  _XmWidgetToAppContext(widget);
  _XmAppLock(app);
  buffer = XtMalloc(allocated_size);
  _XmProcessLock();
  if (CVTtvinited == 0) {
    CVTtvinited = 1;
    size_t len = 0;
    CVTtransfervector[0] = '\0';
    for (i = 0; CVTproperties[i] != NULL && len < sizeof(CVTtransfervector); i++)
      len += snprintf(CVTtransfervector + len,
                      sizeof(CVTtransfervector) - len,
                      "%s,",
                      CVTproperties[i]);
    if (len < sizeof(CVTtransfervector))
      snprintf(CVTtransfervector + len, sizeof(CVTtransfervector) - len, "\n");
  }
  /* Copy the transfer vector into the output buffer. */
  strncpy(buffer, CVTtransfervector, allocated_size - 1);
  buffer[allocated_size - 1] = '\0';
  chars_used = strlen(buffer);
  _XmProcessUnlock();
  /* Now iterate over the list of renditions */
  for (i = 0; i < _XmRTCount(table); i++) {
    rendition = _XmRTRenditions(table)[i];
    /* Names and tab lists can be of any length: add them piecewise */
    /* rather than through a fixed size buffer that truncates them. */
    CVTaddString(buffer, "\"", 1);
    CVTaddString(buffer, _XmRendTag(rendition), strlen(_XmRendTag(rendition)));
    CVTaddString(buffer, "\", ", 3);
    if ((_XmRendFontType(rendition) == XmAS_IS) || (_XmRendFontName(rendition) == NULL)) {
      CVTaddString(buffer, "-1, ", 4);
    }
    else {
      snprintf(temp, sizeof(temp), "%u \"", _XmRendFontType(rendition));
      CVTaddString(buffer, temp, strlen(temp));
      CVTaddString(buffer, _XmRendFontName(rendition), strlen(_XmRendFontName(rendition)));
      snprintf(temp, sizeof(temp), "\" %u,", _XmRendLoadModel(rendition));
      CVTaddString(buffer, temp, strlen(temp));
    }
    if ((unsigned int)(unsigned long)_XmRendTabs(rendition) == XmAS_IS ||
        _XmRendTabs(rendition) == NULL) {
      CVTaddString(buffer, "-1, ", 4);
    }
    else {
      _XmTab tab;
      _XmTabList tlist;
      int number;
      CVTaddString(buffer, "[ ", 2);
      tlist = (_XmTabList)_XmRendTabs(rendition);
      number = tlist->count;
      tab = (_XmTab)tlist->start;
      while (number > 0) {
        snprintf(temp,
                 sizeof temp,
                 " %f %u %u %u, ",
                 tab->value,
                 tab->units,
                 tab->alignment,
                 tab->offsetModel);
        CVTaddString(buffer, temp, strlen(temp));
        tab = (_XmTab)tab->next;
        number--;
      }
      CVTaddString(buffer, " ], ", 4);
    }
    if (_XmRendBG(rendition) == XmUNSPECIFIED_PIXEL)
      str = "-1, ";
    else {
      snprintf(temp, sizeof(temp), "%lu, ", _XmRendBG(rendition));
      str = temp;
    }
    size = strlen(str);
    CVTaddString(buffer, str, size);
    if (_XmRendFG(rendition) == XmUNSPECIFIED_PIXEL)
      str = "-1, ";
    else {
      snprintf(temp, sizeof(temp), "%lu, ", _XmRendFG(rendition));
      str = temp;
    }
    size = strlen(str);
    CVTaddString(buffer, str, size);
    if (_XmRendUnderlineType(rendition) == XmAS_IS)
      str = "-1, ";
    else {
      snprintf(temp, sizeof(temp), "%d, ", _XmRendUnderlineType(rendition));
      str = temp;
    }
    size = strlen(str);
    CVTaddString(buffer, str, size);
    if (_XmRendStrikethruType(rendition) == XmAS_IS)
      str = "-1, ";
    else {
      snprintf(temp, sizeof(temp), "%d, ", _XmRendStrikethruType(rendition));
      str = temp;
    }
    size = strlen(str);
    CVTaddString(buffer, str, size);
    CVTaddString(buffer, "\n", 1);
  }
  /* Return the converted rendertable string */
  *prop_return = buffer;
  _XmAppUnlock(app);
  /* chars_used is always the size - the NULL terminator */
  return (chars_used + 1);
}

typedef enum { T_NL, T_INT, T_FLOAT, T_SEP, T_OPEN, T_CLOSE, T_STR, T_EOF, T_ERROR } TokenType;

typedef struct _TokenRec {
  TokenType type;
  int integer;
  float real;
  char *string;
} TokenRec, *Token;

/* Every token but T_EOF consumes at least one character.  A T_STR */
/* string is owned by the token: callers that keep it must set */
/* token->string to NULL, otherwise it is freed by the next call. */
static Token ReadToken(char *string, int *position, Token reusetoken)
{
  Token new_token = reusetoken;
  int pos = *position;
  int count;
  if (new_token->type == T_STR)
    XtFree(new_token->string);
  new_token->string = NULL;
  /* Skip whitespace but not newlines */
  while (isspace((unsigned char)string[pos]) && !(string[pos] == '\n'))
    pos++;
  /* Select token type */
  switch (string[pos]) {
    case '\0':
      new_token->type = T_EOF;
      break;
    case '\n':
      new_token->type = T_NL;
      pos++;
      break;
    case ',':
      new_token->type = T_SEP;
      pos++;
      break;
    case '[':
      new_token->type = T_OPEN;
      pos++;
      break;
    case ']':
      new_token->type = T_CLOSE;
      pos++;
      break;
    case '"': /* String result */
      count = 0;
      while (string[pos + 1 + count] != '"' && string[pos + 1 + count] != '\0')
        count++; /* Scan for end of string */
      new_token->type = T_STR;
      new_token->string = (char *)XtMalloc(count + 1);
      memcpy(new_token->string, &string[pos + 1], count);
      new_token->string[count] = 0; /* Null terminate */
      pos += count + 1;             /* Move past opening quote and text */
      if (string[pos] == '"')       /* and past the end quote, if any */
        pos++;
      break;
    default:
      if (isalpha((unsigned char)string[pos])) /* String result */ {
        char temp[80];
        for (count = 0; isalpha((unsigned char)string[pos + count]) && count < 79; count++)
          temp[count] = string[pos + count];
        temp[count] = 0;
        pos += count;
        new_token->type = T_STR;
        new_token->string = XtNewString(temp);
      }
      else {
        /* start converting a float number.  If it is exactly integer
       then we return an int,  otherwise return a float */
        double result;
        char *newpos;
        result = strtod(&(string[pos]), &newpos);
        if (newpos == &(string[pos])) {
          /* Not a number either: consume the offending character. */
          new_token->type = T_ERROR;
          pos++;
        }
        else {
          pos = newpos - string;
          if (result >= INT_MIN && result <= INT_MAX && ((double)(int)result) == result) {
            /* Integer result */
            new_token->type = T_INT;
            new_token->integer = (int)result;
          }
          else {
            new_token->type = T_FLOAT;
            new_token->real = (float)result;
          }
        }
      }
  }
  *position = pos;
  return (new_token);
}
#if USE_XFT
/*
 * Per-display Xft state: the XftDraw of each window that has been drawn
 * into, the XftColor of each (colormap, pixel) pair, and the font opened
 * for each font description, all in hash tables so that every Xft draw
 * does not scan a list or ask the server for the colour of a pixel.  A
 * record lives until its display is closed.
 *
 * The colour of a pixel is remembered only where it cannot change: in the
 * default colormap of a screen whose default visual has a static class.
 * In any other colormap a cell may be stored into, or freed and allocated
 * again, so the colour of the pixel of a GC is asked for at every draw.
 * The colours of renditions are remembered in the default colormap, as
 * they always have been.
 */
typedef struct _XmXftColorRec {
  Colormap colormap;
  XftColor color; /* color.pixel is the rest of the key */
} XmXftColorRec;

typedef struct _XmXftDisplayRec {
  struct _XmXftDisplayRec *next;
  Display *display;
  int extension;
  XmHashTable draws;  /* Window -> XftDraw * */
  XmHashTable colors; /* XmXftColorRec * -> XmXftColorRec * */
  XmHashTable fonts;  /* XmXftFontRec * -> XmXftFontRec * */
} XmXftDisplayRec;

static XmXftDisplayRec *_XmXftDisplays = NULL;

static Boolean CompareXftColor(XmHashKey k1, XmHashKey k2)
{
  XmXftColorRec *c1 = (XmXftColorRec *)k1, *c2 = (XmXftColorRec *)k2;
  return (c1->color.pixel == c2->color.pixel && c1->colormap == c2->colormap);
}

static XmHashValue HashXftColor(XmHashKey k)
{
  XmXftColorRec *c = (XmXftColorRec *)k;
  return (XmHashValue)(c->color.pixel ^ (c->colormap << 7));
}

static Boolean FreeXftColor(XmHashKey k, XtPointer value, XtPointer data)
{
  XtFree((char *)value);
  return False;
}

/*
 * A font opened by ValidateAndLoadFont, and the rendition resources that
 * describe it.  The record holds its own reference to the font, and its
 * own copy of the strings, so that it does not depend on what happens to
 * the renditions that use the font.
 */
typedef struct _XmXftFontRec {
  char *name, *foundry, *encoding, *style;
  int size, pixel_size, slant, weight, spacing;
  XftFont *font; /* NULL if it failed to open */
} XmXftFontRec;

static void XftFontDesc(XmRendition rend, XmXftFontRec *desc)
{
  desc->name = NameIsString(_XmRendFontName(rend)) ? _XmRendFontName(rend) : NULL;
  desc->foundry = _XmRendFontFoundry(rend);
  desc->encoding = _XmRendFontEncoding(rend);
  desc->style = _XmRendFontStyle(rend);
  desc->size = _XmRendFontSize(rend);
  desc->pixel_size = _XmRendPixelSize(rend);
  desc->slant = _XmRendFontSlant(rend);
  desc->weight = _XmRendFontWeight(rend);
  desc->spacing = _XmRendFontSpacing(rend);
  desc->font = NULL;
}

static Boolean SameString(char *s1, char *s2)
{
  return ((s1 == NULL && s2 == NULL) || (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0));
}

static Boolean CompareXftFont(XmHashKey k1, XmHashKey k2)
{
  XmXftFontRec *f1 = (XmXftFontRec *)k1, *f2 = (XmXftFontRec *)k2;
  return (f1->size == f2->size && f1->pixel_size == f2->pixel_size && f1->slant == f2->slant &&
          f1->weight == f2->weight && f1->spacing == f2->spacing &&
          SameString(f1->name, f2->name) && SameString(f1->foundry, f2->foundry) &&
          SameString(f1->encoding, f2->encoding) && SameString(f1->style, f2->style));
}

static unsigned int HashString(unsigned int h, char *s)
{
  if (s != NULL)
    while (*s)
      h = h * 31 + (unsigned char)*s++;
  return h * 31;
}

static XmHashValue HashXftFont(XmHashKey k)
{
  XmXftFontRec *f = (XmXftFontRec *)k;
  unsigned int h = 0;
  h = HashString(h, f->name);
  h = HashString(h, f->foundry);
  h = HashString(h, f->encoding);
  h = HashString(h, f->style);
  h = h * 31 + (unsigned int)f->size;
  h = h * 31 + (unsigned int)f->pixel_size;
  h = h * 31 + (unsigned int)f->slant;
  h = h * 31 + (unsigned int)f->weight;
  h = h * 31 + (unsigned int)f->spacing;
  return (XmHashValue)(h & 0x7FFFFFFF);
}

/* Close a font of a display that is being closed; the connection is
 * still open, and Xft releases the font's server resources. */
static Boolean FreeXftFont(XmHashKey k, XtPointer value, XtPointer data)
{
  XmXftFontRec *f = (XmXftFontRec *)value;
  if (f->font != NULL)
    XftFontClose((Display *)data, f->font);
  XtFree(f->name);
  XtFree(f->foundry);
  XtFree(f->encoding);
  XtFree(f->style);
  XtFree((char *)f);
  return False;
}

static int XftDisplayClose(Display *display, XExtCodes *codes)
{
  XmXftDisplayRec **prev, *rec;
  _XmProcessLock();
  for (prev = &_XmXftDisplays; (rec = *prev) != NULL; prev = &rec->next)
    if (rec->display == display && rec->extension == codes->extension) {
      *prev = rec->next;
      /* The XftDraws are not destroyed: the server has already freed the
       * Pictures of the windows that went away, and Xft is closing too. */
      _XmFreeHashTable(rec->draws);
      _XmMapHashTable(rec->colors, FreeXftColor, NULL);
      _XmFreeHashTable(rec->colors);
      _XmMapHashTable(rec->fonts, FreeXftFont, (XtPointer)display);
      _XmFreeHashTable(rec->fonts);
      XtFree((char *)rec);
      break;
    }
  _XmProcessUnlock();
  return 0;
}

/* Call with the process lock held. */
static XmXftDisplayRec *FindXftDisplay(Display *display, Boolean create)
{
  XmXftDisplayRec **prev, *rec;
  XExtCodes *codes;
  for (prev = &_XmXftDisplays; (rec = *prev) != NULL; prev = &rec->next)
    if (rec->display == display) {
      if (prev != &_XmXftDisplays) { /* Keep the last display used first. */
        *prev = rec->next;
        rec->next = _XmXftDisplays;
        _XmXftDisplays = rec;
      }
      return rec;
    }
  if (!create || (codes = XAddExtension(display)) == NULL)
    return NULL;
  XESetCloseDisplay(display, codes->extension, XftDisplayClose);
  rec = XtNew(XmXftDisplayRec);
  rec->display = display;
  rec->extension = codes->extension;
  rec->draws = _XmAllocHashTable(64, NULL, NULL);
  rec->colors = _XmAllocHashTable(64, CompareXftColor, HashXftColor);
  rec->fonts = _XmAllocHashTable(16, CompareXftFont, HashXftFont);
  rec->next = _XmXftDisplays;
  _XmXftDisplays = rec;
  return rec;
}

/*
 * Destroy callback of a widget whose window has an XftDraw: destroy the
 * XftDraw while the window still exists, so that the table does not keep
 * every window ever drawn into, nor hand a stale XftDraw to a new window
 * that gets the same XID.
 */
static void XftDrawWidgetDestroyed(Widget w, XtPointer client_data, XtPointer call_data)
{
  Window window = (Window)client_data;
  XmXftDisplayRec *rec;
  XftDraw *draw = NULL;
  _XmProcessLock();
  if ((rec = FindXftDisplay(XtDisplay(w), False)) != NULL &&
      (draw = (XftDraw *)_XmGetHashEntry(rec->draws, (XmHashKey)window)) != NULL)
    (void)_XmRemoveHashEntry(rec->draws, (XmHashKey)window);
  _XmProcessUnlock();
  /* If the widget was unrealized meanwhile, the server has already freed
   * the window and its Picture: only forget the XftDraw then. */
  if (draw != NULL && XtWindow(w) == window)
    XftDrawDestroy(draw);
}

/* Grow a table once it holds more entries than buckets. */
static void AddHashEntry(XmHashTable table, XmHashKey key, XtPointer value)
{
  _XmAddHashEntry(table, key, value);
  if (_XmHashTableCount(table) > _XmHashTableSize(table))
    _XmResizeHashTable(table, 2 * _XmHashTableSize(table));
}

/*
 * The font opened earlier on display for the font resources of rend,
 * which may be NULL if it failed to open; False if there is none.  This
 * saves building and matching a pattern for every rendition.
 */
static Boolean LookupXftFont(Display *display, XmRendition rend, XftFont **font)
{
  XmXftDisplayRec *rec;
  XmXftFontRec key, *found = NULL;
  XftFontDesc(rend, &key);
  _XmProcessLock();
  if ((rec = FindXftDisplay(display, False)) != NULL)
    found = (XmXftFontRec *)_XmGetHashEntry(rec->fonts, (XmHashKey)&key);
  if (found != NULL)
    *font = found->font;
  _XmProcessUnlock();
  return (found != NULL);
}

/* Remember the font just opened for rend, until display is closed. */
static void CacheXftFont(Display *display, XmRendition rend)
{
  XmXftDisplayRec *rec;
  XmXftFontRec key, *entry;
  XftFontDesc(rend, &key);
  _XmProcessLock();
  if ((rec = FindXftDisplay(display, True)) != NULL &&
      _XmGetHashEntry(rec->fonts, (XmHashKey)&key) == NULL) {
    entry = XtNew(XmXftFontRec);
    *entry = key;
    entry->name = XtNewString(key.name);
    entry->foundry = XtNewString(key.foundry);
    entry->encoding = XtNewString(key.encoding);
    entry->style = XtNewString(key.style);
    if (_XmRendXftFont(rend) != NULL)
      entry->font = XftFontCopy(display, _XmRendXftFont(rend));
    AddHashEntry(rec->fonts, (XmHashKey)entry, (XtPointer)entry);
  }
  _XmProcessUnlock();
}

/* The colormap of the widget that owns window, else the default one. */
static Colormap WindowColormap(Display *display, Window window)
{
  Widget w = XtWindowToWidget(display, window);
  if (w != NULL && w->core.colormap != None)
    return w->core.colormap;
  return DefaultColormap(display, DefaultScreen(display));
}

/*
 * Whether the colours of colormap cannot change: it is the default
 * colormap of a screen whose default visual is StaticGray, StaticColor
 * or TrueColor.  The visual of another colormap is not known here.
 */
static Boolean ColormapIsStatic(Display *display, Colormap colormap)
{
  int i, c;
  for (i = 0; i < ScreenCount(display); i++)
    if (colormap == DefaultColormap(display, i)) {
      c = DefaultVisual(display, i)->class;
      return (c == StaticGray || c == StaticColor || c == TrueColor);
    }
  return False;
}

/* The XftColor for pixel in colormap, asking the server once if cache. */
static XftColor GetXftColor(Display *display, Colormap colormap, Pixel pixel, Boolean cache)
{
  XmXftDisplayRec *rec;
  XmXftColorRec key, *entry;
  XColor xcol;
  key.colormap = colormap;
  key.color.pixel = pixel;
  key.color.color.red = key.color.color.green = key.color.color.blue = 0;
  key.color.color.alpha = 0xFFFF;
  if (display == NULL)
    return key.color;
  if (!cache) {
    memset(&xcol, 0, sizeof(xcol));
    xcol.pixel = pixel;
    XQueryColor(display, colormap, &xcol);
    key.color.color.red = xcol.red;
    key.color.color.green = xcol.green;
    key.color.color.blue = xcol.blue;
    return key.color;
  }
  _XmProcessLock();
  rec = FindXftDisplay(display, True);
  entry = rec ? (XmXftColorRec *)_XmGetHashEntry(rec->colors, (XmHashKey)&key) : NULL;
  if (entry != NULL) {
    key.color = entry->color;
    _XmProcessUnlock();
    return key.color;
  }
  _XmProcessUnlock();
  memset(&xcol, 0, sizeof(xcol));
  xcol.pixel = pixel;
  XQueryColor(display, colormap, &xcol);
  key.color.color.red = xcol.red;
  key.color.color.green = xcol.green;
  key.color.color.blue = xcol.blue;
  _XmProcessLock();
  if ((rec = FindXftDisplay(display, True)) != NULL &&
      _XmGetHashEntry(rec->colors, (XmHashKey)&key) == NULL) {
    entry = XtNew(XmXftColorRec);
    *entry = key;
    AddHashEntry(rec->colors, (XmHashKey)entry, (XtPointer)entry);
  }
  _XmProcessUnlock();
  return key.color;
}

static XftColor GetCachedXftColor(Display *display, Pixel color)
{
  if (display == NULL)
    return GetXftColor(NULL, None, color, True);
  return GetXftColor(display, DefaultColormap(display, DefaultScreen(display)), color, True);
}

/* The XftColor to draw pixel with into window. */
static XftColor GetDrawXftColor(Display *display, Window window, Pixel pixel)
{
  Colormap colormap = WindowColormap(display, window);
  return GetXftColor(display, colormap, pixel, ColormapIsStatic(display, colormap));
}

static XErrorHandler oldErrorHandler __attribute__((unused));
static int xft_error __attribute__((unused));
/* Suppress unused variable warning */
static XErrorHandler oldErrorHandler_ptr __attribute__((unused)) = NULL;

static int __attribute__((unused)) _XmXftErrorHandler(Display *display, XErrorEvent *error)
{
  (void)fprintf(stderr,
                "Ignoring Xlib error: error code %d request code %d\n",
                error->error_code,
                error->request_code);
  xft_error = BadWindow;
  oldErrorHandler_ptr = _XmXftErrorHandler; /* suppress unused function warning */
  /* No exit! - but keep lint happy */
  return 0;
}

XftDraw *_XmXftDrawCreate(Display *display, Window window)
{
  XmXftDisplayRec *rec;
  XftDraw *draw = NULL;
  Widget widget;
  _XmProcessLock();
  if ((rec = FindXftDisplay(display, True)) != NULL)
    draw = (XftDraw *)_XmGetHashEntry(rec->draws, (XmHashKey)window);
  _XmProcessUnlock();
  if (draw != NULL)
    return draw;
  if (!(draw = XftDrawCreate(display,
                             window,
                             DefaultVisual(display, DefaultScreen(display)),
                             DefaultColormap(display, DefaultScreen(display)))))
    draw = XftDrawCreateBitmap(display, window);
  if (draw == NULL)
    return NULL;
  _XmProcessLock();
  if ((rec = FindXftDisplay(display, True)) != NULL)
    AddHashEntry(rec->draws, (XmHashKey)window, (XtPointer)draw);
  _XmProcessUnlock();
  if (rec != NULL && (widget = XtWindowToWidget(display, window)) != NULL)
    XtAddCallback(widget, XtNdestroyCallback, XftDrawWidgetDestroyed, (XtPointer)window);
  return draw;
}

void _XmXftDrawDestroy(Display *display, Window window, XftDraw *draw)
{
  XmXftDisplayRec *rec;
  XtPointer found = NULL;
  _XmProcessLock();
  if ((rec = FindXftDisplay(display, True)) != NULL &&
      (found = _XmGetHashEntry(rec->draws, (XmHashKey)window)) != NULL)
    (void)_XmRemoveHashEntry(rec->draws, (XmHashKey)window);
  _XmProcessUnlock();
  if (found != NULL) {
    XftDrawDestroy(draw);
    return;
  }
  XmeWarning(NULL, "_XmXftDrawDestroy() this should not happen\n");
}

void _XmXftDrawString2(Display *display,
                       Window window,
                       GC gc,
                       XftFont *font,
                       int bpc,
                       Position x,
                       Position y,
                       char *s,
                       int len)
{
  XftDraw *draw = _XmXftDrawCreate(display, window);
  XGCValues gc_val;
  XftColor xftcol;
  /* XGetGCValues reads Xlib's copy of the GC; it is not a round trip. */
  XGetGCValues(display, gc, GCForeground, &gc_val);
  xftcol = GetDrawXftColor(display, window, gc_val.foreground);
  switch (bpc) {
    case 1:
      XftDrawStringUtf8(draw, &xftcol, font, x, y, (XftChar8 *)s, len);
      break;
    case 2:
      XftDrawString16(draw, &xftcol, font, x, y, (XftChar16 *)s, len);
      break;
    case 4:
      XftDrawString32(draw, &xftcol, font, x, y, (XftChar32 *)s, len);
      break;
    default:
      XmeWarning(NULL, "_XmXftDrawString(unsupported bpc)\n");
  }
}

void _XmXftDrawString(Display *display,
                      Window window,
                      XmRendition rend,
                      int bpc,
                      Position x,
                      Position y,
                      char *s,
                      int len,
                      Boolean image)
{
  XftDraw *draw = _XmXftDrawCreate(display, window);
  XftColor fg_color = _XmRendXftFG(rend);
  if (image) {
    XftColor bg_color = _XmRendXftBG(rend);
    XGlyphInfo ext;
    ext.xOff = 0;
    switch (bpc) {
      case 1:
        XftTextExtentsUtf8(display, _XmRendXftFont(rend), (FcChar8 *)s, len, &ext);
        break;
      case 2:
        XftTextExtents16(display, _XmRendXftFont(rend), (FcChar16 *)s, len, &ext);
        break;
      case 4:
        XftTextExtents32(display, _XmRendXftFont(rend), (FcChar32 *)s, len, &ext);
        break;
    }
    if (_XmRendBG(rend) == XmUNSPECIFIED_PIXEL) {
      XGCValues gc_val;
      XGetGCValues(display, _XmRendGC(rend), GCBackground, &gc_val);
      bg_color = GetDrawXftColor(display, window, gc_val.background);
    }
    XftDrawRect(draw,
                &bg_color,
                x,
                y - _XmRendXftFont(rend)->ascent,
                ext.xOff,
                _XmRendXftFont(rend)->ascent + _XmRendXftFont(rend)->descent);
  }
  if (_XmRendFG(rend) == XmUNSPECIFIED_PIXEL) {
    XGCValues gc_val;
    XGetGCValues(display, _XmRendGC(rend), GCForeground, &gc_val);
    fg_color = GetDrawXftColor(display, window, gc_val.foreground);
  }
  switch (bpc) {
    case 1:
      XftDrawStringUtf8(draw, &fg_color, _XmRendXftFont(rend), x, y, (XftChar8 *)s, len);
      break;
    case 2:
      XftDrawString16(draw, &fg_color, _XmRendXftFont(rend), x, y, (XftChar16 *)s, len);
      break;
    case 4:
      XftDrawString32(draw, &fg_color, _XmRendXftFont(rend), x, y, (XftChar32 *)s, len);
      break;
    default:
      XmeWarning(NULL, "_XmXftDrawString(unsupported bpc)\n");
  }
}

void _XmXftSetClipRectangles(
    Display *display, Window window, Position x, Position y, XRectangle *rects, int n)
{
  XftDraw *d = _XmXftDrawCreate(display, window);
  XftDrawSetClipRectangles(d, x, y, rects, n);
}

XftColor _XmXftGetXftColor(Display *display, Pixel color)
{
  return GetCachedXftColor(display, color);
}

void _XmXftFontAverageWidth(Widget w, XtPointer f, int *width)
{
  XftFont *fp = (XftFont *)f;
  static char *s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  int l = 62; /* strlen(s) */
  XGlyphInfo ext;
  XftTextExtents8(XtDisplay(w), fp, (unsigned char *)s, l, &ext);
  if (width)
    *width = ext.width / l;
}
#endif
XmRenderTable XmRenderTableCvtFromProp(Widget w, char *prop, unsigned int len) /* unused */
{
  TokenRec reusetoken;
  XmRenderTable new_rt;
  XmRendition rendition;
  XmRendition *rarray;
  int rarray_count, rarray_max;
  /* Header items; columns beyond these are ignored. */
  char *items[20];
  int nitems;
  char *name;
  /* Each item adds at most 3 args and 1 string; extra columns that */
  /* do not fit are rejected. */
  Arg args[20];
  char *freelater[5];
  XmTabList tablist;
  int scanpointer, j, count, freecount, i;
  Token token;
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  new_rt = NULL;
  scanpointer = 0;
  rarray_max = 10;
  rarray_count = 0;
  rarray = (XmRendition *)_XmMallocArray(rarray_max, sizeof(XmRendition));
  name = "";
  tablist = NULL;
  count = 0;
  freecount = 0;
  reusetoken.type = T_EOF;
  reusetoken.string = NULL;
  token = &reusetoken;
  /* Read the list of items */
  for (nitems = 0; nitems < (int)XtNumber(items);) {
    token = ReadToken(prop, &scanpointer, &reusetoken);
    if (token->type == T_NL || token->type == T_EOF)
      break;
    if (token->type == T_STR) {
      items[nitems++] = token->string;
      token->string = NULL;
    }
  }
  /* Skip any further items */
  while (token->type != T_NL && token->type != T_EOF)
    token = ReadToken(prop, &scanpointer, &reusetoken);
  j = -1;
  while (True) {
    token = ReadToken(prop, &scanpointer, &reusetoken);
    /* We skip the separators */
    while (token->type == T_SEP)
      token = ReadToken(prop, &scanpointer, &reusetoken);
    if (token->type == T_EOF)
      goto finish;
    j++; /* Go to next item in items array */
    if (j >= nitems) {
      /* End of line processing.  Scan for NewLine */
      while (token->type != T_NL && token->type != T_EOF)
        token = ReadToken(prop, &scanpointer, &reusetoken);
      /* Store rendition */
      rendition = XmRenditionCreate(w, name, args, count);
      name = "";
      count = 0;
      /* Reset index into namelist */
      j = -1;
      /* Free temp strings returned by ReadToken */
      for (i = 0; i < freecount; i++)
        XtFree(freelater[i]);
      freecount = 0;
      /* The rendition has its own copy of the tab list */
      if (tablist != NULL) {
        XmTabListFree(tablist);
        tablist = NULL;
      }
      /* Record rendition in array */
      if (rarray_count >= rarray_max) {
        /* Extend array if necessary */
        rarray_max += 10;
        rarray = (XmRendition *)_XmReallocArray((char *)rarray, rarray_max, sizeof(XmRendition));
      }
      rarray[rarray_count] = rendition;
      rarray_count++;
      if (token->type == T_EOF)
        goto finish;
    }
    else if ((count + 3 > (int)XtNumber(args)) || (freecount >= (int)XtNumber(freelater))) {
      /* Too many columns for one rendition */
      goto finish;
    }
    else if (strcmp(items[j], XmNtag) == 0) {
      /* Next item should be a string with the name of the new
         rendition to create */
      if (token->type == T_STR) {
        name = token->string;
        freelater[freecount] = token->string;
        freecount++;
        token->string = NULL;
      }
      else {
        goto finish;
      }
    }
    else if (strcmp(items[j], XmNfont) == 0) {
      /* If the next item is a number then we have a font
         id,  otherwise we are reading in a fontset */
      if (token->type != T_INT)
        goto finish;
      if (token->integer != -1) { /* AS IS */
        XtSetArg(args[count], XmNfontType, token->integer);
        count++;
        token = ReadToken(prop, &scanpointer, &reusetoken);
        if (token->type != T_STR)
          goto finish;
        XtSetArg(args[count], XmNfontName, token->string);
        count++;
        freelater[freecount] = token->string;
        freecount++;
        token->string = NULL;
        token = ReadToken(prop, &scanpointer, &reusetoken);
        if (token->type != T_INT)
          goto finish;
        XtSetArg(args[count], XmNloadModel, token->integer);
        count++;
      }
    }
    else if (strcmp(items[j], XmNtabList) == 0) {
      /* This starts with an OPEN then a number of
         FLOAT INT INT INT then CLOSE and SEP */
      if (token->type == T_INT) { /* Should be AS IS */
        if (token->integer != -1)
          goto finish;
      }
      else if (token->type == T_OPEN && tablist == NULL) {
        float value;
        int units, align;
        XmOffsetModel model;
        XmTab tabs[1];
        token = ReadToken(prop, &scanpointer, &reusetoken);
        while (token->type != T_CLOSE) {
          if (token->type != T_FLOAT && token->type != T_INT)
            goto finish;
          if (token->type == T_FLOAT)
            value = token->real;
          else
            value = (float)token->integer;
          token = ReadToken(prop, &scanpointer, &reusetoken);
          if (token->type != T_INT)
            goto finish;
          units = token->integer;
          token = ReadToken(prop, &scanpointer, &reusetoken);
          if (token->type != T_INT)
            goto finish;
          align = token->integer;
          token = ReadToken(prop, &scanpointer, &reusetoken);
          if (token->type != T_INT)
            goto finish;
          model = (XmOffsetModel)token->integer;
          tabs[0] = XmTabCreate(value, units, model, align, NULL);
          tablist = XmTabListInsertTabs(tablist, tabs, 1, 1000);
          XtFree((char *)tabs[0]);
          /* Go to next separator to skip unknown future values */
          while (token->type != T_SEP) {
            if (token->type == T_NL || token->type == T_EOF)
              goto finish;
            token = ReadToken(prop, &scanpointer, &reusetoken);
          }
          token = ReadToken(prop, &scanpointer, &reusetoken);
        }
        XtSetArg(args[count], XmNtabList, tablist);
        count++;
      }
      else
        goto finish;
    }
    else if (strcmp(items[j], XmNbackground) == 0) {
      if (token->type != T_INT)
        goto finish;
      if (token->integer != -1) {
        XtSetArg(args[count], XmNrenditionBackground, token->integer);
        count++;
      }
    }
    else if (strcmp(items[j], XmNforeground) == 0) {
      if (token->type != T_INT)
        goto finish;
      if (token->integer != -1) {
        XtSetArg(args[count], XmNrenditionForeground, token->integer);
        count++;
      }
    }
    else if (strcmp(items[j], XmNunderlineType) == 0) {
      if (token->type != T_INT)
        goto finish;
      if (token->integer != -1) {
        XtSetArg(args[count], XmNunderlineType, token->integer);
        count++;
      }
    }
    else if (strcmp(items[j], XmNstrikethruType) == 0) {
      if (token->type != T_INT)
        goto finish;
      if (token->integer != -1) {
        XtSetArg(args[count], XmNstrikethruType, token->integer);
        count++;
      }
    }
  }
finish:
  /* Free what a malformed or truncated last line left behind */
  for (i = 0; i < freecount; i++)
    XtFree(freelater[i]);
  if (tablist != NULL)
    XmTabListFree(tablist);
  if (reusetoken.type == T_STR)
    XtFree(reusetoken.string);
  for (i = 0; i < nitems; i++)
    XtFree(items[i]);
  new_rt = XmRenderTableAddRenditions(new_rt, rarray, rarray_count, XmMERGE_REPLACE);
  for (i = 0; i < rarray_count; i++)
    XmRenditionFree(rarray[i]);
  XtFree((char *)rarray);
  _XmAppUnlock(app);
  return (new_rt);
}

void XmRenderTableGetDefaultFontExtents(XmRenderTable rendertable,
                                        int *height,
                                        int *ascent,
                                        int *descent)
{
  XmStringTag tag = XmFONTLIST_DEFAULT_TAG;
  XmRendition rend;
  Boolean success;
  short indx;
  int h, a, d;
  XtAppContext app = NULL;
  if (rendertable && _XmRTDisplay(rendertable))
    app = XtDisplayToApplicationContext(_XmRTDisplay(rendertable));
  if (app)
    _XmAppLock(app);
  else
    _XmProcessLock();
  a = d = h = 0;
  /* Get default rendition */
  success = _XmRenderTableFindFallback(rendertable, tag, FALSE, &indx, &rend);
  /* For backward compatibility we must try to return something for */
  /* any non-null charset, not just XmFONTLIST_DEFAULT_TAG. */
  if (rendertable && tag && !success)
    success = _XmRenderTableFindFirstFont(rendertable, &indx, &rend);
  if (success) {
    /* Find font height */
    switch (_XmRendFontType(rend)) {
      case XmFONT_IS_FONT:
        if (_XmRendFont(rend)) {
          a = ((XFontStruct *)_XmRendFont(rend))->ascent;
          d = ((XFontStruct *)_XmRendFont(rend))->descent;
          h = a + d;
        }
        break;
      case XmFONT_IS_FONTSET:
        if (_XmRendFont(rend)) {
          XFontStruct **font_struct_list;
          char **font_name_list;
          if (XFontsOfFontSet((XFontSet)_XmRendFont(rend), &font_struct_list, &font_name_list)) {
            a = font_struct_list[0]->ascent;
            d = font_struct_list[0]->descent;
            h = a + d;
          }
        }
        break;
#if USE_XFT
      case XmFONT_IS_XFT:
        if (_XmRendXftFont(rend)) {
          a = _XmRendXftFont(rend)->ascent;
          d = _XmRendXftFont(rend)->descent;
          h = a + d;
        }
        break;
#endif
    }
  }
  if (app)
    _XmAppUnlock(app);
  else
    _XmProcessUnlock();
  if (ascent)
    *ascent = a;
  if (descent)
    *descent = d;
  if (height)
    *height = h;
}
