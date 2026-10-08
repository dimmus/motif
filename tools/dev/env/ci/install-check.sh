#!/bin/sh
#
# Install a configured and built Motif tree into a staging directory and
# check the result the way a distribution package build would.  Then
# install it for real into its configured prefix and build and run a
# client against it, once through pkg-config and once through
# find_package(Motif).
#
# Usage: install-check.sh BUILD_DIR STAGE_DIR
#
# The real installation is the point of the second half: it is what
# users get, without DESTDIR or sysroot tricks that hide wrong paths.  CI
# runs this in a throwaway container or VM; the prefix must be writable,
# or sudo available.  To try it locally, configure the build with
# -DCMAKE_INSTALL_PREFIX=<some scratch directory>.
#
# Checks:
#   - no symlink in the staging tree dangles or points outside it (for
#     example back into the build tree);
#   - no installed ELF file has an RPATH/RUNPATH into the build or source
#     tree, and no installed text file mentions those trees;
#   - every installed ELF file resolves all of its libraries (ldd);
#   - after the real installation, consumer-check.sh: motif.pc is usable,
#     and consumer/hello.c builds through pkg-config and through
#     find_package(Motif CONFIG) and runs under Xvfb when xvfb-run exists.

set -eu

if [ $# -ne 2 ]; then
  echo "usage: $0 BUILD_DIR STAGE_DIR" >&2
  exit 2
fi

here=$(cd "$(dirname "$0")" && pwd)
build=$(cd "$1" && pwd)
srcdir=$(sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' "$build/CMakeCache.txt")
prefix=$(sed -n 's/^CMAKE_INSTALL_PREFIX:[A-Z]*=//p' "$build/CMakeCache.txt")
libdir=$(sed -n 's/^CMAKE_INSTALL_LIBDIR:[A-Z]*=//p' "$build/CMakeCache.txt")
case "$libdir" in
  /*) ;;
  *) libdir=$prefix/${libdir:-lib} ;;
esac

rm -rf "$2"
mkdir -p "$2"
stage=$(cd "$2" && pwd)

errors=0
fail() {
  echo "install-check: ERROR: $*" >&2
  errors=$((errors + 1))
}

echo "::group::Install into $stage (prefix $prefix, libdir $libdir)"
DESTDIR=$stage cmake --install "$build"
echo "::endgroup::"

# --- Symlinks -------------------------------------------------------------
find "$stage" -type l | while read -r link; do
  target=$(readlink "$link")
  case "$target" in
    /*) resolved=$stage$target ;;
    *) resolved=$(dirname "$link")/$target ;;
  esac
  if [ ! -e "$resolved" ]; then
    echo "install-check: ERROR: ${link#"$stage"} -> $target does not resolve inside the installation" >&2
    echo x
  fi
done >"$stage.symlinks"
if [ -s "$stage.symlinks" ]; then
  fail "$(wc -l <"$stage.symlinks") bad symlink(s)"
fi
rm -f "$stage.symlinks"

# --- References to the build and source trees -----------------------------
# (The source directories, not the top of the tree, so that a scratch
# install prefix inside the checkout is no false positive.)
leaks=$(grep -rlI -e "$build" -e "$srcdir/src/" -e "$srcdir/include/" -e "$srcdir/tools/" \
  "$stage" 2>/dev/null || true)
if [ -n "$leaks" ]; then
  fail "installed files mention the build or source tree: $leaks"
fi

elf_files=$(find "$stage" -type f | while read -r f; do
  if [ "$(head -c 4 "$f" | od -An -c | tr -d ' ')" = '177ELF' ]; then
    echo "$f"
  fi
done)

for f in $elf_files; do
  if command -v readelf >/dev/null 2>&1; then
    rp=$(readelf -d "$f" 2>/dev/null | sed -n 's/.*(R\(UN\)*PATH).*\[\(.*\)\]/\2/p')
    case "$rp" in
      *"$build"*|*"$srcdir/"*) fail "${f#"$stage"} has RPATH/RUNPATH $rp" ;;
    esac
  fi
  case "$f" in
    *.a|*.o) continue ;;
  esac
  missing=$(LD_LIBRARY_PATH=$stage$libdir ldd "$f" 2>&1 | grep 'not found' || true)
  if [ -n "$missing" ]; then
    fail "${f#"$stage"}: unresolved libraries: $missing"
  fi
done
echo "install-check: $(echo "$elf_files" | grep -c . || true) ELF files checked"

# --- Real installation --------------------------------------------------
SUDO=
if [ ! -w "$prefix" ] && [ "$(id -u)" != 0 ]; then
  if [ -d "$prefix" ] || ! mkdir -p "$prefix" 2>/dev/null; then
    SUDO=sudo
  fi
fi
echo "::group::Install into $prefix"
$SUDO cmake --install "$build"
echo "::endgroup::"

# --- Build and run a client against it ----------------------------------
"$here/consumer-check.sh" "$prefix" "$libdir" ||
  fail "the consumer check against $prefix failed"

if [ "$errors" != 0 ]; then
  echo "install-check: $errors error(s)" >&2
  exit 1
fi
echo "install-check: OK"
