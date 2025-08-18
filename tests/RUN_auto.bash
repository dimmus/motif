#!/bin/bash

# Copyright (c) 1987-2012, The Open Group. All rights reserved.
#
# Motif
#
# These libraries and programs are free software; you can
# redistribute them and/or modify them under the terms of the GNU
# Lesser General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# These libraries and programs are distributed in the hope that
# they will be useful, but WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with these librararies and programs; if not, write
# to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301 USA

# This script runs all the automated QATS tests, as defined in the file
# ./tests/General/dirsAuto

unalias cd 2>/dev/null || true
unalias pushd 2>/dev/null || true
unalias popd 2>/dev/null || true

dirs_file="../tests/General/dirsAuto"
if [[ ! -f "$dirs_file" ]]; then
	echo "Dirs file not found: $dirs_file" >&2
	exit 1
fi

readarray -t DIR_LIST < "$dirs_file"
TESTS_HOME="$PWD"

echo "${#DIR_LIST[@]} directories of automated tests to be run..."

export LIBRARY_PATH="${LIBRARY_PATH}:/usr/lib:/usr/local/lib:${TESTS_HOME}/Manual/lib/Common:${TESTS_HOME}/../lib/Xm/.libs:${TESTS_HOME}/Auto/lib/Common:${TESTS_HOME}/Auto/lib/Create:${TESTS_HOME}/Auto/lib/Script:${TESTS_HOME}/Auto/lib/Synth:${TESTS_HOME}/Auto/lib/Util:${TESTS_HOME}/Auto/lib/Visual:"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/lib:/usr/local/lib:${TESTS_HOME}/Manual/lib/Common:${TESTS_HOME}/../lib/Xm/.libs:${TESTS_HOME}/Auto/lib/Common:${TESTS_HOME}/Auto/lib/Create:${TESTS_HOME}/Auto/lib/Script:${TESTS_HOME}/Auto/lib/Synth:${TESTS_HOME}/Auto/lib/Util:${TESTS_HOME}/Auto/lib/Visual:"
export C_INCLUDE_PATH="${TESTS_HOME}/../lib"
export C_PATH="${TESTS_HOME}/../lib"
export MOTIF_HOME="${TESTS_HOME}/.."

# Go to the General dir and change permission to execute
cd "$TESTS_HOME/General/bin" && chmod +x *
cd "$TESTS_HOME/General" && chmod +x RUN_template

# Go to the lib dir and build the Motif libraries local

if [[ -d environment ]]; then
	cp -f -r environment ~/
fi

cd "$TESTS_HOME/../lib"
echo "I am in $PWD"

DIR="${PWD##*/}"
if [[ -e "make.$DIR" ]]; then
	mv -f "make.$DIR" "make.$DIR.bak"
fi
make -i &> "make.$DIR"

# Go to Auto and Manual directories and build libs
cd "$TESTS_HOME" && ./BUILD_libs

# Go to each automated test directory

for dir in "${DIR_LIST[@]}"; do
	cd "$TESTS_HOME/../$dir"
	DIR="${PWD##*/}"
	echo "I am in $PWD"

	# Make all the automated tests and supporting files in this directory

	if [[ -e "make.$DIR" ]]; then
		mv -f "make.$DIR" "make.$DIR.bak"
	fi
	imake -DUseInstalled -DAUTOMATION -I"$TESTS_HOME/../config/cf" -TMotif.tmpl -TImake.tmpl -DTOPDIR="$TESTS_HOME/.."
	make Makefiles
	make -i
	# Use the generated RUN script to run all of the tests

	if [[ -e "RUN.$DIR" ]]; then
		if [[ -e "RUN.$DIR.bak" ]]; then
			rm -f "RUN.$DIR.bak"
		fi
		mv -f "RUN.$DIR" "RUN.$DIR.bak"
	fi
	export PATH="$PATH:$PWD"
	./RUN -T -D 0 -genrep -Dir -d :0.0 &> "RUN.$DIR"

	# Clean up the directory before leaving
	make clean

done
