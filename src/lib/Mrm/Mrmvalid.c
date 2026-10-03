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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *  ABSTRACT:
 *
 *	This module checks widget records and literals read from a UID
 *	file before they are used. The routines which create widgets and
 *	convert values follow the offsets and counts in these resources
 *	without checking them, so a resource must be validated here as soon
 *	as it has been read into a resource context: every offset, count
 *	and string it contains must lie within the resource.
 *
 *	The checks read each field the way the code that later uses it
 *	does. Widget records are swapped to native byte order as they are
 *	read, except for the values located by their arguments, which are
 *	used in file byte order. Literals are swapped lazily, field by field,
 *	as they are converted, so the fields which that code swaps are read
 *	swapped here when the literal comes from a byte swapped file.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>
#include "MrmMsgI.h"


/*
 *  TABLE OF CONTENTS
 *
 *	Urm__ValidWidgetRecord		- Check a widget record
 *	Urm__ValidLiteral		- Check a literal
 *
 */


/*
 * A part of the resource which has been checked. The code which uses a
 * resource rewrites some structures in place (offsets become pointers,
 * fields are swapped, pixels are stored in color tables, icons are mapped
 * in place). Such a structure is only safe if nothing else that was
 * checked lies in the same bytes, since that would change after the check.
 */
typedef struct {
  size_t	start ;		/* offset of the first byte */
  size_t	end ;		/* offset past the last byte */
  Boolean	rewritten ;	/* rewritten in place when used */
} UrmValidRegion ;

/*
 * The resource being checked.
 */
typedef struct {
  char		*base ;		/* start of the resource; all offsets
				   are relative to it */
  size_t	size ;		/* number of bytes in the resource */
  Boolean	swapped ;	/* lazily swapped fields are still in
				   file byte order */
  Boolean	old ;		/* resource is in the Motif 1.1 format */
  Boolean	literal ;	/* resource is a literal (else a widget
				   record) */
  UrmValidRegion *regions ;	/* the parts checked so far */
  int		num_regions ;	/* # entries used in regions */
  int		max_regions ;	/* # entries allocated in regions */
} UrmValidBuffer ;

#define	_InBuf(vb,offs,len)	_UrmInBuffer(offs,len,(vb)->size)
#define	_StrAt(vb,offs)		ValidString(vb,offs)
#define	_At(vb,type,offs)	((type) ((vb)->base + (offs)))

static Boolean ValidValue (UrmValidBuffer *vb, MrmType reptype, size_t offs);


/*
 * Record the len bytes at offs, which have been checked to lie within the
 * resource, as used by the resource. rewritten tells whether they are
 * rewritten in place when the resource is used.
 */
static void
AddRegion (UrmValidBuffer	*vb,
	   size_t		offs,
	   size_t		len,
	   Boolean		rewritten)
{
  if ( len == 0 ) return ;
  if ( vb->num_regions == vb->max_regions )
    {
      vb->max_regions = (vb->max_regions == 0) ? 32 : 2 * vb->max_regions ;
      vb->regions = (UrmValidRegion *)
	XtRealloc ((char *) vb->regions,
		   vb->max_regions * sizeof (UrmValidRegion)) ;
    }
  vb->regions[vb->num_regions].start = offs ;
  vb->regions[vb->num_regions].end = offs + len ;
  vb->regions[vb->num_regions].rewritten = rewritten ;
  vb->num_regions++ ;
}


static int
CompareRegions (const void	*a,
		const void	*b)
{
  const UrmValidRegion	*ra = (const UrmValidRegion *) a ;
  const UrmValidRegion	*rb = (const UrmValidRegion *) b ;

  if ( ra->start != rb->start ) return (ra->start < rb->start) ? -1 : 1 ;
  if ( ra->end != rb->end ) return (ra->end < rb->end) ? -1 : 1 ;
  return 0 ;
}


/*
 * Check that no region which is rewritten in place shares a byte with
 * any other region (including another reference to the same structure).
 * Regions which are only read, such as strings, may be shared. Frees the
 * region list.
 */
