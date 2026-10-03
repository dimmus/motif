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


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: MrmIindex.c /main/13 1996/11/13 13:57:31 drk $"
#endif
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM): IDB Facility
 *	Index management routines
 *
 *  ABSTRACT:
 *
 *	These routines manage the index of an IDB file, including entering
 *	data entries accessed by index. These routines are read or common
 *	(used by both read and writing (MrmIindexw.c)).
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
#include <Mrm/IDB.h>
#include "MrmMsgI.h"


/*
 *
 *  TABLE OF CONTENTS
 *
 *	Idb__INX_ReturnItem		- Return the data entry for an index
 *
 *	Idb__INX_FindIndex		- Search the index
 *
 *	Idb__INX_SearchIndex		- Search a record for an index
 *
 *	Idb__INX_GetBTreeRecord		- Read a record in the B-tree
 *
 *	Idb__INX_FindResources		- Search the index for resources
 *					  matching the filter
 *
 */


/*
 *
 *  DEFINE and MACRO DEFINITIONS
 *
 */

/*
 * Macros which validate index records in buffers
 */
#define	Idb__INX_ValidLeaf(buffer) \
     (_IdbBufferRecordType(buffer)==IDBrtIndexLeaf)
#define	Idb__INX_ValidNode(buffer) \
     (_IdbBufferRecordType(buffer)==IDBrtIndexNode)
#define	Idb__INX_ValidRecord(buffer) \
     (_IdbBufferRecordType(buffer)==IDBrtIndexLeaf ||  \
      _IdbBufferRecordType(buffer)==IDBrtIndexNode)

/*
 * Maximum depth of the B-tree index. The index is a balanced B-tree in
 * which every node has at least two children, and a file has fewer than
 * 32768 records, so a valid index is at most 16 levels deep. The limit
 * stops the search of a corrupt file in which the record pointers form
 * a cycle.
 */
#define	IDBMaxIndexDepth	64



/*
 *  Helper routines for reading index records, whose contents come from
 *  the file and must be checked before use.
 */

/*
 * Return the number of entries in an index leaf or node record, or -1 if
 * the record is not an index record or the count does not fit in it.
 */
static int
Idb__INX_EntryCount (IDBRecordBufferPtr		buffer)
{
  int			count ;		/* entry count */

  switch ( _IdbBufferRecordType (buffer) )
    {
    case IDBrtIndexLeaf:
      count = ((IDBIndexLeafRecordPtr) buffer->IDB_record)->
	leaf_header.index_count ;
      if ( count < 0 || count > (int) IDBIndexLeafMaxCount ) return -1 ;
      return count ;
    case IDBrtIndexNode:
      count = ((IDBIndexNodeRecordPtr) buffer->IDB_record)->
	node_header.index_count ;
      if ( count < 0 || count > (int) IDBIndexNodeMaxCount ) return -1 ;
      return count ;
    default:
      return -1 ;
    }
}

/*
 * Return the index string at offset stgoffs from the entry vector at
 * stgbase in a record buffer, or NULL if the string does not start and
 * end (with its NUL) within the record.
 */
