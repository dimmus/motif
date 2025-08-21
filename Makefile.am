ACLOCAL_AMFLAGS = -I m4

datadir = ${prefix}/share/Xm/doc

data_DATA = README
pkgconfig_DATA  = motif.pc
# EXTRA_DIST = LICENSE

MAINTAINERCLEANFILES = Makefile.in \
            configure libtool \
            config.guess \
            config.sub \
            config.log \
            ltconfig ltmain.sh \
            aclocal.m4 \
            install-sh \
            missing mkinstalldirs \
            compile \
            motif.pc \
            *.sw? \
            *.[oa] \
            *.l[oa] \
            *.so \
            *~ \
            .libs \
            autom4te.cache \
            acinclude.m4 \
            libtool.m4 \
            lt*.m4 \
            stamp-h* \
            xmstring.list \
            *.uid \
            *.rej \
            *.orig \
            depcomp \
            tap-driver.sh \
            test-driver \
            ylwrap \
            config/util/makestrs \
            $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz \
	        $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.bz2

SUBDIRS	= bindings \
            bitmaps \
            config \
            localized \
            lib \
            include \
            tools \
            clients \
            doc \
            tests \
            $(XM_WTH_DEMOS)

format:
	git ls-files 'lib/Xm/*.[ch]' 'lib/Xm/*.cc' | xargs clang-format -i -style=file

# Custom maintainer-clean target to remove .deps folders
maintainer-clean-local:
	@echo "Removing .deps folders..."
	@find . -name ".deps" -type d -exec rm -rf {} + 2>/dev/null || true