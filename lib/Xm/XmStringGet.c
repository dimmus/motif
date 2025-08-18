/* $XConsortium: XmStringGet.c /main/6 1995/09/19 23:13:21 cde-sun $ */
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
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmStringI.h"
#include "XmI.h"

XmStringComponentType
XmStringGetNextComponent(
        XmStringContext context,
        char **text,
        XmStringTag *str_tag,
        XmStringDirection *direction,
        XmStringComponentType *unknown_tag,
        unsigned short *unknown_length,
        unsigned char **unknown_value )
{
  XmStringComponentType type;
  unsigned int  	len;
  XtPointer		val;

  _XmProcessLock();
  type = XmeStringGetComponent((_XmStringContext) context, True, True, &len, &val);

  /* Case on return type */
  /* Set appropriate return value and return. */
  switch (type)
    {
    case XmSTRING_COMPONENT_DIRECTION:
      *direction = *(XmStringDirection *)val;
      XtFree((char *)val);
      break;
    case XmSTRING_COMPONENT_TAG:
      *str_tag = (XmStringTag)val;
      break;
    case XmSTRING_COMPONENT_TEXT:
    case XmSTRING_COMPONENT_LOCALE_TEXT:
      *text = (char *)val;
      break;
    case XmSTRING_COMPONENT_SEPARATOR:
    case XmSTRING_COMPONENT_END:
      break;
    default:
      *unknown_tag = type;
      *unknown_length = len;
      *unknown_value = (unsigned char *)val;
      type = XmSTRING_COMPONENT_UNKNOWN;
    }
  _XmProcessUnlock();
  return(type);
}

XmStringComponentType
XmStringPeekNextComponent(XmStringContext context)
{
  unsigned int len;
  XtPointer    val;

  return XmeStringGetComponent((_XmStringContext) context, False, False, &len, &val);
}

/*
 * fetch the first text 'segment' of the external TCS that matches the given
 * char set.
 */
Boolean
XmStringGetLtoR(
  XmString string,
  XmStringTag tag,
  char **text )
{
  XmStringContext context;
  char * t;
  XmStringTag c, curtag = NULL;
  XmStringDirection d;
  Boolean s, is_local = FALSE, done = FALSE, is_default = FALSE;

  _XmProcessLock();
  if (!string) {
	_XmProcessUnlock();
	return(FALSE);
  }
  if (!tag) {
	_XmProcessUnlock();
	return (FALSE);
  }

  if ((tag == XmFONTLIST_DEFAULT_TAG) ||
      (strcmp(tag, XmFONTLIST_DEFAULT_TAG) == 0))
    is_local = TRUE;

  *text = NULL;				  /* pre-condition result */

  if (!is_local)
    {
      if ((strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
	{
	  curtag = _XmStringGetCurrentCharset();
	  is_default = TRUE;
	}
      else curtag = tag;
    }

  XmStringInitContext (&context, string);

  while ( ! done)
    {
      if (XmStringGetNextSegment (context, &t, &c, &d, &s))
	{
	  if (c && ((d == XmSTRING_DIRECTION_L_TO_R) ||
		    (d == XmSTRING_DIRECTION_UNSET)) &&
	      (((is_local || is_default) &&
		((c == XmFONTLIST_DEFAULT_TAG) ||
		 (strcmp(c, XmFONTLIST_DEFAULT_TAG) == 0) ||
		 (strcmp(c, _XmStringGetCurrentCharset()) == 0))) ||
	       (curtag && (strcmp (c, curtag) == 0))))
	    {
	      *text = t;		  /* OK, pass text to caller */
	      done = TRUE;
	    }
	  else
	    XtFree (t);			  /* not this text */

	  if (c)
	    XtFree (c);			  /* always dump charset */
	}
      else
	done = TRUE;
    }

  XmStringFreeContext (context);
  _XmProcessUnlock();
  return (*text != NULL);
}
