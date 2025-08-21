dnl Library version
m4_define(XM_MAJOR,   2)
m4_define(XM_MINOR,   4)
m4_define(XM_PATCH,   0)
m4_define(XM_VERSION, XM_MAJOR.XM_MINOR.XM_PATCH)

dnl Shared library version (libtool)
m4_define(ABI_CURRENT,  5)
m4_define(ABI_REVISION, 0)
m4_define(ABI_AGE,      0)
m4_define(ABI_VERSION,  ABI_CURRENT:ABI_REVISION:ABI_AGE)

AC_INIT(motif,XM_VERSION,https://github.com/dimmus/motif)
AC_PREREQ(2.70)
LT_PREREQ(2.4.6)
LT_INIT
# AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR(Makefile.am)
AC_CONFIG_SRCDIR(lib/Xm/Form.c)
AC_CONFIG_AUX_DIR(.)
AC_CONFIG_MACRO_DIRS(m4)
AM_INIT_AUTOMAKE(foreign dist-xz subdir-objects no-define silent-rules no-installinfo)
AM_SILENT_RULES(yes)
AC_LANG(C)

# C_STD="gnu99"
# AC_SUBST(C_STD)

AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PATH_PROG([PKG_CONFIG], [pkg-config])
AC_PROG_LEX(yywrap)
AC_CHECK_PROGS(YACC, byacc, yacc)

dnl Flex needs libfl
LEXLIB=
if test "x$ac_cv_prog_LEX" == "xflex"; then
	LEXLIB=-lfl
fi
AC_SUBST([LEXLIB])

# AC_CHECK_FILE(/usr/X/include/X11/X.h,
#   AC_PREFIX_DEFAULT(/usr/X),
#   AC_PREFIX_DEFAULT(/usr))
# AC_CHECK_FILE(/usr/X11R7/include/X11/X.h,
#   AC_PREFIX_DEFAULT(/usr/X11R7),
#   AC_PREFIX_DEFAULT(/usr))
# AC_CHECK_FILE(/usr/include/X11/X.h,
#   AC_PREFIX_DEFAULT(/usr))

# AC_PATH_X
# AC_PATH_XTRA
AX_LT_LIB_XTHREADS

AM_MAINTAINER_MODE

dnl Check the host system type and compiler characteristics
AC_CANONICAL_HOST
AC_C_CONST
# AC_TYPE_MODE_T
# AC_TYPE_OFF_T
# AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T
AC_CHECK_TYPE(XICProc,,AC_DEFINE(NO_XICPROC, 1, XICProc isn't defined), [#include <X11/Xlib.h>])

dnl Check for X
PKG_INSTALLDIR
PKG_CHECK_MODULES([X], [x11 xpm xt xmu xext])

# save_LIBS="$LIBS"
# save_CFLAGS="$CFLAGS"

# LIBS="$X_LIBS $LIBS"
# CFLAGS="$X_CFLAGS $CFLAGS"

dnl lib/Xm/VendorS.c and lib/Xm/EditresCom.c
AC_CHECK_HEADERS(X11/Xmu/Editres.h,
  AC_CHECK_LIB(Xmu, _XEditResCheckMessages,
    XMU_LIBS=-lXmu
    AC_DEFINE(HAVE_LIBXMU, 1, This System has libXmu)
    , XMU_LIBS="", -lXt -lX11)
  AC_CHECK_LIB(Xmu, XmuNCopyISOLatin1Lowered,
    AC_DEFINE(HAVE_XMU_N_COPY_ISO,1,Have XmuNCopyISOLatin1Lowered)
    , , -lXt -lX11)
  ,,[[#include <X11/Intrinsic.h>]]
)
AC_SUBST(XMU_LIBS)

# AC_HEADER_DIRENT
# AC_CHECK_INCLUDES_DEFAULT

# AC_HEADER_SYS_WAIT
AC_CHECK_HEADER(wctype.h,    , AC_DEFINE(HAS_WIDECHAR_FUNCTIONS, 1, System supports wchar))
AC_CHECK_HEADER(langinfo.h,  , AC_DEFINE(CSRG_BASED, 1, System Has langinfo.h))
AC_CHECK_HEADER(X11/Xos_r.h, , AC_DEFINE(NEED_XOS_R_H, 1, System Missing Xos_r.h))
AC_CHECK_HEADER(X11/Xpoll.h, , AC_DEFINE(NEED_XPOLL_H, 1, system Missing X11/Xpoll.h))
AC_CHECK_HEADERS(sys/malloc.h sys/file.h sys/poll.h sys/time.h X11/Xos_r.h X11/Xpoll.h)
AC_CHECK_HEADERS(alloca.h iconv.h fcntl.h limits.h malloc.h wchar.h)

# AC_STRUCT_TM
# AC_TYPE_UID_T
# AX_WEAK_ALIAS_SUPPORT

# AC_FUNC_ALLOCA
# AC_FUNC_MEMCMP
# AC_FUNC_SETPGRP
# AC_FUNC_STRCOLL
# AC_FUNC_STRFTIME
# AC_FUNC_FORK([])
# AC_FUNC_VPRINTF
# AX_FUNC_VOID_SPRINTF
AC_CHECK_FUNCS(strdup,    ,AC_DEFINE(NEED_STRDUP,     1, no strdup))
AC_CHECK_FUNCS(strcasecmp,,AC_DEFINE(NEED_STRCASECMP, 1, no strcasecmp))
AC_CHECK_FUNCS(putenv,    ,AC_DEFINE(NO_PUTENV,       1, no putenv))
AC_CHECK_FUNCS(regcomp,   ,AC_DEFINE(NO_REGCOMP,      1, no regcmp))
AC_CHECK_FUNCS(memmove,   ,AC_DEFINE(NO_MEMMOVE,      1, no memmove))
AC_CHECK_FUNCS(getcwd getwd iconv nanosleep regex strlcat \
               wcslen wcscpy wcsncpy wcschr wcscat wcsncat \
               gettimeofday mkdir re_comp regcmp select \
               strcspn strerror strstr strtod strtol uname)

if test x$GCC = xyes; then
    CFLAGS="$CFLAGS -fno-strict-aliasing"
    # CFLAGS="$CFLAGS -std=$C_STD"
    if test ` $CC -dumpversion | sed -e 's/\(^.\).*/\1/'` = "4" ; then
        CFLAGS="$CFLAGS -fno-tree-ter"
    fi
fi

AC_ARG_WITH([jpeg],
	[AS_HELP_STRING([--with-jpeg], [Enable jpeg support])],
	[with_jpeg=$withval], [with_jpeg=yes])

AC_ARG_WITH([png],
	[AS_HELP_STRING([--with-png], [Enable png support])],
	[with_png=$withval], [with_png=yes])

AS_IF([test "x$with_jpeg" == "xyes"],[PKG_CHECK_MODULES([JPEG], [libjpeg])])
AS_IF([test "x$with_png"  == "xyes"],[PKG_CHECK_MODULES([PNG],  [libpng])])

# if test "$enable_utf8" = "yes"
# then
#   AC_DEFINE([UTF8_SUPPORTED], [1], [UTF8 is supported])
#   AC_SEARCH_LIBS([iconv_open], [iconv], ,
#     AC_SEARCH_LIBS([libiconv_open], [iconv]))
# fi

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING(
      [--enable-debug],
      [enable debugging support])
	], [enable_debug=$enableval], [enable_debug=no]
)

AC_ARG_ENABLE([demos],
  [AS_HELP_STRING(
      [--disable-demos],
      [disable building demos])
	], [enable_demos=$enableval], [enable_demos=yes]
)

AC_ARG_ENABLE([utf8],
  [AS_HELP_STRING(
      [--enable-utf8],
      [enable UTF-8 support])
	], [enable_utf8=$enableval], [enable_utf8=yes]
)

AC_ARG_ENABLE([message-catalog],
  [AS_HELP_STRING(
      [--enable-message-catalog],
      [enable X/Open message catalog support])
  ],
  [enable_message_catalog=$enableval], [enable_message_catalog=no]
)

AC_ARG_ENABLE([printing],
	[AS_HELP_STRING(
		[--enable-printing],
		[enable printing support])
	], [enable_printing=$enableval], [enable_printing=no]
)

AS_IF([test "x$enable_printing" == "xyes"],[
	PKG_CHECK_MODULES([XP], [xp], [], [enable_printing=no])
	AS_IF([test "x$enable_printing" == "xno"], [AC_MSG_FAILURE([libXp not found])])
])

# case "$host_os" in
#     freebsd*) CFLAGS="$CFLAGS -DCSRG_BASED -DXNO_MTSAFE_API -DXNO_MTSAFE_PWDAPI";
#         LDFLAGS="$LDFLAGS -L/usr/local/lib";;
#     cygwin*) CFLAGS="$CFLAGS -DXNO_MTSAFE_DIRENTDAPI";;
#     solaris* | sunos*) X_LIBS="$X_LIBS -L/usr/X/lib" &&
#         if test x$enable_utf8 = x
#         then
#             enable_utf8="no"
#         fi;
#         if test x$enable_xft = x
#         then
#             enable_xft="no"
#         fi;;
# esac

AX_FIND_XFT
# AC_IMAGE_SUPPORT

dnl Debug builds (assert, etc.)
# AS_IF([test "x$enable_debug" == "xyes"],[
# 	AX_APPEND_COMPILE_FLAGS([-Og -ggdb])
# 	AX_APPEND_COMPILE_FLAGS([-DDEBUG], [CPPFLAGS])
# ],[
# 	AX_APPEND_COMPILE_FLAGS([-O2])
# 	AX_APPEND_COMPILE_FLAGS([-DNDEBUG], [CPPFLAGS])
# ])

dnl Compiler flags
XM_CFLAGS

dnl .pc
AC_SUBST([XM_VERSION], [XM_VERSION])

dnl Automake substitutions
AC_SUBST([ABI_VERSION], [ABI_VERSION])
AM_CONDITIONAL(MessageCatalog, test "x$enable_message_catalog" = "xyes")
AM_CONDITIONAL(XM_PRINTING,  test "x$enable_printing" == "xyes")
AM_CONDITIONAL(XM_WITH_XMU,  test "x$XMU_LIBS" != "x")
AM_CONDITIONAL(XM_WITH_JPEG, test "x$with_jpeg" == "xyes")
AM_CONDITIONAL(XM_WITH_PNG,  test "x$with_png"  == "xyes")
AM_CONDITIONAL(XM_MSGCAT,    test "x$enable_message_catalog" == "xyes")

dnl Substitutions for lib/Xm/Xm.h
AC_SUBST([XM_MAJOR], [XM_MAJOR])
AC_SUBST([XM_MINOR], [XM_MINOR])
AC_SUBST([XM_PATCH], [XM_PATCH])

AC_DEFINE_UNQUOTED(XM_PRINTING,
	$(test "x$enable_printing" != "xyes"; echo $?),
	[1 if Motif was built with printing support]
)

AC_DEFINE_UNQUOTED(XM_WITH_JPEG,
	$(test "x$with_jpeg" != "xyes" ; echo $?),
	[1 if Motif was built with JPEG support]
)

AC_DEFINE_UNQUOTED(XM_WITH_PNG,
	$(test "x$with_png"  != "xyes" ; echo $?),
	[1 if Motif was built with PNG support]
)

AC_DEFINE_UNQUOTED(XM_UTF8,
	$(test "x$enable_utf8" != "xyes"; echo $?),
	[1 if Motif was built with UTF8 Support]
)

AC_DEFINE_UNQUOTED(NO_MESSAGE_CATALOG,
	$(test "x$enable_message_catalog" = "xyes" ; echo $?),
	[0 if X/Open message catalog support is enabled]
)

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
lib/Xm/version.h \
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
  XM_WTH_DEMOS=demos
else
  XM_WTH_DEMOS=
fi
AC_SUBST(XM_WTH_DEMOS)

XM_ADD_FEATURE([XM],[png],[${with_png}])
XM_ADD_FEATURE([XM],[jpg],[${with_jpeg}])
XM_ADD_FEATURE([XM],[xft],[${with_xft}])
XM_ADD_FEATURE([XM],[utf8],[${enable_utf8}])
XM_ADD_FEATURE([XM],[print],[${enable_printing}])

XM_ADD_FEATURE([BUILD],[demos],[${enable_demos}])

# XM_ADD_FEATURE([DBG],[tests],[${enable_tests}])
# XM_ADD_FEATURE([DBG],[debug],[${enable_debug}])

AC_OUTPUT

echo ""
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo ""
echo "System configuration:"
echo "  os.............: ${host_os}"
echo ""
echo "Features:"
echo "  libxm..........: ${features_xm}"
echo "  debug..........: ${features_dbg}"
echo ""
echo "Components:......: ${features_build}"
echo ""
echo "Compilation......: make"
echo "  CPPFLAGS.......: $CPPFLAGS"
echo "  CFLAGS.........: $CFLAGS"
echo "  LDFLAGS........: $LIBS"
echo ""