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
#line 27 "Automation.y"


#include <stdio.h>
#include "mvslib.h"
#include "Automation.h"

char *temp;
char temp1[100];

int line_cnt = 1;


#line 84 "y.tab.c"

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
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
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
    STRING = 258,                  /* STRING  */
    DIGIT = 259,                   /* DIGIT  */
    ISTRING = 260,                 /* ISTRING  */
    CONTINUE = 261,                /* CONTINUE  */
    END = 262,                     /* END  */
    MANUAL = 263,                  /* MANUAL  */
    WAIT = 264,                    /* WAIT  */
    PRESSMB = 265,                 /* PRESSMB  */
    RELEASEMB = 266,               /* RELEASEMB  */
    CLICKMB = 267,                 /* CLICKMB  */
    SHIFTMASK = 268,               /* SHIFTMASK  */
    LOCKMASK = 269,                /* LOCKMASK  */
    CTRLMASK = 270,                /* CTRLMASK  */
    METAMASK = 271,                /* METAMASK  */
    ALTMASK = 272,                 /* ALTMASK  */
    MOD1MASK = 273,                /* MOD1MASK  */
    MOD2MASK = 274,                /* MOD2MASK  */
    MOD3MASK = 275,                /* MOD3MASK  */
    MOD4MASK = 276,                /* MOD4MASK  */
    MOD5MASK = 277,                /* MOD5MASK  */
    BTN1 = 278,                    /* BTN1  */
    BTN2 = 279,                    /* BTN2  */
    BTN3 = 280,                    /* BTN3  */
    BTN4 = 281,                    /* BTN4  */
    BTN5 = 282,                    /* BTN5  */
    CLICKKEY = 283,                /* CLICKKEY  */
    PRESSKEY = 284,                /* PRESSKEY  */
    RELEASEKEY = 285,              /* RELEASEKEY  */
    BACKSPACE = 286,               /* BACKSPACE  */
    INSERT = 287,                  /* INSERT  */
    DELETE = 288,                  /* DELETE  */
    COPY = 289,                    /* COPY  */
    CUT = 290,                     /* CUT  */
    PASTE = 291,                   /* PASTE  */
    ADDMODE = 292,                 /* ADDMODE  */
    PRIMARYPASTE = 293,            /* PRIMARYPASTE  */
    QUICKPASTE = 294,              /* QUICKPASTE  */
    PAGEUP = 295,                  /* PAGEUP  */
    PAGEDOWN = 296,                /* PAGEDOWN  */
    ENDLINE = 297,                 /* ENDLINE  */
    BEGINLINE = 298,               /* BEGINLINE  */
    ACTIVATE = 299,                /* ACTIVATE  */
    MENUBAR = 300,                 /* MENUBAR  */
    CLEAR = 301,                   /* CLEAR  */
    CANCEL = 302,                  /* CANCEL  */
    HELP = 303,                    /* HELP  */
    MENU = 304,                    /* MENU  */
    SELECT = 305,                  /* SELECT  */
    UNDO = 306,                    /* UNDO  */
    UPK = 307,                     /* UPK  */
    RIGHTK = 308,                  /* RIGHTK  */
    DOWNK = 309,                   /* DOWNK  */
    LEFTK = 310,                   /* LEFTK  */
    AUTOMAX = 311,                 /* AUTOMAX  */
    AUTOMIN = 312,                 /* AUTOMIN  */
    BACKTAB = 313,                 /* BACKTAB  */
    BEGINDATA = 314,               /* BEGINDATA  */
    DESELECTALL = 315,             /* DESELECTALL  */
    ENDDATA = 316,                 /* ENDDATA  */
    ENTER = 317,                   /* ENTER  */
    ESCAPE = 318,                  /* ESCAPE  */
    EXTEND = 319,                  /* EXTEND  */
    NEXTFIELD = 320,               /* NEXTFIELD  */
    NEXTMENU = 321,                /* NEXTMENU  */
    PAGERIGHT = 322,               /* PAGERIGHT  */
    PAGELEFT = 323,                /* PAGELEFT  */
    PREVFIELD = 324,               /* PREVFIELD  */
    PREVMENU = 325,                /* PREVMENU  */
    PRIMARYCOPY = 326,             /* PRIMARYCOPY  */
    PRIMARYCUT = 327,              /* PRIMARYCUT  */
    QUICKCOPY = 328,               /* QUICKCOPY  */
    QUICKCUT = 329,                /* QUICKCUT  */
    QUICKEXTEND = 330,             /* QUICKEXTEND  */
    RESELECT = 331,                /* RESELECT  */
    RESTORE = 332,                 /* RESTORE  */
    SELECTALL = 333,               /* SELECTALL  */
    SPACE = 334,                   /* SPACE  */
    TAB = 335,                     /* TAB  */
    KEYA = 336,                    /* KEYA  */
    KEYB = 337,                    /* KEYB  */
    KEYC = 338,                    /* KEYC  */
    KEYD = 339,                    /* KEYD  */
    KEYE = 340,                    /* KEYE  */
    KEYF = 341,                    /* KEYF  */
    KEYG = 342,                    /* KEYG  */
    KEYH = 343,                    /* KEYH  */
    KEYI = 344,                    /* KEYI  */
    KEYJ = 345,                    /* KEYJ  */
    KEYK = 346,                    /* KEYK  */
    KEYL = 347,                    /* KEYL  */
    KEYM = 348,                    /* KEYM  */
    KEYN = 349,                    /* KEYN  */
    KEYO = 350,                    /* KEYO  */
    KEYP = 351,                    /* KEYP  */
    KEYQ = 352,                    /* KEYQ  */
    KEYR = 353,                    /* KEYR  */
    KEYS = 354,                    /* KEYS  */
    KEYT = 355,                    /* KEYT  */
    KEYU = 356,                    /* KEYU  */
    KEYV = 357,                    /* KEYV  */
    KEYW = 358,                    /* KEYW  */
    KEYX = 359,                    /* KEYX  */
    KEYY = 360,                    /* KEYY  */
    KEYZ = 361,                    /* KEYZ  */
    KEYSLASH = 362,                /* KEYSLASH  */
    KEYBACKSLASH = 363,            /* KEYBACKSLASH  */
    KEYBRACERIGHT = 364,           /* KEYBRACERIGHT  */
    KEYBRACELEFT = 365,            /* KEYBRACELEFT  */
    KEYBRACKETRIGHT = 366,         /* KEYBRACKETRIGHT  */
    KEYBRACKETLEFT = 367,          /* KEYBRACKETLEFT  */
    KEYRETURN = 368,               /* KEYRETURN  */
    KEY0 = 369,                    /* KEY0  */
    KEY1 = 370,                    /* KEY1  */
    KEY2 = 371,                    /* KEY2  */
    KEY3 = 372,                    /* KEY3  */
    KEY4 = 373,                    /* KEY4  */
    KEY5 = 374,                    /* KEY5  */
    KEY6 = 375,                    /* KEY6  */
    KEY7 = 376,                    /* KEY7  */
    KEY8 = 377,                    /* KEY8  */
    KEY9 = 378,                    /* KEY9  */
    KEYEXCLAM = 379,               /* KEYEXCLAM  */
    KEYAT = 380,                   /* KEYAT  */
    KEYNUMBERSIGN = 381,           /* KEYNUMBERSIGN  */
    KEYDOLLAR = 382,               /* KEYDOLLAR  */
    KEYPERCENT = 383,              /* KEYPERCENT  */
    KEYCARET = 384,                /* KEYCARET  */
    KEYAMPERSAND = 385,            /* KEYAMPERSAND  */
    KEYASTERISK = 386,             /* KEYASTERISK  */
    KEYPARENLEFT = 387,            /* KEYPARENLEFT  */
    KEYPARENRIGHT = 388,           /* KEYPARENRIGHT  */
    KEYNONE = 389,                 /* KEYNONE  */
    SEPARATOR = 390,               /* SEPARATOR  */
    LABEL = 391,                   /* LABEL  */
    LISTITEM = 392,                /* LISTITEM  */
    FILELISTITEM = 393,            /* FILELISTITEM  */
    DIRLISTITEM = 394,             /* DIRLISTITEM  */
    DIRFILTERLABEL = 395,          /* DIRFILTERLABEL  */
    DIRFILTERTEXT = 396,           /* DIRFILTERTEXT  */
    DIRFILTERTEXTCHAR = 397,       /* DIRFILTERTEXTCHAR  */
    SELECTIONLISTITEM = 398,       /* SELECTIONLISTITEM  */
    TEXTCHAR = 399,                /* TEXTCHAR  */
    CSTEXTCHAR = 400,              /* CSTEXTCHAR  */
    SLIDER = 401,                  /* SLIDER  */
    COMBOBOXDROPDOWNBUTTON = 402,  /* COMBOBOXDROPDOWNBUTTON  */
    OUTLINEBTN = 403,              /* OUTLINEBTN  */
    INCREMENTBTN = 404,            /* INCREMENTBTN  */
    DECREMENTBTN = 405,            /* DECREMENTBTN  */
    PAGESCROLLER = 406,            /* PAGESCROLLER  */
    MAJORTABSCROLLERNEXT = 407,    /* MAJORTABSCROLLERNEXT  */
    MAJORTABSCROLLERPREV = 408,    /* MAJORTABSCROLLERPREV  */
    MINORTABSCROLLERNEXT = 409,    /* MINORTABSCROLLERNEXT  */
    MINORTABSCROLLERPREV = 410,    /* MINORTABSCROLLERPREV  */
    UPORLEFTARROW = 411,           /* UPORLEFTARROW  */
    DOWNORRIGHTARROW = 412,        /* DOWNORRIGHTARROW  */
    UPORLEFTSLIDERAREA = 413,      /* UPORLEFTSLIDERAREA  */
    DOWNORRIGHTSLIDERAREA = 414,   /* DOWNORRIGHTSLIDERAREA  */
    SCROLLBAR = 415,               /* SCROLLBAR  */
    HORIZSCROLLBAR = 416,          /* HORIZSCROLLBAR  */
    HORIZSBLEFTARROW = 417,        /* HORIZSBLEFTARROW  */
    HORIZSBRIGHTARROW = 418,       /* HORIZSBRIGHTARROW  */
    HORIZSBLEFTSLIDERAREA = 419,   /* HORIZSBLEFTSLIDERAREA  */
    HORIZSBRIGHTSLIDERAREA = 420,  /* HORIZSBRIGHTSLIDERAREA  */
    HORIZSBSLIDER = 421,           /* HORIZSBSLIDER  */
    VERTSCROLLBAR = 422,           /* VERTSCROLLBAR  */
    VERTSBUPARROW = 423,           /* VERTSBUPARROW  */
    VERTSBDOWNARROW = 424,         /* VERTSBDOWNARROW  */
    VERTSBUPSLIDERAREA = 425,      /* VERTSBUPSLIDERAREA  */
    VERTSBDOWNSLIDERAREA = 426,    /* VERTSBDOWNSLIDERAREA  */
    VERTSBSLIDER = 427,            /* VERTSBSLIDER  */
    SASH = 428,                    /* SASH  */
    CLOSEBUTTON = 429,             /* CLOSEBUTTON  */
    HELPBUTTON = 430,              /* HELPBUTTON  */
    OKBUTTON = 431,                /* OKBUTTON  */
    FILTERBUTTON = 432,            /* FILTERBUTTON  */
    CANCELBUTTON = 433,            /* CANCELBUTTON  */
    APPLYBUTTON = 434,             /* APPLYBUTTON  */
    MESSAGELABEL = 435,            /* MESSAGELABEL  */
    SYMBOLLABEL = 436,             /* SYMBOLLABEL  */
    SELECTIONTEXT = 437,           /* SELECTIONTEXT  */
    SELECTIONTEXTCHAR = 438,       /* SELECTIONTEXTCHAR  */
    SELECTIONLABEL = 439,          /* SELECTIONLABEL  */
    SELECTIONLIST = 440,           /* SELECTIONLIST  */
    SELECTIONLISTLABEL = 441,      /* SELECTIONLISTLABEL  */
    FILELIST = 442,                /* FILELIST  */
    FILELISTLABEL = 443,           /* FILELISTLABEL  */
    DIRLIST = 444,                 /* DIRLIST  */
    DIRLISTLABEL = 445,            /* DIRLISTLABEL  */
    FILESELECTIONTEXT = 446,       /* FILESELECTIONTEXT  */
    FILESELECTIONTEXTCHAR = 447,   /* FILESELECTIONTEXTCHAR  */
    FILESELECTIONLABEL = 448,      /* FILESELECTIONLABEL  */
    FILEFILTERTEXT = 449,          /* FILEFILTERTEXT  */
    FILEFILTERTEXTCHAR = 450,      /* FILEFILTERTEXTCHAR  */
    FILEFILTERLABEL = 451,         /* FILEFILTERLABEL  */
    WORKAREA = 452,                /* WORKAREA  */
    COMMANDWINDOW = 453,           /* COMMANDWINDOW  */
    MESSAGEWINDOW = 454,           /* MESSAGEWINDOW  */
    WORKWINDOW = 455,              /* WORKWINDOW  */
    SCALE = 456,                   /* SCALE  */
    SCALELABEL = 457,              /* SCALELABEL  */
    OPTIONBUTTON = 458,            /* OPTIONBUTTON  */
    SCROLLEDWINDOW = 459,          /* SCROLLEDWINDOW  */
    SCROLLBARSLIDER = 460,         /* SCROLLBARSLIDER  */
    SCROLLBARUPORLEFTARROW = 461,  /* SCROLLBARUPORLEFTARROW  */
    SCROLLBARDOWNORRIGHTARROW = 462, /* SCROLLBARDOWNORRIGHTARROW  */
    SCROLLBARUPORLEFTSLIDERAREA = 463, /* SCROLLBARUPORLEFTSLIDERAREA  */
    SCROLLBARDOWNORRIGHTSLIDERAREA = 464, /* SCROLLBARDOWNORRIGHTSLIDERAREA  */
    FLHSBUPORLEFTARROW = 465,      /* FLHSBUPORLEFTARROW  */
    FLHSBDOWNORRIGHTARROW = 466,   /* FLHSBDOWNORRIGHTARROW  */
    FLHSBUPORLEFTSLIDERAREA = 467, /* FLHSBUPORLEFTSLIDERAREA  */
    FLHSBDOWNORRIGHTSLIDERAREA = 468, /* FLHSBDOWNORRIGHTSLIDERAREA  */
    FLHSBSLIDER = 469,             /* FLHSBSLIDER  */
    FLVSBUPORLEFTARROW = 470,      /* FLVSBUPORLEFTARROW  */
    FLVSBDOWNORRIGHTARROW = 471,   /* FLVSBDOWNORRIGHTARROW  */
    FLVSBUPORLEFTSLIDERAREA = 472, /* FLVSBUPORLEFTSLIDERAREA  */
    FLVSBDOWNORRIGHTSLIDERAREA = 473, /* FLVSBDOWNORRIGHTSLIDERAREA  */
    FLVSBSLIDER = 474,             /* FLVSBSLIDER  */
    DLHSBUPORLEFTARROW = 475,      /* DLHSBUPORLEFTARROW  */
    DLHSBDOWNORRIGHTARROW = 476,   /* DLHSBDOWNORRIGHTARROW  */
    DLHSBUPORLEFTSLIDERAREA = 477, /* DLHSBUPORLEFTSLIDERAREA  */
    DLHSBDOWNORRIGHTSLIDERAREA = 478, /* DLHSBDOWNORRIGHTSLIDERAREA  */
    DLHSBSLIDER = 479,             /* DLHSBSLIDER  */
    DLVSBUPORLEFTARROW = 480,      /* DLVSBUPORLEFTARROW  */
    DLVSBDOWNORRIGHTARROW = 481,   /* DLVSBDOWNORRIGHTARROW  */
    DLVSBUPORLEFTSLIDERAREA = 482, /* DLVSBUPORLEFTSLIDERAREA  */
    DLVSBDOWNORRIGHTSLIDERAREA = 483, /* DLVSBDOWNORRIGHTSLIDERAREA  */
    DLVSBSLIDER = 484,             /* DLVSBSLIDER  */
    SLHSBUPORLEFTARROW = 485,      /* SLHSBUPORLEFTARROW  */
    SLHSBDOWNORRIGHTARROW = 486,   /* SLHSBDOWNORRIGHTARROW  */
    SLHSBUPORLEFTSLIDERAREA = 487, /* SLHSBUPORLEFTSLIDERAREA  */
    SLHSBDOWNORRIGHTSLIDERAREA = 488, /* SLHSBDOWNORRIGHTSLIDERAREA  */
    SLHSBSLIDER = 489,             /* SLHSBSLIDER  */
    SLVSBUPORLEFTARROW = 490,      /* SLVSBUPORLEFTARROW  */
    SLVSBDOWNORRIGHTARROW = 491,   /* SLVSBDOWNORRIGHTARROW  */
    SLVSBUPORLEFTSLIDERAREA = 492, /* SLVSBUPORLEFTSLIDERAREA  */
    SLVSBDOWNORRIGHTSLIDERAREA = 493, /* SLVSBDOWNORRIGHTSLIDERAREA  */
    SLVSBSLIDER = 494,             /* SLVSBSLIDER  */
    TEAROFFBUTTON = 495,           /* TEAROFFBUTTON  */
    LOCATEPOINTER = 496,           /* LOCATEPOINTER  */
    LOCATEPOINTERABS = 497,        /* LOCATEPOINTERABS  */
    LOCATEPOINTERREL = 498,        /* LOCATEPOINTERREL  */
    SYSTEM = 499,                  /* SYSTEM  */
    COMPAREVISUAL = 500,           /* COMPAREVISUAL  */
    SETFOCUS = 501,                /* SETFOCUS  */
    INPUTSTRING = 502,             /* INPUTSTRING  */
    STOREVISUAL = 503,             /* STOREVISUAL  */
    COMPARESTOREDVISUAL = 504,     /* COMPARESTOREDVISUAL  */
    DRAG = 505,                    /* DRAG  */
    DRAGRELATIVE = 506,            /* DRAGRELATIVE  */
    DRAGSLIDERVAL = 507,           /* DRAGSLIDERVAL  */
    DRAGABS = 508,                 /* DRAGABS  */
    WINDOWICONIFY = 509,           /* WINDOWICONIFY  */
    WINDOWDEICONIFY = 510,         /* WINDOWDEICONIFY  */
    WINDOWMENUPOST = 511,          /* WINDOWMENUPOST  */
    WINDOWMENUUNPOST = 512,        /* WINDOWMENUUNPOST  */
    WINDOWMENUSELECT = 513,        /* WINDOWMENUSELECT  */
    WINDOWMAXIMIZE = 514,          /* WINDOWMAXIMIZE  */
    WINDOWNORMALIZE = 515,         /* WINDOWNORMALIZE  */
    WINDOWRESIZE = 516,            /* WINDOWRESIZE  */
    WINDOWMOVE = 517,              /* WINDOWMOVE  */
    WINDOWRAISE = 518,             /* WINDOWRAISE  */
    WINDOWFOCUS = 519,             /* WINDOWFOCUS  */
    WINDOWCLICK = 520,             /* WINDOWCLICK  */
    WINDOWICHECK = 521,            /* WINDOWICHECK  */
    SYSTEMMENU = 522,              /* SYSTEMMENU  */
    ICONMENU = 523,                /* ICONMENU  */
    WINDOWDECORATION = 524,        /* WINDOWDECORATION  */
    ICONMENUPOST = 525,            /* ICONMENUPOST  */
    ICONMENUUNPOST = 526,          /* ICONMENUUNPOST  */
    ICONMENUSELECT = 527,          /* ICONMENUSELECT  */
    ICONMOVE = 528,                /* ICONMOVE  */
    NORTHWEST = 529,               /* NORTHWEST  */
    NORTH = 530,                   /* NORTH  */
    NORTHEAST = 531,               /* NORTHEAST  */
    WEST = 532,                    /* WEST  */
    EAST = 533,                    /* EAST  */
    SOUTHWEST = 534,               /* SOUTHWEST  */
    SOUTH = 535,                   /* SOUTH  */
    SOUTHEAST = 536,               /* SOUTHEAST  */
    KEYBOARD = 537,                /* KEYBOARD  */
    OFF = 538,                     /* OFF  */
    RIGHT = 539,                   /* RIGHT  */
    LEFT = 540,                    /* LEFT  */
    UP = 541,                      /* UP  */
    DOWN = 542,                    /* DOWN  */
    ANY = 543,                     /* ANY  */
    AUTOPERFORMMONITOR = 544,      /* AUTOPERFORMMONITOR  */
    ON = 545                       /* ON  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define STRING 258
#define DIGIT 259
#define ISTRING 260
#define CONTINUE 261
#define END 262
#define MANUAL 263
#define WAIT 264
#define PRESSMB 265
#define RELEASEMB 266
#define CLICKMB 267
#define SHIFTMASK 268
#define LOCKMASK 269
#define CTRLMASK 270
#define METAMASK 271
#define ALTMASK 272
#define MOD1MASK 273
#define MOD2MASK 274
#define MOD3MASK 275
#define MOD4MASK 276
#define MOD5MASK 277
#define BTN1 278
#define BTN2 279
#define BTN3 280
#define BTN4 281
#define BTN5 282
#define CLICKKEY 283
#define PRESSKEY 284
#define RELEASEKEY 285
#define BACKSPACE 286
#define INSERT 287
#define DELETE 288
#define COPY 289
#define CUT 290
#define PASTE 291
#define ADDMODE 292
#define PRIMARYPASTE 293
#define QUICKPASTE 294
#define PAGEUP 295
#define PAGEDOWN 296
#define ENDLINE 297
#define BEGINLINE 298
#define ACTIVATE 299
#define MENUBAR 300
#define CLEAR 301
#define CANCEL 302
#define HELP 303
#define MENU 304
#define SELECT 305
#define UNDO 306
#define UPK 307
#define RIGHTK 308
#define DOWNK 309
#define LEFTK 310
#define AUTOMAX 311
#define AUTOMIN 312
#define BACKTAB 313
#define BEGINDATA 314
#define DESELECTALL 315
#define ENDDATA 316
#define ENTER 317
#define ESCAPE 318
#define EXTEND 319
#define NEXTFIELD 320
#define NEXTMENU 321
#define PAGERIGHT 322
#define PAGELEFT 323
#define PREVFIELD 324
#define PREVMENU 325
#define PRIMARYCOPY 326
#define PRIMARYCUT 327
#define QUICKCOPY 328
#define QUICKCUT 329
#define QUICKEXTEND 330
#define RESELECT 331
#define RESTORE 332
#define SELECTALL 333
#define SPACE 334
#define TAB 335
#define KEYA 336
#define KEYB 337
#define KEYC 338
#define KEYD 339
#define KEYE 340
#define KEYF 341
#define KEYG 342
#define KEYH 343
#define KEYI 344
#define KEYJ 345
#define KEYK 346
#define KEYL 347
#define KEYM 348
#define KEYN 349
#define KEYO 350
#define KEYP 351
#define KEYQ 352
#define KEYR 353
#define KEYS 354
#define KEYT 355
#define KEYU 356
#define KEYV 357
#define KEYW 358
#define KEYX 359
#define KEYY 360
#define KEYZ 361
#define KEYSLASH 362
#define KEYBACKSLASH 363
#define KEYBRACERIGHT 364
#define KEYBRACELEFT 365
#define KEYBRACKETRIGHT 366
#define KEYBRACKETLEFT 367
#define KEYRETURN 368
#define KEY0 369
#define KEY1 370
#define KEY2 371
#define KEY3 372
#define KEY4 373
#define KEY5 374
#define KEY6 375
#define KEY7 376
#define KEY8 377
#define KEY9 378
#define KEYEXCLAM 379
#define KEYAT 380
#define KEYNUMBERSIGN 381
#define KEYDOLLAR 382
#define KEYPERCENT 383
#define KEYCARET 384
#define KEYAMPERSAND 385
#define KEYASTERISK 386
#define KEYPARENLEFT 387
#define KEYPARENRIGHT 388
#define KEYNONE 389
#define SEPARATOR 390
#define LABEL 391
#define LISTITEM 392
#define FILELISTITEM 393
#define DIRLISTITEM 394
#define DIRFILTERLABEL 395
#define DIRFILTERTEXT 396
#define DIRFILTERTEXTCHAR 397
#define SELECTIONLISTITEM 398
#define TEXTCHAR 399
#define CSTEXTCHAR 400
#define SLIDER 401
#define COMBOBOXDROPDOWNBUTTON 402
#define OUTLINEBTN 403
#define INCREMENTBTN 404
#define DECREMENTBTN 405
#define PAGESCROLLER 406
#define MAJORTABSCROLLERNEXT 407
#define MAJORTABSCROLLERPREV 408
#define MINORTABSCROLLERNEXT 409
#define MINORTABSCROLLERPREV 410
#define UPORLEFTARROW 411
#define DOWNORRIGHTARROW 412
#define UPORLEFTSLIDERAREA 413
#define DOWNORRIGHTSLIDERAREA 414
#define SCROLLBAR 415
#define HORIZSCROLLBAR 416
#define HORIZSBLEFTARROW 417
#define HORIZSBRIGHTARROW 418
#define HORIZSBLEFTSLIDERAREA 419
#define HORIZSBRIGHTSLIDERAREA 420
#define HORIZSBSLIDER 421
#define VERTSCROLLBAR 422
#define VERTSBUPARROW 423
#define VERTSBDOWNARROW 424
#define VERTSBUPSLIDERAREA 425
#define VERTSBDOWNSLIDERAREA 426
#define VERTSBSLIDER 427
#define SASH 428
#define CLOSEBUTTON 429
#define HELPBUTTON 430
#define OKBUTTON 431
#define FILTERBUTTON 432
#define CANCELBUTTON 433
#define APPLYBUTTON 434
#define MESSAGELABEL 435
#define SYMBOLLABEL 436
#define SELECTIONTEXT 437
#define SELECTIONTEXTCHAR 438
#define SELECTIONLABEL 439
#define SELECTIONLIST 440
#define SELECTIONLISTLABEL 441
#define FILELIST 442
#define FILELISTLABEL 443
#define DIRLIST 444
#define DIRLISTLABEL 445
#define FILESELECTIONTEXT 446
#define FILESELECTIONTEXTCHAR 447
#define FILESELECTIONLABEL 448
#define FILEFILTERTEXT 449
#define FILEFILTERTEXTCHAR 450
#define FILEFILTERLABEL 451
#define WORKAREA 452
#define COMMANDWINDOW 453
#define MESSAGEWINDOW 454
#define WORKWINDOW 455
#define SCALE 456
#define SCALELABEL 457
#define OPTIONBUTTON 458
#define SCROLLEDWINDOW 459
#define SCROLLBARSLIDER 460
#define SCROLLBARUPORLEFTARROW 461
#define SCROLLBARDOWNORRIGHTARROW 462
#define SCROLLBARUPORLEFTSLIDERAREA 463
#define SCROLLBARDOWNORRIGHTSLIDERAREA 464
#define FLHSBUPORLEFTARROW 465
#define FLHSBDOWNORRIGHTARROW 466
#define FLHSBUPORLEFTSLIDERAREA 467
#define FLHSBDOWNORRIGHTSLIDERAREA 468
#define FLHSBSLIDER 469
#define FLVSBUPORLEFTARROW 470
#define FLVSBDOWNORRIGHTARROW 471
#define FLVSBUPORLEFTSLIDERAREA 472
#define FLVSBDOWNORRIGHTSLIDERAREA 473
#define FLVSBSLIDER 474
#define DLHSBUPORLEFTARROW 475
#define DLHSBDOWNORRIGHTARROW 476
#define DLHSBUPORLEFTSLIDERAREA 477
#define DLHSBDOWNORRIGHTSLIDERAREA 478
#define DLHSBSLIDER 479
#define DLVSBUPORLEFTARROW 480
#define DLVSBDOWNORRIGHTARROW 481
#define DLVSBUPORLEFTSLIDERAREA 482
#define DLVSBDOWNORRIGHTSLIDERAREA 483
#define DLVSBSLIDER 484
#define SLHSBUPORLEFTARROW 485
#define SLHSBDOWNORRIGHTARROW 486
#define SLHSBUPORLEFTSLIDERAREA 487
#define SLHSBDOWNORRIGHTSLIDERAREA 488
#define SLHSBSLIDER 489
#define SLVSBUPORLEFTARROW 490
#define SLVSBDOWNORRIGHTARROW 491
#define SLVSBUPORLEFTSLIDERAREA 492
#define SLVSBDOWNORRIGHTSLIDERAREA 493
#define SLVSBSLIDER 494
#define TEAROFFBUTTON 495
#define LOCATEPOINTER 496
#define LOCATEPOINTERABS 497
#define LOCATEPOINTERREL 498
#define SYSTEM 499
#define COMPAREVISUAL 500
#define SETFOCUS 501
#define INPUTSTRING 502
#define STOREVISUAL 503
#define COMPARESTOREDVISUAL 504
#define DRAG 505
#define DRAGRELATIVE 506
#define DRAGSLIDERVAL 507
#define DRAGABS 508
#define WINDOWICONIFY 509
#define WINDOWDEICONIFY 510
#define WINDOWMENUPOST 511
#define WINDOWMENUUNPOST 512
#define WINDOWMENUSELECT 513
#define WINDOWMAXIMIZE 514
#define WINDOWNORMALIZE 515
#define WINDOWRESIZE 516
#define WINDOWMOVE 517
#define WINDOWRAISE 518
#define WINDOWFOCUS 519
#define WINDOWCLICK 520
#define WINDOWICHECK 521
#define SYSTEMMENU 522
#define ICONMENU 523
#define WINDOWDECORATION 524
#define ICONMENUPOST 525
#define ICONMENUUNPOST 526
#define ICONMENUSELECT 527
#define ICONMOVE 528
#define NORTHWEST 529
#define NORTH 530
#define NORTHEAST 531
#define WEST 532
#define EAST 533
#define SOUTHWEST 534
#define SOUTH 535
#define SOUTHEAST 536
#define KEYBOARD 537
#define OFF 538
#define RIGHT 539
#define LEFT 540
#define UP 541
#define DOWN 542
#define ANY 543
#define AUTOPERFORMMONITOR 544
#define ON 545

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_STRING = 3,                     /* STRING  */
  YYSYMBOL_DIGIT = 4,                      /* DIGIT  */
  YYSYMBOL_ISTRING = 5,                    /* ISTRING  */
  YYSYMBOL_CONTINUE = 6,                   /* CONTINUE  */
  YYSYMBOL_END = 7,                        /* END  */
  YYSYMBOL_MANUAL = 8,                     /* MANUAL  */
  YYSYMBOL_WAIT = 9,                       /* WAIT  */
  YYSYMBOL_PRESSMB = 10,                   /* PRESSMB  */
  YYSYMBOL_RELEASEMB = 11,                 /* RELEASEMB  */
  YYSYMBOL_CLICKMB = 12,                   /* CLICKMB  */
  YYSYMBOL_SHIFTMASK = 13,                 /* SHIFTMASK  */
  YYSYMBOL_LOCKMASK = 14,                  /* LOCKMASK  */
  YYSYMBOL_CTRLMASK = 15,                  /* CTRLMASK  */
  YYSYMBOL_METAMASK = 16,                  /* METAMASK  */
  YYSYMBOL_ALTMASK = 17,                   /* ALTMASK  */
  YYSYMBOL_MOD1MASK = 18,                  /* MOD1MASK  */
  YYSYMBOL_MOD2MASK = 19,                  /* MOD2MASK  */
  YYSYMBOL_MOD3MASK = 20,                  /* MOD3MASK  */
  YYSYMBOL_MOD4MASK = 21,                  /* MOD4MASK  */
  YYSYMBOL_MOD5MASK = 22,                  /* MOD5MASK  */
  YYSYMBOL_BTN1 = 23,                      /* BTN1  */
  YYSYMBOL_BTN2 = 24,                      /* BTN2  */
  YYSYMBOL_BTN3 = 25,                      /* BTN3  */
  YYSYMBOL_BTN4 = 26,                      /* BTN4  */
  YYSYMBOL_BTN5 = 27,                      /* BTN5  */
  YYSYMBOL_CLICKKEY = 28,                  /* CLICKKEY  */
  YYSYMBOL_PRESSKEY = 29,                  /* PRESSKEY  */
  YYSYMBOL_RELEASEKEY = 30,                /* RELEASEKEY  */
  YYSYMBOL_BACKSPACE = 31,                 /* BACKSPACE  */
  YYSYMBOL_INSERT = 32,                    /* INSERT  */
  YYSYMBOL_DELETE = 33,                    /* DELETE  */
  YYSYMBOL_COPY = 34,                      /* COPY  */
  YYSYMBOL_CUT = 35,                       /* CUT  */
  YYSYMBOL_PASTE = 36,                     /* PASTE  */
  YYSYMBOL_ADDMODE = 37,                   /* ADDMODE  */
  YYSYMBOL_PRIMARYPASTE = 38,              /* PRIMARYPASTE  */
  YYSYMBOL_QUICKPASTE = 39,                /* QUICKPASTE  */
  YYSYMBOL_PAGEUP = 40,                    /* PAGEUP  */
  YYSYMBOL_PAGEDOWN = 41,                  /* PAGEDOWN  */
  YYSYMBOL_ENDLINE = 42,                   /* ENDLINE  */
  YYSYMBOL_BEGINLINE = 43,                 /* BEGINLINE  */
  YYSYMBOL_ACTIVATE = 44,                  /* ACTIVATE  */
  YYSYMBOL_MENUBAR = 45,                   /* MENUBAR  */
  YYSYMBOL_CLEAR = 46,                     /* CLEAR  */
  YYSYMBOL_CANCEL = 47,                    /* CANCEL  */
  YYSYMBOL_HELP = 48,                      /* HELP  */
  YYSYMBOL_MENU = 49,                      /* MENU  */
  YYSYMBOL_SELECT = 50,                    /* SELECT  */
  YYSYMBOL_UNDO = 51,                      /* UNDO  */
  YYSYMBOL_UPK = 52,                       /* UPK  */
  YYSYMBOL_RIGHTK = 53,                    /* RIGHTK  */
  YYSYMBOL_DOWNK = 54,                     /* DOWNK  */
  YYSYMBOL_LEFTK = 55,                     /* LEFTK  */
  YYSYMBOL_AUTOMAX = 56,                   /* AUTOMAX  */
  YYSYMBOL_AUTOMIN = 57,                   /* AUTOMIN  */
  YYSYMBOL_BACKTAB = 58,                   /* BACKTAB  */
  YYSYMBOL_BEGINDATA = 59,                 /* BEGINDATA  */
  YYSYMBOL_DESELECTALL = 60,               /* DESELECTALL  */
  YYSYMBOL_ENDDATA = 61,                   /* ENDDATA  */
  YYSYMBOL_ENTER = 62,                     /* ENTER  */
  YYSYMBOL_ESCAPE = 63,                    /* ESCAPE  */
  YYSYMBOL_EXTEND = 64,                    /* EXTEND  */
  YYSYMBOL_NEXTFIELD = 65,                 /* NEXTFIELD  */
  YYSYMBOL_NEXTMENU = 66,                  /* NEXTMENU  */
  YYSYMBOL_PAGERIGHT = 67,                 /* PAGERIGHT  */
  YYSYMBOL_PAGELEFT = 68,                  /* PAGELEFT  */
  YYSYMBOL_PREVFIELD = 69,                 /* PREVFIELD  */
  YYSYMBOL_PREVMENU = 70,                  /* PREVMENU  */
  YYSYMBOL_PRIMARYCOPY = 71,               /* PRIMARYCOPY  */
  YYSYMBOL_PRIMARYCUT = 72,                /* PRIMARYCUT  */
  YYSYMBOL_QUICKCOPY = 73,                 /* QUICKCOPY  */
  YYSYMBOL_QUICKCUT = 74,                  /* QUICKCUT  */
  YYSYMBOL_QUICKEXTEND = 75,               /* QUICKEXTEND  */
  YYSYMBOL_RESELECT = 76,                  /* RESELECT  */
  YYSYMBOL_RESTORE = 77,                   /* RESTORE  */
  YYSYMBOL_SELECTALL = 78,                 /* SELECTALL  */
  YYSYMBOL_SPACE = 79,                     /* SPACE  */
  YYSYMBOL_TAB = 80,                       /* TAB  */
  YYSYMBOL_KEYA = 81,                      /* KEYA  */
  YYSYMBOL_KEYB = 82,                      /* KEYB  */
  YYSYMBOL_KEYC = 83,                      /* KEYC  */
  YYSYMBOL_KEYD = 84,                      /* KEYD  */
  YYSYMBOL_KEYE = 85,                      /* KEYE  */
  YYSYMBOL_KEYF = 86,                      /* KEYF  */
  YYSYMBOL_KEYG = 87,                      /* KEYG  */
  YYSYMBOL_KEYH = 88,                      /* KEYH  */
  YYSYMBOL_KEYI = 89,                      /* KEYI  */
  YYSYMBOL_KEYJ = 90,                      /* KEYJ  */
  YYSYMBOL_KEYK = 91,                      /* KEYK  */
  YYSYMBOL_KEYL = 92,                      /* KEYL  */
  YYSYMBOL_KEYM = 93,                      /* KEYM  */
  YYSYMBOL_KEYN = 94,                      /* KEYN  */
  YYSYMBOL_KEYO = 95,                      /* KEYO  */
  YYSYMBOL_KEYP = 96,                      /* KEYP  */
  YYSYMBOL_KEYQ = 97,                      /* KEYQ  */
  YYSYMBOL_KEYR = 98,                      /* KEYR  */
  YYSYMBOL_KEYS = 99,                      /* KEYS  */
  YYSYMBOL_KEYT = 100,                     /* KEYT  */
  YYSYMBOL_KEYU = 101,                     /* KEYU  */
  YYSYMBOL_KEYV = 102,                     /* KEYV  */
  YYSYMBOL_KEYW = 103,                     /* KEYW  */
  YYSYMBOL_KEYX = 104,                     /* KEYX  */
  YYSYMBOL_KEYY = 105,                     /* KEYY  */
  YYSYMBOL_KEYZ = 106,                     /* KEYZ  */
  YYSYMBOL_KEYSLASH = 107,                 /* KEYSLASH  */
  YYSYMBOL_KEYBACKSLASH = 108,             /* KEYBACKSLASH  */
  YYSYMBOL_KEYBRACERIGHT = 109,            /* KEYBRACERIGHT  */
  YYSYMBOL_KEYBRACELEFT = 110,             /* KEYBRACELEFT  */
  YYSYMBOL_KEYBRACKETRIGHT = 111,          /* KEYBRACKETRIGHT  */
  YYSYMBOL_KEYBRACKETLEFT = 112,           /* KEYBRACKETLEFT  */
  YYSYMBOL_KEYRETURN = 113,                /* KEYRETURN  */
  YYSYMBOL_KEY0 = 114,                     /* KEY0  */
  YYSYMBOL_KEY1 = 115,                     /* KEY1  */
  YYSYMBOL_KEY2 = 116,                     /* KEY2  */
  YYSYMBOL_KEY3 = 117,                     /* KEY3  */
  YYSYMBOL_KEY4 = 118,                     /* KEY4  */
  YYSYMBOL_KEY5 = 119,                     /* KEY5  */
  YYSYMBOL_KEY6 = 120,                     /* KEY6  */
  YYSYMBOL_KEY7 = 121,                     /* KEY7  */
  YYSYMBOL_KEY8 = 122,                     /* KEY8  */
  YYSYMBOL_KEY9 = 123,                     /* KEY9  */
  YYSYMBOL_KEYEXCLAM = 124,                /* KEYEXCLAM  */
  YYSYMBOL_KEYAT = 125,                    /* KEYAT  */
  YYSYMBOL_KEYNUMBERSIGN = 126,            /* KEYNUMBERSIGN  */
  YYSYMBOL_KEYDOLLAR = 127,                /* KEYDOLLAR  */
  YYSYMBOL_KEYPERCENT = 128,               /* KEYPERCENT  */
  YYSYMBOL_KEYCARET = 129,                 /* KEYCARET  */
  YYSYMBOL_KEYAMPERSAND = 130,             /* KEYAMPERSAND  */
  YYSYMBOL_KEYASTERISK = 131,              /* KEYASTERISK  */
  YYSYMBOL_KEYPARENLEFT = 132,             /* KEYPARENLEFT  */
  YYSYMBOL_KEYPARENRIGHT = 133,            /* KEYPARENRIGHT  */
  YYSYMBOL_KEYNONE = 134,                  /* KEYNONE  */
  YYSYMBOL_SEPARATOR = 135,                /* SEPARATOR  */
  YYSYMBOL_LABEL = 136,                    /* LABEL  */
  YYSYMBOL_LISTITEM = 137,                 /* LISTITEM  */
  YYSYMBOL_FILELISTITEM = 138,             /* FILELISTITEM  */
  YYSYMBOL_DIRLISTITEM = 139,              /* DIRLISTITEM  */
  YYSYMBOL_DIRFILTERLABEL = 140,           /* DIRFILTERLABEL  */
  YYSYMBOL_DIRFILTERTEXT = 141,            /* DIRFILTERTEXT  */
  YYSYMBOL_DIRFILTERTEXTCHAR = 142,        /* DIRFILTERTEXTCHAR  */
  YYSYMBOL_SELECTIONLISTITEM = 143,        /* SELECTIONLISTITEM  */
  YYSYMBOL_TEXTCHAR = 144,                 /* TEXTCHAR  */
  YYSYMBOL_CSTEXTCHAR = 145,               /* CSTEXTCHAR  */
  YYSYMBOL_SLIDER = 146,                   /* SLIDER  */
  YYSYMBOL_COMBOBOXDROPDOWNBUTTON = 147,   /* COMBOBOXDROPDOWNBUTTON  */
  YYSYMBOL_OUTLINEBTN = 148,               /* OUTLINEBTN  */
  YYSYMBOL_INCREMENTBTN = 149,             /* INCREMENTBTN  */
  YYSYMBOL_DECREMENTBTN = 150,             /* DECREMENTBTN  */
  YYSYMBOL_PAGESCROLLER = 151,             /* PAGESCROLLER  */
  YYSYMBOL_MAJORTABSCROLLERNEXT = 152,     /* MAJORTABSCROLLERNEXT  */
  YYSYMBOL_MAJORTABSCROLLERPREV = 153,     /* MAJORTABSCROLLERPREV  */
  YYSYMBOL_MINORTABSCROLLERNEXT = 154,     /* MINORTABSCROLLERNEXT  */
  YYSYMBOL_MINORTABSCROLLERPREV = 155,     /* MINORTABSCROLLERPREV  */
  YYSYMBOL_UPORLEFTARROW = 156,            /* UPORLEFTARROW  */
  YYSYMBOL_DOWNORRIGHTARROW = 157,         /* DOWNORRIGHTARROW  */
  YYSYMBOL_UPORLEFTSLIDERAREA = 158,       /* UPORLEFTSLIDERAREA  */
  YYSYMBOL_DOWNORRIGHTSLIDERAREA = 159,    /* DOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_SCROLLBAR = 160,                /* SCROLLBAR  */
  YYSYMBOL_HORIZSCROLLBAR = 161,           /* HORIZSCROLLBAR  */
  YYSYMBOL_HORIZSBLEFTARROW = 162,         /* HORIZSBLEFTARROW  */
  YYSYMBOL_HORIZSBRIGHTARROW = 163,        /* HORIZSBRIGHTARROW  */
  YYSYMBOL_HORIZSBLEFTSLIDERAREA = 164,    /* HORIZSBLEFTSLIDERAREA  */
  YYSYMBOL_HORIZSBRIGHTSLIDERAREA = 165,   /* HORIZSBRIGHTSLIDERAREA  */
  YYSYMBOL_HORIZSBSLIDER = 166,            /* HORIZSBSLIDER  */
  YYSYMBOL_VERTSCROLLBAR = 167,            /* VERTSCROLLBAR  */
  YYSYMBOL_VERTSBUPARROW = 168,            /* VERTSBUPARROW  */
  YYSYMBOL_VERTSBDOWNARROW = 169,          /* VERTSBDOWNARROW  */
  YYSYMBOL_VERTSBUPSLIDERAREA = 170,       /* VERTSBUPSLIDERAREA  */
  YYSYMBOL_VERTSBDOWNSLIDERAREA = 171,     /* VERTSBDOWNSLIDERAREA  */
  YYSYMBOL_VERTSBSLIDER = 172,             /* VERTSBSLIDER  */
  YYSYMBOL_SASH = 173,                     /* SASH  */
  YYSYMBOL_CLOSEBUTTON = 174,              /* CLOSEBUTTON  */
  YYSYMBOL_HELPBUTTON = 175,               /* HELPBUTTON  */
  YYSYMBOL_OKBUTTON = 176,                 /* OKBUTTON  */
  YYSYMBOL_FILTERBUTTON = 177,             /* FILTERBUTTON  */
  YYSYMBOL_CANCELBUTTON = 178,             /* CANCELBUTTON  */
  YYSYMBOL_APPLYBUTTON = 179,              /* APPLYBUTTON  */
  YYSYMBOL_MESSAGELABEL = 180,             /* MESSAGELABEL  */
  YYSYMBOL_SYMBOLLABEL = 181,              /* SYMBOLLABEL  */
  YYSYMBOL_SELECTIONTEXT = 182,            /* SELECTIONTEXT  */
  YYSYMBOL_SELECTIONTEXTCHAR = 183,        /* SELECTIONTEXTCHAR  */
  YYSYMBOL_SELECTIONLABEL = 184,           /* SELECTIONLABEL  */
  YYSYMBOL_SELECTIONLIST = 185,            /* SELECTIONLIST  */
  YYSYMBOL_SELECTIONLISTLABEL = 186,       /* SELECTIONLISTLABEL  */
  YYSYMBOL_FILELIST = 187,                 /* FILELIST  */
  YYSYMBOL_FILELISTLABEL = 188,            /* FILELISTLABEL  */
  YYSYMBOL_DIRLIST = 189,                  /* DIRLIST  */
  YYSYMBOL_DIRLISTLABEL = 190,             /* DIRLISTLABEL  */
  YYSYMBOL_FILESELECTIONTEXT = 191,        /* FILESELECTIONTEXT  */
  YYSYMBOL_FILESELECTIONTEXTCHAR = 192,    /* FILESELECTIONTEXTCHAR  */
  YYSYMBOL_FILESELECTIONLABEL = 193,       /* FILESELECTIONLABEL  */
  YYSYMBOL_FILEFILTERTEXT = 194,           /* FILEFILTERTEXT  */
  YYSYMBOL_FILEFILTERTEXTCHAR = 195,       /* FILEFILTERTEXTCHAR  */
  YYSYMBOL_FILEFILTERLABEL = 196,          /* FILEFILTERLABEL  */
  YYSYMBOL_WORKAREA = 197,                 /* WORKAREA  */
  YYSYMBOL_COMMANDWINDOW = 198,            /* COMMANDWINDOW  */
  YYSYMBOL_MESSAGEWINDOW = 199,            /* MESSAGEWINDOW  */
  YYSYMBOL_WORKWINDOW = 200,               /* WORKWINDOW  */
  YYSYMBOL_SCALE = 201,                    /* SCALE  */
  YYSYMBOL_SCALELABEL = 202,               /* SCALELABEL  */
  YYSYMBOL_OPTIONBUTTON = 203,             /* OPTIONBUTTON  */
  YYSYMBOL_SCROLLEDWINDOW = 204,           /* SCROLLEDWINDOW  */
  YYSYMBOL_SCROLLBARSLIDER = 205,          /* SCROLLBARSLIDER  */
  YYSYMBOL_SCROLLBARUPORLEFTARROW = 206,   /* SCROLLBARUPORLEFTARROW  */
  YYSYMBOL_SCROLLBARDOWNORRIGHTARROW = 207, /* SCROLLBARDOWNORRIGHTARROW  */
  YYSYMBOL_SCROLLBARUPORLEFTSLIDERAREA = 208, /* SCROLLBARUPORLEFTSLIDERAREA  */
  YYSYMBOL_SCROLLBARDOWNORRIGHTSLIDERAREA = 209, /* SCROLLBARDOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_FLHSBUPORLEFTARROW = 210,       /* FLHSBUPORLEFTARROW  */
  YYSYMBOL_FLHSBDOWNORRIGHTARROW = 211,    /* FLHSBDOWNORRIGHTARROW  */
  YYSYMBOL_FLHSBUPORLEFTSLIDERAREA = 212,  /* FLHSBUPORLEFTSLIDERAREA  */
  YYSYMBOL_FLHSBDOWNORRIGHTSLIDERAREA = 213, /* FLHSBDOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_FLHSBSLIDER = 214,              /* FLHSBSLIDER  */
  YYSYMBOL_FLVSBUPORLEFTARROW = 215,       /* FLVSBUPORLEFTARROW  */
  YYSYMBOL_FLVSBDOWNORRIGHTARROW = 216,    /* FLVSBDOWNORRIGHTARROW  */
  YYSYMBOL_FLVSBUPORLEFTSLIDERAREA = 217,  /* FLVSBUPORLEFTSLIDERAREA  */
  YYSYMBOL_FLVSBDOWNORRIGHTSLIDERAREA = 218, /* FLVSBDOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_FLVSBSLIDER = 219,              /* FLVSBSLIDER  */
  YYSYMBOL_DLHSBUPORLEFTARROW = 220,       /* DLHSBUPORLEFTARROW  */
  YYSYMBOL_DLHSBDOWNORRIGHTARROW = 221,    /* DLHSBDOWNORRIGHTARROW  */
  YYSYMBOL_DLHSBUPORLEFTSLIDERAREA = 222,  /* DLHSBUPORLEFTSLIDERAREA  */
  YYSYMBOL_DLHSBDOWNORRIGHTSLIDERAREA = 223, /* DLHSBDOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_DLHSBSLIDER = 224,              /* DLHSBSLIDER  */
  YYSYMBOL_DLVSBUPORLEFTARROW = 225,       /* DLVSBUPORLEFTARROW  */
  YYSYMBOL_DLVSBDOWNORRIGHTARROW = 226,    /* DLVSBDOWNORRIGHTARROW  */
  YYSYMBOL_DLVSBUPORLEFTSLIDERAREA = 227,  /* DLVSBUPORLEFTSLIDERAREA  */
  YYSYMBOL_DLVSBDOWNORRIGHTSLIDERAREA = 228, /* DLVSBDOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_DLVSBSLIDER = 229,              /* DLVSBSLIDER  */
  YYSYMBOL_SLHSBUPORLEFTARROW = 230,       /* SLHSBUPORLEFTARROW  */
  YYSYMBOL_SLHSBDOWNORRIGHTARROW = 231,    /* SLHSBDOWNORRIGHTARROW  */
  YYSYMBOL_SLHSBUPORLEFTSLIDERAREA = 232,  /* SLHSBUPORLEFTSLIDERAREA  */
  YYSYMBOL_SLHSBDOWNORRIGHTSLIDERAREA = 233, /* SLHSBDOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_SLHSBSLIDER = 234,              /* SLHSBSLIDER  */
  YYSYMBOL_SLVSBUPORLEFTARROW = 235,       /* SLVSBUPORLEFTARROW  */
  YYSYMBOL_SLVSBDOWNORRIGHTARROW = 236,    /* SLVSBDOWNORRIGHTARROW  */
  YYSYMBOL_SLVSBUPORLEFTSLIDERAREA = 237,  /* SLVSBUPORLEFTSLIDERAREA  */
  YYSYMBOL_SLVSBDOWNORRIGHTSLIDERAREA = 238, /* SLVSBDOWNORRIGHTSLIDERAREA  */
  YYSYMBOL_SLVSBSLIDER = 239,              /* SLVSBSLIDER  */
  YYSYMBOL_TEAROFFBUTTON = 240,            /* TEAROFFBUTTON  */
  YYSYMBOL_LOCATEPOINTER = 241,            /* LOCATEPOINTER  */
  YYSYMBOL_LOCATEPOINTERABS = 242,         /* LOCATEPOINTERABS  */
  YYSYMBOL_LOCATEPOINTERREL = 243,         /* LOCATEPOINTERREL  */
  YYSYMBOL_SYSTEM = 244,                   /* SYSTEM  */
  YYSYMBOL_COMPAREVISUAL = 245,            /* COMPAREVISUAL  */
  YYSYMBOL_SETFOCUS = 246,                 /* SETFOCUS  */
  YYSYMBOL_INPUTSTRING = 247,              /* INPUTSTRING  */
  YYSYMBOL_STOREVISUAL = 248,              /* STOREVISUAL  */
  YYSYMBOL_COMPARESTOREDVISUAL = 249,      /* COMPARESTOREDVISUAL  */
  YYSYMBOL_DRAG = 250,                     /* DRAG  */
  YYSYMBOL_DRAGRELATIVE = 251,             /* DRAGRELATIVE  */
  YYSYMBOL_DRAGSLIDERVAL = 252,            /* DRAGSLIDERVAL  */
  YYSYMBOL_DRAGABS = 253,                  /* DRAGABS  */
  YYSYMBOL_WINDOWICONIFY = 254,            /* WINDOWICONIFY  */
  YYSYMBOL_WINDOWDEICONIFY = 255,          /* WINDOWDEICONIFY  */
  YYSYMBOL_WINDOWMENUPOST = 256,           /* WINDOWMENUPOST  */
  YYSYMBOL_WINDOWMENUUNPOST = 257,         /* WINDOWMENUUNPOST  */
  YYSYMBOL_WINDOWMENUSELECT = 258,         /* WINDOWMENUSELECT  */
  YYSYMBOL_WINDOWMAXIMIZE = 259,           /* WINDOWMAXIMIZE  */
  YYSYMBOL_WINDOWNORMALIZE = 260,          /* WINDOWNORMALIZE  */
  YYSYMBOL_WINDOWRESIZE = 261,             /* WINDOWRESIZE  */
  YYSYMBOL_WINDOWMOVE = 262,               /* WINDOWMOVE  */
  YYSYMBOL_WINDOWRAISE = 263,              /* WINDOWRAISE  */
  YYSYMBOL_WINDOWFOCUS = 264,              /* WINDOWFOCUS  */
  YYSYMBOL_WINDOWCLICK = 265,              /* WINDOWCLICK  */
  YYSYMBOL_WINDOWICHECK = 266,             /* WINDOWICHECK  */
  YYSYMBOL_SYSTEMMENU = 267,               /* SYSTEMMENU  */
  YYSYMBOL_ICONMENU = 268,                 /* ICONMENU  */
  YYSYMBOL_WINDOWDECORATION = 269,         /* WINDOWDECORATION  */
  YYSYMBOL_ICONMENUPOST = 270,             /* ICONMENUPOST  */
  YYSYMBOL_ICONMENUUNPOST = 271,           /* ICONMENUUNPOST  */
  YYSYMBOL_ICONMENUSELECT = 272,           /* ICONMENUSELECT  */
  YYSYMBOL_ICONMOVE = 273,                 /* ICONMOVE  */
  YYSYMBOL_NORTHWEST = 274,                /* NORTHWEST  */
  YYSYMBOL_NORTH = 275,                    /* NORTH  */
  YYSYMBOL_NORTHEAST = 276,                /* NORTHEAST  */
  YYSYMBOL_WEST = 277,                     /* WEST  */
  YYSYMBOL_EAST = 278,                     /* EAST  */
  YYSYMBOL_SOUTHWEST = 279,                /* SOUTHWEST  */
  YYSYMBOL_SOUTH = 280,                    /* SOUTH  */
  YYSYMBOL_SOUTHEAST = 281,                /* SOUTHEAST  */
  YYSYMBOL_KEYBOARD = 282,                 /* KEYBOARD  */
  YYSYMBOL_OFF = 283,                      /* OFF  */
  YYSYMBOL_RIGHT = 284,                    /* RIGHT  */
  YYSYMBOL_LEFT = 285,                     /* LEFT  */
  YYSYMBOL_UP = 286,                       /* UP  */
  YYSYMBOL_DOWN = 287,                     /* DOWN  */
  YYSYMBOL_ANY = 288,                      /* ANY  */
  YYSYMBOL_AUTOPERFORMMONITOR = 289,       /* AUTOPERFORMMONITOR  */
  YYSYMBOL_ON = 290,                       /* ON  */
  YYSYMBOL_YYACCEPT = 291,                 /* $accept  */
  YYSYMBOL_CommandList = 292,              /* CommandList  */
  YYSYMBOL_Command = 293,                  /* Command  */
  YYSYMBOL_FlowCommands = 294,             /* FlowCommands  */
  YYSYMBOL_MouseCommands = 295,            /* MouseCommands  */
  YYSYMBOL_MCommandName = 296,             /* MCommandName  */
  YYSYMBOL_ModKeyList = 297,               /* ModKeyList  */
  YYSYMBOL_ModKey = 298,                   /* ModKey  */
  YYSYMBOL_Button = 299,                   /* Button  */
  YYSYMBOL_Count = 300,                    /* Count  */
  YYSYMBOL_KeyCommands = 301,              /* KeyCommands  */
  YYSYMBOL_KCommandName = 302,             /* KCommandName  */
  YYSYMBOL_Key = 303,                      /* Key  */
  YYSYMBOL_PerformanceCommands = 304,      /* PerformanceCommands  */
  YYSYMBOL_PerfCommandName = 305,          /* PerfCommandName  */
  YYSYMBOL_PerformSwitch = 306,            /* PerformSwitch  */
  YYSYMBOL_AbsCommands = 307,              /* AbsCommands  */
  YYSYMBOL_AbsCommandName = 308,           /* AbsCommandName  */
  YYSYMBOL_Coordinates = 309,              /* Coordinates  */
  YYSYMBOL_XCoordinate = 310,              /* XCoordinate  */
  YYSYMBOL_YCoordinate = 311,              /* YCoordinate  */
  YYSYMBOL_DragButton = 312,               /* DragButton  */
  YYSYMBOL_PointerRelCommands = 313,       /* PointerRelCommands  */
  YYSYMBOL_PRelCommandName = 314,          /* PRelCommandName  */
  YYSYMBOL_WidgetName = 315,               /* WidgetName  */
  YYSYMBOL_Component = 316,                /* Component  */
  YYSYMBOL_WidgetComponents = 317,         /* WidgetComponents  */
  YYSYMBOL_PointerCommands = 318,          /* PointerCommands  */
  YYSYMBOL_PCommandName = 319,             /* PCommandName  */
  YYSYMBOL_Location = 320,                 /* Location  */
  YYSYMBOL_SystemCommands = 321,           /* SystemCommands  */
  YYSYMBOL_SCommandName = 322,             /* SCommandName  */
  YYSYMBOL_SCommand = 323,                 /* SCommand  */
  YYSYMBOL_SCommandArgs = 324,             /* SCommandArgs  */
  YYSYMBOL_SCommandArg = 325,              /* SCommandArg  */
  YYSYMBOL_StoreCommands = 326,            /* StoreCommands  */
  YYSYMBOL_StoreCommandName = 327,         /* StoreCommandName  */
  YYSYMBOL_WidgetName1 = 328,              /* WidgetName1  */
  YYSYMBOL_Identifier = 329,               /* Identifier  */
  YYSYMBOL_CompareCommands = 330,          /* CompareCommands  */
  YYSYMBOL_CCommandName = 331,             /* CCommandName  */
  YYSYMBOL_DragCommands = 332,             /* DragCommands  */
  YYSYMBOL_DCommandName = 333,             /* DCommandName  */
  YYSYMBOL_DragList = 334,                 /* DragList  */
  YYSYMBOL_DragRelativeList = 335,         /* DragRelativeList  */
  YYSYMBOL_Direction = 336,                /* Direction  */
  YYSYMBOL_Xdirection = 337,               /* Xdirection  */
  YYSYMBOL_Ydirection = 338,               /* Ydirection  */
  YYSYMBOL_DragComponentList = 339,        /* DragComponentList  */
  YYSYMBOL_InputCommands = 340,            /* InputCommands  */
  YYSYMBOL_InputCommandName = 341,         /* InputCommandName  */
  YYSYMBOL_InputStringData = 342,          /* InputStringData  */
  YYSYMBOL_WindowCommands = 343,           /* WindowCommands  */
  YYSYMBOL_WCommandName = 344,             /* WCommandName  */
  YYSYMBOL_WindowCheckCommands = 345,      /* WindowCheckCommands  */
  YYSYMBOL_WCheckCommandName = 346,        /* WCheckCommandName  */
  YYSYMBOL_Wname = 347,                    /* Wname  */
  YYSYMBOL_Iname = 348,                    /* Iname  */
  YYSYMBOL_Group = 349,                    /* Group  */
  YYSYMBOL_WindowSelectCommands = 350,     /* WindowSelectCommands  */
  YYSYMBOL_WSelectCommandName = 351,       /* WSelectCommandName  */
  YYSYMBOL_Name = 352,                     /* Name  */
  YYSYMBOL_ItemNumber = 353,               /* ItemNumber  */
  YYSYMBOL_Keyboard = 354,                 /* Keyboard  */
  YYSYMBOL_WindowPostCommands = 355,       /* WindowPostCommands  */
  YYSYMBOL_PostCommandName = 356,          /* PostCommandName  */
  YYSYMBOL_ResizeCommands = 357,           /* ResizeCommands  */
  YYSYMBOL_RCommandName = 358,             /* RCommandName  */
  YYSYMBOL_Gravity = 359,                  /* Gravity  */
  YYSYMBOL_Width = 360,                    /* Width  */
  YYSYMBOL_Height = 361,                   /* Height  */
  YYSYMBOL_MoveCommands = 362,             /* MoveCommands  */
  YYSYMBOL_MoveCommandName = 363,          /* MoveCommandName  */
  YYSYMBOL_WaitCommands = 364,             /* WaitCommands  */
  YYSYMBOL_WaitCommandName = 365           /* WaitCommandName  */
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
#define YYLAST   455

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  291
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  75
/* YYNRULES -- Number of rules.  */
#define YYNRULES  483
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  413

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   545


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   361,   361,   362,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   377,   378,   379,
     380,   381,   382,   387,   388,   389,   394,   405,   406,   407,
     411,   412,   415,   417,   419,   421,   423,   425,   427,   429,
     431,   433,   437,   438,   439,   440,   441,   444,   445,   453,
     464,   465,   466,   469,   470,   471,   472,   473,   474,   475,
     476,   477,   478,   479,   480,   481,   482,   483,   484,   485,
     486,   487,   488,   489,   490,   491,   492,   493,   494,   495,
     496,   497,   498,   499,   500,   501,   502,   503,   504,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   525,
     526,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   555,
     556,   557,   558,   559,   560,   561,   562,   563,   564,   565,
     566,   567,   568,   569,   570,   577,   586,   589,   590,   598,
     608,   610,   613,   616,   619,   621,   622,   623,   624,   625,
     626,   634,   643,   647,   650,   651,   654,   655,   656,   657,
     658,   659,   660,   661,   662,   663,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,   677,
     678,   679,   680,   681,   682,   683,   684,   685,   686,   687,
     688,   689,   690,   691,   692,   693,   694,   695,   696,   697,
     698,   699,   700,   701,   702,   703,   704,   705,   706,   707,
     708,   709,   710,   711,   712,   713,   714,   715,   716,   717,
     718,   719,   720,   721,   723,   725,   727,   729,   731,   733,
     735,   737,   739,   741,   743,   745,   747,   749,   751,   753,
     755,   757,   759,   761,   763,   765,   767,   769,   771,   773,
     775,   777,   779,   781,   783,   785,   787,   789,   793,   796,
     799,   807,   816,   819,   822,   823,   826,   827,   828,   829,
     830,   831,   832,   833,   834,   835,   836,   837,   838,   839,
     840,   841,   842,   843,   844,   845,   846,   847,   848,   849,
     850,   851,   852,   853,   854,   855,   856,   857,   858,   859,
     860,   861,   862,   863,   864,   865,   866,   867,   868,   869,
     870,   871,   872,   873,   874,   875,   876,   877,   878,   879,
     880,   881,   882,   883,   884,   885,   886,   887,   888,   889,
     890,   891,   892,   893,   895,   897,   899,   901,   903,   905,
     907,   909,   911,   913,   915,   917,   919,   921,   923,   925,
     927,   929,   931,   933,   935,   937,   939,   941,   943,   945,
     947,   949,   951,   953,   955,   957,   959,   961,   965,   966,
     967,   968,   969,   970,   971,   972,   973,   974,   982,   992,
     995,  1002,  1003,  1006,  1008,  1022,  1031,  1033,  1037,  1043,
    1050,  1058,  1067,  1084,  1085,  1086,  1089,  1092,  1097,  1100,
    1101,  1102,  1103,  1104,  1105,  1106,  1109,  1110,  1111,  1114,
    1115,  1118,  1125,  1132,  1135,  1142,  1149,  1150,  1151,  1153,
    1154,  1155,  1156,  1157,  1158,  1166,  1175,  1178,  1185,  1189,
    1190,  1191,  1200,  1209,  1211,  1215,  1221,  1222,  1225,  1226,
    1229,  1230,  1236,  1243,  1244,  1251,  1258,  1261,  1262,  1263,
    1264,  1265,  1266,  1267,  1268,  1269,  1272,  1275,  1276,  1283,
    1289,  1290,  1297,  1303
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
  "\"end of file\"", "error", "\"invalid token\"", "STRING", "DIGIT",
  "ISTRING", "CONTINUE", "END", "MANUAL", "WAIT", "PRESSMB", "RELEASEMB",
  "CLICKMB", "SHIFTMASK", "LOCKMASK", "CTRLMASK", "METAMASK", "ALTMASK",
  "MOD1MASK", "MOD2MASK", "MOD3MASK", "MOD4MASK", "MOD5MASK", "BTN1",
  "BTN2", "BTN3", "BTN4", "BTN5", "CLICKKEY", "PRESSKEY", "RELEASEKEY",
  "BACKSPACE", "INSERT", "DELETE", "COPY", "CUT", "PASTE", "ADDMODE",
  "PRIMARYPASTE", "QUICKPASTE", "PAGEUP", "PAGEDOWN", "ENDLINE",
  "BEGINLINE", "ACTIVATE", "MENUBAR", "CLEAR", "CANCEL", "HELP", "MENU",
  "SELECT", "UNDO", "UPK", "RIGHTK", "DOWNK", "LEFTK", "AUTOMAX",
  "AUTOMIN", "BACKTAB", "BEGINDATA", "DESELECTALL", "ENDDATA", "ENTER",
  "ESCAPE", "EXTEND", "NEXTFIELD", "NEXTMENU", "PAGERIGHT", "PAGELEFT",
  "PREVFIELD", "PREVMENU", "PRIMARYCOPY", "PRIMARYCUT", "QUICKCOPY",
  "QUICKCUT", "QUICKEXTEND", "RESELECT", "RESTORE", "SELECTALL", "SPACE",
  "TAB", "KEYA", "KEYB", "KEYC", "KEYD", "KEYE", "KEYF", "KEYG", "KEYH",
  "KEYI", "KEYJ", "KEYK", "KEYL", "KEYM", "KEYN", "KEYO", "KEYP", "KEYQ",
  "KEYR", "KEYS", "KEYT", "KEYU", "KEYV", "KEYW", "KEYX", "KEYY", "KEYZ",
  "KEYSLASH", "KEYBACKSLASH", "KEYBRACERIGHT", "KEYBRACELEFT",
  "KEYBRACKETRIGHT", "KEYBRACKETLEFT", "KEYRETURN", "KEY0", "KEY1", "KEY2",
  "KEY3", "KEY4", "KEY5", "KEY6", "KEY7", "KEY8", "KEY9", "KEYEXCLAM",
  "KEYAT", "KEYNUMBERSIGN", "KEYDOLLAR", "KEYPERCENT", "KEYCARET",
  "KEYAMPERSAND", "KEYASTERISK", "KEYPARENLEFT", "KEYPARENRIGHT",
  "KEYNONE", "SEPARATOR", "LABEL", "LISTITEM", "FILELISTITEM",
  "DIRLISTITEM", "DIRFILTERLABEL", "DIRFILTERTEXT", "DIRFILTERTEXTCHAR",
  "SELECTIONLISTITEM", "TEXTCHAR", "CSTEXTCHAR", "SLIDER",
  "COMBOBOXDROPDOWNBUTTON", "OUTLINEBTN", "INCREMENTBTN", "DECREMENTBTN",
  "PAGESCROLLER", "MAJORTABSCROLLERNEXT", "MAJORTABSCROLLERPREV",
  "MINORTABSCROLLERNEXT", "MINORTABSCROLLERPREV", "UPORLEFTARROW",
  "DOWNORRIGHTARROW", "UPORLEFTSLIDERAREA", "DOWNORRIGHTSLIDERAREA",
  "SCROLLBAR", "HORIZSCROLLBAR", "HORIZSBLEFTARROW", "HORIZSBRIGHTARROW",
  "HORIZSBLEFTSLIDERAREA", "HORIZSBRIGHTSLIDERAREA", "HORIZSBSLIDER",
  "VERTSCROLLBAR", "VERTSBUPARROW", "VERTSBDOWNARROW",
  "VERTSBUPSLIDERAREA", "VERTSBDOWNSLIDERAREA", "VERTSBSLIDER", "SASH",
  "CLOSEBUTTON", "HELPBUTTON", "OKBUTTON", "FILTERBUTTON", "CANCELBUTTON",
  "APPLYBUTTON", "MESSAGELABEL", "SYMBOLLABEL", "SELECTIONTEXT",
  "SELECTIONTEXTCHAR", "SELECTIONLABEL", "SELECTIONLIST",
  "SELECTIONLISTLABEL", "FILELIST", "FILELISTLABEL", "DIRLIST",
  "DIRLISTLABEL", "FILESELECTIONTEXT", "FILESELECTIONTEXTCHAR",
  "FILESELECTIONLABEL", "FILEFILTERTEXT", "FILEFILTERTEXTCHAR",
  "FILEFILTERLABEL", "WORKAREA", "COMMANDWINDOW", "MESSAGEWINDOW",
  "WORKWINDOW", "SCALE", "SCALELABEL", "OPTIONBUTTON", "SCROLLEDWINDOW",
  "SCROLLBARSLIDER", "SCROLLBARUPORLEFTARROW", "SCROLLBARDOWNORRIGHTARROW",
  "SCROLLBARUPORLEFTSLIDERAREA", "SCROLLBARDOWNORRIGHTSLIDERAREA",
  "FLHSBUPORLEFTARROW", "FLHSBDOWNORRIGHTARROW", "FLHSBUPORLEFTSLIDERAREA",
  "FLHSBDOWNORRIGHTSLIDERAREA", "FLHSBSLIDER", "FLVSBUPORLEFTARROW",
  "FLVSBDOWNORRIGHTARROW", "FLVSBUPORLEFTSLIDERAREA",
  "FLVSBDOWNORRIGHTSLIDERAREA", "FLVSBSLIDER", "DLHSBUPORLEFTARROW",
  "DLHSBDOWNORRIGHTARROW", "DLHSBUPORLEFTSLIDERAREA",
  "DLHSBDOWNORRIGHTSLIDERAREA", "DLHSBSLIDER", "DLVSBUPORLEFTARROW",
  "DLVSBDOWNORRIGHTARROW", "DLVSBUPORLEFTSLIDERAREA",
  "DLVSBDOWNORRIGHTSLIDERAREA", "DLVSBSLIDER", "SLHSBUPORLEFTARROW",
  "SLHSBDOWNORRIGHTARROW", "SLHSBUPORLEFTSLIDERAREA",
  "SLHSBDOWNORRIGHTSLIDERAREA", "SLHSBSLIDER", "SLVSBUPORLEFTARROW",
  "SLVSBDOWNORRIGHTARROW", "SLVSBUPORLEFTSLIDERAREA",
  "SLVSBDOWNORRIGHTSLIDERAREA", "SLVSBSLIDER", "TEAROFFBUTTON",
  "LOCATEPOINTER", "LOCATEPOINTERABS", "LOCATEPOINTERREL", "SYSTEM",
  "COMPAREVISUAL", "SETFOCUS", "INPUTSTRING", "STOREVISUAL",
  "COMPARESTOREDVISUAL", "DRAG", "DRAGRELATIVE", "DRAGSLIDERVAL",
  "DRAGABS", "WINDOWICONIFY", "WINDOWDEICONIFY", "WINDOWMENUPOST",
  "WINDOWMENUUNPOST", "WINDOWMENUSELECT", "WINDOWMAXIMIZE",
  "WINDOWNORMALIZE", "WINDOWRESIZE", "WINDOWMOVE", "WINDOWRAISE",
  "WINDOWFOCUS", "WINDOWCLICK", "WINDOWICHECK", "SYSTEMMENU", "ICONMENU",
  "WINDOWDECORATION", "ICONMENUPOST", "ICONMENUUNPOST", "ICONMENUSELECT",
  "ICONMOVE", "NORTHWEST", "NORTH", "NORTHEAST", "WEST", "EAST",
  "SOUTHWEST", "SOUTH", "SOUTHEAST", "KEYBOARD", "OFF", "RIGHT", "LEFT",
  "UP", "DOWN", "ANY", "AUTOPERFORMMONITOR", "ON", "$accept",
  "CommandList", "Command", "FlowCommands", "MouseCommands",
  "MCommandName", "ModKeyList", "ModKey", "Button", "Count", "KeyCommands",
  "KCommandName", "Key", "PerformanceCommands", "PerfCommandName",
  "PerformSwitch", "AbsCommands", "AbsCommandName", "Coordinates",
  "XCoordinate", "YCoordinate", "DragButton", "PointerRelCommands",
  "PRelCommandName", "WidgetName", "Component", "WidgetComponents",
  "PointerCommands", "PCommandName", "Location", "SystemCommands",
  "SCommandName", "SCommand", "SCommandArgs", "SCommandArg",
  "StoreCommands", "StoreCommandName", "WidgetName1", "Identifier",
  "CompareCommands", "CCommandName", "DragCommands", "DCommandName",
  "DragList", "DragRelativeList", "Direction", "Xdirection", "Ydirection",
  "DragComponentList", "InputCommands", "InputCommandName",
  "InputStringData", "WindowCommands", "WCommandName",
  "WindowCheckCommands", "WCheckCommandName", "Wname", "Iname", "Group",
  "WindowSelectCommands", "WSelectCommandName", "Name", "ItemNumber",
  "Keyboard", "WindowPostCommands", "PostCommandName", "ResizeCommands",
  "RCommandName", "Gravity", "Width", "Height", "MoveCommands",
  "MoveCommandName", "WaitCommands", "WaitCommandName", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-375)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -375,     1,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -265,  -375,    11,  -375,    14,  -375,    14,  -375,    20,  -375,
      32,  -375,    14,  -375,    -1,  -375,     0,  -375,    14,  -375,
      34,  -375,    32,  -375,    14,  -375,    14,  -375,    14,  -375,
      53,    27,   321,  -375,  -375,  -375,  -375,  -375,    56,  -375,
     -41,   -41,  -375,  -375,  -375,   -41,   -41,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,   -41,  -375,  -375,     2,  -375,  -375,
    -375,  -375,  -375,  -248,    58,  -220,  -191,    11,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,    53,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
      55,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,    11,  -375,    10,    23,    60,
    -375,    10,  -375,  -375,  -375,    61,  -375,  -375,  -375,    88,
    -375,    89,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,   106,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -220,  -375,   107,    55,    55,  -375,  -375,
    -375,  -375,  -375
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,    24,    23,    25,   483,    27,    28,    29,
      50,    51,    52,   282,   160,   172,   399,   411,   433,   406,
     407,   413,   414,   415,   161,   436,   437,   463,   438,   453,
     439,   440,   466,   480,   441,   442,   443,   446,   464,   444,
     454,   481,   156,     3,     4,     5,    30,     6,    30,    22,
       0,     9,     0,     8,     0,     7,     0,    10,     0,    12,
       0,    11,     0,    14,     0,    13,     0,    15,     0,    16,
       0,    17,     0,    18,     0,    19,     0,    20,     0,    21,
      47,     0,     0,   158,   157,   155,   163,    30,     0,   173,
     174,   174,   400,   401,   408,   174,   174,   424,   425,   419,
     420,   421,   422,   423,   174,   412,   416,     0,   417,   434,
     432,   435,   447,     0,   456,   460,   467,     0,    48,   482,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    31,    47,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   143,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,    49,
     165,   164,   162,   209,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   223,
     222,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,     0,   175,   388,   398,     0,
     410,   388,   426,   427,   428,   429,   449,   450,   451,     0,
     457,   458,   461,   462,   473,   468,   472,   471,   470,   475,
     469,   474,     0,   479,    26,   166,   167,   168,   169,   170,
     159,   171,   389,   396,   397,   390,   391,   392,   393,   394,
     395,   281,   403,   404,   402,   409,   405,    30,   430,    30,
     448,   445,   459,   460,   476,   477,   165,   165,   452,   478,
     465,   431,   418
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -375,  -375,  -375,  -375,  -375,  -375,   -48,  -375,  -375,   -72,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -114,  -375,
    -375,  -374,  -375,  -375,   -40,   109,  -375,  -375,  -375,  -259,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,    40,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -290,  -375,  -375,  -375,  -375,  -375,  -375,
    -375,  -375,  -375,  -375,  -375
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,    43,    44,    45,    46,    81,   135,   136,   119,
      47,    48,   239,    49,    50,    85,    51,    52,    87,    88,
     242,   380,    53,    54,    90,   345,   346,    55,    56,   391,
      57,    58,    93,   348,   394,    59,    60,    95,   396,    61,
      62,    63,    64,   105,   106,   107,   355,   399,   108,    65,
      66,   110,    67,    68,    69,    70,   113,   401,   359,    71,
      72,   361,   403,   363,    73,    74,    75,    76,   372,   405,
     410,    77,    78,    79,    80
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      82,     2,    89,   373,   243,   109,   352,     3,     4,     5,
       6,     7,     8,     9,   382,    86,    91,    89,    83,   356,
     357,   358,    96,    92,   104,    84,   392,   393,   111,    10,
      11,    12,   411,   412,   115,    94,   116,   112,   117,   240,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,    97,    98,   118,   353,   354,
     241,   360,   362,   395,   374,   398,   383,   384,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   375,   376,
     377,   378,   379,   364,   365,   366,   367,   368,   369,   370,
     371,   400,   397,   402,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     404,   409,   114,   408,     0,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     347,     0,     0,     0,   349,   350,     0,     0,     0,     0,
       0,     0,     0,   351,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   381,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    13,    14,    15,    16,    17,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     0,     0,
       0,    38,    39,    40,    41,     0,     0,     0,     0,     0,
       0,     0,     0,    99,   100,   101,   102,   103,     0,     0,
      42,     0,     0,   385,   386,   387,   388,   389,   390,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,     0,     0,     0,     0,     0,   406,
       0,   407,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,     0,     0,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238
};