static Boolean
RegionsDisjoint (UrmValidBuffer		*vb)
{
  size_t		end_all = 0 ;	/* furthest end of earlier regions */
  size_t		end_rw = 0 ;	/* same, of rewritten regions */
  UrmValidRegion	*rp ;		/* current region */
  Boolean		ok = TRUE ;
  int			ndx ;

  if ( vb->num_regions > 1 )
    qsort (vb->regions, vb->num_regions, sizeof (UrmValidRegion),
	   CompareRegions) ;

  /*
   * In start order, a region overlaps an earlier one iff it starts before
   * that region's end.
   */
  for ( ndx=0 ; ndx<vb->num_regions && ok ; ndx++ )
    {
      rp = &vb->regions[ndx] ;
      if ( rp->start < end_rw || (rp->rewritten && rp->start < end_all) )
	ok = FALSE ;
      if ( rp->end > end_all ) end_all = rp->end ;
      if ( rp->rewritten && rp->end > end_rw ) end_rw = rp->end ;
    }

  XtFree ((char *) vb->regions) ;
  vb->regions = NULL ;
  vb->num_regions = vb->max_regions = 0 ;
  return ok ;
}


/*
 * A NUL-terminated string at offs, which is only read.
 */
static Boolean
ValidString (UrmValidBuffer	*vb,
	     size_t		offs)
{
  if ( ! _UrmStringInBuffer (vb->base, offs, vb->size) ) return FALSE ;
  AddRegion (vb, offs, strlen (vb->base + offs) + 1, FALSE) ;
  return TRUE ;
}


/*
 * Fields which are swapped as a literal is converted.
 */
static MrmCount
SwappedCount (UrmValidBuffer	*vb,
	      MrmCount		count)
{
  if ( vb->swapped ) swapbytes (count) ;
  return count ;
}

static MrmOffset
SwappedOffset (UrmValidBuffer	*vb,
	       MrmOffset	offs)
{
  if ( vb->swapped ) swapbytes (offs) ;
  return offs ;
}


/*
 * Read the length of an ASN.1 item from the length field at stg[0], of
 * which avail bytes are present, and the size of that field. Only the
 * shortest encoding of each length is accepted, because that is the size
 * XmCvtByteStreamToXmString assumes when it skips over items.
 */
static Boolean
CStringLength (unsigned char	*stg,
	       size_t		avail,
	       size_t		*len,
	       size_t		*lensize)
{
  if ( avail < 1 ) return FALSE ;
  if ( ! (stg[0] & 0x80) )
    {
      *len = stg[0] ;
      *lensize = 1 ;
      return TRUE ;
    }
  if ( avail < 3 ) return FALSE ;
  *len = (size_t) stg[1] << 8 | stg[2] ;
  *lensize = 3 ;
  return *len > 127 ;
}


/*
 * A compound string in ASN.1 byte stream format: a 3-byte header and a
 * length, followed by that many bytes of components, each a 1-byte tag,
 * a length and that many bytes of value.
 */
static Boolean
ValidCString (UrmValidBuffer	*vb,
	      size_t		offs)
{
  unsigned char		*stg ;		/* the byte stream */
  size_t		avail ;		/* bytes left in the resource */
  size_t		len ;		/* length of an item */
  size_t		lensize ;	/* size of its length field */
  size_t		pos ;		/* offset of current component */
  size_t		end ;		/* end of the components */

  if ( ! _InBuf (vb, offs, 4) ) return FALSE ;
  stg = _At (vb, unsigned char *, offs) ;
  avail = vb->size - offs ;
  if ( ! CStringLength (stg + 3, avail - 3, &len, &lensize) ||
       len > avail - 3 - lensize )
    return FALSE ;

  pos = 3 + lensize ;
  end = pos + len ;
  while ( pos < end )
    {
      if ( ! CStringLength (stg + pos + 1, end - pos - 1, &len, &lensize) ||
	   len > end - pos - 1 - lensize )
	return FALSE ;
      pos += 1 + lensize + len ;
    }
  AddRegion (vb, offs, end, FALSE) ;
  return TRUE ;
}


/*
 * A resource descriptor. Index references are NUL-terminated strings.
 * Descriptors in a byte swapped icon or color table are swapped in place
 * when they are used.
 */
