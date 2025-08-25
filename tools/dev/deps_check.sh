#!/bin/sh

# Motif Dependency Checker and Installer
# Supports: Ubuntu, Arch Linux, Debian, OpenIndiana, OmniOS, FreeBSD
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

# Function to detect OS
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

# Function to check if a package is available for installation
check_package_available() {
    pkg_name="$1"
    
    case $PKG_MANAGER in
        apt)
            if apt-cache show "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        pacman)
            if pacman -Ss "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        dnf)
            if dnf search "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        apk)
            if apk search "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        xbps)
            if xbps-query -R "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        pkg)
            if pkg search "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
    esac
    return 1
}

# Function to check if a package is installed
check_package() {
    pkg_name="$1"
    
    case $PKG_MANAGER in
        apt)
            if dpkg -l | grep -q "^ii.*$pkg_name"; then
                return 0
            fi
            ;;
        pacman)
            if pacman -Q "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        dnf)
            if rpm -q "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        apk)
            if apk info -q "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        xbps)
            if xbps-query "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
        pkg)
            if pkg info "$pkg_name" >/dev/null 2>&1; then
                return 0
            fi
            ;;
    esac
    return 1
}

# Function to install packages
install_packages() {
    packages="$*"
    
    log_info "Installing packages: $packages"
    
    case $PKG_MANAGER in
        apt)
            sudo apt-get update
            sudo apt-get install -y "$packages"
            ;;
        pacman)
            sudo pacman -Sy --noconfirm "$packages"
            ;;
        dnf)
            sudo dnf install -y "$packages"
            ;;
        apk)
            sudo apk update
            sudo apk add "$packages"
            ;;
        xbps)
            sudo xbps-install -y "$packages"
            ;;
        pkg)
            sudo pkg install -y "$packages"
            ;;
    esac
}

# Function to get package names for current OS
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

# Main dependency checking function
check_dependencies() {
    log_info "Checking project dependencies..."
    
    # Define required dependencies (essential for building)
    required_deps="autoconf automake autopoint pkg-config gcc make flex bison libX11 libXt libXmu libXext libXpm libXft libjpeg libpng check"
    
    # Define optional dependencies (nice to have but not essential)
    optional_deps="libXp"
    
    missing_required=""
    missing_optional=""
    installed_count=0
    total_required=0
    
    # Count total required dependencies
    for dep in $required_deps; do
        total_required=$((total_required + 1))
    done
    
    # Check required dependencies
    for dep in $required_deps; do
        pkg_name=$(get_package_names "$dep")
        
        if check_package "$pkg_name" "$dep"; then
            log_success "✓ $dep ($pkg_name) - installed"
            installed_count=$((installed_count + 1))
        else
            log_warning "✗ $dep ($pkg_name) - not installed"
            missing_required="$missing_required $pkg_name"
        fi
    done
    
    # Check optional dependencies
    for dep in $optional_deps; do
        pkg_name=$(get_package_names "$dep")
        
        if check_package "$pkg_name" "$dep"; then
            log_success "✓ $dep ($pkg_name) - installed (optional)"
            installed_count=$((installed_count + 1))
        elif check_package_available "$pkg_name"; then
            log_warning "✗ $dep ($pkg_name) - not installed (optional, available)"
            missing_optional="$missing_optional $pkg_name"
        else
            log_warning "✗ $dep ($pkg_name) - not available (optional, not in repositories)"
        fi
    done
    
    log_info "Dependency check complete: $installed_count/$((total_required + 1)) packages installed"
    
    if [ -n "$missing_required" ]; then
        log_error "Missing required packages: $missing_required"
        return 1
    elif [ -n "$missing_optional" ]; then
        log_warning "Missing optional packages: $missing_optional"
        log_info "All required dependencies are installed. Optional packages can be installed separately."
        return 0
    fi
}

# Function to install missing dependencies
install_missing_dependencies() {
    log_info "Installing missing dependencies..."
    
    # Get list of missing required packages
    missing_required=""
    missing_optional=""
    required_deps="autoconf automake autopoint pkg-config gcc make flex bison libX11 libXt libXmu libXext libXpm libXft libjpeg libpng check"
    optional_deps="libXp"
    
    # Check required dependencies
    for dep in $required_deps; do
        pkg_name=$(get_package_names "$dep")
        if ! check_package "$pkg_name" "$dep"; then
            missing_required="$missing_required $pkg_name"
        fi
    done
    
    # Check optional dependencies
    for dep in $optional_deps; do
        pkg_name=$(get_package_names "$dep")
        if ! check_package "$pkg_name" "$dep" && check_package_available "$pkg_name"; then
            missing_optional="$missing_optional $pkg_name"
        fi
    done
    
    if [ -z "$missing_required" ] && [ -z "$missing_optional" ]; then
        log_success "No missing dependencies to install"
        return 0
    fi
    
    # Install required packages first
    if [ -n "$missing_required" ]; then
        log_info "Installing required packages: $missing_required"
        if ! install_packages "$missing_required"; then
            log_error "Failed to install required dependencies"
            return 1
        fi
    fi
    
    # Install optional packages if available
    if [ -n "$missing_optional" ]; then
        log_info "Installing optional packages: $missing_optional"
        if install_packages "$missing_optional"; then
            log_success "Optional packages installed successfully!"
        else
            log_warning "Some optional packages failed to install (this is not critical)"
        fi
    fi
    
    log_success "All required dependencies installed successfully!"
    return 0
}

# Function to verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    if check_dependencies; then
        log_success "All dependencies are properly installed!"
        return 0
    else
        log_error "Some dependencies are still missing after installation"
        return 1
    fi
}

# Main function
main() {
    echo "=========================================="
    echo "    Motif Dependency Checker & Installer"
    echo "=========================================="
    echo
    
    # Check if running as root
    if [ "$(id -u)" -eq 0 ]; then
        log_error "This script should not be run as root"
        exit 1
    fi
    
    # Check if sudo is available
    if ! command -v sudo >/dev/null 2>&1; then
        log_error "sudo is not available. Please install it first."
        exit 1
    fi
    
    # Detect OS
    detect_os
    
    echo
    log_info "Starting dependency check..."
    
    # Check current dependencies
    if check_dependencies; then
        log_success "All dependencies are already installed!"
        exit 0
    fi
    
    echo
    log_info "Some dependencies are missing. Installing them now..."
    
    # Install missing dependencies
    if install_missing_dependencies; then
        echo
        log_info "Installation completed. Verifying..."
        
        # Verify installation
        if verify_installation; then
            log_success "Dependency installation completed successfully!"
            echo
            log_info "You can now build Motif with:"
            echo "  ./autogen.sh"
            echo "  ./configure"
            echo "  make"
            exit 0
        else
            log_error "Installation verification failed"
            exit 1
        fi
    else
        log_error "Failed to install dependencies"
        exit 1
    fi
}

# Run main function
main "$@"
