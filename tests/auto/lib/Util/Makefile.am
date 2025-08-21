# Utility library for Motif tests
# Converted from imake to autotools

noinst_LTLIBRARIES = libautil.la

libautil_la_SOURCES = \
	AutoByteSwap.c \
	AutoMessages.c

libautil_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/Util \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_srcdir)/tests/auto/lib/Script \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	$(X_CFLAGS)

libautil_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# Headers
noinst_HEADERS = AutoMessages.h

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 