static Boolean
ValidResourceDesc (UrmValidBuffer	*vb,
		   size_t		offs,
		   Boolean		rewritten)
{
  RGMResourceDescPtr	resptr ;	/* the descriptor */
  size_t		len ;		/* bytes used */

  if ( ! _InBuf (vb, offs, sizeof (RGMResourceDesc)) ) return FALSE ;
  resptr = _At (vb, RGMResourceDescPtr, offs) ;
  len = sizeof (RGMResourceDesc) ;
  if ( resptr->type == URMrIndex )
    {
      if ( ! _UrmStringInBuffer (vb->base,
				 offs + XtOffsetOf (RGMResourceDesc, key.index),
				 vb->size) )
	return FALSE ;
      len = MAX (len, XtOffsetOf (RGMResourceDesc, key.index) +
		 strlen (resptr->key.index) + 1) ;
    }
  AddRegion (vb, offs, len, rewritten) ;
  return TRUE ;
}


/*
 * A color descriptor. Named colors hold their NUL-terminated name.
 */
static Boolean
ValidColorDesc (UrmValidBuffer	*vb,
		size_t		offs)
{
  RGMColorDescPtr	colorptr ;	/* the descriptor */

  if ( ! _InBuf (vb, offs, XtOffsetOf (RGMColorDesc, desc)) ) return FALSE ;
  colorptr = _At (vb, RGMColorDescPtr, offs) ;
  AddRegion (vb, offs, XtOffsetOf (RGMColorDesc, desc), FALSE) ;
  switch ( colorptr->desc_type )
    {
    case URMColorDescTypeName:
      return _StrAt (vb, offs + XtOffsetOf (RGMColorDesc, desc.name)) ;
    case URMColorDescTypeRGB:
      if ( ! _InBuf (vb, offs + XtOffsetOf (RGMColorDesc, desc.rgb),
		     sizeof (RGBColor)) )
	return FALSE ;
      AddRegion (vb, offs + XtOffsetOf (RGMColorDesc, desc.rgb),
		 sizeof (RGBColor), FALSE) ;
      return TRUE ;
    default:
      return TRUE ;
    }
}


/*
 * A color table. Its count includes the foreground and background
 * entries, which are always present. Like Urm__CW_LoadIconImage, the
 * byte order of the table is told by its validation code.
 */
static Boolean
ValidColorTable (UrmValidBuffer		*vb,
		 size_t			offs)
{
  RGMColorTablePtr	ctable ;	/* the table */
  RGMColorTableEntryPtr	citem ;		/* current entry */
  Boolean		swapped ;	/* table is byte swapped */
  MrmCount		count ;		/* number of entries */
  MrmType		type ;		/* entry type */
  MrmOffset		coffs ;		/* entry offset */
  int			ndx ;		/* loop index */

  if ( ! _InBuf (vb, offs, XtOffsetOf (RGMColorTable, item)) ) return FALSE ;
  ctable = _At (vb, RGMColorTablePtr, offs) ;
  if ( ctable->validation == URMColorTableValid )
    swapped = FALSE ;
  else if ( Urm__SwapValidation (ctable->validation) == URMColorTableValid )
    swapped = TRUE ;
  else
    return FALSE ;

  count = ctable->count ;
  if ( swapped ) swapbytes (count) ;
  if ( count < URMColorTableUserMin ||
       ! _InBuf (vb, offs + XtOffsetOf (RGMColorTable, item),
		 (size_t) count * sizeof (RGMColorTableEntry)) )
    return FALSE ;
  AddRegion (vb, offs, XtOffsetOf (RGMColorTable, item) +
	     (size_t) count * sizeof (RGMColorTableEntry), TRUE) ;

  for ( ndx=URMColorTableUserMin ; ndx<count ; ndx++ )
    {
      citem = &ctable->item[ndx] ;
      type = citem->type ;
      coffs = citem->color_item.coffs ;
      if ( swapped )
	{
	  swapbytes (type) ;
	  swapbytes (coffs) ;
	}
      switch ( type )
	{
	case MrmRtypeColor:
	  if ( ! ValidColorDesc (vb, coffs) ) return FALSE ;
	  break ;
	case MrmRtypeResource:
	  if ( ! ValidResourceDesc (vb, coffs, swapped) ) return FALSE ;
	  break ;
	default:
	  return FALSE ;
	}
    }
  return TRUE ;
}


