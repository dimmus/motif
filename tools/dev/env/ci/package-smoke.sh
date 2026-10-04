#!/bin/sh
#
# Distribution packaging smoke test: build Motif the way a Debian or Fedora
# package would (the distribution's compiler and linker flags, its libdir
# layout, tests run as in a package's check step), then hand the tree to
# install-check.sh for the staged-install checks and the install-and-
# consume test.  Run it from the top of the source tree, as root in a
# throwaway debian:stable or fedora:latest container.
#
# This deliberately does not carry a debian/ directory or a spec file; it
# checks that the CMake build gives packagers what they need.  Elsewhere
# it falls back to generic hardening flags.
#
# Usage: package-smoke.sh [BUILD_DIR [STAGE_DIR]]

set -eu

build=${1:-_build}
stage=${2:-_stage}
here=$(cd "$(dirname "$0")" && pwd)

id=
if [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  id=$(. /etc/os-release && echo "$ID")
fi

case "$id" in
  debian|ubuntu)
    # Same flags as dpkg-buildpackage, including -Werror=format-security.
    DEB_BUILD_MAINT_OPTIONS=hardening=+all
    export DEB_BUILD_MAINT_OPTIONS
    eval "$(dpkg-buildflags --export=sh)"
    libdir=lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)
    ;;
  fedora)
    # Same flags as rpmbuild's %cmake (needs redhat-rpm-config).
    CFLAGS=$(rpm --eval '%{build_cflags}')
    CXXFLAGS=$(rpm --eval '%{build_cxxflags}')
    LDFLAGS=$(rpm --eval '%{build_ldflags}')
    export CFLAGS CXXFLAGS LDFLAGS
    libdir=$(rpm --eval '%{_lib}')
    ;;
  *)
    CFLAGS="-O2 -g -D_FORTIFY_SOURCE=3 -fstack-protector-strong -Wformat -Werror=format-security"
    LDFLAGS="-Wl,-z,relro,-z,now"
    export CFLAGS LDFLAGS
    libdir=lib
    ;;
esac
echo "CFLAGS=${CFLAGS:-}"
echo "LDFLAGS=${LDFLAGS:-}"

# CMAKE_BUILD_TYPE=None: use exactly the distribution's flags, as the
# Debian and Fedora CMake helpers do.
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=None -DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_INSTALL_LIBDIR=$libdir ${CMAKE_ARGS:-}"
BUILD_DIR=$build MOTIF_CI_PROFILE=release CMAKE_ARGS=$CMAKE_ARGS \
  "$here/build.sh"
"$here/install-check.sh" "$build" "$stage"
