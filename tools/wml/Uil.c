/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 36 "Uil.y"



	/* Begin user declarations section */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "UilDefI.h"
#include "UilCompGl.h"

#define		YYSTYPE		yystype

#define		YYDEBUG		1

/*   Declare and initialize stack entry for epsilon productions.    */

YYSTYPE		gz_yynullval = {NULL,0,0,0,0};

/*   Declare token to aid in saving source context	*/

YYSTYPE		prev_yylval;

	/* End user declarations section */


#line 97 "Uil.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_UIL_H_INCLUDED
# define YY_YY_UIL_H_INCLUDED
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
    UILEOF = 0,                    /* UILEOF  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NAME = 1,                      /* NAME  */
    FONT_NAME = 2,                 /* FONT_NAME  */
    COLOR_NAME = 3,                /* COLOR_NAME  */
    ARGUMENT_NAME = 4,             /* ARGUMENT_NAME  */
    REASON_NAME = 5,               /* REASON_NAME  */
    CHARSET_NAME = 6,              /* CHARSET_NAME  */
    ENUMVAL_NAME = 7,              /* ENUMVAL_NAME  */
    CLASS_NAME = 8,                /* CLASS_NAME  */
    CHILD_NAME = 96,               /* CHILD_NAME  */
    UNS_FLOAT_LITERAL = 9,         /* UNS_FLOAT_LITERAL  */
    COMP_STRING = 10,              /* COMP_STRING  */
    CHAR_8_LITERAL = 11,           /* CHAR_8_LITERAL  */
    UNS_INT_LITERAL = 12,          /* UNS_INT_LITERAL  */
    LEFT_PAREN = 13,               /* LEFT_PAREN  */
    RIGHT_PAREN = 14,              /* RIGHT_PAREN  */
    COLON = 15,                    /* COLON  */
    SEMICOLON = 16,                /* SEMICOLON  */
    LEFT_BRACE = 17,               /* LEFT_BRACE  */
    RIGHT_BRACE = 18,              /* RIGHT_BRACE  */
    COMMA = 19,                    /* COMMA  */
    EQUAL_SIGN = 20,               /* EQUAL_SIGN  */
    NOT = 21,                      /* NOT  */
    PLUS = 22,                     /* PLUS  */
    MINUS = 23,                    /* MINUS  */
    AND = 24,                      /* AND  */
    OR = 25,                       /* OR  */
    XOR = 26,                      /* XOR  */
    MULTIPLY = 27,                 /* MULTIPLY  */
    DIVIDE = 28,                   /* DIVIDE  */
    LEFT_SHIFT = 29,               /* LEFT_SHIFT  */
    RIGHT_SHIFT = 30,              /* RIGHT_SHIFT  */
    LIST = 31,                     /* LIST  */
    IDENTIFIER = 32,               /* IDENTIFIER  */
    END = 33,                      /* END  */
    MODULE = 34,                   /* MODULE  */
    UILTRUE = 35,                  /* UILTRUE  */
    UILFALSE = 36,                 /* UILFALSE  */
    INCLUDE = 37,                  /* INCLUDE  */
    MACRO = 38,                    /* MACRO  */
    ON = 39,                       /* ON  */
    OFF = 40,                      /* OFF  */
    VALUE = 41,                    /* VALUE  */
    ARGUMENTS = 42,                /* ARGUMENTS  */
    CALLBACKS = 43,                /* CALLBACKS  */
    PROCEDURES = 44,               /* PROCEDURES  */
    CONTROLS = 45,                 /* CONTROLS  */
    PROCEDURE = 46,                /* PROCEDURE  */
    OBJECT = 47,                   /* OBJECT  */
    OBJECTS = 48,                  /* OBJECTS  */
    WIDGET = 49,                   /* WIDGET  */
    GADGET = 50,                   /* GADGET  */
    FONT = 51,                     /* FONT  */
    ARGUMENT = 52,                 /* ARGUMENT  */
    REASON = 53,                   /* REASON  */
    PIXMAP = 54,                   /* PIXMAP  */
    COLOR = 55,                    /* COLOR  */
    NAMES = 56,                    /* NAMES  */
    CHARACTER_SET = 57,            /* CHARACTER_SET  */
    CASE_SENSITIVE = 58,           /* CASE_SENSITIVE  */
    CASE_INSENSITIVE = 59,         /* CASE_INSENSITIVE  */
    VERSION = 60,                  /* VERSION  */
    MANAGED = 61,                  /* MANAGED  */
    UNMANAGED = 62,                /* UNMANAGED  */
    PRIVATE = 63,                  /* PRIVATE  */
    IMPORTED = 64,                 /* IMPORTED  */
    EXPORTED = 65,                 /* EXPORTED  */
    UILFILE = 66,                  /* UILFILE  */
    STRING_TABLE = 67,             /* STRING_TABLE  */
    TRANSLATION_TABLE = 68,        /* TRANSLATION_TABLE  */
    COMPOUND_STRING = 69,          /* COMPOUND_STRING  */
    FONT_TABLE = 70,               /* FONT_TABLE  */
    ANY = 71,                      /* ANY  */
    STRING = 72,                   /* STRING  */
    BOOLEAN = 73,                  /* BOOLEAN  */
    ICON = 74,                     /* ICON  */
    RIGHT_TO_LEFT = 75,            /* RIGHT_TO_LEFT  */
    BACKGROUND = 76,               /* BACKGROUND  */
    FOREGROUND = 77,               /* FOREGROUND  */
    COLOR_TABLE = 78,              /* COLOR_TABLE  */
    FLOAT = 79,                    /* FLOAT  */
    INTEGER = 80,                  /* INTEGER  */
    CLASS_REC_NAME = 81,           /* CLASS_REC_NAME  */
    ASCIZ_TABLE = 82,              /* ASCIZ_TABLE  */
    INTEGER_TABLE = 83,            /* INTEGER_TABLE  */
    ASCIZ_STRING_TABLE = 84,       /* ASCIZ_STRING_TABLE  */
    COMPOUND_STRING_TABLE = 85,    /* COMPOUND_STRING_TABLE  */
    XBITMAPFILE = 86,              /* XBITMAPFILE  */
    SEPARATE = 87,                 /* SEPARATE  */
    SIXTEEN_BIT = 88,              /* SIXTEEN_BIT  */
    POUND = 89,                    /* POUND  */
    KEYSYM = 90,                   /* KEYSYM  */
    SINGLE_FLOAT = 91,             /* SINGLE_FLOAT  */
    RGB = 92,                      /* RGB  */
    WIDE_CHARACTER = 93,           /* WIDE_CHARACTER  */
    LOC_STRING = 94,               /* LOC_STRING  */
    FONTSET = 95,                  /* FONTSET  */
    COMPOUND_STRING_COMPONENT = 97, /* COMPOUND_STRING_COMPONENT  */
    PIX = 115,                     /* PIX  */
    PIXEL = 103,                   /* PIXEL  */
    PIXELS = 104,                  /* PIXELS  */
    IN = 98,                       /* IN  */
    INCH = 105,                    /* INCH  */
    INCHES = 106,                  /* INCHES  */
    CM = 99,                       /* CM  */
    CENTIMETER = 107,              /* CENTIMETER  */
    CENTIMETERS = 108,             /* CENTIMETERS  */
    MM = 100,                      /* MM  */
    MILLIMETER = 109,              /* MILLIMETER  */
    MILLIMETERS = 110,             /* MILLIMETERS  */
    PT = 101,                      /* PT  */
    POINT = 111,                   /* POINT  */
    POINTS = 112,                  /* POINTS  */
    FU = 102,                      /* FU  */
    FONT_UNIT = 113,               /* FONT_UNIT  */
    FONT_UNITS = 114               /* FONT_UNITS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define UILEOF 0
#define YYerror 256
#define YYUNDEF 257
#define NAME 1
#define FONT_NAME 2
#define COLOR_NAME 3
#define ARGUMENT_NAME 4
#define REASON_NAME 5
#define CHARSET_NAME 6
#define ENUMVAL_NAME 7
#define CLASS_NAME 8
#define CHILD_NAME 96
#define UNS_FLOAT_LITERAL 9
#define COMP_STRING 10
#define CHAR_8_LITERAL 11
#define UNS_INT_LITERAL 12
#define LEFT_PAREN 13
#define RIGHT_PAREN 14
#define COLON 15
#define SEMICOLON 16
#define LEFT_BRACE 17
#define RIGHT_BRACE 18
#define COMMA 19
#define EQUAL_SIGN 20
#define NOT 21
#define PLUS 22
#define MINUS 23
#define AND 24
#define OR 25
#define XOR 26
#define MULTIPLY 27
#define DIVIDE 28
#define LEFT_SHIFT 29
#define RIGHT_SHIFT 30
#define LIST 31
#define IDENTIFIER 32
#define END 33
#define MODULE 34
#define UILTRUE 35
#define UILFALSE 36
#define INCLUDE 37
#define MACRO 38
#define ON 39
#define OFF 40
#define VALUE 41
#define ARGUMENTS 42
#define CALLBACKS 43
#define PROCEDURES 44
#define CONTROLS 45
#define PROCEDURE 46
#define OBJECT 47
#define OBJECTS 48
#define WIDGET 49
#define GADGET 50
#define FONT 51
#define ARGUMENT 52
#define REASON 53
#define PIXMAP 54
#define COLOR 55
#define NAMES 56
#define CHARACTER_SET 57
#define CASE_SENSITIVE 58
#define CASE_INSENSITIVE 59
#define VERSION 60
#define MANAGED 61
#define UNMANAGED 62
#define PRIVATE 63
#define IMPORTED 64
#define EXPORTED 65
#define UILFILE 66
#define STRING_TABLE 67
#define TRANSLATION_TABLE 68
#define COMPOUND_STRING 69
#define FONT_TABLE 70
#define ANY 71
#define STRING 72
#define BOOLEAN 73
#define ICON 74
#define RIGHT_TO_LEFT 75
#define BACKGROUND 76
#define FOREGROUND 77
#define COLOR_TABLE 78
#define FLOAT 79
#define INTEGER 80
#define CLASS_REC_NAME 81
#define ASCIZ_TABLE 82
#define INTEGER_TABLE 83
#define ASCIZ_STRING_TABLE 84
#define COMPOUND_STRING_TABLE 85
#define XBITMAPFILE 86
#define SEPARATE 87
#define SIXTEEN_BIT 88
#define POUND 89
#define KEYSYM 90
#define SINGLE_FLOAT 91
#define RGB 92
#define WIDE_CHARACTER 93
#define LOC_STRING 94
#define FONTSET 95
#define COMPOUND_STRING_COMPONENT 97
#define PIX 115
#define PIXEL 103
#define PIXELS 104
#define IN 98
#define INCH 105
#define INCHES 106
#define CM 99
#define CENTIMETER 107
#define CENTIMETERS 108
#define MM 100
#define MILLIMETER 109
#define MILLIMETERS 110
#define PT 101
#define POINT 111
#define POINTS 112
#define FU 102
#define FONT_UNIT 113
#define FONT_UNITS 114

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_UIL_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* UILEOF  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NAME = 3,                       /* NAME  */
  YYSYMBOL_FONT_NAME = 4,                  /* FONT_NAME  */
  YYSYMBOL_COLOR_NAME = 5,                 /* COLOR_NAME  */
  YYSYMBOL_ARGUMENT_NAME = 6,              /* ARGUMENT_NAME  */
  YYSYMBOL_REASON_NAME = 7,                /* REASON_NAME  */
  YYSYMBOL_CHARSET_NAME = 8,               /* CHARSET_NAME  */
  YYSYMBOL_ENUMVAL_NAME = 9,               /* ENUMVAL_NAME  */
  YYSYMBOL_CLASS_NAME = 10,                /* CLASS_NAME  */
  YYSYMBOL_CHILD_NAME = 11,                /* CHILD_NAME  */
  YYSYMBOL_UNS_FLOAT_LITERAL = 12,         /* UNS_FLOAT_LITERAL  */
  YYSYMBOL_COMP_STRING = 13,               /* COMP_STRING  */
  YYSYMBOL_CHAR_8_LITERAL = 14,            /* CHAR_8_LITERAL  */
  YYSYMBOL_UNS_INT_LITERAL = 15,           /* UNS_INT_LITERAL  */
  YYSYMBOL_LEFT_PAREN = 16,                /* LEFT_PAREN  */
  YYSYMBOL_RIGHT_PAREN = 17,               /* RIGHT_PAREN  */
  YYSYMBOL_COLON = 18,                     /* COLON  */
  YYSYMBOL_SEMICOLON = 19,                 /* SEMICOLON  */
  YYSYMBOL_LEFT_BRACE = 20,                /* LEFT_BRACE  */
  YYSYMBOL_RIGHT_BRACE = 21,               /* RIGHT_BRACE  */
  YYSYMBOL_COMMA = 22,                     /* COMMA  */
  YYSYMBOL_EQUAL_SIGN = 23,                /* EQUAL_SIGN  */
  YYSYMBOL_NOT = 24,                       /* NOT  */
  YYSYMBOL_PLUS = 25,                      /* PLUS  */
  YYSYMBOL_MINUS = 26,                     /* MINUS  */
  YYSYMBOL_AND = 27,                       /* AND  */
  YYSYMBOL_OR = 28,                        /* OR  */
  YYSYMBOL_XOR = 29,                       /* XOR  */
  YYSYMBOL_MULTIPLY = 30,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 31,                    /* DIVIDE  */
  YYSYMBOL_LEFT_SHIFT = 32,                /* LEFT_SHIFT  */
  YYSYMBOL_RIGHT_SHIFT = 33,               /* RIGHT_SHIFT  */
  YYSYMBOL_LIST = 34,                      /* LIST  */
  YYSYMBOL_IDENTIFIER = 35,                /* IDENTIFIER  */
  YYSYMBOL_END = 36,                       /* END  */
  YYSYMBOL_MODULE = 37,                    /* MODULE  */
  YYSYMBOL_UILTRUE = 38,                   /* UILTRUE  */
  YYSYMBOL_UILFALSE = 39,                  /* UILFALSE  */
  YYSYMBOL_INCLUDE = 40,                   /* INCLUDE  */
  YYSYMBOL_MACRO = 41,                     /* MACRO  */
  YYSYMBOL_ON = 42,                        /* ON  */
  YYSYMBOL_OFF = 43,                       /* OFF  */
  YYSYMBOL_VALUE = 44,                     /* VALUE  */
  YYSYMBOL_ARGUMENTS = 45,                 /* ARGUMENTS  */
  YYSYMBOL_CALLBACKS = 46,                 /* CALLBACKS  */
  YYSYMBOL_PROCEDURES = 47,                /* PROCEDURES  */
  YYSYMBOL_CONTROLS = 48,                  /* CONTROLS  */
  YYSYMBOL_PROCEDURE = 49,                 /* PROCEDURE  */
  YYSYMBOL_OBJECT = 50,                    /* OBJECT  */
  YYSYMBOL_OBJECTS = 51,                   /* OBJECTS  */
  YYSYMBOL_WIDGET = 52,                    /* WIDGET  */
  YYSYMBOL_GADGET = 53,                    /* GADGET  */
  YYSYMBOL_FONT = 54,                      /* FONT  */
  YYSYMBOL_ARGUMENT = 55,                  /* ARGUMENT  */
  YYSYMBOL_REASON = 56,                    /* REASON  */
  YYSYMBOL_PIXMAP = 57,                    /* PIXMAP  */
  YYSYMBOL_COLOR = 58,                     /* COLOR  */
  YYSYMBOL_NAMES = 59,                     /* NAMES  */
  YYSYMBOL_CHARACTER_SET = 60,             /* CHARACTER_SET  */
  YYSYMBOL_CASE_SENSITIVE = 61,            /* CASE_SENSITIVE  */
  YYSYMBOL_CASE_INSENSITIVE = 62,          /* CASE_INSENSITIVE  */
  YYSYMBOL_VERSION = 63,                   /* VERSION  */
  YYSYMBOL_MANAGED = 64,                   /* MANAGED  */
  YYSYMBOL_UNMANAGED = 65,                 /* UNMANAGED  */
  YYSYMBOL_PRIVATE = 66,                   /* PRIVATE  */
  YYSYMBOL_IMPORTED = 67,                  /* IMPORTED  */
  YYSYMBOL_EXPORTED = 68,                  /* EXPORTED  */
  YYSYMBOL_UILFILE = 69,                   /* UILFILE  */
  YYSYMBOL_STRING_TABLE = 70,              /* STRING_TABLE  */
  YYSYMBOL_TRANSLATION_TABLE = 71,         /* TRANSLATION_TABLE  */
  YYSYMBOL_COMPOUND_STRING = 72,           /* COMPOUND_STRING  */
  YYSYMBOL_FONT_TABLE = 73,                /* FONT_TABLE  */
  YYSYMBOL_ANY = 74,                       /* ANY  */
  YYSYMBOL_STRING = 75,                    /* STRING  */
  YYSYMBOL_BOOLEAN = 76,                   /* BOOLEAN  */
  YYSYMBOL_ICON = 77,                      /* ICON  */
  YYSYMBOL_RIGHT_TO_LEFT = 78,             /* RIGHT_TO_LEFT  */
  YYSYMBOL_BACKGROUND = 79,                /* BACKGROUND  */
  YYSYMBOL_FOREGROUND = 80,                /* FOREGROUND  */
  YYSYMBOL_COLOR_TABLE = 81,               /* COLOR_TABLE  */
  YYSYMBOL_FLOAT = 82,                     /* FLOAT  */
  YYSYMBOL_INTEGER = 83,                   /* INTEGER  */
  YYSYMBOL_CLASS_REC_NAME = 84,            /* CLASS_REC_NAME  */
  YYSYMBOL_ASCIZ_TABLE = 85,               /* ASCIZ_TABLE  */
  YYSYMBOL_INTEGER_TABLE = 86,             /* INTEGER_TABLE  */
  YYSYMBOL_ASCIZ_STRING_TABLE = 87,        /* ASCIZ_STRING_TABLE  */
  YYSYMBOL_COMPOUND_STRING_TABLE = 88,     /* COMPOUND_STRING_TABLE  */
  YYSYMBOL_XBITMAPFILE = 89,               /* XBITMAPFILE  */
  YYSYMBOL_SEPARATE = 90,                  /* SEPARATE  */
  YYSYMBOL_SIXTEEN_BIT = 91,               /* SIXTEEN_BIT  */
  YYSYMBOL_POUND = 92,                     /* POUND  */
  YYSYMBOL_KEYSYM = 93,                    /* KEYSYM  */
  YYSYMBOL_SINGLE_FLOAT = 94,              /* SINGLE_FLOAT  */
  YYSYMBOL_RGB = 95,                       /* RGB  */
  YYSYMBOL_WIDE_CHARACTER = 96,            /* WIDE_CHARACTER  */
  YYSYMBOL_LOC_STRING = 97,                /* LOC_STRING  */
  YYSYMBOL_FONTSET = 98,                   /* FONTSET  */
  YYSYMBOL_COMPOUND_STRING_COMPONENT = 99, /* COMPOUND_STRING_COMPONENT  */
  YYSYMBOL_PIX = 100,                      /* PIX  */
  YYSYMBOL_PIXEL = 101,                    /* PIXEL  */
  YYSYMBOL_PIXELS = 102,                   /* PIXELS  */
  YYSYMBOL_IN = 103,                       /* IN  */
  YYSYMBOL_INCH = 104,                     /* INCH  */
  YYSYMBOL_INCHES = 105,                   /* INCHES  */
  YYSYMBOL_CM = 106,                       /* CM  */
  YYSYMBOL_CENTIMETER = 107,               /* CENTIMETER  */
  YYSYMBOL_CENTIMETERS = 108,              /* CENTIMETERS  */
  YYSYMBOL_MM = 109,                       /* MM  */
  YYSYMBOL_MILLIMETER = 110,               /* MILLIMETER  */
  YYSYMBOL_MILLIMETERS = 111,              /* MILLIMETERS  */
  YYSYMBOL_PT = 112,                       /* PT  */
  YYSYMBOL_POINT = 113,                    /* POINT  */
  YYSYMBOL_POINTS = 114,                   /* POINTS  */
  YYSYMBOL_FU = 115,                       /* FU  */
  YYSYMBOL_FONT_UNIT = 116,                /* FONT_UNIT  */
  YYSYMBOL_FONT_UNITS = 117,               /* FONT_UNITS  */
  YYSYMBOL_YYACCEPT = 118,                 /* $accept  */
  YYSYMBOL_module_block = 119,             /* module_block  */
  YYSYMBOL_120_1 = 120,                    /* $@1  */
  YYSYMBOL_module_header = 121,            /* module_header  */
  YYSYMBOL_122_2 = 122,                    /* @2  */
  YYSYMBOL_123_3 = 123,                    /* @3  */
  YYSYMBOL_opt_module_clauses = 124,       /* opt_module_clauses  */
  YYSYMBOL_module_clauses = 125,           /* module_clauses  */
  YYSYMBOL_module_clause = 126,            /* module_clause  */
  YYSYMBOL_127_4 = 127,                    /* $@4  */
  YYSYMBOL_sensitivity_value = 128,        /* sensitivity_value  */
  YYSYMBOL_variant_spec = 129,             /* variant_spec  */
  YYSYMBOL_130_5 = 130,                    /* $@5  */
  YYSYMBOL_variant_list = 131,             /* variant_list  */
  YYSYMBOL_132_6 = 132,                    /* $@6  */
  YYSYMBOL_variant_clause = 133,           /* variant_clause  */
  YYSYMBOL_134_7 = 134,                    /* $@7  */
  YYSYMBOL_variant = 135,                  /* variant  */
  YYSYMBOL_module_declaration_list = 136,  /* module_declaration_list  */
  YYSYMBOL_module_declaration = 137,       /* module_declaration  */
  YYSYMBOL_value_declaration = 138,        /* value_declaration  */
  YYSYMBOL_139_8 = 139,                    /* @8  */
  YYSYMBOL_value_declaration_list = 140,   /* value_declaration_list  */
  YYSYMBOL_141_9 = 141,                    /* $@9  */
  YYSYMBOL_value_decl = 142,               /* value_decl  */
  YYSYMBOL_value_definition = 143,         /* value_definition  */
  YYSYMBOL_value_type = 144,               /* value_type  */
  YYSYMBOL_arg_value_type = 145,           /* arg_value_type  */
  YYSYMBOL_string_table_type = 146,        /* string_table_type  */
  YYSYMBOL_asciz_table_type = 147,         /* asciz_table_type  */
  YYSYMBOL_procedure_declaration = 148,    /* procedure_declaration  */
  YYSYMBOL_149_10 = 149,                   /* @10  */
  YYSYMBOL_procedure_declaration_list = 150, /* procedure_declaration_list  */
  YYSYMBOL_151_11 = 151,                   /* $@11  */
  YYSYMBOL_procedure_decl = 152,           /* procedure_decl  */
  YYSYMBOL_opt_formal_parameters = 153,    /* opt_formal_parameters  */
  YYSYMBOL_formal_parameter_type = 154,    /* formal_parameter_type  */
  YYSYMBOL_opt_class = 155,                /* opt_class  */
  YYSYMBOL_identifier_declaration = 156,   /* identifier_declaration  */
  YYSYMBOL_157_12 = 157,                   /* @12  */
  YYSYMBOL_identifier_declaration_list = 158, /* identifier_declaration_list  */
  YYSYMBOL_159_13 = 159,                   /* $@13  */
  YYSYMBOL_identifier_decl = 160,          /* identifier_decl  */
  YYSYMBOL_include_directive = 161,        /* include_directive  */
  YYSYMBOL_162_14 = 162,                   /* $@14  */
  YYSYMBOL_object_declaration = 163,       /* object_declaration  */
  YYSYMBOL_164_15 = 164,                   /* @15  */
  YYSYMBOL_object_decl_list = 165,         /* object_decl_list  */
  YYSYMBOL_166_16 = 166,                   /* $@16  */
  YYSYMBOL_object_decl = 167,              /* object_decl  */
  YYSYMBOL_168_17 = 168,                   /* @17  */
  YYSYMBOL_object_definition = 169,        /* object_definition  */
  YYSYMBOL_170_18 = 170,                   /* $@18  */
  YYSYMBOL_171_19 = 171,                   /* $@19  */
  YYSYMBOL_172_20 = 172,                   /* $@20  */
  YYSYMBOL_173_21 = 173,                   /* $@21  */
  YYSYMBOL_174_22 = 174,                   /* $@22  */
  YYSYMBOL_control_object_definition = 175, /* control_object_definition  */
  YYSYMBOL_176_23 = 176,                   /* $@23  */
  YYSYMBOL_177_24 = 177,                   /* $@24  */
  YYSYMBOL_178_25 = 178,                   /* $@25  */
  YYSYMBOL_179_26 = 179,                   /* $@26  */
  YYSYMBOL_180_27 = 180,                   /* $@27  */
  YYSYMBOL_181_28 = 181,                   /* $@28  */
  YYSYMBOL_182_29 = 182,                   /* $@29  */
  YYSYMBOL_object_specification = 183,     /* object_specification  */
  YYSYMBOL_184_30 = 184,                   /* $@30  */
  YYSYMBOL_opt_variant = 185,              /* opt_variant  */
  YYSYMBOL_control_object_specification = 186, /* control_object_specification  */
  YYSYMBOL_187_31 = 187,                   /* @31  */
  YYSYMBOL_opt_create_proc_ref = 188,      /* opt_create_proc_ref  */
  YYSYMBOL_opt_parens = 189,               /* opt_parens  */
  YYSYMBOL_object_spec = 190,              /* object_spec  */
  YYSYMBOL_191_32 = 191,                   /* $@32  */
  YYSYMBOL_control_object_spec = 192,      /* control_object_spec  */
  YYSYMBOL_193_33 = 193,                   /* $@33  */
  YYSYMBOL_child_object_spec = 194,        /* child_object_spec  */
  YYSYMBOL_195_34 = 195,                   /* $@34  */
  YYSYMBOL_object_feature_list = 196,      /* object_feature_list  */
  YYSYMBOL_object_feature = 197,           /* object_feature  */
  YYSYMBOL_198_35 = 198,                   /* $@35  */
  YYSYMBOL_epsilon_production = 199,       /* epsilon_production  */
  YYSYMBOL_list_declaration = 200,         /* list_declaration  */
  YYSYMBOL_201_36 = 201,                   /* @36  */
  YYSYMBOL_list_decl_list = 202,           /* list_decl_list  */
  YYSYMBOL_203_37 = 203,                   /* $@37  */
  YYSYMBOL_list_decl = 204,                /* list_decl  */
  YYSYMBOL_205_38 = 205,                   /* @38  */
  YYSYMBOL_list_specification = 206,       /* list_specification  */
  YYSYMBOL_control_list_specification = 207, /* control_list_specification  */
  YYSYMBOL_208_39 = 208,                   /* @39  */
  YYSYMBOL_argument_list_specification = 209, /* argument_list_specification  */
  YYSYMBOL_210_40 = 210,                   /* @40  */
  YYSYMBOL_callback_list_specification = 211, /* callback_list_specification  */
  YYSYMBOL_212_41 = 212,                   /* @41  */
  YYSYMBOL_procedure_list_specification = 213, /* procedure_list_specification  */
  YYSYMBOL_214_42 = 214,                   /* @42  */
  YYSYMBOL_argument_list_type = 215,       /* argument_list_type  */
  YYSYMBOL_callback_list_type = 216,       /* callback_list_type  */
  YYSYMBOL_procedure_list_type = 217,      /* procedure_list_type  */
  YYSYMBOL_control_list_type = 218,        /* control_list_type  */
  YYSYMBOL_argument_list_spec = 219,       /* argument_list_spec  */
  YYSYMBOL_220_43 = 220,                   /* $@43  */
  YYSYMBOL_callback_list_spec = 221,       /* callback_list_spec  */
  YYSYMBOL_222_44 = 222,                   /* $@44  */
  YYSYMBOL_procedure_list_spec = 223,      /* procedure_list_spec  */
  YYSYMBOL_224_45 = 224,                   /* $@45  */
  YYSYMBOL_control_list_spec = 225,        /* control_list_spec  */
  YYSYMBOL_226_46 = 226,                   /* $@46  */
  YYSYMBOL_argument_list_clause_list = 227, /* argument_list_clause_list  */
  YYSYMBOL_callback_list_clause_list = 228, /* callback_list_clause_list  */
  YYSYMBOL_procedure_list_clause_list = 229, /* procedure_list_clause_list  */
  YYSYMBOL_control_list_clause_list = 230, /* control_list_clause_list  */
  YYSYMBOL_argument_list_clause = 231,     /* argument_list_clause  */
  YYSYMBOL_232_47 = 232,                   /* $@47  */
  YYSYMBOL_callback_list_clause = 233,     /* callback_list_clause  */
  YYSYMBOL_234_48 = 234,                   /* $@48  */
  YYSYMBOL_procedure_list_clause = 235,    /* procedure_list_clause  */
  YYSYMBOL_236_49 = 236,                   /* $@49  */
  YYSYMBOL_control_list_clause = 237,      /* control_list_clause  */
  YYSYMBOL_238_50 = 238,                   /* $@50  */
  YYSYMBOL_control_list_def = 239,         /* control_list_def  */
  YYSYMBOL_240_51 = 240,                   /* @51  */
  YYSYMBOL_argument_list_def = 241,        /* argument_list_def  */
  YYSYMBOL_242_52 = 242,                   /* @52  */
  YYSYMBOL_callback_list_def = 243,        /* callback_list_def  */
  YYSYMBOL_244_53 = 244,                   /* @53  */
  YYSYMBOL_procedure_list_def = 245,       /* procedure_list_def  */
  YYSYMBOL_246_54 = 246,                   /* @54  */
  YYSYMBOL_control_list_item = 247,        /* control_list_item  */
  YYSYMBOL_248_55 = 248,                   /* @55  */
  YYSYMBOL_249_56 = 249,                   /* @56  */
  YYSYMBOL_250_57 = 250,                   /* @57  */
  YYSYMBOL_opt_child_managed = 251,        /* opt_child_managed  */
  YYSYMBOL_argument_list_item = 252,       /* argument_list_item  */
  YYSYMBOL_callback_list_item = 253,       /* callback_list_item  */
  YYSYMBOL_control_item = 254,             /* control_item  */
  YYSYMBOL_255_58 = 255,                   /* @58  */
  YYSYMBOL_opt_managed = 256,              /* opt_managed  */
  YYSYMBOL_procedure_reference = 257,      /* procedure_reference  */
  YYSYMBOL_procedure_list_def_ref = 258,   /* procedure_list_def_ref  */
  YYSYMBOL_opt_procedure_argument = 259,   /* opt_procedure_argument  */
  YYSYMBOL_id = 260,                       /* id  */
  YYSYMBOL_id_ref = 261,                   /* id_ref  */
  YYSYMBOL_keyword = 262,                  /* keyword  */
  YYSYMBOL_character_8_value = 263,        /* character_8_value  */
  YYSYMBOL_private_value = 264,            /* private_value  */
  YYSYMBOL_private_value_plus = 265,       /* private_value_plus  */
  YYSYMBOL_non_export_value = 266,         /* non_export_value  */
  YYSYMBOL_value = 267,                    /* value  */
  YYSYMBOL_value_1 = 268,                  /* value_1  */
  YYSYMBOL_value_2 = 269,                  /* value_2  */
  YYSYMBOL_value_3 = 270,                  /* value_3  */
  YYSYMBOL_value_4 = 271,                  /* value_4  */
  YYSYMBOL_value_5 = 272,                  /* value_5  */
  YYSYMBOL_value_6 = 273,                  /* value_6  */
  YYSYMBOL_operand = 274,                  /* operand  */
  YYSYMBOL_string_list = 275,              /* string_list  */
  YYSYMBOL_string_list_2 = 276,            /* string_list_2  */
  YYSYMBOL_asciz_list = 277,               /* asciz_list  */
  YYSYMBOL_asciz_list_2 = 278,             /* asciz_list_2  */
  YYSYMBOL_integer_list = 279,             /* integer_list  */
  YYSYMBOL_integer_list_2 = 280,           /* integer_list_2  */
  YYSYMBOL_rgb_list = 281,                 /* rgb_list  */
  YYSYMBOL_comp_str_list = 282,            /* comp_str_list  */
  YYSYMBOL_comp_str_list_2 = 283,          /* comp_str_list_2  */
  YYSYMBOL_opt_arg_type = 284,             /* opt_arg_type  */
  YYSYMBOL_comp_str_result = 285,          /* comp_str_result  */
  YYSYMBOL_comp_str_comp_result = 286,     /* comp_str_comp_result  */
  YYSYMBOL_comp_str_attrs = 287,           /* comp_str_attrs  */
  YYSYMBOL_comp_str_attr = 288,            /* comp_str_attr  */
  YYSYMBOL_wchar_str_result = 289,         /* wchar_str_result  */
  YYSYMBOL_font_spec = 290,                /* font_spec  */
  YYSYMBOL_fontset_spec = 291,             /* fontset_spec  */
  YYSYMBOL_font_list = 292,                /* font_list  */
  YYSYMBOL_font_item = 293,                /* font_item  */
  YYSYMBOL_optional_charset = 294,         /* optional_charset  */
  YYSYMBOL_valid_charset = 295,            /* valid_charset  */
  YYSYMBOL_charset_info = 296,             /* charset_info  */
  YYSYMBOL_charset_attrs = 297,            /* charset_attrs  */
  YYSYMBOL_charset_attr = 298,             /* charset_attr  */
  YYSYMBOL_color_list = 299,               /* color_list  */
  YYSYMBOL_color_item = 300,               /* color_item  */
  YYSYMBOL_color_spec = 301,               /* color_spec  */
  YYSYMBOL_mono_color_spec = 302,          /* mono_color_spec  */
  YYSYMBOL_icon_spec = 303,                /* icon_spec  */
  YYSYMBOL_icon_rows = 304,                /* icon_rows  */
  YYSYMBOL_units_specification = 305       /* units_specification  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3848

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  118
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  188
/* YYNRULES -- Number of rules.  */
#define YYNRULES  474
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  750

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   257


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    11,    99,   103,   106,
     109,   112,   115,   101,   102,   104,   105,   107,   108,   110,
     111,   113,   114,   116,   117,   100,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   239,   239,   238,   244,   245,   244,   250,   251,   255,
     256,   260,   261,   262,   263,   263,   267,   268,   272,   273,
     274,   274,   278,   279,   280,   280,   284,   284,   289,   290,
     294,   295,   299,   300,   301,   302,   303,   304,   312,   312,
     316,   317,   318,   318,   322,   326,   327,   328,   329,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,   353,
     354,   355,   356,   357,   362,   363,   364,   365,   366,   367,
     368,   369,   370,   371,   372,   373,   374,   375,   376,   377,
     378,   379,   380,   381,   385,   386,   390,   391,   398,   398,
     402,   403,   404,   404,   408,   412,   413,   417,   418,   419,
     421,   432,   445,   445,   449,   450,   451,   451,   455,   465,
     466,   466,   474,   474,   478,   479,   480,   480,   485,   484,
     493,   492,   496,   495,   499,   498,   502,   504,   501,   519,
     518,   522,   521,   525,   524,   528,   527,   531,   530,   534,
     536,   533,   543,   542,   551,   553,   559,   558,   567,   568,
     574,   575,   584,   586,   587,   588,   588,   592,   594,   597,
     600,   600,   604,   607,   610,   610,   614,   615,   619,   621,
     623,   626,   626,   631,   639,   639,   644,   645,   646,   646,
     651,   650,   658,   659,   660,   661,   666,   665,   673,   672,
     680,   679,   687,   686,   693,   698,   703,   707,   712,   713,
     714,   715,   715,   719,   720,   721,   722,   722,   726,   728,
     729,   730,   730,   734,   735,   736,   737,   737,   741,   742,
     746,   747,   751,   752,   756,   757,   761,   763,   765,   765,
     769,   771,   773,   773,   777,   779,   781,   781,   785,   787,
     789,   789,   794,   793,   801,   800,   808,   807,   815,   814,
     824,   828,   827,   834,   833,   839,   838,   845,   846,   854,
     859,   873,   875,   881,   880,   887,   889,   891,   896,   901,
     906,   907,   909,   917,   926,   927,   928,   929,   930,   931,
     932,   933,   939,   944,   945,   946,   947,   948,   949,   950,
     951,   952,   954,   955,   956,   958,   960,   961,   963,   965,
     966,   967,   968,   969,   970,   971,   972,   973,   974,   975,
     976,   977,   978,   979,   980,   982,   984,   985,   986,   987,
     988,   989,   990,   997,  1014,  1027,  1035,  1045,  1051,  1052,
    1053,  1057,  1058,  1062,  1063,  1064,  1068,  1069,  1070,  1074,
    1075,  1076,  1080,  1081,  1082,  1083,  1087,  1088,  1092,  1094,
    1096,  1100,  1101,  1103,  1105,  1106,  1107,  1108,  1109,  1111,
    1113,  1116,  1118,  1120,  1122,  1124,  1128,  1130,  1132,  1134,
    1136,  1137,  1138,  1139,  1140,  1141,  1143,  1146,  1149,  1151,
    1153,  1154,  1160,  1161,  1166,  1168,  1174,  1175,  1180,  1182,
    1188,  1189,  1194,  1196,  1202,  1212,  1213,  1218,  1220,  1225,
    1226,  1227,  1231,  1232,  1236,  1237,  1238,  1239,  1240,  1244,
    1245,  1249,  1253,  1255,  1260,  1264,  1266,  1270,  1272,  1276,
    1277,  1284,  1285,  1289,  1292,  1297,  1298,  1299,  1305,  1308,
    1314,  1315,  1319,  1323,  1330,  1331,  1335,  1336,  1337,  1341,
    1342,  1346,  1347,  1351,  1352,  1357,  1359,  1364,  1366,  1368,
    1370,  1372,  1374,  1376,  1378,  1380,  1382,  1384,  1386,  1388,
    1390,  1392,  1394,  1396,  1398
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "UILEOF", "error", "\"invalid token\"", "NAME", "FONT_NAME",
  "COLOR_NAME", "ARGUMENT_NAME", "REASON_NAME", "CHARSET_NAME",
  "ENUMVAL_NAME", "CLASS_NAME", "CHILD_NAME", "UNS_FLOAT_LITERAL",
  "COMP_STRING", "CHAR_8_LITERAL", "UNS_INT_LITERAL", "LEFT_PAREN",
  "RIGHT_PAREN", "COLON", "SEMICOLON", "LEFT_BRACE", "RIGHT_BRACE",
  "COMMA", "EQUAL_SIGN", "NOT", "PLUS", "MINUS", "AND", "OR", "XOR",
  "MULTIPLY", "DIVIDE", "LEFT_SHIFT", "RIGHT_SHIFT", "LIST", "IDENTIFIER",
  "END", "MODULE", "UILTRUE", "UILFALSE", "INCLUDE", "MACRO", "ON", "OFF",
  "VALUE", "ARGUMENTS", "CALLBACKS", "PROCEDURES", "CONTROLS", "PROCEDURE",
  "OBJECT", "OBJECTS", "WIDGET", "GADGET", "FONT", "ARGUMENT", "REASON",
  "PIXMAP", "COLOR", "NAMES", "CHARACTER_SET", "CASE_SENSITIVE",
  "CASE_INSENSITIVE", "VERSION", "MANAGED", "UNMANAGED", "PRIVATE",
  "IMPORTED", "EXPORTED", "UILFILE", "STRING_TABLE", "TRANSLATION_TABLE",
  "COMPOUND_STRING", "FONT_TABLE", "ANY", "STRING", "BOOLEAN", "ICON",
  "RIGHT_TO_LEFT", "BACKGROUND", "FOREGROUND", "COLOR_TABLE", "FLOAT",
  "INTEGER", "CLASS_REC_NAME", "ASCIZ_TABLE", "INTEGER_TABLE",
  "ASCIZ_STRING_TABLE", "COMPOUND_STRING_TABLE", "XBITMAPFILE", "SEPARATE",
  "SIXTEEN_BIT", "POUND", "KEYSYM", "SINGLE_FLOAT", "RGB",
  "WIDE_CHARACTER", "LOC_STRING", "FONTSET", "COMPOUND_STRING_COMPONENT",
  "PIX", "PIXEL", "PIXELS", "IN", "INCH", "INCHES", "CM", "CENTIMETER",
  "CENTIMETERS", "MM", "MILLIMETER", "MILLIMETERS", "PT", "POINT",
  "POINTS", "FU", "FONT_UNIT", "FONT_UNITS", "$accept", "module_block",
  "$@1", "module_header", "@2", "@3", "opt_module_clauses",
  "module_clauses", "module_clause", "$@4", "sensitivity_value",
  "variant_spec", "$@5", "variant_list", "$@6", "variant_clause", "$@7",
  "variant", "module_declaration_list", "module_declaration",
  "value_declaration", "@8", "value_declaration_list", "$@9", "value_decl",
  "value_definition", "value_type", "arg_value_type", "string_table_type",
  "asciz_table_type", "procedure_declaration", "@10",
  "procedure_declaration_list", "$@11", "procedure_decl",
  "opt_formal_parameters", "formal_parameter_type", "opt_class",
  "identifier_declaration", "@12", "identifier_declaration_list", "$@13",
  "identifier_decl", "include_directive", "$@14", "object_declaration",
  "@15", "object_decl_list", "$@16", "object_decl", "@17",
  "object_definition", "$@18", "$@19", "$@20", "$@21", "$@22",
  "control_object_definition", "$@23", "$@24", "$@25", "$@26", "$@27",
  "$@28", "$@29", "object_specification", "$@30", "opt_variant",
  "control_object_specification", "@31", "opt_create_proc_ref",
  "opt_parens", "object_spec", "$@32", "control_object_spec", "$@33",
  "child_object_spec", "$@34", "object_feature_list", "object_feature",
  "$@35", "epsilon_production", "list_declaration", "@36",
  "list_decl_list", "$@37", "list_decl", "@38", "list_specification",
  "control_list_specification", "@39", "argument_list_specification",
  "@40", "callback_list_specification", "@41",
  "procedure_list_specification", "@42", "argument_list_type",
  "callback_list_type", "procedure_list_type", "control_list_type",
  "argument_list_spec", "$@43", "callback_list_spec", "$@44",
  "procedure_list_spec", "$@45", "control_list_spec", "$@46",
  "argument_list_clause_list", "callback_list_clause_list",
  "procedure_list_clause_list", "control_list_clause_list",
  "argument_list_clause", "$@47", "callback_list_clause", "$@48",
  "procedure_list_clause", "$@49", "control_list_clause", "$@50",
  "control_list_def", "@51", "argument_list_def", "@52",
  "callback_list_def", "@53", "procedure_list_def", "@54",
  "control_list_item", "@55", "@56", "@57", "opt_child_managed",
  "argument_list_item", "callback_list_item", "control_item", "@58",
  "opt_managed", "procedure_reference", "procedure_list_def_ref",
  "opt_procedure_argument", "id", "id_ref", "keyword", "character_8_value",
  "private_value", "private_value_plus", "non_export_value", "value",
  "value_1", "value_2", "value_3", "value_4", "value_5", "value_6",
  "operand", "string_list", "string_list_2", "asciz_list", "asciz_list_2",
  "integer_list", "integer_list_2", "rgb_list", "comp_str_list",
  "comp_str_list_2", "opt_arg_type", "comp_str_result",
  "comp_str_comp_result", "comp_str_attrs", "comp_str_attr",
  "wchar_str_result", "font_spec", "fontset_spec", "font_list",
  "font_item", "optional_charset", "valid_charset", "charset_info",
  "charset_attrs", "charset_attr", "color_list", "color_item",
  "color_spec", "mono_color_spec", "icon_spec", "icon_rows",
  "units_specification", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-675)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-438)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -675,    51,  -675,    16,  -675,  -675,  3645,    73,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,    44,    29,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,    -2,    70,  1988,  2087,
      72,  3257,  2186,  2285,  2384,  -675,   102,   113,   115,  -675,
      -2,  -675,  -675,  -675,  3645,   108,  -675,  -675,  3645,  -675,
     123,  -675,   217,   217,  3257,  3257,  3257,  3257,  -675,  -675,
    -675,  -675,   139,   141,   170,   178,   186,   200,   211,   215,
     230,   231,   236,   242,   258,   283,    15,   284,   289,   297,
     298,  -675,   319,   320,   321,   322,  -675,  -675,   206,    77,
     312,    86,   120,   118,  -675,  -675,  -675,   152,  -675,  3645,
    -675,   323,  -675,  3645,  -675,   324,  -675,  3645,   325,  -675,
     330,   121,    15,  3257,  -675,   326,   327,  -675,   336,   329,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,    65,  -675,  -675,  -675,  3257,  3257,  3257,  3257,
    3257,  2675,  3257,  2772,  3354,  3257,  3257,  3257,  2869,  3257,
    -675,  -675,   341,  -675,  3257,  3257,  3257,  3257,  3257,   334,
    2966,  3063,  -675,  3257,  3257,  3257,  3257,  3257,  3257,  3257,
    3257,  3257,  -675,  -675,   339,  -675,  2481,   340,  -675,  3701,
    -675,   342,   343,  -675,   345,    26,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,   264,  -675,  -675,   338,   353,   349,   355,
     356,   354,   358,  -675,    77,   361,   357,   359,   363,   360,
     341,  -675,    11,  -675,   362,   365,  -675,   367,   364,   104,
     111,   372,  -675,   373,   371,   377,  3257,   378,   124,   375,
     381,  -675,   382,   379,   383,   384,   386,  -675,   387,   385,
    -675,   391,   388,   312,   312,    86,   120,   120,   118,   118,
    -675,  -675,  -675,  3451,  3749,  3257,   366,   392,  -675,  -675,
      77,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
     396,   395,  -675,  -675,   -36,  -675,   110,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,   369,  -675,   541,   398,  -675,  -675,   117,  -675,  -675,
    3257,   -35,   394,  -675,  -675,  2772,  3257,  3257,  -675,  3257,
    -675,  -675,  -675,  -675,  3257,  -675,   397,   401,  -675,  -675,
    3257,  -675,  -675,   374,  -675,     6,  -675,  -675,  3257,  -675,
    3257,  -675,  -675,  -675,  3548,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,   399,  -675,  -675,  -675,    94,   403,  1394,
    1493,  1592,  1691,   412,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,   413,   416,   417,  -675,   -35,  -675,  -675,   419,  -675,
    -675,   -28,   420,  -675,   421,   422,  -675,  -675,  -675,  -675,
    -675,  -675,   393,   400,    31,   173,  -675,   434,   436,   434,
     434,  -675,   433,   430,  -675,   435,  -675,  -675,   701,  -675,
    -675,  -675,  -675,   800,  -675,  -675,  -675,  1196,  -675,  -675,
    -675,   208,  -675,  -675,    15,    15,  3257,  3257,  -675,  3257,
     432,   437,  -675,   -28,  3257,    15,   439,   441,  3257,  -675,
    3548,  -675,  -675,  -675,  -675,  -675,  -675,   168,  -675,   438,
    -675,  -675,   411,   899,  -675,   446,   448,   151,   447,  -675,
    -675,   423,   998,  -675,   451,   452,   164,   453,  -675,  -675,
    -675,  1295,  -675,   454,   456,   460,   457,  -675,    22,  -675,
     459,   461,   462,  1097,  -675,   463,   464,  -675,  -675,  -675,
    -675,  -675,  -675,   364,  3257,  3257,  -675,  -675,  -675,  3257,
    3257,  -675,  -675,   431,   431,  -675,  -675,  -675,  -675,   465,
     440,  -675,  -675,  -675,  -675,  3160,  -675,   467,   442,  -675,
    -675,  -675,  -675,   443,  -675,   471,   449,  -675,  -675,  -675,
    -675,  2578,  -675,  -675,  -675,   475,  -675,  -675,  -675,  -675,
     450,  -675,  -675,  -675,  -675,  -675,  -675,   477,  -675,  -675,
    -675,  -675,  3645,   168,  -675,  -675,  -675,  -675,  3645,    77,
    -675,  -675,  3645,  -675,  -675,  -675,  -675,  3645,  -675,   156,
    -675,    28,    28,  -675,   487,   177,   483,  -675,  1790,  -675,
    -675,  -675,   460,   485,  -675,  -675,    20,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,   486,  -675,
    -675,  -675,    89,  -675,  -675,  -675,  -675,   479,  -675,  -675,
     143,  -675,   159,   488,   492,   494,   431,   434,   434,   434,
     504,   434,   434,  -675,   496,  -675,   155,  -675,   499,  -675,
    -675,  -675,  -675,  -675,  -675,   168,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  1889,  -675,  -675,   205,
    -675,  -675,  -675,   498,  -675,   209,  -675,  -675,  -675,  -675
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       4,     0,     2,     0,     1,    30,     0,     0,   284,   285,
     286,   287,   288,   289,   290,   327,   297,   298,   299,   300,
     301,   302,   318,   303,   304,   305,   306,   307,   326,   308,
      94,   312,   313,   316,   325,   293,   294,   320,   317,   321,
     322,   319,   295,   296,   315,    96,   311,    97,    95,   323,
     324,   328,   329,   330,   331,   332,   314,   309,   310,     5,
     291,   120,   184,   112,     0,     0,    38,    98,   122,    31,
      32,    34,    33,    37,    35,    36,     7,     0,     0,     0,
       0,   183,     0,     0,     0,    14,     0,     0,     0,     6,
       8,     9,   121,   188,   185,     0,   190,   116,   113,   114,
       0,     3,   362,   358,   183,   183,   183,   183,   371,   372,
     373,   374,   297,   298,   299,   300,   301,   312,   313,   316,
     320,   295,   296,   315,   311,   323,     0,   328,   329,   330,
     331,   391,   332,   314,   309,   310,   434,   361,     0,   333,
     338,   341,   343,   346,   349,   352,   356,     0,    42,    39,
      40,     0,   102,    99,   100,   105,   126,   123,     0,   128,
       0,     0,     0,   183,    10,     0,     0,   186,     0,     0,
     115,   118,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     363,   359,     0,   354,   355,   353,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     437,   435,     0,   433,   183,   183,   183,   183,   183,     0,
     183,   183,   119,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   375,   360,     0,    41,   183,     0,   101,   107,
     111,     0,     0,   124,     0,     0,    16,    17,    12,    13,
      11,   189,   187,     0,   117,   357,   426,     0,   409,     0,
       0,   449,     0,   394,   334,     0,   393,   412,     0,   284,
     318,   432,     0,   429,     0,   319,   455,     0,   453,     0,
       0,     0,   402,     0,   401,     0,   183,     0,     0,     0,
       0,   424,     0,   428,     0,   414,     0,   407,     0,   406,
     398,     0,   397,   339,   340,   342,   344,   345,   347,   348,
     350,   351,    43,   183,   326,   183,   319,     0,    47,   335,
     336,   103,   110,    73,    54,    56,    55,    51,    57,    61,
      64,    62,   109,    50,    53,    63,    52,    49,    66,    60,
      67,    68,    69,    70,    71,    72,    65,   108,    58,    59,
       0,     0,   127,   125,   183,    20,     0,    15,   204,   205,
     206,   207,   191,   192,   193,   194,   195,   198,   200,   202,
     196,     0,   365,     0,     0,   368,   364,     0,   367,   379,
     183,     0,   413,   380,   382,   183,   183,   183,   383,   183,
     384,   385,   369,   378,   183,   386,   438,     0,   387,   388,
     183,   389,   390,     0,   366,     0,   381,   376,   183,   377,
     183,    46,    48,    45,   183,    44,   106,   104,   132,   136,
     130,   129,   134,     0,    24,    26,    18,     0,     0,     0,
       0,     0,     0,     0,    93,    79,    80,    76,    81,    85,
      88,    86,   411,    75,    78,    87,    77,    74,    84,    89,
      90,    91,    92,   410,    82,    83,   370,   451,   452,   450,
     395,     0,     0,     0,   419,     0,   430,   431,     0,   456,
     403,     0,   439,   436,     0,     0,   418,   415,   416,   417,
     408,   399,   321,   322,     0,     0,   444,     0,     0,     0,
       0,    21,     0,     0,    19,     0,    22,   211,     0,   199,
     292,   208,   216,     0,   201,   213,   221,     0,   203,   218,
     226,     0,   197,   223,     0,     0,   183,   183,   420,   183,
       0,     0,   440,     0,   183,     0,     0,     0,   183,   337,
     183,   152,   133,   137,   131,   135,    25,     0,    23,     0,
     238,   209,   434,     0,   228,     0,     0,     0,     0,   242,
     214,   434,     0,   230,     0,     0,     0,     0,   246,   219,
     258,     0,   232,     0,     0,   183,     0,   250,   183,   224,
     276,   277,   252,     0,   234,     0,     0,   183,   261,   425,
     421,   422,   423,   454,   183,   183,   441,   404,   427,   183,
     183,   448,   445,   183,   183,    28,    29,    27,   212,     0,
       0,   210,   229,   236,   237,   183,   217,     0,     0,   215,
     231,   240,   241,   183,   222,     0,     0,   220,   233,   244,
     245,   183,   280,   279,   227,     0,   268,   265,   267,   263,
       0,   225,   235,   248,   249,   273,   260,     0,   442,   443,
     446,   447,     0,   183,   158,   183,   239,   255,     0,   269,
     243,   257,     0,   272,   271,   247,   259,     0,   283,     0,
     251,     0,     0,   253,     0,   183,   183,   155,     0,   154,
     138,   270,   183,     0,   281,   174,     0,   266,   264,   156,
     274,   143,   145,   141,   149,   139,   262,   147,     0,   159,
     160,   165,     0,   153,   162,   278,   282,     0,   181,   183,
       0,   176,   252,     0,     0,     0,   183,     0,     0,     0,
       0,     0,     0,   161,     0,   163,     0,   175,     0,   172,
     183,   177,   180,   178,   179,   183,   144,   146,   142,   150,
     140,   148,   166,   164,   182,   173,     0,   183,   170,     0,
     157,   167,   151,     0,   183,     0,   171,   168,   183,   169
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,   414,  -675,
    -675,  -675,  -675,  -675,  -675,    54,  -675,   -15,  -675,  -675,
    -675,  -675,  -675,  -675,   389,  -675,   207,  -675,    -6,     1,
    -675,  -675,  -675,  -675,   370,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,   426,  -675,  -675,  -675,  -675,  -675,  -675,   376,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -473,  -675,  -199,  -675,  -675,
    -592,  -675,  -675,  -675,  -675,  -675,  -135,  -675,  -670,  -674,
    -675,  -156,  -675,  -675,  -675,  -675,   445,  -675,  -675,  -100,
    -675,   -69,  -675,   -76,  -675,   -75,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,     2,  -675,   -12,  -675,   -14,  -675,   -27,
    -675,  -503,  -675,  -479,  -675,  -490,  -675,   -64,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -121,    18,  -420,  -675,  -162,   148,   241,  -675,   -37,
     -16,   332,    -3,    32,  -101,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
      96,  -675,  -675,  -675,  -675,   174,  -675,  -123,  -675,  -675,
      40,  -675,    35,  -675,  -675,  -675,    47,   468
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     5,     2,     3,    76,    89,    90,    91,   160,
     248,   357,   423,   427,   492,   428,   493,   667,     7,    69,
      70,    82,   149,   234,   150,   317,   347,   453,   134,   135,
      71,    83,   153,   237,   154,   240,   350,   351,    72,    79,
      98,   169,    99,    73,    77,    74,    84,   157,   241,   158,
     244,   421,   489,   487,   490,   488,   594,   686,   711,   709,
     707,   708,   712,   710,   737,   532,   593,   668,   680,   706,
     643,   689,   693,   714,   740,   743,   677,   697,   700,   701,
     718,   136,    75,    78,    94,   165,    95,   168,   362,   363,
     432,   364,   429,   365,   430,   366,   431,   367,   368,   369,
     370,   499,   539,   504,   548,   508,   557,   512,   566,   543,
     552,   561,   573,   544,   599,   553,   607,   562,   615,   574,
     625,   703,   630,   704,   600,   705,   608,   563,   616,   576,
     637,   662,   661,   629,   546,   555,   636,   664,   577,   654,
     564,   623,   137,   565,    60,   138,   271,   318,   319,   264,
     140,   141,   142,   143,   144,   145,   146,   265,   266,   301,
     302,   283,   284,   290,   298,   299,   374,   268,   296,   382,
     464,   292,   257,   294,   272,   273,   147,   274,   397,   472,
     522,   485,   486,   262,   459,   277,   278,   190
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      57,   250,   645,   213,   193,   194,   195,    58,   575,   501,
     505,   509,   513,   554,   476,   477,   534,   535,   210,   545,
     478,   698,   716,   211,    59,   461,   721,   355,   384,   675,
     418,   419,   420,   385,   256,   258,   259,   260,   261,   249,
    -290,   699,   721,   462,   139,   281,   356,   285,   676,    85,
     520,     4,   287,     6,   528,   463,   293,    86,    87,   223,
     224,    88,   554,   521,   545,  -183,  -183,   192,  -183,   745,
     575,   721,    57,    57,    61,   212,    57,    57,    57,    58,
      58,    80,   255,    58,    58,    58,   626,   627,    57,    92,
     698,   101,    57,   223,   224,    58,    96,   100,    81,    58,
     151,   155,   159,   479,   425,   223,   224,    62,    63,    64,
     715,   424,    96,    65,   725,   494,   100,    66,   226,   227,
     425,   390,    67,    68,   396,   161,   139,   167,   391,   310,
     311,   426,   223,   224,  -183,  -183,   162,  -183,   163,   223,
     224,   399,   171,    57,   698,   228,   229,    57,   230,   231,
      58,    57,   223,   224,    58,   196,   698,   197,    58,   139,
     139,   139,   139,   139,   720,   232,   233,   151,   279,   280,
     139,   155,   139,   674,   605,   159,   733,   139,   288,   223,
     224,   139,   246,   247,   223,   224,   198,   613,  -183,  -183,
     529,  -183,   223,   224,   199,   530,   457,   458,   422,   320,
    -183,  -183,   200,  -183,  -254,  -256,   698,   303,   304,   567,
     698,     8,     9,    10,    11,    12,   201,    13,  -183,   568,
     595,   596,   666,   306,   307,   222,   744,   202,   671,   569,
     748,   203,   672,   348,   726,   727,   728,   673,   730,   731,
     349,   681,   682,   683,   684,   685,   204,   205,   694,   139,
    -183,  -183,   206,  -183,  -183,  -183,  -183,  -183,   207,    15,
     308,   309,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,   570,   571,   208,    28,   320,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,   209,
     214,    51,    52,    53,    54,   215,    55,    56,   348,   358,
     359,   360,   361,   216,   217,   349,   741,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   218,   219,   220,   221,   225,
     239,   236,   542,   295,   243,   251,   252,   551,   254,   263,
     267,   560,   276,   245,   253,   572,   282,   286,   312,   321,
     371,   352,   353,   354,   289,   291,   591,   454,   297,   300,
     372,   373,   375,   376,   455,   378,   377,   484,   379,   380,
     383,   381,   414,  -437,   388,   386,   389,   542,   387,   392,
     393,   579,   580,   394,   395,   398,   551,   400,   401,   402,
     404,   403,   588,   406,   407,   560,   405,   408,   409,   622,
     410,   415,   628,   416,   417,   456,   465,   572,   473,   471,
     491,   635,   496,    57,    57,    57,    57,   640,   641,   433,
      58,    58,    58,    58,   475,   514,   515,   644,   644,   516,
     517,   519,   523,   524,   531,   525,   533,   500,   500,   500,
     500,   526,   536,   537,   538,   584,  -254,   560,   527,   598,
     585,   547,   589,   413,   590,   603,   556,   604,   606,  -256,
     611,   612,  -275,   619,   614,   620,   621,  -306,   624,  -307,
     642,   495,   633,   634,   646,   358,   650,   669,   359,   670,
     655,   139,   652,   484,   660,   665,   360,   679,   361,   688,
     717,    57,   696,   713,   164,    57,   547,   722,    58,   687,
     690,   723,    58,   724,   729,   556,   622,   732,   734,   746,
     702,   412,   597,   238,   170,   500,   736,   678,   460,   578,
     663,   647,   651,   242,   467,   468,   702,   469,   235,   166,
     610,   656,   470,   719,   702,   602,   632,   618,   474,   653,
     644,   695,   139,   139,   411,    57,   480,   305,   481,   466,
     702,   518,    58,   586,   735,   592,   583,    57,   649,   669,
       0,   191,     0,     0,    58,     0,     0,     0,     0,   500,
       0,   742,     0,   702,   659,     0,     0,     0,   747,   702,
       0,   578,   749,   434,     0,   435,     0,   436,   437,   438,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    30,   439,   440,   441,   442,   443,   444,   445,     0,
       0,     0,     0,   446,   447,     0,    45,   448,    47,    48,
       0,     0,     0,     0,   449,   450,    57,   451,     0,   452,
       0,     0,    57,    58,     0,     0,    57,     0,     0,    58,
       0,    57,     0,    58,     0,     0,     0,     0,    58,     0,
     500,     0,    57,     0,   581,   582,   500,   276,     0,    58,
     500,     0,   587,     0,     0,   500,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   500,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   540,     0,     8,     9,    10,    11,    12,     0,
      13,     0,    14,   102,  -183,  -183,   103,   104,     0,     0,
       0,     0,   541,     0,     0,   105,   106,   107,     0,     0,
      57,     0,   638,   639,     0,     0,     0,    58,     0,   108,
     109,     0,     0,   110,   111,     0,  -183,     0,     0,     0,
       0,     0,    15,     0,   500,   112,   113,   114,   115,   116,
      21,    22,    23,    24,    25,    26,    27,     0,    28,     0,
      29,    30,   117,   118,   119,    34,    35,    36,   120,    38,
      39,    40,    41,   121,   122,   123,    45,   124,    47,    48,
     125,    50,     0,   126,   127,   128,   129,   130,   131,   132,
     133,   549,     0,     8,     9,    10,    11,    12,     0,    13,
       0,    14,   102,  -183,  -183,   103,   104,     0,     0,     0,
       0,   550,     0,     0,   105,   106,   107,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   109,
       0,     0,   110,   111,     0,     0,  -183,     0,     0,     0,
       0,    15,     0,     0,   112,   113,   114,   115,   116,    21,
      22,    23,    24,    25,    26,    27,     0,    28,     0,    29,
      30,   117,   118,   119,    34,    35,    36,   120,    38,    39,
      40,    41,   121,   122,   123,    45,   124,    47,    48,   125,
      50,     0,   126,   127,   128,   129,   130,   131,   132,   133,
     540,     0,     8,     9,    10,    11,    12,     0,    13,     0,
      14,   102,  -183,  -183,   103,   104,     0,     0,     0,     0,
     601,     0,     0,   105,   106,   107,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   108,   109,     0,
       0,   110,   111,     0,  -183,     0,     0,     0,     0,     0,
      15,     0,     0,   112,   113,   114,   115,   116,    21,    22,
      23,    24,    25,    26,    27,     0,    28,     0,    29,    30,
     117,   118,   119,    34,    35,    36,   120,    38,    39,    40,
      41,   121,   122,   123,    45,   124,    47,    48,   125,    50,
       0,   126,   127,   128,   129,   130,   131,   132,   133,   549,
       0,     8,     9,    10,    11,    12,     0,    13,     0,    14,
     102,  -183,  -183,   103,   104,     0,     0,     0,     0,   609,
       0,     0,   105,   106,   107,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   108,   109,     0,     0,
     110,   111,     0,     0,  -183,     0,     0,     0,     0,    15,
       0,     0,   112,   113,   114,   115,   116,    21,    22,    23,
      24,    25,    26,    27,     0,    28,     0,    29,    30,   117,
     118,   119,    34,    35,    36,   120,    38,    39,    40,    41,
     121,   122,   123,    45,   124,    47,    48,   125,    50,     0,
     126,   127,   128,   129,   130,   131,   132,   133,   567,     0,
       8,     9,    10,    11,    12,     0,    13,  -183,   568,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   631,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -183,     0,     0,    15,     0,
       0,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,   570,   571,     0,    28,     0,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,     0,     0,
      51,    52,    53,    54,     0,    55,    56,   558,     0,     8,
       9,    10,    11,    12,     0,    13,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   559,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -183,     0,     0,     0,    15,     0,     0,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,     0,    28,     0,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,     0,     0,    51,
      52,    53,    54,     0,    55,    56,   558,     0,     8,     9,
      10,    11,    12,     0,    13,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   617,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -183,     0,     0,     0,    15,     0,     0,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,    28,     0,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,     0,     0,    51,    52,
      53,    54,     0,    55,    56,   497,     0,     8,     9,    10,
      11,    12,     0,    13,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,   498,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,     0,     0,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
       0,    28,     0,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,     0,     0,    51,    52,    53,
      54,     0,    55,    56,   502,     0,     8,     9,    10,    11,
      12,     0,    13,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,   503,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,     0,
      28,     0,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,     0,     0,    51,    52,    53,    54,
       0,    55,    56,   506,     0,     8,     9,    10,    11,    12,
       0,    13,     0,    14,     0,     0,     0,     0,     0,     0,
       0,     0,   507,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,     0,    28,
       0,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,     0,     0,    51,    52,    53,    54,     0,
      55,    56,   510,     0,     8,     9,    10,    11,    12,     0,
      13,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,   511,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,     0,     0,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,     0,    28,     0,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,     0,     0,    51,    52,    53,    54,     0,    55,
      56,   691,     0,     8,     9,    10,    11,    12,     0,    13,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
     692,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,     0,     0,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,    28,     0,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,     0,     0,    51,    52,    53,    54,     0,    55,    56,
     738,     0,     8,     9,    10,    11,    12,     0,    13,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,   739,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,     0,     0,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,     0,    28,     0,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
       0,     0,    51,    52,    53,    54,     0,    55,    56,    93,
       0,     8,     9,    10,    11,    12,     0,    13,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,     0,    28,     0,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,     0,
       0,    51,    52,    53,    54,     0,    55,    56,    97,     0,
       8,     9,    10,    11,    12,     0,    13,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    15,     0,
       0,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,     0,    28,     0,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,     0,     0,
      51,    52,    53,    54,     0,    55,    56,   148,     0,     8,
       9,    10,    11,    12,     0,    13,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,     0,     0,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,     0,    28,     0,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,     0,     0,    51,
      52,    53,    54,     0,    55,    56,   152,     0,     8,     9,
      10,    11,    12,     0,    13,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,     0,     0,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,    28,     0,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,     0,     0,    51,    52,
      53,    54,     0,    55,    56,   156,     0,     8,     9,    10,
      11,    12,     0,    13,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,     0,     0,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
       0,    28,     0,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,     0,     0,    51,    52,    53,
      54,     0,    55,    56,     8,     9,    10,    11,    12,     0,
      13,     0,    14,   102,     0,     0,   103,   104,     0,     0,
       0,     0,     0,     0,     0,   105,   106,   107,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   108,
     109,     0,     0,   110,   111,     0,     0,     0,     0,     0,
       0,     0,    15,     0,     0,   112,   113,   114,   115,   116,
      21,    22,    23,    24,    25,    26,    27,   313,   314,   315,
      29,    30,   117,   118,   119,    34,    35,    36,   120,    38,
      39,    40,   316,   121,   122,   123,    45,   124,    47,    48,
     125,    50,     0,   126,   127,   128,   129,   130,   131,   132,
     133,     8,     9,    10,    11,    12,     0,    13,   657,    14,
     102,     0,     0,   103,   104,   658,     0,     0,     0,     0,
       0,     0,   105,   106,   107,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   108,   109,     0,     0,
     110,   111,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,   112,   113,   114,   115,   116,    21,    22,    23,
      24,    25,    26,    27,     0,    28,     0,    29,    30,   117,
     118,   119,    34,    35,    36,   120,    38,    39,    40,    41,
     121,   122,   123,    45,   124,    47,    48,   125,    50,     0,
     126,   127,   128,   129,   130,   131,   132,   133,     8,     9,
      10,    11,    12,     0,    13,     0,    14,   102,     0,     0,
     103,   104,  -392,     0,     0,     0,     0,     0,     0,   105,
     106,   107,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   109,     0,     0,   110,   111,     0,
       0,     0,     0,     0,     0,     0,    15,     0,     0,   112,
     113,   114,   115,   116,    21,    22,    23,    24,    25,    26,
      27,     0,    28,     0,    29,    30,   117,   118,   119,    34,
      35,    36,   120,    38,    39,    40,    41,   121,   122,   123,
      45,   124,    47,    48,   125,    50,     0,   126,   127,   128,
     129,   130,   131,   132,   133,   269,     9,    10,    11,    12,
     211,    13,     0,    14,   102,     0,     0,   103,   104,     0,
       0,     0,     0,     0,     0,     0,   105,   106,   107,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     108,   109,     0,     0,   110,   111,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,   112,   113,   114,   115,
     116,    21,   270,    23,    24,    25,    26,    27,     0,    28,
       0,    29,    30,   117,   118,   119,    34,    35,    36,   120,
      38,    39,    40,    41,   121,   122,   123,    45,   124,    47,
      48,   125,    50,     0,   126,   127,   128,   129,   130,   131,
     132,   133,     8,     9,    10,    11,    12,     0,    13,     0,
      14,   102,     0,     0,   103,   104,  -400,     0,     0,     0,
       0,     0,     0,   105,   106,   107,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   108,   109,     0,
       0,   110,   111,     0,     0,     0,     0,     0,     0,     0,
      15,     0,     0,   112,   113,   114,   115,   116,    21,    22,
      23,    24,    25,    26,    27,     0,    28,     0,    29,    30,
     117,   118,   119,    34,    35,    36,   120,    38,    39,    40,
      41,   121,   122,   123,    45,   124,    47,    48,   125,    50,
       0,   126,   127,   128,   129,   130,   131,   132,   133,     8,
       9,    10,    11,    12,     0,    13,     0,    14,   102,     0,
       0,   103,   104,  -405,     0,     0,     0,     0,     0,     0,
     105,   106,   107,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   108,   109,     0,     0,   110,   111,
       0,     0,     0,     0,     0,     0,     0,    15,     0,     0,
     112,   113,   114,   115,   116,    21,    22,    23,    24,    25,
      26,    27,     0,    28,     0,    29,    30,   117,   118,   119,
      34,    35,    36,   120,    38,    39,    40,    41,   121,   122,
     123,    45,   124,    47,    48,   125,    50,     0,   126,   127,
     128,   129,   130,   131,   132,   133,     8,     9,    10,    11,
      12,     0,    13,     0,    14,   102,     0,     0,   103,   104,
    -396,     0,     0,     0,     0,     0,     0,   105,   106,   107,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,   109,     0,     0,   110,   111,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,   112,   113,   114,
     115,   116,    21,    22,    23,    24,    25,    26,    27,     0,
      28,     0,    29,    30,   117,   118,   119,    34,    35,    36,
     120,    38,    39,    40,    41,   121,   122,   123,    45,   124,
      47,    48,   125,    50,     0,   126,   127,   128,   129,   130,
     131,   132,   133,     8,     9,    10,    11,    12,     0,    13,
     648,    14,   102,     0,     0,   103,   104,     0,     0,     0,
       0,     0,     0,     0,   105,   106,   107,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   109,
       0,     0,   110,   111,     0,     0,     0,     0,     0,     0,
       0,    15,     0,     0,   112,   113,   114,   115,   116,    21,
      22,    23,    24,    25,    26,    27,     0,    28,     0,    29,
      30,   117,   118,   119,    34,    35,    36,   120,    38,    39,
      40,    41,   121,   122,   123,    45,   124,    47,    48,   125,
      50,     0,   126,   127,   128,   129,   130,   131,   132,   133,
       8,     9,    10,    11,    12,     0,    13,     0,    14,   102,
       0,     0,   103,   104,     0,     0,     0,     0,     0,     0,
       0,   105,   106,   107,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   108,   109,     0,     0,   110,
     111,     0,     0,     0,     0,     0,     0,     0,    15,     0,
       0,   112,   113,   114,   115,   116,    21,    22,    23,    24,
      25,    26,    27,     0,    28,     0,    29,    30,   117,   118,
     119,    34,    35,    36,   120,    38,    39,    40,    41,   121,
     122,   123,    45,   124,    47,    48,   125,    50,     0,   126,
     127,   128,   129,   130,   131,   132,   133,     8,     9,    10,
      11,    12,     0,    13,     0,    14,   102,     0,     0,   103,
     104,     0,     0,     0,     0,     0,     0,     0,   105,   106,
     107,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   108,   109,     0,     0,   110,   111,     0,     0,
       0,     0,     0,     0,     0,    15,     0,     0,   112,   113,
     114,   115,   116,    21,    22,    23,    24,    25,    26,    27,
       0,    28,     0,    29,    30,   117,   118,   119,    34,    35,
      36,   120,    38,    39,    40,   275,   121,   122,   123,    45,
     124,    47,    48,   125,    50,     0,   126,   127,   128,   129,
     130,   131,   132,   133,     8,     9,    10,    11,    12,     0,
      13,     0,    14,   102,     0,     0,   103,   104,     0,     0,
       0,     0,     0,     0,     0,   105,   106,   107,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   108,
     109,     0,     0,   110,   111,     0,     0,     0,     0,     0,
       0,     0,    15,     0,     0,   112,   113,   114,   115,   116,
      21,    22,    23,    24,    25,    26,    27,     0,    28,     0,
      29,    30,   117,   118,   119,    34,    35,    36,   120,    38,
      39,    40,   316,   121,   122,   123,    45,   124,    47,    48,
     125,    50,     0,   126,   127,   128,   129,   130,   131,   132,
     133,     8,     9,    10,    11,    12,     0,    13,     0,    14,
     102,     0,     0,   103,   104,     0,     0,     0,     0,     0,
       0,     0,   105,   106,   107,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   108,   109,     0,     0,
     110,   111,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,   112,   113,   114,   115,   116,    21,    22,    23,
      24,    25,    26,    27,     0,    28,     0,    29,    30,   117,
     118,   119,    34,    35,    36,   120,    38,   482,   483,    41,
     121,   122,   123,    45,   124,    47,    48,   125,    50,     0,
     126,   127,   128,   129,   130,   131,   132,   133,     8,     9,
      10,    11,    12,     0,    13,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,     0,     0,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,   322,    28,     0,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,     0,     0,    51,    52,
      53,    54,     0,    55,    56,     0,     0,     0,     0,     0,
       0,     0,     0,   323,     0,   324,   325,   326,   327,   328,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    30,   329,   330,   331,   332,   333,   334,   335,     0,
       0,     0,     0,   336,   337,   338,    45,   339,    47,    48,
     340,     0,     0,     0,   341,   342,   343,   344,     0,   345,
     346,   323,     0,   324,   325,   326,   327,   328,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
     329,   330,   331,     0,   333,   334,   335,     0,     0,     0,
       0,   336,   337,   338,    45,   339,    47,    48,   340,     0,
       0,     0,   341,   342,   343,   344,     0,   345,   346
};

static const yytype_int16 yycheck[] =
{
       6,   163,   594,   126,   105,   106,   107,     6,   511,   429,
     430,   431,   432,   503,     8,     9,   489,   490,     3,   498,
      14,     1,   692,     8,     6,    60,   700,     1,    17,     1,
      66,    67,    68,    22,   196,   197,   198,   199,   200,   162,
      18,    21,   716,    78,    81,   207,    20,   209,    20,    51,
      78,     0,   214,    37,    23,    90,   218,    59,    60,    28,
      29,    63,   552,    91,   543,    45,    46,   104,    48,   739,
     573,   745,    78,    79,     1,    60,    82,    83,    84,    78,
      79,    37,    17,    82,    83,    84,    64,    65,    94,    19,
       1,    19,    98,    28,    29,    94,    78,    79,    69,    98,
      82,    83,    84,    97,    10,    28,    29,    34,    35,    36,
      21,     1,    94,    40,   706,    21,    98,    44,    32,    33,
      10,    17,    49,    50,   286,    23,   163,    19,    17,   230,
     231,    21,    28,    29,    45,    46,    23,    48,    23,    28,
      29,    17,    19,   149,     1,    25,    26,   153,    30,    31,
     149,   157,    28,    29,   153,    16,     1,    16,   157,   196,
     197,   198,   199,   200,    21,    13,    14,   149,   205,   206,
     207,   153,   209,    17,    23,   157,    21,   214,   215,    28,
      29,   218,    61,    62,    28,    29,    16,    23,    45,    46,
      17,    48,    28,    29,    16,    22,    79,    80,   354,   236,
      45,    46,    16,    48,    45,    46,     1,   223,   224,     1,
       1,     3,     4,     5,     6,     7,    16,     9,    10,    11,
      52,    53,   642,   226,   227,    19,    21,    16,   648,    21,
      21,    16,   652,   239,   707,   708,   709,   657,   711,   712,
     239,    64,    65,    66,    67,    68,    16,    16,   668,   286,
      45,    46,    16,    48,    45,    46,    48,    48,    16,    51,
     228,   229,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    16,    67,   313,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    16,
      16,    93,    94,    95,    96,    16,    98,    99,   314,    45,
      46,    47,    48,    16,    16,   314,   736,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,    16,    16,    16,    16,    27,
      16,    18,   498,     9,    19,    19,    19,   503,    19,   201,
     202,   507,   204,    23,    18,   511,   208,    16,    19,    19,
      22,    19,    19,    18,   216,   217,   528,   373,   220,   221,
      17,    22,    17,    17,   373,    17,    22,   414,    17,    22,
      17,    22,    16,    23,    17,    23,    22,   543,    23,    17,
      17,   514,   515,    22,    17,    17,   552,    22,    17,    17,
      17,    22,   525,    17,    17,   561,    22,    22,    17,   565,
      22,    19,   568,    17,    19,    17,    22,   573,    17,    22,
      21,   577,    19,   429,   430,   431,   432,   589,   590,    60,
     429,   430,   431,   432,    60,    23,    23,   593,   594,    23,
      23,    22,    22,    22,    10,    23,    10,   429,   430,   431,
     432,    58,    19,    23,    19,    23,    45,   613,    58,    21,
      23,   498,    23,   315,    23,    19,   503,    19,    21,    46,
      19,    19,    10,    19,    21,    19,    16,    18,    21,    18,
      49,   427,    19,    19,    19,    45,    19,   643,    46,   645,
      19,   528,    49,   530,    19,    18,    47,    10,    48,    16,
      21,   507,    17,    17,    90,   511,   543,    19,   507,   665,
     666,    19,   511,    19,    10,   552,   672,    21,    19,    21,
     676,   314,   537,   153,    98,   507,   725,   662,   380,   511,
     630,   600,   608,   157,   386,   387,   692,   389,   149,    94,
     552,   616,   394,   699,   700,   543,   573,   561,   400,   613,
     706,   672,   589,   590,   313,   561,   408,   225,   410,   385,
     716,   465,   561,   523,   720,   530,   519,   573,   605,   725,
      -1,   103,    -1,    -1,   573,    -1,    -1,    -1,    -1,   561,
      -1,   737,    -1,   739,   621,    -1,    -1,    -1,   744,   745,
      -1,   573,   748,    52,    -1,    54,    -1,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,    73,    74,    75,    76,    77,    -1,
      -1,    -1,    -1,    82,    83,    -1,    85,    86,    87,    88,
      -1,    -1,    -1,    -1,    93,    94,   642,    96,    -1,    98,
      -1,    -1,   648,   642,    -1,    -1,   652,    -1,    -1,   648,
      -1,   657,    -1,   652,    -1,    -1,    -1,    -1,   657,    -1,
     642,    -1,   668,    -1,   516,   517,   648,   519,    -1,   668,
     652,    -1,   524,    -1,    -1,   657,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   668,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    -1,     3,     4,     5,     6,     7,    -1,
       9,    -1,    11,    12,    13,    14,    15,    16,    -1,    -1,
      -1,    -1,    21,    -1,    -1,    24,    25,    26,    -1,    -1,
     736,    -1,   584,   585,    -1,    -1,    -1,   736,    -1,    38,
      39,    -1,    -1,    42,    43,    -1,    45,    -1,    -1,    -1,
      -1,    -1,    51,    -1,   736,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    92,    93,    94,    95,    96,    97,    98,
      99,     1,    -1,     3,     4,     5,     6,     7,    -1,     9,
      -1,    11,    12,    13,    14,    15,    16,    -1,    -1,    -1,
      -1,    21,    -1,    -1,    24,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,
      -1,    -1,    42,    43,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    -1,    67,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,    94,    95,    96,    97,    98,    99,
       1,    -1,     3,     4,     5,     6,     7,    -1,     9,    -1,
      11,    12,    13,    14,    15,    16,    -1,    -1,    -1,    -1,
      21,    -1,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,    -1,
      -1,    42,    43,    -1,    45,    -1,    -1,    -1,    -1,    -1,
      51,    -1,    -1,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    98,    99,     1,
      -1,     3,     4,     5,     6,     7,    -1,     9,    -1,    11,
      12,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    21,
      -1,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    39,    -1,    -1,
      42,    43,    -1,    -1,    46,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    98,    99,     1,    -1,
       3,     4,     5,     6,     7,    -1,     9,    10,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    51,    -1,
      -1,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      93,    94,    95,    96,    -1,    98,    99,     1,    -1,     3,
       4,     5,     6,     7,    -1,     9,    -1,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    67,    -1,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    -1,    93,
      94,    95,    96,    -1,    98,    99,     1,    -1,     3,     4,
       5,     6,     7,    -1,     9,    -1,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    21,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    93,    94,
      95,    96,    -1,    98,    99,     1,    -1,     3,     4,     5,
       6,     7,    -1,     9,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    67,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    -1,    93,    94,    95,
      96,    -1,    98,    99,     1,    -1,     3,     4,     5,     6,
       7,    -1,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    -1,    -1,    93,    94,    95,    96,
      -1,    98,    99,     1,    -1,     3,     4,     5,     6,     7,
      -1,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    51,    -1,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    -1,    67,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    -1,    93,    94,    95,    96,    -1,
      98,    99,     1,    -1,     3,     4,     5,     6,     7,    -1,
       9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    51,    -1,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    -1,    93,    94,    95,    96,    -1,    98,
      99,     1,    -1,     3,     4,     5,     6,     7,    -1,     9,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    -1,    67,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    -1,    93,    94,    95,    96,    -1,    98,    99,
       1,    -1,     3,     4,     5,     6,     7,    -1,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    -1,    -1,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    -1,    93,    94,    95,    96,    -1,    98,    99,     1,
      -1,     3,     4,     5,     6,     7,    -1,     9,    -1,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      -1,    93,    94,    95,    96,    -1,    98,    99,     1,    -1,
       3,     4,     5,     6,     7,    -1,     9,    -1,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,
      -1,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      93,    94,    95,    96,    -1,    98,    99,     1,    -1,     3,
       4,     5,     6,     7,    -1,     9,    -1,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    67,    -1,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    -1,    93,
      94,    95,    96,    -1,    98,    99,     1,    -1,     3,     4,
       5,     6,     7,    -1,     9,    -1,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    93,    94,
      95,    96,    -1,    98,    99,     1,    -1,     3,     4,     5,
       6,     7,    -1,     9,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    67,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    -1,    93,    94,    95,
      96,    -1,    98,    99,     3,     4,     5,     6,     7,    -1,
       9,    -1,    11,    12,    -1,    -1,    15,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    51,    -1,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    92,    93,    94,    95,    96,    97,    98,
      99,     3,     4,     5,     6,     7,    -1,     9,    10,    11,
      12,    -1,    -1,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    39,    -1,    -1,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    98,    99,     3,     4,
       5,     6,     7,    -1,     9,    -1,    11,    12,    -1,    -1,
      15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    24,
      25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    39,    -1,    -1,    42,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    92,    93,    94,
      95,    96,    97,    98,    99,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    15,    16,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    24,    25,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    39,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    51,    -1,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    -1,    67,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    94,    95,    96,    97,
      98,    99,     3,     4,     5,     6,     7,    -1,     9,    -1,
      11,    12,    -1,    -1,    15,    16,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,    -1,
      -1,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    -1,    -1,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    98,    99,     3,
       4,     5,     6,     7,    -1,     9,    -1,    11,    12,    -1,
      -1,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    38,    39,    -1,    -1,    42,    43,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    67,    -1,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    92,    93,
      94,    95,    96,    97,    98,    99,     3,     4,     5,     6,
       7,    -1,     9,    -1,    11,    12,    -1,    -1,    15,    16,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    39,    -1,    -1,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    -1,    92,    93,    94,    95,    96,
      97,    98,    99,     3,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,    -1,    -1,    15,    16,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    24,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,
      -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    -1,    67,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,    94,    95,    96,    97,    98,    99,
       3,     4,     5,     6,     7,    -1,     9,    -1,    11,    12,
      -1,    -1,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    -1,    -1,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,
      -1,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    92,
      93,    94,    95,    96,    97,    98,    99,     3,     4,     5,
       6,     7,    -1,     9,    -1,    11,    12,    -1,    -1,    15,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    39,    -1,    -1,    42,    43,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    67,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    98,    99,     3,     4,     5,     6,     7,    -1,
       9,    -1,    11,    12,    -1,    -1,    15,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    51,    -1,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    92,    93,    94,    95,    96,    97,    98,
      99,     3,     4,     5,     6,     7,    -1,     9,    -1,    11,
      12,    -1,    -1,    15,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    39,    -1,    -1,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    98,    99,     3,     4,
       5,     6,     7,    -1,     9,    -1,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    10,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    93,    94,
      95,    96,    -1,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,    73,    74,    75,    76,    77,    -1,
      -1,    -1,    -1,    82,    83,    84,    85,    86,    87,    88,
      89,    -1,    -1,    -1,    93,    94,    95,    96,    -1,    98,
      99,    52,    -1,    54,    55,    56,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,
      71,    72,    73,    -1,    75,    76,    77,    -1,    -1,    -1,
      -1,    82,    83,    84,    85,    86,    87,    88,    89,    -1,
      -1,    -1,    93,    94,    95,    96,    -1,    98,    99
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   119,   121,   122,     0,   120,    37,   136,     3,     4,
       5,     6,     7,     9,    11,    51,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    67,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    93,    94,    95,    96,    98,    99,   146,   147,   260,
     262,     1,    34,    35,    36,    40,    44,    49,    50,   137,
     138,   148,   156,   161,   163,   200,   123,   162,   201,   157,
      37,    69,   139,   149,   164,    51,    59,    60,    63,   124,
     125,   126,    19,     1,   202,   204,   260,     1,   158,   160,
     260,    19,    12,    15,    16,    24,    25,    26,    38,    39,
      42,    43,    54,    55,    56,    57,    58,    71,    72,    73,
      77,    82,    83,    84,    86,    89,    92,    93,    94,    95,
      96,    97,    98,    99,   146,   147,   199,   260,   263,   267,
     268,   269,   270,   271,   272,   273,   274,   294,     1,   140,
     142,   260,     1,   150,   152,   260,     1,   165,   167,   260,
     127,    23,    23,    23,   126,   203,   204,    19,   205,   159,
     160,    19,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     305,   305,   267,   272,   272,   272,    16,    16,    16,    16,
      16,    16,    16,    16,    16,    16,    16,    16,    16,    16,
       3,     8,    60,   295,    16,    16,    16,    16,    16,    16,
      16,    16,    19,    28,    29,    27,    32,    33,    25,    26,
      30,    31,    13,    14,   141,   142,    18,   151,   152,    16,
     153,   166,   167,    19,   168,    23,    61,    62,   128,   295,
     263,    19,    19,    18,    19,    17,   263,   290,   263,   263,
     263,   263,   301,   264,   267,   275,   276,   264,   285,     3,
      60,   264,   292,   293,   295,    81,   264,   303,   304,   267,
     267,   263,   264,   279,   280,   263,    16,   263,   267,   264,
     281,   264,   289,   263,   291,     9,   286,   264,   282,   283,
     264,   277,   278,   268,   268,   269,   270,   270,   271,   271,
     272,   272,    19,    66,    67,    68,    81,   143,   265,   266,
     267,    19,    10,    52,    54,    55,    56,    57,    58,    71,
      72,    73,    74,    75,    76,    77,    82,    83,    84,    86,
      89,    93,    94,    95,    96,    98,    99,   144,   146,   147,
     154,   155,    19,    19,    18,     1,    20,   129,    45,    46,
      47,    48,   206,   207,   209,   211,   213,   215,   216,   217,
     218,    22,    17,    22,   284,    17,    17,    22,    17,    17,
      22,    22,   287,    17,    17,    22,    23,    23,    17,    22,
      17,    17,    17,    17,    22,    17,   263,   296,    17,    17,
      22,    17,    17,    22,    17,    22,    17,    17,    22,    17,
      22,   265,   144,   264,    16,    19,    17,    19,    66,    67,
      68,   169,   199,   130,     1,    10,    21,   131,   133,   210,
     212,   214,   208,    60,    52,    54,    56,    57,    58,    71,
      72,    73,    74,    75,    76,    77,    82,    83,    86,    93,
      94,    96,    98,   145,   146,   147,    17,    79,    80,   302,
     264,    60,    78,    90,   288,    22,   293,   264,   264,   264,
     264,    22,   297,    17,   264,    60,     8,     9,    14,    97,
     264,   264,    79,    80,   267,   299,   300,   171,   173,   170,
     172,    21,   132,   134,    21,   133,    19,     1,    20,   219,
     260,   261,     1,    20,   221,   261,     1,    20,   223,   261,
       1,    20,   225,   261,    23,    23,    23,    23,   288,    22,
      78,    91,   298,    22,    22,    23,    58,    58,    23,    17,
      22,    10,   183,    10,   183,   183,    19,    23,    19,   220,
       1,    21,   199,   227,   231,   241,   252,   267,   222,     1,
      21,   199,   228,   233,   243,   253,   267,   224,     1,    21,
     199,   229,   235,   245,   258,   261,   226,     1,    11,    21,
      64,    65,   199,   230,   237,   239,   247,   256,   260,   295,
     295,   264,   264,   304,    23,    23,   298,   264,   295,    23,
      23,   263,   300,   184,   174,    52,    53,   135,    21,   232,
     242,    21,   231,    19,    19,    23,    21,   234,   244,    21,
     233,    19,    19,    23,    21,   236,   246,    21,   235,    19,
      19,    16,   199,   259,    21,   238,    64,    65,   199,   251,
     240,    21,   237,    19,    19,   199,   254,   248,   264,   264,
     263,   263,    49,   188,   199,   188,    19,   209,    10,   267,
      19,   211,    49,   245,   257,    19,   213,    10,    17,   267,
      19,   250,   249,   207,   255,    18,   261,   135,   185,   199,
     199,   261,   261,   261,    17,     1,    20,   194,   194,    10,
     186,    64,    65,    66,    67,    68,   175,   199,    16,   189,
     199,     1,    20,   190,   261,   259,    17,   195,     1,    21,
     196,   197,   199,   239,   241,   243,   187,   178,   179,   177,
     181,   176,   180,    17,   191,    21,   196,    21,   198,   199,
      21,   197,    19,    19,    19,   188,   183,   183,   183,    10,
     183,   183,    21,    21,    19,   199,   185,   182,     1,    20,
     192,   261,   199,   193,    21,   196,    21,   199,    21,   199
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   118,   120,   119,   122,   123,   121,   124,   124,   125,
     125,   126,   126,   126,   127,   126,   128,   128,   129,   129,
     130,   129,   131,   131,   132,   131,   134,   133,   135,   135,
     136,   136,   137,   137,   137,   137,   137,   137,   139,   138,
     140,   140,   141,   140,   142,   143,   143,   143,   143,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   146,   146,   147,   147,   149,   148,
     150,   150,   151,   150,   152,   153,   153,   154,   154,   154,
     154,   155,   157,   156,   158,   158,   159,   158,   160,   161,
     162,   161,   164,   163,   165,   165,   166,   165,   168,   167,
     170,   169,   171,   169,   172,   169,   173,   174,   169,   176,
     175,   177,   175,   178,   175,   179,   175,   180,   175,   181,
     182,   175,   184,   183,   185,   185,   187,   186,   188,   188,
     189,   189,   190,   190,   190,   191,   190,   192,   192,   192,
     193,   192,   194,   194,   195,   194,   196,   196,   197,   197,
     197,   198,   197,   199,   201,   200,   202,   202,   203,   202,
     205,   204,   206,   206,   206,   206,   208,   207,   210,   209,
     212,   211,   214,   213,   215,   216,   217,   218,   219,   219,
     219,   220,   219,   221,   221,   221,   222,   221,   223,   223,
     223,   224,   223,   225,   225,   225,   226,   225,   227,   227,
     228,   228,   229,   229,   230,   230,   231,   231,   232,   231,
     233,   233,   234,   233,   235,   235,   236,   235,   237,   237,
     238,   237,   240,   239,   242,   241,   244,   243,   246,   245,
     247,   248,   247,   249,   247,   250,   247,   251,   251,   252,
     252,   253,   253,   255,   254,   256,   256,   256,   257,   258,
     259,   259,   259,   259,   260,   260,   260,   260,   260,   260,
     260,   260,   261,   262,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   263,   264,   265,   265,   266,   267,   267,
     267,   268,   268,   269,   269,   269,   270,   270,   270,   271,
     271,   271,   272,   272,   272,   272,   273,   273,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   275,   275,   276,   276,   277,   277,   278,   278,
     279,   279,   280,   280,   281,   282,   282,   283,   283,   284,
     284,   284,   285,   285,   286,   286,   286,   286,   286,   287,
     287,   288,   288,   288,   289,   290,   290,   291,   291,   292,
     292,   293,   293,   294,   294,   295,   295,   295,   296,   296,
     297,   297,   298,   298,   299,   299,   300,   300,   300,   301,
     301,   302,   302,   303,   303,   304,   304,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     6,     0,     0,     5,     0,     1,     1,
       2,     3,     3,     3,     0,     4,     1,     1,     2,     3,
       0,     3,     2,     3,     0,     3,     0,     4,     1,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     0,     3,
       1,     2,     0,     3,     4,     2,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     3,
       1,     2,     0,     3,     4,     0,     3,     0,     1,     1,
       1,     0,     0,     3,     1,     2,     0,     3,     2,     4,
       0,     3,     0,     3,     2,     3,     0,     3,     0,     4,
       0,     3,     0,     3,     0,     3,     0,     0,     6,     0,
       3,     0,     3,     0,     3,     0,     3,     0,     3,     0,
       0,     5,     0,     5,     1,     1,     0,     5,     1,     3,
       1,     2,     1,     2,     3,     0,     3,     1,     3,     4,
       0,     3,     3,     4,     0,     3,     1,     2,     2,     2,
       2,     0,     3,     0,     0,     3,     2,     3,     0,     3,
       0,     4,     1,     1,     1,     1,     0,     3,     0,     3,
       0,     3,     0,     3,     1,     1,     1,     1,     1,     2,
       3,     0,     3,     1,     2,     3,     0,     3,     1,     2,
       3,     0,     3,     1,     2,     3,     0,     3,     1,     2,
       1,     2,     1,     2,     1,     2,     2,     2,     0,     3,
       2,     2,     0,     3,     2,     2,     0,     3,     2,     2,
       0,     3,     0,     3,     0,     3,     0,     3,     0,     3,
       2,     0,     4,     0,     4,     0,     4,     1,     1,     3,
       4,     3,     3,     0,     3,     1,     1,     1,     3,     2,
       1,     3,     4,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     1,     3,
       3,     1,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     1,     2,     2,     2,     1,     3,     1,     2,
       2,     1,     1,     2,     4,     4,     4,     4,     4,     4,
       5,     1,     1,     1,     1,     2,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     1,     0,     1,     1,     3,     0,     1,     1,     3,
       0,     1,     1,     3,     5,     0,     1,     1,     3,     0,
       2,     2,     1,     2,     1,     3,     3,     3,     3,     2,
       3,     3,     3,     3,     1,     5,     1,     5,     1,     1,
       3,     3,     1,     2,     1,     1,     4,     1,     1,     2,
       2,     3,     3,     3,     1,     3,     4,     4,     3,     1,
       3,     1,     1,     1,     5,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= UILEOF)
    {
      yychar = UILEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 239 "Uil.y"
        { sar_save_module_source (); }
#line 2883 "Uil.c"
    break;

  case 4: /* @2: %empty  */
#line 244 "Uil.y"
                                                { sar_create_root ( &yyval ); }
#line 2889 "Uil.c"
    break;

  case 5: /* @3: %empty  */
#line 245 "Uil.y"
                                                { sar_create_module( &yyval, &yyvsp[0], &yyvsp[-1] ); }
#line 2895 "Uil.c"
    break;

  case 11: /* module_clause: VERSION EQUAL_SIGN character_8_value  */
#line 260 "Uil.y"
                                                        { sar_process_module_version(  &yyvsp[0] , &yyvsp[-2] ); }
#line 2901 "Uil.c"
    break;

  case 12: /* module_clause: NAMES EQUAL_SIGN sensitivity_value  */
#line 261 "Uil.y"
                                                        { sar_process_module_sensitivity( &yyvsp[0] , &yyvsp[-2] ); }
#line 2907 "Uil.c"
    break;

  case 13: /* module_clause: CHARACTER_SET EQUAL_SIGN valid_charset  */
#line 262 "Uil.y"
                                                        { sar_process_module_charset( &yyvsp[0] , &yyvsp[-2] ); }
#line 2913 "Uil.c"
    break;

  case 14: /* $@4: %empty  */
#line 263 "Uil.y"
              { sar_make_def_obj (&yyvsp[0]); }
#line 2919 "Uil.c"
    break;

  case 20: /* $@5: %empty  */
#line 274 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 2925 "Uil.c"
    break;

  case 24: /* $@6: %empty  */
#line 280 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 2931 "Uil.c"
    break;

  case 26: /* $@7: %empty  */
#line 284 "Uil.y"
                 { sar_make_def_obj (&yyvsp[0]); }
#line 2937 "Uil.c"
    break;

  case 27: /* variant_clause: CLASS_NAME $@7 EQUAL_SIGN variant  */
#line 285 "Uil.y"
                                                        { sar_process_module_variant (&yyvsp[-3], &yyvsp[0]); yyval = gz_yynullval; }
#line 2943 "Uil.c"
    break;

  case 28: /* variant: WIDGET  */
#line 289 "Uil.y"
                                                        { yyval = gz_yynullval; yyval.b_type = sym_k_widget_entry; }
#line 2949 "Uil.c"
    break;

  case 29: /* variant: GADGET  */
#line 290 "Uil.y"
                                                        { yyval = gz_yynullval; yyval.b_type = sym_k_gadget_entry; }
#line 2955 "Uil.c"
    break;

  case 38: /* @8: %empty  */
#line 312 "Uil.y"
            { yyval = yyvsp[0]; sar_save_section_source (&yyvsp[0], sym_k_value_section); }
#line 2961 "Uil.c"
    break;

  case 42: /* $@9: %empty  */
#line 318 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 2967 "Uil.c"
    break;

  case 44: /* value_decl: id COLON value_definition SEMICOLON  */
#line 322 "Uil.y"
                                                        { sar_bind_value_name( &yyvsp[-3], &yyvsp[-1], &yyvsp[0] ); }
#line 2973 "Uil.c"
    break;

  case 45: /* value_definition: EXPORTED private_value  */
#line 326 "Uil.y"
                                                        { yyval = yyvsp[0]; yyval.b_flags |= sym_m_exported; yyval.b_flags &= ~sym_m_private; }
#line 2979 "Uil.c"
    break;

  case 46: /* value_definition: PRIVATE private_value_plus  */
#line 327 "Uil.y"
                                                        { yyval = yyvsp[0]; yyval.b_flags |= sym_m_private; yyval.b_flags &= ~sym_m_exported; }
#line 2985 "Uil.c"
    break;

  case 47: /* value_definition: private_value_plus  */
#line 328 "Uil.y"
                                                        { yyval = yyvsp[0]; yyval.b_flags |= sym_m_private; yyval.b_flags &= ~sym_m_exported; }
#line 2991 "Uil.c"
    break;

  case 48: /* value_definition: IMPORTED value_type  */
#line 329 "Uil.y"
                                                        { sar_import_value_entry( &yyval, &yyvsp[0] ); }
#line 2997 "Uil.c"
    break;

  case 49: /* value_type: INTEGER  */
#line 333 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_integer_value; }
#line 3003 "Uil.c"
    break;

  case 50: /* value_type: STRING  */
#line 334 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_char_8_value; }
#line 3009 "Uil.c"
    break;

  case 51: /* value_type: PIXMAP  */
#line 335 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_pixmap_value; }
#line 3015 "Uil.c"
    break;

  case 52: /* value_type: FLOAT  */
#line 336 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_float_value; }
#line 3021 "Uil.c"
    break;

  case 53: /* value_type: BOOLEAN  */
#line 337 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_bool_value; }
#line 3027 "Uil.c"
    break;

  case 54: /* value_type: FONT  */
#line 338 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_font_value; }
#line 3033 "Uil.c"
    break;

  case 55: /* value_type: REASON  */
#line 339 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_reason_value; }
#line 3039 "Uil.c"
    break;

  case 56: /* value_type: ARGUMENT  */
#line 340 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_argument_value; }
#line 3045 "Uil.c"
    break;

  case 57: /* value_type: COLOR  */
#line 341 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_color_value; }
#line 3051 "Uil.c"
    break;

  case 58: /* value_type: string_table_type  */
#line 342 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_string_table_value; }
#line 3057 "Uil.c"
    break;

  case 59: /* value_type: asciz_table_type  */
#line 343 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_asciz_table_value; }
#line 3063 "Uil.c"
    break;

  case 60: /* value_type: INTEGER_TABLE  */
#line 344 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_integer_table_value; }
#line 3069 "Uil.c"
    break;

  case 61: /* value_type: TRANSLATION_TABLE  */
#line 345 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_trans_table_value; }
#line 3075 "Uil.c"
    break;

  case 62: /* value_type: FONT_TABLE  */
#line 346 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_font_table_value; }
#line 3081 "Uil.c"
    break;

  case 63: /* value_type: ICON  */
#line 347 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_icon_value; }
#line 3087 "Uil.c"
    break;

  case 64: /* value_type: COMPOUND_STRING  */
#line 348 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_compound_string_value; }
#line 3093 "Uil.c"
    break;

  case 65: /* value_type: COMPOUND_STRING_COMPONENT  */
#line 349 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_compound_string_value; }
#line 3099 "Uil.c"
    break;

  case 66: /* value_type: CLASS_REC_NAME  */
#line 350 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_class_rec_name_value; }
#line 3105 "Uil.c"
    break;

  case 67: /* value_type: XBITMAPFILE  */
#line 351 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_xbitmapfile_value; }
#line 3111 "Uil.c"
    break;

  case 68: /* value_type: KEYSYM  */
#line 352 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_keysym_value; }
#line 3117 "Uil.c"
    break;

  case 69: /* value_type: SINGLE_FLOAT  */
#line 353 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_single_float_value; }
#line 3123 "Uil.c"
    break;

  case 70: /* value_type: RGB  */
#line 354 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_rgb_value; }
#line 3129 "Uil.c"
    break;

  case 71: /* value_type: WIDE_CHARACTER  */
#line 355 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_wchar_string_value; }
#line 3135 "Uil.c"
    break;

  case 72: /* value_type: FONTSET  */
#line 356 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_fontset_value; }
#line 3141 "Uil.c"
    break;

  case 73: /* value_type: WIDGET  */
#line 357 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_widget_ref_value; }
#line 3147 "Uil.c"
    break;

  case 74: /* arg_value_type: INTEGER  */
#line 362 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_integer_value; }
#line 3153 "Uil.c"
    break;

  case 75: /* arg_value_type: STRING  */
#line 363 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_char_8_value; }
#line 3159 "Uil.c"
    break;

  case 76: /* arg_value_type: PIXMAP  */
#line 364 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_pixmap_value; }
#line 3165 "Uil.c"
    break;

  case 77: /* arg_value_type: FLOAT  */
#line 365 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_float_value; }
#line 3171 "Uil.c"
    break;

  case 78: /* arg_value_type: BOOLEAN  */
#line 366 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_bool_value; }
#line 3177 "Uil.c"
    break;

  case 79: /* arg_value_type: FONT  */
#line 367 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_font_value; }
#line 3183 "Uil.c"
    break;

  case 80: /* arg_value_type: REASON  */
#line 368 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_reason_value; }
#line 3189 "Uil.c"
    break;

  case 81: /* arg_value_type: COLOR  */
#line 369 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_color_value; }
#line 3195 "Uil.c"
    break;

  case 82: /* arg_value_type: string_table_type  */
#line 370 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_string_table_value; }
#line 3201 "Uil.c"
    break;

  case 83: /* arg_value_type: asciz_table_type  */
#line 371 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_asciz_table_value; }
#line 3207 "Uil.c"
    break;

  case 84: /* arg_value_type: INTEGER_TABLE  */
#line 372 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_integer_table_value; }
#line 3213 "Uil.c"
    break;

  case 85: /* arg_value_type: TRANSLATION_TABLE  */
#line 373 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_trans_table_value; }
#line 3219 "Uil.c"
    break;

  case 86: /* arg_value_type: FONT_TABLE  */
#line 374 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_font_table_value; }
#line 3225 "Uil.c"
    break;

  case 87: /* arg_value_type: ICON  */
#line 375 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_icon_value; }
#line 3231 "Uil.c"
    break;

  case 88: /* arg_value_type: COMPOUND_STRING  */
#line 376 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_compound_string_value; }
#line 3237 "Uil.c"
    break;

  case 89: /* arg_value_type: KEYSYM  */
#line 377 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_keysym_value; }
#line 3243 "Uil.c"
    break;

  case 90: /* arg_value_type: SINGLE_FLOAT  */
#line 378 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_single_float_value; }
#line 3249 "Uil.c"
    break;

  case 91: /* arg_value_type: WIDE_CHARACTER  */
#line 379 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_wchar_string_value; }
#line 3255 "Uil.c"
    break;

  case 92: /* arg_value_type: FONTSET  */
#line 380 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_fontset_value; }
#line 3261 "Uil.c"
    break;

  case 93: /* arg_value_type: WIDGET  */
#line 381 "Uil.y"
                                { yyval = yyvsp[0]; yyval.b_type = sym_k_widget_ref_value; }
#line 3267 "Uil.c"
    break;

  case 98: /* @10: %empty  */
#line 398 "Uil.y"
                { yyval = yyvsp[0]; sar_save_section_source (&yyvsp[0], sym_k_procedure_section); }
#line 3273 "Uil.c"
    break;

  case 102: /* $@11: %empty  */
#line 404 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3279 "Uil.c"
    break;

  case 104: /* procedure_decl: id opt_formal_parameters opt_class SEMICOLON  */
#line 408 "Uil.y"
                                                        { sar_create_procedure( &yyvsp[-3], &yyvsp[-2], &yyvsp[-1], &yyvsp[0] );}
#line 3285 "Uil.c"
    break;

  case 105: /* opt_formal_parameters: %empty  */
#line 412 "Uil.y"
                                                        { yyval = gz_yynullval; }
#line 3291 "Uil.c"
    break;

  case 106: /* opt_formal_parameters: LEFT_PAREN formal_parameter_type RIGHT_PAREN  */
#line 413 "Uil.y"
                                                        { yyval = yyvsp[-1]; }
#line 3297 "Uil.c"
    break;

  case 107: /* formal_parameter_type: %empty  */
#line 417 "Uil.y"
                                                        { yyval = gz_yynullval; yyval.b_type = sym_k_no_value; }
#line 3303 "Uil.c"
    break;

  case 109: /* formal_parameter_type: ANY  */
#line 419 "Uil.y"
                                                        { yyval = yyvsp[0]; yyval.b_type = 
sym_k_any_value; }
#line 3310 "Uil.c"
    break;

  case 110: /* formal_parameter_type: CLASS_NAME  */
#line 422 "Uil.y"
        { YYSTYPE temp[2];
	  temp[0] = gz_yynullval;
	  sar_create_object(&temp[0], sym_k_widget_entry);
	  temp[1] = yyvsp[0]; sar_set_object_class(&temp[1]);
	  sar_verify_object(&temp[1]);
	  yyval = temp[0];
	}
#line 3322 "Uil.c"
    break;

  case 111: /* opt_class: %empty  */
#line 432 "Uil.y"
                                                        { yyval.b_flags = sym_m_private; }
#line 3328 "Uil.c"
    break;

  case 112: /* @12: %empty  */
#line 445 "Uil.y"
                 { yyval = yyvsp[0]; sar_save_section_source (&yyvsp[0], sym_k_identifier_section); }
#line 3334 "Uil.c"
    break;

  case 116: /* $@13: %empty  */
#line 451 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3340 "Uil.c"
    break;

  case 118: /* identifier_decl: id SEMICOLON  */
#line 455 "Uil.y"
                                                        { sar_create_identifier ( &yyvsp[-1], &yyvsp[0] ); }
#line 3346 "Uil.c"
    break;

  case 119: /* include_directive: INCLUDE UILFILE character_8_value SEMICOLON  */
#line 465 "Uil.y"
                                                        { sar_include_file (&yyvsp[-1], &yyvsp[-3], &yyvsp[0]); }
#line 3352 "Uil.c"
    break;

  case 120: /* $@14: %empty  */
#line 466 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3358 "Uil.c"
    break;

  case 122: /* @15: %empty  */
#line 474 "Uil.y"
             { yyval = yyvsp[0]; sar_save_section_source (&yyvsp[0], sym_k_object_section); }
#line 3364 "Uil.c"
    break;

  case 124: /* object_decl_list: object_decl SEMICOLON  */
#line 478 "Uil.y"
                                                        { yyval = yyvsp[-1]; sar_save_src_semicolon_pos (&yyvsp[0] ); }
#line 3370 "Uil.c"
    break;

  case 125: /* object_decl_list: object_decl_list object_decl SEMICOLON  */
#line 479 "Uil.y"
                                                        { yyval = yyvsp[-2]; sar_save_src_semicolon_pos (&yyvsp[0] ); }
#line 3376 "Uil.c"
    break;

  case 126: /* $@16: %empty  */
#line 480 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3382 "Uil.c"
    break;

  case 128: /* @17: %empty  */
#line 485 "Uil.y"
        { sar_create_object (&yyvsp[0], sym_k_widget_entry); 
	  sar_link_section (&yyvsp[0]); yyval = gz_yynullval;}
#line 3389 "Uil.c"
    break;

  case 129: /* object_decl: id @17 COLON object_definition  */
#line 488 "Uil.y"
        { yyval = gz_yynullval; }
#line 3395 "Uil.c"
    break;

  case 130: /* $@18: %empty  */
#line 493 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_exported); }
#line 3401 "Uil.c"
    break;

  case 132: /* $@19: %empty  */
#line 496 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_private); }
#line 3407 "Uil.c"
    break;

  case 134: /* $@20: %empty  */
#line 499 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_exported); }
#line 3413 "Uil.c"
    break;

  case 136: /* $@21: %empty  */
#line 502 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_imported); }
#line 3419 "Uil.c"
    break;

  case 137: /* $@22: %empty  */
#line 504 "Uil.y"
        { sar_set_object_class (&yyvsp[0]); }
#line 3425 "Uil.c"
    break;

  case 138: /* object_definition: IMPORTED $@21 CLASS_NAME $@22 opt_create_proc_ref epsilon_production  */
#line 514 "Uil.y"
        { sar_verify_object (&yyvsp[-5]); }
#line 3431 "Uil.c"
    break;

  case 139: /* $@23: %empty  */
#line 519 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_exported); }
#line 3437 "Uil.c"
    break;

  case 141: /* $@24: %empty  */
#line 522 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_private); }
#line 3443 "Uil.c"
    break;

  case 143: /* $@25: %empty  */
#line 525 "Uil.y"
        { sar_set_object_flags (& yyvsp[0], (sym_m_managed | sym_m_exported)); }
#line 3449 "Uil.c"
    break;

  case 145: /* $@26: %empty  */
#line 528 "Uil.y"
        { sar_unset_object_flags (& yyvsp[0], sym_m_managed), sar_set_object_flags (& yyvsp[0], sym_m_exported); }
#line 3455 "Uil.c"
    break;

  case 147: /* $@27: %empty  */
#line 531 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_exported); }
#line 3461 "Uil.c"
    break;

  case 149: /* $@28: %empty  */
#line 534 "Uil.y"
        { sar_set_object_flags (&yyvsp[0], sym_m_imported); }
#line 3467 "Uil.c"
    break;

  case 150: /* $@29: %empty  */
#line 536 "Uil.y"
        { sar_set_object_class (&yyvsp[0]); }
#line 3473 "Uil.c"
    break;

  case 151: /* control_object_definition: IMPORTED $@28 CLASS_NAME $@29 epsilon_production  */
#line 538 "Uil.y"
        { sar_verify_object (&yyvsp[-4]); }
#line 3479 "Uil.c"
    break;

  case 152: /* $@30: %empty  */
#line 543 "Uil.y"
        { sar_set_object_class (&yyvsp[0]); }
#line 3485 "Uil.c"
    break;

  case 153: /* object_specification: CLASS_NAME $@30 opt_create_proc_ref opt_variant object_spec  */
#line 547 "Uil.y"
        { sar_verify_object (&yyvsp[-4]); }
#line 3491 "Uil.c"
    break;

  case 154: /* opt_variant: epsilon_production  */
#line 552 "Uil.y"
        { sar_set_object_variant (&yyvsp[0]); }
#line 3497 "Uil.c"
    break;

  case 155: /* opt_variant: variant  */
#line 554 "Uil.y"
        { sar_set_object_variant (&yyvsp[0]); }
#line 3503 "Uil.c"
    break;

  case 156: /* @31: %empty  */
#line 559 "Uil.y"
        { sar_set_object_class (&yyvsp[0]); yyval = gz_yynullval;}
#line 3509 "Uil.c"
    break;

  case 157: /* control_object_specification: CLASS_NAME @31 opt_create_proc_ref opt_variant control_object_spec  */
#line 563 "Uil.y"
        { sar_verify_object (&yyvsp[-4]); }
#line 3515 "Uil.c"
    break;

  case 159: /* opt_create_proc_ref: PROCEDURE id_ref opt_parens  */
#line 569 "Uil.y"
        { sar_process_proc_ref (&yyvsp[-1], &gz_yynullval, sym_k_object_proc);
	  sar_save_user_proc_ref_src ( &yyvsp[-2], &yyvsp[-1], &yyvsp[0] ); }
#line 3522 "Uil.c"
    break;

  case 161: /* opt_parens: LEFT_PAREN RIGHT_PAREN  */
#line 575 "Uil.y"
                             { yyval = yyvsp[0]; }
#line 3528 "Uil.c"
    break;

  case 162: /* object_spec: id_ref  */
#line 585 "Uil.y"
        { sar_object_reference (&yyvsp[0]); }
#line 3534 "Uil.c"
    break;

  case 165: /* $@32: %empty  */
#line 588 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 3540 "Uil.c"
    break;

  case 167: /* control_object_spec: id_ref  */
#line 593 "Uil.y"
        { sar_object_reference (&yyvsp[0]); }
#line 3546 "Uil.c"
    break;

  case 168: /* control_object_spec: LEFT_BRACE RIGHT_BRACE epsilon_production  */
#line 596 "Uil.y"
        { sar_set_object_flags (&yyvsp[-2], sym_m_exported); }
#line 3552 "Uil.c"
    break;

  case 169: /* control_object_spec: LEFT_BRACE object_feature_list RIGHT_BRACE epsilon_production  */
#line 599 "Uil.y"
        { sar_set_object_flags (&yyvsp[-3], sym_m_exported); }
#line 3558 "Uil.c"
    break;

  case 170: /* $@33: %empty  */
#line 600 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 3564 "Uil.c"
    break;

  case 172: /* child_object_spec: LEFT_BRACE RIGHT_BRACE epsilon_production  */
#line 606 "Uil.y"
        { sar_set_object_flags (&yyvsp[-2], sym_m_private); }
#line 3570 "Uil.c"
    break;

  case 173: /* child_object_spec: LEFT_BRACE object_feature_list RIGHT_BRACE epsilon_production  */
#line 609 "Uil.y"
        { sar_set_object_flags (&yyvsp[-3], sym_m_private); }
#line 3576 "Uil.c"
    break;

  case 174: /* $@34: %empty  */
#line 610 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 3582 "Uil.c"
    break;

  case 178: /* object_feature: argument_list_def SEMICOLON  */
#line 620 "Uil.y"
        { sar_save_list_end ( &yyvsp[0]); sar_save_feature (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3588 "Uil.c"
    break;

  case 179: /* object_feature: callback_list_def SEMICOLON  */
#line 622 "Uil.y"
        { sar_save_list_end ( &yyvsp[0]); sar_save_feature (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3594 "Uil.c"
    break;

  case 180: /* object_feature: control_list_def SEMICOLON  */
#line 624 "Uil.y"
        { sar_save_list_end ( &yyvsp[0]); sar_save_feature (&yyvsp[-1]); 
sar_update_parent_list (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3601 "Uil.c"
    break;

  case 181: /* $@35: %empty  */
#line 626 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3607 "Uil.c"
    break;

  case 183: /* epsilon_production: %empty  */
#line 631 "Uil.y"
        { yyval = gz_yynullval; }
#line 3613 "Uil.c"
    break;

  case 184: /* @36: %empty  */
#line 639 "Uil.y"
           { yyval = yyvsp[0]; sar_save_section_source (&yyvsp[0], sym_k_list_section); }
#line 3619 "Uil.c"
    break;

  case 186: /* list_decl_list: list_decl SEMICOLON  */
#line 644 "Uil.y"
                          { yyval = yyvsp[-1]; sar_save_src_semicolon_pos (&yyvsp[0] ); }
#line 3625 "Uil.c"
    break;

  case 187: /* list_decl_list: list_decl_list list_decl SEMICOLON  */
#line 645 "Uil.y"
                                         { yyval = yyvsp[-2]; sar_save_src_semicolon_pos (&yyvsp[0] ); }
#line 3631 "Uil.c"
    break;

  case 188: /* $@37: %empty  */
#line 646 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3637 "Uil.c"
    break;

  case 190: /* @38: %empty  */
#line 651 "Uil.y"
        { sar_create_object (&yyvsp[0], sym_k_list_entry); 
sar_link_section (&yyvsp[0]); yyval = gz_yynullval;}
#line 3644 "Uil.c"
    break;

  case 191: /* list_decl: id @38 COLON list_specification  */
#line 654 "Uil.y"
        { yyval = gz_yynullval; }
#line 3650 "Uil.c"
    break;

  case 196: /* @39: %empty  */
#line 666 "Uil.y"
        { yyval = gz_yynullval; sar_set_list_type (&yyvsp[0]); }
#line 3656 "Uil.c"
    break;

  case 197: /* control_list_specification: control_list_type @39 control_list_spec  */
#line 668 "Uil.y"
        { sar_verify_object (&yyvsp[-2]); }
#line 3662 "Uil.c"
    break;

  case 198: /* @40: %empty  */
#line 673 "Uil.y"
        { yyval = gz_yynullval; sar_set_list_type (&yyvsp[0]); }
#line 3668 "Uil.c"
    break;

  case 199: /* argument_list_specification: argument_list_type @40 argument_list_spec  */
#line 675 "Uil.y"
        { sar_verify_object (&yyvsp[-2]); }
#line 3674 "Uil.c"
    break;

  case 200: /* @41: %empty  */
#line 680 "Uil.y"
        { yyval = gz_yynullval; sar_set_list_type (&yyvsp[0]); }
#line 3680 "Uil.c"
    break;

  case 201: /* callback_list_specification: callback_list_type @41 callback_list_spec  */
#line 682 "Uil.y"
        { sar_verify_object (&yyvsp[-2]); }
#line 3686 "Uil.c"
    break;

  case 202: /* @42: %empty  */
#line 687 "Uil.y"
        { yyval = gz_yynullval; sar_set_list_type (&yyvsp[0]); }
#line 3692 "Uil.c"
    break;

  case 203: /* procedure_list_specification: procedure_list_type @42 procedure_list_spec  */
#line 689 "Uil.y"
        { sar_verify_object (&yyvsp[-2]); }
#line 3698 "Uil.c"
    break;

  case 204: /* argument_list_type: ARGUMENTS  */
#line 694 "Uil.y"
        { yyval = gz_yynullval; yyval.b_type = sym_k_argument_list; }
#line 3704 "Uil.c"
    break;

  case 205: /* callback_list_type: CALLBACKS  */
#line 699 "Uil.y"
        { yyval = gz_yynullval; yyval.b_type = sym_k_callback_list; }
#line 3710 "Uil.c"
    break;

  case 206: /* procedure_list_type: PROCEDURES  */
#line 704 "Uil.y"
        { yyval = gz_yynullval; yyval.b_type = sym_k_proc_ref_list; }
#line 3716 "Uil.c"
    break;

  case 207: /* control_list_type: CONTROLS  */
#line 708 "Uil.y"
        { yyval = gz_yynullval; yyval.b_type = sym_k_control_list; }
#line 3722 "Uil.c"
    break;

  case 208: /* argument_list_spec: id_ref  */
#line 712 "Uil.y"
             { sar_object_reference (&yyvsp[0]); }
#line 3728 "Uil.c"
    break;

  case 211: /* $@43: %empty  */
#line 715 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 3734 "Uil.c"
    break;

  case 213: /* callback_list_spec: id_ref  */
#line 719 "Uil.y"
             { sar_object_reference (&yyvsp[0]); }
#line 3740 "Uil.c"
    break;

  case 216: /* $@44: %empty  */
#line 722 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 3746 "Uil.c"
    break;

  case 218: /* procedure_list_spec: id_ref  */
#line 727 "Uil.y"
        { sar_object_reference (&yyvsp[0]); }
#line 3752 "Uil.c"
    break;

  case 221: /* $@45: %empty  */
#line 730 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 3758 "Uil.c"
    break;

  case 223: /* control_list_spec: id_ref  */
#line 734 "Uil.y"
             { sar_object_reference (&yyvsp[0]); }
#line 3764 "Uil.c"
    break;

  case 226: /* $@46: %empty  */
#line 737 "Uil.y"
            { lex_issue_error (RIGHT_BRACE); }
#line 3770 "Uil.c"
    break;

  case 236: /* argument_list_clause: argument_list_def SEMICOLON  */
#line 762 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3776 "Uil.c"
    break;

  case 237: /* argument_list_clause: argument_list_item SEMICOLON  */
#line 764 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3782 "Uil.c"
    break;

  case 238: /* $@47: %empty  */
#line 765 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3788 "Uil.c"
    break;

  case 240: /* callback_list_clause: callback_list_def SEMICOLON  */
#line 770 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3794 "Uil.c"
    break;

  case 241: /* callback_list_clause: callback_list_item SEMICOLON  */
#line 772 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3800 "Uil.c"
    break;

  case 242: /* $@48: %empty  */
#line 773 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3806 "Uil.c"
    break;

  case 244: /* procedure_list_clause: procedure_list_def SEMICOLON  */
#line 778 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3812 "Uil.c"
    break;

  case 245: /* procedure_list_clause: procedure_list_def_ref SEMICOLON  */
#line 780 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3818 "Uil.c"
    break;

  case 246: /* $@49: %empty  */
#line 781 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3824 "Uil.c"
    break;

  case 248: /* control_list_clause: control_list_def SEMICOLON  */
#line 786 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3830 "Uil.c"
    break;

  case 249: /* control_list_clause: control_list_item SEMICOLON  */
#line 788 "Uil.y"
        { sar_save_src_entry_end (&yyvsp[0], &yyvsp[-1]); sar_add_list_entry (&yyvsp[-1]); yyval = gz_yynullval; }
#line 3836 "Uil.c"
    break;

  case 250: /* $@50: %empty  */
#line 789 "Uil.y"
            { lex_issue_error (SEMICOLON); }
#line 3842 "Uil.c"
    break;

  case 252: /* @51: %empty  */
#line 794 "Uil.y"
        { yyval = gz_yynullval; sar_create_object (&yyvsp[0], sym_k_list_entry); }
#line 3848 "Uil.c"
    break;

  case 253: /* control_list_def: epsilon_production @51 control_list_specification  */
#line 796 "Uil.y"
        { yyval = yyvsp[-2]; }
#line 3854 "Uil.c"
    break;

  case 254: /* @52: %empty  */
#line 801 "Uil.y"
        { yyval = gz_yynullval; sar_create_object (&yyvsp[0], sym_k_list_entry); }
#line 3860 "Uil.c"
    break;

  case 255: /* argument_list_def: epsilon_production @52 argument_list_specification  */
#line 803 "Uil.y"
        { yyval = yyvsp[-2]; }
#line 3866 "Uil.c"
    break;

  case 256: /* @53: %empty  */
#line 808 "Uil.y"
        { yyval = gz_yynullval; sar_create_object (&yyvsp[0], sym_k_list_entry); }
#line 3872 "Uil.c"
    break;

  case 257: /* callback_list_def: epsilon_production @53 callback_list_specification  */
#line 810 "Uil.y"
        { yyval = yyvsp[-2]; }
#line 3878 "Uil.c"
    break;

  case 258: /* @54: %empty  */
#line 815 "Uil.y"
        { yyval = gz_yynullval; sar_create_object (&yyvsp[0], sym_k_list_entry); }
#line 3884 "Uil.c"
    break;

  case 259: /* procedure_list_def: epsilon_production @54 procedure_list_specification  */
#line 817 "Uil.y"
        { yyval = yyvsp[-2]; }
#line 3890 "Uil.c"
    break;

  case 260: /* control_list_item: opt_managed control_item  */
#line 825 "Uil.y"
        { sar_save_control_item (&yyvsp[-1], &yyvsp[0]); yyval = yyvsp[-1]; }
#line 3896 "Uil.c"
    break;

  case 261: /* @55: %empty  */
#line 828 "Uil.y"
        { sar_create_object (& yyvsp[0], sym_k_widget_entry); yyvsp[0].b_flags = sym_m_managed; yyval = gz_yynullval;}
#line 3902 "Uil.c"
    break;

  case 262: /* control_list_item: id @55 COLON control_object_definition  */
#line 831 "Uil.y"
        { sar_save_control_widget (& yyvsp[-3], & yyvsp[-2]); yyval = yyvsp[-3]; }
#line 3908 "Uil.c"
    break;

  case 263: /* @56: %empty  */
#line 834 "Uil.y"
        { sar_create_child (& yyvsp[-1]); yyvsp[-1].b_flags = sym_m_managed; 
	  yyval = gz_yynullval; }
#line 3915 "Uil.c"
    break;

  case 264: /* control_list_item: CHILD_NAME opt_child_managed @56 child_object_spec  */
#line 837 "Uil.y"
        { sar_save_control_widget (& yyvsp[-3], & yyvsp[-2]); yyval = yyvsp[-3]; }
#line 3921 "Uil.c"
    break;

  case 265: /* @57: %empty  */
#line 839 "Uil.y"
        { sar_create_child (& yyvsp[-1]); yyval = gz_yynullval; }
#line 3927 "Uil.c"
    break;

  case 266: /* control_list_item: CHILD_NAME UNMANAGED @57 child_object_spec  */
#line 841 "Uil.y"
        { sar_save_control_widget (& yyvsp[-3], & yyvsp[-1]); yyval = yyvsp[-3]; }
#line 3933 "Uil.c"
    break;

  case 269: /* argument_list_item: value EQUAL_SIGN value  */
#line 855 "Uil.y"
        { sar_save_argument_pair (&yyvsp[-2], &yyvsp[0], &yyvsp[-1]); yyval = yyvsp[-2]; }
#line 3939 "Uil.c"
    break;

  case 270: /* argument_list_item: value EQUAL_SIGN CLASS_NAME id_ref  */
#line 860 "Uil.y"
        { YYSTYPE temp [2];
	  temp [0] = gz_yynullval;
	  sar_create_object (&temp [0], sym_k_widget_entry);
	  temp [1] = yyvsp[-1]; sar_set_object_class (&temp [1]);
	  temp [1] = yyvsp[0]; sar_object_reference (&temp [1]);
	  sar_verify_object (&temp [1]);
	  sar_save_argument_pair (&yyvsp[-3], &temp [0], &yyvsp[-2]); yyval = yyvsp[-3]; }
#line 3951 "Uil.c"
    break;

  case 271: /* callback_list_item: value EQUAL_SIGN procedure_reference  */
#line 874 "Uil.y"
        { sar_save_reason_binding (&yyvsp[-2], &yyvsp[0], &yyvsp[-1]); yyval = yyvsp[-2]; }
#line 3957 "Uil.c"
    break;

  case 272: /* callback_list_item: value EQUAL_SIGN procedure_list_def  */
#line 876 "Uil.y"
        { sar_save_reason_binding (&yyvsp[-2], &yyvsp[0], &yyvsp[-1]); yyval = yyvsp[-2]; }
#line 3963 "Uil.c"
    break;

  case 273: /* @58: %empty  */
#line 881 "Uil.y"
        { sar_create_object (&yyvsp[0], sym_k_widget_entry); yyval = gz_yynullval; }
#line 3969 "Uil.c"
    break;

  case 274: /* control_item: epsilon_production @58 control_object_specification  */
#line 883 "Uil.y"
        { yyval = yyvsp[-2]; }
#line 3975 "Uil.c"
    break;

  case 275: /* opt_managed: epsilon_production  */
#line 888 "Uil.y"
        { yyval.b_flags = sym_m_managed; }
#line 3981 "Uil.c"
    break;

  case 276: /* opt_managed: MANAGED  */
#line 890 "Uil.y"
        { yyval = gz_yynullval; yyval.b_flags = sym_m_managed; }
#line 3987 "Uil.c"
    break;

  case 277: /* opt_managed: UNMANAGED  */
#line 892 "Uil.y"
        { yyval = gz_yynullval; }
#line 3993 "Uil.c"
    break;

  case 278: /* procedure_reference: PROCEDURE id_ref opt_procedure_argument  */
#line 897 "Uil.y"
        { sar_process_proc_ref (&yyvsp[-1], &yyvsp[0], sym_k_callback_proc); yyval = yyvsp[-1]; }
#line 3999 "Uil.c"
    break;

  case 279: /* procedure_list_def_ref: id_ref opt_procedure_argument  */
#line 902 "Uil.y"
        { sar_process_proc_ref (&yyvsp[-1], &yyvsp[0], sym_k_callback_proc); yyval = yyvsp[-1]; }
#line 4005 "Uil.c"
    break;

  case 281: /* opt_procedure_argument: LEFT_PAREN value RIGHT_PAREN  */
#line 908 "Uil.y"
        { yyval = yyvsp[-1]; }
#line 4011 "Uil.c"
    break;

  case 282: /* opt_procedure_argument: LEFT_PAREN CLASS_NAME id_ref RIGHT_PAREN  */
#line 910 "Uil.y"
        { YYSTYPE temp [2];
	  temp [0] = gz_yynullval;
	  sar_create_object (&temp [0], sym_k_widget_entry);
	  temp [1] = yyvsp[-2]; sar_set_object_class (&temp [1]);
	  temp [1] = yyvsp[-1]; sar_object_reference (&temp [1]);
	  sar_verify_object (&temp [1]);
	  yyval = temp [0]; }
#line 4023 "Uil.c"
    break;

  case 283: /* opt_procedure_argument: LEFT_PAREN RIGHT_PAREN  */
#line 918 "Uil.y"
        { yyval = gz_yynullval; }
#line 4029 "Uil.c"
    break;

  case 291: /* id: keyword  */
#line 933 "Uil.y"
                                                        { sar_map_keyword_to_name( &yyval, &yyvsp[0]); }
#line 4035 "Uil.c"
    break;

  case 292: /* id_ref: id  */
#line 940 "Uil.y"
        { sar_process_id_ref (&yyvsp[0]); yyval = yyvsp[0]; }
#line 4041 "Uil.c"
    break;

  case 333: /* character_8_value: value  */
#line 998 "Uil.y"
               { 
                 yyval = yyvsp[0]; 
		 if ((yyval.b_type != sym_k_error_value) &&
		     (yyval.b_type != sym_k_any_value) &&
		    ((yyval.b_flags &sym_m_forward_ref) == 0))
		       {
			if ((yyval.b_type != sym_k_char_8_value) &&
			    (yyval.b_type != sym_k_localized_string_value))
		  	    sar_value_type_error( &yyval, sym_k_char_8_value );
			if ((yyval.b_flags &sym_m_private) == 0) 
			    sar_private_error( &yyval );
		       }
		}
#line 4059 "Uil.c"
    break;

  case 334: /* private_value: value  */
#line 1014 "Uil.y"
                                                                { yyval = yyvsp[0]; 
								  if (yyval.b_type != sym_k_error_value)
								  {
								    if (((yyval.b_flags &sym_m_private) == 0) &&
									((yyval.b_flags &sym_m_forward_ref) == 0))
									sar_private_error( &yyval );
								  }
 								}
#line 4072 "Uil.c"
    break;

  case 335: /* private_value_plus: non_export_value  */
#line 1027 "Uil.y"
                                                                { yyval = yyvsp[0]; 
 								  if (yyval.b_type != sym_k_error_value)
 								  {
 								    if (((yyval.b_flags &sym_m_private) == 0) &&
 									((yyval.b_flags &sym_m_forward_ref) == 0))
 									sar_private_error( &yyval );
 								  }
							}
#line 4085 "Uil.c"
    break;

  case 336: /* private_value_plus: value  */
#line 1035 "Uil.y"
                                                                { yyval = yyvsp[0]; 
 								  if (yyval.b_type != sym_k_error_value)
 								  {
 								    if (((yyval.b_flags &sym_m_private) == 0) &&
 									((yyval.b_flags &sym_m_forward_ref) == 0))
 									sar_private_error( &yyval );
 								  }
 								}
#line 4098 "Uil.c"
    break;

  case 337: /* non_export_value: COLOR_TABLE LEFT_PAREN color_list RIGHT_PAREN  */
#line 1045 "Uil.y"
                                                                { sar_make_color_table( &yyval, &yyvsp[-1], &yyvsp[-3]); }
#line 4104 "Uil.c"
    break;

  case 339: /* value: value OR value_1  */
#line 1052 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4110 "Uil.c"
    break;

  case 340: /* value: value XOR value_1  */
#line 1053 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4116 "Uil.c"
    break;

  case 342: /* value_1: value_1 AND value_2  */
#line 1058 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4122 "Uil.c"
    break;

  case 344: /* value_2: value_2 LEFT_SHIFT value_3  */
#line 1063 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4128 "Uil.c"
    break;

  case 345: /* value_2: value_2 RIGHT_SHIFT value_3  */
#line 1064 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4134 "Uil.c"
    break;

  case 347: /* value_3: value_3 PLUS value_4  */
#line 1069 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4140 "Uil.c"
    break;

  case 348: /* value_3: value_3 MINUS value_4  */
#line 1070 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4146 "Uil.c"
    break;

  case 350: /* value_4: value_4 MULTIPLY value_5  */
#line 1075 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4152 "Uil.c"
    break;

  case 351: /* value_4: value_4 DIVIDE value_5  */
#line 1076 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_binary_op( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4158 "Uil.c"
    break;

  case 353: /* value_5: MINUS value_5  */
#line 1081 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_unary_op( &yyval, &yyvsp[0] ); }
#line 4164 "Uil.c"
    break;

  case 354: /* value_5: NOT value_5  */
#line 1082 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_unary_op( &yyval, &yyvsp[0] ); }
#line 4170 "Uil.c"
    break;

  case 355: /* value_5: PLUS value_5  */
#line 1083 "Uil.y"
                                                                { yyval = yyvsp[-1]; sar_unary_op( &yyval, &yyvsp[0] ); }
#line 4176 "Uil.c"
    break;

  case 357: /* value_6: LEFT_PAREN value RIGHT_PAREN  */
#line 1088 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4182 "Uil.c"
    break;

  case 358: /* operand: UNS_INT_LITERAL  */
#line 1092 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[0], sym_k_integer_value, 
								  &yyvsp[0], XmPIXELS); }
#line 4189 "Uil.c"
    break;

  case 359: /* operand: UNS_INT_LITERAL units_specification  */
#line 1094 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[-1], sym_k_integer_value, 
								  &yyvsp[-1], sar_get_units_type(&yyvsp[0])); }
#line 4196 "Uil.c"
    break;

  case 360: /* operand: optional_charset CHAR_8_LITERAL  */
#line 1096 "Uil.y"
                                                                { /* Don't use FALSE because it is a token not a Boolean! */
								  Uil_lex_l_charset_specified = 0; 
								  sar_make_private_value( &yyval, &yyvsp[0], sym_k_char_8_value, 
								  &yyvsp[0], 0); }
#line 4205 "Uil.c"
    break;

  case 361: /* operand: id  */
#line 1100 "Uil.y"
                                                                { sar_process_id( &yyval, &yyvsp[0] );}
#line 4211 "Uil.c"
    break;

  case 362: /* operand: UNS_FLOAT_LITERAL  */
#line 1101 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[0], sym_k_float_value, 
								  &yyvsp[0], XmPIXELS); }
#line 4218 "Uil.c"
    break;

  case 363: /* operand: UNS_FLOAT_LITERAL units_specification  */
#line 1103 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[-1], sym_k_float_value, 
								  &yyvsp[-1], sar_get_units_type(&yyvsp[0])); }
