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
