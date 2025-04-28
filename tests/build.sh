#!/bin/sh

TESTS_HOME=$pwd

DIR_LIST_MANUAL='Manual/lib/Common \
Manual/lib \
Manual'

for dir in $DIR_LIST_MANUAL; do
    cd $TESTS_HOME/$dir
    echo "I am in " $pwd

	if (-e Makefile) then
		mv -f  Makefile Makefile.bak
	endif
	imake -DUseInstalled -DMANUAL -I$TESTS_HOME/../config/cf -TMotif.tmpl -TImake.tmpl -DTOPDIR=$TESTS_HOME/..
end

cd $TESTS_HOME/Manual && make -i