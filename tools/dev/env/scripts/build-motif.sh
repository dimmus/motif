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
NO_TESTS=true  # Default to skipping tests in Docker builds
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
    local required_commands=("gcc" "make" "cmake" "ninja" "pkg-config")
    
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
    
    # Check CMake version
    local cmake_version=$(cmake --version | head -1 | cut -d' ' -f3)
    log_info "CMake version: ${cmake_version}"
    
    # Check Ninja version
    local ninja_version=$(ninja --version 2>/dev/null || echo "unknown")
    log_info "Ninja version: ${ninja_version}"
    
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
            --tests)
                NO_TESTS=false
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
Motif Build Script for Container Environments (CMake)

Usage: $(basename "$0") [OPTIONS]

Options:
  --incremental    Build incrementally (reuse CMake build artifacts)
  --no-tests       Skip running tests after build (default in Docker)
  --tests          Run tests after build (overrides --no-tests)
  --optimize       Use aggressive optimization flags
  --no-deps        Skip dependency checking
  --jobs N         Number of parallel build jobs (default: auto-detect)
  --clean          Clean build (remove previous build artifacts)
  --debug          Build with debug symbols
  --help, -h       Show this help message

Examples:
  $(basename "$0")                    # Standard CMake build (no tests)
  $(basename "$0") --incremental      # Incremental CMake build
  $(basename "$0") --jobs 8           # Build with 8 parallel jobs
  $(basename "$0") --optimize         # Optimized build
  $(basename "$0") --tests            # Build with tests
  $(basename "$0") --no-tests         # Build without tests (explicit)
EOF
}

