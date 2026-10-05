#!/bin/sh
#
# Build and run a client against an installed Motif, once through
# pkg-config and once through find_package(Motif CONFIG).  Used by
# install-check.sh after "cmake --install", and by distro-package.sh
# after installing the Debian or RPM packages.
#
# Usage: consumer-check.sh PREFIX LIBDIR
#
# Checks:
#   - motif.pc is usable: --cflags/--libs work, name -lXm, and every -I/-L
#     directory exists;
#   - consumer/hello.c builds through pkg-config and through
#     find_package(Motif CONFIG), links the libXm of LIBDIR, and runs
#     under Xvfb when xvfb-run exists.

set -eu

if [ $# -ne 2 ]; then
  echo "usage: $0 PREFIX LIBDIR" >&2
  exit 2
fi

here=$(cd "$(dirname "$0")" && pwd)
prefix=$1
libdir=$2
: "${CC:=cc}"

work=$(mktemp -d "${TMPDIR:-/tmp}/motif-consumer.XXXXXX")
trap 'rm -rf "$work"' EXIT

errors=0
fail() {
  echo "consumer-check: ERROR: $*" >&2
  errors=$((errors + 1))
}

# --- pkg-config -----------------------------------------------------------
PKG_CONFIG_PATH=$libdir/pkgconfig:$prefix/share/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}
export PKG_CONFIG_PATH

if pkg-config --exists motif; then
  if pkg-config --help 2>&1 | grep -q -e '--validate'; then
    pkg-config --validate motif || fail "pkg-config --validate motif failed"
  fi
  echo "motif.pc: version $(pkg-config --modversion motif)"
  echo "motif.pc: cflags $(pkg-config --cflags motif)"
  echo "motif.pc: libs $(pkg-config --libs motif)"
  case " $(pkg-config --libs motif) " in
    *" -lXm "*) ;;
    *) fail "pkg-config --libs motif does not contain -lXm" ;;
  esac
  for flag in $(pkg-config --cflags-only-I --libs-only-L motif); do
    dir=${flag#-[IL]}
    [ -d "$dir" ] || fail "motif.pc: $flag is not an existing directory"
  done

  pc_mods=motif
  pc_defs=
  if pkg-config --exists mrm; then
    pc_mods="motif mrm"
    pc_defs=-DHELLO_MRM
  fi
  # shellcheck disable=SC2046,SC2086 # pkg-config output must be split
  $CC $pc_defs -o "$work/hello-pkgconfig" "$here/consumer/hello.c" \
    $(pkg-config --cflags $pc_mods) $(pkg-config --libs $pc_mods) ||
    fail "hello.c does not build with pkg-config $pc_mods"
else
  fail "pkg-config cannot find motif.pc in $libdir/pkgconfig"
fi

# --- find_package(Motif) --------------------------------------------------
if cmake -S "$here/consumer" -B "$work/consumer" \
     -DCMAKE_PREFIX_PATH="$prefix" -DCMAKE_BUILD_TYPE=Release &&
   cmake --build "$work/consumer"; then
  cp "$work/consumer/hello" "$work/hello-cmake"
else
  fail "hello.c does not build with find_package(Motif CONFIG)"
fi

# --- Run the clients ------------------------------------------------------
for hello in "$work/hello-pkgconfig" "$work/hello-cmake"; do
  [ -x "$hello" ] || continue
  # LD_LIBRARY_PATH only matters for a prefix outside the linker's path.
  LD_LIBRARY_PATH=$libdir ldd "$hello" | grep -e libXm -e libMrm || true
  # Compare real paths: with a merged /usr, ld.so reports /lib64/libXm.so
  # for a library installed into /usr/lib64.
  xm=$(LD_LIBRARY_PATH=$libdir ldd "$hello" | sed -n 's/.*libXm\.so[^ ]* => \([^ ]*\).*/\1/p')
  if [ -z "$xm" ] ||
     [ "$(readlink -f "$(dirname "$xm")")" != "$(readlink -f "$libdir")" ]; then
    fail "${hello##*-}: libXm resolves to '$xm', not into $libdir"
  fi
  if command -v xvfb-run >/dev/null 2>&1; then
    rc=0
    LD_LIBRARY_PATH=$libdir \
      xvfb-run -a -s "-screen 0 1280x1024x24 +extension RENDER" "$hello" || rc=$?
    [ "$rc" = 0 ] || fail "${hello##*-} exited with status $rc"
  else
    echo "consumer-check: no xvfb-run, not running ${hello##*-}"
  fi
done

if [ "$errors" != 0 ]; then
  echo "consumer-check: $errors error(s)" >&2
  exit 1
fi
echo "consumer-check: OK"
