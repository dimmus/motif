# SPDX-License-Identifier: LGPL-2.1-or-later
#
# Convenience wrapper around an out-of-source CMake build, for people who
# like to configure and build Motif with a single command.  Run "make help"
# for the targets.  Everything here can also be done with cmake directly:
#
#   cmake -S . -B ../build_linux -G Ninja
#   cmake --build ../build_linux
#   cmake --install ../build_linux

define HELP_TEXT

Motif convenience targets (several can be combined, e.g. "make lite ninja"):

  build        Configure and build (the default).
  debug        Debug build (in BUILD_DIR_debug).
  release      Optimised build matching the releases (preset motif_release).
  full         All optional features (preset motif_full).
  lite         Without non-essential features (preset motif_lite).
  developer    Developer settings: tests, sanitizer, ... (preset motif_developer).
  ninja        Use the Ninja generator.
  ccache       Compile through ccache.

  install      Install the build.  Use DESTDIR=... for staging; run it as a
               user that may write to the prefix (e.g. "sudo make install").
  test         Run the tests with ctest.
  config       Edit the build options (ccmake or cmake-gui).
  clean        Remove the CMake cache of BUILD_DIR.
  clean_all    Remove BUILD_DIR.
  source_archive  Create a compressed archive of the sources.
  update       Update the git checkout.
  help         Show this text.

Variables:

  BUILD_DIR         Build directory (default: ../build_<os>).
  BUILD_CMAKE_ARGS  Extra arguments for cmake, e.g. -DCMAKE_INSTALL_PREFIX=/usr.
  NPROCS            Parallel build jobs (default: number of CPUs).
  PYTHON            Python interpreter for the helper scripts.

endef

ifeq ($(OS),Windows_NT)
$(error This makefile does not support Windows)
endif

OS_NCASE := $(shell uname -s | tr '[A-Z]' '[a-z]')
MOTIF_DIR := $(shell pwd -P)

# Empty: the preset's build type, or RelWithDebInfo (see CMakeLists.txt)
BUILD_TYPE :=
CMAKE_CONFIG_ARGS := $(BUILD_CMAKE_ARGS)

ifndef BUILD_DIR
	BUILD_DIR := $(shell dirname "$(MOTIF_DIR)")/build_$(OS_NCASE)
endif

ifndef PYTHON
	PYTHON := python3
endif

ifndef NPROCS
	NPROCS := $(shell getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)
endif

# Presets, see tools/cmake/config.  They seed a new cache only.
ifneq "$(findstring debug, $(MAKECMDGOALS))" ""
	BUILD_DIR := $(BUILD_DIR)_debug
	BUILD_TYPE := Debug
endif
ifneq "$(findstring full, $(MAKECMDGOALS))" ""
	BUILD_DIR := $(BUILD_DIR)_full
	CMAKE_CONFIG_ARGS := -C"$(MOTIF_DIR)/tools/cmake/config/motif_full.cmake" $(CMAKE_CONFIG_ARGS)
endif
ifneq "$(findstring lite, $(MAKECMDGOALS))" ""
	BUILD_DIR := $(BUILD_DIR)_lite
	CMAKE_CONFIG_ARGS := -C"$(MOTIF_DIR)/tools/cmake/config/motif_lite.cmake" $(CMAKE_CONFIG_ARGS)
endif
ifneq "$(findstring release, $(MAKECMDGOALS))" ""
	BUILD_DIR := $(BUILD_DIR)_release
	CMAKE_CONFIG_ARGS := -C"$(MOTIF_DIR)/tools/cmake/config/motif_release.cmake" $(CMAKE_CONFIG_ARGS)
endif
ifneq "$(findstring developer, $(MAKECMDGOALS))" ""
	CMAKE_CONFIG_ARGS := -C"$(MOTIF_DIR)/tools/cmake/config/motif_developer.cmake" $(CMAKE_CONFIG_ARGS)
endif
ifneq "$(findstring ccache, $(MAKECMDGOALS))" ""
	CMAKE_CONFIG_ARGS := -DWITH_COMPILER_CCACHE=ON $(CMAKE_CONFIG_ARGS)
endif
ifneq "$(findstring ninja, $(MAKECMDGOALS))" ""
	CMAKE_CONFIG_ARGS := -G Ninja $(CMAKE_CONFIG_ARGS)
endif

ifdef DISPLAY
	CMAKE_CONFIG_TOOL := cmake-gui
else
	CMAKE_CONFIG_TOOL := ccmake
endif

# An existing cache keeps its build type and generator.
build: .FORCE
	@echo "Configuring Motif in \"$(BUILD_DIR)\" ..."
	@if [ -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		cmake $(BUILD_CMAKE_ARGS) -S "$(MOTIF_DIR)" -B "$(BUILD_DIR)"; \
	else \
		cmake $(CMAKE_CONFIG_ARGS) $(if $(BUILD_TYPE),-DCMAKE_BUILD_TYPE=$(BUILD_TYPE)) \
		      -S "$(MOTIF_DIR)" -B "$(BUILD_DIR)"; \
	fi
	@echo "Building Motif ..."
	cmake --build "$(BUILD_DIR)" --parallel $(NPROCS)

debug release full lite developer ninja ccache: build

all: build

install: .FORCE
	cmake --install "$(BUILD_DIR)"

test: .FORCE
	@$(PYTHON) ./tools/cmake/scripts/make_test.py "$(BUILD_DIR)"

config: .FORCE
	$(CMAKE_CONFIG_TOOL) "$(BUILD_DIR)"

clean: .FORCE
	rm -rf "$(BUILD_DIR)/CMakeCache.txt" "$(BUILD_DIR)/CMakeFiles"

clean_all: .FORCE
	rm -rf "$(BUILD_DIR)"

source_archive: .FORCE
	@$(PYTHON) ./tools/cmake/scripts/make_source_archive.py

update: .FORCE
	@$(PYTHON) ./tools/cmake/scripts/make_update.py

export HELP_TEXT
help: .FORCE
	@echo "$$HELP_TEXT"

.PHONY: all build install test config clean clean_all source_archive update help \
	debug release full lite developer ninja ccache

.FORCE:
