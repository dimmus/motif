/* $XConsortium: MrmIswap.c /main/7 1996/11/13 13:58:35 drk $ */
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
#include <config.h>
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM): IDB Facility
 *	Byte Swap routines for IDB records
 *
 *  ABSTRACT:
 *
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#include <stdio.h>
#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>
#include <Mrm/IDB.h>
#include "MrmosI.h"
#include "MrmMsgI.h"


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__BM_SwapBytes performs byte swapping on the (currently 6)
 * 	record types in an IDB file
 *
 *  FORMAL PARAMETERS:
 *
 *	buffer		Record buffer to swap in place
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Idb__BM_SwapRecordBytes (IDBRecordBufferPtr		buffer)
{

  /*
   *  Local variables
   */
  Cardinal		ndx;	    /* loop index */
  IDBDummyRecordPtr	idb_record; /* pointer to the generic IDB record */
  IDBRecordHeaderPtr	idb_header; /* pointer to hdr w/type and record # */
  IDBHeaderRecordPtr	header_rec; /* pointer to record type IDBrtHeader */
  IDBHeaderHdrPtr	header_hdr; /* pointer to the header in the header */
  IDBIndexLeafRecordPtr	leaf_rec;   /* pointer to record type IDBrtIndexLeaf */
  IDBIndexNodeRecordPtr node_rec;   /* pointer to record type IDBrtIndexNode */
  IDBridMapRecordPtr	ridmap_rec; /* pointer to record type IDBrtRIDMap */
  IDBDataRecordPtr	data_rec;   /* pointer to record type IDBrtData */
  char			err_msg[300] ;

  if ( ! Idb__BM_Valid(buffer) )
    return Urm__UT_Error("Idb__BM_MarkActivity", _MrmMMsg_0002,
			 NULL, NULL, MrmNOT_VALID) ;

  /* load pointers to the record and record header */

  idb_record = (IDBDummyRecordPtr) buffer->IDB_record ;
  idb_header = (IDBRecordHeaderPtr)&idb_record->header ;


  /* swap the remaining record entries in IDBRecordHeader */
  swapbytes( idb_header->record_type ) ;
  swapbytes( idb_header->record_num ) ;

  /*
   * Swap IDB record items based on record type
   */

  switch ( idb_header->record_type )
    {
    case IDBrtHeader:
      header_rec = (IDBHeaderRecordPtr)buffer->IDB_record ;
      header_hdr = (IDBHeaderHdrPtr)&header_rec->header_hdr ;

      /* swap the HeaderHdr first */
      swapbytes( header_hdr->index_root );
      swapbytes( header_hdr->num_indexed );
      swapbytes( header_hdr->num_RID );
      /* VAR check */
#ifdef WORD64
      swap4bytes( header_hdr->next_RID.internal_id.map_rec );
      swap4bytes( header_hdr->next_RID.internal_id.res_index );
#else
      swap2bytes( header_hdr->next_RID.internal_id.map_rec );
      swap2bytes( header_hdr->next_RID.internal_id.res_index );
#endif
      swapbytes( header_hdr->last_record );
      swapbytes( header_hdr->last_data_record );
      for( ndx=0 ; ndx < URMgVecSize ; ndx++)
	swapbytes(header_hdr->group_counts[ndx]);
      for( ndx=0 ; ndx < IDBrtVecSize ; ndx++)
	swapbytes(header_hdr->rt_counts[ndx]);

      /* now swap the rest of the header */
      /* VAR check */
      for( ndx=0 ; ndx < IDBHeaderRIDMax ; ndx++)
	{
	  swap2bytes(header_rec->RID_pointers[ndx].internal_id.rec_no);
	  swap2bytes(header_rec->RID_pointers[ndx].internal_id.item_offs);
	}
      swapbytes( header_rec->num_entry );
      swapbytes( header_rec->last_entry );
      swapbytes( header_rec->free_ptr );
      swapbytes( header_rec->free_count );
      break;

    case IDBrtIndexLeaf:
      leaf_rec = (IDBIndexLeafRecordPtr)buffer->IDB_record ;
      swapbytes( leaf_rec->leaf_header.parent );
      swapbytes( leaf_rec->leaf_header.index_count );
      swapbytes( leaf_rec->leaf_header.heap_start );
      swapbytes( leaf_rec->leaf_header.free_bytes );
      if ( leaf_rec->leaf_header.index_count < 0 ||
	   leaf_rec->leaf_header.index_count > (int) IDBIndexLeafMaxCount )
	goto bad_record;
      for( ndx=0 ; (int)ndx < leaf_rec->leaf_header.index_count ; ndx++ )
	{
	  swapbytes( leaf_rec->index[ndx].index_stg );
	  swap2bytes( leaf_rec->index[ndx].data.internal_id.rec_no );
	  swap2bytes( leaf_rec->index[ndx].data.internal_id.item_offs );
	}
      break;

    case IDBrtIndexNode:
      node_rec = (IDBIndexNodeRecordPtr)buffer->IDB_record ;
      swapbytes( node_rec->node_header.parent );
      swapbytes( node_rec->node_header.index_count );
      swapbytes( node_rec->node_header.heap_start );
      swapbytes( node_rec->node_header.free_bytes );
      if ( node_rec->node_header.index_count < 0 ||
	   node_rec->node_header.index_count > (int) IDBIndexNodeMaxCount )
	goto bad_record;
      for( ndx=0 ; (int)ndx < node_rec->node_header.index_count ; ndx++ )
	{
	  swapbytes( node_rec->index[ndx].index_stg );
	  swap2bytes( node_rec->index[ndx].data.internal_id.rec_no );
	  swap2bytes( node_rec->index[ndx].data.internal_id.item_offs );
	  swapbytes( node_rec->index[ndx].LT_record );
	  swapbytes( node_rec->index[ndx].GT_record );
	}
      break;

    case IDBrtRIDMap:
      ridmap_rec = (IDBridMapRecordPtr)buffer->IDB_record ;
      ndx = 0;
      while ( (ndx < IDBridPtrVecMax) &&
	      (ridmap_rec->pointers[ndx].internal_id.rec_no != 0) )
	{
	  swap2bytes( ridmap_rec->pointers[ndx].internal_id.rec_no );
	  swap2bytes( ridmap_rec->pointers[ndx].internal_id.item_offs );
	  ndx++;
	}
      break;

    case IDBrtData:
      data_rec = (IDBDataRecordPtr)buffer->IDB_record ;
      swapbytes( data_rec->data_header.num_entry );
      swapbytes( data_rec->data_header.last_entry );
      swapbytes( data_rec->data_header.free_ptr );
      swapbytes( data_rec->data_header.free_count );
      break;

    default:
    bad_record:
      snprintf (err_msg, sizeof(err_msg), _MrmMMsg_0020,
		idb_header->record_num, idb_header->record_type);
      return Urm__UT_Error ("Idb__BM_SwapRecordBytes",
			    err_msg, NULL, NULL, MrmFAILURE) ;
    }
  return MrmSUCCESS ;
}

