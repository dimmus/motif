# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# This Makefile does an out-of-source CMake build in ../build_`OS`
# eg:
#   ../build_linux_x64
# This is for users who like to configure & build motif with a single command.

define HELP_TEXT

Motif Convenience Targets
   Provided for building Motif (multiple targets can be used at once).

   * debug:         Build a debug binary.
   * full:          Enable all supported dependencies & options.
   * lite:          Disable non essential features for a smaller binary and faster build.
   * release:       Complete build with all options enabled, matching the releases.
   * developer:     Enable faster builds, error checking and tests, recommended for developers.
   * ninja:         Use ninja build tool for faster builds.
   * ccache:        Use ccache for faster rebuilds.

   Note: when passing in multiple targets their order is not important.
   For example, for a fast build you can run 'make lite ccache ninja'.
   Note: passing the argument 'BUILD_DIR=path' when calling make will override the default build dir.
   Note: passing the argument 'BUILD_CMAKE_ARGS=args' lets you add cmake arguments.

Other Convenience Targets
   Provided for other building operations.

   * config:        Run cmake configuration tool to set build options.
   * deps:          Build library dependencies (intended only for platform maintainers).

Project Files
   Generate project files for development environments.

   * project_qtcreator:     QtCreator Project Files.
   * project_eclipse:       Eclipse CDT4 Project Files.

Package Targets

   * package_archive:   Build an archive package.

Testing Targets
   Not associated with building Motif.

   * test:
     Run automated tests with ctest.

Static Source Code Checking
   Not associated with building Motif.

   * check_cppcheck:
     Run motif source through cppcheck (C & C++).

     To write log files into a user defined location append 'OUTPUT_DIR',
     e.g. 'OUTPUT_DIR=/example/path'

   * check_clang_array:     Run motif source through clang array checking script (C & C++).
   * check_struct_comments: Check struct member comments are correct (C & C++).
   * check_size_comments:   Check array size comments match defines/enums (C & C++).
   * check_deprecated:      Check if there is any deprecated code to remove.
   * check_descriptions:    Check for duplicate/invalid descriptions.
   * check_licenses:        Check license headers follow the SPDX license specification.
   * check_cmake:           Runs our own cmake file checker which detects errors in the cmake file list definitions.

Documentation Checking

   * check_docs_file_structure:
     Check the documentation for the source-tree's file structure
     matches Motif's source-code.

Spell Checkers
   This runs the spell checker from the developer tools repository.

   * check_spelling_c:       Check for spelling errors (C/C++ only),
   * check_spelling_cmake:   Check for spelling errors (CMake only).

Utilities
   Not associated with building Motif.

   * source_archive:
     Create a compressed archive of the source code.

   * update:
     Update motif repository and libraries.

   * update_code:
     Updates motif repository only, without updating libraries.

   * format:
     Format source code using clang-format. For example::

        make format PATHS="lib/Xm clients"

   * license:
     Create a combined file with all the license information relative to the libraries and other
     code dependencies.

Environment Variables

   * BUILD_CMAKE_ARGS:      Arguments passed to CMake.
   * BUILD_DIR:             Override default build path.
   * PYTHON:                Use this for the Python command (used for checking tools).
   * NPROCS:                Number of processes to use building (auto-detect when omitted).

Documentation Targets
   Not associated with building Motif.

   * doc_doxy:
     Generate doxygen C/C++ docs.
   * doc_man:
     Generate manpage.

Information

   * help:              This help message.
   * help_features:     Show a list of optional features when building.

endef
# HELP_TEXT (end)

# This makefile is not meant for Windows,
# Note that a TAB indent prevents the message from showing, no indentation is intended.
ifeq ($(OS),Windows_NT)
$(error On Windows, use "cmd //c make.bat" instead of "make")
endif

# System Vars
OS:=$(shell uname -s)
OS_NCASE:=$(shell uname -s | tr '[A-Z]' '[a-z]')
CPU:=$(shell uname -m)