static const yytype_int16 yycheck[] =
{
      48,     0,     3,   117,    45,     5,     4,     6,     7,     8,
       9,    10,    11,    12,     4,     4,    56,     3,   283,   267,
     268,   269,    62,     3,    64,   290,     3,     4,    68,    28,
      29,    30,   406,   407,    74,     3,    76,     3,    78,    87,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    56,    57,     4,    56,    57,
       4,     3,   282,     3,   136,     4,    56,    57,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,   274,   275,   276,   277,   278,   279,   280,
     281,     3,   351,     4,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
       4,     4,    72,   403,    -1,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
      91,    -1,    -1,    -1,    95,    96,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   104,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   345,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   241,   242,   243,   244,   245,    -1,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,    -1,    -1,
      -1,   270,   271,   272,   273,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   284,   285,   286,   287,   288,    -1,    -1,
     289,    -1,    -1,   283,   284,   285,   286,   287,   288,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    -1,    -1,    -1,    -1,   397,
      -1,   399,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   292,     0,     6,     7,     8,     9,    10,    11,    12,
      28,    29,    30,   241,   242,   243,   244,   245,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   270,   271,
     272,   273,   289,   293,   294,   295,   296,   301,   302,   304,
     305,   307,   308,   313,   314,   318,   319,   321,   322,   326,
     327,   330,   331,   332,   333,   340,   341,   343,   344,   345,
     346,   350,   351,   355,   356,   357,   358,   362,   363,   364,
     365,   297,   297,   283,   290,   306,     4,   309,   310,     3,
     315,   315,     3,   323,     3,   328,   315,    56,    57,   284,
     285,   286,   287,   288,   315,   334,   335,   336,   339,     5,
     342,   315,     3,   347,   328,   315,   315,   315,     4,   300,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,   298,   299,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   303,
     297,     4,   311,    45,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   316,   317,   316,   324,   316,
     316,   316,     4,    56,    57,   337,   267,   268,   269,   349,
       3,   352,   282,   354,   274,   275,   276,   277,   278,   279,
     280,   281,   359,   309,   300,    23,    24,    25,    26,    27,
     312,   309,     4,    56,    57,   283,   284,   285,   286,   287,
     288,   320,     3,     4,   325,     3,   329,   320,     4,   338,
       3,   348,     4,   353,     4,   360,   297,   297,   354,     4,
     361,   312,   312
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   291,   292,   292,   293,   293,   293,   293,   293,   293,
     293,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     293,   293,   293,   294,   294,   294,   295,   296,   296,   296,
     297,   297,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   299,   299,   299,   299,   299,   300,   300,   301,
     302,   302,   302,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   304,   305,   306,   306,   307,
     308,   308,   309,   310,   311,   312,   312,   312,   312,   312,
     312,   313,   314,   315,   316,   316,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   309,   310,
     311,   318,   319,   315,   316,   316,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   320,   320,
     320,   320,   320,   320,   320,   320,   320,   320,   321,   322,
     323,   324,   324,   325,   325,   326,   327,   327,   328,   329,
     330,   331,   332,   333,   333,   333,   334,   334,   335,   336,
     336,   336,   336,   336,   336,   336,   337,   337,   337,   338,
     338,   339,   340,   341,   342,   343,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   345,   346,   347,   348,   349,
     349,   349,   350,   351,   351,   328,   352,   352,   353,   353,
     354,   354,   355,   356,   356,   357,   358,   359,   359,   359,
     359,   359,   359,   359,   359,   359,   360,   361,   361,   362,
     363,   363,   364,   365
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     1,     1,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     4,
       1,     1,     2,     1,     1,     0,     1,     1,     1,     1,
       1,     4,     1,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     4,     1,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     0,     2,     1,     1,     4,     1,     1,     1,     1,
       3,     1,     2,     1,     1,     1,     1,     1,     5,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     5,     2,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     1,     1,     1,     1,
       1,     1,     5,     1,     1,     1,     0,     1,     0,     1,
       0,     1,     3,     1,     1,     5,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     3,
       1,     1,     2,     1
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
  case 3: /* CommandList: CommandList Command  */
#line 362 "Automation.y"
                                                        { line_cnt++; }
#line 2523 "y.tab.c"
    break;

  case 23: /* FlowCommands: END  */
#line 387 "Automation.y"
                                                { BuildFlowCommand(END); }
#line 2529 "y.tab.c"
    break;

  case 24: /* FlowCommands: CONTINUE  */
#line 388 "Automation.y"
                                                { BuildFlowCommand(CONTINUE); }
#line 2535 "y.tab.c"
    break;

  case 25: /* FlowCommands: MANUAL  */
#line 389 "Automation.y"
                                                { BuildFlowCommand(MANUAL); }
#line 2541 "y.tab.c"
    break;

  case 26: /* MouseCommands: MCommandName ModKeyList Button Count  */
#line 395 "Automation.y"
                        {
				BuildMouseCommand(yytknval1,
						  yymodmaskval,
						  yymaskcnt,
						  yytknval3,
						  yydigitval);
				yymaskcnt = 0;
				yydigitval = 0;
			}
#line 2555 "y.tab.c"
    break;

  case 27: /* MCommandName: PRESSMB  */
#line 405 "Automation.y"
                                                { yytknval1 = PRESSMB; }
#line 2561 "y.tab.c"
    break;

  case 28: /* MCommandName: RELEASEMB  */
#line 406 "Automation.y"
                                                { yytknval1 = RELEASEMB; }
#line 2567 "y.tab.c"
    break;

  case 29: /* MCommandName: CLICKMB  */
#line 407 "Automation.y"
                                                { yytknval1 = CLICKMB; }
#line 2573 "y.tab.c"
    break;

  case 30: /* ModKeyList: %empty  */
#line 411 "Automation.y"
                                                { yymaskcnt = 0; }
#line 2579 "y.tab.c"
    break;

  case 32: /* ModKey: SHIFTMASK  */
#line 415 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= SHIFTMASK; }
#line 2586 "y.tab.c"
    break;

  case 33: /* ModKey: LOCKMASK  */
#line 417 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= LOCKMASK; }
#line 2593 "y.tab.c"
    break;

  case 34: /* ModKey: CTRLMASK  */
#line 419 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= CTRLMASK; }
#line 2600 "y.tab.c"
    break;

  case 35: /* ModKey: METAMASK  */
#line 421 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= METAMASK; }
#line 2607 "y.tab.c"
    break;

  case 36: /* ModKey: ALTMASK  */
#line 423 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= ALTMASK; }
#line 2614 "y.tab.c"
    break;

  case 37: /* ModKey: MOD1MASK  */
#line 425 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= MOD1MASK; }
#line 2621 "y.tab.c"
    break;

  case 38: /* ModKey: MOD2MASK  */
#line 427 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= MOD2MASK; }
#line 2628 "y.tab.c"
    break;

  case 39: /* ModKey: MOD3MASK  */
#line 429 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= MOD3MASK; }
#line 2635 "y.tab.c"
    break;

  case 40: /* ModKey: MOD4MASK  */
#line 431 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= MOD4MASK; }
#line 2642 "y.tab.c"
    break;

  case 41: /* ModKey: MOD5MASK  */
#line 433 "Automation.y"
                                                { yymodmaskval[yymaskcnt++]
								= MOD5MASK; }
#line 2649 "y.tab.c"
    break;

  case 42: /* Button: BTN1  */
#line 437 "Automation.y"
                                                { yytknval3 = BTN1; }
#line 2655 "y.tab.c"
    break;

  case 43: /* Button: BTN2  */
#line 438 "Automation.y"
                                                { yytknval3 = BTN2; }
#line 2661 "y.tab.c"
    break;

  case 44: /* Button: BTN3  */
#line 439 "Automation.y"
                                                { yytknval3 = BTN3; }
#line 2667 "y.tab.c"
    break;

  case 45: /* Button: BTN4  */
#line 440 "Automation.y"
                                                { yytknval3 = BTN4; }
#line 2673 "y.tab.c"
    break;

  case 46: /* Button: BTN5  */
#line 441 "Automation.y"
                                                { yytknval3 = BTN5; }
#line 2679 "y.tab.c"
    break;

  case 47: /* Count: %empty  */
#line 444 "Automation.y"
                                                { yydigitval = 0; }
#line 2685 "y.tab.c"
    break;

  case 49: /* KeyCommands: KCommandName ModKeyList Key  */
#line 455 "Automation.y"
                        {
				BuildKeyCommand(yytknval1,
						yymodmaskval,
						yymaskcnt,
						yytknval3);
				yymaskcnt = 0;
				yydigitval = 0;
			}
#line 2698 "y.tab.c"
    break;

  case 50: /* KCommandName: CLICKKEY  */
#line 464 "Automation.y"
                                                { yytknval1 = CLICKKEY; }
#line 2704 "y.tab.c"
    break;

  case 51: /* KCommandName: PRESSKEY  */
#line 465 "Automation.y"
                                                { yytknval1 = PRESSKEY; }
#line 2710 "y.tab.c"
    break;

  case 52: /* KCommandName: RELEASEKEY  */
#line 466 "Automation.y"
                                                { yytknval1 = RELEASEKEY; }
#line 2716 "y.tab.c"
    break;

  case 53: /* Key: BACKSPACE  */
#line 469 "Automation.y"
                                                { yytknval3 = BACKSPACE; }
#line 2722 "y.tab.c"
    break;

  case 54: /* Key: INSERT  */
#line 470 "Automation.y"
                                                { yytknval3 = INSERT; }
#line 2728 "y.tab.c"
    break;

  case 55: /* Key: DELETE  */
#line 471 "Automation.y"
                                                { yytknval3 = DELETE; }
#line 2734 "y.tab.c"
    break;

  case 56: /* Key: COPY  */
#line 472 "Automation.y"
                                                { yytknval3 = COPY; }
#line 2740 "y.tab.c"
    break;

  case 57: /* Key: CUT  */
#line 473 "Automation.y"
                                                { yytknval3 = CUT; }
#line 2746 "y.tab.c"
    break;

  case 58: /* Key: PASTE  */
#line 474 "Automation.y"
                                                { yytknval3 = PASTE; }
#line 2752 "y.tab.c"
    break;

  case 59: /* Key: ADDMODE  */
#line 475 "Automation.y"
                                                { yytknval3 = ADDMODE; }
#line 2758 "y.tab.c"
    break;

  case 60: /* Key: PRIMARYPASTE  */
#line 476 "Automation.y"
                                                { yytknval3 = PRIMARYPASTE; }
#line 2764 "y.tab.c"
    break;

  case 61: /* Key: QUICKPASTE  */
#line 477 "Automation.y"
                                                { yytknval3 = QUICKPASTE; }
#line 2770 "y.tab.c"
    break;

  case 62: /* Key: PAGEUP  */
#line 478 "Automation.y"
                                                { yytknval3 = PAGEUP; }
#line 2776 "y.tab.c"
    break;

  case 63: /* Key: PAGEDOWN  */
#line 479 "Automation.y"
                                                { yytknval3 = PAGEDOWN; }
#line 2782 "y.tab.c"
    break;

  case 64: /* Key: ENDLINE  */
#line 480 "Automation.y"
                                                { yytknval3 = ENDLINE; }
#line 2788 "y.tab.c"
    break;

  case 65: /* Key: BEGINLINE  */
#line 481 "Automation.y"
                                                { yytknval3 = BEGINLINE; }
#line 2794 "y.tab.c"
    break;

  case 66: /* Key: ACTIVATE  */
#line 482 "Automation.y"
                                                { yytknval3 = ACTIVATE; }
#line 2800 "y.tab.c"
    break;

  case 67: /* Key: MENUBAR  */
#line 483 "Automation.y"
                                                { yytknval3 = MENUBAR; }
#line 2806 "y.tab.c"
    break;

  case 68: /* Key: CLEAR  */
#line 484 "Automation.y"
                                                { yytknval3 = CLEAR; }
#line 2812 "y.tab.c"
    break;

  case 69: /* Key: CANCEL  */
#line 485 "Automation.y"
                                                { yytknval3 = CANCEL; }
#line 2818 "y.tab.c"
    break;

  case 70: /* Key: HELP  */
#line 486 "Automation.y"
                                                { yytknval3 = HELP; }
#line 2824 "y.tab.c"
    break;

  case 71: /* Key: MENU  */
#line 487 "Automation.y"
                                                { yytknval3 = MENU; }
#line 2830 "y.tab.c"
    break;

  case 72: /* Key: SELECT  */
#line 488 "Automation.y"
                                                { yytknval3 = SELECT; }
#line 2836 "y.tab.c"
    break;

  case 73: /* Key: UNDO  */
#line 489 "Automation.y"
                                                { yytknval3 = UNDO; }
#line 2842 "y.tab.c"
    break;

  case 74: /* Key: UPK  */
#line 490 "Automation.y"
                                                { yytknval3 = UPK; }
#line 2848 "y.tab.c"
    break;

  case 75: /* Key: RIGHTK  */
#line 491 "Automation.y"
                                                { yytknval3 = RIGHTK; }
#line 2854 "y.tab.c"
    break;

  case 76: /* Key: DOWNK  */
#line 492 "Automation.y"
                                                { yytknval3 = DOWNK; }
#line 2860 "y.tab.c"
    break;

  case 77: /* Key: LEFTK  */
#line 493 "Automation.y"
                                                { yytknval3 = LEFTK; }
#line 2866 "y.tab.c"
    break;

  case 78: /* Key: BACKTAB  */
#line 494 "Automation.y"
                                                { yytknval3 = BACKTAB; }
#line 2872 "y.tab.c"
    break;

  case 79: /* Key: BEGINDATA  */
#line 495 "Automation.y"
                                                { yytknval3 = BEGINDATA; }
#line 2878 "y.tab.c"
    break;

  case 80: /* Key: DESELECTALL  */
#line 496 "Automation.y"
                                                { yytknval3 = DESELECTALL; }
#line 2884 "y.tab.c"
    break;

  case 81: /* Key: ENDDATA  */
#line 497 "Automation.y"
                                                { yytknval3 = ENDDATA; }
#line 2890 "y.tab.c"
    break;

  case 82: /* Key: ENTER  */
#line 498 "Automation.y"
                                                { yytknval3 = ENTER; }
#line 2896 "y.tab.c"
    break;

  case 83: /* Key: ESCAPE  */
#line 499 "Automation.y"
                                                { yytknval3 = ESCAPE; }
#line 2902 "y.tab.c"
    break;

  case 84: /* Key: EXTEND  */
#line 500 "Automation.y"
                                                { yytknval3 = EXTEND; }
#line 2908 "y.tab.c"
    break;

  case 85: /* Key: NEXTFIELD  */
#line 501 "Automation.y"
                                                { yytknval3 = NEXTFIELD; }
#line 2914 "y.tab.c"
    break;

  case 86: /* Key: NEXTMENU  */
#line 502 "Automation.y"
                                                { yytknval3 = NEXTMENU; }
#line 2920 "y.tab.c"
    break;

  case 87: /* Key: PAGERIGHT  */
#line 503 "Automation.y"
                                                { yytknval3 = PAGERIGHT; }
#line 2926 "y.tab.c"
    break;

  case 88: /* Key: PAGELEFT  */
#line 504 "Automation.y"
                                                { yytknval3 = PAGELEFT; }
#line 2932 "y.tab.c"
    break;

  case 89: /* Key: PREVFIELD  */
#line 505 "Automation.y"
                                                { yytknval3 = PREVFIELD; }
#line 2938 "y.tab.c"
    break;

  case 90: /* Key: PREVMENU  */
#line 506 "Automation.y"
                                                { yytknval3 = PREVMENU; }
#line 2944 "y.tab.c"
    break;

  case 91: /* Key: PRIMARYCOPY  */
#line 507 "Automation.y"
                                                { yytknval3 = PRIMARYCOPY; }
#line 2950 "y.tab.c"
    break;

  case 92: /* Key: PRIMARYCUT  */
#line 508 "Automation.y"
                                                { yytknval3 = PRIMARYCUT; }
#line 2956 "y.tab.c"
    break;

  case 93: /* Key: QUICKCOPY  */
#line 509 "Automation.y"
                                                { yytknval3 = QUICKCOPY; }
#line 2962 "y.tab.c"
    break;

  case 94: /* Key: QUICKCUT  */
#line 510 "Automation.y"
                                                { yytknval3 = QUICKCUT; }
#line 2968 "y.tab.c"
    break;

  case 95: /* Key: QUICKEXTEND  */
#line 511 "Automation.y"
                                                { yytknval3 = QUICKEXTEND; }
#line 2974 "y.tab.c"
    break;

  case 96: /* Key: RESELECT  */
#line 512 "Automation.y"
                                                { yytknval3 = RESELECT; }
#line 2980 "y.tab.c"
    break;

  case 97: /* Key: RESTORE  */
#line 513 "Automation.y"
                                                { yytknval3 = RESTORE; }
#line 2986 "y.tab.c"
    break;

  case 98: /* Key: SELECTALL  */
#line 514 "Automation.y"
                                                { yytknval3 = SELECTALL; }
#line 2992 "y.tab.c"
    break;

  case 99: /* Key: SPACE  */
#line 515 "Automation.y"
                                                { yytknval3 = SPACE; }
#line 2998 "y.tab.c"
    break;

  case 100: /* Key: TAB  */
#line 516 "Automation.y"
                                                { yytknval3 = TAB; }
#line 3004 "y.tab.c"
    break;

  case 101: /* Key: KEYA  */
#line 517 "Automation.y"
                                                { yytknval3 = KEYA; }
#line 3010 "y.tab.c"
    break;

  case 102: /* Key: KEYB  */
#line 518 "Automation.y"
                                                { yytknval3 = KEYB; }
#line 3016 "y.tab.c"
    break;

  case 103: /* Key: KEYC  */
#line 519 "Automation.y"
                                                { yytknval3 = KEYC; }
#line 3022 "y.tab.c"
    break;

  case 104: /* Key: KEYD  */
#line 520 "Automation.y"
                                                { yytknval3 = KEYD; }
#line 3028 "y.tab.c"
    break;

  case 105: /* Key: KEYE  */
#line 521 "Automation.y"
                                                { yytknval3 = KEYE; }
#line 3034 "y.tab.c"
    break;

  case 106: /* Key: KEYF  */
#line 522 "Automation.y"
                                                { yytknval3 = KEYF; }
#line 3040 "y.tab.c"
    break;

  case 107: /* Key: KEYG  */
#line 523 "Automation.y"
                                                { yytknval3 = KEYG; }
#line 3046 "y.tab.c"
    break;

  case 108: /* Key: KEYH  */
#line 524 "Automation.y"
                                                { yytknval3 = KEYH; }
#line 3052 "y.tab.c"
    break;

  case 109: /* Key: KEYI  */
#line 525 "Automation.y"
                                                { yytknval3 = KEYI; }
#line 3058 "y.tab.c"
    break;

  case 110: /* Key: KEYJ  */
#line 526 "Automation.y"
                                                { yytknval3 = KEYJ; }
#line 3064 "y.tab.c"
    break;

  case 111: /* Key: KEYK  */
#line 527 "Automation.y"
                                                { yytknval3 = KEYK; }
#line 3070 "y.tab.c"
    break;

  case 112: /* Key: KEYL  */
#line 528 "Automation.y"
                                                { yytknval3 = KEYL; }
#line 3076 "y.tab.c"
    break;

  case 113: /* Key: KEYM  */
#line 529 "Automation.y"
                                                { yytknval3 = KEYM; }
#line 3082 "y.tab.c"
    break;

  case 114: /* Key: KEYN  */
#line 530 "Automation.y"
                                                { yytknval3 = KEYN; }
#line 3088 "y.tab.c"
    break;

  case 115: /* Key: KEYO  */
#line 531 "Automation.y"
                                                { yytknval3 = KEYO; }
#line 3094 "y.tab.c"
    break;

  case 116: /* Key: KEYP  */
#line 532 "Automation.y"
                                                { yytknval3 = KEYP; }
#line 3100 "y.tab.c"
    break;

  case 117: /* Key: KEYQ  */
#line 533 "Automation.y"
                                                { yytknval3 = KEYQ; }
#line 3106 "y.tab.c"
    break;

  case 118: /* Key: KEYR  */
#line 534 "Automation.y"
                                                { yytknval3 = KEYR; }
#line 3112 "y.tab.c"
    break;

  case 119: /* Key: KEYS  */
#line 535 "Automation.y"
                                                { yytknval3 = KEYS; }
#line 3118 "y.tab.c"
    break;

  case 120: /* Key: KEYT  */
#line 536 "Automation.y"
                                                { yytknval3 = KEYT; }
#line 3124 "y.tab.c"
    break;

  case 121: /* Key: KEYU  */
#line 537 "Automation.y"
                                                { yytknval3 = KEYU; }
#line 3130 "y.tab.c"
    break;

  case 122: /* Key: KEYV  */
#line 538 "Automation.y"
                                                { yytknval3 = KEYV; }
#line 3136 "y.tab.c"
    break;

  case 123: /* Key: KEYW  */
#line 539 "Automation.y"
                                                { yytknval3 = KEYW; }
#line 3142 "y.tab.c"
    break;

  case 124: /* Key: KEYX  */
#line 540 "Automation.y"
                                                { yytknval3 = KEYX; }
#line 3148 "y.tab.c"
    break;

  case 125: /* Key: KEYY  */
#line 541 "Automation.y"
                                                { yytknval3 = KEYY; }
#line 3154 "y.tab.c"
    break;

  case 126: /* Key: KEYZ  */
#line 542 "Automation.y"
                                                { yytknval3 = KEYZ; }
#line 3160 "y.tab.c"
    break;

  case 127: /* Key: KEYSLASH  */
#line 543 "Automation.y"
                                                { yytknval3 = KEYSLASH; }
#line 3166 "y.tab.c"
    break;

  case 128: /* Key: KEYBACKSLASH  */
#line 544 "Automation.y"
                                                { yytknval3 = KEYBACKSLASH; }
#line 3172 "y.tab.c"
    break;

  case 129: /* Key: KEYBRACERIGHT  */
#line 545 "Automation.y"
                                                { yytknval3 = KEYBRACERIGHT; }
#line 3178 "y.tab.c"
    break;

  case 130: /* Key: KEYBRACELEFT  */
#line 546 "Automation.y"
                                                { yytknval3 = KEYBRACELEFT; }
#line 3184 "y.tab.c"
    break;

  case 131: /* Key: KEYBRACKETRIGHT  */
#line 547 "Automation.y"
                                                { yytknval3 = KEYBRACKETRIGHT; }
#line 3190 "y.tab.c"
    break;

  case 132: /* Key: KEYBRACKETLEFT  */
#line 548 "Automation.y"
                                                { yytknval3 = KEYBRACKETLEFT; }
#line 3196 "y.tab.c"
    break;

  case 133: /* Key: KEYRETURN  */
#line 549 "Automation.y"
                                                { yytknval3 = KEYRETURN; }
#line 3202 "y.tab.c"
    break;

  case 134: /* Key: KEY1  */
#line 550 "Automation.y"
                                                { yytknval3 = KEY1; }
#line 3208 "y.tab.c"
    break;

  case 135: /* Key: KEY2  */
#line 551 "Automation.y"
                                                { yytknval3 = KEY2; }
#line 3214 "y.tab.c"
    break;

  case 136: /* Key: KEY3  */
#line 552 "Automation.y"
                                                { yytknval3 = KEY3; }
#line 3220 "y.tab.c"
    break;

  case 137: /* Key: KEY4  */
#line 553 "Automation.y"
                                                { yytknval3 = KEY4; }
#line 3226 "y.tab.c"
    break;

  case 138: /* Key: KEY5  */
#line 554 "Automation.y"
                                                { yytknval3 = KEY5; }
#line 3232 "y.tab.c"
    break;

  case 139: /* Key: KEY6  */
#line 555 "Automation.y"
                                                { yytknval3 = KEY6; }
#line 3238 "y.tab.c"
    break;

  case 140: /* Key: KEY7  */
#line 556 "Automation.y"
                                                { yytknval3 = KEY7; }
#line 3244 "y.tab.c"
    break;

  case 141: /* Key: KEY8  */
#line 557 "Automation.y"
                                                { yytknval3 = KEY8; }
#line 3250 "y.tab.c"
    break;

  case 142: /* Key: KEY9  */
#line 558 "Automation.y"
                                                { yytknval3 = KEY9; }
#line 3256 "y.tab.c"
    break;

  case 143: /* Key: KEY0  */
#line 559 "Automation.y"
                                                { yytknval3 = KEY0; }
#line 3262 "y.tab.c"
    break;

  case 144: /* Key: KEYEXCLAM  */
#line 560 "Automation.y"
                                                { yytknval3 = KEYEXCLAM; }
#line 3268 "y.tab.c"
    break;

  case 145: /* Key: KEYAT  */
#line 561 "Automation.y"
                                                { yytknval3 = KEYAT; }
#line 3274 "y.tab.c"
    break;

  case 146: /* Key: KEYNUMBERSIGN  */
#line 562 "Automation.y"
                                                { yytknval3 = KEYNUMBERSIGN; }
#line 3280 "y.tab.c"
    break;

  case 147: /* Key: KEYDOLLAR  */
#line 563 "Automation.y"
                                                { yytknval3 = KEYDOLLAR; }
#line 3286 "y.tab.c"
    break;

  case 148: /* Key: KEYPERCENT  */
#line 564 "Automation.y"
                                                { yytknval3 = KEYPERCENT; }
#line 3292 "y.tab.c"
    break;

  case 149: /* Key: KEYCARET  */
#line 565 "Automation.y"
                                                { yytknval3 = KEYCARET; }
#line 3298 "y.tab.c"
    break;

  case 150: /* Key: KEYAMPERSAND  */
#line 566 "Automation.y"
                                                { yytknval3 = KEYAMPERSAND; }
#line 3304 "y.tab.c"
    break;

  case 151: /* Key: KEYASTERISK  */
#line 567 "Automation.y"
                                                { yytknval3 = KEYASTERISK; }
#line 3310 "y.tab.c"
    break;

  case 152: /* Key: KEYPARENLEFT  */
#line 568 "Automation.y"
                                                { yytknval3 = KEYPARENLEFT; }
#line 3316 "y.tab.c"
    break;

  case 153: /* Key: KEYPARENRIGHT  */
#line 569 "Automation.y"
                                                { yytknval3 = KEYPARENRIGHT; }
#line 3322 "y.tab.c"
    break;

  case 154: /* Key: KEYNONE  */
#line 570 "Automation.y"
                                                { yytknval3 = KEYNONE; }
#line 3328 "y.tab.c"
    break;

  case 155: /* PerformanceCommands: PerfCommandName PerformSwitch  */
#line 579 "Automation.y"
                        {
                                BuildPerformCommand(yytknval1,
						    yytknval2);
				yymaskcnt = 0;
				yydigitval = 0;
			}
#line 3339 "y.tab.c"
    break;

  case 156: /* PerfCommandName: AUTOPERFORMMONITOR  */
#line 586 "Automation.y"
                                                    { yytknval1 = AUTOPERFORMMONITOR; }
#line 3345 "y.tab.c"
    break;

  case 157: /* PerformSwitch: ON  */
#line 589 "Automation.y"
                                                    { yytknval2 = ON ; }
#line 3351 "y.tab.c"
    break;

  case 158: /* PerformSwitch: OFF  */
#line 590 "Automation.y"
                                                    { yytknval2 = OFF; }
#line 3357 "y.tab.c"
    break;

  case 159: /* AbsCommands: AbsCommandName Coordinates ModKeyList DragButton  */
#line 599 "Automation.y"
                        {
				BuildAbsCommand(yytknval1,
					        yydigitval2,
					        yydigitval,
						yymodmaskval,
					        yymaskcnt,
						yytknval3);
			}
#line 3370 "y.tab.c"
    break;

  case 160: /* AbsCommandName: LOCATEPOINTERABS  */
#line 609 "Automation.y"
                                            { yytknval1 = LOCATEPOINTERABS; }
#line 3376 "y.tab.c"
    break;

  case 161: /* AbsCommandName: DRAGABS  */
#line 610 "Automation.y"
                                            { yytknval1 = DRAGABS; }
#line 3382 "y.tab.c"
    break;

  case 163: /* XCoordinate: DIGIT  */
#line 616 "Automation.y"
                                                { yydigitval2 = yydigitval; }
#line 3388 "y.tab.c"
    break;

  case 165: /* DragButton: %empty  */
#line 621 "Automation.y"
                                                { yytknval3 = 0; }
#line 3394 "y.tab.c"
    break;

  case 166: /* DragButton: BTN1  */
#line 622 "Automation.y"
                                                { yytknval3 = BTN1; }
#line 3400 "y.tab.c"
    break;

  case 167: /* DragButton: BTN2  */
#line 623 "Automation.y"
                                                { yytknval3 = BTN2; }
#line 3406 "y.tab.c"
    break;

  case 168: /* DragButton: BTN3  */
#line 624 "Automation.y"
                                                { yytknval3 = BTN3; }
#line 3412 "y.tab.c"
    break;

  case 169: /* DragButton: BTN4  */
#line 625 "Automation.y"
                                                { yytknval3 = BTN4; }
#line 3418 "y.tab.c"
    break;

  case 170: /* DragButton: BTN5  */
#line 626 "Automation.y"
                                                { yytknval3 = BTN5; }
#line 3424 "y.tab.c"
    break;

  case 171: /* PointerRelCommands: PRelCommandName WidgetName Component Coordinates  */
#line 635 "Automation.y"
                        {
				BuildRelCommand(yytknval1,
						yystringval,
						yytknval4,
						yydigitval2,
					        yydigitval);
			}
#line 3436 "y.tab.c"
    break;

  case 172: /* PRelCommandName: LOCATEPOINTERREL  */
#line 644 "Automation.y"
                                                { yytknval1 = LOCATEPOINTERREL; }
#line 3442 "y.tab.c"
    break;

  case 174: /* Component: %empty  */
#line 650 "Automation.y"
                                                { yytknval4 = -1; }
#line 3448 "y.tab.c"
    break;

  case 176: /* WidgetComponents: SEPARATOR  */
#line 654 "Automation.y"
                                                { yytknval4 = SEPARATOR; }
#line 3454 "y.tab.c"
    break;

  case 177: /* WidgetComponents: LABEL  */
#line 655 "Automation.y"
                                                { yytknval4 = LABEL; }
#line 3460 "y.tab.c"
    break;

  case 178: /* WidgetComponents: LISTITEM  */
#line 656 "Automation.y"
                                                { yytknval4 = LISTITEM; }
#line 3466 "y.tab.c"
    break;

  case 179: /* WidgetComponents: FILELISTITEM  */
#line 657 "Automation.y"
                                                { yytknval4 = FILELISTITEM; }
#line 3472 "y.tab.c"
    break;

  case 180: /* WidgetComponents: DIRLISTITEM  */
#line 658 "Automation.y"
                                                { yytknval4 = DIRLISTITEM; }
#line 3478 "y.tab.c"
    break;

  case 181: /* WidgetComponents: DIRFILTERLABEL  */
#line 659 "Automation.y"
                                                { yytknval4 = DIRFILTERLABEL; }
#line 3484 "y.tab.c"
    break;

  case 182: /* WidgetComponents: DIRFILTERTEXT  */
#line 660 "Automation.y"
                                                { yytknval4 = DIRFILTERTEXT; }
#line 3490 "y.tab.c"
    break;

  case 183: /* WidgetComponents: DIRFILTERTEXTCHAR  */
#line 661 "Automation.y"
                                                { yytknval4 = DIRFILTERTEXTCHAR; }
#line 3496 "y.tab.c"
    break;

  case 184: /* WidgetComponents: SELECTIONLISTITEM  */
#line 662 "Automation.y"
                                                { yytknval4 = SELECTIONLISTITEM; }
#line 3502 "y.tab.c"
    break;

  case 185: /* WidgetComponents: TEXTCHAR  */
#line 663 "Automation.y"
                                                { yytknval4 = TEXTCHAR; }
#line 3508 "y.tab.c"
    break;

  case 186: /* WidgetComponents: CSTEXTCHAR  */
#line 664 "Automation.y"
                                                { yytknval4 = CSTEXTCHAR; }
#line 3514 "y.tab.c"
    break;

  case 187: /* WidgetComponents: SLIDER  */
#line 665 "Automation.y"
                                                { yytknval4 = SLIDER; }
#line 3520 "y.tab.c"
    break;

  case 188: /* WidgetComponents: COMBOBOXDROPDOWNBUTTON  */
#line 666 "Automation.y"
                                                { yytknval4 = COMBOBOXDROPDOWNBUTTON;}
#line 3526 "y.tab.c"
    break;

  case 189: /* WidgetComponents: OUTLINEBTN  */
#line 667 "Automation.y"
                                                { yytknval4 = OUTLINEBTN;}
#line 3532 "y.tab.c"
    break;

  case 190: /* WidgetComponents: INCREMENTBTN  */
#line 668 "Automation.y"
                                                { yytknval4 = INCREMENTBTN;}
#line 3538 "y.tab.c"
    break;

  case 191: /* WidgetComponents: DECREMENTBTN  */
#line 669 "Automation.y"
                                                { yytknval4 = DECREMENTBTN;}
#line 3544 "y.tab.c"
    break;

  case 192: /* WidgetComponents: UPORLEFTARROW  */
#line 670 "Automation.y"
                                                { yytknval4 = UPORLEFTARROW; }
#line 3550 "y.tab.c"
    break;

  case 193: /* WidgetComponents: DOWNORRIGHTARROW  */
#line 671 "Automation.y"
                                                { yytknval4 = DOWNORRIGHTARROW;}
#line 3556 "y.tab.c"
    break;

  case 194: /* WidgetComponents: UPORLEFTSLIDERAREA  */
#line 672 "Automation.y"
                                                { yytknval4 = UPORLEFTSLIDERAREA;}
#line 3562 "y.tab.c"
    break;

  case 195: /* WidgetComponents: DOWNORRIGHTSLIDERAREA  */
#line 673 "Automation.y"
                                                { yytknval4 = DOWNORRIGHTSLIDERAREA;}
#line 3568 "y.tab.c"
    break;

  case 196: /* WidgetComponents: SCROLLBAR  */
#line 674 "Automation.y"
                                                { yytknval4 = SCROLLBAR; }
#line 3574 "y.tab.c"
    break;

  case 197: /* WidgetComponents: HORIZSCROLLBAR  */
#line 675 "Automation.y"
                                                { yytknval4 = HORIZSCROLLBAR; }
#line 3580 "y.tab.c"
    break;

  case 198: /* WidgetComponents: HORIZSBLEFTARROW  */
#line 676 "Automation.y"
                                                { yytknval4 = HORIZSBLEFTARROW; }
#line 3586 "y.tab.c"
    break;

  case 199: /* WidgetComponents: HORIZSBRIGHTARROW  */
#line 677 "Automation.y"
                                                { yytknval4 = HORIZSBRIGHTARROW; }
#line 3592 "y.tab.c"
    break;

  case 200: /* WidgetComponents: HORIZSBLEFTSLIDERAREA  */
#line 678 "Automation.y"
                                                { yytknval4 = HORIZSBLEFTSLIDERAREA; }
#line 3598 "y.tab.c"
    break;

  case 201: /* WidgetComponents: HORIZSBRIGHTSLIDERAREA  */
#line 679 "Automation.y"
                                                 { yytknval4 = HORIZSBRIGHTSLIDERAREA; }
#line 3604 "y.tab.c"
    break;

  case 202: /* WidgetComponents: HORIZSBSLIDER  */
#line 680 "Automation.y"
                                                { yytknval4 = HORIZSBSLIDER; }
#line 3610 "y.tab.c"
    break;

  case 203: /* WidgetComponents: VERTSCROLLBAR  */
#line 681 "Automation.y"
                                                { yytknval4 = VERTSCROLLBAR; }
#line 3616 "y.tab.c"
    break;

  case 204: /* WidgetComponents: VERTSBUPARROW  */
#line 682 "Automation.y"
                                                { yytknval4 = VERTSBUPARROW; }
#line 3622 "y.tab.c"
    break;

  case 205: /* WidgetComponents: VERTSBDOWNARROW  */
#line 683 "Automation.y"
                                                { yytknval4 = VERTSBDOWNARROW; }
#line 3628 "y.tab.c"
    break;

  case 206: /* WidgetComponents: VERTSBUPSLIDERAREA  */
#line 684 "Automation.y"
                                                { yytknval4 = VERTSBUPSLIDERAREA; }
#line 3634 "y.tab.c"
    break;

  case 207: /* WidgetComponents: VERTSBDOWNSLIDERAREA  */
#line 685 "Automation.y"
                                                { yytknval4 = VERTSBDOWNSLIDERAREA; }
#line 3640 "y.tab.c"
    break;

  case 208: /* WidgetComponents: VERTSBSLIDER  */
#line 686 "Automation.y"
                                                { yytknval4 = VERTSBSLIDER; }
#line 3646 "y.tab.c"
    break;

  case 209: /* WidgetComponents: MENUBAR  */
#line 687 "Automation.y"
                                                { yytknval4 = MENUBAR; }
#line 3652 "y.tab.c"
    break;

  case 210: /* WidgetComponents: SASH  */
#line 688 "Automation.y"
                                                { yytknval4 = SASH; }
#line 3658 "y.tab.c"
    break;

  case 211: /* WidgetComponents: CLOSEBUTTON  */
#line 689 "Automation.y"
                                                { yytknval4 = CLOSEBUTTON; }
#line 3664 "y.tab.c"
    break;

  case 212: /* WidgetComponents: HELPBUTTON  */
#line 690 "Automation.y"
                                                { yytknval4 = HELPBUTTON; }
#line 3670 "y.tab.c"
    break;

  case 213: /* WidgetComponents: OKBUTTON  */
#line 691 "Automation.y"
                                                { yytknval4 = OKBUTTON; }
#line 3676 "y.tab.c"
    break;

  case 214: /* WidgetComponents: FILTERBUTTON  */
#line 692 "Automation.y"
                                                { yytknval4 = FILTERBUTTON; }
#line 3682 "y.tab.c"
    break;

  case 215: /* WidgetComponents: CANCELBUTTON  */
#line 693 "Automation.y"
                                                { yytknval4 = CANCELBUTTON; }
#line 3688 "y.tab.c"
    break;

  case 216: /* WidgetComponents: APPLYBUTTON  */
#line 694 "Automation.y"
                                                { yytknval4 = APPLYBUTTON; }
#line 3694 "y.tab.c"
    break;

  case 217: /* WidgetComponents: MESSAGELABEL  */
#line 695 "Automation.y"
                                                { yytknval4 = MESSAGELABEL; }
#line 3700 "y.tab.c"
    break;

  case 218: /* WidgetComponents: SYMBOLLABEL  */
#line 696 "Automation.y"
                                                { yytknval4 = SYMBOLLABEL; }
#line 3706 "y.tab.c"
    break;

  case 219: /* WidgetComponents: SELECTIONTEXT  */
#line 697 "Automation.y"
                                                { yytknval4 = SELECTIONTEXT; }
#line 3712 "y.tab.c"
    break;

  case 220: /* WidgetComponents: SELECTIONTEXTCHAR  */
#line 698 "Automation.y"
                                                { yytknval4 = SELECTIONTEXTCHAR; }
#line 3718 "y.tab.c"
    break;

  case 221: /* WidgetComponents: SELECTIONLABEL  */
#line 699 "Automation.y"
                                                { yytknval4 = SELECTIONLABEL; }
#line 3724 "y.tab.c"
    break;

  case 222: /* WidgetComponents: SELECTIONLISTLABEL  */
#line 700 "Automation.y"
                                                { yytknval4 = SELECTIONLISTLABEL;}
#line 3730 "y.tab.c"
    break;

  case 223: /* WidgetComponents: SELECTIONLIST  */
#line 701 "Automation.y"
                                                { yytknval4 = SELECTIONLIST; }
#line 3736 "y.tab.c"
    break;

  case 224: /* WidgetComponents: FILELIST  */
#line 702 "Automation.y"
                                                { yytknval4 = FILELIST; }
#line 3742 "y.tab.c"
    break;

  case 225: /* WidgetComponents: FILELISTLABEL  */
#line 703 "Automation.y"
                                                { yytknval4 = FILELISTLABEL; }
#line 3748 "y.tab.c"
    break;

  case 226: /* WidgetComponents: DIRLIST  */
#line 704 "Automation.y"
                                                { yytknval4 = DIRLIST; }
#line 3754 "y.tab.c"
    break;

  case 227: /* WidgetComponents: DIRLISTLABEL  */
#line 705 "Automation.y"
                                                { yytknval4 = DIRLISTLABEL; }
#line 3760 "y.tab.c"
    break;

  case 228: /* WidgetComponents: FILESELECTIONTEXT  */
#line 706 "Automation.y"
                                                { yytknval4 = FILESELECTIONTEXT; }
#line 3766 "y.tab.c"
    break;

  case 229: /* WidgetComponents: FILESELECTIONTEXTCHAR  */
#line 707 "Automation.y"
                                                { yytknval4 = FILESELECTIONTEXTCHAR; }
#line 3772 "y.tab.c"
    break;

  case 230: /* WidgetComponents: FILESELECTIONLABEL  */
#line 708 "Automation.y"
                                                { yytknval4 = FILESELECTIONLABEL; }
#line 3778 "y.tab.c"
    break;

  case 231: /* WidgetComponents: FILEFILTERTEXT  */
#line 709 "Automation.y"
                                                { yytknval4 = FILEFILTERTEXT; }
#line 3784 "y.tab.c"
    break;

  case 232: /* WidgetComponents: FILEFILTERTEXTCHAR  */
#line 710 "Automation.y"
                                                { yytknval4 = FILEFILTERTEXTCHAR; }
#line 3790 "y.tab.c"
    break;

  case 233: /* WidgetComponents: FILEFILTERLABEL  */
#line 711 "Automation.y"
                                                { yytknval4 = FILEFILTERLABEL; }
#line 3796 "y.tab.c"
    break;

  case 234: /* WidgetComponents: WORKAREA  */
#line 712 "Automation.y"
                                                { yytknval4 = WORKAREA; }
#line 3802 "y.tab.c"
    break;

  case 235: /* WidgetComponents: COMMANDWINDOW  */
#line 713 "Automation.y"
                                                { yytknval4 = COMMANDWINDOW; }
#line 3808 "y.tab.c"
    break;

  case 236: /* WidgetComponents: MESSAGEWINDOW  */
#line 714 "Automation.y"
                                                { yytknval4 = MESSAGEWINDOW; }
#line 3814 "y.tab.c"
    break;

  case 237: /* WidgetComponents: WORKWINDOW  */
#line 715 "Automation.y"
                                                { yytknval4 = WORKWINDOW; }
#line 3820 "y.tab.c"
    break;

  case 238: /* WidgetComponents: SCALE  */
#line 716 "Automation.y"
                                                { yytknval4 = SCALE; }
#line 3826 "y.tab.c"
    break;

  case 239: /* WidgetComponents: SCALELABEL  */
#line 717 "Automation.y"
                                                { yytknval4 = SCALELABEL; }
#line 3832 "y.tab.c"
    break;

  case 240: /* WidgetComponents: OPTIONBUTTON  */
#line 718 "Automation.y"
                                                { yytknval4 = OPTIONBUTTON; }
#line 3838 "y.tab.c"
    break;

  case 241: /* WidgetComponents: SCROLLEDWINDOW  */
#line 719 "Automation.y"
                                                { yytknval4 = SCROLLEDWINDOW; }
#line 3844 "y.tab.c"
    break;

  case 242: /* WidgetComponents: SCROLLBARSLIDER  */
#line 720 "Automation.y"
                                                { yytknval4 = SCROLLBARSLIDER; }
#line 3850 "y.tab.c"
    break;

  case 243: /* WidgetComponents: SCROLLBARUPORLEFTARROW  */
#line 722 "Automation.y"
                                { yytknval4 = SCROLLBARUPORLEFTARROW; }
#line 3856 "y.tab.c"
    break;

  case 244: /* WidgetComponents: SCROLLBARDOWNORRIGHTARROW  */
#line 724 "Automation.y"
                                { yytknval4 = SCROLLBARDOWNORRIGHTARROW; }
#line 3862 "y.tab.c"
    break;

  case 245: /* WidgetComponents: SCROLLBARUPORLEFTSLIDERAREA  */
#line 726 "Automation.y"
                                { yytknval4 = SCROLLBARUPORLEFTSLIDERAREA; }
#line 3868 "y.tab.c"
    break;

  case 246: /* WidgetComponents: SCROLLBARDOWNORRIGHTSLIDERAREA  */
#line 728 "Automation.y"
                                { yytknval4 = SCROLLBARDOWNORRIGHTSLIDERAREA; }
#line 3874 "y.tab.c"
    break;

  case 247: /* WidgetComponents: FLHSBUPORLEFTARROW  */
#line 730 "Automation.y"
                                { yytknval4 = FLHSBUPORLEFTARROW; }
#line 3880 "y.tab.c"
    break;

  case 248: /* WidgetComponents: FLHSBDOWNORRIGHTARROW  */
#line 732 "Automation.y"
                                { yytknval4 = FLHSBDOWNORRIGHTARROW; }
#line 3886 "y.tab.c"
    break;

  case 249: /* WidgetComponents: FLHSBUPORLEFTSLIDERAREA  */
#line 734 "Automation.y"
                                { yytknval4 = FLHSBUPORLEFTSLIDERAREA; }
#line 3892 "y.tab.c"
    break;

  case 250: /* WidgetComponents: FLHSBDOWNORRIGHTSLIDERAREA  */
#line 736 "Automation.y"
                                { yytknval4 = FLHSBDOWNORRIGHTSLIDERAREA; }
#line 3898 "y.tab.c"
    break;

  case 251: /* WidgetComponents: FLHSBSLIDER  */
#line 738 "Automation.y"
                                { yytknval4 = FLHSBSLIDER; }
#line 3904 "y.tab.c"
    break;

  case 252: /* WidgetComponents: FLVSBUPORLEFTARROW  */
#line 740 "Automation.y"
                                { yytknval4 = FLVSBUPORLEFTARROW; }
#line 3910 "y.tab.c"
    break;

  case 253: /* WidgetComponents: FLVSBDOWNORRIGHTARROW  */
#line 742 "Automation.y"
                                { yytknval4 = FLVSBDOWNORRIGHTARROW; }
#line 3916 "y.tab.c"
    break;

  case 254: /* WidgetComponents: FLVSBUPORLEFTSLIDERAREA  */
#line 744 "Automation.y"
                                { yytknval4 = FLVSBUPORLEFTSLIDERAREA; }
#line 3922 "y.tab.c"
    break;

  case 255: /* WidgetComponents: FLVSBDOWNORRIGHTSLIDERAREA  */
#line 746 "Automation.y"
                                { yytknval4 = FLVSBDOWNORRIGHTSLIDERAREA; }
#line 3928 "y.tab.c"
    break;

  case 256: /* WidgetComponents: FLVSBSLIDER  */
#line 748 "Automation.y"
                                { yytknval4 = FLVSBSLIDER; }
#line 3934 "y.tab.c"
    break;

  case 257: /* WidgetComponents: DLHSBUPORLEFTARROW  */
#line 750 "Automation.y"
                                { yytknval4 = DLHSBUPORLEFTARROW; }
#line 3940 "y.tab.c"
    break;

  case 258: /* WidgetComponents: DLHSBDOWNORRIGHTARROW  */
#line 752 "Automation.y"
                                { yytknval4 = DLHSBDOWNORRIGHTARROW; }
#line 3946 "y.tab.c"
    break;

  case 259: /* WidgetComponents: DLHSBUPORLEFTSLIDERAREA  */
#line 754 "Automation.y"
                                { yytknval4 = DLHSBUPORLEFTSLIDERAREA; }
#line 3952 "y.tab.c"
    break;

  case 260: /* WidgetComponents: DLHSBDOWNORRIGHTSLIDERAREA  */
#line 756 "Automation.y"
                                { yytknval4 = DLHSBDOWNORRIGHTSLIDERAREA; }
#line 3958 "y.tab.c"
    break;

  case 261: /* WidgetComponents: DLHSBSLIDER  */
#line 758 "Automation.y"
                                { yytknval4 = DLHSBSLIDER; }
#line 3964 "y.tab.c"
    break;

  case 262: /* WidgetComponents: DLVSBUPORLEFTARROW  */
#line 760 "Automation.y"
                                { yytknval4 = DLVSBUPORLEFTARROW; }
#line 3970 "y.tab.c"
    break;

  case 263: /* WidgetComponents: DLVSBDOWNORRIGHTARROW  */
#line 762 "Automation.y"
                                { yytknval4 = DLVSBDOWNORRIGHTARROW; }
#line 3976 "y.tab.c"
    break;

  case 264: /* WidgetComponents: DLVSBUPORLEFTSLIDERAREA  */
#line 764 "Automation.y"
                                { yytknval4 = DLVSBUPORLEFTSLIDERAREA; }
#line 3982 "y.tab.c"
    break;

  case 265: /* WidgetComponents: DLVSBDOWNORRIGHTSLIDERAREA  */
#line 766 "Automation.y"
                                { yytknval4 = DLVSBDOWNORRIGHTSLIDERAREA; }
#line 3988 "y.tab.c"
    break;

  case 266: /* WidgetComponents: DLVSBSLIDER  */
#line 768 "Automation.y"
                                { yytknval4 = DLVSBSLIDER; }
#line 3994 "y.tab.c"
    break;

  case 267: /* WidgetComponents: SLHSBUPORLEFTARROW  */
#line 770 "Automation.y"
                                { yytknval4 = SLHSBUPORLEFTARROW; }
#line 4000 "y.tab.c"
    break;

  case 268: /* WidgetComponents: SLHSBDOWNORRIGHTARROW  */
#line 772 "Automation.y"
                                { yytknval4 = SLHSBDOWNORRIGHTARROW; }
#line 4006 "y.tab.c"
    break;

  case 269: /* WidgetComponents: SLHSBUPORLEFTSLIDERAREA  */
#line 774 "Automation.y"
                                { yytknval4 = SLHSBUPORLEFTSLIDERAREA; }
#line 4012 "y.tab.c"
    break;

  case 270: /* WidgetComponents: SLHSBDOWNORRIGHTSLIDERAREA  */
#line 776 "Automation.y"
                                { yytknval4 = SLHSBDOWNORRIGHTSLIDERAREA; }
#line 4018 "y.tab.c"
    break;

  case 271: /* WidgetComponents: SLHSBSLIDER  */
#line 778 "Automation.y"
                                { yytknval4 = SLHSBSLIDER; }
#line 4024 "y.tab.c"
    break;

  case 272: /* WidgetComponents: SLVSBUPORLEFTARROW  */
#line 780 "Automation.y"
                                { yytknval4 = SLVSBUPORLEFTARROW; }
#line 4030 "y.tab.c"
    break;

  case 273: /* WidgetComponents: SLVSBDOWNORRIGHTARROW  */
#line 782 "Automation.y"
                                { yytknval4 = SLVSBDOWNORRIGHTARROW; }
#line 4036 "y.tab.c"
    break;

  case 274: /* WidgetComponents: SLVSBUPORLEFTSLIDERAREA  */
#line 784 "Automation.y"
                                { yytknval4 = SLVSBUPORLEFTSLIDERAREA; }
#line 4042 "y.tab.c"
    break;

  case 275: /* WidgetComponents: SLVSBDOWNORRIGHTSLIDERAREA  */
#line 786 "Automation.y"
                                { yytknval4 = SLVSBDOWNORRIGHTSLIDERAREA; }
#line 4048 "y.tab.c"
    break;

  case 276: /* WidgetComponents: SLVSBSLIDER  */
#line 788 "Automation.y"
                                { yytknval4 = SLVSBSLIDER; }
#line 4054 "y.tab.c"
    break;

  case 277: /* WidgetComponents: TEAROFFBUTTON  */
#line 790 "Automation.y"
                                { yytknval4 = TEAROFFBUTTON; }
#line 4060 "y.tab.c"
    break;

  case 279: /* XCoordinate: DIGIT  */
#line 796 "Automation.y"
                                                { yydigitval2 = yydigitval; }
#line 4066 "y.tab.c"
    break;

  case 281: /* PointerCommands: PCommandName WidgetName Component Location  */
#line 808 "Automation.y"
                        {
				BuildPointerCommand(yytknval1,
						    yystringval,
						    yytknval4,
						    yydigitval3);
				yydigitval3 = 0;
			}
#line 4078 "y.tab.c"
    break;

  case 282: /* PCommandName: LOCATEPOINTER  */
#line 816 "Automation.y"
                                                { yytknval1 = LOCATEPOINTER; }
#line 4084 "y.tab.c"
    break;

  case 284: /* Component: %empty  */
#line 822 "Automation.y"
                                                { yytknval4 = -1; }
#line 4090 "y.tab.c"
    break;

  case 286: /* WidgetComponents: SEPARATOR  */
#line 826 "Automation.y"
                                                { yytknval4 = SEPARATOR; }
#line 4096 "y.tab.c"
    break;

  case 287: /* WidgetComponents: LABEL  */
#line 827 "Automation.y"
                                                { yytknval4 = LABEL; }
#line 4102 "y.tab.c"
    break;

  case 288: /* WidgetComponents: LISTITEM  */
#line 828 "Automation.y"
                                                { yytknval4 = LISTITEM; }
#line 4108 "y.tab.c"
    break;

  case 289: /* WidgetComponents: FILELISTITEM  */
#line 829 "Automation.y"
                                                { yytknval4 = FILELISTITEM; }
#line 4114 "y.tab.c"
    break;

  case 290: /* WidgetComponents: DIRLISTITEM  */
#line 830 "Automation.y"
                                                { yytknval4 = DIRLISTITEM; }
#line 4120 "y.tab.c"
    break;

  case 291: /* WidgetComponents: DIRFILTERLABEL  */
#line 831 "Automation.y"
                                                { yytknval4 = DIRFILTERLABEL; }
#line 4126 "y.tab.c"
    break;

  case 292: /* WidgetComponents: DIRFILTERTEXT  */
#line 832 "Automation.y"
                                                { yytknval4 = DIRFILTERTEXT; }
#line 4132 "y.tab.c"
    break;

  case 293: /* WidgetComponents: DIRFILTERTEXTCHAR  */
#line 833 "Automation.y"
                                                { yytknval4 = DIRFILTERTEXTCHAR; }
#line 4138 "y.tab.c"
    break;

  case 294: /* WidgetComponents: SELECTIONLISTITEM  */
#line 834 "Automation.y"
                                                { yytknval4 = SELECTIONLISTITEM; }
#line 4144 "y.tab.c"
    break;

  case 295: /* WidgetComponents: TEXTCHAR  */
#line 835 "Automation.y"
                                                { yytknval4 = TEXTCHAR; }
#line 4150 "y.tab.c"
    break;

  case 296: /* WidgetComponents: CSTEXTCHAR  */
#line 836 "Automation.y"
                                                { yytknval4 = CSTEXTCHAR; }
#line 4156 "y.tab.c"
    break;

  case 297: /* WidgetComponents: SLIDER  */
#line 837 "Automation.y"
                                                { yytknval4 = SLIDER; }
#line 4162 "y.tab.c"
    break;

  case 298: /* WidgetComponents: COMBOBOXDROPDOWNBUTTON  */
#line 838 "Automation.y"
                                                { yytknval4 = COMBOBOXDROPDOWNBUTTON;}
#line 4168 "y.tab.c"
    break;

  case 299: /* WidgetComponents: OUTLINEBTN  */
#line 839 "Automation.y"
                                                { yytknval4 = OUTLINEBTN;}
#line 4174 "y.tab.c"
    break;

  case 300: /* WidgetComponents: INCREMENTBTN  */
#line 840 "Automation.y"
                                                { yytknval4 = INCREMENTBTN;}
#line 4180 "y.tab.c"
    break;

  case 301: /* WidgetComponents: DECREMENTBTN  */
#line 841 "Automation.y"
                                                { yytknval4 = DECREMENTBTN;}
#line 4186 "y.tab.c"
    break;

  case 302: /* WidgetComponents: UPORLEFTARROW  */
#line 842 "Automation.y"
                                                { yytknval4 = UPORLEFTARROW; }
#line 4192 "y.tab.c"
    break;

  case 303: /* WidgetComponents: DOWNORRIGHTARROW  */
#line 843 "Automation.y"
                                                { yytknval4 = DOWNORRIGHTARROW;}
#line 4198 "y.tab.c"
    break;

  case 304: /* WidgetComponents: UPORLEFTSLIDERAREA  */
#line 844 "Automation.y"
                                                { yytknval4 = UPORLEFTSLIDERAREA;}
#line 4204 "y.tab.c"
    break;

  case 305: /* WidgetComponents: DOWNORRIGHTSLIDERAREA  */
#line 845 "Automation.y"
                                                { yytknval4 = DOWNORRIGHTSLIDERAREA;}
#line 4210 "y.tab.c"
    break;

  case 306: /* WidgetComponents: SCROLLBAR  */
#line 846 "Automation.y"
                                                { yytknval4 = SCROLLBAR; }
#line 4216 "y.tab.c"
    break;

  case 307: /* WidgetComponents: HORIZSCROLLBAR  */
#line 847 "Automation.y"
                                                { yytknval4 = HORIZSCROLLBAR; }
#line 4222 "y.tab.c"
    break;

  case 308: /* WidgetComponents: HORIZSBLEFTARROW  */
#line 848 "Automation.y"
                                                { yytknval4 = HORIZSBLEFTARROW; }
#line 4228 "y.tab.c"
    break;

  case 309: /* WidgetComponents: HORIZSBRIGHTARROW  */
#line 849 "Automation.y"
                                                { yytknval4 = HORIZSBRIGHTARROW; }
#line 4234 "y.tab.c"
    break;

  case 310: /* WidgetComponents: HORIZSBLEFTSLIDERAREA  */
#line 850 "Automation.y"
                                                { yytknval4 = HORIZSBLEFTSLIDERAREA; }
#line 4240 "y.tab.c"
    break;

  case 311: /* WidgetComponents: HORIZSBRIGHTSLIDERAREA  */
#line 851 "Automation.y"
                                                 { yytknval4 = HORIZSBRIGHTSLIDERAREA; }
#line 4246 "y.tab.c"
    break;

  case 312: /* WidgetComponents: HORIZSBSLIDER  */
#line 852 "Automation.y"
                                                { yytknval4 = HORIZSBSLIDER; }
#line 4252 "y.tab.c"
    break;

  case 313: /* WidgetComponents: VERTSCROLLBAR  */
#line 853 "Automation.y"
                                                { yytknval4 = VERTSCROLLBAR; }
#line 4258 "y.tab.c"
    break;

  case 314: /* WidgetComponents: VERTSBUPARROW  */
#line 854 "Automation.y"
                                                { yytknval4 = VERTSBUPARROW; }
#line 4264 "y.tab.c"
    break;

  case 315: /* WidgetComponents: VERTSBDOWNARROW  */
#line 855 "Automation.y"
                                                { yytknval4 = VERTSBDOWNARROW; }
#line 4270 "y.tab.c"
    break;

  case 316: /* WidgetComponents: VERTSBUPSLIDERAREA  */
#line 856 "Automation.y"
                                                { yytknval4 = VERTSBUPSLIDERAREA; }
#line 4276 "y.tab.c"
    break;

  case 317: /* WidgetComponents: VERTSBDOWNSLIDERAREA  */
#line 857 "Automation.y"
                                                { yytknval4 = VERTSBDOWNSLIDERAREA; }
#line 4282 "y.tab.c"
    break;

  case 318: /* WidgetComponents: VERTSBSLIDER  */
#line 858 "Automation.y"
                                                { yytknval4 = VERTSBSLIDER; }
#line 4288 "y.tab.c"
    break;

  case 319: /* WidgetComponents: MENUBAR  */
#line 859 "Automation.y"
                                                { yytknval4 = MENUBAR; }
#line 4294 "y.tab.c"
    break;

  case 320: /* WidgetComponents: SASH  */
#line 860 "Automation.y"
                                                { yytknval4 = SASH; }
#line 4300 "y.tab.c"
    break;

  case 321: /* WidgetComponents: CLOSEBUTTON  */
#line 861 "Automation.y"
                                                { yytknval4 = CLOSEBUTTON; }
#line 4306 "y.tab.c"
    break;

  case 322: /* WidgetComponents: HELPBUTTON  */
#line 862 "Automation.y"
                                                { yytknval4 = HELPBUTTON; }
#line 4312 "y.tab.c"
    break;

  case 323: /* WidgetComponents: OKBUTTON  */
#line 863 "Automation.y"
                                                { yytknval4 = OKBUTTON; }
#line 4318 "y.tab.c"
    break;

  case 324: /* WidgetComponents: FILTERBUTTON  */
#line 864 "Automation.y"
                                                { yytknval4 = FILTERBUTTON; }
#line 4324 "y.tab.c"
    break;

  case 325: /* WidgetComponents: CANCELBUTTON  */
#line 865 "Automation.y"
                                                { yytknval4 = CANCELBUTTON; }
#line 4330 "y.tab.c"
    break;

  case 326: /* WidgetComponents: APPLYBUTTON  */
#line 866 "Automation.y"
                                                { yytknval4 = APPLYBUTTON; }
#line 4336 "y.tab.c"
    break;

  case 327: /* WidgetComponents: MESSAGELABEL  */
#line 867 "Automation.y"
                                                { yytknval4 = MESSAGELABEL; }
#line 4342 "y.tab.c"
    break;

  case 328: /* WidgetComponents: SYMBOLLABEL  */
#line 868 "Automation.y"
                                                { yytknval4 = SYMBOLLABEL; }
#line 4348 "y.tab.c"
    break;

  case 329: /* WidgetComponents: SELECTIONTEXT  */
#line 869 "Automation.y"
                                                { yytknval4 = SELECTIONTEXT; }
#line 4354 "y.tab.c"
    break;

  case 330: /* WidgetComponents: SELECTIONTEXTCHAR  */
#line 870 "Automation.y"
                                                { yytknval4 = SELECTIONTEXTCHAR; }
#line 4360 "y.tab.c"
    break;

  case 331: /* WidgetComponents: SELECTIONLABEL  */
#line 871 "Automation.y"
                                                { yytknval4 = SELECTIONLABEL; }
#line 4366 "y.tab.c"
    break;

  case 332: /* WidgetComponents: SELECTIONLISTLABEL  */
#line 872 "Automation.y"
                                                { yytknval4 = SELECTIONLISTLABEL;}
#line 4372 "y.tab.c"
    break;

  case 333: /* WidgetComponents: SELECTIONLIST  */
#line 873 "Automation.y"
                                                { yytknval4 = SELECTIONLIST; }
#line 4378 "y.tab.c"
    break;

  case 334: /* WidgetComponents: FILELIST  */
#line 874 "Automation.y"
                                                { yytknval4 = FILELIST; }
#line 4384 "y.tab.c"
    break;

  case 335: /* WidgetComponents: FILELISTLABEL  */
#line 875 "Automation.y"
                                                { yytknval4 = FILELISTLABEL; }
#line 4390 "y.tab.c"
    break;

  case 336: /* WidgetComponents: DIRLIST  */
#line 876 "Automation.y"
                                                { yytknval4 = DIRLIST; }
#line 4396 "y.tab.c"
    break;

  case 337: /* WidgetComponents: DIRLISTLABEL  */
#line 877 "Automation.y"
                                                { yytknval4 = DIRLISTLABEL; }
#line 4402 "y.tab.c"
    break;

  case 338: /* WidgetComponents: FILESELECTIONTEXT  */
#line 878 "Automation.y"
                                                { yytknval4 = FILESELECTIONTEXT; }
#line 4408 "y.tab.c"
    break;

  case 339: /* WidgetComponents: FILESELECTIONTEXTCHAR  */
#line 879 "Automation.y"
                                                { yytknval4 = FILESELECTIONTEXTCHAR; }
#line 4414 "y.tab.c"
    break;

  case 340: /* WidgetComponents: FILESELECTIONLABEL  */
#line 880 "Automation.y"
                                                { yytknval4 = FILESELECTIONLABEL; }
#line 4420 "y.tab.c"
    break;

  case 341: /* WidgetComponents: FILEFILTERTEXT  */
#line 881 "Automation.y"
                                                { yytknval4 = FILEFILTERTEXT; }
#line 4426 "y.tab.c"
    break;

  case 342: /* WidgetComponents: FILEFILTERTEXTCHAR  */
#line 882 "Automation.y"
                                                { yytknval4 = FILEFILTERTEXTCHAR; }
#line 4432 "y.tab.c"
    break;

  case 343: /* WidgetComponents: FILEFILTERLABEL  */
#line 883 "Automation.y"
                                                { yytknval4 = FILEFILTERLABEL; }
#line 4438 "y.tab.c"
    break;

  case 344: /* WidgetComponents: WORKAREA  */
#line 884 "Automation.y"
                                                { yytknval4 = WORKAREA; }
#line 4444 "y.tab.c"
    break;

  case 345: /* WidgetComponents: COMMANDWINDOW  */
#line 885 "Automation.y"
                                                { yytknval4 = COMMANDWINDOW; }
#line 4450 "y.tab.c"
    break;

  case 346: /* WidgetComponents: MESSAGEWINDOW  */
#line 886 "Automation.y"
                                                { yytknval4 = MESSAGEWINDOW; }
#line 4456 "y.tab.c"
    break;

  case 347: /* WidgetComponents: WORKWINDOW  */
#line 887 "Automation.y"
                                                { yytknval4 = WORKWINDOW; }
#line 4462 "y.tab.c"
    break;

  case 348: /* WidgetComponents: SCALE  */
#line 888 "Automation.y"
                                                { yytknval4 = SCALE; }
#line 4468 "y.tab.c"
    break;

  case 349: /* WidgetComponents: SCALELABEL  */
#line 889 "Automation.y"
                                                { yytknval4 = SCALELABEL; }
#line 4474 "y.tab.c"
    break;

  case 350: /* WidgetComponents: OPTIONBUTTON  */
#line 890 "Automation.y"
                                                { yytknval4 = OPTIONBUTTON; }
#line 4480 "y.tab.c"
    break;

  case 351: /* WidgetComponents: SCROLLEDWINDOW  */
#line 891 "Automation.y"
                                                { yytknval4 = SCROLLEDWINDOW; }
#line 4486 "y.tab.c"
    break;

  case 352: /* WidgetComponents: SCROLLBARSLIDER  */
#line 892 "Automation.y"
                                                { yytknval4 = SCROLLBARSLIDER; }
#line 4492 "y.tab.c"
    break;

  case 353: /* WidgetComponents: SCROLLBARUPORLEFTARROW  */
#line 894 "Automation.y"
                                { yytknval4 = SCROLLBARUPORLEFTARROW; }
#line 4498 "y.tab.c"
    break;

  case 354: /* WidgetComponents: SCROLLBARDOWNORRIGHTARROW  */
#line 896 "Automation.y"
                                { yytknval4 = SCROLLBARDOWNORRIGHTARROW; }
#line 4504 "y.tab.c"
    break;

  case 355: /* WidgetComponents: SCROLLBARUPORLEFTSLIDERAREA  */
#line 898 "Automation.y"
                                { yytknval4 = SCROLLBARUPORLEFTSLIDERAREA; }
#line 4510 "y.tab.c"
    break;

  case 356: /* WidgetComponents: SCROLLBARDOWNORRIGHTSLIDERAREA  */
#line 900 "Automation.y"
                                { yytknval4 = SCROLLBARDOWNORRIGHTSLIDERAREA; }
#line 4516 "y.tab.c"
    break;

  case 357: /* WidgetComponents: FLHSBUPORLEFTARROW  */
#line 902 "Automation.y"
                                { yytknval4 = FLHSBUPORLEFTARROW; }
#line 4522 "y.tab.c"
    break;

  case 358: /* WidgetComponents: FLHSBDOWNORRIGHTARROW  */
#line 904 "Automation.y"
                                { yytknval4 = FLHSBDOWNORRIGHTARROW; }
#line 4528 "y.tab.c"
    break;

  case 359: /* WidgetComponents: FLHSBUPORLEFTSLIDERAREA  */
#line 906 "Automation.y"
                                { yytknval4 = FLHSBUPORLEFTSLIDERAREA; }
#line 4534 "y.tab.c"
    break;

  case 360: /* WidgetComponents: FLHSBDOWNORRIGHTSLIDERAREA  */
#line 908 "Automation.y"
                                { yytknval4 = FLHSBDOWNORRIGHTSLIDERAREA; }
#line 4540 "y.tab.c"
    break;

  case 361: /* WidgetComponents: FLHSBSLIDER  */
#line 910 "Automation.y"
                                { yytknval4 = FLHSBSLIDER; }
#line 4546 "y.tab.c"
    break;

  case 362: /* WidgetComponents: FLVSBUPORLEFTARROW  */
#line 912 "Automation.y"
                                { yytknval4 = FLVSBUPORLEFTARROW; }
#line 4552 "y.tab.c"
    break;

  case 363: /* WidgetComponents: FLVSBDOWNORRIGHTARROW  */
#line 914 "Automation.y"
                                { yytknval4 = FLVSBDOWNORRIGHTARROW; }
#line 4558 "y.tab.c"
    break;

  case 364: /* WidgetComponents: FLVSBUPORLEFTSLIDERAREA  */
#line 916 "Automation.y"
                                { yytknval4 = FLVSBUPORLEFTSLIDERAREA; }
#line 4564 "y.tab.c"
    break;

  case 365: /* WidgetComponents: FLVSBDOWNORRIGHTSLIDERAREA  */
#line 918 "Automation.y"
                                { yytknval4 = FLVSBDOWNORRIGHTSLIDERAREA; }
#line 4570 "y.tab.c"
    break;

  case 366: /* WidgetComponents: FLVSBSLIDER  */
#line 920 "Automation.y"
                                { yytknval4 = FLVSBSLIDER; }
#line 4576 "y.tab.c"
    break;

  case 367: /* WidgetComponents: DLHSBUPORLEFTARROW  */
#line 922 "Automation.y"
                                { yytknval4 = DLHSBUPORLEFTARROW; }
#line 4582 "y.tab.c"
    break;

  case 368: /* WidgetComponents: DLHSBDOWNORRIGHTARROW  */
#line 924 "Automation.y"
                                { yytknval4 = DLHSBDOWNORRIGHTARROW; }
#line 4588 "y.tab.c"
    break;

  case 369: /* WidgetComponents: DLHSBUPORLEFTSLIDERAREA  */
#line 926 "Automation.y"
                                { yytknval4 = DLHSBUPORLEFTSLIDERAREA; }
#line 4594 "y.tab.c"
    break;

  case 370: /* WidgetComponents: DLHSBDOWNORRIGHTSLIDERAREA  */
#line 928 "Automation.y"
                                { yytknval4 = DLHSBDOWNORRIGHTSLIDERAREA; }
#line 4600 "y.tab.c"
    break;

  case 371: /* WidgetComponents: DLHSBSLIDER  */
#line 930 "Automation.y"
                                { yytknval4 = DLHSBSLIDER; }
#line 4606 "y.tab.c"
    break;

  case 372: /* WidgetComponents: DLVSBUPORLEFTARROW  */
#line 932 "Automation.y"
                                { yytknval4 = DLVSBUPORLEFTARROW; }
#line 4612 "y.tab.c"
    break;

  case 373: /* WidgetComponents: DLVSBDOWNORRIGHTARROW  */
#line 934 "Automation.y"
                                { yytknval4 = DLVSBDOWNORRIGHTARROW; }
#line 4618 "y.tab.c"
    break;

  case 374: /* WidgetComponents: DLVSBUPORLEFTSLIDERAREA  */
#line 936 "Automation.y"
                                { yytknval4 = DLVSBUPORLEFTSLIDERAREA; }
#line 4624 "y.tab.c"
    break;

  case 375: /* WidgetComponents: DLVSBDOWNORRIGHTSLIDERAREA  */
#line 938 "Automation.y"
                                { yytknval4 = DLVSBDOWNORRIGHTSLIDERAREA; }
#line 4630 "y.tab.c"
    break;

  case 376: /* WidgetComponents: DLVSBSLIDER  */
#line 940 "Automation.y"
                                { yytknval4 = DLVSBSLIDER; }
#line 4636 "y.tab.c"
    break;

  case 377: /* WidgetComponents: SLHSBUPORLEFTARROW  */
#line 942 "Automation.y"
                                { yytknval4 = SLHSBUPORLEFTARROW; }
#line 4642 "y.tab.c"
    break;

  case 378: /* WidgetComponents: SLHSBDOWNORRIGHTARROW  */
#line 944 "Automation.y"
                                { yytknval4 = SLHSBDOWNORRIGHTARROW; }
#line 4648 "y.tab.c"
    break;

  case 379: /* WidgetComponents: SLHSBUPORLEFTSLIDERAREA  */
#line 946 "Automation.y"
                                { yytknval4 = SLHSBUPORLEFTSLIDERAREA; }
#line 4654 "y.tab.c"
    break;

  case 380: /* WidgetComponents: SLHSBDOWNORRIGHTSLIDERAREA  */
#line 948 "Automation.y"
                                { yytknval4 = SLHSBDOWNORRIGHTSLIDERAREA; }
#line 4660 "y.tab.c"
    break;

  case 381: /* WidgetComponents: SLHSBSLIDER  */
#line 950 "Automation.y"
                                { yytknval4 = SLHSBSLIDER; }
#line 4666 "y.tab.c"
    break;

  case 382: /* WidgetComponents: SLVSBUPORLEFTARROW  */
#line 952 "Automation.y"
                                { yytknval4 = SLVSBUPORLEFTARROW; }
#line 4672 "y.tab.c"
    break;

  case 383: /* WidgetComponents: SLVSBDOWNORRIGHTARROW  */
#line 954 "Automation.y"
                                { yytknval4 = SLVSBDOWNORRIGHTARROW; }
#line 4678 "y.tab.c"
    break;

  case 384: /* WidgetComponents: SLVSBUPORLEFTSLIDERAREA  */
#line 956 "Automation.y"
                                { yytknval4 = SLVSBUPORLEFTSLIDERAREA; }
#line 4684 "y.tab.c"
    break;

  case 385: /* WidgetComponents: SLVSBDOWNORRIGHTSLIDERAREA  */
#line 958 "Automation.y"
                                { yytknval4 = SLVSBDOWNORRIGHTSLIDERAREA; }
#line 4690 "y.tab.c"
    break;

  case 386: /* WidgetComponents: SLVSBSLIDER  */
#line 960 "Automation.y"
                                { yytknval4 = SLVSBSLIDER; }
#line 4696 "y.tab.c"
    break;

  case 387: /* WidgetComponents: TEAROFFBUTTON  */
#line 962 "Automation.y"
                                { yytknval4 = TEAROFFBUTTON; }
#line 4702 "y.tab.c"
    break;

  case 388: /* Location: %empty  */
#line 965 "Automation.y"
                                                { yydigitval3 = -1; }
#line 4708 "y.tab.c"
    break;

  case 389: /* Location: DIGIT  */
#line 966 "Automation.y"
                                                { yydigitval3 = yydigitval; }
#line 4714 "y.tab.c"
    break;

  case 390: /* Location: OFF  */
#line 967 "Automation.y"
                                                { yydigitval3 = OFF; }
#line 4720 "y.tab.c"
    break;

  case 391: /* Location: RIGHT  */
#line 968 "Automation.y"
                                                { yydigitval3 = RIGHT; }
#line 4726 "y.tab.c"
    break;

  case 392: /* Location: LEFT  */
#line 969 "Automation.y"
                                                { yydigitval3 = LEFT; }
#line 4732 "y.tab.c"
    break;

  case 393: /* Location: UP  */
#line 970 "Automation.y"
                                                { yydigitval3 = UP; }
#line 4738 "y.tab.c"
    break;

  case 394: /* Location: DOWN  */
#line 971 "Automation.y"
                                                { yydigitval3 = DOWN; }
#line 4744 "y.tab.c"
    break;

  case 395: /* Location: ANY  */
#line 972 "Automation.y"
                                                { yydigitval3 = ANY; }
#line 4750 "y.tab.c"
    break;

  case 396: /* Location: AUTOMAX  */
#line 973 "Automation.y"
                                                { yydigitval3 = AUTOMAX; }
#line 4756 "y.tab.c"
    break;

  case 397: /* Location: AUTOMIN  */
#line 974 "Automation.y"
                                                { yydigitval3 = AUTOMIN; }
#line 4762 "y.tab.c"
    break;

  case 398: /* SystemCommands: SCommandName SCommand SCommandArgs  */
#line 984 "Automation.y"
                        {
				BuildSystemCommand(yytknval1,
						   yystringval2,
						   yysysarglist,
						   yysysargs);
				yysysargs = 0;
			}
#line 4774 "y.tab.c"
    break;

  case 399: /* SCommandName: SYSTEM  */
#line 992 "Automation.y"
                                                { yytknval1 = SYSTEM; }
#line 4780 "y.tab.c"
    break;

  case 400: /* SCommand: STRING  */
#line 995 "Automation.y"
                                                { yystringval2 = (char *)malloc(
						     sizeof(char) *
						     (strlen(yystringval) + 1));
						  strcpy(yystringval2,
							 yystringval); }
#line 4790 "y.tab.c"
    break;

  case 403: /* SCommandArg: STRING  */
#line 1006 "Automation.y"
                                                { yysysarglist[yysysargs++] =
						  	yystringval; }
#line 4797 "y.tab.c"
    break;

  case 404: /* SCommandArg: DIGIT  */
#line 1008 "Automation.y"
                                                {
						  temp = (char *) malloc
							  (sizeof(char) * 10);
						  sprintf(temp, "%d",
							       yydigitval);
						  yysysarglist[yysysargs++] =
							temp; }
#line 4809 "y.tab.c"
    break;

  case 405: /* StoreCommands: StoreCommandName WidgetName1 Component Identifier  */
#line 1024 "Automation.y"
                        {
				BuildStoreCommand(yytknval1,
						  yystringval2,
						  yytknval4,
						  yystringval);
			}
#line 4820 "y.tab.c"
    break;

  case 406: /* StoreCommandName: STOREVISUAL  */
#line 1032 "Automation.y"
                                               { yytknval1 = STOREVISUAL; }
#line 4826 "y.tab.c"
    break;

  case 407: /* StoreCommandName: COMPARESTOREDVISUAL  */
#line 1034 "Automation.y"
                                               { yytknval1 = COMPARESTOREDVISUAL; }
#line 4832 "y.tab.c"
    break;

  case 408: /* WidgetName1: STRING  */
#line 1037 "Automation.y"
                                               { yystringval2 = (char *)malloc(
						    sizeof(char) *
						    (strlen(yystringval) + 1));
						  strcpy(yystringval2,
						         yystringval); }
#line 4842 "y.tab.c"
    break;

  case 410: /* CompareCommands: CCommandName WidgetName Component  */
#line 1052 "Automation.y"
                        {
				BuildCompareCommand(yytknval1,
						    yystringval,
						    yytknval4);
			}
#line 4852 "y.tab.c"
    break;

  case 411: /* CCommandName: COMPAREVISUAL  */
#line 1058 "Automation.y"
                                                { yytknval1 = COMPAREVISUAL; }
#line 4858 "y.tab.c"
    break;

  case 412: /* DragCommands: DCommandName DragList  */
#line 1069 "Automation.y"
                        {
				BuildDragCommand(yytknval1,
						 yydigitval2,
						 yydigitval,
						 yystringval,
						 yytknval4,
						 yydigitval3,
						 yytknval3,
						 yymodmaskval,
						 yymaskcnt);
				yydigitval2 = yydigitval = yydigitval3 = 0;
				yymaskcnt = 0;
			}
#line 4876 "y.tab.c"
    break;

  case 413: /* DCommandName: DRAG  */
#line 1084 "Automation.y"
                                                { yytknval1 = DRAG; }
#line 4882 "y.tab.c"
    break;

  case 414: /* DCommandName: DRAGRELATIVE  */
#line 1085 "Automation.y"
                                                { yytknval1 = DRAGRELATIVE; }
#line 4888 "y.tab.c"
    break;

  case 415: /* DCommandName: DRAGSLIDERVAL  */
#line 1086 "Automation.y"
                                                { yytknval1 = DRAGSLIDERVAL; }
#line 4894 "y.tab.c"
    break;

  case 416: /* DragList: DragRelativeList  */
#line 1090 "Automation.y"
                                                { yystringval = NULL;
					      	  yystringval2 = NULL; }
#line 4901 "y.tab.c"
    break;

  case 417: /* DragList: DragComponentList  */
#line 1093 "Automation.y"
                                                { yydigitval = 0;
						  yydigitval2 = 0; }
#line 4908 "y.tab.c"
    break;

  case 419: /* Direction: RIGHT  */
#line 1100 "Automation.y"
                                                { yydigitval3 = RIGHT; }
#line 4914 "y.tab.c"
    break;

  case 420: /* Direction: LEFT  */
#line 1101 "Automation.y"
                                                { yydigitval3 = LEFT; }
#line 4920 "y.tab.c"
    break;

  case 421: /* Direction: UP  */
#line 1102 "Automation.y"
                                                { yydigitval3 = UP; }
#line 4926 "y.tab.c"
    break;

  case 422: /* Direction: DOWN  */
#line 1103 "Automation.y"
                                                { yydigitval3 = DOWN; }
#line 4932 "y.tab.c"
    break;

  case 423: /* Direction: ANY  */
#line 1104 "Automation.y"
                                                { yydigitval3 = ANY; }
#line 4938 "y.tab.c"
    break;

  case 424: /* Direction: AUTOMAX  */
#line 1105 "Automation.y"
                                                { yydigitval3 = AUTOMAX; }
#line 4944 "y.tab.c"
    break;

  case 425: /* Direction: AUTOMIN  */
#line 1106 "Automation.y"
                                                { yydigitval3 = AUTOMIN; }
#line 4950 "y.tab.c"
    break;

  case 426: /* Xdirection: DIGIT  */
#line 1109 "Automation.y"
                                                { yydigitval2 = yydigitval; }
#line 4956 "y.tab.c"
    break;

  case 427: /* Xdirection: AUTOMAX  */
#line 1110 "Automation.y"
                                                { yydigitval2 = AUTOMAX; }
#line 4962 "y.tab.c"
    break;

  case 428: /* Xdirection: AUTOMIN  */
#line 1111 "Automation.y"
                                                { yydigitval2 = AUTOMIN; }
#line 4968 "y.tab.c"
    break;

  case 429: /* Ydirection: %empty  */
#line 1114 "Automation.y"
                                                { yydigitval = -1; }
#line 4974 "y.tab.c"
    break;

  case 432: /* InputCommands: InputCommandName InputStringData  */
#line 1127 "Automation.y"
                        {
				BuildInputCommand(yytknval1,
						  yyinputstring);
			}
#line 4983 "y.tab.c"
    break;

  case 433: /* InputCommandName: INPUTSTRING  */
#line 1132 "Automation.y"
                                                { yytknval1 = INPUTSTRING; }
#line 4989 "y.tab.c"
    break;

  case 435: /* WindowCommands: WCommandName WidgetName  */
#line 1143 "Automation.y"
                        {
				BuildWindowCommand(yytknval1,
						   yystringval);
			}
#line 4998 "y.tab.c"
    break;

  case 436: /* WCommandName: WINDOWICONIFY  */
#line 1149 "Automation.y"
                                                { yytknval1 = WINDOWICONIFY; }
#line 5004 "y.tab.c"
    break;

  case 437: /* WCommandName: WINDOWDEICONIFY  */
#line 1150 "Automation.y"
                                                { yytknval1 = WINDOWDEICONIFY; }
#line 5010 "y.tab.c"
    break;

  case 438: /* WCommandName: WINDOWMENUUNPOST  */
#line 1152 "Automation.y"
                                              { yytknval1 = WINDOWMENUUNPOST; }
#line 5016 "y.tab.c"
    break;

  case 439: /* WCommandName: WINDOWMAXIMIZE  */
#line 1153 "Automation.y"
                                                { yytknval1 = WINDOWMAXIMIZE; }
#line 5022 "y.tab.c"
    break;

  case 440: /* WCommandName: WINDOWNORMALIZE  */
#line 1154 "Automation.y"
                                                { yytknval1 = WINDOWNORMALIZE; }
#line 5028 "y.tab.c"
    break;

  case 441: /* WCommandName: WINDOWRAISE  */
#line 1155 "Automation.y"
                                                { yytknval1 = WINDOWRAISE; }
#line 5034 "y.tab.c"
    break;

  case 442: /* WCommandName: WINDOWFOCUS  */
#line 1156 "Automation.y"
                                                { yytknval1 = WINDOWFOCUS; }
#line 5040 "y.tab.c"
    break;

  case 443: /* WCommandName: WINDOWCLICK  */
#line 1157 "Automation.y"
                                                { yytknval1 = WINDOWCLICK; }
#line 5046 "y.tab.c"
    break;

  case 444: /* WCommandName: ICONMENUUNPOST  */
#line 1158 "Automation.y"
                                                { yytknval1 = ICONMENUUNPOST; }
#line 5052 "y.tab.c"
    break;

  case 445: /* WindowCheckCommands: WCheckCommandName Wname Group Iname  */
#line 1167 "Automation.y"
                        {
				BuildWindowCheckCommand(yytknval1,
						   	yystringval2,
						   	yytknval2,
						   	yystringval);
			}
#line 5063 "y.tab.c"
    break;

  case 446: /* WCheckCommandName: WINDOWICHECK  */
#line 1175 "Automation.y"
                                                { yytknval1 = WINDOWICHECK; }
#line 5069 "y.tab.c"
    break;

  case 447: /* Wname: STRING  */
#line 1178 "Automation.y"
                                                { yystringval2 = (char *)malloc(
						     sizeof(char) *
						     (strlen(yystringval) + 1));
						  strcpy(yystringval2,
							 yystringval); }
#line 5079 "y.tab.c"
    break;

  case 449: /* Group: SYSTEMMENU  */
#line 1189 "Automation.y"
                                                { yytknval2 = SYSTEMMENU; }
#line 5085 "y.tab.c"
    break;

  case 450: /* Group: ICONMENU  */
#line 1190 "Automation.y"
                                                { yytknval2 = ICONMENU; }
#line 5091 "y.tab.c"
    break;

  case 451: /* Group: WINDOWDECORATION  */
#line 1192 "Automation.y"
                                               { yytknval2 = WINDOWDECORATION; }
#line 5097 "y.tab.c"
    break;

  case 452: /* WindowSelectCommands: WSelectCommandName WidgetName1 Name ItemNumber Keyboard  */
#line 1201 "Automation.y"
                        {
				BuildWindowSelectCommand(yytknval1,
						   	 yystringval2,
						   	 yystringval,
						   	 yydigitval,
							 yytknval2);
			}
#line 5109 "y.tab.c"
    break;

  case 453: /* WSelectCommandName: WINDOWMENUSELECT  */
#line 1210 "Automation.y"
                                               { yytknval1 = WINDOWMENUSELECT; }
#line 5115 "y.tab.c"
    break;

  case 454: /* WSelectCommandName: ICONMENUSELECT  */
#line 1212 "Automation.y"
                                               { yytknval1 = ICONMENUSELECT; }
#line 5121 "y.tab.c"
    break;

  case 455: /* WidgetName1: STRING  */
#line 1215 "Automation.y"
                                                { yystringval2 = (char *)malloc(
						     sizeof(char) *
						     (strlen(yystringval) + 1));
						     strcpy(yystringval2,
							 yystringval); }
#line 5131 "y.tab.c"
    break;

  case 456: /* Name: %empty  */
#line 1221 "Automation.y"
                                                { yystringval = NULL; }
#line 5137 "y.tab.c"
    break;

  case 457: /* Name: STRING  */
#line 1222 "Automation.y"
                                                { yydigitval = -1; }
#line 5143 "y.tab.c"
    break;

  case 458: /* ItemNumber: %empty  */
#line 1225 "Automation.y"
                                                { yydigitval = -1; }
#line 5149 "y.tab.c"
    break;

  case 460: /* Keyboard: %empty  */
#line 1229 "Automation.y"
                                                { yytknval2 = -1; }
#line 5155 "y.tab.c"
    break;

  case 461: /* Keyboard: KEYBOARD  */
#line 1230 "Automation.y"
                                                { yytknval2 = KEYBOARD; }
#line 5161 "y.tab.c"
    break;

  case 462: /* WindowPostCommands: PostCommandName WidgetName Keyboard  */
#line 1237 "Automation.y"
                        {
				BuildPostCommand(yytknval1,
						 yystringval,
						 yytknval2);
			}
#line 5171 "y.tab.c"
    break;

  case 463: /* PostCommandName: WINDOWMENUPOST  */
#line 1243 "Automation.y"
                                                { yytknval1 = WINDOWMENUPOST; }
#line 5177 "y.tab.c"
    break;

  case 464: /* PostCommandName: ICONMENUPOST  */
#line 1244 "Automation.y"
                                                { yytknval1 = ICONMENUPOST; }
#line 5183 "y.tab.c"
    break;

  case 465: /* ResizeCommands: RCommandName WidgetName Gravity Width Height  */
#line 1252 "Automation.y"
                        {
				BuildResizeCommand(yytknval1, yystringval,
						   yytknval2, yydigitval2,
						   yydigitval);
			}
#line 5193 "y.tab.c"
    break;

  case 466: /* RCommandName: WINDOWRESIZE  */
#line 1258 "Automation.y"
                                                { yytknval1 = WINDOWRESIZE; }
#line 5199 "y.tab.c"
    break;

  case 467: /* Gravity: %empty  */
#line 1261 "Automation.y"
                                                { yytknval2 = 0; }
#line 5205 "y.tab.c"
    break;

  case 468: /* Gravity: NORTH  */
#line 1262 "Automation.y"
                                                { yytknval2 = NORTH; }
#line 5211 "y.tab.c"
    break;

  case 469: /* Gravity: SOUTH  */
#line 1263 "Automation.y"
                                                { yytknval2 = SOUTH; }
#line 5217 "y.tab.c"
    break;

  case 470: /* Gravity: EAST  */
#line 1264 "Automation.y"
                                                { yytknval2 = EAST; }
#line 5223 "y.tab.c"
    break;

  case 471: /* Gravity: WEST  */
#line 1265 "Automation.y"
                                                { yytknval2 = WEST; }
#line 5229 "y.tab.c"
    break;

  case 472: /* Gravity: NORTHEAST  */
#line 1266 "Automation.y"
                                                { yytknval2 = NORTHEAST; }
#line 5235 "y.tab.c"
    break;

  case 473: /* Gravity: NORTHWEST  */
#line 1267 "Automation.y"
                                                { yytknval2 = NORTHWEST; }
#line 5241 "y.tab.c"
    break;

  case 474: /* Gravity: SOUTHEAST  */
#line 1268 "Automation.y"
                                                { yytknval2 = SOUTHEAST; }
#line 5247 "y.tab.c"
    break;

  case 475: /* Gravity: SOUTHWEST  */
#line 1269 "Automation.y"
                                                { yytknval2 = SOUTHWEST; }
#line 5253 "y.tab.c"
    break;

  case 476: /* Width: DIGIT  */
#line 1272 "Automation.y"
                                                { yydigitval2 = yydigitval; }
#line 5259 "y.tab.c"
    break;

  case 477: /* Height: %empty  */
#line 1275 "Automation.y"
                                                { yydigitval = -1; }
#line 5265 "y.tab.c"
    break;

  case 479: /* MoveCommands: MoveCommandName WidgetName Coordinates  */
#line 1284 "Automation.y"
                        {
				BuildMoveCommand(yytknval1, yystringval,
						 yydigitval2, yydigitval);
			}
#line 5274 "y.tab.c"
    break;

  case 480: /* MoveCommandName: WINDOWMOVE  */
#line 1289 "Automation.y"
                                                { yytknval1 = WINDOWMOVE; }
#line 5280 "y.tab.c"
    break;

  case 481: /* MoveCommandName: ICONMOVE  */
#line 1290 "Automation.y"
                                                { yytknval1 = ICONMOVE; }
#line 5286 "y.tab.c"
    break;

  case 482: /* WaitCommands: WaitCommandName Count  */
#line 1298 "Automation.y"
                        {
				BuildWaitCommand(yytknval1, yydigitval);
				yydigitval = 0;
			}
#line 5295 "y.tab.c"
    break;

  case 483: /* WaitCommandName: WAIT  */
#line 1303 "Automation.y"
                                                { yytknval1 = WAIT; }
#line 5301 "y.tab.c"
    break;


#line 5305 "y.tab.c"

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

#line 1308 "Automation.y"


yyerror (s)
char *s;
{

	fprintf(stderr, "%s found parsing input - line %d - Exiting...\n",
		s, line_cnt);
	exit(0);

}
