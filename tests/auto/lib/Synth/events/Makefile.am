# Synth events library for Motif tests
# Converted from imake to autotools

noinst_LTLIBRARIES = libsynthevents.la

libsynthevents_la_SOURCES = \
	GetEventTime.c \
	GetServrTime.c \
	MovePointer.c \
	PrintEvent.c \
	ProcesEvents.c \
	SendEvent.c \
	Synchronize.c

libsynthevents_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/Synth \
	-I$(top_srcdir)/tests/auto/lib/Util \
	-I$(top_srcdir)/tests/auto/lib/Create \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	$(X_CFLAGS)

libsynthevents_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 