#line 4225 "Uil.c"
    break;

  case 364: /* operand: PIXMAP LEFT_PAREN character_8_value RIGHT_PAREN  */
#line 1105 "Uil.y"
                                                                { sar_value_not_implemented( &yyval, &yyvsp[-3], "pixmap literal" ); }
#line 4231 "Uil.c"
    break;

  case 365: /* operand: FONT LEFT_PAREN font_spec RIGHT_PAREN  */
#line 1106 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4237 "Uil.c"
    break;

  case 366: /* operand: FONTSET LEFT_PAREN fontset_spec RIGHT_PAREN  */
#line 1107 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4243 "Uil.c"
    break;

  case 367: /* operand: COLOR LEFT_PAREN color_spec RIGHT_PAREN  */
#line 1108 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4249 "Uil.c"
    break;

  case 368: /* operand: REASON LEFT_PAREN character_8_value RIGHT_PAREN  */
#line 1109 "Uil.y"
                                                                { sar_make_private_value(&yyval, &yyvsp[-1], sym_k_reason_value, 
								  &yyvsp[-3], 0); }
#line 4256 "Uil.c"
    break;

  case 369: /* operand: CLASS_REC_NAME LEFT_PAREN character_8_value RIGHT_PAREN  */
#line 1111 "Uil.y"
                                                                { sar_make_private_value(&yyval, &yyvsp[-1], sym_k_class_rec_name_value,
											 &yyvsp[-3], 0); }