# Use our OS and CPU architecture naming conventions.
ifeq ($(CPU),x86_64)
	CPU:=x64
endif
ifeq ($(CPU),aarch64)
	CPU:=arm64
endif
ifeq ($(OS_NCASE),darwin)
	OS_LIBDIR:=macos
else
	OS_LIBDIR:=$(OS_NCASE)
endif


# Source and Build DIR's
MOTIF_DIR:=$(shell pwd -P)
BUILD_TYPE:=Release

# CMake arguments, assigned to local variable to make it mutable.
CMAKE_CONFIG_ARGS := $(BUILD_CMAKE_ARGS)

ifndef BUILD_DIR
	BUILD_DIR:=$(shell dirname "$(MOTIF_DIR)")/build_$(OS_NCASE)
endif

# Dependencies DIR's
DEPS_SOURCE_DIR:=$(MOTIF_DIR)/tools/cmake/build_environment

ifndef DEPS_BUILD_DIR
	DEPS_BUILD_DIR:=$(BUILD_DIR)/deps_$(CPU)
endif

ifndef DEPS_INSTALL_DIR
	DEPS_INSTALL_DIR:=$(MOTIF_DIR)/lib/$(OS_LIBDIR)_$(CPU)
endif

# Set the LIBDIR, an empty string when not found.
LIBDIR:=$(wildcard $(MOTIF_DIR)/lib/${OS_LIBDIR}_${CPU})
ifeq (, $(LIBDIR))
	LIBDIR:=$(wildcard $(MOTIF_DIR)/lib/${OS_LIBDIR})
endif

# Allow to use alternative binary (pypy3, etc)
ifndef PYTHON
	# If not overridden, first try using Python from LIBDIR.
	PYTHON:=python3
	ifeq (, $(shell command -v $(PYTHON)))
		PYTHON:=python
	endif
endif


# -----------------------------------------------------------------------------
# Additional targets for the build configuration

# NOTE: These targets can be combined and are applied in reverse order listed here.
# So it's important that `developer` comes before `release` (for example)
# `make developer release` first loads `release` configuration, then `developer`.
# This is important as `developer` will turn off some settings enabled by release.

ifneq "$(findstring debug, $(MAKECMDGOALS))" ""
	BUILD_DIR:=$(BUILD_DIR)_debug
	BUILD_TYPE:=Debug
endif
ifneq "$(findstring full, $(MAKECMDGOALS))" ""
	BUILD_DIR:=$(BUILD_DIR)_full
	CMAKE_CONFIG_ARGS:=-C"$(MOTIF_DIR)/tools/cmake/config/motif_full.cmake" $(CMAKE_CONFIG_ARGS)
endif
ifneq "$(findstring lite, $(MAKECMDGOALS))" ""
	BUILD_DIR:=$(BUILD_DIR)_lite
	CMAKE_CONFIG_ARGS:=-C"$(MOTIF_DIR)/tools/cmake/config/motif_lite.cmake" $(CMAKE_CONFIG_ARGS)
endif
ifneq "$(findstring release, $(MAKECMDGOALS))" ""
	BUILD_DIR:=$(BUILD_DIR)_release
	CMAKE_CONFIG_ARGS:=-C"$(MOTIF_DIR)/tools/cmake/config/motif_release.cmake" $(CMAKE_CONFIG_ARGS)
endif

ifneq "$(findstring developer, $(MAKECMDGOALS))" ""
	CMAKE_CONFIG_ARGS:=-C"$(MOTIF_DIR)/tools/cmake/config/motif_developer.cmake" $(CMAKE_CONFIG_ARGS)
endif

ifneq "$(findstring ccache, $(MAKECMDGOALS))" ""
	CMAKE_CONFIG_ARGS:=-DWITH_COMPILER_CCACHE=ON $(CMAKE_CONFIG_ARGS)
endif

# -----------------------------------------------------------------------------
# build tool

# Check if ninja is available
NINJA_AVAILABLE:=$(shell which ninja 2>/dev/null)

ifneq "$(findstring ninja, $(MAKECMDGOALS))" ""
	ifneq ($(NINJA_AVAILABLE),)
		CMAKE_CONFIG_ARGS:=$(CMAKE_CONFIG_ARGS) -G Ninja
		BUILD_COMMAND:=ninja
		DEPS_BUILD_COMMAND:=ninja
	else
		$(warning Ninja not found, falling back to Unix Makefiles)
		CMAKE_CONFIG_ARGS:=$(CMAKE_CONFIG_ARGS) -G "Unix Makefiles"
		BUILD_COMMAND:=make -s
		DEPS_BUILD_COMMAND:=make -s
	endif
else
	ifneq ($(NINJA_AVAILABLE),)
		ifneq ("$(wildcard $(BUILD_DIR)/build.ninja)","")
			BUILD_COMMAND:=ninja
		else
			BUILD_COMMAND:=make -s
		endif

		ifneq ("$(wildcard $(DEPS_BUILD_DIR)/build.ninja)","")
			DEPS_BUILD_COMMAND:=ninja
		else
			DEPS_BUILD_COMMAND:=make -s
		endif
	else
		BUILD_COMMAND:=make -s
		DEPS_BUILD_COMMAND:=make -s
	endif
endif

# -----------------------------------------------------------------------------
# Motif binary path

ifeq ($(OS), Darwin)
	MOTIF_BIN?="$(BUILD_DIR)/bin/Motif.app/Contents/MacOS/Motif"
	MOTIF_BIN_DIR?="$(BUILD_DIR)/bin/Motif.app/Contents/MacOS/Motif"
else
	MOTIF_BIN?="$(BUILD_DIR)/bin/motif"
	MOTIF_BIN_DIR?="$(BUILD_DIR)/bin"
endif


# -----------------------------------------------------------------------------
# Get the number of cores for threaded build
ifndef NPROCS
	NPROCS:=1
	ifeq ($(OS), Linux)
		NPROCS:=$(shell nproc)
	endif
	ifeq ($(OS), NetBSD)
		NPROCS:=$(shell getconf NPROCESSORS_ONLN)
	endif
	ifneq (,$(filter $(OS),Darwin FreeBSD))
		NPROCS:=$(shell sysctl -n hw.ncpu)
	endif
endif


# -----------------------------------------------------------------------------
# Macro for configuring cmake

CMAKE_CONFIG = cmake $(CMAKE_CONFIG_ARGS) \
                     -H"$(MOTIF_DIR)" \
                     -B"$(BUILD_DIR)" \
                     -DCMAKE_BUILD_TYPE_INIT:STRING=$(BUILD_TYPE)


# -----------------------------------------------------------------------------
# Tool for 'make config'

# X11 specific.
ifdef DISPLAY
	CMAKE_CONFIG_TOOL = cmake-gui
else
	CMAKE_CONFIG_TOOL = ccmake
endif


