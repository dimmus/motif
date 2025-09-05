ACLOCAL_AMFLAGS = -I m4

datadir = ${prefix}/share/Xm/doc

# Declare phony targets
.PHONY: help format deps release release-configure release-build release-portable release-portable-configure dev-build gcov clean-gcov

data_DATA = README.md
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
            tools/makestrs/makestrs \
            *.gcda \
            *.gcno \
            *.gcov \
            *.tar.gz \
	        *.tar.xz

SUBDIRS	= data \
            localized \
            lib \
            include \
            tools \
            clients \
            doc \
            ${XM_WITH_TESTS} \
            $(XM_WITH_DEMOS)

# Help target
help:
	@echo "Available build targets:"
	@echo "  all           - Standard build (default)"
	@echo "  release       - Full optimized release build with stripping"
	@echo "  release-portable - Portable optimized release (no CPU-specific opts)"
	@echo "  release-configure - Configure for release build only"
	@echo "  release-build - Build release version (after configure)"
	@echo "  dev-build     - Development build with debug symbols"
	@echo "  clean         - Clean build artifacts"
	@echo "  distclean     - Clean everything including configuration"
	@echo "  format        - Format source code with clang-format"
	@echo "  deps          - Check dependencies"
	@echo "  install       - Install the built software"
	@echo "  uninstall     - Uninstall the software"
	@echo "  gcov          - Generate code coverage reports (requires --enable-tests)"
	@echo "  clean-gcov    - Clean up coverage files"
	@echo ""
	@echo "Release build optimizations:"
	@echo "  -O3                    - Maximum optimization"
	@echo "  -march=native          - Optimize for current CPU"
	@echo "  -mtune=native          - Tune for current CPU"
	@echo "  -flto                  - Link-time optimization"
	@echo "  -ffast-math            - Fast math operations"
	@echo "  -funroll-loops         - Unroll loops"
	@echo "  -fomit-frame-pointer   - Omit frame pointer"
	@echo "  Binary stripping       - Remove debug symbols"

format:
	git ls-files 'lib/Xm/*.[ch]' 'lib/Xm/*.cc' | xargs clang-format -i

# Dependency checking target
deps:
	@$(top_srcdir)/tools/dev/scripts/deps_check.sh

# Release build targets
release-configure:
	@echo "Configuring for optimized release build..."
	@if [ ! -f configure ]; then \
		echo "Running autoreconf to generate configure script..."; \
		autoreconf -fiv; \
	fi
	@CFLAGS="-O3 -march=native -mtune=native -flto -ffast-math -funroll-loops -fomit-frame-pointer -DNDEBUG" \
	 CXXFLAGS="-O3 -march=native -mtune=native -flto -ffast-math -funroll-loops -fomit-frame-pointer -DNDEBUG" \
	 LDFLAGS="-O3 -flto -s" \
	 ./configure --disable-dependency-tracking --enable-shared --disable-static
	@echo "Release configuration complete."

release-build:
	@echo "Building optimized release version..."
	@$(MAKE) clean
	@$(MAKE) all
	@echo "Stripping binaries..."
	@find . -path "./.attic" -prune -o -type f \( -name "*.so*" -o -perm -111 \) -exec file {} \; | \
	 grep -E "(ELF.*executable|ELF.*shared object)" | \
	 cut -d: -f1 | \
	 xargs -r strip --strip-unneeded 2>/dev/null || true
	@echo "Release build complete with stripped binaries."

release: release-configure release-build
	@echo "Full release build completed successfully!"
	@echo "Optimized and stripped binaries are ready for distribution."

# Portable release build (without CPU-specific optimizations)
release-portable-configure:
	@echo "Configuring for portable optimized release build..."
	@if [ ! -f configure ]; then \
		echo "Running autoreconf to generate configure script..."; \
		autoreconf -fiv; \
	fi
	@CFLAGS="-O3 -flto -ffast-math -funroll-loops -fomit-frame-pointer -DNDEBUG" \
	 CXXFLAGS="-O3 -flto -ffast-math -funroll-loops -fomit-frame-pointer -DNDEBUG" \
	 LDFLAGS="-O3 -flto -s" \
	 ./configure --disable-dependency-tracking --enable-shared --disable-static
	@echo "Portable release configuration complete."

release-portable: release-portable-configure release-build
	@echo "Portable release build completed successfully!"
	@echo "Optimized and stripped binaries are ready for distribution."

# Development build (default optimization)
dev-build:
	@echo "Building development version..."
	@if [ ! -f configure ]; then \
		echo "Running autoreconf to generate configure script..."; \
		autoreconf -fiv; \
	fi
	@./configure --enable-debug
	@$(MAKE) clean
	@$(MAKE) all
	@echo "Development build complete."

# Custom maintainer-clean target
maintainer-clean-local:
	@echo "Removing .deps folders..."
	@find . -name ".deps" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "Removing .libs folders..."
	@find . -name ".libs" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "Removing Makefile.in files..."
	@find . -name "Makefile.in" -type f -exec rm -rf {} + 2>/dev/null || true
	@echo "Removing coverage files..."
	@find . -name "*.gcda" -delete 2>/dev/null || true
	@find . -name "*.gcno" -delete 2>/dev/null || true
	@find . -name "*.gcov" -delete 2>/dev/null || true

# Gcov code coverage target (only available when tests are enabled)
if XM_WITH_TESTS
gcov:
	@echo "Running code coverage analysis with $(ax_cc_gcov_command)..."
	@if [ ! -f tests/tests ]; then \
		echo "Error: Test executable not found. Please run 'make check' first."; \
		exit 1; \
	fi
	@echo "Running tests to generate coverage data..."
	@cd tests && ./tests
	@echo "Generating coverage reports..."
	@find . -name "*.gcda" -exec $(ax_cc_gcov_command) {} \; 2>/dev/null || true
	@echo "Coverage analysis complete. Check *.gcov files for results."
	@echo "Note: For HTML reports, consider using lcov or gcovr tools."

clean-gcov:
	@echo "Cleaning up coverage files..."
	@find . -name "*.gcda" -delete 2>/dev/null || true
	@find . -name "*.gcno" -delete 2>/dev/null || true
	@find . -name "*.gcov" -delete 2>/dev/null || true
	@echo "Coverage files cleaned up."
else
gcov:
	@echo "Code coverage analysis is only available when tests are enabled."
	@echo "Please configure with --enable-tests and run 'make check' first."

clean-gcov:
	@echo "Code coverage is not enabled."
endif