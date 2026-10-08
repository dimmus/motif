#!/bin/sh
#
# Build the distribution packages of tools/packaging the way the
# distribution does, check them with its linter, install them and build
# a client against the installed packages:
#
#   Debian, Ubuntu:  dpkg-buildpackage -us -uc -b with tools/packaging/debian
#                    copied into a copy of the tree, lintian, apt-get install
#   Fedora:          rpmbuild -ba of tools/packaging/rpm/motif.spec on a
#                    tarball of the tree, rpmlint, dnf install
#
# then consumer-check.sh against /usr.  The package builds run the test
# suite (dh_auto_test, %check) under Xvfb.  Run it as root, from the top
# of the source tree, in a throwaway debian:trixie or fedora:latest
# container: it installs the build dependencies and the packages.
#
# Usage: distro-package.sh [OUT_DIR]
#
# OUT_DIR (default: _pkg) receives the copy of the tree, the packages, the
# build log and the linter reports.  The packaging must be at the version
# of project() in CMakeLists.txt (debian/changelog, the spec's Version).
# lintian and rpmlint fail the run on errors, not on warnings.

set -eu

out=${1:-_pkg}
here=$(cd "$(dirname "$0")" && pwd)
top=$(pwd)
packaging=$top/tools/packaging

die() {
  echo "distro-package: $*" >&2
  exit 1
}

version=$(sed -n 's/^project(Motif VERSION \([^ )]*\).*/\1/p' CMakeLists.txt)
[ -n "$version" ] || die "no project(Motif VERSION ...) in CMakeLists.txt"

id=
if [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  id=$(. /etc/os-release && echo "$ID")
fi

rm -rf "$out"
mkdir -p "$out"
out=$(cd "$out" && pwd)

# copy_tree DEST: copy the source tree without VCS data and build trees
# (the _* directories at the top, such as _build or this script's own
# output).
copy_tree() {
  mkdir -p "$1"
  tar -c --exclude=./.git --exclude='./_*' --exclude=./.ccache . | tar -x -C "$1"
}

# run_logged TITLE COMMAND...: run COMMAND, show its output and keep it
# in $out/build.log.
run_logged() {
  title=$1
  shift
  echo "::group::$title"
  rc=0
  "$@" >"$out/build.log" 2>&1 || rc=$?
  cat "$out/build.log"
  echo "::endgroup::"
  [ "$rc" = 0 ] || die "$title failed (status $rc), see $out/build.log"
}

case "$id" in
  debian|ubuntu)
    export DEBIAN_FRONTEND=noninteractive
    # A failed index download is only a warning to apt-get update; retry.
    echo 'Acquire::Retries "3";' >/etc/apt/apt.conf.d/80retries
    apt-get update -qq
    apt-get install -y -qq --no-install-recommends dpkg-dev lintian >/dev/null
    src=$out/motif-$version
    copy_tree "$src"
    cp -R "$packaging/debian" "$src/debian"
    deb_version=$(dpkg-parsechangelog -l "$src/debian/changelog" -S Version)
    case $deb_version in
      "$version"-*) ;;
      *) die "debian/changelog is at $deb_version, CMakeLists.txt at $version" ;;
    esac
    apt-get build-dep -y -qq "$src" >/dev/null
    run_logged "dpkg-buildpackage -us -uc -b" \
      sh -c 'cd "$1" && dpkg-buildpackage -us -uc -b' sh "$src"
    changes=$out/motif_${deb_version}_$(dpkg-architecture -qDEB_HOST_ARCH).changes
    [ -f "$changes" ] || die "dpkg-buildpackage did not write $changes"
    echo "::group::lintian"
    rc=0
    lintian --fail-on error --display-info --pedantic "$changes" \
      >"$out/lintian.txt" 2>&1 || rc=$?
    cat "$out/lintian.txt"
    echo "::endgroup::"
    [ "$rc" = 0 ] || die "lintian reports errors (status $rc)"
    # shellcheck disable=SC2046 # one argument per package
    apt-get install -y -qq --no-install-recommends \
      $(sed -n 's/^ [0-9a-f]\{64\} [0-9]* \(.*\.deb\)$/\1/p' "$changes" |
        grep -v -e '-dbgsym_' | sed "s|^|$out/|") >/dev/null
    libdir=/usr/lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)
    ;;
  fedora)
    # rpmlint pulls in openh264 (through libheif); Fedora's own repository
    # has a stub of it, the Cisco repository is often unreachable.
    dnf install -y -q --setopt=install_weak_deps=False \
      --disablerepo=fedora-cisco-openh264 \
      rpm-build rpmlint tar xz 'dnf-command(builddep)'
    topdir=$out/rpmbuild
    spec=$packaging/rpm/motif.spec
    spec_version=$(rpmspec -q --srpm --qf '%{version}\n' "$spec")
    [ "$spec_version" = "$version" ] ||
      die "motif.spec is at $spec_version, CMakeLists.txt at $version"
    mkdir -p "$topdir/SOURCES"
    copy_tree "$out/motif-$version"
    tar -C "$out" -cJf "$topdir/SOURCES/motif-$version.tar.xz" "motif-$version"
    dnf builddep -y -q --setopt=install_weak_deps=False "$spec"
    run_logged "rpmbuild -ba" rpmbuild -ba --define "_topdir $topdir" "$spec"
    echo "::group::rpmlint"
    rc=0
    rpmlint -r "$packaging/rpm/motif.rpmlintrc" \
      "$spec" "$topdir"/SRPMS/*.rpm "$topdir"/RPMS/*/*.rpm \
      >"$out/rpmlint.txt" 2>&1 || rc=$?
    cat "$out/rpmlint.txt"
    echo "::endgroup::"
    [ "$rc" = 0 ] || die "rpmlint reports errors (status $rc)"
    dnf install -y -q --setopt=install_weak_deps=False \
      "$topdir"/RPMS/*/motif-"$version"-*.rpm \
      "$topdir"/RPMS/*/motif-devel-"$version"-*.rpm
    rpm -q motif motif-devel
    libdir=$(rpm --eval '%{_libdir}')
    ;;
  *)
    die "unsupported system '$id' (Debian, Ubuntu or Fedora)"
    ;;
esac

"$here/consumer-check.sh" /usr "$libdir"
echo "distro-package: OK"
