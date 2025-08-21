# Other utilities library for Motif tests
# Converted from imake to autotools

noinst_LTLIBRARIES = libother.la

libother_la_SOURCES = \
	AlwaysTrue.c \
	ExtractStrng.c \
	FindString.c \
	GetClassCode.c \
	GetClassName.c \
	GetObjName.c \
	InitRootInfo.c \
	IsParent.c \
	IsWidgMapped.c \
	msg.c \
	mvsAssertion.c \
	mvsGetRsnNam.c \
	SetFocus.c \
	WidToWidgInf.c \
	WinToWidgInf.c \
	WindowToWidg.c

libother_la_CPPFLAGS = \
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

libother_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 