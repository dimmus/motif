# Create library for Motif tests
# Converted from imake to autotools

SUBDIRS = geometry instances other resources specific widgets

# Combined Create library
noinst_LTLIBRARIES = libacreate.la

libacreate_la_SOURCES = 

libacreate_la_LIBADD = \
	geometry/libgeometry.la \
	instances/libinstances.la \
	other/libother.la \
	resources/libresources.la \
	specific/libspecific.la \
	widgets/libwidgets.la \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

libacreate_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/Create \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_builddir)/lib \
	-I$(top_builddir)/include \
	$(X_CFLAGS)

# Headers
noinst_HEADERS = \
	mvs_defines.h \
	mvs_protos.h \
	mvs_types.h \
	mvscolor.h \
	mvslib.h \
	instances.h

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 