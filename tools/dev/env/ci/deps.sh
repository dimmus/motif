#!/bin/sh
#
# Install the packages needed to build and test Motif on the CI systems:
# Debian/Ubuntu, Fedora, Alpine, Arch Linux and FreeBSD.  Runs as root
# (or through sudo when it is available).  Extra package names given as
# arguments are installed as well, e.g. "deps.sh cppcheck".
#
# Besides the libraries this installs both compilers (the CI matrix picks
# one through $CC), lld and the LLVM tools (llvm-ar, llvm-symbolizer) for
# clang LTO and sanitizer reports, ccache, libcheck and an Xvfb with
# xauth and the core fonts, which the X11 test suites use, plus xdotool
# and Xephyr for the tests that drive real input (Text.xdotool,
# Mwm.xdotool), which CI must not skip.

set -eu

SUDO=
if [ "$(id -u)" != 0 ] && command -v sudo >/dev/null 2>&1; then
  SUDO=sudo
fi

os=$(uname -s)
id=
if [ "$os" = Linux ] && [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  id=$(. /etc/os-release && echo "$ID")
fi

case "$os:$id" in
  Linux:debian|Linux:ubuntu)
    export DEBIAN_FRONTEND=noninteractive
    $SUDO apt-get update -qq
    # clang's sanitizer runtimes are a separate package, not built for
    # every architecture (s390x has none).
    rt=
    case $(apt-cache policy libclang-rt-dev | sed -n 's/^ *Candidate: //p') in
      ''|'(none)') ;;
      *) rt=libclang-rt-dev ;;
    esac
    $SUDO apt-get install -y -qq --no-install-recommends \
      build-essential gcc g++ clang $rt lld llvm cmake ninja-build pkg-config ccache \
      flex libfl-dev bison file ca-certificates \
      libx11-dev libxt-dev libxmu-dev libxext-dev libxft-dev libxpm-dev \
      libxrender-dev libfontconfig-dev libfreetype-dev libpng-dev libjpeg-dev \
      x11proto-dev xbitmaps check \
      xvfb xauth xfonts-base xdotool xserver-xephyr \
      "$@"
    ;;
  Linux:fedora)
    # The libraries are named by their pkg-config provides, which do not
    # change when Fedora renames a package.
    $SUDO dnf install -y --setopt=install_weak_deps=False \
      gcc gcc-c++ clang lld llvm cmake ninja-build pkgconf-pkg-config ccache \
      flex libfl-devel bison byacc file findutils which \
      'pkgconfig(x11)' 'pkgconfig(xt)' 'pkgconfig(xmu)' 'pkgconfig(xext)' \
      'pkgconfig(xft)' 'pkgconfig(xpm)' 'pkgconfig(xrender)' \
      'pkgconfig(fontconfig)' 'pkgconfig(freetype2)' 'pkgconfig(libpng)' \
      'pkgconfig(libjpeg)' 'pkgconfig(xproto)' 'pkgconfig(xbitmaps)' \
      'pkgconfig(check)' \
      xorg-x11-server-Xvfb xorg-x11-xauth xorg-x11-fonts-misc \
      xdotool xorg-x11-server-Xephyr \
      "$@"
    ;;
  Linux:alpine)
    $SUDO apk add --no-cache \
      build-base gcc g++ clang lld cmake ninja pkgconf ccache \
      flex flex-dev bison file linux-headers \
      libx11-dev libxt-dev libxmu-dev libxext-dev libxft-dev libxpm-dev \
      libxrender-dev fontconfig-dev freetype-dev libpng-dev \
      libjpeg-turbo-dev xorgproto xbitmaps check-dev \
      xvfb xvfb-run xauth font-misc-misc xdotool xorg-server-xephyr \
      "$@"
    # llvm-ar for clang LTO; Alpine names the LLVM tools by major version.
    $SUDO apk add --no-cache llvm ||
      $SUDO apk add --no-cache "llvm$(clang --version | sed -n 's/.*clang version \([0-9]*\).*/\1/p')"
    ;;
  Linux:arch)
    $SUDO pacman -Syu --noconfirm --needed \
      base-devel gcc clang lld llvm cmake ninja pkgconf ccache \
      flex bison file \
      libx11 libxt libxmu libxext libxft libxpm libxrender fontconfig \
      freetype2 libpng libjpeg-turbo xorgproto xbitmaps check \
      xorg-server-xvfb xorg-xauth xorg-fonts-misc \
      xdotool xorg-server-xephyr \
      "$@"
    ;;
  FreeBSD:*)
    $SUDO env ASSUME_ALWAYS_YES=yes pkg install -y \
      cmake ninja pkgconf ccache bison flex \
      libX11 libXt libXmu libXext libXft libXpm libXrender fontconfig \
      freetype2 png jpeg-turbo xorgproto xbitmaps check \
      xorg-vfbserver xauth xorg-fonts-miscbitmaps font-alias xdotool xephyr \
      "$@"
    ;;
  *)
    echo "deps.sh: unsupported system '$os' '$id'" >&2
    exit 1
    ;;
esac