#line 4263 "Uil.c"
    break;

  case 370: /* operand: ARGUMENT LEFT_PAREN character_8_value opt_arg_type RIGHT_PAREN  */
#line 1114 "Uil.y"
                                                                { sar_make_private_value
								      ( &yyval, &yyvsp[-2], sym_k_argument_value, &yyvsp[-4], yyvsp[-1].b_type ); }
#line 4270 "Uil.c"
    break;

  case 371: /* operand: UILTRUE  */
#line 1116 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[0], sym_k_bool_value, 
								  &yyvsp[0], 0); }
#line 4277 "Uil.c"
    break;

  case 372: /* operand: UILFALSE  */
#line 1118 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[0], sym_k_bool_value, 
								  &yyvsp[0], 0); }
#line 4284 "Uil.c"
    break;

  case 373: /* operand: ON  */
#line 1120 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[0], sym_k_bool_value, 
								  &yyvsp[0], 0); }
#line 4291 "Uil.c"
    break;

  case 374: /* operand: OFF  */
#line 1122 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[0], sym_k_bool_value, 
								  &yyvsp[0], 0); }
#line 4298 "Uil.c"
    break;

  case 375: /* operand: optional_charset COMP_STRING  */
#line 1124 "Uil.y"
                                                                { /* Don't use FALSE because it is a token not a Boolean! */
								  Uil_lex_l_charset_specified = 0;
								  sar_make_private_value
								      ( &yyval, &yyvsp[0], sym_k_compound_string_value, &yyvsp[0], 0); }
