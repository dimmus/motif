#!/bin/bash
#
# Motif Build Script for Container Environments
# This script is executed inside Docker/Podman containers to build and test Motif
#

set -euo pipefail

# Configuration
MOTIF_SOURCE="/motif"
BUILD_DIR="/home/builder/motif-build"
INSTALL_DIR="/home/builder/motif-install"
LOG_FILE="/home/builder/build.log"

# Build options (can be overridden via command line)
BUILD_TYPE="release"
INCREMENTAL=false
NO_TESTS=false
OPTIMIZE=false
NO_DEPS=false
JOBS=""
CLEAN_BUILD=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*" | tee -a "${LOG_FILE}"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*" | tee -a "${LOG_FILE}"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*" | tee -a "${LOG_FILE}"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" | tee -a "${LOG_FILE}"
}

log_debug() {
    echo -e "${PURPLE}[DEBUG]${NC} $*" | tee -a "${LOG_FILE}"
}

# Get system information
get_system_info() {
    log_info "Gathering system information..."
    
    echo "System Information" | tee -a "${LOG_FILE}"
    echo "==================" | tee -a "${LOG_FILE}"
    echo "Date: $(date)" | tee -a "${LOG_FILE}"
    echo "Hostname: $(hostname 2>/dev/null || echo 'unknown')" | tee -a "${LOG_FILE}"
    echo "User: $(whoami)" | tee -a "${LOG_FILE}"
    echo "Working Directory: $(pwd)" | tee -a "${LOG_FILE}"
    echo "PATH: ${PATH}" | tee -a "${LOG_FILE}"
    echo "" | tee -a "${LOG_FILE}"
    
    # OS Information
    if [[ -f /etc/os-release ]]; then
        echo "OS Release Information:" | tee -a "${LOG_FILE}"
        cat /etc/os-release | tee -a "${LOG_FILE}"
    elif [[ -f /etc/lsb-release ]]; then
        echo "LSB Release Information:" | tee -a "${LOG_FILE}"
        cat /etc/lsb-release | tee -a "${LOG_FILE}"
    fi
    echo "" | tee -a "${LOG_FILE}"
    
    # Kernel information
    echo "Kernel: $(uname -a)" | tee -a "${LOG_FILE}"
    echo "" | tee -a "${LOG_FILE}"
    
    # CPU information
    if [[ -f /proc/cpuinfo ]]; then
        echo "CPU Information:" | tee -a "${LOG_FILE}"
        grep -E "^(processor|model name|cpu MHz|cache size|cpu cores)" /proc/cpuinfo | head -20 | tee -a "${LOG_FILE}"
        echo "" | tee -a "${LOG_FILE}"
    fi
    
    # Memory information
    if [[ -f /proc/meminfo ]]; then
        echo "Memory Information:" | tee -a "${LOG_FILE}"
        grep -E "^(MemTotal|MemFree|MemAvailable|SwapTotal|SwapFree)" /proc/meminfo | tee -a "${LOG_FILE}"
        echo "" | tee -a "${LOG_FILE}"
    fi
    
    # Compiler information
    echo "Compiler Information:" | tee -a "${LOG_FILE}"
    if command -v gcc &> /dev/null; then
        echo "GCC: $(gcc --version | head -1)" | tee -a "${LOG_FILE}"
    fi
    if command -v clang &> /dev/null; then
        echo "Clang: $(clang --version | head -1)" | tee -a "${LOG_FILE}"
    fi
    if command -v make &> /dev/null; then
        echo "Make: $(make --version | head -1)" | tee -a "${LOG_FILE}"
    fi
    if command -v pkg-config &> /dev/null; then
        echo "pkg-config: $(pkg-config --version)" | tee -a "${LOG_FILE}"
    fi
    echo "" | tee -a "${LOG_FILE}"
    
    # Environment variables
    echo "Build Environment Variables:" | tee -a "${LOG_FILE}"
    echo "CC=${CC:-not set}" | tee -a "${LOG_FILE}"
    echo "CXX=${CXX:-not set}" | tee -a "${LOG_FILE}"
    echo "CFLAGS=${CFLAGS:-not set}" | tee -a "${LOG_FILE}"
    echo "CXXFLAGS=${CXXFLAGS:-not set}" | tee -a "${LOG_FILE}"
    echo "CPPFLAGS=${CPPFLAGS:-not set}" | tee -a "${LOG_FILE}"
    echo "LDFLAGS=${LDFLAGS:-not set}" | tee -a "${LOG_FILE}"
    echo "MAKEFLAGS=${MAKEFLAGS:-not set}" | tee -a "${LOG_FILE}"
    echo "" | tee -a "${LOG_FILE}"
}