unsigned
Urm__SwapValidation (unsigned 		validation)
{
  swapbytes(validation);
  return validation;
}

Cardinal
Urm__SwapRGMResourceDesc (RGMResourceDescPtr	res_desc)
{
  IDBridDesc  *idb_rid_ptr;

  swapbytes( res_desc->size );
  swapbytes( res_desc->annex1 );
  if ( res_desc->type == URMrRID )
    {
      idb_rid_ptr = (IDBridDesc *)&(res_desc->key.id);
#ifdef WORD64
      swap4bytes( idb_rid_ptr->internal_id.map_rec );
      swap4bytes( idb_rid_ptr->internal_id.res_index );
#else
      swap2bytes( idb_rid_ptr->internal_id.map_rec );
      swap2bytes( idb_rid_ptr->internal_id.res_index );
#endif
    }


  return MrmSUCCESS;
}

/*
 * Swap the callback descriptor at offset cb_offs of a widget record of
 * size bytes. Return FALSE if any part of it is outside the record.
 */
static Boolean
Urm__SwapCallbackDescIn (RGMWidgetRecordPtr	widget_rec,
			 size_t			size,
			 size_t			cb_offs)
{
  RGMCallbackDescPtr	callb_desc; 	/* the callback descriptor */
  RGMCallbackItemPtr	item;		/* current callback item */
  RGMResourceDescPtr	res_desc;	/* resource description literal */
  int			ndx;		/* loop index */

  if ( ! _UrmInBuffer (cb_offs, XtOffsetOf (RGMCallbackDesc, item), size) )
    return FALSE;
  callb_desc = (RGMCallbackDescPtr) ((char *)widget_rec + cb_offs);

  swapbytes( callb_desc->validation );
  swapbytes( callb_desc->count );
  swapbytes( callb_desc->annex );
  swapbytes( callb_desc->unres_ref_count );
  if ( callb_desc->count < 0 ||
       ! _UrmInBuffer (cb_offs + XtOffsetOf (RGMCallbackDesc, item),
		       (size_t) callb_desc->count * sizeof (RGMCallbackItem),
		       size) )
    return FALSE;

  for (ndx=0 ; ndx < callb_desc->count ; ndx++)
    {
      item = &callb_desc->item[ndx];
#ifdef WORD64
      swap4bytes( item->cb_item.routine );
      swap4bytes( item->cb_item.rep_type );
#else
      swap2bytes( item->cb_item.routine );
      swap2bytes( item->cb_item.rep_type );
#endif
      switch (item->cb_item.rep_type)
	{
	case MrmRtypeInteger:
	case MrmRtypeBoolean:
	  swapbytes( item->cb_item.datum.ival );
	  break;
	case MrmRtypeSingleFloat:
	  swapbytes( item->cb_item.datum.ival );
	  _MrmOSIEEEFloatToHost((float *) &(item->cb_item.datum.ival));
	  break;
	case MrmRtypeNull:
	  break;
	case MrmRtypeResource:
	  swapbytes( item->cb_item.datum.offset );
	  if ( ! _UrmInBuffer (item->cb_item.datum.offset,
			       sizeof (RGMResourceDesc), size) )
	    return FALSE;
	  res_desc = (RGMResourceDesc *)
	    ((char *)widget_rec + item->cb_item.datum.offset);
	  Urm__SwapRGMResourceDesc( res_desc );
	  /* flag this resource as needing further byte swapping */
	  res_desc->cvt_type |= MrmResourceUnswapped;
	  break;
	default:
	  /*
	   * Other values are located by an offset in the record and are
	   * used in file byte order.
	   */
	  swapbytes( item->cb_item.datum.offset );
	  break;
	}
    }
  return TRUE;
}

