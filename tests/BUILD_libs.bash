#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_HOME="$SCRIPT_DIR"

DIR_LIST_AUTO=( \
Auto/lib/Common \
Auto/lib/Create/geometry \
Auto/lib/Create/instances \
Auto/lib/Create/other \
Auto/lib/Create/resources \
Auto/lib/Create/specific \
Auto/lib/Create/widgets \
Auto/lib/Create \
Auto/lib/Script \
Auto/lib/Synth/events \
Auto/lib/Synth/info \
Auto/lib/Synth/other \
Auto/lib/Synth/primitives \
Auto/lib/Synth \
Auto/lib/Util \
Auto/lib/Visual/visual \
Auto/lib/Visual/other \
Auto/lib/Visual \
Auto/lib \
Auto \
)

DIR_LIST_MANUAL=( \
Manual/lib/Common \
Manual/lib \
Manual \
)

DIR_TEXT=( \
Toolkit/Text/lib/Auto \
Toolkit/Text/lib/Manual \
Toolkit/Text/lib \
)

# Generate Manual makefiles
for dir in "${DIR_LIST_MANUAL[@]}"; do
	cd "$TESTS_HOME/$dir"
	echo "I am in $PWD"
	if [[ -e Makefile ]]; then
		mv -f Makefile Makefile.bak
	fi
	imake -DUseInstalled -DMANUAL -I"$TESTS_HOME/../config/cf" -TMotif.tmpl -TImake.tmpl -DTOPDIR="$TESTS_HOME/.."
done

cd "$TESTS_HOME/Manual" && make -i

# Generate Auto makefiles
for dir in "${DIR_LIST_AUTO[@]}"; do
	cd "$TESTS_HOME/$dir"
	echo "I am in $PWD"
	if [[ -e Makefile ]]; then
		mv -f Makefile Makefile.bak
	fi
	imake -DUseInstalled -DAUTOMATIC -I"$TESTS_HOME/../config/cf" -TMotif.tmpl -TImake.tmpl -DTOPDIR="$TESTS_HOME/.."
done

cd "$TESTS_HOME/Auto" && make -i

# Generate Text makefiles and build
for dir in "${DIR_TEXT[@]}"; do
	cd "$TESTS_HOME/$dir"
	echo "I am in $PWD"
	if [[ -e Makefile ]]; then
		mv -f Makefile Makefile.bak
	fi
	imake -DUseInstalled -DAUTOMATIC -I"$TESTS_HOME/../config/cf" -TMotif.tmpl -TImake.tmpl -DTOPDIR="$TESTS_HOME/.."
	make

done 