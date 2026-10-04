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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: UilDB.c /main/11 1996/11/21 20:03:11 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Bindary Database :
 *
 *  ABSTRACT:
 *
 *--
 */

/*
 * This file contains routines which change the internal tables of UIL based on
 * a binary data base parameter in the command line
 */


/*
 *
 *  INCLUDE FILES
 *
 */
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>  /* for getpwnam, getpwuid */
#include "UilDefI.h"

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

/*
 *
 *  TABLE OF CONTENTS
 *
 *
 */


/*
 *
 *  DEFINE and MACRO DEFINITIONS
 *
 */
#define _check_read( __number_returned ) \
	if (( (__number_returned) != 1) || (feof(dbfile)) || (ferror(dbfile)) ) \
	{  diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column ); }

/*
 * A table read from the database must have room for every item the header
 * claims it holds, plus 'extra' items for tables indexed 0..num_items.
 * A database that fails this check is corrupt, which is a fatal error.
 */
static void
db_check_table_size (_db_header_ptr header, int extra, size_t item_size)
{
    if (header->num_items < 0 || header->table_size < 0 ||
	(size_t) header->num_items + extra >
	(size_t) header->table_size / item_size)
	diag_issue_diagnostic (d_bad_database, diag_k_no_source, diag_k_no_column);
}

/*
 * The number of entries, counted from index 0, in a table that has just
 * been read from the database.
 */
static int
db_table_entries (_db_header_ptr header)
{
    switch (header->table_id)
	{
	case Constraint_Tab:
	case Argument_Type_Table_Value:
	case Child_Class_Table:
	case Charset_Wrdirection_Table:
	case Charset_Parsdirection_Table:
	case Charset_Charsize_Table:
	case Key_Table:
	case Key_Table_Case_Ins:
	    return header->num_items;
	case Allowed_Argument_Table:
	case Allowed_Child_Table:
	case Allowed_Control_Table:
	case Allowed_Reason_Table:
	case Charset_Xmstring_Names_Table:
	case Charset_Lang_Names_Table:
	case Uil_Widget_Names:
	case Uil_Children_Names:
	case Uil_Argument_Names:
	case Uil_Reason_Names:
	case Uil_Enumval_names:
	case Uil_Charset_Names:
	case Uil_Widget_Funcs:
	case Uil_Argument_Toolkit_Names:
	case Uil_Reason_Toolkit_Names:
	case Enum_Set_Table:
	    return header->num_items + 1;
	case Charset_Lang_Codes_Table:
	case Argument_Enum_Set_Table:
	case Related_Argument_Table:
	case Uil_Gadget_Funcs:
	case Uil_Urm_Nondialog_Class:
	case Uil_Urm_Subtree_Resource:
	    return header->table_size / (int) sizeof (unsigned short int);
	case Enumval_Values_Table:
	    return header->table_size / (int) sizeof (int);
	default:
	    return 0;
	}
}

/*
 * The number of entries, counted from index 0, that a table must have
 * for the maxima in the database globals.  The compiler indexes the
 * tables with codes up to those maxima.
 */
static int
db_table_required_entries (_db_globals *globals, int table_id)
{
    switch (table_id)
	{
	case Key_Table:
	case Key_Table_Case_Ins:
	    return globals->key_k_keyword_count;
	case Charset_Lang_Names_Table:
	case Charset_Lang_Codes_Table:
	    return globals->charset_lang_table_max;
	case Constraint_Tab:
	    /* a bit vector indexed by argument code - 1 */
	    return (globals->uil_max_arg + 7) / 8;
	case Allowed_Control_Table:
	case Uil_Widget_Names:
	case Uil_Widget_Funcs:
	case Uil_Gadget_Funcs:
	case Uil_Urm_Nondialog_Class:
	case Uil_Urm_Subtree_Resource:
	    return globals->uil_max_object + 1;
	case Argument_Type_Table_Value:
	case Allowed_Argument_Table:
	case Uil_Argument_Names:
	case Uil_Argument_Toolkit_Names:
	case Argument_Enum_Set_Table:
	case Related_Argument_Table:
	    return globals->uil_max_arg + 1;
	case Allowed_Reason_Table:
	case Uil_Reason_Names:
	case Uil_Reason_Toolkit_Names:
	    return globals->uil_max_reason + 1;
	case Charset_Xmstring_Names_Table:
	case Charset_Wrdirection_Table:
	case Charset_Parsdirection_Table:
	case Charset_Charsize_Table:
	case Uil_Charset_Names:
	    return globals->uil_max_charset + 1;
	case Uil_Enumval_names:
	case Enumval_Values_Table:
	    return globals->uil_max_enumval + 1;
	case Enum_Set_Table:
	    return globals->uil_max_enumset + 1;
	case Child_Class_Table:
	case Allowed_Child_Table:
	case Uil_Children_Names:
	    return globals->uil_max_child + 1;
	default:
	    return 0;
	}
}




