dnl SYNOPSIS
dnl
dnl AX_CFLAGS - Set compiler flags
dnl
dnl DESCRIPTION
dnl
dnl This macro checks a number of warning options for GCC and
dnl Clang, and adds supoprted options to CFLAGS. -Werror doesn't
dnl get added here, simply because having -Werror and -Wstrict-prototypes
dnl will cause compilations from autoconf to fail.
dnl
dnl COVERAGE
dnl
dnl This macro exports ax_cc_gcov_command as the proper gcov command
dnl to invoke (i.e. llvm-cov gcov on clang.)
dnl

AC_DEFUN([XM_CFLAGS],[
	AX_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])
	AX_REQUIRE_DEFINED([AX_CHECK_COMPILE_FLAG])
	AC_LANG_PUSH([C])

	dnl Clang uses -Q, GCC doesn't support it.
	AX_CHECK_COMPILE_FLAG([-Werror -Qunused-arguments],dnl
	                      [ax_cc_clang="yes"],dnl
	                      [ax_cc_clang="no"])

	AS_IF([test "$ax_cc_clang" == "yes"],[
		ax_cc_gcov_command="llvm-cov gcov"
		AX_APPEND_COMPILE_FLAGS([ dnl
			-Wall dnl
			-Wno-unused dnl
			-Wno-comment dnl
			-Wno-unused-result dnl
			-Wno-misleading-indentation dnl
			-Wno-implicit-fallthrough dnl
			-Wno-int-in-bool-context dnl
			-Wno-deprecated-non-prototype dnl
			-Wno-padded dnl
			-Wno-switch-enum dnl
			-Wno-disabled-macro-expansion dnl
			-Wno-format-nonliteral dnl
			-Wno-long-long dnl
			-Wno-format dnl
			-Wno-extra-semi-stmt dnl
			-Wno-documentation dnl
			-Qunused-arguments dnl
		])
	],[	dnl Assume GCC
		ax_cc_gcov_command="gcov"
		
		dnl Add GCC-specific flags
		AX_APPEND_COMPILE_FLAGS([-fno-strict-aliasing])
		
		dnl Check GCC version and add version-specific flags
		AS_IF([test x$GCC = xyes], [
			dnl Check if GCC version is 4.x
			AS_IF([test `$CC -dumpversion | sed -e 's/\(^.\).*/\1/'` = "4"], [
				AX_APPEND_COMPILE_FLAGS([-fno-tree-ter])
			])
		])
		
		AX_APPEND_COMPILE_FLAGS([ dnl
			-Wall dnl
			-Wshadow dnl
			-Wwrite-strings dnl
			-Wcast-align dnl
			-Wformat-security dnl
			-Wno-unused dnl
			-Wno-comment dnl
			-Wno-unused-result dnl
			-Wno-misleading-indentation dnl
			-Wno-implicit-fallthrough dnl
			-Wno-int-in-bool-context dnl
			-Wno-conversion dnl
			-Wno-discarded-qualifiers dnl
			-Wno-strict-prototypes dnl
			-Wno-missing-prototypes dnl
			-Wno-missing-declarations dnl
			-Wno-nested-externs dnl
			-Wno-cast-qual dnl
			-Wno-redundant-decls dnl
			-Wno-bad-function-cast dnl
			-Wno-pointer-to-int-cast dnl
			-Wno-int-to-pointer-cast dnl
		])
	])

	dnl Add host-specific flags for different operating systems
	case "$host_os" in
		freebsd*)
			dnl FreeBSD specific flags
			AX_APPEND_COMPILE_FLAGS([-DCSRG_BASED -DXNO_MTSAFE_API -DXNO_MTSAFE_PWDAPI])
			dnl Add library path for FreeBSD
			AS_IF([test -d "/usr/local/lib"], [
				LDFLAGS="$LDFLAGS -L/usr/local/lib"
			])
			;;
		netbsd*|openbsd*)
			dnl NetBSD and OpenBSD specific flags
			AX_APPEND_COMPILE_FLAGS([-DCSRG_BASED])
			;;
		cygwin*)
			dnl Cygwin specific flags
			AX_APPEND_COMPILE_FLAGS([-DXNO_MTSAFE_DIRENTDAPI])
			;;
		solaris*|sunos*)
			dnl Solaris/SunOS specific flags
			AS_IF([test -d "/usr/X/lib"], [
				X_LIBS="$X_LIBS -L/usr/X/lib"
			])
			;;
		darwin*)
			dnl macOS specific flags
			AX_APPEND_COMPILE_FLAGS([-D_DARWIN_C_SOURCE])
			;;
		*)
			dnl Default case - no additional flags
			;;
	esac

	dnl Check for pwd.h availability and configure X_INCLUDE_PWD_H
	AC_CHECK_HEADER([pwd.h], [
		AC_DEFINE([X_INCLUDE_PWD_H], [1], [Define if pwd.h should be included])
	], [
		dnl pwd.h not available, don't define X_INCLUDE_PWD_H
	])

	dnl Check for POSIX compliance
	AC_CHECK_HEADER([unistd.h], [
		dnl Modern systems are POSIX compliant
		AC_DEFINE([_POSIX_SOURCE], [1], [Define for POSIX compliance])
	], [
		dnl Very old systems might not have unistd.h
		AC_DEFINE([X_NOT_POSIX], [1], [Define if system is not POSIX compliant])
	])

	dnl Check for ANSI C environment - modern compilers always support ANSI C
	AC_DEFINE([__STDC__], [1], [Define if compiler supports ANSI C])
	dnl X_NOT_STDC_ENV is no longer needed for modern builds

	dnl Configure multi-threading options
	AS_IF([test "$have_xthreads" = "yes"], [
		dnl XTHREADS is available, configure MTSAFE options
		AC_DEFINE([XUSE_MTSAFE_API], [1], [Define to use multi-thread safe API])
		AC_DEFINE([XUSE_MTSAFE_PWDAPI], [1], [Define to use multi-thread safe password API])
		
		dnl Check if we should force MTSAFE API usage
		AC_ARG_ENABLE([force-mtsafe],
			[AS_HELP_STRING([--enable-force-mtsafe], [Force use of multi-thread safe API])],
			[enable_force_mtsafe=$enableval], [enable_force_mtsafe=no])
		
		AS_IF([test "$enable_force_mtsafe" = "yes"], [
			AC_DEFINE([X_FORCE_USE_MTSAFE_API], [1], [Force use of multi-thread safe API])
		])
		
		dnl Configure debugging options for threading
		AC_ARG_ENABLE([xthreads-debug],
			[AS_HELP_STRING([--enable-xthreads-debug], [Enable XTHREADS debugging features])],
			[enable_xthreads_debug=$enableval], [enable_xthreads_debug=no])
		
		AS_IF([test "$enable_xthreads_debug" = "yes"], [
			AC_DEFINE([XTHREADS_WARN], [1], [Enable XTHREADS warnings])
			AC_DEFINE([XTHREADS_FILE_LINE], [1], [Enable XTHREADS file/line debugging])
		])
	], [
		dnl XTHREADS not available, use single-threaded API
		AC_DEFINE([XNO_MTSAFE_API], [1], [Define if multi-thread safe API is not available])
		AC_DEFINE([XNO_MTSAFE_PWDAPI], [1], [Define if multi-thread safe password API is not available])
	])

	AC_SUBST([ax_cc_gcov_command])
	AC_LANG_POP([C])
]) dnl AX_CFLAGS
