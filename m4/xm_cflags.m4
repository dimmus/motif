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
	dnl Only define __STDC__ if not already defined by compiler
	AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([], [[
			#ifndef __STDC__
			#error "__STDC__ not defined"
			#endif
		]])
	], [
		dnl __STDC__ is already defined by compiler
		AC_MSG_NOTICE([__STDC__ already defined by compiler])
	], [
		dnl __STDC__ not defined, so define it
		AC_DEFINE([__STDC__], [1], [Define if compiler supports ANSI C])
	])
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
		
		dnl Configure XOS locking strategy
		AC_ARG_ENABLE([xos-locking],
			[AS_HELP_STRING([--enable-xos-locking=STRATEGY], [XOS locking strategy: xlib, xt, none, or auto])],
			[enable_xos_locking=$enableval], [enable_xos_locking=auto])
		
		AS_IF([test "$enable_xos_locking" = "xlib"], [
			AC_DEFINE([XOS_USE_XLIB_LOCKING], [1], [Use Xlib-only locking])
		], [test "$enable_xos_locking" = "xt"], [
			AC_DEFINE([XOS_USE_XT_LOCKING], [1], [Use Xt-based locking])
		], [test "$enable_xos_locking" = "none"], [
			AC_DEFINE([XOS_USE_NO_LOCKING], [1], [Disable XOS locking])
		], [
			dnl Auto-detect: prefer Xt locking if available, fallback to Xlib
			AC_DEFINE([XOS_USE_XT_LOCKING], [1], [Use Xt-based locking (auto-detected)])
		])
	], [
		dnl XTHREADS not available, use single-threaded API
		AC_DEFINE([XNO_MTSAFE_API], [1], [Define if multi-thread safe API is not available])
		AC_DEFINE([XNO_MTSAFE_PWDAPI], [1], [Define if multi-thread safe password API is not available])
		AC_DEFINE([XOS_USE_NO_LOCKING], [1], [Disable XOS locking for single-threaded builds])
	])

	dnl Configure CDE/DTE integration options
	AC_ARG_ENABLE([cde-integration],
		[AS_HELP_STRING([--enable-cde-integration], [Enable CDE/DTE WorkSpace integration (WSM)])],
		[enable_cde_integration=$enableval], [enable_cde_integration=auto])
	
	AS_IF([test "$enable_cde_integration" = "yes"], [
		dnl Check for required WSM headers before enabling
		AC_CHECK_HEADERS([Dt/Action.h Dt/Service.h Dt/Wsm.h], [
			AC_DEFINE([WSM], [1], [Enable CDE/DTE integration])
		], [
			AC_MSG_WARN([WSM headers not found, disabling CDE/DTE integration])
			enable_cde_integration=no
		])
	], [test "$enable_cde_integration" = "no"], [
		dnl WSM disabled
	], [
		dnl Auto-detect: check for required WSM headers and dependencies
		AC_CHECK_HEADERS([Dt/Action.h Dt/Service.h Dt/Wsm.h], [
			dnl Check for ToolTalk dependency
			AC_CHECK_HEADER([Tt/tt_c.h], [
				AC_DEFINE([WSM], [1], [Enable CDE/DTE integration (auto-detected)])
			], [
				AC_MSG_WARN([ToolTalk headers not found, WSM may not compile properly])
				AC_DEFINE([WSM], [1], [Enable CDE/DTE integration (auto-detected with warnings)])
			])
		], [
			dnl Headers not found, WSM disabled
			AC_MSG_NOTICE([WSM headers not found, CDE/DTE integration disabled])
		])
	])
	
	dnl Configure HP Panelist support
	AC_ARG_ENABLE([panelist],
		[AS_HELP_STRING([--enable-panelist], [Enable HP Panelist front panel support])],
		[enable_panelist=$enableval], [enable_panelist=no])
	
	AS_IF([test "$enable_panelist" = "yes"], [
		AC_DEFINE([PANELIST], [1], [Enable HP Panelist support])
	])
	
	dnl Configure MWM QATS protocol support
	AC_ARG_ENABLE([mwm-qats],
		[AS_HELP_STRING([--enable-mwm-qats], [Enable MWM Quality Assurance Test Suite protocol])],
		[enable_mwm_qats=$enableval], [enable_mwm_qats=no])
	
	AS_IF([test "$enable_mwm_qats" = "yes"], [
		AC_DEFINE([MWM_QATS_PROTOCOL], [1], [Enable MWM QATS protocol support])
	])
	
	dnl Configure locale and internationalization support
	AC_ARG_ENABLE([locale-support],
		[AS_HELP_STRING([--enable-locale-support], [Enable locale and message catalog support])],
		[enable_locale_support=$enableval], [enable_locale_support=auto])
	
	AS_IF([test "$enable_locale_support" = "yes"], [
		AC_DEFINE([NL_CAT_LOCALE], [1], [Enable locale support])
	], [test "$enable_locale_support" = "no"], [
		dnl Locale support disabled
	], [
		dnl Auto-detect: check for locale support
		AC_CHECK_HEADER([nl_types.h], [
			AC_DEFINE([NL_CAT_LOCALE], [1], [Enable locale support (auto-detected)])
		])
	])
	
	dnl Configure wide character support
	AC_ARG_ENABLE([wchar-support],
		[AS_HELP_STRING([--enable-wchar-support], [Enable wide character support])],
		[enable_wchar_support=$enableval], [enable_wchar_support=auto])
	
	AS_IF([test "$enable_wchar_support" = "yes"], [
		AC_DEFINE([SUPPORTS_WCHARS], [1], [Enable wide character support])
	], [test "$enable_wchar_support" = "no"], [
		dnl Wide character support disabled
	], [
		dnl Auto-detect: check for wchar support
		AC_CHECK_HEADER([wchar.h], [
			AC_DEFINE([SUPPORTS_WCHARS], [1], [Enable wide character support (auto-detected)])
		])
	])
	
	dnl Configure local string functions
	AC_ARG_ENABLE([local-string-funcs],
		[AS_HELP_STRING([--enable-local-string-funcs], [Use local string comparison functions])],
		[enable_local_string_funcs=$enableval], [enable_local_string_funcs=no])
	
	AS_IF([test "$enable_local_string_funcs" = "yes"], [
		AC_DEFINE([LOCAL_STRCASECMP], [1], [Use local string case comparison functions])
	])

	AC_SUBST([ax_cc_gcov_command])
	AC_LANG_POP([C])
]) dnl AX_CFLAGS
