#!/bin/sh

# Test script for OS detection logic
# This script tests the OS detection functions without installing packages
# POSIX-compliant shell script for maximum compatibility

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    printf "%b[INFO]%b %s\n" "${BLUE}" "${NC}" "$1"
}

log_success() {
    printf "%b[SUCCESS]%b %s\n" "${GREEN}" "${NC}" "$1"
}

log_warning() {
    printf "%b[WARNING]%b %s\n" "${YELLOW}" "${NC}" "$1"
}

log_error() {
    printf "%b[ERROR]%b %s\n" "${RED}" "${NC}" "$1"
}

# Function to detect OS (copied from deps_check.sh)
detect_os() {
    log_info "Detecting operating system..."
    
    if [ -f /etc/os-release ]; then
        # shellcheck disable=SC1091
        . /etc/os-release
        OS_NAME="$ID"
        OS_VERSION="$VERSION_ID"
        log_success "Detected: $OS_NAME $OS_VERSION"
    elif [ -f /etc/redhat-release ]; then
        OS_NAME="rhel"
        OS_VERSION=$(grep -oE '[0-9]+\.[0-9]+' < /etc/redhat-release 2>/dev/null || echo "unknown")
        log_success "Detected: RHEL/CentOS $OS_VERSION"
    elif [ -f /etc/freebsd-update.conf ]; then
        OS_NAME="freebsd"
        OS_VERSION=$(freebsd-version 2>/dev/null | cut -d'-' -f1 2>/dev/null || echo "unknown")
        log_success "Detected: FreeBSD $OS_VERSION"
    elif [ -f /etc/release ]; then
        if grep -q "OmniOS" /etc/release 2>/dev/null; then
            OS_NAME="omnios"
            OS_VERSION=$(grep "OmniOS" /etc/release 2>/dev/null | grep -oE '[0-9]+\.[0-9]+' 2>/dev/null || echo "unknown")
            log_success "Detected: OmniOS $OS_VERSION"
        elif grep -q "OpenIndiana" /etc/release 2>/dev/null; then
            OS_NAME="openindiana"
            OS_VERSION=$(grep "OpenIndiana" /etc/release 2>/dev/null | grep -oE '[0-9]+\.[0-9]+' 2>/dev/null || echo "unknown")
            log_success "Detected: OpenIndiana $OS_VERSION"
        fi
    else
        log_error "Could not detect operating system"
        exit 1
    fi
    
    # Map OS names to package managers
    case $OS_NAME in
        ubuntu|debian|linuxmint)
            PKG_MANAGER="apt"
            ;;
        arch|manjaro)
            PKG_MANAGER="pacman"
            ;;
        rhel|centos|fedora|rocky|alma)
            PKG_MANAGER="dnf"
            ;;
        alpine)
            PKG_MANAGER="apk"
            ;;
        void)
            PKG_MANAGER="xbps"
            ;;
        openindiana|omnios)
            PKG_MANAGER="pkg"
            ;;
        freebsd)
            PKG_MANAGER="pkg"
            ;;
        *)
            log_error "Unsupported operating system: $OS_NAME"
            exit 1
            ;;
    esac
    
    log_info "Package manager: $PKG_MANAGER"
}