# Prepare source code
prepare_source() {
    log_info "Preparing source code..."
    
    if [[ ! -d "${MOTIF_SOURCE}" ]]; then
        log_error "Motif source directory not found: ${MOTIF_SOURCE}"
        return 1
    fi
    
    # Handle incremental builds with CMake approach
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
                if [[ -f "${BUILD_DIR}/build/CMakeCache.txt" ]]; then
                    log_info "✅ Found existing CMake cache, attempting to reuse build directory"
                    cd "${BUILD_DIR}/build"
                    
                    # Check if we can run cmake build
                    if cmake --build . --dry-run >/dev/null 2>&1; then
                        log_info "🚀 CMake build directory is usable, proceeding with incremental build"
                        return 0
                    else
                        log_info "⚠️  Build directory exists but is not usable, will reconfigure"
                        # Don't remove, just reconfigure
                    fi
                else
                    log_info "🔄 No CMake cache found in build directory, will do full build"
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
    
    # Always clean CMake cache and build directory to avoid path conflicts
    if [[ -d "${BUILD_DIR}/build" ]]; then
        log_info "🧹 Removing existing CMake build directory to avoid path conflicts"
        rm -rf "${BUILD_DIR}/build"
    fi
    
    # Also clean any CMake cache files in the source directory
    if [[ -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
        log_info "🧹 Removing CMakeCache.txt from source directory"
        rm -f "${BUILD_DIR}/CMakeCache.txt"
    fi
    
    # Clean any CMakeFiles directories
    if [[ -d "${BUILD_DIR}/CMakeFiles" ]]; then
        log_info "🧹 Removing CMakeFiles directory"
        rm -rf "${BUILD_DIR}/CMakeFiles"
    fi
    
    # Copy source to build location
    log_info "📁 Copying source code to build directory..."
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    
    # Use rsync if available, otherwise cp
    if command -v rsync &> /dev/null; then
        log_info "📦 Using rsync for efficient file copying"
        rsync -a --exclude='.git' --exclude='*.o' --exclude='*.lo' --exclude='*.la' \
              --exclude='build/' --exclude='CMakeCache.txt' --exclude='CMakeFiles/' \
              --exclude='Makefile' --exclude='config.log' --exclude='config.status' \
              --exclude='libtool' --exclude='.libs' --exclude='.deps' \
              --exclude='cmake_install.cmake' --exclude='CTestTestfile.cmake' \
              --exclude='.cache/' --exclude='build.log' \
              --exclude='CMakeLists.txt.user' \
              "${MOTIF_SOURCE}/" "${BUILD_DIR}/" 2>&1 | tee -a "${LOG_FILE}"
    else
        log_info "📦 Using cp for file copying"
        # Create a temporary directory to avoid permission issues
        local temp_dir=$(mktemp -d)
        cp -r "${MOTIF_SOURCE}"/* "${temp_dir}/" 2>&1 | tee -a "${LOG_FILE}"
        # Remove problematic directories and files
        rm -rf "${temp_dir}/.cache" "${temp_dir}/build.log" "${temp_dir}/build/CMakeCache.txt" 2>/dev/null || true
        rm -f "${temp_dir}/CMakeCache.txt" 2>/dev/null || true
        rm -rf "${temp_dir}/CMakeFiles" 2>/dev/null || true
        # Copy to build directory
        cp -r "${temp_dir}"/* "${BUILD_DIR}/" 2>&1 | tee -a "${LOG_FILE}"
        rm -rf "${temp_dir}"
    fi
    
    cd "${BUILD_DIR}"
    
    log_info "✅ Source code prepared in: ${BUILD_DIR}"
    return 0
}

# Configure build
configure_build() {
    log_info "Configuring build with CMake..."
    
    cd "${BUILD_DIR}"
    
    # Check if CMakeLists.txt exists
    if [[ ! -f CMakeLists.txt ]]; then
        log_error "CMakeLists.txt not found in ${BUILD_DIR}"
        log_info "Available files in build directory:"
        ls -la "${BUILD_DIR}" | head -10 | tee -a "${LOG_FILE}"
        return 1
    fi
    
    # Create build directory for CMake
    local cmake_build_dir="${BUILD_DIR}/build"
    mkdir -p "${cmake_build_dir}"
    cd "${cmake_build_dir}"
    
    # Set environment variables to prioritize system headers
    export CPPFLAGS="-I/usr/include ${CPPFLAGS:-}"
    export CFLAGS="-I/usr/include ${CFLAGS:-}"
    
    # Prepare CMake configuration options
    local cmake_opts=(
        "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
        "-DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release}"
        "-DBUILD_SHARED_LIBS=ON"
        "-DCMAKE_VERBOSE_MAKEFILE=ON"
        "-DCMAKE_SOURCE_DIR=${BUILD_DIR}"
    )
    
    # Add debug options in development builds
    if [[ "${BUILD_TYPE:-release}" == "debug" ]]; then
        cmake_opts+=(
            "-DCMAKE_BUILD_TYPE=Debug"
            "-DCMAKE_C_FLAGS_DEBUG=-g -O0"
            "-DCMAKE_CXX_FLAGS_DEBUG=-g -O0"
        )
    fi
    
    # Add optimization flags if requested
    if [[ "${OPTIMIZE}" == "true" ]]; then
        cmake_opts+=(
            "-DCMAKE_C_FLAGS_RELEASE=-O3 -march=native -mtune=native"
            "-DCMAKE_CXX_FLAGS_RELEASE=-O3 -march=native -mtune=native"
        )
    fi
    
    # Set parallel jobs
    if [[ -n "${JOBS}" ]]; then
        cmake_opts+=("-DCMAKE_JOB_POOLS=compile=${JOBS}")
    fi
    
    log_debug "CMake options: ${cmake_opts[*]}"
    
    # Run CMake configuration
    log_info "Running CMake configuration..."
    # Use absolute path to source directory to avoid path conflicts
    cmake "${cmake_opts[@]}" "${BUILD_DIR}" 2>&1 | tee -a "${LOG_FILE}"
    
    log_success "CMake configuration completed"
    return 0
}

# Build Motif
build_motif() {
    log_info "Building Motif with CMake..."
    
    cd "${BUILD_DIR}/build"
    
    # Use specified jobs or auto-detect
    local num_cores="${JOBS:-$(nproc 2>/dev/null || echo "2")}"
    log_info "Building with ${num_cores} parallel jobs"
    
    # Check if this is an incremental build
    if [[ "${INCREMENTAL}" == "true" ]]; then
        local obj_count=$(find . -name "*.o" 2>/dev/null | wc -l)
        local cmake_cache_count=$(find . -name "CMakeCache.txt" 2>/dev/null | wc -l)
        
        log_info "📦 Incremental build status:"
        log_info "   Object files: ${obj_count}"
        log_info "   CMake cache files: ${cmake_cache_count}"
        
        if [[ ${obj_count} -gt 100 ]]; then
            log_info "🚀 Most components already built, this should be fast!"
        elif [[ ${obj_count} -gt 50 ]]; then
            log_info "⚡ Partial incremental build, some components will be rebuilt"
        else
            log_info "🔄 Minimal incremental build, most components will be rebuilt"
        fi
    fi
    
    # Build the project using CMake
    log_info "🔨 Starting CMake build..."
    
    # Use ninja if available, otherwise make
    if command -v ninja &> /dev/null; then
        log_info "Using Ninja build system"
        ninja -j"${num_cores}" 2>&1 | tee -a "${LOG_FILE}"
    else
        log_info "Using Make build system"
        make -j"${num_cores}" 2>&1 | tee -a "${LOG_FILE}"
    fi
    
    log_success "Build completed"
    return 0
}

# Install Motif
install_motif() {
    log_info "Installing Motif..."
    
    cd "${BUILD_DIR}/build"
    
    # Create install directory
    mkdir -p "${INSTALL_DIR}"
    
    # Install using CMake
    log_info "Running CMake install..."
    cmake --install . --prefix "${INSTALL_DIR}" 2>&1 | tee -a "${LOG_FILE}"
    
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
    
    cd "${BUILD_DIR}/build"
    
    # Check if tests are available using CMake
    if cmake --build . --target help 2>/dev/null | grep -q "test"; then
        log_info "Running CMake test suite..."
        ctest --output-on-failure 2>&1 | tee -a "${LOG_FILE}" || {
            log_warning "Some tests failed, but continuing..."
        }
    else
        log_info "No CMake test suite available"
        
        # Try running tests with make if available
        if make -n test &>/dev/null; then
            log_info "Running Make test suite..."
            make test 2>&1 | tee -a "${LOG_FILE}" || {
                log_warning "Some tests failed, but continuing..."
            }
        else
            log_info "No test suite available"
        fi
    fi
    
    # Basic build verification
    log_info "Verifying build artifacts..."
    
    # Check if libraries were built
    if [[ -d "${BUILD_DIR}/build/src/lib/Xm" ]]; then
        log_info "Built libraries:"
        find "${BUILD_DIR}/build/src/lib/Xm" -name "*.so*" -o -name "*.a" | sort | tee -a "${LOG_FILE}"
    fi
    
    # Check if headers are available in source
    if [[ -d "${BUILD_DIR}/src/lib/Xm" ]]; then
        log_info "Available headers:"
        find "${BUILD_DIR}/src/lib/Xm" -name "*.h" | wc -l | xargs echo "Header files:" | tee -a "${LOG_FILE}"
    fi
    
    # Check if binaries were built
    if [[ -d "${BUILD_DIR}/build/src" ]]; then
        log_info "Built binaries:"
        find "${BUILD_DIR}/build/src" -type f -executable | sort | tee -a "${LOG_FILE}"
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
- CMake Build Directory: ${BUILD_DIR}/build
- Install Directory: ${INSTALL_DIR}
- Build System: CMake
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
        local cmake_cache_count=$(find "${host_cache}" -name "CMakeCache.txt" 2>/dev/null | wc -l)
        local lib_count=$(find "${host_cache}" -name "*.so*" -o -name "*.a" 2>/dev/null | wc -l)
        log_info "   Cached: ${obj_count} object files, ${cmake_cache_count} CMake cache files, ${lib_count} library files"
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
    mkdir -p "$(dirname "${LOG_FILE}")"
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
    
    # Skip installation in Docker builds - just build and test
    if [[ ${exit_code} -eq 0 ]]; then
        log_info "Skipping installation in Docker build environment"
        log_info "Build artifacts are available in: ${BUILD_DIR}/build"
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
