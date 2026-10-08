#!/bin/sh
# Build one Motif version and the profiling programs against it.
#
#   build.sh SOURCE PREFIX
#
# SOURCE is a source tree or a tarball of one (an archive of a commit
# of this tree, or a release: 2.3.x and 2.4.x build with autotools,
# later versions with CMake).  The libraries, mwm and the profiling
# programs are installed under PREFIX; the build goes to
# $BUILD_ROOT/<name of PREFIX> (default /build), never into SOURCE,
# which may be mounted read-only.
#
# Every version is built with the same flags, "-O2 -g
# -fno-omit-frame-pointer" (frame pointers so that perf can also unwind
# with --call-graph fp), plus whatever its own build adds; the CMake
# build adds -fno-semantic-interposition and -fno-plt, which are part
# of what is being measured.
set -eu

src=$1
prefix=$2
here=$(cd "$(dirname "$0")" && pwd)
build_root=${BUILD_ROOT:-/build}
jobs=${JOBS:-2}
cflags="-O2 -g -fno-omit-frame-pointer"

name=$(basename "$prefix")
bdir=$build_root/$name
rm -rf "$bdir"
mkdir -p "$bdir"

if [ -f "$src" ]; then
	mkdir "$bdir/src"
	tar -C "$bdir/src" -xf "$src"
	src=$(echo "$bdir"/src/*)
fi
log=$bdir/build.log

if [ ! -f "$src/CMakeLists.txt" ]; then
	cd "$src"
	[ -x configure ] || autoreconf -fi >"$log" 2>&1
	# -fcommon: 2.3.x defines some globals in headers.  LIBS: its
	# demos link libX11 before a static library that needs it.
	./configure --prefix="$prefix" --disable-static \
		--enable-xft --enable-jpeg --enable-png \
		CFLAGS="$cflags -fcommon -Wno-error -std=gnu17" \
		LIBS=-lX11 >>"$log" 2>&1 &&
	make -j"$jobs" >>"$log" 2>&1 &&
	make install >>"$log" 2>&1 || { tail -n 40 "$log"; exit 1; }
else
	b=$bdir/build
	{
		cmake -S "$src" -B "$b" -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
			-DCMAKE_C_FLAGS_RELWITHDEBINFO="$cflags -DNDEBUG" \
			-DCMAKE_INSTALL_PREFIX="$prefix" -DWITH_TESTS=ON &&
		ninja -C "$b" -j"$jobs" all xmbench xmbench_preload &&
		ninja -C "$b" install
	} >"$log" 2>&1 || { tail -n 40 "$log"; exit 1; }
	cp "$b/src/tests/bench/xmbench" \
	   "$b/src/tests/bench/libxmbench_preload.so" "$prefix/bin/"
fi

# The probes, built the same way against each version.
cc $cflags -o "$prefix/bin/hello_motif" "$here/hello_motif.c" \
	-I"$prefix/include" -L"$prefix/lib" -Wl,-rpath,"$prefix/lib" \
	-lXm -lXt -lX11
cc $cflags -o "$prefix/bin/xclients" "$here/xclients.c" -lX11
echo "built $name in $prefix"