#line 4307 "Uil.c"
    break;

  case 376: /* operand: string_table_type LEFT_PAREN comp_str_list RIGHT_PAREN  */
#line 1128 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[-1], sym_k_string_table_value, 
								  &yyvsp[-3], 0); }
#line 4314 "Uil.c"
    break;

  case 377: /* operand: asciz_table_type LEFT_PAREN asciz_list RIGHT_PAREN  */
#line 1130 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[-1], sym_k_asciz_table_value, 
								  &yyvsp[-3], 0); }
#line 4321 "Uil.c"
    break;

  case 378: /* operand: INTEGER_TABLE LEFT_PAREN integer_list RIGHT_PAREN  */
#line 1132 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[-1], sym_k_integer_table_value, 
								  &yyvsp[-3], 0); }
#line 4328 "Uil.c"
    break;

  case 379: /* operand: TRANSLATION_TABLE LEFT_PAREN string_list RIGHT_PAREN  */
#line 1134 "Uil.y"
                                                                { sar_make_private_value( &yyval, &yyvsp[-1], sym_k_trans_table_value, 
								  &yyvsp[-3], 0); }
#line 4335 "Uil.c"
    break;

  case 380: /* operand: COMPOUND_STRING LEFT_PAREN comp_str_result RIGHT_PAREN  */
