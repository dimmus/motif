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
static char rcsid[] = "$TOG: uilsymdump.c /main/6 1998/04/17 11:26:22 mgreess $"
#endif
#endif

#include <uil/UilDef.h>
#include <stdio.h>
#include <ctype.h>

/*
**
** Table of Contents
**
*/
Uil_continue_type MessageCB(void);
Uil_continue_type StatusCB(void);


/*
** globals
*/
FILE	*source_file;


/*
**  Entry point
*/
int
main(int argc, char *argv[])
{
    Uil_command_type	    command_desc;
    Uil_compile_desc_type   compile_desc;
    Uil_status_type	    return_status;
    int			    x = 1, y = 1;
    char		    *include_list[3];
    char		    file_name[255], listing_name[255], uid_name[255];

    /*
    ** Ask for the file name.
    */
    printf("Enter file to be compiled (leave off the .uil): ");

    *file_name = '\0';
    fgets (file_name, sizeof(file_name), stdin);
    if (strlen(file_name) && file_name[strlen(file_name)-1] == '\n')
      file_name[strlen(file_name)-1] = '\0';

    strcpy (uid_name, file_name);	strcat (uid_name, ".uid");
    strcpy (listing_name, file_name);	strcat (listing_name, ".lis");
    strcat (file_name, ".uil");

    /*
    **  Fill in the command decs
    */
    command_desc.resource_file = uid_name;
    command_desc.listing_file = listing_name;
    command_desc.include_dir_count = 2;
    command_desc.include_dir = (char **)include_list;
    command_desc.source_file = file_name;
    include_list[0] = "com:";
    include_list[1] = "../include";


    command_desc.listing_file_flag = 1;
    command_desc.resource_file_flag = 1;
    command_desc.machine_code_flag = 1;
    command_desc.report_info_msg_flag = 1;
    command_desc.report_warn_msg_flag = 1;
    command_desc.parse_tree_flag = 1;
    command_desc.status_update_delay = 400;

    return_status = Uil(&command_desc, &compile_desc, MessageCB, (char *)&x,
			StatusCB, (char *)&y);

    UilDumpSymbolTable( (sym_entry_type *)compile_desc.parse_tree_root );
    return return_status;
}

Uil_continue_type MessageCB(void)
{
    /* Simple callback - just return continue */
    return Uil_k_continue;
}



Uil_continue_type StatusCB(void)
{
    /* Simple callback - just return continue */
    return Uil_k_continue;
}
