dnl
dnl This file provides the following macros
dnl
dnl AX_LT_LIB_XTHREADS
dnl	AX_FUNC_VOID_SPRINTF
dnl	AX_WEAK_ALIAS_SUPPORT
dnl

dnl This should be the canonical approach to check for XTHREADS.
dnl (see "man XtToolkitThreadInitialize")
AC_DEFUN([AX_LT_LIB_XTHREADS],
[AC_REQUIRE([AC_PATH_X])
AC_CACHE_CHECK(whether libXt was compiled with -DXTHREADS, lt_cv_xthreads,
[lt_save_CFLAGS="$CFLAGS"
lt_save_CPPFLAGS="$CPPFLAGS"
lt_save_LIBS="$LIBS"
LIBS="$X_LIBS -lXt $X_PRE_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <X11/Intrinsic.h>
int main() {
Boolean brc;
brc=XtToolkitThreadInitialize();
if (True==brc)
  exit(0);
else
  exit(1);
}
]])],
lt_cv_xthreads=yes,
lt_cv_xthreads=no,
lt_cv_xthreads=dunno)
CFLAGS="$lt_save_CFLAGS"
CPPFLAGS="$lt_save_CPPFLAGS"
LIBS="$lt_save_LIBS"
])
if test $lt_cv_xthreads = dunno; then
  AC_MSG_WARN(Can't check for XTHREADS if cross-compiling. Assume XTHREADS)
  AC_DEFINE(XTHREADS,1,"Has XThreads")
  AC_DEFINE(XUSE_MTSAFE_API,1,"API is MTSAFE")
fi
if test $lt_cv_xthreads = yes; then
  AC_DEFINE(XTHREADS,1,"Has XThreads")
  AC_DEFINE(XUSE_MTSAFE_API,1,"API is MTSAFE")
fi
])

dnl This test is not very reliable probably ...
AC_DEFUN([AX_FUNC_VOID_SPRINTF],
[AC_CACHE_CHECK(whether sprintf returns void, ac_cv_func_void_sprintf,
[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
int sprintf(); 
int main() { exit(sprintf(".")); }
]])],
  ac_cv_func_void_sprintf=no, ac_cv_func_void_sprintf=yes, ac_cv_func_void_sprintf=yes)])
if test $ac_cv_func_void_sprintf = no; then
  AC_DEFINE(VOID_SPRINTF,1,
  [Define if sprintf doesn't return the number of chars written])
fi
])

dnl This tests whether weak aliases are supported.
AC_DEFUN([AX_WEAK_ALIAS_SUPPORT],
[AC_CACHE_CHECK(whether weak aliases are supported, ac_cv_weak_alias_support,
[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
int get_value(int *value) {
  return *value;
}
int get_value_alias(int *high) __attribute__((__weak__,alias("get_value")));
]])],
[],
ac_cv_weak_alias_support=yes,
ac_cv_weak_alias_support=no)])
if test x$ac_cv_weak_alias_support = xyes; then
  AC_DEFINE(HAVE_WEAK_ALIASES,1,[Define if weak aliases are supported])
fi
])

dnl
dnl End of special macros.
dnl