#line 1136 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4341 "Uil.c"
    break;

  case 381: /* operand: COMPOUND_STRING_COMPONENT LEFT_PAREN comp_str_comp_result RIGHT_PAREN  */
#line 1137 "Uil.y"
                                                                                { yyval = yyvsp[-1]; }
#line 4347 "Uil.c"
    break;

  case 382: /* operand: FONT_TABLE LEFT_PAREN font_list RIGHT_PAREN  */
#line 1138 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4353 "Uil.c"
    break;

  case 383: /* operand: ICON LEFT_PAREN icon_spec RIGHT_PAREN  */
#line 1139 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4359 "Uil.c"
    break;

  case 384: /* operand: FLOAT LEFT_PAREN value RIGHT_PAREN  */
#line 1140 "Uil.y"
                                                                { yyval = yyvsp[-3]; sar_unary_op( &yyval, &yyvsp[-1] ); }
#line 4365 "Uil.c"
    break;

  case 385: /* operand: INTEGER LEFT_PAREN value RIGHT_PAREN  */
#line 1141 "Uil.y"
                                                                { yyval = yyvsp[-3]; sar_unary_op( &yyval, &yyvsp[-1] ); }
#line 4371 "Uil.c"
    break;

  case 386: /* operand: XBITMAPFILE LEFT_PAREN character_8_value RIGHT_PAREN  */
