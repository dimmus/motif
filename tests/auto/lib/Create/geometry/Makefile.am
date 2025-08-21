# Geometry library for Motif tests
# Converted from imake to autotools

noinst_LTLIBRARIES = libgeometry.la

libgeometry_la_SOURCES = \
	GenObjects.c \
	GetGeometry.c \
	GetSubObjcts.c

libgeometry_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/Create \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_srcdir)/tests/auto/lib/Synth \
	-I$(top_srcdir)/tests/auto/lib/Visual \
	-I$(top_srcdir)/tests/auto/lib/Util \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include \
	$(X_CFLAGS)

libgeometry_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 