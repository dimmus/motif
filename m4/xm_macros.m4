dnl XM_COLOR
dnl will check if terminal supports color and if color is wanted by user.
dnl
dnl Used Variables:
dnl     WANT_COLOR: if no, forces no color output.
dnl     TERM: used to check if color should be enabled.
dnl
dnl Defined Variables:
dnl     COLOR_YES: to be used in positive/yes conditions
dnl     COLOR_NO: to be used in negative/no conditions
dnl     COLOR_OTHER: to be used to highlight some other condition
dnl     COLOR_RESET: to reset color
dnl     want_color: yes or no
AC_DEFUN([XM_COLOR],
[dnl
case "$TERM" in
   xterm|xterm-color|xterm-256color|Eterm|aterm|kterm|rxvt*|screen|gnome|interix)
      want_color="${WANT_COLOR:-yes}"
      ;;
   *)
      want_color="no"
      ;;
esac

### echo compatibility

## the BSD echo does not have the -e option (it is the default behaviour)
echo_e=
if test "`echo -e x`" = "x"; then
   echo_e=-e
fi
AC_SUBST([ECHO_E], [${echo_e}])

if test "${want_color}" = "yes"; then
   COLOR_YES=`echo $echo_e "\033@<:@1;32m"`
   COLOR_NO=`echo $echo_e "\033@<:@1;31m"`
   COLOR_OTHER=`echo $echo_e "\033@<:@1;36m"`
   COLOR_RESET=`echo $echo_e "\033@<:@0m"`
else
   COLOR_YES=""
   COLOR_NO=""
   COLOR_OTHER=""
   COLOR_RESET=""
fi
])

dnl XML_ADD_FEATURE(PKG, NAME, [VALUE])
dnl if VALUE is not specified, will use ${have_name} instead.
dnl
dnl Defined Variables:
dnl     features_pkg
AC_DEFUN([XM_ADD_FEATURE],
[dnl
AC_REQUIRE([XM_COLOR])dnl
m4_pushdef([DOWNPKG], m4_translit([$1], [-A-Z], [_a-z]))dnl
m4_pushdef([DOWNNAME], m4_translit([$2], [-A-Z], [_a-z]))dnl

value="m4_if($3, , [${have_]m4_defn([DOWNNAME])[:-${want_]m4_defn([DOWNNAME])[}}], [$3])"
case "${value}" in
   yes)
      tmp="${COLOR_YES}+$2${COLOR_RESET}"
      ;;
   no)
      tmp="${COLOR_NO}-$2${COLOR_RESET}"
      ;;
   *)
      tmp="${COLOR_OTHER}$2=${value}${COLOR_RESET}"
      ;;
esac
if test -z "${features_[]m4_defn([DOWNPKG])}"; then
   features_[]m4_defn([DOWNPKG])="${tmp}"
else
   features_[]m4_defn([DOWNPKG])="${features_[]m4_defn([DOWNPKG])} ${tmp}"
fi
m4_popdef([DOWNNAME])dnl
m4_popdef([DOWNPKG])dnl
])

dnl XM_XTHREADS
dnl Check if libXt was compiled with -DXTHREADS support and define XTHREADS appropriately
dnl
dnl This macro tests if the X Toolkit library supports multi-threading by checking
dnl for the presence of XtAppLock/XtAppUnlock functions and thread-safe behavior.
dnl
dnl Defined Variables:
dnl     XTHREADS: defined if libXt has thread support
dnl     have_xthreads: yes or no
AC_DEFUN([XM_XTHREADS],
[dnl
AC_REQUIRE([AC_PATH_XTRA])dnl
AC_MSG_CHECKING([if libXt was compiled with XTHREADS support])

# Save current flags
xm_save_CFLAGS="$CFLAGS"
xm_save_LIBS="$LIBS"

# Set up X flags and libraries
CFLAGS="$CFLAGS $X_CFLAGS"
LIBS="$LIBS $X_PRE_LIBS $X_LIBS -lXt -lX11 $X_EXTRA_LIBS"

# Test program to check for XTHREADS support
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
]], [[
    /* Check if XtAppLock and XtAppUnlock are available */
    XtAppContext app_context;
    XtAppLock(app_context);
    XtAppUnlock(app_context);
    
    /* These functions are only available when libXt is compiled with XTHREADS */
    return 0;
]])], [
    # Compilation succeeded - check if the functions actually work
    AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <stdio.h>
]], [[
    /* Try to initialize Xt and test thread functions */
    XtToolkitInitialize();
    XtAppContext app_context = XtCreateApplicationContext();
    
    if (app_context != NULL) {
        /* If these functions exist and don't crash, XTHREADS is supported */
        XtAppLock(app_context);
        XtAppUnlock(app_context);
        XtDestroyApplicationContext(app_context);
        return 0;  /* Success */
    }
    return 1;  /* Failure */
]])], [
        have_xthreads=yes
    ], [
        have_xthreads=no
    ], [
        # Cross-compilation case - assume no XTHREADS support
        have_xthreads=no
    ])
], [
    # Compilation failed - no XTHREADS support
    have_xthreads=no
])

# Restore original flags
CFLAGS="$xm_save_CFLAGS"
LIBS="$xm_save_LIBS"

# Define XTHREADS if support is available
if test "$have_xthreads" = "yes"; then
    AC_DEFINE([XTHREADS], [1], [Define if libXt was compiled with thread support])
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi

# Make the result available to other macros
AM_CONDITIONAL([HAVE_XTHREADS], [test "$have_xthreads" = "yes"])
])