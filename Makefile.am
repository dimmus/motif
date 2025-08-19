ACLOCAL_AMFLAGS = -I m4

datadir = ${prefix}/share/Xm/doc

data_DATA = README
pkgconfig_DATA  = motif.pc
EXTRA_DIST = LICENSE

MAINTAINERCLEANFILES = Makefile.in \
            configure libtool \
            config.guess \
            config.sub \
            config.log \
            depcomp \
            ltconfig ltmain.sh \
            aclocal.m4 \
            config.h.in \
            install-sh \
            missing mkinstalldirs \
            ylwrap \
            compile \
            XmStrDefs.c \
            m4/lt*.m4 \
            m4/libtool.m4 \
            build-aux/* \
            *~

SUBDIRS	= bindings \
            bitmaps \
            config \
            localized \
            lib \
            include \
            tools \
            clients \
            doc \
            $(MAYBE_DEMOS)

format:
	git ls-files 'lib/Xm/*.[ch]' 'lib/Xm/*.cc' | xargs clang-format -i -style=file