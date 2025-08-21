# XmString Tests - Converted from imake to autotools
# Tests for XmString and XmFontList functionality

# Test programs to build
check_PROGRAMS = FontListC FontListA StringComp StringCopy StringEmpty StringGLR \
                 StringConcat StringDisp StringDraw StringExt StringNext \
                 StringParse StringRend StringTab StringToByte StringToCT \
                 StringWidth RendToProp

# Common convenience source used by multiple tests
convenience_SOURCES = convenience.c

# FontList tests
FontListC_SOURCES = FontListC.c convenience.c
FontListA_SOURCES = FontListA.c convenience.c

# String operation tests
StringComp_SOURCES = StringComp.c convenience.c
StringCopy_SOURCES = StringCopy.c convenience.c
StringEmpty_SOURCES = StringEmpty.c convenience.c
StringGLR_SOURCES = StringGLR.c convenience.c

# Complex string tests
StringConcat_SOURCES = StringConcat.c convenience.c
StringDisp_SOURCES = StringDisp.c convenience.c
StringDraw_SOURCES = StringDraw.c convenience.c
StringExt_SOURCES = StringExt.c convenience.c
StringNext_SOURCES = StringNext.c convenience.c
StringParse_SOURCES = StringParse.c convenience.c
StringRend_SOURCES = StringRend.c convenience.c
StringTab_SOURCES = StringTab.c convenience.c
StringToByte_SOURCES = StringToByte.c convenience.c
StringToCT_SOURCES = StringToCT.c convenience.c
StringWidth_SOURCES = StringWidth.c convenience.c
RendToProp_SOURCES = RendToProp.c convenience.c

# Common compiler flags for all tests
AM_CPPFLAGS = -I. -I$(top_srcdir)/tests/auto/lib/common \
               -I$(top_srcdir)/tests/auto/lib/Create \
               -I$(top_srcdir)/tests/auto/lib/Util \
               -I$(top_srcdir)/tests/auto/lib/Synth \
               -I$(top_srcdir)/tests/auto/lib/Visual \
               -I$(top_srcdir)/tests/auto/lib/Script \
               -I$(top_srcdir)/lib -I$(top_srcdir)/include

# Common linker flags for all tests
AM_LDFLAGS = -L$(top_builddir)/lib

# Common libraries for all tests
LDADD = -lXm -lXt -lX11

# Test data files
EXTRA_DIST = *.dat *.scr *.h README

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 