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
#line 34 "wmlparse.y"


#include "wml.h"

#if defined(__STDC__)
#include <string.h>		/* for strcpy() */
#endif

#ifndef XmConst
#if defined(__STDC__) || !defined( NO_CONST )
#define XmConst const
#else
#define XmConst
#endif /* __STDC__ */
#endif /* XmConst */

#if !defined(__STDC__)
/*
 * Undefine NULL, since it is defined in stdio
 */
#undef NULL
#endif


#line 96 "wmlparse.c"

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
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_CLASS = 3,                      /* CLASS  */
  YYSYMBOL_RESOURCE = 4,                   /* RESOURCE  */
  YYSYMBOL_DATATYPE = 5,                   /* DATATYPE  */
  YYSYMBOL_CONTROLLIST = 6,                /* CONTROLLIST  */
  YYSYMBOL_ENUMERATIONSET = 7,             /* ENUMERATIONSET  */
  YYSYMBOL_ENUMERATIONVALUE = 8,           /* ENUMERATIONVALUE  */
  YYSYMBOL_CHARACTERSET = 9,               /* CHARACTERSET  */
  YYSYMBOL_CHILD = 10,                     /* CHILD  */
  YYSYMBOL_DOCNAME = 11,                   /* DOCNAME  */
  YYSYMBOL_CONVFUNC = 12,                  /* CONVFUNC  */
  YYSYMBOL_ALIAS = 13,                     /* ALIAS  */
  YYSYMBOL_TYPE = 14,                      /* TYPE  */
  YYSYMBOL_RESOURCELITERAL = 15,           /* RESOURCELITERAL  */
  YYSYMBOL_RELATED = 16,                   /* RELATED  */
  YYSYMBOL_INTERNALLITERAL = 17,           /* INTERNALLITERAL  */
  YYSYMBOL_XRMRESOURCE = 18,               /* XRMRESOURCE  */
  YYSYMBOL_EXCLUDE = 19,                   /* EXCLUDE  */
  YYSYMBOL_RESOURCES = 20,                 /* RESOURCES  */
  YYSYMBOL_SUPERCLASS = 21,                /* SUPERCLASS  */
  YYSYMBOL_CONTROLS = 22,                  /* CONTROLS  */
  YYSYMBOL_WIDGETCLASS = 23,               /* WIDGETCLASS  */
  YYSYMBOL_DIALOGCLASS = 24,               /* DIALOGCLASS  */
  YYSYMBOL_DEFAULT = 25,                   /* DEFAULT  */
  YYSYMBOL_ENUMLITERAL = 26,               /* ENUMLITERAL  */
  YYSYMBOL_XMSTRINGCHARSETNAME = 27,       /* XMSTRINGCHARSETNAME  */
  YYSYMBOL_DIRECTION = 28,                 /* DIRECTION  */
  YYSYMBOL_PARSEDIRECTION = 29,            /* PARSEDIRECTION  */
  YYSYMBOL_CHARACTERSIZE = 30,             /* CHARACTERSIZE  */
  YYSYMBOL_CTRLMAPSRESOURCE = 31,          /* CTRLMAPSRESOURCE  */
  YYSYMBOL_CHILDREN = 32,                  /* CHILDREN  */
  YYSYMBOL_PARENTCLASS = 33,               /* PARENTCLASS  */
  YYSYMBOL_METACLASS = 34,                 /* METACLASS  */
  YYSYMBOL_WIDGET = 35,                    /* WIDGET  */
  YYSYMBOL_GADGET = 36,                    /* GADGET  */
  YYSYMBOL_ARGUMENT = 37,                  /* ARGUMENT  */
  YYSYMBOL_REASON = 38,                    /* REASON  */
  YYSYMBOL_CONSTRAINT = 39,                /* CONSTRAINT  */
  YYSYMBOL_SUBRESOURCE = 40,               /* SUBRESOURCE  */
  YYSYMBOL_ATTRTRUE = 41,                  /* ATTRTRUE  */
  YYSYMBOL_ATTRFALSE = 42,                 /* ATTRFALSE  */
  YYSYMBOL_LEFTTORIGHT = 43,               /* LEFTTORIGHT  */
  YYSYMBOL_RIGHTTOLEFT = 44,               /* RIGHTTOLEFT  */
  YYSYMBOL_ONEBYTE = 45,                   /* ONEBYTE  */
  YYSYMBOL_TWOBYTE = 46,                   /* TWOBYTE  */
  YYSYMBOL_MIXED1_2BYTE = 47,              /* MIXED1_2BYTE  */
  YYSYMBOL_COLON = 48,                     /* COLON  */
  YYSYMBOL_SEMICOLON = 49,                 /* SEMICOLON  */
  YYSYMBOL_EQUALS = 50,                    /* EQUALS  */
  YYSYMBOL_LBRACE = 51,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 52,                    /* RBRACE  */
  YYSYMBOL_STRING = 53,                    /* STRING  */
  YYSYMBOL_ERRORTOKEN = 54,                /* ERRORTOKEN  */
  YYSYMBOL_YYACCEPT = 55,                  /* $accept  */
  YYSYMBOL_statement_block_list = 56,      /* statement_block_list  */
  YYSYMBOL_statement_block = 57,           /* statement_block  */
  YYSYMBOL_58_1 = 58,                      /* $@1  */
  YYSYMBOL_class_statement_block = 59,     /* class_statement_block  */
  YYSYMBOL_class_statement_list = 60,      /* class_statement_list  */
  YYSYMBOL_61_2 = 61,                      /* $@2  */
  YYSYMBOL_resource_statement_block = 62,  /* resource_statement_block  */
  YYSYMBOL_resource_statement_list = 63,   /* resource_statement_list  */
  YYSYMBOL_64_3 = 64,                      /* $@3  */
  YYSYMBOL_child_statement_block = 65,     /* child_statement_block  */
  YYSYMBOL_child_statement_list = 66,      /* child_statement_list  */
  YYSYMBOL_67_4 = 67,                      /* $@4  */
  YYSYMBOL_datatype_statement_block = 68,  /* datatype_statement_block  */
  YYSYMBOL_datatype_statement_list = 69,   /* datatype_statement_list  */
  YYSYMBOL_70_5 = 70,                      /* $@5  */
  YYSYMBOL_ctrlist_statement_block = 71,   /* ctrlist_statement_block  */
  YYSYMBOL_ctrlist_statement_list = 72,    /* ctrlist_statement_list  */
  YYSYMBOL_73_6 = 73,                      /* $@6  */
  YYSYMBOL_enumset_statement_block = 74,   /* enumset_statement_block  */
  YYSYMBOL_enumset_statement_list = 75,    /* enumset_statement_list  */
  YYSYMBOL_76_7 = 76,                      /* $@7  */
  YYSYMBOL_enumval_statement_block = 77,   /* enumval_statement_block  */
  YYSYMBOL_enumval_statement_list = 78,    /* enumval_statement_list  */
  YYSYMBOL_79_8 = 79,                      /* $@8  */
  YYSYMBOL_charset_statement_block = 80,   /* charset_statement_block  */
  YYSYMBOL_charset_statement_list = 81,    /* charset_statement_list  */
  YYSYMBOL_82_9 = 82,                      /* $@9  */
  YYSYMBOL_class_statement = 83,           /* class_statement  */
  YYSYMBOL_84_10 = 84,                     /* $@10  */
  YYSYMBOL_class_type = 85,                /* class_type  */
  YYSYMBOL_class_definition = 86,          /* class_definition  */
  YYSYMBOL_87_11 = 87,                     /* $@11  */
  YYSYMBOL_class_attribute_list = 88,      /* class_attribute_list  */
  YYSYMBOL_class_attribute = 89,           /* class_attribute  */
  YYSYMBOL_90_12 = 90,                     /* $@12  */
  YYSYMBOL_91_13 = 91,                     /* $@13  */
  YYSYMBOL_92_14 = 92,                     /* $@14  */
  YYSYMBOL_class_attribute_name = 93,      /* class_attribute_name  */
  YYSYMBOL_boolean_class_attribute_name = 94, /* boolean_class_attribute_name  */
  YYSYMBOL_boolean_attribute_value = 95,   /* boolean_attribute_value  */
  YYSYMBOL_class_resources = 96,           /* class_resources  */
  YYSYMBOL_class_resources_block = 97,     /* class_resources_block  */
  YYSYMBOL_98_15 = 98,                     /* $@15  */
  YYSYMBOL_class_resource_list = 99,       /* class_resource_list  */
  YYSYMBOL_class_resource_element = 100,   /* class_resource_element  */
  YYSYMBOL_101_16 = 101,                   /* $@16  */
  YYSYMBOL_102_17 = 102,                   /* $@17  */
  YYSYMBOL_class_resource_attributes = 103, /* class_resource_attributes  */
  YYSYMBOL_104_18 = 104,                   /* $@18  */
  YYSYMBOL_class_resource_attribute_list = 105, /* class_resource_attribute_list  */
  YYSYMBOL_class_resource_attribute_element = 106, /* class_resource_attribute_element  */
  YYSYMBOL_107_19 = 107,                   /* $@19  */
  YYSYMBOL_108_20 = 108,                   /* $@20  */
  YYSYMBOL_109_21 = 109,                   /* $@21  */
  YYSYMBOL_class_resource_attribute_name = 110, /* class_resource_attribute_name  */
  YYSYMBOL_boolean_class_resource_attribute_name = 111, /* boolean_class_resource_attribute_name  */
  YYSYMBOL_class_controls = 112,           /* class_controls  */
  YYSYMBOL_class_controls_block = 113,     /* class_controls_block  */
  YYSYMBOL_114_22 = 114,                   /* $@22  */
  YYSYMBOL_class_controls_list = 115,      /* class_controls_list  */
  YYSYMBOL_class_controls_element = 116,   /* class_controls_element  */
  YYSYMBOL_117_23 = 117,                   /* $@23  */
  YYSYMBOL_118_24 = 118,                   /* $@24  */
  YYSYMBOL_class_controls_attributes = 119, /* class_controls_attributes  */
  YYSYMBOL_120_25 = 120,                   /* $@25  */
  YYSYMBOL_class_children = 121,           /* class_children  */
  YYSYMBOL_class_children_block = 122,     /* class_children_block  */
  YYSYMBOL_123_26 = 123,                   /* $@26  */
  YYSYMBOL_class_children_list = 124,      /* class_children_list  */
  YYSYMBOL_class_children_element = 125,   /* class_children_element  */
  YYSYMBOL_126_27 = 126,                   /* $@27  */
  YYSYMBOL_127_28 = 127,                   /* $@28  */
  YYSYMBOL_resource_statement = 128,       /* resource_statement  */
  YYSYMBOL_129_29 = 129,                   /* $@29  */
  YYSYMBOL_resource_type = 130,            /* resource_type  */
  YYSYMBOL_resource_definition = 131,      /* resource_definition  */
  YYSYMBOL_132_30 = 132,                   /* $@30  */
  YYSYMBOL_resource_attribute_list = 133,  /* resource_attribute_list  */
  YYSYMBOL_resource_attribute = 134,       /* resource_attribute  */
  YYSYMBOL_135_31 = 135,                   /* $@31  */
  YYSYMBOL_136_32 = 136,                   /* $@32  */
  YYSYMBOL_137_33 = 137,                   /* $@33  */
  YYSYMBOL_resource_attribute_name = 138,  /* resource_attribute_name  */
  YYSYMBOL_boolean_resource_resource_attribute_name = 139, /* boolean_resource_resource_attribute_name  */
  YYSYMBOL_child_statement = 140,          /* child_statement  */
  YYSYMBOL_datatype_statement = 141,       /* datatype_statement  */
  YYSYMBOL_142_34 = 142,                   /* $@34  */
  YYSYMBOL_datatype_definition = 143,      /* datatype_definition  */
  YYSYMBOL_144_35 = 144,                   /* $@35  */
  YYSYMBOL_datatype_attribute_list = 145,  /* datatype_attribute_list  */
  YYSYMBOL_datatype_attribute = 146,       /* datatype_attribute  */
  YYSYMBOL_147_36 = 147,                   /* $@36  */
  YYSYMBOL_148_37 = 148,                   /* $@37  */
  YYSYMBOL_datatype_attribute_name = 149,  /* datatype_attribute_name  */
  YYSYMBOL_ctrlist_statement = 150,        /* ctrlist_statement  */
  YYSYMBOL_151_38 = 151,                   /* $@38  */
  YYSYMBOL_ctrlist_definition = 152,       /* ctrlist_definition  */
  YYSYMBOL_153_39 = 153,                   /* $@39  */
  YYSYMBOL_ctrlist_controls_list = 154,    /* ctrlist_controls_list  */
  YYSYMBOL_ctrlist_control = 155,          /* ctrlist_control  */
  YYSYMBOL_156_40 = 156,                   /* $@40  */
  YYSYMBOL_157_41 = 157,                   /* $@41  */
  YYSYMBOL_ctrlist_controls_attributes = 158, /* ctrlist_controls_attributes  */
  YYSYMBOL_159_42 = 159,                   /* $@42  */
  YYSYMBOL_enumset_statement = 160,        /* enumset_statement  */
  YYSYMBOL_161_43 = 161,                   /* $@43  */
  YYSYMBOL_enumset_data_type = 162,        /* enumset_data_type  */
  YYSYMBOL_enumset_definition = 163,       /* enumset_definition  */
  YYSYMBOL_164_44 = 164,                   /* $@44  */
  YYSYMBOL_enumset_value_list = 165,       /* enumset_value_list  */
  YYSYMBOL_enumset_value = 166,            /* enumset_value  */
  YYSYMBOL_167_45 = 167,                   /* $@45  */
  YYSYMBOL_168_46 = 168,                   /* $@46  */
  YYSYMBOL_enumval_statement = 169,        /* enumval_statement  */
  YYSYMBOL_170_47 = 170,                   /* $@47  */
  YYSYMBOL_enumvalue_definition = 171,     /* enumvalue_definition  */
  YYSYMBOL_172_48 = 172,                   /* $@48  */
  YYSYMBOL_enumvalue_attributes_list = 173, /* enumvalue_attributes_list  */
  YYSYMBOL_enumvalue_attribute = 174,      /* enumvalue_attribute  */
  YYSYMBOL_175_49 = 175,                   /* $@49  */
  YYSYMBOL_176_50 = 176,                   /* $@50  */
  YYSYMBOL_enumvalue_attribute_name = 177, /* enumvalue_attribute_name  */
  YYSYMBOL_charset_statement = 178,        /* charset_statement  */
  YYSYMBOL_179_51 = 179,                   /* $@51  */
  YYSYMBOL_charset_definition = 180,       /* charset_definition  */
  YYSYMBOL_181_52 = 181,                   /* $@52  */
  YYSYMBOL_charset_attribute_list = 182,   /* charset_attribute_list  */
  YYSYMBOL_charset_attribute = 183,        /* charset_attribute  */
  YYSYMBOL_184_53 = 184,                   /* $@53  */
  YYSYMBOL_185_54 = 185,                   /* $@54  */
  YYSYMBOL_186_55 = 186,                   /* $@55  */
  YYSYMBOL_charset_attribute_name = 187,   /* charset_attribute_name  */
  YYSYMBOL_predefined_charset_attribute_name = 188, /* predefined_charset_attribute_name  */
  YYSYMBOL_predefined_charset_attribute_value = 189, /* predefined_charset_attribute_value  */
  YYSYMBOL_name = 190                      /* name  */
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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   339

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  136
/* YYNRULES -- Number of rules.  */
#define YYNRULES  258
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  387

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
       0,     3,     4,     5,     6,     7,     8,     9,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      10,    32,    33,     2,     2,     2,     2,     2,     2,     2,
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
       0,   161,   161,   162,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   174,   183,   188,   189,   190,   190,   194,
     199,   200,   201,   201,   205,   210,   211,   212,   212,   216,
     221,   222,   223,   223,   227,   232,   233,   234,   234,   238,
     243,   244,   245,   245,   249,   254,   255,   256,   256,   260,
     265,   266,   267,   267,   279,   276,   284,   285,   286,   290,
     291,   292,   293,   293,   297,   298,   305,   302,   310,   307,
     312,   313,   314,   315,   315,   319,   320,   321,   322,   323,
     324,   325,   326,   330,   334,   335,   339,   343,   344,   345,
     346,   346,   350,   351,   356,   355,   359,   359,   363,   364,
     365,   366,   366,   370,   371,   378,   375,   383,   380,   385,
     385,   389,   390,   394,   398,   402,   403,   404,   405,   405,
     409,   410,   415,   414,   418,   418,   422,   423,   424,   424,
     428,   432,   433,   434,   435,   435,   439,   440,   445,   444,
     447,   447,   459,   456,   464,   465,   466,   467,   471,   472,
     473,   474,   474,   478,   479,   486,   483,   491,   488,   493,
     493,   497,   498,   499,   500,   501,   502,   503,   504,   508,
     516,   526,   525,   531,   532,   533,   534,   534,   538,   539,
     546,   543,   548,   548,   552,   553,   554,   564,   563,   569,
     570,   571,   572,   572,   576,   577,   582,   581,   585,   585,
     589,   590,   591,   591,   603,   600,   609,   613,   614,   615,
     616,   616,   620,   621,   626,   625,   628,   628,   639,   637,
     644,   645,   646,   647,   647,   651,   652,   659,   656,   661,
     661,   665,   675,   674,   680,   681,   682,   683,   683,   687,
     688,   695,   692,   700,   697,   702,   702,   706,   707,   708,
     712,   713,   714,   718,   719,   720,   721,   722,   730
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
  "\"end of file\"", "error", "\"invalid token\"", "CLASS", "RESOURCE",
  "DATATYPE", "CONTROLLIST", "ENUMERATIONSET", "ENUMERATIONVALUE",
  "CHARACTERSET", "CHILD", "DOCNAME", "CONVFUNC", "ALIAS", "TYPE",
  "RESOURCELITERAL", "RELATED", "INTERNALLITERAL", "XRMRESOURCE",
  "EXCLUDE", "RESOURCES", "SUPERCLASS", "CONTROLS", "WIDGETCLASS",
  "DIALOGCLASS", "DEFAULT", "ENUMLITERAL", "XMSTRINGCHARSETNAME",
  "DIRECTION", "PARSEDIRECTION", "CHARACTERSIZE", "CTRLMAPSRESOURCE",
  "CHILDREN", "PARENTCLASS", "METACLASS", "WIDGET", "GADGET", "ARGUMENT",
  "REASON", "CONSTRAINT", "SUBRESOURCE", "ATTRTRUE", "ATTRFALSE",
  "LEFTTORIGHT", "RIGHTTOLEFT", "ONEBYTE", "TWOBYTE", "MIXED1_2BYTE",
  "COLON", "SEMICOLON", "EQUALS", "LBRACE", "RBRACE", "STRING",
  "ERRORTOKEN", "$accept", "statement_block_list", "statement_block",
  "$@1", "class_statement_block", "class_statement_list", "$@2",
  "resource_statement_block", "resource_statement_list", "$@3",
  "child_statement_block", "child_statement_list", "$@4",
  "datatype_statement_block", "datatype_statement_list", "$@5",
  "ctrlist_statement_block", "ctrlist_statement_list", "$@6",
  "enumset_statement_block", "enumset_statement_list", "$@7",
  "enumval_statement_block", "enumval_statement_list", "$@8",
  "charset_statement_block", "charset_statement_list", "$@9",
  "class_statement", "$@10", "class_type", "class_definition", "$@11",
  "class_attribute_list", "class_attribute", "$@12", "$@13", "$@14",
  "class_attribute_name", "boolean_class_attribute_name",
  "boolean_attribute_value", "class_resources", "class_resources_block",
  "$@15", "class_resource_list", "class_resource_element", "$@16", "$@17",
  "class_resource_attributes", "$@18", "class_resource_attribute_list",
  "class_resource_attribute_element", "$@19", "$@20", "$@21",
  "class_resource_attribute_name", "boolean_class_resource_attribute_name",
  "class_controls", "class_controls_block", "$@22", "class_controls_list",
  "class_controls_element", "$@23", "$@24", "class_controls_attributes",
  "$@25", "class_children", "class_children_block", "$@26",
  "class_children_list", "class_children_element", "$@27", "$@28",
  "resource_statement", "$@29", "resource_type", "resource_definition",
  "$@30", "resource_attribute_list", "resource_attribute", "$@31", "$@32",
  "$@33", "resource_attribute_name",
  "boolean_resource_resource_attribute_name", "child_statement",
  "datatype_statement", "$@34", "datatype_definition", "$@35",
  "datatype_attribute_list", "datatype_attribute", "$@36", "$@37",
  "datatype_attribute_name", "ctrlist_statement", "$@38",
  "ctrlist_definition", "$@39", "ctrlist_controls_list", "ctrlist_control",
  "$@40", "$@41", "ctrlist_controls_attributes", "$@42",
  "enumset_statement", "$@43", "enumset_data_type", "enumset_definition",
  "$@44", "enumset_value_list", "enumset_value", "$@45", "$@46",
  "enumval_statement", "$@47", "enumvalue_definition", "$@48",
  "enumvalue_attributes_list", "enumvalue_attribute", "$@49", "$@50",
  "enumvalue_attribute_name", "charset_statement", "$@51",
  "charset_definition", "$@52", "charset_attribute_list",
  "charset_attribute", "$@53", "$@54", "$@55", "charset_attribute_name",
  "predefined_charset_attribute_name",
  "predefined_charset_attribute_value", "name", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-272)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-235)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -272,   264,  -272,  -272,    27,    28,    29,    31,    32,    33,
      35,    36,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,
    -272,   -40,  -272,  -272,   -42,   -36,   -33,  -272,   -42,   -32,
     -29,  -272,   -42,   -28,  -272,  -272,   -42,   -24,  -272,  -272,
     -42,   -11,    -2,  -272,   -42,     2,     4,  -272,   -42,     6,
    -272,  -272,   -42,    38,     5,  -272,    54,    66,  -272,   150,
      67,    69,  -272,   238,    70,    74,  -272,    43,    77,    83,
    -272,    44,    93,    97,  -272,    99,   101,   105,  -272,  -272,
     107,   114,  -272,    47,   117,   118,  -272,   155,  -272,  -272,
    -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,
    -272,  -272,  -272,  -272,    39,  -272,  -272,  -272,  -272,     7,
    -272,  -272,  -272,  -272,  -272,  -272,  -272,    48,  -272,  -272,
    -272,   170,  -272,  -272,  -272,  -272,   124,   142,   153,  -272,
    -272,  -272,  -272,  -272,   113,  -272,     8,   157,  -272,  -272,
       9,  -272,  -272,   143,  -272,    53,  -272,   158,  -272,  -272,
    -272,  -272,  -272,  -272,  -272,  -272,   189,  -272,   162,   164,
    -272,    89,  -272,  -272,   144,  -272,  -272,   172,  -272,  -272,
     173,  -272,   174,  -272,  -272,   152,  -272,    11,  -272,   175,
    -272,  -272,  -272,    82,  -272,   178,  -272,   184,  -272,  -272,
     183,   204,   186,  -272,  -272,  -272,  -272,  -272,   181,  -272,
     188,  -272,  -272,  -272,   194,  -272,  -272,   116,  -272,   190,
     192,   195,   197,   208,   214,  -272,  -272,  -272,  -272,  -272,
    -272,  -272,  -272,  -272,  -272,  -272,   163,  -272,   205,   229,
    -272,  -272,  -272,  -272,   228,   211,   230,  -272,  -272,    13,
    -272,  -272,  -272,   232,  -272,  -272,   231,  -272,  -272,  -272,
    -272,  -272,  -272,  -272,  -272,  -272,   234,  -272,    15,  -272,
    -272,    17,  -272,  -272,    19,  -272,  -272,  -272,   233,     0,
    -272,  -272,  -272,  -272,   236,  -272,  -272,   235,     0,   240,
     241,  -272,  -272,  -272,   243,  -272,  -272,   245,  -272,  -272,
     246,   247,  -272,   248,  -272,  -272,    21,  -272,  -272,   249,
    -272,  -272,    23,  -272,  -272,   250,  -272,  -272,    25,  -272,
    -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,
    -272,  -272,   255,  -272,  -272,  -272,   256,  -272,  -272,   203,
    -272,   257,  -272,  -272,   212,  -272,   258,  -272,  -272,   259,
     260,   261,   262,   263,  -272,  -272,  -272,   206,   265,  -272,
    -272,   266,   267,  -272,  -272,  -272,  -272,  -272,  -272,   268,
    -272,  -272,  -272,  -272,  -272,   210,  -272,   237,   269,  -272,
     270,  -272,  -272,  -272,   272,  -272,  -272,   271,     0,  -272,
    -272,  -272,  -272,   274,   276,  -272,  -272
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,    12,     0,     0,     0,     0,     0,     0,
       0,     0,     3,     4,     5,     6,     7,     8,     9,    10,
      11,     0,    17,   258,    14,     0,     0,    22,    19,     0,
       0,    32,    29,     0,   171,    37,    34,     0,   187,    42,
      39,     0,     0,    47,    44,     0,     0,    52,    49,     0,
     232,    27,    24,     0,     0,    13,     0,     0,    15,     0,
       0,     0,    20,     0,     0,     0,    30,     0,     0,     0,
      35,     0,     0,     0,    40,     0,     0,     0,    45,   218,
       0,     0,    50,     0,     0,     0,    25,     0,    18,    16,
      56,    57,    58,    54,    23,    21,   144,   145,   146,   147,
     142,    33,    31,   176,     0,   172,    38,    36,   192,     0,
     188,    43,    41,   206,   204,    48,    46,     0,    53,    51,
     237,     0,   233,    28,    26,   170,     0,     0,     0,   182,
     185,   184,   186,   174,     0,   178,     0,     0,   198,   190,
       0,   194,   196,     0,   223,     0,   219,     0,   245,   248,
     247,   249,   250,   251,   252,   235,     0,   239,     0,     0,
      62,     0,    55,   151,     0,   143,   177,     0,   175,   179,
       0,   193,     0,   191,   195,     0,   210,     0,   205,     0,
     229,   231,   221,     0,   225,     0,   238,     0,   236,   240,
       0,     0,     0,    73,    78,    79,    82,    77,     0,    75,
       0,    80,    83,    81,     0,    76,    60,     0,    64,     0,
       0,     0,     0,     0,     0,   159,   163,   166,   168,   161,
     162,   165,   164,   169,   167,   149,     0,   153,     0,     0,
     183,   180,   199,   202,     0,     0,     0,   216,   208,     0,
     212,   214,   224,     0,   222,   226,     0,   246,   241,   253,
     254,   255,   256,   257,   243,    63,     0,    90,     0,    86,
     118,     0,   114,   134,     0,   130,    61,    65,     0,     0,
      70,    71,    72,   152,     0,   150,   154,     0,     0,     0,
       0,   201,   197,   211,     0,   209,   213,     0,   230,   227,
       0,     0,    74,     0,    96,    88,     0,    92,    94,     0,
     124,   116,     0,   120,   122,     0,   140,   132,     0,   136,
     138,    66,    84,    85,    68,   160,   155,   157,   181,   203,
     217,   215,     0,   242,   244,    91,     0,    89,    93,     0,
     119,     0,   117,   121,     0,   135,     0,   133,   137,     0,
       0,     0,     0,     0,   228,    97,   101,     0,     0,   125,
     128,     0,     0,   141,   139,    67,    69,   156,   158,     0,
     109,   111,   113,   112,    99,     0,   103,     0,     0,    95,
       0,   127,   123,   102,     0,   100,   104,     0,     0,   129,
     110,   105,   107,     0,     0,   106,   108
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,
    -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,
    -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,   275,  -272,
    -272,  -272,  -272,  -272,    91,  -272,  -272,  -272,  -272,  -272,
    -271,  -272,  -272,  -272,  -272,    -6,  -272,  -272,  -272,  -272,
    -272,   -52,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,
    -272,    24,  -272,  -272,  -272,  -272,  -272,  -272,  -272,  -272,
      20,  -272,  -272,   287,  -272,  -272,  -272,  -272,  -272,   103,
    -272,  -272,  -272,  -272,  -272,   278,   285,  -272,  -272,  -272,
    -272,   193,  -272,  -272,  -272,   295,  -272,  -272,  -272,  -272,
     196,  -272,  -272,  -272,  -272,   292,  -272,  -272,  -272,  -272,
    -272,    94,  -272,  -272,   290,  -272,  -272,  -272,  -272,   154,
    -272,  -272,  -272,   291,  -272,  -272,  -272,  -272,   179,  -272,
    -272,  -272,  -272,  -272,  -272,    -5
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,    12,    21,    13,    24,    56,    14,    28,    60,
      15,    52,    84,    16,    32,    64,    17,    36,    68,    18,
      40,    72,    19,    44,    76,    20,    48,    80,    25,   126,
      93,   162,   192,   207,   208,   340,   341,   256,   209,   210,
     314,   211,   259,   293,   296,   297,   329,   326,   348,   359,
     365,   366,   383,   384,   374,   367,   368,   212,   262,   299,
     302,   303,   334,   331,   352,   370,   213,   265,   305,   308,
     309,   339,   336,    29,   127,   100,   165,   214,   226,   227,
     342,   343,   274,   228,   229,    53,    33,    67,   105,   128,
     134,   135,   279,   167,   136,    37,    71,   110,   137,   140,
     141,   175,   172,   235,   280,    41,   143,   114,   178,   236,
     239,   240,   287,   284,    45,   117,   146,   179,   183,   184,
     322,   243,   185,    49,    83,   122,   147,   156,   157,   290,
     291,   187,   158,   159,   254,    26
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      30,    34,    38,    42,    46,    50,    54,   317,   138,    55,
     138,    23,   237,    58,   237,    59,   294,    62,   300,    63,
     306,    66,   294,    30,   300,    70,   306,    34,    22,    27,
      31,    38,    35,    39,    43,    42,    47,    51,    74,    46,
     129,   312,   313,    50,   103,   108,    75,    54,   120,   144,
     130,    78,    79,    87,   180,    82,   131,   132,   170,   139,
      23,   173,    23,   238,    23,   285,    23,   295,    23,   301,
      23,   307,    23,   327,    23,   332,    23,   337,    23,   181,
      23,    23,    23,   180,    23,    23,    23,    86,    23,    23,
     193,   133,  -173,  -189,   104,   109,  -234,  -220,   121,   145,
     194,   195,   196,    88,   142,   182,   197,   382,   181,   198,
     199,   200,   201,   202,   129,    89,    94,   193,    95,   101,
     203,   204,   205,   102,   130,   160,   106,   194,   195,   196,
     131,   132,   107,   197,   244,   142,   198,   199,   200,   201,
     202,   206,   111,   163,   176,   215,   112,   203,   204,   205,
     115,   216,   113,   233,   116,   217,   118,   218,   219,   220,
     221,   222,   223,   119,   215,   168,   123,   124,   266,   224,
     216,   148,   241,   -59,   217,   161,   218,   219,   220,   221,
     222,   223,   257,   149,    90,    91,    92,   150,   224,   260,
     148,  -148,  -207,   164,   177,   263,   225,   151,   152,   153,
     154,  -200,   149,   234,   346,   166,   150,   360,   125,   171,
     186,   360,   190,   350,   191,   275,   151,   152,   153,   154,
     361,   230,   155,   232,   361,   362,   231,   242,   246,   362,
     -87,   363,   258,   247,   241,   363,   248,  -115,   255,   261,
     268,   188,   269,  -131,   270,   264,   271,   249,   250,   251,
     252,   253,   -98,   298,   347,   277,   304,   272,   364,   310,
     282,  -126,   375,   351,     2,     3,   273,     4,     5,     6,
       7,     8,     9,    10,    11,    96,    97,    98,    99,   278,
     281,   288,   283,   292,   289,   315,   311,   377,   316,   318,
     328,   298,   320,   319,   321,   323,   324,   304,   267,    57,
     325,   330,   335,   310,   344,   345,   349,   353,   354,   355,
     356,   357,   358,   376,   369,    61,   372,    65,   371,   378,
     373,   380,   379,   385,   381,   386,   333,   169,   338,   276,
      85,    69,    73,   286,    77,   189,   174,   245,     0,    81
};

static const yytype_int16 yycheck[] =
{
       5,     6,     7,     8,     9,    10,    11,   278,     1,    49,
       1,    53,     1,    49,     1,    48,     1,    49,     1,    48,
       1,    49,     1,    28,     1,    49,     1,    32,     1,     1,
       1,    36,     1,     1,     1,    40,     1,     1,    49,    44,
       1,    41,    42,    48,     1,     1,    48,    52,     1,     1,
      11,    49,    48,    48,     1,    49,    17,    18,    50,    52,
      53,    52,    53,    52,    53,    52,    53,    52,    53,    52,
      53,    52,    53,    52,    53,    52,    53,    52,    53,    26,
      53,    53,    53,     1,    53,    53,    53,    49,    53,    53,
       1,    52,    49,    49,    51,    51,    49,    49,    51,    51,
      11,    12,    13,    49,   109,    52,    17,   378,    26,    20,
      21,    22,    23,    24,     1,    49,    49,     1,    49,    49,
      31,    32,    33,    49,    11,     1,    49,    11,    12,    13,
      17,    18,    49,    17,    52,   140,    20,    21,    22,    23,
      24,    52,    49,     1,     1,     1,    49,    31,    32,    33,
      49,     7,    53,     1,    49,    11,    49,    13,    14,    15,
      16,    17,    18,    49,     1,    52,    49,    49,    52,    25,
       7,     1,   177,    49,    11,    51,    13,    14,    15,    16,
      17,    18,     1,    13,    34,    35,    36,    17,    25,     1,
       1,    49,    49,    51,    51,     1,    52,    27,    28,    29,
      30,    49,    13,    51,     1,    52,    17,     1,    53,    52,
      52,     1,    50,     1,    50,    52,    27,    28,    29,    30,
      14,    49,    52,    49,    14,    19,    53,    52,    50,    19,
      49,    25,    51,    49,   239,    25,    53,    49,    52,    51,
      50,    52,    50,    49,    49,    51,    49,    43,    44,    45,
      46,    47,    49,   258,    51,    50,   261,    49,    52,   264,
      49,    49,    52,    51,     0,     1,    52,     3,     4,     5,
       6,     7,     8,     9,    10,    37,    38,    39,    40,    50,
      52,    49,    52,    49,    53,    49,    53,    50,    53,    49,
     296,   296,    49,    52,    49,    49,    49,   302,   207,    24,
      52,    52,    52,   308,    49,    49,    49,    49,    49,    49,
      49,    49,    49,   365,    49,    28,    49,    32,    52,    50,
      52,    49,    52,    49,    53,    49,   302,   134,   308,   226,
      52,    36,    40,   239,    44,   156,   140,   183,    -1,    48
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    56,     0,     1,     3,     4,     5,     6,     7,     8,
       9,    10,    57,    59,    62,    65,    68,    71,    74,    77,
      80,    58,     1,    53,    60,    83,   190,     1,    63,   128,
     190,     1,    69,   141,   190,     1,    72,   150,   190,     1,
      75,   160,   190,     1,    78,   169,   190,     1,    81,   178,
     190,     1,    66,   140,   190,    49,    61,    83,    49,    48,
      64,   128,    49,    48,    70,   141,    49,   142,    73,   150,
      49,   151,    76,   160,    49,    48,    79,   169,    49,    48,
      82,   178,    49,   179,    67,   140,    49,    48,    49,    49,
      34,    35,    36,    85,    49,    49,    37,    38,    39,    40,
     130,    49,    49,     1,    51,   143,    49,    49,     1,    51,
     152,    49,    49,    53,   162,    49,    49,   170,    49,    49,
       1,    51,   180,    49,    49,    53,    84,   129,   144,     1,
      11,    17,    18,    52,   145,   146,   149,   153,     1,    52,
     154,   155,   190,   161,     1,    51,   171,   181,     1,    13,
      17,    27,    28,    29,    30,    52,   182,   183,   187,   188,
       1,    51,    86,     1,    51,   131,    52,   148,    52,   146,
      50,    52,   157,    52,   155,   156,     1,    51,   163,   172,
       1,    26,    52,   173,   174,   177,    52,   186,    52,   183,
      50,    50,    87,     1,    11,    12,    13,    17,    20,    21,
      22,    23,    24,    31,    32,    33,    52,    88,    89,    93,
      94,    96,   112,   121,   132,     1,     7,    11,    13,    14,
      15,    16,    17,    18,    25,    52,   133,   134,   138,   139,
      49,    53,    49,     1,    51,   158,   164,     1,    52,   165,
     166,   190,    52,   176,    52,   174,    50,    49,    53,    43,
      44,    45,    46,    47,   189,    52,    92,     1,    51,    97,
       1,    51,   113,     1,    51,   122,    52,    89,    50,    50,
      49,    49,    49,    52,   137,    52,   134,    50,    50,   147,
     159,    52,    49,    52,   168,    52,   166,   167,    49,    53,
     184,   185,    49,    98,     1,    52,    99,   100,   190,   114,
       1,    52,   115,   116,   190,   123,     1,    52,   124,   125,
     190,    53,    41,    42,    95,    49,    53,    95,    49,    52,
      49,    49,   175,    49,    49,    52,   102,    52,   100,   101,
      52,   118,    52,   116,   117,    52,   127,    52,   125,   126,
      90,    91,   135,   136,    49,    49,     1,    51,   103,    49,
       1,    51,   119,    49,    49,    49,    49,    49,    49,   104,
       1,    14,    19,    25,    52,   105,   106,   110,   111,    49,
     120,    52,    49,    52,   109,    52,   106,    50,    50,    52,
      49,    53,    95,   107,   108,    49,    49
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    56,    57,    57,    57,    57,    57,    57,
      57,    57,    58,    57,    59,    60,    60,    61,    60,    62,
      63,    63,    64,    63,    65,    66,    66,    67,    66,    68,
      69,    69,    70,    69,    71,    72,    72,    73,    72,    74,
      75,    75,    76,    75,    77,    78,    78,    79,    78,    80,
      81,    81,    82,    81,    84,    83,    85,    85,    85,    86,
      86,    86,    87,    86,    88,    88,    90,    89,    91,    89,
      89,    89,    89,    92,    89,    93,    93,    93,    93,    93,
      93,    93,    93,    94,    95,    95,    96,    97,    97,    97,
      98,    97,    99,    99,   101,   100,   102,   100,   103,   103,
     103,   104,   103,   105,   105,   107,   106,   108,   106,   109,
     106,   110,   110,   111,   112,   113,   113,   113,   114,   113,
     115,   115,   117,   116,   118,   116,   119,   119,   120,   119,
     121,   122,   122,   122,   123,   122,   124,   124,   126,   125,
     127,   125,   129,   128,   130,   130,   130,   130,   131,   131,
     131,   132,   131,   133,   133,   135,   134,   136,   134,   137,
     134,   138,   138,   138,   138,   138,   138,   138,   138,   139,
     140,   142,   141,   143,   143,   143,   144,   143,   145,   145,
     147,   146,   148,   146,   149,   149,   149,   151,   150,   152,
     152,   152,   153,   152,   154,   154,   156,   155,   157,   155,
     158,   158,   159,   158,   161,   160,   162,   163,   163,   163,
     164,   163,   165,   165,   167,   166,   168,   166,   170,   169,
     171,   171,   171,   172,   171,   173,   173,   175,   174,   176,
     174,   177,   179,   178,   180,   180,   180,   181,   180,   182,
     182,   184,   183,   185,   183,   186,   183,   187,   187,   187,
     188,   188,   188,   189,   189,   189,   189,   189,   190
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     2,     2,     3,     0,     3,     2,
       2,     3,     0,     3,     2,     2,     3,     0,     3,     2,
       2,     3,     0,     3,     2,     2,     3,     0,     3,     2,
       2,     3,     0,     3,     2,     2,     3,     0,     3,     2,
       2,     3,     0,     3,     0,     5,     1,     1,     1,     0,
       2,     3,     0,     3,     1,     2,     0,     5,     0,     5,
       2,     2,     2,     0,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     0,     2,     3,
       0,     3,     1,     2,     0,     4,     0,     3,     0,     2,
       3,     0,     3,     1,     2,     0,     5,     0,     5,     0,
       3,     1,     1,     1,     2,     0,     2,     3,     0,     3,
       1,     2,     0,     4,     0,     3,     0,     2,     0,     3,
       2,     0,     2,     3,     0,     3,     1,     2,     0,     3,
       0,     3,     0,     5,     1,     1,     1,     1,     0,     2,
       3,     0,     3,     1,     2,     0,     5,     0,     5,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     0,     3,     0,     2,     3,     0,     3,     1,     2,
       0,     5,     0,     3,     1,     1,     1,     0,     3,     0,
       2,     3,     0,     3,     1,     2,     0,     4,     0,     3,
       0,     2,     0,     3,     0,     5,     1,     0,     2,     3,
       0,     3,     1,     2,     0,     3,     0,     3,     0,     4,
       0,     2,     3,     0,     3,     1,     2,     0,     5,     0,
       3,     1,     0,     3,     0,     2,     3,     0,     3,     1,
       2,     0,     5,     0,     5,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
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

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
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
  case 12: /* $@1: %empty  */
#line 174 "wmlparse.y"
                { LexIssueError(0); }
#line 1736 "wmlparse.c"
    break;

  case 17: /* $@2: %empty  */
#line 190 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1742 "wmlparse.c"
    break;

  case 22: /* $@3: %empty  */
#line 201 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1748 "wmlparse.c"
    break;

  case 27: /* $@4: %empty  */
#line 212 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1754 "wmlparse.c"
    break;

  case 32: /* $@5: %empty  */
#line 223 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1760 "wmlparse.c"
    break;

  case 37: /* $@6: %empty  */
#line 234 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1766 "wmlparse.c"
    break;

  case 42: /* $@7: %empty  */
#line 245 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1772 "wmlparse.c"
    break;

  case 47: /* $@8: %empty  */
#line 256 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1778 "wmlparse.c"
    break;

  case 52: /* $@9: %empty  */
#line 267 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1784 "wmlparse.c"
    break;

  case 54: /* $@10: %empty  */
#line 279 "wmlparse.y"
                { wmlCreateClass (yynameval, yytknval1); }
#line 1790 "wmlparse.c"
    break;

  case 56: /* class_type: METACLASS  */
#line 284 "wmlparse.y"
                                { yytknval1 = METACLASS; }
#line 1796 "wmlparse.c"
    break;

  case 57: /* class_type: WIDGET  */
#line 285 "wmlparse.y"
                                { yytknval1 = WIDGET; }
#line 1802 "wmlparse.c"
    break;

  case 58: /* class_type: GADGET  */
#line 286 "wmlparse.y"
                                { yytknval1 = GADGET; }
#line 1808 "wmlparse.c"
    break;

  case 62: /* $@11: %empty  */
#line 293 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 1814 "wmlparse.c"
    break;

  case 66: /* $@12: %empty  */
#line 305 "wmlparse.y"
                { wmlAddClassAttribute (yytknval1, yystringval); }
#line 1820 "wmlparse.c"
    break;

  case 68: /* $@13: %empty  */
#line 310 "wmlparse.y"
                { wmlAddClassAttribute (yytknval1, yytknval2); }
#line 1826 "wmlparse.c"
    break;

  case 73: /* $@14: %empty  */
#line 315 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1832 "wmlparse.c"
    break;

  case 75: /* class_attribute_name: SUPERCLASS  */
#line 319 "wmlparse.y"
                                { yytknval1 = SUPERCLASS; }
#line 1838 "wmlparse.c"
    break;

  case 76: /* class_attribute_name: PARENTCLASS  */
#line 320 "wmlparse.y"
                                { yytknval1 = PARENTCLASS; }
#line 1844 "wmlparse.c"
    break;

  case 77: /* class_attribute_name: INTERNALLITERAL  */
#line 321 "wmlparse.y"
                                { yytknval1 = INTERNALLITERAL; }
#line 1850 "wmlparse.c"
    break;

  case 78: /* class_attribute_name: DOCNAME  */
#line 322 "wmlparse.y"
                                { yytknval1 = DOCNAME; }
#line 1856 "wmlparse.c"
    break;

  case 79: /* class_attribute_name: CONVFUNC  */
#line 323 "wmlparse.y"
                                { yytknval1 = CONVFUNC; }
#line 1862 "wmlparse.c"
    break;

  case 80: /* class_attribute_name: WIDGETCLASS  */
#line 324 "wmlparse.y"
                                { yytknval1 = WIDGETCLASS; }
#line 1868 "wmlparse.c"
    break;

  case 81: /* class_attribute_name: CTRLMAPSRESOURCE  */
#line 325 "wmlparse.y"
                                { yytknval1 = CTRLMAPSRESOURCE; }
#line 1874 "wmlparse.c"
    break;

  case 82: /* class_attribute_name: ALIAS  */
#line 326 "wmlparse.y"
                                { yytknval1 = ALIAS; }
#line 1880 "wmlparse.c"
    break;

  case 83: /* boolean_class_attribute_name: DIALOGCLASS  */
#line 330 "wmlparse.y"
                                { yytknval1 = DIALOGCLASS; }
#line 1886 "wmlparse.c"
    break;

  case 84: /* boolean_attribute_value: ATTRTRUE  */
#line 334 "wmlparse.y"
                                { yytknval2 = ATTRTRUE; }
#line 1892 "wmlparse.c"
    break;

  case 85: /* boolean_attribute_value: ATTRFALSE  */
#line 335 "wmlparse.y"
                                { yytknval2 = ATTRFALSE; }
#line 1898 "wmlparse.c"
    break;

  case 90: /* $@15: %empty  */
#line 346 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 1904 "wmlparse.c"
    break;

  case 94: /* $@16: %empty  */
#line 356 "wmlparse.y"
                { wmlAddClassResource (yynameval); }
#line 1910 "wmlparse.c"
    break;

  case 96: /* $@17: %empty  */
#line 359 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1916 "wmlparse.c"
    break;

  case 101: /* $@18: %empty  */
#line 366 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 1922 "wmlparse.c"
    break;

  case 105: /* $@19: %empty  */
#line 378 "wmlparse.y"
                { wmlAddClassResourceAttribute (yytknval1, yystringval); }
#line 1928 "wmlparse.c"
    break;

  case 107: /* $@20: %empty  */
#line 383 "wmlparse.y"
                { wmlAddClassResourceAttribute (yytknval1, yytknval2); }
#line 1934 "wmlparse.c"
    break;

  case 109: /* $@21: %empty  */
#line 385 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1940 "wmlparse.c"
    break;

  case 111: /* class_resource_attribute_name: TYPE  */
#line 389 "wmlparse.y"
                                { yytknval1 = TYPE; }
#line 1946 "wmlparse.c"
    break;

  case 112: /* class_resource_attribute_name: DEFAULT  */
#line 390 "wmlparse.y"
                                { yytknval1 = DEFAULT; }
#line 1952 "wmlparse.c"
    break;

  case 113: /* boolean_class_resource_attribute_name: EXCLUDE  */
#line 394 "wmlparse.y"
                                { yytknval1 = EXCLUDE; }
#line 1958 "wmlparse.c"
    break;

  case 118: /* $@22: %empty  */
#line 405 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 1964 "wmlparse.c"
    break;

  case 122: /* $@23: %empty  */
#line 415 "wmlparse.y"
                { wmlAddClassControl (yynameval); }
#line 1970 "wmlparse.c"
    break;

  case 124: /* $@24: %empty  */
#line 418 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 1976 "wmlparse.c"
    break;

  case 128: /* $@25: %empty  */
#line 424 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 1982 "wmlparse.c"
    break;

  case 134: /* $@26: %empty  */
#line 435 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 1988 "wmlparse.c"
    break;

  case 138: /* $@27: %empty  */
#line 445 "wmlparse.y"
                { wmlAddClassChild (yynameval); }
#line 1994 "wmlparse.c"
    break;

  case 140: /* $@28: %empty  */
#line 447 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 2000 "wmlparse.c"
    break;

  case 142: /* $@29: %empty  */
#line 459 "wmlparse.y"
                { wmlCreateResource (yynameval, yytknval1); }
#line 2006 "wmlparse.c"
    break;

  case 144: /* resource_type: ARGUMENT  */
#line 464 "wmlparse.y"
                                { yytknval1 = ARGUMENT; }
#line 2012 "wmlparse.c"
    break;

  case 145: /* resource_type: REASON  */
#line 465 "wmlparse.y"
                                { yytknval1 = REASON; }
#line 2018 "wmlparse.c"
    break;

  case 146: /* resource_type: CONSTRAINT  */
#line 466 "wmlparse.y"
                                { yytknval1 = CONSTRAINT; }
#line 2024 "wmlparse.c"
    break;

  case 147: /* resource_type: SUBRESOURCE  */
#line 467 "wmlparse.y"
                                { yytknval1 = SUBRESOURCE; }
#line 2030 "wmlparse.c"
    break;

  case 151: /* $@30: %empty  */
#line 474 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 2036 "wmlparse.c"
    break;

  case 155: /* $@31: %empty  */
#line 486 "wmlparse.y"
                { wmlAddResourceAttribute (yytknval1, yystringval); }
#line 2042 "wmlparse.c"
    break;

  case 157: /* $@32: %empty  */
#line 491 "wmlparse.y"
                { wmlAddResourceAttribute (yytknval1, yytknval2); }
#line 2048 "wmlparse.c"
    break;

  case 159: /* $@33: %empty  */
#line 493 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 2054 "wmlparse.c"
    break;

  case 161: /* resource_attribute_name: TYPE  */
#line 497 "wmlparse.y"
                                { yytknval1 = TYPE; }
#line 2060 "wmlparse.c"
    break;

  case 162: /* resource_attribute_name: RESOURCELITERAL  */
#line 498 "wmlparse.y"
                                { yytknval1 = RESOURCELITERAL; }
#line 2066 "wmlparse.c"
    break;

  case 163: /* resource_attribute_name: ENUMERATIONSET  */
#line 499 "wmlparse.y"
                                { yytknval1 = ENUMERATIONSET; }
#line 2072 "wmlparse.c"
    break;

  case 164: /* resource_attribute_name: INTERNALLITERAL  */
#line 500 "wmlparse.y"
                                { yytknval1 = INTERNALLITERAL; }
#line 2078 "wmlparse.c"
    break;

  case 165: /* resource_attribute_name: RELATED  */
#line 501 "wmlparse.y"
                                { yytknval1 = RELATED; }
#line 2084 "wmlparse.c"
    break;

  case 166: /* resource_attribute_name: DOCNAME  */
#line 502 "wmlparse.y"
                                { yytknval1 = DOCNAME; }
#line 2090 "wmlparse.c"
    break;

  case 167: /* resource_attribute_name: DEFAULT  */
#line 503 "wmlparse.y"
                                { yytknval1 = DEFAULT; }
#line 2096 "wmlparse.c"
    break;

  case 168: /* resource_attribute_name: ALIAS  */
#line 504 "wmlparse.y"
                                { yytknval1 = ALIAS; }
#line 2102 "wmlparse.c"
    break;

  case 169: /* boolean_resource_resource_attribute_name: XRMRESOURCE  */
#line 508 "wmlparse.y"
                                { yytknval1 = XRMRESOURCE; }
#line 2108 "wmlparse.c"
    break;

  case 170: /* child_statement: name COLON STRING  */
#line 517 "wmlparse.y"
          { wmlCreateChild(yynameval, yystringval); }
#line 2114 "wmlparse.c"
    break;

  case 171: /* $@34: %empty  */
#line 526 "wmlparse.y"
                { wmlCreateDatatype (yystringval); }
#line 2120 "wmlparse.c"
    break;

  case 176: /* $@35: %empty  */
#line 534 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 2126 "wmlparse.c"
    break;

  case 180: /* $@36: %empty  */
#line 546 "wmlparse.y"
                { wmlAddDatatypeAttribute (yytknval1, yystringval); }
#line 2132 "wmlparse.c"
    break;

  case 182: /* $@37: %empty  */
#line 548 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 2138 "wmlparse.c"
    break;

  case 184: /* datatype_attribute_name: INTERNALLITERAL  */
#line 552 "wmlparse.y"
                                { yytknval1 = INTERNALLITERAL; }
#line 2144 "wmlparse.c"
    break;

  case 185: /* datatype_attribute_name: DOCNAME  */
#line 553 "wmlparse.y"
                                { yytknval1 = DOCNAME; }
#line 2150 "wmlparse.c"
    break;

  case 186: /* datatype_attribute_name: XRMRESOURCE  */
#line 554 "wmlparse.y"
                                { yytknval1 = XRMRESOURCE; }
#line 2156 "wmlparse.c"
    break;

  case 187: /* $@38: %empty  */
#line 564 "wmlparse.y"
                { wmlCreateOrAppendCtrlList (yynameval); }
#line 2162 "wmlparse.c"
    break;

  case 192: /* $@39: %empty  */
#line 572 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 2168 "wmlparse.c"
    break;

  case 196: /* $@40: %empty  */
#line 582 "wmlparse.y"
                { wmlAddCtrlListControl (yynameval); }
#line 2174 "wmlparse.c"
    break;

  case 198: /* $@41: %empty  */
#line 585 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 2180 "wmlparse.c"
    break;

  case 202: /* $@42: %empty  */
#line 591 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 2186 "wmlparse.c"
    break;

  case 204: /* $@43: %empty  */
#line 603 "wmlparse.y"
                { wmlCreateEnumSet (yynameval, yystringval); }
#line 2192 "wmlparse.c"
    break;

  case 210: /* $@44: %empty  */
#line 616 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 2198 "wmlparse.c"
    break;

  case 214: /* $@45: %empty  */
#line 626 "wmlparse.y"
            { wmlAddEnumSetValue (yynameval); }
#line 2204 "wmlparse.c"
    break;

  case 216: /* $@46: %empty  */
#line 628 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 2210 "wmlparse.c"
    break;

  case 218: /* $@47: %empty  */
#line 639 "wmlparse.y"
                { wmlCreateEnumValue (yynameval); }
#line 2216 "wmlparse.c"
    break;

  case 223: /* $@48: %empty  */
#line 647 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 2222 "wmlparse.c"
    break;

  case 227: /* $@49: %empty  */
#line 659 "wmlparse.y"
                { wmlAddEnumValueAttribute (yytknval1, yystringval); }
#line 2228 "wmlparse.c"
    break;

  case 229: /* $@50: %empty  */
#line 661 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 2234 "wmlparse.c"
    break;

  case 231: /* enumvalue_attribute_name: ENUMLITERAL  */
#line 665 "wmlparse.y"
                                { yytknval1 = ENUMLITERAL; }
#line 2240 "wmlparse.c"
    break;

  case 232: /* $@51: %empty  */
#line 675 "wmlparse.y"
                { wmlCreateCharset (yystringval); }
#line 2246 "wmlparse.c"
    break;

  case 237: /* $@52: %empty  */
#line 683 "wmlparse.y"
                { LexIssueError(RBRACE); }
#line 2252 "wmlparse.c"
    break;

  case 241: /* $@53: %empty  */
#line 695 "wmlparse.y"
                { wmlAddCharsetAttribute (yytknval1, yystringval); }
#line 2258 "wmlparse.c"
    break;

  case 243: /* $@54: %empty  */
#line 700 "wmlparse.y"
                { wmlAddCharsetAttribute (yytknval1, yytknval2); }
#line 2264 "wmlparse.c"
    break;

  case 245: /* $@55: %empty  */
#line 702 "wmlparse.y"
                { LexIssueError(SEMICOLON); }
#line 2270 "wmlparse.c"
    break;

  case 247: /* charset_attribute_name: INTERNALLITERAL  */
#line 706 "wmlparse.y"
                                { yytknval1 = INTERNALLITERAL; }
#line 2276 "wmlparse.c"
    break;

  case 248: /* charset_attribute_name: ALIAS  */
#line 707 "wmlparse.y"
                                { yytknval1 = ALIAS; }
#line 2282 "wmlparse.c"
    break;

  case 249: /* charset_attribute_name: XMSTRINGCHARSETNAME  */
#line 708 "wmlparse.y"
                                { yytknval1 = XMSTRINGCHARSETNAME; }
#line 2288 "wmlparse.c"
    break;

  case 250: /* predefined_charset_attribute_name: DIRECTION  */
#line 712 "wmlparse.y"
                                { yytknval1 = DIRECTION; }
#line 2294 "wmlparse.c"
    break;

  case 251: /* predefined_charset_attribute_name: PARSEDIRECTION  */
#line 713 "wmlparse.y"
                                { yytknval1 = PARSEDIRECTION; }
#line 2300 "wmlparse.c"
    break;

  case 252: /* predefined_charset_attribute_name: CHARACTERSIZE  */
#line 714 "wmlparse.y"
                                { yytknval1 = CHARACTERSIZE; }
#line 2306 "wmlparse.c"
    break;

  case 253: /* predefined_charset_attribute_value: LEFTTORIGHT  */
#line 718 "wmlparse.y"
                                { yytknval2 = LEFTTORIGHT; }
#line 2312 "wmlparse.c"
    break;

  case 254: /* predefined_charset_attribute_value: RIGHTTOLEFT  */
#line 719 "wmlparse.y"
                                { yytknval2 = RIGHTTOLEFT; }
#line 2318 "wmlparse.c"
    break;

  case 255: /* predefined_charset_attribute_value: ONEBYTE  */
#line 720 "wmlparse.y"
                                { yytknval2 = ONEBYTE; }
#line 2324 "wmlparse.c"
    break;

  case 256: /* predefined_charset_attribute_value: TWOBYTE  */
#line 721 "wmlparse.y"
                                { yytknval2 = TWOBYTE; }
#line 2330 "wmlparse.c"
    break;

  case 257: /* predefined_charset_attribute_value: MIXED1_2BYTE  */
#line 722 "wmlparse.y"
                                { yytknval2 = MIXED1_2BYTE; }
#line 2336 "wmlparse.c"
    break;

  case 258: /* name: STRING  */
#line 731 "wmlparse.y"
                { strcpy (yynameval, (XmConst char *)yystringval); }
#line 2342 "wmlparse.c"
    break;


#line 2346 "wmlparse.c"

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

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
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

#line 734 "wmlparse.y"


/*
 * Dummmy error routine, since we output our own.
 */
int yyerror (s)
char *s;
{
	return 0;
}


#include "wmllex.c"

