/* $XConsortium: mkmsgcat.c /main/1 1996/06/05 18:21:57 drk $ */
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
char buf[1024];

char *OSF_COPYRIGHT = "$\n\
$ Motif\n\
$\n\
$ Copyright (c) 1987-2012, The Open Group. All rights reserved.\n\
$\n\
$ These libraries and programs are free software; you can\n\
$ redistribute them and/or modify them under the terms of the GNU\n\
$ Lesser General Public License as published by the Free Software\n\
$ Foundation; either version 2 of the License, or (at your option)\n\
$ any later version.\n\
$\n\
$ These libraries and programs are distributed in the hope that\n\
$ they will be useful, but WITHOUT ANY WARRANTY; without even the\n\
$ implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\n\
$ PURPOSE. See the GNU Lesser General Public License for more\n\
$ details.\n\
$\n\
$ You should have received a copy of the GNU Lesser General Public\n\
$ License along with these librararies and programs; if not, write\n\
$ to the Free Software Foundation, Inc., 51 Franklin Street, Fifth\n\
$ Floor, Boston, MA 02110-1301 USA\n\
$";

char *OSF_COPYRIGHT_C = "/*\n\
 *\n\
 * Motif\n\
 *\n\
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.\n\
 *\n\
 * These libraries and programs are free software; you can\n\
 * redistribute them and/or modify them under the terms of the GNU\n\
 * Lesser General Public License as published by the Free Software\n\
 * Foundation; either version 2 of the License, or (at your option)\n\
 * any later version.\n\
 *\n\
 * These libraries and programs are distributed in the hope that\n\
 * they will be useful, but WITHOUT ANY WARRANTY; without even the\n\
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\n\
 * PURPOSE. See the GNU Lesser General Public License for more\n\
 * details.\n\
 *\n\
 * You should have received a copy of the GNU Lesser General Public\n\
 * License along with these librararies and programs; if not, write\n\
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth\n\
 * Floor, Boston, MA 02110-1301 USA\n\
 */\n";

static void parse_args(
           int argc, char **argv, char **prefix, char **source_file_name);
static FILE *open_output(const char *name);

/****************************************************************************
 *
 * This utility should be run with 2 arguments. The first one is used as a
 * prefix to generate the names of the output files, as well as the contents.
 * The second name is the full path of the source file to use.
 *
 * Running like this:
 *       mkmsgcat -Xm /proj/motif/lib/Xm/Messages.c
 *
 * it will open the /proj/motif/lib/Xm/Messages.c file and generate 2 output
 * files:
 *     - Xm.msg
 *     - XmMsgI.h
 *
 * The externaldef message names are expected to be of the following form
 * in the source file, and not be longer than 10 characters:
 *     _<prefix>Msg<name>     for example (_XmMsgBulletinB_0001, _MrmMsg_0000)
 *
 ***************************************************************************/

