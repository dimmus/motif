/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_WMLPARSE_H_INCLUDED
# define YY_YY_WMLPARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    CLASS = 1,                     /* CLASS  */
    RESOURCE = 2,                  /* RESOURCE  */
    DATATYPE = 3,                  /* DATATYPE  */
    CONTROLLIST = 4,               /* CONTROLLIST  */
    ENUMERATIONSET = 5,            /* ENUMERATIONSET  */
    ENUMERATIONVALUE = 6,          /* ENUMERATIONVALUE  */
    CHARACTERSET = 7,              /* CHARACTERSET  */
    CHILD = 50,                    /* CHILD  */
    DOCNAME = 8,                   /* DOCNAME  */
    CONVFUNC = 9,                  /* CONVFUNC  */
    ALIAS = 10,                    /* ALIAS  */
    TYPE = 11,                     /* TYPE  */
    RESOURCELITERAL = 12,          /* RESOURCELITERAL  */
    RELATED = 13,                  /* RELATED  */
    INTERNALLITERAL = 14,          /* INTERNALLITERAL  */
    XRMRESOURCE = 15,              /* XRMRESOURCE  */
    EXCLUDE = 16,                  /* EXCLUDE  */
    RESOURCES = 17,                /* RESOURCES  */
    SUPERCLASS = 18,               /* SUPERCLASS  */
    CONTROLS = 19,                 /* CONTROLS  */
    WIDGETCLASS = 20,              /* WIDGETCLASS  */
    DIALOGCLASS = 21,              /* DIALOGCLASS  */
    DEFAULT = 22,                  /* DEFAULT  */
    ENUMLITERAL = 23,              /* ENUMLITERAL  */
    XMSTRINGCHARSETNAME = 24,      /* XMSTRINGCHARSETNAME  */
    DIRECTION = 25,                /* DIRECTION  */
    PARSEDIRECTION = 26,           /* PARSEDIRECTION  */
    CHARACTERSIZE = 27,            /* CHARACTERSIZE  */
    CTRLMAPSRESOURCE = 28,         /* CTRLMAPSRESOURCE  */
    CHILDREN = 51,                 /* CHILDREN  */
    PARENTCLASS = 52,              /* PARENTCLASS  */
    METACLASS = 29,                /* METACLASS  */
    WIDGET = 30,                   /* WIDGET  */
    GADGET = 31,                   /* GADGET  */
    ARGUMENT = 32,                 /* ARGUMENT  */
    REASON = 33,                   /* REASON  */
    CONSTRAINT = 34,               /* CONSTRAINT  */
    SUBRESOURCE = 35,              /* SUBRESOURCE  */
    ATTRTRUE = 36,                 /* ATTRTRUE  */
    ATTRFALSE = 37,                /* ATTRFALSE  */
    LEFTTORIGHT = 38,              /* LEFTTORIGHT  */
    RIGHTTOLEFT = 39,              /* RIGHTTOLEFT  */
    ONEBYTE = 40,                  /* ONEBYTE  */
    TWOBYTE = 41,                  /* TWOBYTE  */
    MIXED1_2BYTE = 42,             /* MIXED1_2BYTE  */
    COLON = 43,                    /* COLON  */
    SEMICOLON = 44,                /* SEMICOLON  */
    EQUALS = 45,                   /* EQUALS  */
    LBRACE = 46,                   /* LBRACE  */
    RBRACE = 47,                   /* RBRACE  */
    STRING = 48,                   /* STRING  */
    ERRORTOKEN = 49                /* ERRORTOKEN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define CLASS 1
#define RESOURCE 2
#define DATATYPE 3
#define CONTROLLIST 4
#define ENUMERATIONSET 5
#define ENUMERATIONVALUE 6
#define CHARACTERSET 7
#define CHILD 50
#define DOCNAME 8
#define CONVFUNC 9
#define ALIAS 10
#define TYPE 11
#define RESOURCELITERAL 12
#define RELATED 13
#define INTERNALLITERAL 14
#define XRMRESOURCE 15
#define EXCLUDE 16
#define RESOURCES 17
#define SUPERCLASS 18
#define CONTROLS 19
#define WIDGETCLASS 20
#define DIALOGCLASS 21
#define DEFAULT 22
#define ENUMLITERAL 23
#define XMSTRINGCHARSETNAME 24
#define DIRECTION 25
#define PARSEDIRECTION 26
#define CHARACTERSIZE 27
#define CTRLMAPSRESOURCE 28
#define CHILDREN 51
#define PARENTCLASS 52
#define METACLASS 29
#define WIDGET 30
#define GADGET 31
#define ARGUMENT 32
#define REASON 33
#define CONSTRAINT 34
#define SUBRESOURCE 35
#define ATTRTRUE 36
#define ATTRFALSE 37
#define LEFTTORIGHT 38
#define RIGHTTOLEFT 39
#define ONEBYTE 40
#define TWOBYTE 41
#define MIXED1_2BYTE 42
#define COLON 43
#define SEMICOLON 44
#define EQUALS 45
#define LBRACE 46
#define RBRACE 47
#define STRING 48
#define ERRORTOKEN 49

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_WMLPARSE_H_INCLUDED  */