# Check dependencies
check_dependencies() {
    log_info "Checking build dependencies..."
    
    local missing_deps=()
    local required_commands=("gcc" "make" "autoconf" "automake" "libtool" "pkg-config")
    
    for cmd in "${required_commands[@]}"; do
        if ! command -v "${cmd}" &> /dev/null; then
            missing_deps+=("${cmd}")
        fi
    done
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        return 1
    fi
    
    # Check for X11 development libraries
    log_info "Checking X11 development libraries..."
    if ! pkg-config --exists x11 xt xmu xpm; then
        log_error "Missing X11 development libraries"
        log_info "Available packages:"
        pkg-config --list-all | grep -i x11 | head -10 | tee -a "${LOG_FILE}" || true
        return 1
    fi
    
    log_success "All dependencies found"
    return 0
}

# Parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --incremental)
                INCREMENTAL=true
                shift
                ;;
            --no-tests)
                NO_TESTS=true
                shift
                ;;
            --optimize)
                OPTIMIZE=true
                shift
                ;;
            --no-deps)
                NO_DEPS=true
                shift
                ;;
            --jobs)
                shift
                JOBS="$1"
                shift
                ;;
            --clean)
                CLEAN_BUILD=true
                shift
                ;;
            --debug)
                BUILD_TYPE="debug"
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Set default jobs if not specified
    if [[ -z "${JOBS}" ]]; then
        JOBS=$(nproc 2>/dev/null || echo "2")
    fi
    
    # Apply optimizations
    if [[ "${OPTIMIZE}" == "true" ]]; then
        export CFLAGS="-O3 -march=native -mtune=native ${CFLAGS:-}"
        export CXXFLAGS="-O3 -march=native -mtune=native ${CXXFLAGS:-}"
        export MAKEFLAGS="-j${JOBS} ${MAKEFLAGS:-}"
    fi
    
    log_info "Build options:"
    log_info "  Type: ${BUILD_TYPE}"
    log_info "  Incremental: ${INCREMENTAL}"
    log_info "  Skip tests: ${NO_TESTS}"
    log_info "  Optimized: ${OPTIMIZE}"
    log_info "  Skip deps: ${NO_DEPS}"
    log_info "  Jobs: ${JOBS}"
    log_info "  Clean build: ${CLEAN_BUILD}"
}

# Show help
show_help() {
    cat << EOF
Motif Build Script for Container Environments

Usage: $(basename "$0") [OPTIONS]

Options:
  --incremental    Build incrementally (skip tests, reuse build artifacts)
  --no-tests       Skip running tests after build
  --optimize       Use aggressive optimization flags
  --no-deps        Skip dependency checking
  --jobs N         Number of parallel build jobs (default: auto-detect)
  --clean          Clean build (remove previous build artifacts)
  --debug          Build with debug symbols
  --help, -h       Show this help message

Examples:
  $(basename "$0")                    # Standard build
  $(basename "$0") --incremental      # Incremental build
  $(basename "$0") --jobs 8           # Build with 8 parallel jobs
  $(basename "$0") --optimize         # Optimized build
  $(basename "$0") --no-tests         # Build without tests
EOF
}

