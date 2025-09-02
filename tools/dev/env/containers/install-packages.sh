#!/bin/bash
#
# Package installation helper script for Arch Linux container
# Handles retries and fallbacks for package installation issues
#

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

# Function to install packages with retry logic
install_packages() {
    local packages=("$@")
    local max_retries=3
    local retry_count=0
    
    log_info "Installing packages: ${packages[*]}"
    
    while [[ ${retry_count} -lt ${max_retries} ]]; do
        if pacman -S --noconfirm "${packages[@]}"; then
            log_success "Successfully installed packages"
            return 0
        else
            retry_count=$((retry_count + 1))
            log_warning "Package installation failed (attempt ${retry_count}/${max_retries})"
            
            if [[ ${retry_count} -lt ${max_retries} ]]; then
                log_info "Updating package database and retrying..."
                pacman -Syu --noconfirm
                sleep 2
            fi
        fi
    done
    
    log_error "Failed to install packages after ${max_retries} attempts"
    return 1
}

# Function to install packages with fallback options
install_packages_with_fallback() {
    local primary_packages=("$@")
    local fallback_packages=()
    
    # Try to install primary packages
    if install_packages "${primary_packages[@]}"; then
        return 0
    fi
    
    log_warning "Primary package installation failed, trying fallback packages..."
    
    # Create fallback package list (remove problematic packages)
    for pkg in "${primary_packages[@]}"; do
        case "${pkg}" in
            "ghostscript"|"svt-av1"|"doxygen"|"graphviz"|"man-db"|"man-pages")
                log_info "Skipping problematic package: ${pkg}"
                ;;
            *)
                fallback_packages+=("${pkg}")
                ;;
        esac
    done
    
    if [[ ${#fallback_packages[@]} -gt 0 ]]; then
        log_info "Installing fallback packages: ${fallback_packages[*]}"
        install_packages "${fallback_packages[@]}"
    else
        log_error "No fallback packages available"
        return 1
    fi
}

# Main installation logic
main() {
    log_info "Starting package installation with enhanced error handling..."
    
    # Update system first
    log_info "Updating system packages..."
    pacman -Syu --noconfirm
    
    # Install base development tools
    log_info "Installing base development tools..."
    install_packages \
        base-devel \
        git \
        autoconf \
        automake \
        libtool \
        pkgconf \
        make \
        gcc \
        clang \
        cmake \
        ninja \
        gdb \
        valgrind \
        strace \
        ltrace \
        time \
        which \
        file \
        binutils \
        coreutils \
        findutils \
        grep \
        sed \
        gawk \
        tar \
        gzip \
        xz \
        bzip2 \
        unzip \
        curl \
        wget
    
    # Install X11 and Motif dependencies
    log_info "Installing X11 and Motif dependencies..."
    install_packages \
        xorg-server-devel \
        xorg-util-macros \
        libx11 \
        libxt \
        libxmu \
        libxpm \
        libxext \
        libxft \
        libxrender \
        libxrandr \
        libxi \
        libxinerama \
        libxcursor \
        libxdamage \
        libxfixes \
        libxcomposite \
        libxss \
        libxtst \
        libxv \
        libxvmc \
        libxxf86vm \
        libxres \
        libsm \
        libice \
        libxau \
        libxdmcp \
        libxcb \
        xcb-util \
        xcb-util-image \
        xcb-util-keysyms \
        xcb-util-renderutil \
        xcb-util-wm \
        xcb-util-xrm
    
    # Install additional libraries
    log_info "Installing additional libraries..."
    install_packages \
        fontconfig \
        freetype2 \
        libpng \
        libjpeg-turbo \
        libtiff \
        zlib \
        bzip2 \
        expat \
        libxml2 \
        libxslt \
        openssl \
        glibc \
        glib2 \
        pcre \
        pcre2
    
    # Install build tools
    log_info "Installing build tools..."
    install_packages \
        flex \
        bison \
        m4 \
        gettext \
        intltool \
        patch \
        diffutils \
        dos2unix \
        inetutils
    
    # Install documentation tools (with fallback)
    log_info "Installing documentation tools..."
    install_packages_with_fallback \
        texinfo \
        help2man \
        doxygen \
        graphviz \
        man-db \
        man-pages
    
    # Clean package cache
    log_info "Cleaning package cache..."
    pacman -Scc --noconfirm
    
    log_success "Package installation completed successfully!"
}

# Run main function
main "$@"
