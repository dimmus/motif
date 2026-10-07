%{
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

#include <string.h>
#include <stdlib.h>

#ifndef XmConst
#define XmConst const
#endif /* XmConst */

	/*
	 * lex program to construct token data for WML to generate token
	 * table data.
 	 *
	 * stdin is the file Uil.y
	 *
	 * it is searched for each occurance of a line starting with '%token'
	 * when found various fields are extracted.
	 *
	 * when Uil.y has been scanned the collected data is written
	 * to stdout.
	 */

%}

%a 9999						/* transitions */

%{
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _DEFAULT_SOURCE
# define _DEFAULT_SOURCE
#endif
#ifndef _GNU_SOURCE
 #define _GNU_SOURCE
#endif
#ifndef _BSD_SOURCE
# define _BSD_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>

/* Explicit declaration for fileno if not available */
#ifndef fileno
extern int fileno(FILE *stream);
#endif

#define TRUE  1
#define FALSE 0

#define MAX_SYN 10

#ifdef yywrap
#undef yywrap
#endif

typedef struct _token
{
    char * name;
    char * id;
    char * class;
    int    num_syn;
    char * syn[MAX_SYN];
}
    TokenRec, *Token;

TokenRec token[1000];
TokenRec temp;

/* A malloc'd copy of text (the current token). */
static char *copy_text (const char *text)
{
    size_t size = strlen (text) + 1;
    char *copy = (char *) malloc (size);

    if (copy == NULL)
    {
	fputs ("wmluiltok: out of memory\n", stderr);
	exit (1);
    }
    return (char *) memcpy (copy, text, size);
}

void add_token (Token t);
int yywrap (void);

int phase		= 0;
int keyword_count;
int line_num		= 0;
int in_comment		= 0;
int in_include		= 0;
int in_token		= 0;
int state		= 0;
int used		= 0;

%}

%option noinput nounput

%%


[\n]		{
		    if (in_token == TRUE)
		    {
			add_token (&temp);
		    }

		    in_token = FALSE;
		    line_num++;
		}

[ \t]		{				/* swallow */
		} 


"/\*"		{				/* swallow */
		}


^%token		{
		    in_token = TRUE;
		    state = 0;
		}

[a-zA-Z0-9_]+	{
		    if (in_token == TRUE)
		    {
			switch (state)
			{
			    case 0:
				temp.name = copy_text ((XmConst char *)yytext);

				temp.num_syn = 0; 
				break;

			    case 1:
				temp.id = copy_text ((XmConst char *)yytext);
				break;

			    case 2:
				temp.class = copy_text ((XmConst char *)yytext);
				break;

			    case 3:
				if (temp.num_syn >= MAX_SYN)
				{
				    printf (
					"ERROR, too many synonyms, line %d\n",
					line_num);
				    break;
				}
				temp.syn[temp.num_syn] =
				    copy_text ((XmConst char *)yytext);
				temp.num_syn++;
				break;
			}

			state++;
		    }
		}

.		{				/* swallow */
		}

%%


void
add_token(Token t)
{
    int i, j;

    for (j=0; j<used; j++)			/* for each token */
    {
	if (strcmp (token[j].name, t->name) > 0)	/* goes here */
  	{
	    for (i=used++; i>j; i--)		/* make hole */
		token[i] = token[i-1];

	    token[j] = *t;			/* insert it */

	    return;
	}
    }

    /*
     * if we get there then it goes at the end of the list 
     */

    token[used++] = *t;
}



int yywrap(void)
{
    int i;

    for (i=0; i<used; i++)
	 { 
	 printf ("%s %s %s\n", token[i].name, token[i].id, token[i].class);
	 }
    return (1);
}