# Prepare source code
prepare_source() {
    log_info "Preparing source code..."
    
    if [[ ! -d "${MOTIF_SOURCE}" ]]; then
        log_error "Motif source directory not found: ${MOTIF_SOURCE}"
        return 1
    fi
    
    # Handle incremental builds with simple copy approach
    if [[ "${INCREMENTAL}" == "true" ]]; then
        log_info "📦 Incremental build requested"
        
        # Check if there's a build cache on the host
        local host_cache="/motif/.motif-build-cache"
        if [[ -d "${host_cache}" ]]; then
            log_info "📦 Found build cache on host, copying into container"
            
            # Copy the cache into the build directory
            if cp -r "${host_cache}" "${BUILD_DIR}" 2>/dev/null; then
                log_info "✅ Build cache copied successfully"
                
                # Check if we can reuse the copied build directory
                if [[ -f "${BUILD_DIR}/Makefile" ]]; then
                    log_info "✅ Found existing Makefile, attempting to reuse build directory"
                    cd "${BUILD_DIR}"
                    
                    # Check if we can run make
                    if make -n all >/dev/null 2>&1; then
                        log_info "🚀 Build directory is usable, proceeding with incremental build"
                        return 0
                    else
                        log_info "⚠️  Build directory exists but is not usable, will reconfigure"
                        # Don't remove, just reconfigure
                    fi
                else
                    log_info "🔄 No Makefile found in cache, will do full build"
                fi
            else
                log_info "⚠️  Failed to copy build cache, will do full build"
            fi
        else
            log_info "🔄 No build cache found on host, will do full build"
        fi
    fi
    
    # Clean build if requested
    if [[ "${CLEAN_BUILD}" == "true" ]]; then
        log_info "🧹 Clean build requested, removing previous build artifacts"
        rm -rf "${BUILD_DIR}"
    fi
    
    # Copy source to build location
    log_info "📁 Copying source code to build directory..."
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    
    # Use rsync if available, otherwise cp
    if command -v rsync &> /dev/null; then
        log_info "📦 Using rsync for efficient file copying"
        rsync -a --exclude='.git' --exclude='*.o' --exclude='*.lo' --exclude='*.la' \
              --exclude='Makefile' --exclude='config.log' --exclude='config.status' \
              --exclude='libtool' --exclude='.libs' --exclude='.deps' \
              "${MOTIF_SOURCE}/" "${BUILD_DIR}/" | tee -a "${LOG_FILE}"
    else
        log_info "📦 Using cp for file copying"
        cp -r "${MOTIF_SOURCE}"/* "${BUILD_DIR}/" | tee -a "${LOG_FILE}"
    fi
    
    cd "${BUILD_DIR}"
    
    log_info "✅ Source code prepared in: ${BUILD_DIR}"
    return 0
}

# Configure build
configure_build() {
    log_info "Configuring build..."
    
    cd "${BUILD_DIR}"
    
    # Run autogen if needed
    if [[ -f autogen.sh && ! -f configure ]]; then
        log_info "Running autogen.sh..."
        ./autogen.sh 2>&1 | tee -a "${LOG_FILE}"
    fi
    
    # Check if configure script exists
    if [[ ! -f configure ]]; then
        log_error "Configure script not found"
        return 1
    fi
    
    # Configure with standard options
    log_info "Running configure..."
    
    # Set environment variables to prioritize system headers
    export CPPFLAGS="-I/usr/include ${CPPFLAGS:-}"
    export CFLAGS="-I/usr/include ${CFLAGS:-}"
    
    local configure_opts=(
        "--prefix=${INSTALL_DIR}"
        "--enable-shared"
        "--disable-static"
        "--with-x"
        "--enable-xft"
        "--enable-jpeg"
        "--enable-png"
    )
    
    # Add debug options in development builds
    if [[ "${BUILD_TYPE:-release}" == "debug" ]]; then
        configure_opts+=(
            "--enable-debug"
            "--enable-debug-themes"
        )
    fi
    
    log_debug "Configure options: ${configure_opts[*]}"
    
    ./configure "${configure_opts[@]}" 2>&1 | tee -a "${LOG_FILE}"
    
    log_success "Configuration completed"
    return 0
}

# Build Motif
build_motif() {
    log_info "Building Motif..."
    
    cd "${BUILD_DIR}"
    
    # Use specified jobs or auto-detect
    local num_cores="${JOBS:-$(nproc 2>/dev/null || echo "2")}"
    log_info "Building with ${num_cores} parallel jobs"
    
    # Check if this is an incremental build
    if [[ "${INCREMENTAL}" == "true" ]]; then
        local obj_count=$(find . -name "*.o" 2>/dev/null | wc -l)
        local lib_count=$(find . -name "*.la" 2>/dev/null | wc -l)
        
        log_info "📦 Incremental build status:"
        log_info "   Object files: ${obj_count}"
        log_info "   Library files: ${lib_count}"
        
        if [[ ${obj_count} -gt 100 ]]; then
            log_info "🚀 Most components already built, this should be fast!"
        elif [[ ${obj_count} -gt 50 ]]; then
            log_info "⚡ Partial incremental build, some components will be rebuilt"
        else
            log_info "🔄 Minimal incremental build, most components will be rebuilt"
        fi
    fi
    
    # Build the project
    log_info "🔨 Starting compilation..."
    make -j"${num_cores}" 2>&1 | tee -a "${LOG_FILE}"
    
    log_success "Build completed"
    return 0
}

# Install Motif
install_motif() {
    log_info "Installing Motif..."
    
    cd "${BUILD_DIR}"
    
    # Create install directory
    mkdir -p "${INSTALL_DIR}"
    
    # Install
    make install 2>&1 | tee -a "${LOG_FILE}"
    
    log_success "Installation completed"
    return 0
}

# Run tests
run_tests() {
    if [[ "${NO_TESTS}" == "true" ]]; then
        log_info "Skipping tests (--no-tests flag)"
        return 0
    fi
    
    log_info "Running tests..."
    
    cd "${BUILD_DIR}"
    
    # Check if tests are available
    if make -n check &>/dev/null; then
        log_info "Running test suite..."
        make check 2>&1 | tee -a "${LOG_FILE}" || {
            log_warning "Some tests failed, but continuing..."
        }
    else
        log_info "No test suite available"
    fi
    
    # Basic installation verification
    log_info "Verifying installation..."
    
    # Check if libraries were installed
    if [[ -d "${INSTALL_DIR}/lib" ]]; then
        log_info "Installed libraries:"
        find "${INSTALL_DIR}/lib" -name "*.so*" -o -name "*.a" | sort | tee -a "${LOG_FILE}"
    fi
    
    # Check if headers were installed
    if [[ -d "${INSTALL_DIR}/include" ]]; then
        log_info "Installed headers:"
        find "${INSTALL_DIR}/include" -name "*.h" | wc -l | xargs echo "Header files:" | tee -a "${LOG_FILE}"
    fi
    
    # Check if binaries were installed
    if [[ -d "${INSTALL_DIR}/bin" ]]; then
        log_info "Installed binaries:"
        find "${INSTALL_DIR}/bin" -type f -executable | sort | tee -a "${LOG_FILE}"
    fi
    
    return 0
}

# Generate build report
generate_report() {
    log_info "Generating build report..."
    
    local report_file="/home/builder/build-report.txt"
    
    cat > "${report_file}" << EOF
Motif Build Report
==================
Date: $(date)
Host: $(hostname 2>/dev/null || echo 'unknown')
User: $(whoami)

Build Configuration:
- Source: ${MOTIF_SOURCE}
- Build Directory: ${BUILD_DIR}
- Install Directory: ${INSTALL_DIR}
- Compiler: ${CC:-gcc}
- C++ Compiler: ${CXX:-g++}
- CFLAGS: ${CFLAGS:-default}
- CXXFLAGS: ${CXXFLAGS:-default}
- LDFLAGS: ${LDFLAGS:-default}

Build Status: ${BUILD_STATUS:-UNKNOWN}
Build Time: ${BUILD_TIME:-UNKNOWN}

EOF
    
    if [[ -f "${LOG_FILE}" ]]; then
        echo "" >> "${report_file}"
        echo "Build Log Summary:" >> "${report_file}"
        echo "==================" >> "${report_file}"
        
        # Extract errors and warnings
        if grep -i "error" "${LOG_FILE}" > /dev/null; then
            echo "" >> "${report_file}"
            echo "Errors found:" >> "${report_file}"
            grep -i "error" "${LOG_FILE}" | tail -20 >> "${report_file}"
        fi
        
        if grep -i "warning" "${LOG_FILE}" > /dev/null; then
            echo "" >> "${report_file}"
            echo "Warnings found:" >> "${report_file}"
            grep -i "warning" "${LOG_FILE}" | wc -l | xargs echo "Total warnings:" >> "${report_file}"
        fi
    fi
    
    log_info "Build report saved to: ${report_file}"
    
    # Display summary
    echo "" | tee -a "${LOG_FILE}"
    echo "Build Summary" | tee -a "${LOG_FILE}"
    echo "=============" | tee -a "${LOG_FILE}"
    cat "${report_file}" | tee -a "${LOG_FILE}"
}

# Cleanup function
cleanup() {
    log_info "Cleaning up..."
    
    # Keep build artifacts for debugging
    log_info "Build artifacts preserved in: ${BUILD_DIR}"
    log_info "Installation preserved in: ${INSTALL_DIR}"
    log_info "Build log: ${LOG_FILE}"
}

# Save build cache for incremental builds
save_build_cache() {
    if [[ "${INCREMENTAL}" != "true" ]]; then
        return 0
    fi
    
    log_info "📦 Saving build cache for incremental builds..."
    
    # Create cache directory on host
    local host_cache="/motif/.motif-build-cache"
    mkdir -p "${host_cache}"
    
    # Copy build artifacts to host cache
    if cp -r "${BUILD_DIR}"/* "${host_cache}/" 2>/dev/null; then
        log_info "✅ Build cache saved successfully"
        log_info "   Cache location: ${host_cache}"
        
        # Show what was cached
        local obj_count=$(find "${host_cache}" -name "*.o" 2>/dev/null | wc -l)
        local lib_count=$(find "${host_cache}" -name "*.la" 2>/dev/null | wc -l)
        log_info "   Cached: ${obj_count} object files, ${lib_count} library files"
    else
        log_warning "⚠️  Failed to save build cache"
    fi
}

# Main function
main() {
    local start_time=$(date +%s)
    local exit_code=0
    
    # Parse command line arguments
    parse_arguments "$@"
    
    # Initialize log file
    echo "Motif Build Log" > "${LOG_FILE}"
    echo "===============" >> "${LOG_FILE}"
    echo "Started: $(date)" >> "${LOG_FILE}"
    echo "" >> "${LOG_FILE}"
    
    log_info "Starting Motif build process..."
    
    # Execute build steps
    get_system_info || exit_code=$?
    
    if [[ ${exit_code} -eq 0 ]]; then
        check_dependencies || exit_code=$?
    fi
    
    if [[ ${exit_code} -eq 0 ]]; then
        prepare_source || exit_code=$?
    fi
    
    if [[ ${exit_code} -eq 0 ]]; then
        configure_build || exit_code=$?
    fi
    
    if [[ ${exit_code} -eq 0 ]]; then
        build_motif || exit_code=$?
    fi
    
    if [[ ${exit_code} -eq 0 ]]; then
        install_motif || exit_code=$?
    fi
    
         if [[ ${exit_code} -eq 0 ]]; then
         run_tests || exit_code=$?
     fi
     
     # Save build cache for incremental builds
     if [[ ${exit_code} -eq 0 && "${INCREMENTAL}" == "true" ]]; then
         save_build_cache
     fi
     
     # Calculate build time
    local end_time=$(date +%s)
    local build_time=$((end_time - start_time))
    export BUILD_TIME="${build_time} seconds"
    
    # Set build status
    if [[ ${exit_code} -eq 0 ]]; then
        export BUILD_STATUS="SUCCESS"
        log_success "Motif build completed successfully in ${build_time} seconds"
    else
        export BUILD_STATUS="FAILED"
        log_error "Motif build failed after ${build_time} seconds"
    fi
    
    # Generate report
    generate_report
    
    # Cleanup
    cleanup
    
    echo "" | tee -a "${LOG_FILE}"
    echo "Build completed: $(date)" | tee -a "${LOG_FILE}"
    echo "Exit code: ${exit_code}" | tee -a "${LOG_FILE}"
    
    exit ${exit_code}
}

# Handle signals
trap cleanup EXIT

# Run main function
main "$@"