/*
 *
 *  EXTERNAL VARIABLE DECLARATIONS
 *
 */

/*
 *
 *  GLOBAL VARIABLE DECLARATIONS
 *
 */


/*
 *
 *  OWN VARIABLE DECLARATIONS
 *
 */
static FILE *dbfile;
static int  num_bits;

void db_incorporate(void)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine incorporate the binary database passed in the command line.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

/*
 *  External Functions
 */

/*
 *  Local variables
 */
{
    int			return_num_items;
    _db_header		header;
    _db_globals		globals;
    int			entries[Uil_Children_Names + 1];
    int			i;

    db_open_file();

    return_num_items = fread (&globals, sizeof(_db_globals), 1, dbfile);
    _check_read (return_num_items);

    /*
     * Some heuristics to see if this is a reasonable database.
     * The magic numbers are about 10 times as big as the DXm database
     * for DECWindows V3. The casts reject negative values as well.
     * The diagnostic does a fatal exit.
     */
    if ( (unsigned) globals.uil_max_arg>5000 ||
	 (unsigned) globals.uil_max_charset>200 ||
	 (unsigned) globals.charset_lang_table_max>1000 ||
	 (unsigned) globals.uil_max_object>500 ||
	 (unsigned) globals.uil_max_reason>1000 ||
	 (unsigned) globals.uil_max_enumval>3000 ||
	 (unsigned) globals.uil_max_enumset>1000 ||
	 (unsigned) globals.key_k_keyword_count>10000 ||
	 (unsigned) globals.key_k_keyword_max_length>200 ||
	 (unsigned) globals.uil_max_child>250 ||
	 globals.key_k_keyword_count<1)
	diag_issue_diagnostic (d_bad_database,
			       diag_k_no_source,
			       diag_k_no_column);

    uil_max_arg = globals.uil_max_arg ;
    uil_max_charset = globals.uil_max_charset ;
    charset_lang_table_max = globals.charset_lang_table_max ;
    uil_max_object = globals.uil_max_object ;
    uil_max_reason = globals.uil_max_reason ;
    uil_max_enumval = globals.uil_max_enumval ;
    uil_max_enumset = globals.uil_max_enumset ;
    key_k_keyword_count = globals.key_k_keyword_count ;
    key_k_keyword_max_length = globals.key_k_keyword_max_length ;
    uil_max_child = globals.uil_max_child;
    if (globals.version >= 3)
	num_bits = _DB_BIT_VECTOR_SIZE (uil_max_object);
    else
	num_bits = (uil_max_object +7) / 8;

    if (globals.version > DB_Compiled_Version)
	diag_issue_diagnostic( d_future_version, diag_k_no_source, diag_k_no_column );

    /* -1 marks a table that is not in the file */
    for (i = 0; i <= Uil_Children_Names; i++)
	entries[i] = -1;

    for (;;)
	{
	return_num_items = fread (&header, sizeof(_db_header), 1, dbfile);
	if (feof(dbfile)) break;
	_check_read (return_num_items);
	
	/* Validate header values to prevent exploitation of tainted data */
	if (header.table_size <= 0 || header.table_size > SIZE_MAX / 4) {
	    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
	    continue;
	}
	if (header.num_items < 0 || header.num_items > SIZE_MAX / 4) {
	    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
	    continue;
	}
	switch (header.table_id)
	    {
	    case Constraint_Tab:
		db_check_table_size (&header, 0, sizeof (unsigned char));
		constraint_tab = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (constraint_tab,
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Argument_Type_Table_Value:
		/*
		 * NOTE: The first entry is not used but we copy it anyway
		 */
		db_check_table_size (&header, 0, sizeof (unsigned char));
		argument_type_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (argument_type_table,
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Child_Class_Table:
		/*
		 * NOTE: The first entry is not used but we copy it anyway
		 */
		db_check_table_size (&header, 0, sizeof (unsigned char));
		child_class_table =
		  (unsigned char *) XtMalloc (header.table_size);
		return_num_items =
		  fread (child_class_table,
			 sizeof(unsigned char) * header.num_items, 1, dbfile);
		_check_read (return_num_items);
		break;
	    case Charset_Wrdirection_Table:
		db_check_table_size (&header, 0, sizeof (unsigned char));
		charset_writing_direction_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (charset_writing_direction_table,
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Charset_Parsdirection_Table:
		db_check_table_size (&header, 0, sizeof (unsigned char));
		charset_parsing_direction_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (charset_parsing_direction_table,
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Charset_Charsize_Table:
		db_check_table_size (&header, 0, sizeof (unsigned char));
		charset_character_size_table = (unsigned char *) XtMalloc (header.table_size);
		return_num_items = fread (charset_character_size_table,
					     sizeof(unsigned char) * header.num_items,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Key_Table:
	    case Key_Table_Case_Ins:
		db_read_ints_and_string (&header);
		break;
	    case Allowed_Argument_Table:
	    case Allowed_Child_Table:
	    case Allowed_Control_Table:
	    case Allowed_Reason_Table:
		db_read_char_table (&header);
		break;
	    case Charset_Xmstring_Names_Table:
	    case Charset_Lang_Names_Table:
	    case Uil_Widget_Names:
	    case Uil_Children_Names:
	    case Uil_Argument_Names:
	    case Uil_Reason_Names:
	    case Uil_Enumval_names:
	    case Uil_Charset_Names:
	    case Uil_Widget_Funcs:
	    case Uil_Argument_Toolkit_Names:
	    case Uil_Reason_Toolkit_Names:
		db_read_length_and_string (&header);
		break;
	    case Charset_Lang_Codes_Table:
		charset_lang_codes_table = (unsigned short int *) XtMalloc (header.table_size);
		/* Validate table_size to prevent overflow */
		if (header.table_size > SIZE_MAX) {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		    break;
		}
		return_num_items = fread (charset_lang_codes_table,
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Argument_Enum_Set_Table:
		argument_enumset_table = (unsigned short int *) XtMalloc (header.table_size);
		/* Validate table_size to prevent overflow */
		if (header.table_size > SIZE_MAX) {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		    break;
		}
		return_num_items = fread (argument_enumset_table,
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Related_Argument_Table:
		related_argument_table = (unsigned short int *) XtMalloc (header.table_size);
		/* Validate table_size to prevent overflow */
		if (header.table_size > SIZE_MAX) {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		    break;
		}
		return_num_items = fread (related_argument_table,
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Uil_Gadget_Funcs:
		uil_gadget_variants = (unsigned short int *) XtMalloc (header.table_size);
		/* Validate table_size to prevent overflow */
		if (header.table_size > SIZE_MAX) {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		    break;
		}
		return_num_items = fread (uil_gadget_variants,
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Uil_Urm_Nondialog_Class:
		uil_urm_nondialog_class = (unsigned short int *) XtMalloc (header.table_size);
		/* Validate table_size to prevent overflow */
		if (header.table_size > SIZE_MAX) {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		    break;
		}
		return_num_items = fread (uil_urm_nondialog_class,
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Uil_Urm_Subtree_Resource:
		uil_urm_subtree_resource = (unsigned short int *) XtMalloc (header.table_size);
		/* Validate table_size to prevent overflow */
		if (header.table_size > SIZE_MAX) {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		    break;
		}
		return_num_items = fread (uil_urm_subtree_resource,
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    case Enum_Set_Table:
		db_read_int_and_shorts(&header);
		break;
	    case Enumval_Values_Table:
		enumval_values_table = (int *) XtMalloc (header.table_size);
		/* Validate table_size to prevent overflow */
		if (header.table_size > SIZE_MAX) {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		    break;
		}
		return_num_items = fread (enumval_values_table,
					     header.table_size,
					     1, dbfile);
		_check_read (return_num_items);
		break;
	    default:
		diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
	    } /* end switch */

	/* the table ids that reach here are 1..Uil_Children_Names */
	entries[header.table_id] = db_table_entries (&header);
	} /* end for */

    /*
     * Every table must be present and cover the maxima given in the
     * globals; the compiler does not check the codes it indexes them with.
     */
    for (i = 1; i <= Uil_Children_Names; i++)
	if (entries[i] < db_table_required_entries (&globals, i))
	    diag_issue_diagnostic (d_bad_database,
				   diag_k_no_source,
				   diag_k_no_column);

    fclose (dbfile);
    return;
}



void db_read_ints_and_string(_db_header_ptr header)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in tables of integers and one string unsigned chars and places them into
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Key_Table:
 *	    Key_Table_Case_Ins:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int			return_num_items, i, string_size=0;
	key_keytable_entry_type	*table = NULL;
	char			*string_table;

	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here to protect against bad
	     *	     pointers.
	     */
	    case Key_Table:
		key_table = (key_keytable_entry_type *) XtCalloc (1, header->table_size);
		table = key_table;
		break;
	    case Key_Table_Case_Ins:
		key_table_case_ins = (key_keytable_entry_type *) XtCalloc (1, header->table_size);
		table = key_table_case_ins;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_ints_and_string");
	    }

	/*
	 * Get the entire table with one read.
	 * Then loop through the table and up the length of the strings.
	 * Get all the strings with one read.
	 * Reassign the addresses
	 */
	if (table == NULL) {
	    diag_issue_internal_error("Table not initialized in db_read_ints_and_string");
	    return;
	}
	
	return_num_items = fread(table, header->table_size, 1, dbfile);
	_check_read (return_num_items);

	db_check_table_size (header, 0, sizeof (key_keytable_entry_type));

	for ( i=0 ; i<header->num_items; i++)
	    {
	    /*
	     * Add one for the null character on the string
	     */
	    if (table[i].b_length >= INT_MAX - string_size)
		diag_issue_diagnostic (d_bad_database,
				       diag_k_no_source, diag_k_no_column);
	    string_size += table[i].b_length + 1;
	    };

	string_table = XtMalloc (sizeof (char) * string_size);
	return_num_items = fread(string_table,
				    sizeof(unsigned char) * string_size,
				    1, dbfile);
	_check_read (return_num_items);

	for ( i=0 ; i<header->num_items; i++)
	    {
	    table[i].at_name = string_table;
	    string_table[table[i].b_length] = '\0';
	    string_table +=  table[i].b_length + 1;
	    };

	return;
}



void db_read_char_table(_db_header_ptr header)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in tables of unsigned chars and places them into
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Allowed_Argument_Table:
 *	    Allowed_Child_Table:
 *	    Allowed_Control_Table:
 *	    Allowed_Reason_Table:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	unsigned char	**ptr = NULL;
	int		return_num_items, i;
	unsigned char	*table;
	int		vec_size;

	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here to protect against bad
	     *	     pointers.
	     */
	    case Allowed_Argument_Table:
		allowed_argument_table = (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_argument_table;
		break;
	    case Allowed_Child_Table:
		allowed_child_table =
		  (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_child_table;
		break;
	    case Allowed_Control_Table:
		allowed_control_table = (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_control_table;
		break;
	    case Allowed_Reason_Table:
		allowed_reason_table = (unsigned char **) XtCalloc (1, header->table_size);
		ptr = allowed_reason_table;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_char_table");
	}

	/*
	 * Read the bit vectors one by one and set the addresses.
	 *
	 * The vectors are indexed by object class, 0..uil_max_object.  A
	 * database older than version 3 holds one bit less than that when
	 * uil_max_object is a multiple of 8, so each vector is given the
	 * full size and any bits missing from the file are clear.
	 */
	if (ptr == NULL) {
	    diag_issue_internal_error("Table not initialized in db_read_char_table");
	    return;
	}

	/* ptr[1..num_items] are set below; ptr[0] is not used */
	db_check_table_size (header, 1, sizeof (unsigned char *));

	vec_size = _DB_BIT_VECTOR_SIZE (uil_max_object);
	table = (unsigned char *) XtCalloc (header->num_items, vec_size);
	for ( i=1 ; i<=header->num_items; i++ )
	    {
	    return_num_items = fread(table, sizeof(char) * num_bits, 1, dbfile);
	    _check_read (return_num_items);
	    ptr[i] = table;
	    table += vec_size;
	    };

	return;
}



void db_read_length_and_string(_db_header_ptr header)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in length and strings of unsigned chars and places them into
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Charset_Xmstring_Names_Table:
 *	    Charset_Lang_Names_Table:
 *	    Uil_Widget_Names:
 *	    Uil_Children_Names:
 *	    Uil_Argument_Names:
 *	    Uil_Reason_Names:
 *	    Uil_Enumval_names:
 *	    Uil_Charset_Names:
 *	    Uil_Widget_Funcs:
 *	    Uil_Argument_Toolkit_Names:
 *	    Uil_Reason_Toolkit_Names:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int		return_num_items, i, string_size=0;
	int		*lengths;
	char		*string_table;
	char		**table = NULL;

	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here because it might be possible to
	     *	     have a string of zero length, particularly for the
	     *	     first record. Ergo we Calloc to protect against bad
	     *	     pointers.
	     */
	    case Charset_Xmstring_Names_Table:
		charset_xmstring_names_table = (char **) XtCalloc (1, header->table_size);
		table = charset_xmstring_names_table;
		break;
	    case Charset_Lang_Names_Table:
		charset_lang_names_table = (char **) XtCalloc (1, header->table_size);
		table = charset_lang_names_table;
		break;
	    case Uil_Widget_Names:
		uil_widget_names = (char **) XtCalloc (1, header->table_size);
		table = uil_widget_names ;
		break;
	    case Uil_Children_Names:
		uil_child_names = (char **) XtCalloc (1, header->table_size);
		table = uil_child_names ;
		break;
	    case Uil_Argument_Names:
		uil_argument_names = (char **) XtCalloc (1, header->table_size);
		table = uil_argument_names;
		break;
	    case Uil_Reason_Names:
		uil_reason_names = (char **) XtCalloc (1, header->table_size);
		table = uil_reason_names;
		break;
	    case Uil_Enumval_names:
		uil_enumval_names = (char **) XtCalloc (1, header->table_size);
		table = uil_enumval_names;
		break;
	    case Uil_Charset_Names:
		uil_charset_names = (char **) XtCalloc (1, header->table_size);
		table = uil_charset_names;
		break;
	    case Uil_Widget_Funcs:
		uil_widget_funcs = (char **) XtCalloc (1, header->table_size);
		table = uil_widget_funcs;
		break;
	    case Uil_Argument_Toolkit_Names:
		uil_argument_toolkit_names = (char **) XtCalloc (1, header->table_size);
		table = uil_argument_toolkit_names;
		break;
	    case Uil_Reason_Toolkit_Names:
		uil_reason_toolkit_names = (char **) XtCalloc (1, header->table_size);
		table = uil_reason_toolkit_names;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_length_and_string");
	    }

	/*
	 * Get the lengths of all the strings with one read.
	 * Then loop through the table and up the length of the strings.
	 * Get all the strings with one read.
	 * Reassign the addresses using the length table and string table.
	 * Cleanup by Freeing length table.
	 *
	 * NOTE: In some tables the counting starts at 1 not 0 so you
	 *	 have to be carefull.
	 */

	if (table == NULL) {
	    diag_issue_internal_error("Table not initialized in db_read_length_and_string");
	    return;
	}

	/* table[0..num_items] are set below */
	db_check_table_size (header, 1, sizeof (char *));

	lengths = (int *) XtMalloc (sizeof (int) * ((size_t) header->num_items + 1));
	return_num_items = fread(lengths,
				    sizeof(int) * ((size_t) header->num_items + 1),
				    1, dbfile);
	_check_read (return_num_items);
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    /*
	     * Add one for the null terminator
	     */
	    if (lengths[i] < 0 || lengths[i] >= INT_MAX - string_size)
		diag_issue_diagnostic (d_bad_database,
				       diag_k_no_source, diag_k_no_column);
	    if (lengths[i] > 0)
		string_size += lengths[i] + 1;
	    }

	string_table = XtMalloc (sizeof (unsigned char) * string_size);
	return_num_items = fread(string_table,
				    sizeof(unsigned char) * string_size,
				    1, dbfile);
	_check_read (return_num_items);

	for ( i=0 ; i<=header->num_items; i++)
	    {
	    if (lengths[i] > 0)
		{
		/* Ensure string is null-terminated */
		string_table[lengths[i]] = '\0';
		table[i] = string_table;
/* BEGIN HaL Fix CR 5618 */
		  if ((header->table_id == Uil_Widget_Names) &&
		      (strcmp(table[i], "user_defined") == 0))
		    uil_sym_user_defined_object = i;
/* END HaL Fix CR 5618 */
		string_table +=  lengths[i] + 1;
		}
	    }

	XtFree ((char *)lengths);

	return;
}



void db_read_int_and_shorts(_db_header_ptr header)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in a structure consisting of one integer and a
 *	pointer to a table of integer and places them into
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Enum_Set_Table:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int			return_num_items, i, int_table_size=0;
	UilEnumSetDescDef	*table = NULL;
	unsigned short int 	*int_table;

	switch (header->table_id)
	    {
	    case Enum_Set_Table:
		enum_set_table = (UilEnumSetDescDef *) XtCalloc (1, header->table_size);
		table = enum_set_table;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_int_shorts");
	    }

	/*
	 * Get the entire table with one read.
	 * Then loop through the table and add up the number of ints in each int table.
	 * Get all the integer tables with one read.
	 * Reassign the addresses of the tables.
	 */
	if (table == NULL) {
	    diag_issue_internal_error("Table not initialized in db_read_int_and_shorts");
	    return;
	}
	
	return_num_items = fread(table, header->table_size, 1, dbfile);
	_check_read (return_num_items);
	
	/* table[0..num_items] are used below */
	db_check_table_size (header, 1, sizeof (UilEnumSetDescDef));

	for ( i=0 ; i<=header->num_items; i++)
	    {
	    if (table[i].values_cnt < 0 ||
		(size_t) table[i].values_cnt >
		INT_MAX / sizeof (short) - (size_t) int_table_size)
		diag_issue_diagnostic (d_bad_database,
				       diag_k_no_source, diag_k_no_column);
	    int_table_size += table[i].values_cnt;
	    }

	int_table = (unsigned short int *) XtCalloc (1, sizeof (short) * int_table_size);
	return_num_items = fread(int_table,
				    sizeof(short) * int_table_size,
				    1, dbfile);
	_check_read (return_num_items);

	for ( i=0 ; i<=header->num_items; i++)
	    {
	    /* the pointer read from the file is meaningless */
	    table[i].values = NULL;
	    if (table[i].values_cnt)
		{
		table[i].values = int_table;
		int_table += table[i].values_cnt;
		}
	    }

	return;
}



void db_open_file (void)

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine opens the binary database file in a platform-dependent way,
 *	performing i18n language switching in order to do so.
 *
 *	Per the latest agreement on semantics, this routine does:
 *		- first, try to open in the local directory (that is, with
 *		  no switching).
 *		- second, try language switching and open
 *
 *  FORMAL PARAMETERS:
 *
 *	name		A system-dependent string specifying the IDB file
 *			to be opened.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	char			*resolvedname;		/* current resolved name */
	SubstitutionRec		subs[3];
	char			*wmdPath;
	size_t			len;

	/*
	 * Use XtFindFile instead of XtResolvePathName. XtResolvePathName requires a
	 * display which UIL doesn't have. At the current time there is no support for
	 * $LANG in the path string. If such support was deamed necessary, the %L, %l,
	 * %t, %c values would be set up as subs here using globals from the fetch of
	 * LANG variable used to determine the default codeset (or vice versa depending
	 * on which is called first)
	 *
	 * If the last 4 characters of the file name are not .bdb
	 * then pass in the suffix of .bdb. If a file isn't found with the suffix passed
	 * in then try without the suffix.
	 */

	/*
	 * Make sure 'S' is the last one so we can remove the suffix for the first pass.
	 */
	subs[0].match = 'N';
	subs[0].substitution = Uil_cmd_z_command.ac_database;
	subs[1].match = 'T';
	subs[1].substitution = "wmd";
	subs[2].match = 'S';
	subs[2].substitution = ".wmd";

	wmdPath = init_wmd_path(Uil_cmd_z_command.ac_database);

	resolvedname = 0;

	/*
	 * Check and see if the .wmd suffix is already on the file. If not then try to
	 * resolve the pathname with .wmd suffix first. If that fails or the suffix is
	 * already on the file then just try to resolve the pathname.
	 */
	len = strlen (Uil_cmd_z_command.ac_database);
	if ( len < 4 ||
	     strcmp (&Uil_cmd_z_command.ac_database[len - 4], ".wmd") != 0 )
		resolvedname = XtFindFile(wmdPath,
					      subs,
					      XtNumber(subs),
					      (XtFilePredicate)NULL);

	/*
	 * No .wmd suffix or a failure to resolve the pathname with the .wmd suffix
	 * Try without the suffix.
	 */
	subs[2].substitution = "";
	if (resolvedname == 0)
		resolvedname = XtFindFile(wmdPath,
					      subs,
					      XtNumber(subs),
					      (XtFilePredicate)NULL);

	if (resolvedname == 0)
	    {
	    diag_issue_diagnostic( d_wmd_open,
				   diag_k_no_source, diag_k_no_column,
				   Uil_cmd_z_command.ac_database);
	    return;  /* Exit early if no resolved name */
	    }

	dbfile = fopen (resolvedname, "r");

	/* If the file is not found, a fatal error is generated.	*/
	if (dbfile == NULL)
	    {
	    diag_issue_diagnostic( d_src_open,
				   diag_k_no_source, diag_k_no_column,
				   resolvedname);
	    }

	return;
}




String get_root_dir_name(void)
{
	int uid;
	struct passwd *pwd_value;
	static char *ptr = NULL;
	char *outptr;
	size_t size;

	if (ptr == NULL)
	{
	if((ptr = (char *)getenv("HOME")) == NULL)
	    {
	    if((ptr = (char *)getenv(USER_VAR)) != NULL)
		{
		pwd_value = getpwnam(ptr);
		}
	    else
		{
		uid = getuid();
		pwd_value = getpwuid(uid);
		}
	    if (pwd_value != NULL)
		{
		ptr = pwd_value->pw_dir;
		}
	    else
		{
		 ptr = "";
		}
	    }
	}

	size = strlen(ptr) + 2;
	outptr = XtMalloc (size);
	snprintf (outptr, size, "%s/", ptr);
	return outptr;
}

/*
 * XAPPLRES_DEFAULT and UIDPATH_DEFAULT are intentionally split to support
 * SCCS. DO NOT reformat the lines else %-N-%-S could be converted by SCCS into
 * something totally bizarre causing MrmOpenHierarchy failures.
 */

/* The following are usually defined in the Makefile */

#ifndef LIBDIR
#define LIBDIR "/usr/lib/X11"
#endif
#ifndef INCDIR
#define INCDIR "/usr/include/X11"
#endif

static char libdir[] = LIBDIR;
static char incdir[] = INCDIR;

static char XAPPLRES_DEFAULT[] = "\
%%N\
%%S:\
%s/%%T/%%N\
%%S:\
%s%%T/%%N\
%%S:\
%s%%N\
%%S:\
%s/%%T/%%N\
%%S:\
%s/%%T/%%N\
%%S";

static char WMDPATH_DEFAULT[] = "\
%%N\
%%S:\
%s%%T/%%N\
%%S:\
%s%%N\
%%S:\
%s/%%L/%%T/%%N\
%%S:\
%s/%%T/%%N\
%%S";

static char ABSOLUTE_PATH[] = "\
%N\
%S";

String init_wmd_path(String filename)
{
    String path;
    String old_path;
    String homedir;
    String wmd_path;
    size_t size;


    if (filename[0] == '/')
	{
	wmd_path = XtNewString(ABSOLUTE_PATH);
	}
    else
	{
	path = (char *)getenv ("WMDPATH");
	if (path  == NULL)
	    {
	    homedir = get_root_dir_name();
	    old_path = (char *)getenv ("XAPPLRESDIR");
	    if (old_path == NULL)
		{
		size = 2*strlen(homedir) + strlen(libdir) + strlen(incdir) +
		       strlen(WMDPATH_DEFAULT);
		wmd_path = XtCalloc(1, size);
		snprintf( wmd_path, size, WMDPATH_DEFAULT,
			 homedir, homedir, libdir, incdir);
		}
	    else
		{
		size = 1*strlen(old_path) + 2*strlen(homedir) +
		       strlen(libdir) + strlen(incdir) + strlen(XAPPLRES_DEFAULT);
		wmd_path = XtCalloc(1, size);
		snprintf(wmd_path, size, XAPPLRES_DEFAULT,
			old_path,
			homedir, homedir, libdir, incdir);
		}
	    XtFree (homedir);
	    }
	else
	    {
	    wmd_path = XtNewString(path);
	    }
	}
    return (wmd_path);
}
