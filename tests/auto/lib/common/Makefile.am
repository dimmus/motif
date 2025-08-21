# Common test library for Motif tests
# Converted from imake to autotools

noinst_LTLIBRARIES = libacommon.la libacommonuil.la

# Main common library
libacommon_la_SOURCES = \
	Callbacks.c \
	Color.c \
	CommandLine.c \
	Conversion.c \
	DumpHier.c \
	Font.c \
	Pause.c \
	TestMalloc.c \
	ExtraArgs.c \
	TestInit.c \
	TestMemory.c

libacommon_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_srcdir)/tests/auto/lib/Create \
	-I$(top_srcdir)/tests/auto/lib/Util \
	-I$(top_srcdir)/tests/auto/lib/Synth \
	-I$(top_srcdir)/tests/auto/lib/Visual \
	-I$(top_srcdir)/tests/auto/lib/Script \
	-I$(top_srcdir)/tests/auto/lib/create \
	-I$(top_srcdir)/tests/auto/lib/synth \
	-I$(top_srcdir)/tests/auto/lib/visual \
	-I$(top_srcdir)/tests/auto/lib/util \
	-I$(top_builddir)/lib \
	-I$(top_builddir)/include \
	$(X_CFLAGS)

libacommon_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# UIL-specific common library (without Mrm dependencies)
libacommonuil_la_SOURCES = TestUilInit.c

libacommonuil_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_srcdir)/tests/auto/lib/Create \
	-I$(top_srcdir)/tests/auto/lib/Util \
	-I$(top_srcdir)/tests/auto/lib/Synth \
	-I$(top_builddir)/lib \
	-I$(top_builddir)/include \
	$(X_CFLAGS)

libacommonuil_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# Headers
noinst_HEADERS = testlib.h

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 