/*
 * An icon image: its pixel data, of height rows of width pixels padded to
 * a byte, and its color table, in the icon or a separate literal. Like
 * Urm__CW_LoadIconImage, the byte order is told by the validation code.
 */
static Boolean
ValidIconImage (UrmValidBuffer	*vb,
		size_t		offs)
{
  RGMIconImagePtr	iconptr ;	/* the icon */
  Boolean		swapped ;	/* icon is byte swapped */
  MrmCount		width ;		/* width in pixels */
  MrmCount		height ;	/* height in pixels */
  MrmType		ct_type ;	/* color table type */
  MrmOffset		ctoff ;		/* color table offset */
  MrmOffset		pdoff ;		/* pixel data offset */
  size_t		bits ;		/* bits per pixel */

  if ( ! _InBuf (vb, offs, sizeof (RGMIconImage)) ) return FALSE ;
  iconptr = _At (vb, RGMIconImagePtr, offs) ;
  if ( iconptr->validation == URMIconImageValid )
    swapped = FALSE ;
  else if ( Urm__SwapValidation (iconptr->validation) == URMIconImageValid )
    swapped = TRUE ;
  else
    return FALSE ;

  width = iconptr->width ;
  height = iconptr->height ;
  ct_type = iconptr->ct_type ;
  ctoff = iconptr->color_table.ctoff ;
  pdoff = iconptr->pixel_data.pdoff ;
  if ( swapped )
    {
      swapbytes (width) ;
      swapbytes (height) ;
      swapbytes (ct_type) ;
      swapbytes (ctoff) ;
      swapbytes (pdoff) ;
    }

  switch ( iconptr->pixel_size )
    {
    case URMPixelSize1Bit: bits = 1 ; break ;
    case URMPixelSize2Bit: bits = 2 ; break ;
    case URMPixelSize4Bit: bits = 4 ; break ;
    case URMPixelSize8Bit: bits = 8 ; break ;
    default: return FALSE ;
    }
  if ( width <= 0 || height <= 0 ||
       ! _InBuf (vb, pdoff, ((width * bits + 7) / 8) * (size_t) height) )
    return FALSE ;

  /*
   * The icon is swapped and fixed up in place, and its pixels may be
   * mapped in place.
   */
  AddRegion (vb, offs, sizeof (RGMIconImage), TRUE) ;
  AddRegion (vb, pdoff, ((width * bits + 7) / 8) * (size_t) height, TRUE) ;

  switch ( ct_type )
    {
    case MrmRtypeColorTable:
      return ValidColorTable (vb, ctoff) ;
    case MrmRtypeResource:
      return ValidResourceDesc (vb, ctoff, swapped) ;
    default:
      return FALSE ;
    }
}


/*
 * A vector of strings or compound strings. Literal vectors are copied
 * from their first item to the end of the literal, so the strings must
 * follow the items.
 */
static Boolean
ValidTextVector (UrmValidBuffer		*vb,
		 MrmType		reptype,
		 size_t			offs)
{
  RGMTextVectorPtr	vecptr ;	/* the vector */
  MrmCount		count ;		/* number of items */
  MrmOffset		item_offs ;	/* offset of an item */
  int			ndx ;		/* loop index */

  if ( ! _InBuf (vb, offs, XtOffsetOf (RGMTextVector, item)) ) return FALSE ;
  vecptr = _At (vb, RGMTextVectorPtr, offs) ;
  count = SwappedCount (vb, vecptr->count) ;
  if ( count < 0 ||
       ! _InBuf (vb, offs + XtOffsetOf (RGMTextVector, item),
		 (size_t) count * sizeof (RGMTextEntry)) )
    return FALSE ;
  AddRegion (vb, offs, XtOffsetOf (RGMTextVector, item) +
	     (size_t) count * sizeof (RGMTextEntry), TRUE) ;

  for ( ndx=0 ; ndx<count ; ndx++ )
    {
      item_offs = SwappedOffset (vb, vecptr->item[ndx].text_item.offset) ;
      if ( vb->literal && item_offs < XtOffsetOf (RGMTextVector, item) )
	return FALSE ;
      if ( reptype == MrmRtypeChar8Vector ?
	   ! _StrAt (vb, item_offs) : ! ValidCString (vb, item_offs) )
	return FALSE ;
    }
  return TRUE ;
}