# Function to test package name mapping (copied from deps_check.sh)
get_package_names() {
    base_pkg="$1"
    
    case $OS_NAME in
        ubuntu|debian|linuxmint)
            case $base_pkg in
                autoconf) echo "autoconf" ;;
                automake) echo "automake" ;;
                autopoint) echo "autopoint" ;;
                pkg-config) echo "pkg-config" ;;
                gcc) echo "build-essential" ;;
                make) echo "make" ;;
                flex) echo "flex" ;;
                bison) echo "bison" ;;
                libX11) echo "libx11-dev" ;;
                libXt) echo "libxt-dev" ;;
                libXmu) echo "libxmu-dev" ;;
                libXext) echo "libxext-dev" ;;
                libXpm) echo "libxpm-dev" ;;
                libXft) echo "libxft-dev" ;;
                libjpeg) echo "libjpeg-dev" ;;
                libpng) echo "libpng-dev" ;;
                libXp) echo "libxp-dev" ;;
                check) echo "check" ;;
                *) echo "$base_pkg" ;;
            esac
            ;;
        arch|manjaro)
            case $base_pkg in
                autoconf) echo "autoconf" ;;
                automake) echo "automake" ;;
                autopoint) echo "gettext" ;;
                pkg-config) echo "pkgconf" ;;
                gcc) echo "base-devel" ;;
                make) echo "make" ;;
                flex) echo "flex" ;;
                bison) echo "bison" ;;
                libX11) echo "libx11" ;;
                libXt) echo "libxt" ;;
                libXmu) echo "libxmu" ;;
                libXext) echo "libxext" ;;
                libXpm) echo "libxpm" ;;
                libXft) echo "libxft" ;;
                libjpeg) echo "libjpeg-turbo" ;;
                libpng) echo "libpng" ;;
                libXp) echo "libxp" ;;
                check) echo "check" ;;
                *) echo "$base_pkg" ;;
            esac
            ;;
        rhel|centos|fedora|rocky|alma)
            case $base_pkg in
                autoconf) echo "autoconf" ;;
                automake) echo "automake" ;;
                autopoint) echo "gettext" ;;
                pkg-config) echo "pkgconfig" ;;
                gcc) echo "gcc" ;;
                make) echo "make" ;;
                flex) echo "flex" ;;
                bison) echo "bison" ;;
                libX11) echo "libX11-devel" ;;
                libXt) echo "libXt-devel" ;;
                libXmu) echo "libXmu-devel" ;;
                libXext) echo "libXext-devel" ;;
                libXpm) echo "libXpm-devel" ;;
                libXft) echo "libXft-devel" ;;
                libjpeg) echo "libjpeg-devel" ;;
                libpng) echo "libpng-devel" ;;
                libXp) echo "libXp-devel" ;;
                check) echo "check-devel" ;;
                *) echo "$base_pkg" ;;
            esac
            ;;
        alpine)
            case $base_pkg in
                autoconf) echo "autoconf" ;;
                automake) echo "automake" ;;
                autopoint) echo "gettext" ;;
                pkg-config) echo "pkgconf" ;;
                gcc) echo "build-base" ;;
                make) echo "make" ;;
                flex) echo "flex" ;;
                bison) echo "bison" ;;
                libX11) echo "libx11-dev" ;;
                libXt) echo "libxt-dev" ;;
                libXmu) echo "libxmu-dev" ;;
                libXext) echo "libxext-dev" ;;
                libXpm) echo "libxpm-dev" ;;
                libXft) echo "libxft-dev" ;;
                libjpeg) echo "libjpeg-turbo-dev" ;;
                libpng) echo "libpng-dev" ;;
                libXp) echo "libxp-dev" ;;
                check) echo "check-dev" ;;
                *) echo "$base_pkg" ;;
            esac
            ;;
        void)
            case $base_pkg in
                autoconf) echo "autoconf" ;;
                automake) echo "automake" ;;
                autopoint) echo "gettext" ;;
                pkg-config) echo "pkg-config" ;;
                gcc) echo "base-devel" ;;
                make) echo "make" ;;
                flex) echo "flex" ;;
                bison) echo "bison" ;;
                libX11) echo "libX11-devel" ;;
                libXt) echo "libXt-devel" ;;
                libXmu) echo "libXmu-devel" ;;
                libXext) echo "libXext-devel" ;;
                libXpm) echo "libXpm-devel" ;;
                libXft) echo "libXft-devel" ;;
                libjpeg) echo "libjpeg-turbo-devel" ;;
                libpng) echo "libpng-devel" ;;
                libXp) echo "libXp-devel" ;;
                check) echo "check-devel" ;;
                *) echo "$base_pkg" ;;
            esac
            ;;
        openindiana|omnios)
            case $base_pkg in
                autoconf) echo "autoconf" ;;
                automake) echo "automake" ;;
                autopoint) echo "gettext" ;;
                pkg-config) echo "pkg-config" ;;
                gcc) echo "gcc" ;;
                make) echo "make" ;;
                flex) echo "flex" ;;
                bison) echo "bison" ;;
                libX11) echo "x11/header/x11" ;;
                libXt) echo "x11/header/xt" ;;
                libXmu) echo "x11/header/xmu" ;;
                libXext) echo "x11/header/xext" ;;
                libXpm) echo "x11/header/xpm" ;;
                libXft) echo "x11/header/xft" ;;
                libjpeg) echo "image/library/libjpeg" ;;
                libpng) echo "image/library/libpng" ;;
                libXp) echo "x11/header/xp" ;;
                check) echo "developer/check" ;;
                *) echo "$base_pkg" ;;
            esac
            ;;
        freebsd)
            case $base_pkg in
                autoconf) echo "autoconf" ;;
                automake) echo "automake" ;;
                autopoint) echo "gettext" ;;
                pkg-config) echo "pkgconf" ;;
                gcc) echo "gcc" ;;
                make) echo "gmake" ;;
                flex) echo "flex" ;;
                bison) echo "bison" ;;
                libX11) echo "libX11" ;;
                libXt) echo "libXt" ;;
                libXmu) echo "libXmu" ;;
                libXext) echo "libXext" ;;
                libXpm) echo "libXpm" ;;
                libXft) echo "libXft" ;;
                libjpeg) echo "jpeg" ;;
                libpng) echo "libpng" ;;
                libXp) echo "libXp" ;;
                check) echo "check" ;;
                *) echo "$base_pkg" ;;
            esac
            ;;
    esac
}

# Test function
test_package_mapping() {
    log_info "Testing package name mapping for $OS_NAME..."
    
    test_deps="autoconf automake pkg-config gcc libX11 libXp"
    
    for dep in $test_deps; do
        pkg_name=$(get_package_names "$dep")
        log_info "  $dep -> $pkg_name"
    done
}

# Main test function
main() {
    echo "=========================================="
    echo "    OS Detection Test Script"
    echo "=========================================="
    echo
    
    # Test OS detection
    detect_os
    
    echo
    # Test package mapping
    test_package_mapping
    
    echo
    log_success "OS detection test completed successfully!"
}

# Run main function
main "$@"
