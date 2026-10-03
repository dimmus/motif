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
#   - after the real installation, motif.pc is usable: --cflags/--libs
#     work, name -lXm, and every -I/-L directory exists;
#   - consumer/hello.c builds through pkg-config and through
#     find_package(Motif CONFIG), and runs under Xvfb when xvfb-run exists.

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
: "${CC:=cc}"

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
  $CC $pc_defs -o "$stage.hello-pkgconfig" "$here/consumer/hello.c" \
    $(pkg-config --cflags $pc_mods) $(pkg-config --libs $pc_mods) ||
    fail "hello.c does not build with pkg-config $pc_mods"
else
  fail "pkg-config cannot find motif.pc in $libdir/pkgconfig"
fi

# --- find_package(Motif) --------------------------------------------------
if cmake -S "$here/consumer" -B "$stage.consumer" \
     -DCMAKE_PREFIX_PATH="$prefix" -DCMAKE_BUILD_TYPE=Release &&
   cmake --build "$stage.consumer"; then
  cp "$stage.consumer/hello" "$stage.hello-cmake"
else
  fail "hello.c does not build with find_package(Motif CONFIG)"
fi
rm -rf "$stage.consumer"

# --- Run the clients ------------------------------------------------------
for hello in "$stage.hello-pkgconfig" "$stage.hello-cmake"; do
  [ -x "$hello" ] || continue
  # LD_LIBRARY_PATH only matters for a prefix outside the linker's path.
  LD_LIBRARY_PATH=$libdir ldd "$hello" | grep -e libXm -e libMrm || true
  # Compare real paths: with a merged /usr, ld.so reports /lib64/libXm.so
  # for a library installed into /usr/lib64.
  xm=$(LD_LIBRARY_PATH=$libdir ldd "$hello" | sed -n 's/.*libXm\.so[^ ]* => \([^ ]*\).*/\1/p')
  if [ -z "$xm" ] ||
     [ "$(readlink -f "$(dirname "$xm")")" != "$(readlink -f "$libdir")" ]; then
    fail "${hello##*.}: libXm resolves to '$xm', not into $libdir"
  fi
  if command -v xvfb-run >/dev/null 2>&1; then
    rc=0
    LD_LIBRARY_PATH=$libdir \
      xvfb-run -a -s "-screen 0 1280x1024x24 +extension RENDER" "$hello" || rc=$?
    [ "$rc" = 0 ] || fail "${hello##*.} exited with status $rc"
  else
    echo "install-check: no xvfb-run, not running ${hello##*.}"
  fi
  rm -f "$hello"
done

if [ "$errors" != 0 ]; then
  echo "install-check: $errors error(s)" >&2
  exit 1
fi
echo "install-check: OK"
