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
