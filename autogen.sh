#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR="`pwd`"
cd "$srcdir"

libtoolize --force --automake
aclocal -I .
autoconf
autoheader
automake --foreign  --include-deps --add-missing

cd "$THEDIR"
exec "$srcdir"/configure "$@"