#line 1143 "Uil.y"
                                                                { sar_make_private_value(&yyval, &yyvsp[-1], sym_k_xbitmapfile_value,
											 &yyvsp[-3], 0); }
#line 4378 "Uil.c"
    break;

  case 387: /* operand: KEYSYM LEFT_PAREN character_8_value RIGHT_PAREN  */
#line 1146 "Uil.y"
                                                                { sar_make_private_value(&yyval, &yyvsp[-1], sym_k_keysym_value,
											 &yyvsp[-3], 0); }
#line 4385 "Uil.c"
    break;

  case 388: /* operand: SINGLE_FLOAT LEFT_PAREN value RIGHT_PAREN  */
#line 1149 "Uil.y"
                                                                { yyval = yyvsp[-3]; sar_unary_op( &yyval, &yyvsp[-1] ); }
#line 4391 "Uil.c"
    break;

  case 389: /* operand: RGB LEFT_PAREN rgb_list RIGHT_PAREN  */
#line 1151 "Uil.y"
                                                                { sar_make_private_value( & yyval, & yyvsp[-1], sym_k_rgb_value, 
								  &yyvsp[-3], 0); }
#line 4398 "Uil.c"
    break;

  case 390: /* operand: WIDE_CHARACTER LEFT_PAREN wchar_str_result RIGHT_PAREN  */