Cardinal
Urm__SwapRGMCallbackDesc (RGMCallbackDescPtr	callb_desc,
			  RGMWidgetRecordPtr	widget_rec)
{
  char			err_msg[300];

  /*
   * The descriptor must be in the widget record, whose size has already
   * been swapped.
   */
  if ( (char *)callb_desc < (char *)widget_rec ||
       ! Urm__SwapCallbackDescIn (widget_rec, widget_rec->size,
				  (char *)callb_desc - (char *)widget_rec) )
    {
      snprintf (err_msg, sizeof(err_msg), _MrmMMsg_0021,
		MrmRtypeCallback, 0);
      return Urm__UT_Error ("Urm__SwapRGMCallbackDesc",
			    err_msg, NULL, NULL, MrmFAILURE) ;
    }
  return MrmSUCCESS;
}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Urm__SwapRGMWidgetRecord swaps a widget record read from a byte
 *	swapped file to native byte order: the record header, the argument
 *	list descriptors, any callback and resource descriptors they locate,
 *	the children list and the creation callback. Other argument values
 *	are used in file byte order. Every part of the record must lie within
 *	the size given in its header, which the caller has checked against
 *	the buffer that holds the record.
 *
 *  FORMAL PARAMETERS:
 *
 *	widget_rec	Widget record to swap in place
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_WIDGET_REC	the record does not fit in its size
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__SwapRGMWidgetRecord(RGMWidgetRecordPtr	widget_rec)
{

  /*
   *  Local variables
   */
  RGMArgListDescPtr	arg_list;   	/* pointer to widget arglist */
  RGMChildrenDescPtr	child_list; 	/* pointer to the widgets children */
  RGMArgumentPtr	arg;		/* current argument */
  RGMResourceDescPtr	res_desc;   	/* resource description literal */
  int			ndx;	   	/* loop index */
  IDBridDesc	  	*idb_rid_ptr;
  size_t		size;		/* bytes in the record */
  size_t		offs;		/* offset of a value in the record */

  /* Swap the main part of the widget record */

  swapbytes( widget_rec->size );
  swapbytes( widget_rec->access );
  swapbytes( widget_rec->lock );
  swapbytes( widget_rec->type );
  swapbytes( widget_rec->name_offs );
  swapbytes( widget_rec->class_offs );
  swapbytes( widget_rec->arglist_offs );
  swapbytes( widget_rec->children_offs );
  swapbytes( widget_rec->comment_offs );
  swapbytes( widget_rec->creation_offs );
  swapbytes( widget_rec->variety );
  swapbytes( widget_rec->annex );

  size = widget_rec->size;
  if ( size < sizeof (RGMWidgetRecord) )
    goto bad_record;

  /* handle the argument list */

  if (widget_rec->arglist_offs > 0)
    {
      if ( ! _UrmInBuffer (widget_rec->arglist_offs,
			   XtOffsetOf (RGMArgListDesc, args), size) )
	goto bad_record;
      arg_list = (RGMArgListDesc *)
	((char *)widget_rec + widget_rec->arglist_offs);
      swapbytes( arg_list->count );
      swapbytes( arg_list->extra );
      if ( arg_list->count < 0 ||
	   ! _UrmInBuffer (widget_rec->arglist_offs +
			   XtOffsetOf (RGMArgListDesc, args),
			   (size_t) arg_list->count * sizeof (RGMArgument),
			   size) )
	goto bad_record;
      for ( ndx=0 ; ndx<arg_list->count ; ndx++ )
	{
	  arg = &arg_list->args[ndx];
	  swapbytes( arg->tag_code );
	  swapbytes( arg->stg_or_relcode.tag_offs );
	  swapbytes( arg->arg_val.rep_type );

	  switch( arg->arg_val.rep_type )
	    {
	    case MrmRtypeInteger:
	    case MrmRtypeBoolean:
	      swapbytes( arg->arg_val.datum.ival );
	      break;
	    case MrmRtypeSingleFloat:
	      swapbytes( arg->arg_val.datum.ival );
	      _MrmOSIEEEFloatToHost((float *) &(arg->arg_val.datum.ival));
	      break;
	    default:
	      swapbytes( arg->arg_val.datum.offset );
	      break;
	    }

	  offs = arg->arg_val.datum.offset;

	  switch( arg->arg_val.rep_type )
	    {
	      /* these are offsets into the file, handle them specially */
	    case MrmRtypeCallback:
	      if ( ! Urm__SwapCallbackDescIn (widget_rec, size, offs) )
		goto bad_record;
	      break;
	    case MrmRtypeResource:
	      if ( ! _UrmInBuffer (offs, sizeof (RGMResourceDesc), size) )
		goto bad_record;
	      res_desc = (RGMResourceDesc *)((char *)widget_rec + offs);
	      Urm__SwapRGMResourceDesc( res_desc );
	      /* flag this resource as needing further byte swapping */
	      res_desc->cvt_type |= MrmResourceUnswapped;
	      break;
	    default:
	      /*
	       * Immediate values need no more swapping, and other values in
	       * the record are used in file byte order.
	       */
	      break;
	    }
	}
    }

  /* handle the child list */

  if (widget_rec->children_offs > 0)
    {
      if ( ! _UrmInBuffer (widget_rec->children_offs,
			   XtOffsetOf (RGMChildrenDesc, child), size) )
	goto bad_record;
      child_list = (RGMChildrenDesc *)
	((char *)widget_rec + widget_rec->children_offs);
      swapbytes( child_list->count );
      swapbytes( child_list->unused1 );
      swapbytes( child_list->annex1 );
      if ( child_list->count < 0 ||
	   ! _UrmInBuffer (widget_rec->children_offs +
			   XtOffsetOf (RGMChildrenDesc, child),
			   (size_t) child_list->count * sizeof (RGMChildDesc),
			   size) )
	goto bad_record;
      for ( ndx=0 ; ndx<child_list->count ; ndx++ )
	{
	  swapbytes( child_list->child[ndx].annex1 );
	  if (child_list->child[ndx].type ==  URMrRID )
	    {
	      idb_rid_ptr = (IDBridDesc *)&(child_list->child[ndx].key.id);
#ifdef WORD64
	      swap4bytes( idb_rid_ptr->internal_id.map_rec );
	      swap4bytes( idb_rid_ptr->internal_id.res_index );
#else
	      swap2bytes( idb_rid_ptr->internal_id.map_rec );
	      swap2bytes( idb_rid_ptr->internal_id.res_index );
#endif
	    }
	  else
	    swapbytes( child_list->child[ndx].key.index_offs );
	}
    }

  /* handle the creation callback, if any */

  if (widget_rec->creation_offs > 0)
    {
      if ( ! Urm__SwapCallbackDescIn (widget_rec, size,
				      widget_rec->creation_offs) )
	goto bad_record;
    }

  return MrmSUCCESS ;

 bad_record:
  return Urm__UT_Error ("Urm__SwapRGMWidgetRecord", _MrmMMsg_0026,
			NULL, NULL, MrmBAD_WIDGET_REC) ;
}
