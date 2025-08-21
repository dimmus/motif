# Resources library for Motif tests
# Converted from imake to autotools

noinst_LTLIBRARIES = libresources.la

libresources_la_SOURCES = \
	mvsCmpRscs.c \
	mvsConToType.c \
	mvsCpRep1Arg.c \
	mvsCpRepArgs.c \
	mvsCpRsc.c \
	mvsFreeRsc.c \
	mvsFreeRscs.c \
	mvsGetLcRsc.c \
	mvsGetRscNum.c \
	mvsGetRscVal.c \
	mvsIntRes.c \
	mvsLoadRscs.c \
	mvsRscVToStr.c \
	mvsSetLocCon.c \
	mvsSetLocVal.c \
	mvsSetRscDef.c \
	mvsSetUpGlob.c \
	mvsSetValNCk.c \
	mvsSetValues.c \
	mvsUtils.c

libresources_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/Create \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_srcdir)/tests/auto/lib/Synth \
	-I$(top_srcdir)/tests/auto/lib/Visual \
	-I$(top_srcdir)/tests/auto/lib/Util \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	$(X_CFLAGS)

libresources_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 