/*
 * A font list. Font lists in the 1.1 format are only converted as
 * literals, and their fields are never swapped.
 */
static Boolean
ValidFontList (UrmValidBuffer	*vb,
	       size_t		offs)
{
  RGMFontListPtr	fontlist ;	/* the list */
  OldRGMFontListPtr	oldlist ;	/* the list in 1.1 format */
  MrmCount		count ;		/* number of items */
  int			ndx ;		/* loop index */

  if ( vb->old )
    {
      if ( ! vb->literal || offs != 0 ||
	   ! _InBuf (vb, offs, XtOffsetOf (OldRGMFontList, item)) )
	return FALSE ;
      oldlist = _At (vb, OldRGMFontListPtr, offs) ;
      count = oldlist->count ;
      if ( count < 0 ||
	   ! _InBuf (vb, offs + XtOffsetOf (OldRGMFontList, item),
		     (size_t) count * sizeof (OldRGMFontItem)) )
	return FALSE ;
      AddRegion (vb, offs, XtOffsetOf (OldRGMFontList, item) +
		 (size_t) count * sizeof (OldRGMFontItem), FALSE) ;
      for ( ndx=0 ; ndx<count ; ndx++ )
	if ( ! _StrAt (vb, oldlist->item[ndx].cset.cs_offs) ||
	     ! _StrAt (vb, oldlist->item[ndx].font.font_offs) )
	  return FALSE ;
      return TRUE ;
    }

  if ( ! _InBuf (vb, offs, XtOffsetOf (RGMFontList, item)) ) return FALSE ;
  fontlist = _At (vb, RGMFontListPtr, offs) ;
  count = SwappedCount (vb, fontlist->count) ;
  if ( count < 0 ||
       ! _InBuf (vb, offs + XtOffsetOf (RGMFontList, item),
		 (size_t) count * sizeof (RGMFontItem)) )
    return FALSE ;
  AddRegion (vb, offs, XtOffsetOf (RGMFontList, item) +
	     (size_t) count * sizeof (RGMFontItem), TRUE) ;
  for ( ndx=0 ; ndx<count ; ndx++ )
    if ( ! _StrAt (vb, SwappedOffset (vb, fontlist->item[ndx].cset.cs_offs)) ||
	 ! _StrAt (vb, SwappedOffset (vb, fontlist->item[ndx].font.font_offs)) )
      return FALSE ;
  return TRUE ;
}


/*
 * A callback list, of count items plus the terminating item which is
 * copied with them, each naming a routine and giving a tag value.
 */
static Boolean
ValidCallbackDesc (UrmValidBuffer	*vb,
		   size_t		offs)
{
  RGMCallbackDescPtr	cbptr = NULL ;	/* the list */
  OldRGMCallbackDescPtr	oldptr = NULL ;	/* the list in 1.1 format */
  MrmCount		count ;		/* number of items */
  MrmOffset		routine ;	/* routine name offset */
  MrmType		reptype ;	/* tag value type */
  RGMdatum		datum ;		/* tag value */
  int			ndx ;		/* loop index */

  if ( vb->old )
    {
      if ( ! _InBuf (vb, offs, XtOffsetOf (OldRGMCallbackDesc, item)) )
	return FALSE ;
      oldptr = _At (vb, OldRGMCallbackDescPtr, offs) ;
      count = oldptr->count ;
      if ( count < 0 ||
	   ! _InBuf (vb, offs + XtOffsetOf (OldRGMCallbackDesc, item),
		     ((size_t) count + 1) * sizeof (OldRGMCallbackItem)) )
	return FALSE ;
      /* translated into a new list before it is used */
      AddRegion (vb, offs, XtOffsetOf (OldRGMCallbackDesc, item) +
		 ((size_t) count + 1) * sizeof (OldRGMCallbackItem), FALSE) ;
    }
  else
    {
      if ( ! _InBuf (vb, offs, XtOffsetOf (RGMCallbackDesc, item)) )
	return FALSE ;
      cbptr = _At (vb, RGMCallbackDescPtr, offs) ;
      count = cbptr->count ;
      if ( count < 0 ||
	   ! _InBuf (vb, offs + XtOffsetOf (RGMCallbackDesc, item),
		     ((size_t) count + 1) * sizeof (RGMCallbackItem)) )
	return FALSE ;
      AddRegion (vb, offs, XtOffsetOf (RGMCallbackDesc, item) +
		 ((size_t) count + 1) * sizeof (RGMCallbackItem), TRUE) ;
    }

  for ( ndx=0 ; ndx<count ; ndx++ )
    {
      if ( vb->old )
	{
	  routine = oldptr->item[ndx].cb_item.routine ;
	  reptype = oldptr->item[ndx].cb_item.rep_type ;
	  datum = oldptr->item[ndx].cb_item.datum ;
	}
      else
	{
	  routine = cbptr->item[ndx].cb_item.routine ;
	  reptype = cbptr->item[ndx].cb_item.rep_type ;
	  datum = cbptr->item[ndx].cb_item.datum ;
	}
      if ( ! _StrAt (vb, routine) ) return FALSE ;

      /*
       * A callback tag is any value except a nested callback list, which
       * is passed on as a pointer without being read.
       */
      switch ( reptype )
	{
	case MrmRtypeInteger:
	case MrmRtypeBoolean:
	case MrmRtypeSingleFloat:
	case MrmRtypeNull:
	  break ;
	case MrmRtypeCallback:
	  if ( datum.offset > vb->size ) return FALSE ;
	  break ;
	default:
	  if ( ! ValidValue (vb, reptype, datum.offset) ) return FALSE ;
	  break ;
	}
    }
  return TRUE ;
}