static char *
Idb__INX_EntryString (IDBRecordBufferPtr	buffer,
		      char			*stgbase,
		      MrmOffset			stgoffs)
{
  size_t		offs ;		/* string offset in record */

  offs = (stgbase - (char *) buffer->IDB_record) + (size_t) stgoffs ;
  if ( ! _UrmStringInBuffer (buffer->IDB_record, offs, IDBRecordSize) )
    return NULL ;
  return (char *) buffer->IDB_record + offs ;
}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_ReturnItem locates a data entry in the file, and returns
 *	the data entry pointer (without reading the data record).
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file in which to write entry
 *	index		The entry's case-sensitive index
 *	data_entry	To return data entry pointer for data
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
Idb__INX_ReturnItem (IDBFile			file_id,
		     char			*index,
		     IDBDataHandle		*data_entry)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordBufferPtr	bufptr ;	/* buffer containing entry */
  MrmCount		entndx ;	/* entry index */
  IDBIndexLeafRecordPtr	leafrec ;	/* index leaf record */
  IDBIndexNodeRecordPtr	noderec ;	/* index node record */

  /*
   * Attempt to find the index
   */
  result = Idb__INX_FindIndex (file_id, index, &bufptr, &entndx) ;
  switch ( result )
    {
    case MrmINDEX_GT:
    case MrmINDEX_LT:
      return MrmNOT_FOUND ;
    case MrmSUCCESS:
      break ;
    default:
      return result ;
    }

  /*
   * Point into the buffer, and retrieve the data pointer
   */
  switch ( _IdbBufferRecordType (bufptr) )
    {
    case IDBrtIndexLeaf:
      leafrec = (IDBIndexLeafRecordPtr) bufptr->IDB_record ;
      data_entry->rec_no = leafrec->index[entndx].data.internal_id.rec_no ;
      data_entry->item_offs =
	leafrec->index[entndx].data.internal_id.item_offs ;
      return MrmSUCCESS ;
    case IDBrtIndexNode:
      noderec = (IDBIndexNodeRecordPtr) bufptr->IDB_record ;
      data_entry->rec_no = noderec->index[entndx].data.internal_id.rec_no ;
      data_entry->item_offs =
	noderec->index[entndx].data.internal_id.item_offs ;
      return MrmSUCCESS ;
    default:
      return Urm__UT_Error ("Idb__INX_ReturnItem", _MrmMMsg_0010,
			    file_id, NULL, MrmBAD_RECORD) ;
    }

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_FindIndex finds the index record containing an index entry,
 *	and returns the buffer containing that record. It is used both as the
 *	low-level routine for locating an index for retrieving a data entry,
 *	and for locating the record in which a new index should be inserted.
 *	Thus the interpretation of the return code is:
 *
 *	MrmSUCCESS	found the index, the index record is in the buffer
 *			and the index_return locates the entry
 *	MrmINDEX_GT	buffer contains the leaf index record which should
 *	MrmINDEX_LT	contain the index, and index_return locates the entry
 *			in the buffer at which search terminated. The result
 *			value indicates how the given index orders against
 *			the entry in index_return.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file in which to find index
 *	index		Case-sensitive index string
 *	buffer_return	To return pointer to buffer containing index record
 *	index_return	To return item's index in the records index vector
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmINDEX_GT	index not found, but orders greater-than entry at
 *			index_return
 *	MrmINDEX_LT	index not found, but orders less-than entry at
 *			index_return
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Idb__INX_FindIndex (IDBFile			file_id,
		    char			*index,
		    IDBRecordBufferPtr		*buffer_return,
		    MrmCount			*index_return)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  int			depth = 0 ;	/* levels searched */

  /*
   * Initialize search at the root of the index, then continue searching
   * until either the index is found or search terminates at some leaf record.
   */
  if ( !file_id->index_root ) return MrmFAILURE ;
  result = Idb__BM_GetRecord (file_id, file_id->index_root, buffer_return) ;
  if ( result != MrmSUCCESS ) return result ;
  if ( ! Idb__INX_ValidRecord(*buffer_return) )
    return Urm__UT_Error ("Idb__INX_FindIndex", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_RECORD) ;

  do  {
    if ( ++depth > IDBMaxIndexDepth )
      return Urm__UT_Error ("Idb__INX_FindIndex", _MrmMMsg_0010,
			    file_id, NULL, MrmBAD_BTREE) ;
    result =
      Idb__INX_SearchIndex (file_id, index, *buffer_return, index_return) ;
    if ( _IdbBufferRecordType(*buffer_return) == IDBrtIndexLeaf) return result ;
    switch ( result )
      {
      case MrmINDEX_GT:
      case MrmINDEX_LT:
	result = Idb__INX_GetBtreeRecord
	  (file_id, buffer_return, *index_return, result) ;
	if (result != MrmSUCCESS )
	  {
	    if (result == MrmNOT_FOUND)
	      result = MrmEOF;
	    return result ;
	  }
	break ;
      default:
	return result ;
      }
  } while ( TRUE ) ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_SearchIndex searches a record for an index. The record
 *	may be either a leaf or a node record. If the index is found,
 *	index_return is its entry in the records index vector. If it is not
 *	found, then index_return locates the entry in the record at which
 *	search terminated.
 *
 *	Thus the interpretation of the return code is:
 *
 *	MrmSUCCESS	found the index, and the index_return locates the entry
 *	MrmINDEX_GT	index orders greater-than the entry at index_return
 *	MrmINDEX_LT	index orders less-than the entry at index_return
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file in which to find index
 *	index		Case-sensitive index string
 *	buffer		Buffer containing record to be searched
 *	index_return	To return item's index in the records index vector
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmINDEX_GT	index not found, but orders greater-than entry at
 *			index_return
 *	MrmINDEX_LT	index not found, but orders less-than entry at
 *			index_return
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Idb__INX_SearchIndex (IDBFile			file_id,
		      char			*index,
		      IDBRecordBufferPtr	buffer,
		      MrmCount			*index_return)
{

  /*
   *  Local variables
   */
  MrmType		buftyp ;	/* buffer type */
  IDBIndexLeafRecordPtr	leafrec =NULL ;	/* index leaf record */
  IDBIndexNodeRecordPtr	noderec ;	/* index node record */
  IDBIndexLeafEntryPtr	leaf_ndxvec = NULL;	/* index leaf entry vector */
  IDBIndexNodeEntryPtr	node_ndxvec = NULL;	/* index node entry vector */
  int			ndxcnt ;	/* number of entries in vector */
  char			*stgbase ;	/* base adddress for string offsets */
  int			lowlim ;	/* binary search lower limit index */
  int			uprlim ;	/* binary search upper limit index */
  char			*ndxstg ;	/* pointer to current index string */
  int			cmpres=0;	/* strncmp result */


  /*
   * Set up search pointers based on the record type
   */
  buftyp = _IdbBufferRecordType (buffer) ;
  switch ( buftyp )
    {
    case IDBrtIndexLeaf:
      leafrec = (IDBIndexLeafRecordPtr) buffer->IDB_record ;
      leaf_ndxvec = leafrec->index ;
      stgbase = (char *) leafrec->index ;
      break ;
    case IDBrtIndexNode:
      noderec = (IDBIndexNodeRecordPtr) buffer->IDB_record ;
      node_ndxvec = noderec->index ;
      stgbase = (char *) noderec->index ;
      break ;
    default:
      return Urm__UT_Error ("Idb__INX_SearchIndex", _MrmMMsg_0010,
			    file_id, NULL, MrmBAD_RECORD) ;
    }
  ndxcnt = Idb__INX_EntryCount (buffer) ;
  if ( ndxcnt < 0 )
    return Urm__UT_Error ("Idb__INX_SearchIndex", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_BTREE) ;

  /*
   * Search the index vector for the given index (binary search). An
   * empty record orders the index before its (nonexistent) first entry.
   */
  Idb__BM_MarkActivity (buffer) ;
  *index_return = 0 ;
  for ( lowlim=0,uprlim=ndxcnt-1 ; lowlim<=uprlim ; )
    {
      *index_return = (lowlim+uprlim) / 2 ;
      ndxstg = Idb__INX_EntryString
	(buffer, stgbase, (buftyp==IDBrtIndexLeaf) ?
	 leaf_ndxvec[*index_return].index_stg :
	 node_ndxvec[*index_return].index_stg) ;
      if ( ndxstg == NULL )
	return Urm__UT_Error ("Idb__INX_SearchIndex", _MrmMMsg_0010,
			      file_id, NULL, MrmBAD_BTREE) ;
      cmpres = strncmp (index, ndxstg, IDBMaxIndexLength) ;
      if ( cmpres == 0 ) return MrmSUCCESS ;
      if ( cmpres < 0 ) uprlim = *index_return - 1 ;
      if ( cmpres > 0 ) lowlim = *index_return + 1 ;
    }

  /*
   * Not found, result determined by final ordering.
   */
  return (cmpres>0) ? MrmINDEX_GT : MrmINDEX_LT ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in the next level index record in the B-tree
 *	associated with some entry in the current record (i.e. the one
 *	currently contained in the buffer). The buffer pointer is reset.
 *	The order variable indicates which record to read:
 *		MrmINDEX_GT - read the record ordering greater-than the entry
 *		MrmINDEX_LT - read the record ordering less-than the entry
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file from which to read record
 *	buffer_return	points to current buffer; reset to buffer read in
 *	entry_index	entry in current buffer to use as reference
 *	order		MrmINDEX_GT for GT ordered record, else MrmINDEX_LT
 *			for LT ordered record.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_ORDER	Order variable has illegal value
 *	MrmBAD_RECORD	new record not an index record
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Idb__INX_GetBtreeRecord ( IDBFile		file_id,
			  IDBRecordBufferPtr	*buffer_return,
			  MrmCount		entry_index,
			  Cardinal		order)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBIndexNodeRecordPtr	recptr ;	/* node record in buffer */
  IDBRecordNumber	recno ;		/* Record number to read in */

  /*
   * Set buffer pointers
   */
  recptr = (IDBIndexNodeRecordPtr) (*buffer_return)->IDB_record ;
  if ( ! Idb__INX_ValidNode(*buffer_return) ||
       entry_index < 0 || entry_index >= (int) IDBIndexNodeMaxCount )
    return Urm__UT_Error ("Idb__INX_GetBTreeRecord", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_BTREE) ;

  /*
   * Retrieve the record number
   */
  switch ( order )
    {
    case MrmINDEX_GT:
      recno = recptr->index[entry_index].GT_record ;
      break ;
    case MrmINDEX_LT:
      recno = recptr->index[entry_index].LT_record ;
      break ;
    default:
      return Urm__UT_Error ("Idb__INX_GetBTreeRecord", _MrmMMsg_0010,
			    file_id, NULL, MrmBAD_ORDER) ;
    }

  /*
   * Retrieve and sanity check the record
   */
  result = Idb__BM_GetRecord (file_id, recno, buffer_return) ;
  if ( result != MrmSUCCESS ) return result ;
  if ( ! Idb__INX_ValidRecord(*buffer_return) )
    return Urm__UT_Error ("Idb__INX_GetBTreeRecord", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_RECORD) ;

  /*
   * Record successfully retrieved
   */
  return MrmSUCCESS ;

}



