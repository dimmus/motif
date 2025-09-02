
AM_TESTS_ENVIRONMENT = env AM_TAP_AWK='$(AWK)'
LOG_DRIVER           = $(SHELL) $(top_srcdir)/tap-driver.sh
TESTS                = tests
AM_CPPFLAGS          = -I$(top_builddir)/include -I$(top_builddir)/lib $(X_CFLAGS) $(CHECK_CFLAGS)
AM_CFLAGS            = -fprofile-arcs -ftest-coverage
check_PROGRAMS       = tests
noinst_HEADERS       = suites.h
tests_SOURCES        = runner.c XmFontListEntry.c XmFontList.c Png.c Jpeg.c Svg.c Log.c LogConfig.c
tests_LDADD          = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS) $(CHECK_LIBS) -lgcov

# Auto test subdirectories
# SUBDIRS = auto environment General XmString

MAINTAINERCLEANFILES = Makefile.in *.gcda *.gcno *.gcov

