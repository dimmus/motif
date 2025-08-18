dnl Library version
m4_define([XM_MAJOR], [2])
m4_define([XM_MINOR], [4])
m4_define([XM_PATCH], [0])
m4_define([XM_VERSION], [XM_MAJOR.XM_MINOR.XM_PATCH])

dnl Shared library version (libtool)
m4_define([ABI_CURRENT],  [5])
m4_define([ABI_REVISION], [0])
m4_define([ABI_AGE],      [0])
m4_define([ABI_VERSION],  [ABI_CURRENT:ABI_REVISION:ABI_AGE])

AC_INIT([motif],[XM_VERSION],[https://github.com/dimmus/motif])
AC_PREREQ([2.70])
LT_PREREQ([2.4.6])
LT_INIT
# AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_SRCDIR([lib/Xm/Form.c])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign dist-xz subdir-objects no-define silent-rules no-installinfo ])
AM_SILENT_RULES([yes])
AM_EXTRA_RECURSIVE_TARGETS([demos doc])
AC_LANG([C])

C_STD="gnu99"
AC_SUBST(C_STD)

# AC_CHECK_FILE(/usr/X/include/X11/X.h,
#   AC_PREFIX_DEFAULT(/usr/X),
#   AC_PREFIX_DEFAULT(/usr))
# AC_CHECK_FILE(/usr/X11R7/include/X11/X.h,
#   AC_PREFIX_DEFAULT(/usr/X11R7),
#   AC_PREFIX_DEFAULT(/usr))
# AC_CHECK_FILE(/usr/include/X11/X.h,
#   AC_PREFIX_DEFAULT(/usr))

AC_PATH_X
AC_PATH_XTRA
# AX_LT_LIB_XTHREADS

AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LEX(yywrap)
dnl Do this the old fashioned way. 'bison -y' doesn't cut it
AC_CHECK_PROGS(YACC, byacc, yacc)
dnl AC_PROG_SPLINT
AC_CHECK_PROGS(SPLINT, splint)

if test "$LEX" = :; then
  LEX=${am_missing_run}flex
fi
if test "x$LEX" == xflex; then
  AC_SUBST([LEXLIB], ['-lfl'])
fi

dnl Check the host system type and compiler characteristics
AC_CANONICAL_HOST
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T
AC_CHECK_TYPE([XICProc],,,[#include <X11/Xlib.h>])

dnl Check for X
PKG_INSTALLDIR
PKG_CHECK_MODULES([X], [x11 xpm xt xmu xext])

# Required when PKG_CHECK_MODULES called within an if statement
# PKG_PROG_PKG_CONFIG

save_LIBS="$LIBS"
save_CFLAGS="$CFLAGS"

LIBS="$X_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"

AC_CHECK_HEADERS(X11/Xmu/Editres.h,
  AC_CHECK_LIB(Xmu, _XEditResCheckMessages,
    X_XMU=-lXmu
    AC_DEFINE(HAVE_LIBXMU, 1, This System has libXmu)
    , X_XMU="", -lXt -lX11)
  AC_CHECK_LIB(Xmu, XmuNCopyISOLatin1Lowered,
    AC_DEFINE(HAVE_XMU_N_COPY_ISO,1,Have XmuNCopyISOLatin1Lowered)
    , , -lXt -lX11)
  ,,[[#include <X11/Intrinsic.h>]]
)
AC_SUBST(X_XMU)

AC_HEADER_DIRENT
# AC_CHECK_INCLUDES_DEFAULT

AC_HEADER_SYS_WAIT
AC_CHECK_HEADER(wctype.h,    , AC_DEFINE(HAS_WIDECHAR_FUNCTIONS, 1, System supports wchar))
AC_CHECK_HEADER(langinfo.h,  , AC_DEFINE(CSRG_BASED, 1, System Has langinfo.h))
AC_CHECK_HEADER(X11/Xos_r.h, , AC_DEFINE(NEED_XOS_R_H, 1, System Missing Xos_r.h))
AC_CHECK_HEADER(X11/Xpoll.h, , AC_DEFINE(NEED_XPOLL_H, 1, system Missing X11/Xpoll.h))
AC_CHECK_HEADERS(sys/malloc.h sys/file.h sys/poll.h sys/time.h X11/Xos_r.h X11/Xpoll.h)
AC_CHECK_HEADERS(alloca.h iconv.h fcntl.h limits.h malloc.h strings.h unistd.h wchar.h)

AC_STRUCT_TM
AC_TYPE_UID_T
# AX_WEAK_ALIAS_SUPPORT

AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_SETPGRP
AC_FUNC_STRCOLL
AC_FUNC_STRFTIME
AC_FUNC_FORK([])
AC_FUNC_VPRINTF
# AX_FUNC_VOID_SPRINTF
AC_CHECK_FUNCS(strdup,    ,AC_DEFINE(NEED_STRDUP,     1, no strdup))
AC_CHECK_FUNCS(strcasecmp,,AC_DEFINE(NEED_STRCASECMP, 1, no strcasecmp))
AC_CHECK_FUNCS(putenv,    ,AC_DEFINE(NO_PUTENV,       1, no putenv))
AC_CHECK_FUNCS(regcomp,   ,AC_DEFINE(NO_REGCOMP,      1, no regcmp))
AC_CHECK_FUNCS(memmove,   ,AC_DEFINE(NO_MEMMOVE,      1, no memmove))
AC_CHECK_FUNCS(getcwd getwd iconv nanosleep putenv regcomp regex strcasecmp strdup strlcat)
AC_CHECK_FUNCS(wcslen wcscpy wcsncpy wcschr wcscat wcsncat getcwd gettimeofday mkdir re_comp regcmp select strcspn strerror strstr strtod strtol uname)

AC_CHECK_TYPE(XICProc,,AC_DEFINE(NO_XICPROC, 1, XICProc isn't defined), [#include <X11/Xlib.h>])

AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([#include <sys/types.h>
                      #include <signal.h>
                      ],
                     [return *(signal(0, 0)) (0) == 1;])],
    [ac_cv_type_signal=int],
    [ac_cv_type_signal=void]
)

# Cache the result using AC_CACHE_VAL instead of AC_CACHE_CHECK
AC_CACHE_VAL(ac_cv_type_signal,
    [AC_MSG_RESULT([$ac_cv_type_signal])])

AC_DEFINE_UNQUOTED([RETSIGTYPE], [$ac_cv_type_signal], [Define as the return type of signal handlers (`int' or `void').])

# if test x$enable_themes = x
# then
#     enable_themes="yes"
# fi
# AC_ARG_ENABLE(themes, [  --enable-themes         Enable themes (default=no)])
# if test "$enable_themes" = "yes"
# then
#     if test -f $srcdir/lib/Xm/Theme.h
#     then
#       AC_DEFINE(USE_XMTHEMES, 1, "Use XmThemes")
#       XMTHEME_DIST = "Theme.h ThemeP.h DynLibP.h Theme.c DynLib.c"
#     else
#       AC_MSG_WARN([--enable-themes specified, but I do not have the code. Disabled])
#       enable_themes="no"
#     fi
# fi
# AC_SUBST(XMTHEME_DIST)
# AM_CONDITIONAL(OM_XMTHEMES, test "$enable_themes" = "yes")

# if test x$enable_debug_themes = x
# then
#     enable_debug_themes="yes"
# fi
# AC_ARG_ENABLE(debug-themes, [  --enable-debug-themes   Enable themes debugging (default=no)])
# if test "$enable_themes" = "yes"
# then
#     if test "$enable_debug_themes" = "yes"
#     then
# 	    AC_DEFINE(DEBUGTHEMES, 1, "Debug Themes")
#     fi
# fi

if test x$GCC = xyes
then
    CFLAGS="$CFLAGS -std=$C_STD -Wall -g -fno-strict-aliasing \
        -Wno-unused -Wno-comment -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -Wno-unused-result"
    if test ` $CC -dumpversion | sed -e 's/\(^.\).*/\1/'` = "4" ; then
        CFLAGS="$CFLAGS -fno-tree-ter"
    fi
fi

dnl Defines/substitutions for mwm
AC_DEFINE(NO_OL_COMPAT, 1, "No OL Compatability")
AC_SUBST(CDE_INSTALLATION_TOP, "\${prefix}/dt")
AC_SUBST(CDE_CONFIGURATION_TOP, "\${prefix}/etc/dt")
AC_SUBST(MWMRCDIR, "${libdir}/X11")

dnl lib/Xm/Xmos.c
AC_SUBST(LIBDIR,  "${libdir}/X11")
AC_SUBST(INCDIR,  "${includedir}/X11")
AC_SUBST(DATADIR, "${datadir}")

dnl lib/Xm/VirtKeys.c
AC_SUBST(XMBINDDIR_FALLBACK, "${libdir}/X11/bindings")

dnl config/*.cf and tests
AC_SUBST(RM, "rm -f")

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING(
      [--enable-debug],
      [enable debugging support])
	], [enable_debug=$enableval], [enable_debug=no]
)

dnl UTF8
# if test x$enable_utf8 = x
# then
#     enable_utf8="yes"
# fi

# if test "$enable_utf8" = "yes"
# then
#   AC_DEFINE([UTF8_SUPPORTED], [1], [UTF8 is supported])
#   AC_SEARCH_LIBS([iconv_open], [iconv], ,
#     AC_SEARCH_LIBS([libiconv_open], [iconv]))
# fi

AC_ARG_ENABLE([utf8],
  [AS_HELP_STRING(
      [--enable-utf8],
      [enable UTF-8 support])
	], [enable_utf8=$enableval], [enable_utf8=yes]
)

dnl Message catalog
# if test x$enable_message_catalog = x
# then
#     enable_message_catalog="yes"
# fi
# AC_ARG_ENABLE(message-catalog, [  --enable-message-catalog
#                           Enable building of the X/Open message catalog (default=no)])
# if test "$enable_message_catalog" != "yes"
# then
#     AC_DEFINE(NO_MESSAGE_CATALOG, 1, No Defined Message Catalog)
# fi
AC_ARG_ENABLE([message-catalog],
  [AS_HELP_STRING(
      [--enable-message-catalog],
      [enable X/Open message catalog support])
  ],
  [enable_message_catalog=$enableval],
  [enable_message_catalog=no]
)

dnl Printing

AC_ARG_ENABLE(printing, [  --disable-printing
                       Disable printing support])

if test x$enable_printing = x
then
    enable_printing="yes"
fi

if test "$enable_printing" = "yes"
then
  AC_MSG_CHECKING([for libXp])
  AC_CHECK_HEADERS(X11/extensions/Print.h,
    AC_CHECK_LIB(Xp, XpCreateContext, ,enable_printing="no"),
  enable_printing="no")
fi

if test "$enable_printing" = "yes"
then
  LIB_XP=-lXp
  AC_DEFINE([PRINTING_SUPPORTED], [1], [Printing is supported])
  PRINTING_SUPPORTED_FALSE=!
  PRINTING_SUPPORTED_TRUE=
else
  LIB_XP=
  PRINTING_SUPPORTED_FALSE=
  PRINTING_SUPPORTED_TRUE=!
fi
AC_SUBST(PRINTING_SUPPORTED_FALSE)
AC_SUBST(PRINTING_SUPPORTED_TRUE)

AC_SUBST(LIB_XP)

case "$host_os" in
    freebsd*) CFLAGS="$CFLAGS -DCSRG_BASED -DXNO_MTSAFE_API -DXNO_MTSAFE_PWDAPI";
        LDFLAGS="$LDFLAGS -L/usr/local/lib";;
    cygwin*) CFLAGS="$CFLAGS -DXNO_MTSAFE_DIRENTDAPI";;
    solaris* | sunos*) X_LIBS="$X_LIBS -L/usr/X/lib" &&
        if test x$enable_utf8 = x
        then
            enable_utf8="no"
        fi;
        if test x$enable_xft = x
        then
            enable_xft="no"
        fi;;
esac

AC_FIND_XFT
AC_IMAGE_SUPPORT

dnl Debug builds (assert, etc.)
# AS_IF([test "x$enable_debug" == "xyes"],[
# 	AX_APPEND_COMPILE_FLAGS([-Og -ggdb])
# 	AX_APPEND_COMPILE_FLAGS([-DDEBUG], [CPPFLAGS])
# ],[
# 	AX_APPEND_COMPILE_FLAGS([-O2])
# 	AX_APPEND_COMPILE_FLAGS([-DNDEBUG], [CPPFLAGS])
# ])

dnl Compiler flags
# AX_CFLAGS

dnl .pc
AC_SUBST([XM_VERSION], [XM_VERSION])

dnl Automake substitutions
AC_SUBST([ABI_VERSION], [ABI_VERSION])
AM_CONDITIONAL(MessageCatalog, test "x$enable_message_catalog" = "xyes")
AM_CONDITIONAL(PRINTING,       test "x$enable_printing" = "xyes")
AM_CONDITIONAL(OM_XMU,         test "x$X_XMU" != "x")
AM_CONDITIONAL([XM_PRINTING],  test "x$enable_printing" == "xyes")
AM_CONDITIONAL([XM_WITH_JPEG], test "x$with_jpeg" == "xyes")
AM_CONDITIONAL([XM_WITH_PNG],  test "x$with_png"  == "xyes")
AM_CONDITIONAL([XM_MSGCAT],    test "x$enable_message_catalog" == "xyes")

dnl Substitutions for lib/Xm/Xm.h
AC_SUBST([XM_MAJOR], [XM_MAJOR])
AC_SUBST([XM_MINOR], [XM_MINOR])
AC_SUBST([XM_PATCH], [XM_PATCH])

AC_DEFINE_UNQUOTED(
	[XM_UTF8],
	$(test "x$enable_utf8" != "xyes"; echo $?),
	[1 if Motif was built with UTF8 Support]
)

AC_DEFINE_UNQUOTED(
	[NO_MESSAGE_CATALOG],
	$(test "x$enable_message_catalog" = "xyes" ; echo $?),
	[0 if X/Open message catalog support is enabled]
)

# Option to disable building of demos
AC_ARG_ENABLE([demos],
  [AS_HELP_STRING(
      [--disable-demos],
      [disable building demos])
	], [enable_demos=$enableval], [enable_demos=yes]
)

AC_CONFIG_HEADERS(include/config.h lib/Xm/Xm.h)
AC_CONFIG_FILES([Makefile \
motif.pc \
bindings/Makefile \
bitmaps/Makefile \
clients/Makefile \
clients/mwm/Makefile \
clients/mwm/WmWsmLib/Makefile \
clients/uil/Makefile \
clients/xmbind/Makefile \
config/Makefile \
config/util/Makefile \
include/Makefile \
include/Dt/Makefile \
lib/Makefile \
lib/Xm/Makefile \
lib/Mrm/Makefile \
localized/Makefile \
localized/util/Makefile \
doc/Makefile \
doc/man/Makefile \
doc/man/man1/Makefile \
doc/man/man3/Makefile \
doc/man/man4/Makefile \
doc/man/man5/Makefile \
tools/Makefile \
tools/wml/Makefile \
lib/Xm/xmstring.list \
])

# Conditionally configure demos
if test "$enable_demos" = "yes"
then
  AC_CONFIG_FILES([demos/Makefile \
  demos/lib/Makefile \
  demos/lib/Xmd/Makefile \
  demos/lib/Wsm/Makefile \
  demos/lib/Exm/Makefile \
  demos/lib/Exm/wml/Makefile \
  demos/programs/Makefile \
  demos/programs/Exm/Makefile \
  demos/programs/Exm/app_in_c/Makefile \
  demos/programs/Exm/app_in_uil/Makefile \
  demos/programs/Exm/simple_app/Makefile \
  demos/programs/airport/Makefile \
  demos/programs/animate/Makefile \
  demos/programs/drag_and_drop/Makefile \
  demos/programs/draw/Makefile \
  demos/programs/earth/Makefile \
  demos/programs/filemanager/Makefile \
  demos/programs/fileview/Makefile \
  demos/programs/getsubres/Makefile \
  demos/programs/hellomotif/Makefile \
  demos/programs/hellomotifi18n/Makefile \
  demos/programs/hellomotifi18n/C/Makefile \
  demos/programs/hellomotifi18n/C/uid/Makefile \
  demos/programs/hellomotifi18n/english/Makefile \
  demos/programs/hellomotifi18n/english/uid/Makefile \
  demos/programs/hellomotifi18n/french/Makefile \
  demos/programs/hellomotifi18n/french/uid/Makefile \
  demos/programs/hellomotifi18n/hebrew/Makefile \
  demos/programs/hellomotifi18n/hebrew/uid/Makefile \
  demos/programs/hellomotifi18n/japan/Makefile \
  demos/programs/hellomotifi18n/japan/uid/Makefile \
  demos/programs/hellomotifi18n/japanese/Makefile \
  demos/programs/hellomotifi18n/japanese/uid/Makefile \
  demos/programs/hellomotifi18n/swedish/Makefile \
  demos/programs/hellomotifi18n/swedish/uid/Makefile \
  demos/programs/i18ninput/Makefile \
  demos/programs/panner/Makefile \
  demos/programs/periodic/Makefile \
  demos/programs/piano/Makefile \
  demos/programs/popups/Makefile \
  demos/programs/sampler2_0/Makefile \
  demos/programs/setdate/Makefile \
  demos/programs/todo/Makefile \
  demos/programs/workspace/Makefile \
  demos/programs/tooltips/Makefile \
  demos/programs/FontSel/Makefile \
  demos/programs/ButtonBox/Makefile \
  demos/programs/ColorSel/Makefile \
  demos/programs/Column/Makefile \
  demos/programs/Combo2/Makefile \
  demos/programs/Ext18List/Makefile \
  demos/programs/Ext18List/pixmaps/Makefile \
  demos/programs/IconB/Makefile \
  demos/programs/Outline/Makefile \
  demos/programs/Paned/Makefile \
  demos/programs/TabStack/Makefile \
  demos/programs/Tree/Makefile \
  demos/programs/pixmaps/Makefile \
  demos/unsupported/Makefile \
  demos/unsupported/Exm/Makefile \
  demos/unsupported/aicon/Makefile \
  demos/unsupported/dainput/Makefile \
  demos/unsupported/dogs/Makefile \
  demos/unsupported/hellomotif/Makefile \
  demos/unsupported/motifshell/Makefile \
  demos/unsupported/uilsymdump/Makefile \
  demos/unsupported/xmapdef/Makefile \
  demos/unsupported/xmfonts/Makefile \
  demos/unsupported/xmforc/Makefile \
  demos/unsupported/xmform/Makefile \
  demos/doc/Makefile \
  demos/doc/programGuide/Makefile \
  demos/doc/programGuide/ch05/Makefile \
  demos/doc/programGuide/ch05/Scale/Makefile \
  demos/doc/programGuide/ch06/Makefile \
  demos/doc/programGuide/ch06/spin_box/Makefile \
  demos/doc/programGuide/ch06/combo_box/Makefile \
  demos/doc/programGuide/ch08/Makefile \
  demos/doc/programGuide/ch08/Notebook/Makefile \
  demos/doc/programGuide/ch08/Container/Makefile \
  demos/doc/programGuide/ch16/Makefile \
  demos/doc/programGuide/ch17/Makefile \
  demos/doc/programGuide/ch17/simple_drop/Makefile \
  demos/doc/programGuide/ch17/simple_drag/Makefile \
  ])
  MAYBE_DEMOS=demos
else
  MAYBE_DEMOS=
fi
AC_SUBST(MAYBE_DEMOS)

AC_OUTPUT

echo ""
echo "The following optional features will be used:"
echo "  Enable message catalog:    $enable_message_catalog"
echo "  Enable demos:              $enable_demos"
# echo "  Enable themes:             $enable_themes"
# echo "  Enable themes debugging:   $enable_debug_themes"
echo "  Enable UTF-8 support:      $enable_utf8"
echo "  Enable XFT support:        $enable_xft"
echo "  Enable printing support:   $enable_printing"
echo ""