int
main(
       int argc,
       char **argv )
{
    FILE *header = NULL, *catalog = NULL, *source = NULL;
    int stars, i;
    char *p, *q, setname[32], messagename[64];
    char *prefix = NULL;
    char *source_file_name = NULL;
    char header_name[32], catalog_name[32];

    /* parse command line and get the prefix and source file name to use. */
    parse_args(argc, argv, &prefix, &source_file_name);

    if (prefix == NULL || source_file_name == NULL) {
	fprintf(stderr, "Usage: mkmsgcat -<prefix> <source_file_name>\n");
	exit(1);
    }

    /* subs below holds "_<prefix>Msg" in 11 bytes. */
    if (strlen(prefix) > 6) {
	fprintf(stderr, "Prefix too long: %s\n", prefix);
	exit(1);
    }

    snprintf(header_name, sizeof header_name, "%sMsgI.h", prefix);

    header = open_output(header_name);

    snprintf(catalog_name, sizeof catalog_name, "%s.msg", prefix);

    catalog = open_output(catalog_name);

    source = fopen(source_file_name, "r");

    if (!catalog) {
      fprintf(stderr, "Cannot write message catalog, exiting!\n");
      exit(1);
    }
    if (!header) {
      fprintf(stderr, "Cannot write header file, exiting!\n");
      exit(1);
    }
    if (!source) {
      fprintf(stderr, "Cannot read source file, exiting!\n");
      exit(1);
    }
    fprintf(catalog, "$ $%s$\n", "XConsortium");
    fprintf(catalog, "%s\n", OSF_COPYRIGHT);
    fprintf(catalog, "$ This file is automatically generated. \n");
    fprintf(catalog, "$ Do not edit. \n");
    fprintf(catalog, "\n");
    if (header) {
      fprintf(header, "/* $%s$ */\n", "XConsortium");
      fprintf(header, "%s", OSF_COPYRIGHT_C);
      fprintf(header, "/* This file is automatically generated. */\n");
      fprintf(header, "/* Do not edit. */\n\n");
      fprintf(header, "#if XM_MSGCAT\n\n");
      fprintf(header, "#include <nl_types.h>\n#include \"%sMsgCatI.h\"\n\n",
	      prefix);
      fprintf(header, "externalref nl_catd %s_catd;\n\n", prefix);
      fprintf(header, "\
#define _%sCatgets(catd, set_num, msg_num, def_str) \\\n\
        catgets(catd, set_num, msg_num, def_str)\n\n", prefix);
      fprintf(header, "#else\n\n");
      fprintf(header,
	      "#define _%sCatgets(catd, set_num, msg_num, def_str)   def_str\n\n",
	      prefix);
      fprintf(header, "#endif /* NO_MESSAGE_CATALOG */\n\n");
    }
    while (fgets(buf, 1024, source)) {
      if (!buf[0])
	continue;
      p = buf;
      while (isspace(*p)) p++;
      if (*p == '/') {
	/*
	 * Start of a comment. If there is at least 3 '*' following the '/',
         * this is information for the message catalog.
	 */
	for (stars = 0, p++;
	     *p && *p == '*';
	     stars++, p++);
	if (stars < 3)
	  continue;
	while (isspace(*p)) p++;
	if (*p == '+') {
	  /*
	   * Extra message catalog text.
	   * (ex. messages obsoleted in the code, but needed
	   * for binary BC, comments)
	   */
	  p++;
	  for (q = p; *q && *q != '\n' && *q != '*'; q++);
	  *q = '\0';
	  fprintf(catalog, "%s\n", p);
	  continue;
	} else {
	  /*
	   * This is the set name
	   */
	  for (i = 0;
	       isalpha(*p) && i < 31;
	       i++, p++)
	    setname[i] = *p;
	  setname[i] = '\0';
	  fprintf(catalog, "\n$set MS_%s\n\n", setname);
	  continue;
	}
      }
      if (*p == 'e') {
	char subs[11];
	int len_subs;
	/*
	 * This is a message.
	 */
	if (strncmp(p, "externaldef", 11) != 0)
	  continue;

	/* Find the substring to look for, based on the prefix. */
	snprintf(subs, sizeof subs, "_%sMsg", prefix);
	len_subs = strlen(subs);

	p = strstr(p, subs);
	if (!p) continue;
	p += len_subs;
	for (i = 0;
	     !isspace(*p) && i < 63;
	     i++, p++)
	  messagename[i] = *p;
	messagename[i] = '\0';
	p = strchr(p, '"');
	if (!p) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	  do
	    fgets(buf, 1024, source);
	  while (!buf[0]);
#pragma GCC diagnostic pop
	  p = strchr(buf, '"');
	}
	if (!p)
	  continue;
	fprintf(catalog, "MSG_%s ", messagename);
	p++;
	do {
	  q = strchr(p, '"');
	  while (q && *(--q) == '\\') {
	    q += 2;
	    q = strchr(q, '"');
	  }
	  if (q) {
	    q++;
	    *q = '\0';
	    fprintf(catalog, "%s\n", p);
	  } else {
	    fprintf(catalog, "%s", p);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	    fgets(buf, 1024, source);
#pragma GCC diagnostic pop
	    p = buf;
	  }
	} while (!q);
	if (header)
	  fprintf(header,
		  "#define _%sMMsg%s%*s _%sCatgets(%s_catd, MS_%s, MSG_%s, _%sMsg%s)\n",
		  prefix, messagename, 20-i, " ", prefix, prefix, setname,
		  messagename, prefix, messagename);
      }
    }

    return 0;
}


static void parse_args(
       int argc,
       char **argv,
       char **prefix,
       char **source_file_name)
{

    if (argc < 3) {
	fprintf(stderr, "Usage: mkmsgcat -<prefix> <source_file_name>\n");
	exit(1);
    }

    /* Skip the first argument since it is the program name. */
    argv++;
    argc--;

    /* This argument should be the prefix, preceded with a '-'. */
    if (strncmp(*argv, "-", 1)) {
	fprintf(stderr, "Usage: mkmsgcat -<prefix> <source_file_name>\n");
	exit(1);
    }
    else {
	/* Skip the '-' */
	(*argv)++;
	*prefix = *argv;

	/* Now get the source file name. */
	argv++;
	argc--;
	*source_file_name = *argv;
    }

}


/* Like fopen(name, "w"), but create the file with mode 0644. */
static FILE *open_output(const char *name)
{
    FILE *fp;
    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0)
	return NULL;
    if ((fp = fdopen(fd, "w")) == NULL)
	close(fd);
    return fp;
}