/*
 * A value of type reptype located at offset offs. Immediate values have
 * nothing to check.
 */
static Boolean
ValidValue (UrmValidBuffer	*vb,
	    MrmType		reptype,
	    size_t		offs)
{
  RGMIntegerVectorPtr	intvec ;	/* integer vector */
  RGMWCharEntryPtr	wcharentry ;	/* wide character string */
  RGMFontItemPtr	fontitem ;	/* font */
  MrmCount		count ;		/* number of items */

  switch ( reptype )
    {
    case MrmRtypeInteger:
    case MrmRtypeBoolean:
    case MrmRtypeSingleFloat:
    case MrmRtypeNull:
      return TRUE ;

    case MrmRtypeChar8:
    case MrmRtypeAddrName:
    case MrmRtypeTransTable:
    case MrmRtypeClassRecName:
    case MrmRtypeKeysym:
    case MrmRtypeXBitmapFile:
      return _StrAt (vb, offs) ;

    case MrmRtypeCString:
      return ValidCString (vb, offs) ;

    case MrmRtypeChar8Vector:
    case MrmRtypeCStringVector:
      return ValidTextVector (vb, reptype, offs) ;

    case MrmRtypeIntegerVector:
      if ( ! _InBuf (vb, offs, XtOffsetOf (RGMIntegerVector, item)) )
	return FALSE ;
      intvec = _At (vb, RGMIntegerVectorPtr, offs) ;
      count = intvec->count ;
      if ( count < 0 ||
	   ! _InBuf (vb, offs + XtOffsetOf (RGMIntegerVector, item),
		     (size_t) count * sizeof (int)) )
	return FALSE ;
      AddRegion (vb, offs, XtOffsetOf (RGMIntegerVector, item) +
		 (size_t) count * sizeof (int), FALSE) ;
      return TRUE ;

    case MrmRtypeWideCharacter:
      /*
       * A NUL-terminated multibyte string. The count is only used to size
       * the conversion; the UIL compiler stores the size of the whole
       * literal there.
       */
      if ( ! _InBuf (vb, offs, XtOffsetOf (RGMWCharEntry, wchar_item.bytes)) )
	return FALSE ;
      wcharentry = _At (vb, RGMWCharEntryPtr, offs) ;
      count = SwappedCount (vb, wcharentry->wchar_item.count) ;
      if ( count < 0 ||
	   ! _UrmStringInBuffer (vb->base,
				 offs + XtOffsetOf (RGMWCharEntry,
						    wchar_item.bytes),
				 vb->size) )
	return FALSE ;
      /* the converted string is stored over the entry */
      AddRegion (vb, offs,
		 MAX (sizeof (RGMWCharEntry),
		      XtOffsetOf (RGMWCharEntry, wchar_item.bytes) +
		      strlen (wcharentry->wchar_item.bytes) + 1), TRUE) ;
      return TRUE ;

    case MrmRtypeFont:
    case MrmRtypeFontSet:
      if ( ! _InBuf (vb, offs, sizeof (RGMFontItem)) ) return FALSE ;
      fontitem = _At (vb, RGMFontItemPtr, offs) ;
      AddRegion (vb, offs, sizeof (RGMFontItem), TRUE) ;
      return ( _StrAt (vb, SwappedOffset (vb, fontitem->cset.cs_offs)) &&
	       _StrAt (vb, SwappedOffset (vb, fontitem->font.font_offs)) ) ;

    case MrmRtypeFontList:
      return ValidFontList (vb, offs) ;

    case MrmRtypeFloat:
      if ( ! _InBuf (vb, offs, sizeof (double)) ) return FALSE ;
      AddRegion (vb, offs, sizeof (double), TRUE) ;
      return TRUE ;

    case MrmRtypeHorizontalInteger:
    case MrmRtypeVerticalInteger:
      if ( ! _InBuf (vb, offs, sizeof (RGMUnitsInteger)) ) return FALSE ;
      AddRegion (vb, offs, sizeof (RGMUnitsInteger), TRUE) ;
      return TRUE ;

    case MrmRtypeHorizontalFloat:
    case MrmRtypeVerticalFloat:
      if ( ! _InBuf (vb, offs, sizeof (RGMUnitsFloat)) ) return FALSE ;
      AddRegion (vb, offs, sizeof (RGMUnitsFloat), TRUE) ;
      return TRUE ;

    case MrmRtypeColor:
      return ValidColorDesc (vb, offs) ;

    case MrmRtypeColorTable:
      return ValidColorTable (vb, offs) ;

    case MrmRtypeIconImage:
      return ValidIconImage (vb, offs) ;

    case MrmRtypeResource:
      return ValidResourceDesc (vb, offs, FALSE) ;

    case MrmRtypeCallback:
      return ValidCallbackDesc (vb, offs) ;

    default:
      /*
       * Any other value is passed on as a pointer into the resource.
       */
      return offs <= vb->size ;
    }
}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Urm__ValidWidgetRecord checks the widget record in a resource
 *	context, after it has been swapped to native byte order: its name
 *	and class name, its argument list and the values it locates, its
 *	children list and its creation callbacks must all lie within the
 *	record, which must lie within the context.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		IDB file from which the record was read
 *	context_id	context holding the widget record
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS		the record is valid
 *	MrmBAD_WIDGET_REC	it is not
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__ValidWidgetRecord (IDBFile			file_id,
			URMResourceContextPtr	context_id)
{

  /*
   *  Local variables
   */
  UrmValidBuffer	vb ;		/* the record */
  RGMWidgetRecordPtr	widgetrec ;	/* the record */
  RGMArgListDescPtr	argdesc ;	/* argument list */
  RGMArgumentPtr	argptr ;	/* current argument */
  RGMChildrenDescPtr	childrendesc ;	/* children list */
  RGMChildDescPtr	childptr ;	/* current child */
  int			ndx ;		/* loop index */

  vb.base = UrmRCBuffer (context_id) ;
  vb.size = UrmRCSize (context_id) ;
  vb.swapped = FALSE ;
  vb.old = strcmp (file_id->db_version, URM1_1version) <= 0 ;
  vb.literal = FALSE ;
  vb.regions = NULL ;
  vb.num_regions = vb.max_regions = 0 ;

  if ( vb.base == NULL || vb.size < sizeof (RGMWidgetRecord) )
    goto bad_record ;
  widgetrec = (RGMWidgetRecordPtr) vb.base ;
  if ( widgetrec->size > vb.size )
    goto bad_record ;
  AddRegion (&vb, 0, sizeof (RGMWidgetRecord), FALSE) ;

  if ( ! _StrAt (&vb, widgetrec->name_offs) )
    goto bad_record ;
  if ( (widgetrec->type == UilMrmUnknownCode || widgetrec->class_offs != 0) &&
       ! _StrAt (&vb, widgetrec->class_offs) )
    goto bad_record ;

  if ( widgetrec->arglist_offs != 0 )
    {
      if ( ! _InBuf (&vb, widgetrec->arglist_offs,
		     XtOffsetOf (RGMArgListDesc, args)) )
	goto bad_record ;
      argdesc = _At (&vb, RGMArgListDescPtr, widgetrec->arglist_offs) ;
      if ( argdesc->count < 0 || argdesc->extra < 0 ||
	   ! _InBuf (&vb, widgetrec->arglist_offs +
		     XtOffsetOf (RGMArgListDesc, args),
		     (size_t) argdesc->count * sizeof (RGMArgument)) )
	goto bad_record ;
      AddRegion (&vb, widgetrec->arglist_offs,
		 XtOffsetOf (RGMArgListDesc, args) +
		 (size_t) argdesc->count * sizeof (RGMArgument), FALSE) ;
      for ( ndx=0 ; ndx<argdesc->count ; ndx++ )
	{
	  argptr = &argdesc->args[ndx] ;
	  if ( argptr->tag_code == UilMrmUnknownCode &&
	       ! _StrAt (&vb, argptr->stg_or_relcode.tag_offs) )
	    goto bad_record ;
	  if ( ! ValidValue (&vb, argptr->arg_val.rep_type,
			     argptr->arg_val.datum.offset) )
	    goto bad_record ;
	}
    }

  if ( widgetrec->children_offs != 0 )
    {
      if ( ! _InBuf (&vb, widgetrec->children_offs,
		     XtOffsetOf (RGMChildrenDesc, child)) )
	goto bad_record ;
      childrendesc = _At (&vb, RGMChildrenDescPtr, widgetrec->children_offs) ;
      if ( childrendesc->count < 0 ||
	   ! _InBuf (&vb, widgetrec->children_offs +
		     XtOffsetOf (RGMChildrenDesc, child),
		     (size_t) childrendesc->count * sizeof (RGMChildDesc)) )
	goto bad_record ;
      AddRegion (&vb, widgetrec->children_offs,
		 XtOffsetOf (RGMChildrenDesc, child) +
		 (size_t) childrendesc->count * sizeof (RGMChildDesc), FALSE) ;
      for ( ndx=0 ; ndx<childrendesc->count ; ndx++ )
	{
	  childptr = &childrendesc->child[ndx] ;
	  if ( childptr->type == URMrIndex &&
	       ( childptr->key.index_offs < 0 ||
		 ! _StrAt (&vb, childptr->key.index_offs) ) )
	    goto bad_record ;
	}
    }

  if ( widgetrec->creation_offs != 0 &&
       ! ValidCallbackDesc (&vb, widgetrec->creation_offs) )
    goto bad_record ;

  /*
   * Nothing may share the bytes of a structure that is rewritten while the
   * widget is created.
   */
  if ( RegionsDisjoint (&vb) )
    return MrmSUCCESS ;

 bad_record:
  XtFree ((char *) vb.regions) ;
  return Urm__UT_Error ("Urm__ValidWidgetRecord", _MrmMMsg_0026,
			NULL, context_id, MrmBAD_WIDGET_REC) ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Urm__ValidLiteral checks the literal in a resource context, which
 *	starts at the beginning of the context buffer and has the type of
 *	the context. Every offset, count and string in the literal must lie
 *	within it.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		IDB file from which the literal was read
 *	context_id	context holding the literal
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	the literal is valid
 *	MrmNOT_VALID	it is not
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__ValidLiteral (IDBFile			file_id,
		   URMResourceContextPtr	context_id)
{

  /*
   *  Local variables
   */
  UrmValidBuffer	vb ;		/* the literal */

  vb.base = UrmRCBuffer (context_id) ;
  vb.size = UrmRCSize (context_id) ;
  vb.swapped = UrmRCByteSwap (context_id) ;
  vb.old = strcmp (file_id->db_version, URM1_1version) <= 0 ;
  vb.literal = TRUE ;
  vb.regions = NULL ;
  vb.num_regions = vb.max_regions = 0 ;

  if ( vb.base != NULL && ValidValue (&vb, UrmRCType (context_id), 0) &&
       RegionsDisjoint (&vb) )
    return MrmSUCCESS ;

  XtFree ((char *) vb.regions) ;
  return Urm__UT_Error ("Urm__ValidLiteral", _MrmMMsg_0028,
			NULL, context_id, MrmNOT_VALID) ;

}