#line 1153 "Uil.y"
                                                                { yyval = yyvsp[-1]; }
#line 4404 "Uil.c"
    break;

  case 391: /* operand: LOC_STRING  */
#line 1154 "Uil.y"
                                                                { sar_make_private_value(&yyval, &yyvsp[0], sym_k_localized_string_value,
								  &yyvsp[0], 0); }
#line 4411 "Uil.c"
    break;

  case 392: /* string_list: %empty  */
#line 1160 "Uil.y"
        { yyval = gz_yynullval; }
#line 4417 "Uil.c"
    break;

  case 393: /* string_list: string_list_2  */
#line 1162 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4423 "Uil.c"
    break;

  case 394: /* string_list_2: private_value  */
#line 1167 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &gz_yynullval, sym_k_trans_table_value, &yyvsp[0]); yyval = yyvsp[0]; }
#line 4429 "Uil.c"
    break;

  case 395: /* string_list_2: string_list_2 COMMA private_value  */
#line 1169 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &yyvsp[-2], sym_k_trans_table_value, &yyvsp[-1]); yyval = yyvsp[0]; }
#line 4435 "Uil.c"
    break;

  case 396: /* asciz_list: %empty  */
#line 1174 "Uil.y"
        { yyval = gz_yynullval; }
#line 4441 "Uil.c"
    break;

  case 397: /* asciz_list: asciz_list_2  */
#line 1176 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4447 "Uil.c"
    break;

  case 398: /* asciz_list_2: private_value  */
#line 1181 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &gz_yynullval, sym_k_asciz_table_value, &yyvsp[0]); yyval = yyvsp[0]; }
#line 4453 "Uil.c"
    break;

  case 399: /* asciz_list_2: asciz_list_2 COMMA private_value  */
#line 1183 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &yyvsp[-2], sym_k_asciz_table_value, &yyvsp[-1]); yyval = yyvsp[0]; }
#line 4459 "Uil.c"
    break;

  case 400: /* integer_list: %empty  */
#line 1188 "Uil.y"
        { yyval = gz_yynullval; }
#line 4465 "Uil.c"
    break;

  case 401: /* integer_list: integer_list_2  */
#line 1190 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4471 "Uil.c"
    break;

  case 402: /* integer_list_2: private_value  */
#line 1195 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &gz_yynullval, sym_k_integer_table_value, &yyvsp[0]); yyval = yyvsp[0]; }
#line 4477 "Uil.c"
    break;

  case 403: /* integer_list_2: integer_list_2 COMMA private_value  */
#line 1197 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &yyvsp[-2], sym_k_integer_table_value, &yyvsp[-1]); yyval = yyvsp[0]; }
#line 4483 "Uil.c"
    break;

  case 404: /* rgb_list: private_value COMMA private_value COMMA private_value  */
#line 1203 "Uil.y"
{sar_append_table_value(&yyvsp[-4], & gz_yynullval, sym_k_integer_table_value, & yyvsp[-4]);
     sar_append_table_value(& yyvsp[-2], & yyvsp[-4], sym_k_integer_table_value, & yyvsp[-3]);
     sar_append_table_value(& yyvsp[0], & yyvsp[-2], sym_k_integer_table_value, & yyvsp[-1]);
     yyval = yyvsp[0];}
#line 4492 "Uil.c"
    break;

  case 405: /* comp_str_list: %empty  */
#line 1212 "Uil.y"
        { yyval = gz_yynullval; }
#line 4498 "Uil.c"
    break;

  case 406: /* comp_str_list: comp_str_list_2  */
#line 1214 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4504 "Uil.c"
    break;

  case 407: /* comp_str_list_2: private_value  */
#line 1219 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &gz_yynullval, sym_k_string_table_value, &yyvsp[0]); yyval = yyvsp[0]; }
#line 4510 "Uil.c"
    break;

  case 408: /* comp_str_list_2: comp_str_list_2 COMMA private_value  */
#line 1221 "Uil.y"
        { sar_append_table_value (&yyvsp[0], &yyvsp[-2], sym_k_string_table_value, &yyvsp[-1]); yyval = yyvsp[0]; }
#line 4516 "Uil.c"
    break;

  case 409: /* opt_arg_type: %empty  */
#line 1225 "Uil.y"
                                                        { yyval = gz_yynullval; yyval.b_type = sym_k_any_value; }
#line 4522 "Uil.c"
    break;

  case 410: /* opt_arg_type: COMMA arg_value_type  */
#line 1226 "Uil.y"
                                                                { yyval = gz_yynullval; yyval.b_type = yyvsp[0].b_type; }
#line 4528 "Uil.c"
    break;

  case 411: /* opt_arg_type: COMMA ANY  */
#line 1227 "Uil.y"
                                                        { yyval = gz_yynullval; yyval.b_type = sym_k_any_value; }
#line 4534 "Uil.c"
    break;

  case 412: /* comp_str_result: private_value  */
#line 1231 "Uil.y"
                                                        { sar_make_comp_str( &yyval, &yyvsp[0], &gz_yynullval, &yyvsp[(-1) - (1)] ); }
#line 4540 "Uil.c"
    break;

  case 413: /* comp_str_result: private_value comp_str_attrs  */
#line 1232 "Uil.y"
                                                        { sar_make_comp_str( &yyval, &yyvsp[-1], &yyvsp[0], &yyvsp[(-1) - (2)] ); }
#line 4546 "Uil.c"
    break;

  case 414: /* comp_str_comp_result: ENUMVAL_NAME  */
#line 1236 "Uil.y"
                        { sar_make_comp_str_comp(&yyval, &yyvsp[0], &gz_yynullval, &yyvsp[(-1) - (1)] ); }
#line 4552 "Uil.c"
    break;

  case 415: /* comp_str_comp_result: ENUMVAL_NAME COMMA ENUMVAL_NAME  */
#line 1237 "Uil.y"
                                        { sar_make_comp_str_comp( &yyval, &yyvsp[-2], &yyvsp[0], &yyvsp[(-1) - (3)] ); }
#line 4558 "Uil.c"
    break;

  case 416: /* comp_str_comp_result: ENUMVAL_NAME COMMA CHAR_8_LITERAL  */
#line 1238 "Uil.y"
                                        { sar_make_comp_str_comp( &yyval, &yyvsp[-2], &yyvsp[0], &yyvsp[(-1) - (3)] ); }
#line 4564 "Uil.c"
    break;

  case 417: /* comp_str_comp_result: ENUMVAL_NAME COMMA LOC_STRING  */
#line 1239 "Uil.y"
                                        { sar_make_comp_str_comp( &yyval, &yyvsp[-2], &yyvsp[0], &yyvsp[(-1) - (3)] ); }
#line 4570 "Uil.c"
    break;

  case 418: /* comp_str_comp_result: ENUMVAL_NAME COMMA CHARSET_NAME  */
#line 1240 "Uil.y"
                                        { sar_make_comp_str_comp( &yyval, &yyvsp[-2], &yyvsp[0], &yyvsp[(-1) - (3)] ); }
#line 4576 "Uil.c"
    break;

  case 419: /* comp_str_attrs: COMMA comp_str_attr  */
#line 1244 "Uil.y"
                                                        { sar_chk_comp_str_attr( &yyval, &yyvsp[0], &gz_yynullval ); }
#line 4582 "Uil.c"
    break;

  case 420: /* comp_str_attrs: comp_str_attrs COMMA comp_str_attr  */
#line 1245 "Uil.y"
                                                        { sar_chk_comp_str_attr( &yyval, &yyvsp[0], &yyvsp[-2] ); }
#line 4588 "Uil.c"
    break;

  case 421: /* comp_str_attr: CHARACTER_SET EQUAL_SIGN valid_charset  */
#line 1249 "Uil.y"
                                                        { yyval.b_type = yyvsp[-2].b_type; 
							  yyval.b_tag = yyvsp[0].b_tag;
							  yyval.value.az_keyword_entry = yyvsp[0].value.az_keyword_entry;
							  yyval.value.az_symbol_entry = yyvsp[0].value.az_symbol_entry; }
#line 4597 "Uil.c"
    break;

  case 422: /* comp_str_attr: RIGHT_TO_LEFT EQUAL_SIGN private_value  */
#line 1253 "Uil.y"
                                                        { yyval.b_type = yyvsp[-2].b_type; 
							  yyval.value.az_symbol_entry = yyvsp[0].value.az_symbol_entry; }
#line 4604 "Uil.c"
    break;

  case 423: /* comp_str_attr: SEPARATE EQUAL_SIGN private_value  */
#line 1255 "Uil.y"
                                                        { yyval.b_type = yyvsp[-2].b_type; 
							  yyval.value.az_symbol_entry = yyvsp[0].value.az_symbol_entry; }
#line 4611 "Uil.c"
    break;

  case 424: /* wchar_str_result: private_value  */
#line 1260 "Uil.y"
                                                        { sar_make_wchar_str( &yyval, &yyvsp[0], &gz_yynullval, &yyvsp[(-1) - (1)] ); }
#line 4617 "Uil.c"
    break;

  case 425: /* font_spec: character_8_value COMMA CHARACTER_SET EQUAL_SIGN valid_charset  */
#line 1265 "Uil.y"
                                                        { sar_make_font( &yyval, &yyvsp[0], &yyvsp[-4], &yyvsp[(-1) - (5)] ); }
#line 4623 "Uil.c"
    break;

  case 426: /* font_spec: character_8_value  */
#line 1266 "Uil.y"
                                                        { sar_make_font( &yyval, &gz_yynullval, &yyvsp[0], &yyvsp[(-1) - (1)] ); }
#line 4629 "Uil.c"
    break;

  case 427: /* fontset_spec: character_8_value COMMA CHARACTER_SET EQUAL_SIGN valid_charset  */
#line 1271 "Uil.y"
                                                        { sar_make_fontset( &yyval, &yyvsp[0], &yyvsp[-4], &yyvsp[(-1) - (5)] ); }
#line 4635 "Uil.c"
    break;

  case 428: /* fontset_spec: character_8_value  */
#line 1272 "Uil.y"
                                                        { sar_make_fontset( &yyval, &gz_yynullval, &yyvsp[0], &yyvsp[(-1) - (1)] ); }
#line 4641 "Uil.c"
    break;

  case 429: /* font_list: font_item  */
#line 1276 "Uil.y"
                                                        { sar_make_font_table( &yyval, &yyvsp[0], &gz_yynullval, &yyvsp[(-1) - (1)] ); }
#line 4647 "Uil.c"
    break;

  case 430: /* font_list: font_list COMMA font_item  */
#line 1277 "Uil.y"
                                                        { sar_make_font_table( &yyval, &yyvsp[0], &yyvsp[-2], &yyvsp[(-1) - (3)] ); }
#line 4653 "Uil.c"
    break;

  case 431: /* font_item: valid_charset EQUAL_SIGN private_value  */
#line 1284 "Uil.y"
                                                        { sar_make_font_item( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4659 "Uil.c"
    break;

  case 432: /* font_item: private_value  */
#line 1285 "Uil.y"
                                                        { sar_make_font_item( &yyval, &gz_yynullval, &yyvsp[0] ); }
#line 4665 "Uil.c"
    break;

  case 433: /* optional_charset: POUND valid_charset  */
#line 1289 "Uil.y"
                                                        { /* Don't use TRUE because it is a token not a Boolean! */
							  yyval = yyvsp[0]; 
							  Uil_lex_l_charset_specified = 1;}
#line 4673 "Uil.c"
    break;

  case 435: /* valid_charset: CHARSET_NAME  */
#line 1297 "Uil.y"
                                                        { sar_charset_verify (&yyvsp[0]); yyval = yyvsp[0]; }
#line 4679 "Uil.c"
    break;

  case 436: /* valid_charset: CHARACTER_SET LEFT_PAREN charset_info RIGHT_PAREN  */
#line 1298 "Uil.y"
                                                        { yyval = yyvsp[-1]; }
#line 4685 "Uil.c"
    break;

  case 437: /* valid_charset: NAME  */
#line 1299 "Uil.y"
                                { sar_make_fallback_charset(&yyvsp[0]);
				  yyval = yyvsp[0]; }
#line 4692 "Uil.c"
    break;

  case 438: /* charset_info: character_8_value  */
#line 1305 "Uil.y"
                                                        { sar_make_private_value(&yyval, &yyvsp[0], sym_k_char_8_value, 
							  &yyvsp[0], 0);
							  sar_make_charset (&yyval, &yyval, &gz_yynullval, &yyvsp[0]); }
#line 4700 "Uil.c"
    break;

  case 439: /* charset_info: character_8_value charset_attrs  */
#line 1308 "Uil.y"
                                                        { sar_make_private_value(&yyval, &yyvsp[-1], sym_k_char_8_value, 
							  &yyvsp[-1], 0);
							  sar_make_charset (&yyval, &yyval, &yyvsp[0], &yyvsp[-1]); }
#line 4708 "Uil.c"
    break;

  case 440: /* charset_attrs: COMMA charset_attr  */
#line 1314 "Uil.y"
                                                        { sar_chk_charset_attr( &yyval, &yyvsp[0], &gz_yynullval ); }
#line 4714 "Uil.c"
    break;

  case 441: /* charset_attrs: charset_attrs COMMA charset_attr  */
#line 1315 "Uil.y"
                                                        { sar_chk_charset_attr( &yyval, &yyvsp[0], &yyvsp[-2] ); }
#line 4720 "Uil.c"
    break;

  case 442: /* charset_attr: RIGHT_TO_LEFT EQUAL_SIGN private_value  */
#line 1319 "Uil.y"
                                                        { yyval.b_type = yyvsp[-2].b_type; 
							  if (yyvsp[0].b_type != sym_k_bool_value) 
							      sar_value_type_error( &yyvsp[0], sym_k_bool_value );
							  yyval.value.az_symbol_entry = yyvsp[0].value.az_symbol_entry; }
#line 4729 "Uil.c"
    break;

  case 443: /* charset_attr: SIXTEEN_BIT EQUAL_SIGN private_value  */
#line 1323 "Uil.y"
                                                        { yyval.b_type = yyvsp[-2].b_type; 
							  if (yyvsp[0].b_type != sym_k_bool_value) 
							      sar_value_type_error( &yyvsp[0], sym_k_bool_value );
							  yyval.value.az_symbol_entry = yyvsp[0].value.az_symbol_entry; }
#line 4738 "Uil.c"
    break;

  case 444: /* color_list: color_item  */
#line 1330 "Uil.y"
                                                        { sar_append_color_item( &yyval, &yyvsp[0], &gz_yynullval ); }
#line 4744 "Uil.c"
    break;

  case 445: /* color_list: color_list COMMA color_item  */
#line 1331 "Uil.y"
                                                        { sar_append_color_item( &yyval, &yyvsp[0], &yyvsp[-2] ); }
#line 4750 "Uil.c"
    break;

  case 446: /* color_item: BACKGROUND COLOR EQUAL_SIGN character_8_value  */
#line 1335 "Uil.y"
                                                        { sar_make_color_item( &yyval, &yyvsp[-3], &yyvsp[0] ); }
#line 4756 "Uil.c"
    break;

  case 447: /* color_item: FOREGROUND COLOR EQUAL_SIGN character_8_value  */
#line 1336 "Uil.y"
                                                        { sar_make_color_item( &yyval, &yyvsp[-3], &yyvsp[0] ); }
#line 4762 "Uil.c"
    break;

  case 448: /* color_item: value EQUAL_SIGN character_8_value  */
#line 1337 "Uil.y"
                                                        { sar_make_color_item( &yyval, &yyvsp[-2], &yyvsp[0] ); }
#line 4768 "Uil.c"
    break;

  case 449: /* color_spec: character_8_value  */
#line 1341 "Uil.y"
                                                        { sar_make_color( &yyval, &yyvsp[0], &gz_yynullval, &yyvsp[(-1) - (1)] ); }
#line 4774 "Uil.c"
    break;

  case 450: /* color_spec: character_8_value COMMA mono_color_spec  */
#line 1342 "Uil.y"
                                                        { sar_make_color( &yyval, &yyvsp[-2], &yyvsp[0], &yyvsp[(-1) - (3)] ); }
#line 4780 "Uil.c"
    break;

  case 453: /* icon_spec: icon_rows  */
#line 1351 "Uil.y"
                                                        { sar_make_icon( &yyval, &yyvsp[0], &gz_yynullval, &yyvsp[(-1) - (1)]); }
#line 4786 "Uil.c"
    break;

  case 454: /* icon_spec: COLOR_TABLE EQUAL_SIGN private_value COMMA icon_rows  */
#line 1353 "Uil.y"
                                                        { sar_make_icon( &yyval, &yyvsp[0], &yyvsp[-2], &yyvsp[(-1) - (5)] ); }
#line 4792 "Uil.c"
    break;

  case 455: /* icon_rows: private_value  */
#line 1358 "Uil.y"
                                { sar_append_table_value (&yyvsp[0], &gz_yynullval, sym_k_icon_value, &yyvsp[0]); yyval = yyvsp[0]; }
#line 4798 "Uil.c"
    break;

  case 456: /* icon_rows: icon_rows COMMA private_value  */
#line 1360 "Uil.y"
                                { sar_append_table_value (&yyvsp[0], &yyvsp[-2], sym_k_icon_value, &yyvsp[-1]); yyval = yyvsp[0]; }
#line 4804 "Uil.c"
    break;

  case 457: /* units_specification: PIX  */
#line 1365 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4810 "Uil.c"
    break;

  case 458: /* units_specification: PIXEL  */
#line 1367 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4816 "Uil.c"
    break;

  case 459: /* units_specification: PIXELS  */
#line 1369 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4822 "Uil.c"
    break;

  case 460: /* units_specification: IN  */
#line 1371 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4828 "Uil.c"
    break;

  case 461: /* units_specification: INCH  */
#line 1373 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4834 "Uil.c"
    break;

  case 462: /* units_specification: INCHES  */
#line 1375 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4840 "Uil.c"
    break;

  case 463: /* units_specification: CM  */
#line 1377 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4846 "Uil.c"
    break;

  case 464: /* units_specification: CENTIMETER  */
#line 1379 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4852 "Uil.c"
    break;

  case 465: /* units_specification: CENTIMETERS  */
#line 1381 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4858 "Uil.c"
    break;

  case 466: /* units_specification: MM  */
#line 1383 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4864 "Uil.c"
    break;

  case 467: /* units_specification: MILLIMETER  */
#line 1385 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4870 "Uil.c"
    break;

  case 468: /* units_specification: MILLIMETERS  */
#line 1387 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4876 "Uil.c"
    break;

  case 469: /* units_specification: PT  */
#line 1389 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4882 "Uil.c"
    break;

  case 470: /* units_specification: POINT  */
#line 1391 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4888 "Uil.c"
    break;

  case 471: /* units_specification: POINTS  */
#line 1393 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4894 "Uil.c"
    break;

  case 472: /* units_specification: FU  */
#line 1395 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4900 "Uil.c"
    break;

  case 473: /* units_specification: FONT_UNIT  */
#line 1397 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4906 "Uil.c"
    break;

  case 474: /* units_specification: FONT_UNITS  */
#line 1399 "Uil.y"
        { yyval = yyvsp[0]; }
#line 4912 "Uil.c"
    break;


#line 4916 "Uil.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= UILEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == UILEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1402 "Uil.y"


/*    Dummy error routine for the parser.  We will output our own
      error messages.    */

int yyerror (s)
char	* s;
{
	return 0;
}