/*
 * The recursive part of Idb__INX_FindResources. visited is a bit vector
 * of the records already searched, and depth is the level of recno in
 * the tree; both keep a corrupt index from making the search loop.
 */
static Cardinal
Idb__INX_FindResourcesIn (IDBFile		file_id,
			  IDBRecordNumber	recno,
			  MrmGroup		group_filter,
			  MrmType		type_filter,
			  URMPointerListPtr	index_list,
			  unsigned char		*visited,
			  int			depth)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordBufferPtr	bufptr ;	/* buffer containing entry */
  int			entndx ;	/* entry loop index */
  IDBIndexLeafRecordPtr	leafrec ;	/* index leaf record */
  IDBIndexNodeRecordPtr	noderec ;	/* index node record */
  IDBIndexLeafEntryPtr	leaf_ndxvec ;	/* index leaf entry vector */
  IDBIndexNodeEntryPtr	node_ndxvec ;	/* index node entry vector */
  int			ndxcnt ;	/* number of entries in vector */
  char			*stgbase ;	/* base adddress for string offsets */
  char			*ndxstg ;	/* index string of current entry */
  IDBRecordNumber	gt_record ;	/* GT record of current entry */
  IDBDataHandle		entry_data ;	/* data entry of current entry */


  /*
   * Each record of a valid index is reached exactly once, so a record
   * seen twice, or a tree deeper than any valid one, means the record
   * pointers are corrupt.
   */
  if ( recno < IDBHeaderRecordNumber || depth > IDBMaxIndexDepth ||
       (visited[recno / 8] & (1 << (recno % 8))) )
    return Urm__UT_Error ("Idb__INX_FindResources", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_BTREE) ;
  visited[recno / 8] |= 1 << (recno % 8) ;

  /*
   * Read the record in, then bind pointers and process the record.
   */
  result = Idb__BM_GetRecord (file_id, recno, &bufptr) ;
  if ( result != MrmSUCCESS ) return result ;
  ndxcnt = Idb__INX_EntryCount (bufptr) ;
  if ( ndxcnt < 0 )
    return Urm__UT_Error ("Idb__INX_FindResources", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_RECORD) ;

  switch ( _IdbBufferRecordType (bufptr) )
    {

      /*
       * Simply apply the filter to all entries in the leaf record
       */
    case IDBrtIndexLeaf:
      leafrec = (IDBIndexLeafRecordPtr) bufptr->IDB_record ;
      leaf_ndxvec = leafrec->index ;
      stgbase = (char *) leafrec->index ;

      for ( entndx=0 ; entndx<ndxcnt ; entndx++ )
	{
	  /*
	   * Matching the filter reads another record, which may reuse
	   * this buffer, so re-read this record before using it again.
	   */
	  if ( entndx > 0 )
	    {
	      result = Idb__BM_GetRecord (file_id, recno, &bufptr) ;
	      if ( result != MrmSUCCESS ) return result ;
	      if ( ! Idb__INX_ValidLeaf(bufptr) ||
		   Idb__INX_EntryCount (bufptr) != ndxcnt )
		return Urm__UT_Error ("Idb__INX_FindResources", _MrmMMsg_0010,
				      file_id, NULL, MrmBAD_RECORD) ;
	      leafrec = (IDBIndexLeafRecordPtr) bufptr->IDB_record ;
	      leaf_ndxvec = leafrec->index ;
	      stgbase = (char *) leafrec->index ;
	    }

	  entry_data.rec_no = leaf_ndxvec[entndx].data.internal_id.rec_no;
	  entry_data.item_offs =
	    leaf_ndxvec[entndx].data.internal_id.item_offs;
	  ndxstg = Idb__INX_EntryString
	    (bufptr, stgbase, leaf_ndxvec[entndx].index_stg) ;
	  if ( ndxstg == NULL )
	    return Urm__UT_Error ("Idb__INX_FindResources", _MrmMMsg_0010,
				  file_id, NULL, MrmBAD_BTREE) ;
	  ndxstg = XtNewString (ndxstg) ;

	  if ( Idb__DB_MatchFilter(file_id, entry_data, group_filter,
				   type_filter) )
	    UrmPlistAppendString (index_list, ndxstg) ;
	  XtFree (ndxstg) ;
	}
      return MrmSUCCESS ;

      /*
       * Process the first LT record, then process each index followed by
       * its GT record. This will produce a correctly ordered list. The
       * record is read again, and all pointers bound, after each FindResources
       * call in order to guarantee that buffer turning has not purged the
       * current record from memory
       */
    case IDBrtIndexNode:
      noderec = (IDBIndexNodeRecordPtr) bufptr->IDB_record ;
      node_ndxvec = noderec->index ;
      result = Idb__INX_FindResourcesIn
	(file_id, node_ndxvec[0].LT_record,
	 group_filter, type_filter, index_list, visited, depth + 1) ;
      if ( result != MrmSUCCESS ) return result ;

      for ( entndx=0 ; entndx<ndxcnt ; entndx++ )
	{
	  result = Idb__BM_GetRecord (file_id, recno, &bufptr) ;
	  if ( result != MrmSUCCESS ) return result ;
	  if ( ! Idb__INX_ValidNode(bufptr) ||
	       Idb__INX_EntryCount (bufptr) != ndxcnt )
	    return Urm__UT_Error ("Idb__INX_FindResources", _MrmMMsg_0010,
				  file_id, NULL, MrmBAD_RECORD) ;
	  noderec = (IDBIndexNodeRecordPtr) bufptr->IDB_record ;
	  node_ndxvec = noderec->index ;
	  stgbase = (char *) noderec->index ;

	  entry_data.rec_no = node_ndxvec[entndx].data.internal_id.rec_no;
	  entry_data.item_offs =
	    node_ndxvec[entndx].data.internal_id.item_offs;
	  gt_record = node_ndxvec[entndx].GT_record ;
	  ndxstg = Idb__INX_EntryString
	    (bufptr, stgbase, node_ndxvec[entndx].index_stg) ;
	  if ( ndxstg == NULL )
	    return Urm__UT_Error ("Idb__INX_FindResources", _MrmMMsg_0010,
				  file_id, NULL, MrmBAD_BTREE) ;
	  ndxstg = XtNewString (ndxstg) ;

	  if ( Idb__DB_MatchFilter
	       (file_id, entry_data, group_filter, type_filter) )
	    UrmPlistAppendString (index_list, ndxstg) ;
	  XtFree (ndxstg) ;
	  result = Idb__INX_FindResourcesIn
	    (file_id, gt_record, group_filter, type_filter, index_list,
	     visited, depth + 1) ;
	  if ( result != MrmSUCCESS ) return result ;
	}
      return MrmSUCCESS ;

    default:
      return Urm__UT_Error ("Idb__INX_FindResources", _MrmMMsg_0010,
			    file_id, NULL, MrmBAD_RECORD) ;
    }

}




/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This is the internal routine which searches the database for
 *	indexed resources matching a filter. It starts at the current node,
 *	then recurses down the BTree inspecting every entry. Each entry
 *	which matches the filter is appended to the index list.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		The IDB file id returned by XmIdbOpenFile
 *	recno		The record to be searched. If a node entry,
 *			then each pointed-to record is also searched.
 *	group_filter	if not null, entries found must match this group
 *	type_filter	if not null, entries found must match this type
 *	index_list	A pointer list in which to return index
 *			strings for matches. The required strings
 *			are automatically allocated.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	operation failed, no further reason
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Idb__INX_FindResources (IDBFile			file_id,
			IDBRecordNumber		recno,
			MrmGroup		group_filter,
			MrmType			type_filter,
			URMPointerListPtr	index_list)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  unsigned char		*visited ;	/* records searched, one bit each */

  /*
   * Record numbers are positive shorts, so one bit for each of 32768
   * possible records.
   */
  visited = (unsigned char *) XtCalloc (32768 / 8, 1) ;
  result = Idb__INX_FindResourcesIn (file_id, recno, group_filter,
				     type_filter, index_list, visited, 1) ;
  XtFree ((char *) visited) ;
  return result ;

}