# -----------------------------------------------------------------------------
# Build Motif
all: .FORCE
	@echo
	@echo Configuring Motif in \"$(BUILD_DIR)\" ...

	# Check if we need to clean the build directory due to generator mismatch
	@if [ -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		CURRENT_GENERATOR=$$(grep "CMAKE_GENERATOR:" "$(BUILD_DIR)/CMakeCache.txt" | cut -d: -f2 | tr -d ' '); \
		REQUESTED_GENERATOR=""; \
		if echo "$(CMAKE_CONFIG_ARGS)" | grep -q "\-G Ninja"; then \
			REQUESTED_GENERATOR="Ninja"; \
		elif echo "$(CMAKE_CONFIG_ARGS)" | grep -q "\-G Unix Makefiles"; then \
			REQUESTED_GENERATOR="Unix Makefiles"; \
		fi; \
		if [ -n "$$REQUESTED_GENERATOR" ] && [ "$$CURRENT_GENERATOR" != "$$REQUESTED_GENERATOR" ]; then \
			echo "Generator mismatch detected. Cleaning build directory..."; \
			rm -rf "$(BUILD_DIR)/CMakeCache.txt" "$(BUILD_DIR)/CMakeFiles"; \
		fi; \
	fi

	@$(CMAKE_CONFIG)

	@echo
	@echo Building Motif ...
	$(BUILD_COMMAND) -C "$(BUILD_DIR)" -j $(NPROCS) install
	@echo
	@echo Edit build configuration with: \"$(BUILD_DIR)/CMakeCache.txt\" run make again to rebuild.
	@echo Motif successfully built, run from: $(MOTIF_BIN)
	@echo

debug: all
full: all
lite: all
release: all
developer: all
ninja: all
ccache: all

# -----------------------------------------------------------------------------
# Build dependencies
DEPS_TARGET = install
ifneq "$(findstring clean, $(MAKECMDGOALS))" ""
	DEPS_TARGET = clean
endif

# Set the SOURCE_DATE_EPOCH to make builds reproducible (locks timestamps to the specified date).
deps: export SOURCE_DATE_EPOCH = 1745584760
deps: .FORCE
	@echo
	@echo Configuring dependencies in \"$(DEPS_BUILD_DIR)\", install to \"$(DEPS_INSTALL_DIR)\"

	@cmake -H"$(DEPS_SOURCE_DIR)" \
	       -B"$(DEPS_BUILD_DIR)" \
	       -DHARVEST_TARGET=$(DEPS_INSTALL_DIR)

	@echo
	@echo Building dependencies ...
	$(DEPS_BUILD_COMMAND) -C "$(DEPS_BUILD_DIR)" -j $(NPROCS) $(DEPS_TARGET)
	@echo
	@echo Dependencies successfully built and installed to $(DEPS_INSTALL_DIR).
	@echo

# -----------------------------------------------------------------------------
# Configuration (save some cd'ing around)
config: .FORCE
	$(CMAKE_CONFIG_TOOL) "$(BUILD_DIR)"


# -----------------------------------------------------------------------------
# Help for build targets
export HELP_TEXT
help: .FORCE
	@echo "$$HELP_TEXT"

# -----------------------------------------------------------------------------
# Packages
#

package_archive: .FORCE
	make -C "$(BUILD_DIR)" -s package_archive
	@echo archive in "$(BUILD_DIR)/release"


# -----------------------------------------------------------------------------
# Tests
#
test: .FORCE
	@$(PYTHON) ./tools/cmake/utils/make_test.py "$(BUILD_DIR)"


# -----------------------------------------------------------------------------
# Project Files
#

project_qtcreator: .FORCE
	$(PYTHON) tools/utils_ide/cmake_qtcreator_project.py --build-dir "$(BUILD_DIR)"

project_eclipse: .FORCE
	cmake -G"Eclipse CDT4 - Unix Makefiles" -H"$(MOTIF_DIR)" -B"$(BUILD_DIR)"


# -----------------------------------------------------------------------------
# Static Checking
#

check_cppcheck: .FORCE
	@$(CMAKE_CONFIG)
	$(PYTHON) \
	    "$(MOTIF_DIR)/tools/check_source/static_check_cppcheck.py" \
	    --build-dir=$(BUILD_DIR) \
	    --output-dir=$(OUTPUT_DIR)

check_struct_comments: .FORCE
	@$(CMAKE_CONFIG)
	@cd "$(BUILD_DIR)" ; \
	$(PYTHON) \
	    "$(MOTIF_DIR)/tools/check_source/static_check_clang.py" \
	    --checks=struct_comments --match=".*" --jobs=$(NPROCS)

check_size_comments: .FORCE
	$(PYTHON) \
	    "$(MOTIF_DIR)/tools/check_source/static_check_size_comments.py"

check_clang_array: .FORCE
	@$(CMAKE_CONFIG)
	@cd "$(BUILD_DIR)" ; \
	$(PYTHON) "$(MOTIF_DIR)/tools/check_source/static_check_clang_array.py"

check_docs_file_structure: .FORCE
	@PYTHONIOENCODING=utf_8 $(PYTHON) \
	    "$(MOTIF_DIR)/tools/check_docs/check_docs_code_layout.py"

check_spelling_c: .FORCE
	@PYTHONIOENCODING=utf_8 $(PYTHON) \
	    "$(MOTIF_DIR)/tools/check_source/check_spelling.py" \
	    --cache-file=$(CHECK_SPELLING_CACHE) \
	    --match=".*\.(c|cc|cpp|cxx|h|hh|hpp|hxx|inl|m|mm)$$" \
	    $(CHECK_SPELLING_EXTRA_ARGS) \
	    "$(MOTIF_DIR)/lib" \
	    "$(MOTIF_DIR)/clients" \
	    "$(MOTIF_DIR)/tools"

check_spelling_cmake: .FORCE
	@PYTHONIOENCODING=utf_8 $(PYTHON) \
	    "$(MOTIF_DIR)/tools/check_source/check_spelling.py" \
	    --cache-file=$(CHECK_SPELLING_CACHE) \
	    --match=".*\.(cmake)$$" \
	    --match=".*\bCMakeLists\.(txt)$$" \
	    $(CHECK_SPELLING_EXTRA_ARGS) \
	    "$(MOTIF_DIR)/build_files/" \
	    "$(MOTIF_DIR)/lib/" \
	    "$(MOTIF_DIR)/clients/" \
	    "$(MOTIF_DIR)/CMakeLists.txt"

check_descriptions: .FORCE
	@$(MOTIF_BIN) --background --factory-startup --python \
	    "$(MOTIF_DIR)/tools/check_source/check_descriptions.py"

check_deprecated: .FORCE
	@PYTHONIOENCODING=utf_8 $(PYTHON) \
	    tools/check_source/check_deprecated.py

check_licenses: .FORCE
	@PYTHONIOENCODING=utf_8 $(PYTHON) \
	    "$(MOTIF_DIR)/tools/check_source/check_licenses.py" \
	    "--show-headers=$(SHOW_HEADERS)"

check_cmake: .FORCE
	@PYTHONIOENCODING=utf_8 $(PYTHON) \
	    tools/check_source/check_cmake_consistency.py


# -----------------------------------------------------------------------------
# Utilities
#

source_archive: .FORCE
	@$(PYTHON) ./tools/cmake/utils/make_source_archive.py

update: .FORCE
	@$(PYTHON) ./tools/cmake/utils/make_update.py

update_code: .FORCE
	@$(PYTHON) ./tools/cmake/utils/make_update.py --no-libraries

format: .FORCE
	@PATH="${LIBDIR}/llvm/bin/:$(PATH)" $(PYTHON) tools/utils_maintenance/clang_format_paths.py $(PATHS)

license: .FORCE
	@$(PYTHON) tools/utils_maintenance/make_license.py

# -----------------------------------------------------------------------------
# Documentation
#

doc_doxy: .FORCE
	@cd doc/doxygen; doxygen Doxyfile
	@echo "docs written into: '$(MOTIF_DIR)/doc/doxygen/html/index.html'"

doc_man: .FORCE
	@$(PYTHON) doc/manpage/motif.1.py --motif="$(MOTIF_BIN)" --output=motif.1 --verbose

help_features: .FORCE
	@$(PYTHON) "$(MOTIF_DIR)/tools/cmake/cmake_print_build_options.py" $(MOTIF_DIR)"/CMakeLists.txt"

clean: .FORCE
	@echo Cleaning build directory...
	@rm -rf "$(BUILD_DIR)/CMakeCache.txt" "$(BUILD_DIR)/CMakeFiles" "$(BUILD_DIR)/build.ninja"
	@echo Build directory cleaned.

clean_all: .FORCE
	@echo Cleaning entire build directory...
	@rm -rf "$(BUILD_DIR)"
	@echo Build directory completely removed.

.PHONY: all

.FORCE: