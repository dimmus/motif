#!/bin/bash
#
# Motif Development Environment Test Runner
# Automated testing of Motif building on different operating systems using Docker/Podman
#
# Usage: ./test-motif.sh [OPTIONS] [OS_NAME]
#
# Options:
#   -h, --help          Show this help message
#   -l, --list          List available OS environments
#   -a, --all           Test all available OS environments
#   -c, --clean         Clean containers and images after testing
#   -k, --keep          Keep containers running after test (for debugging)
#   -v, --verbose       Verbose output
#   -r, --rebuild       Force rebuild of container images
#   -p, --podman        Use podman instead of docker
#   --logs-only         Only show logs from previous runs
#
# Examples:
#   ./test-motif.sh archlinux     # Test on Arch Linux
#   ./test-motif.sh freebsd       # Test on FreeBSD
#   ./test-motif.sh --all         # Test on all available OS
#   ./test-motif.sh -v archlinux  # Test with verbose output
#

set -euo pipefail

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../../../" && pwd)"
CONTAINERS_DIR="${SCRIPT_DIR}/containers"
SCRIPTS_DIR="${SCRIPT_DIR}/scripts"
LOGS_DIR="${SCRIPT_DIR}/logs"
TEMPLATES_DIR="${SCRIPT_DIR}/templates"

# Default settings
CONTAINER_ENGINE="docker"
VERBOSE=false
CLEAN_AFTER=false
KEEP_RUNNING=false
REBUILD=false
TEST_ALL=false
LOGS_ONLY=false

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

log_debug() {
    if [[ "${VERBOSE}" == "true" ]]; then
        echo -e "${PURPLE}[DEBUG]${NC} $*"
    fi
}

# Help function
show_help() {
    cat << EOF
Motif Development Environment Test Runner

This script automates the testing of Motif building on different operating systems
using Docker/Podman containers. It creates isolated environments, installs
dependencies, builds Motif, and generates detailed logs.

Usage: $(basename "$0") [OPTIONS] [OS_NAME]

Options:
  -h, --help          Show this help message
  -l, --list          List available OS environments
  -a, --all           Test all available OS environments
  -c, --clean         Clean containers and images after testing
  -k, --keep          Keep containers running after test (for debugging)
  -v, --verbose       Verbose output
  -r, --rebuild       Force rebuild of container images
  -p, --podman        Use podman instead of docker
  --logs-only         Only show logs from previous runs

Available OS environments:
$(list_available_os | sed 's/^/  - /')

Examples:
  $(basename "$0") archlinux              # Test on Arch Linux
  $(basename "$0") freebsd                # Test on FreeBSD
  $(basename "$0") --all                  # Test on all available OS
  $(basename "$0") -v -r archlinux        # Verbose test with rebuild
  $(basename "$0") --logs-only            # Show previous test logs

Logs are saved to: ${LOGS_DIR}/
Container definitions: ${CONTAINERS_DIR}/
EOF
}

# List available OS environments
list_available_os() {
    if [[ -d "${CONTAINERS_DIR}" ]]; then
        find "${CONTAINERS_DIR}" -name "Dockerfile.*" -type f | \
            sed 's/.*Dockerfile\.//' | sort
    else
        echo "No OS environments found. Run with --help for more information."
    fi
}

# Check if container engine is available
check_container_engine() {
    if ! command -v "${CONTAINER_ENGINE}" &> /dev/null; then
        log_error "${CONTAINER_ENGINE} is not installed or not in PATH"
        log_info "Please install ${CONTAINER_ENGINE} or use --podman flag"
        exit 1
    fi
    
    log_debug "Using container engine: ${CONTAINER_ENGINE}"
}

# Create log directory with timestamp
create_log_session() {
    local timestamp=$(date '+%Y%m%d_%H%M%S')
    local session_dir="${LOGS_DIR}/${timestamp}"
    mkdir -p "${session_dir}"
    echo "${session_dir}"
}

# Build container image
build_container() {
    local os_name="$1"
    local dockerfile="${CONTAINERS_DIR}/Dockerfile.${os_name}"
    local image_name="motif-test-${os_name}"
    
    if [[ ! -f "${dockerfile}" ]]; then
        log_error "Dockerfile not found: ${dockerfile}"
        return 1
    fi
    
    log_info "Building container image for ${os_name}..."
    log_debug "Dockerfile: ${dockerfile}"
    log_debug "Image name: ${image_name}"
    
    if [[ "${REBUILD}" == "true" ]]; then
        log_debug "Forcing rebuild (--no-cache)"
        "${CONTAINER_ENGINE}" build --no-cache -f "${dockerfile}" -t "${image_name}" "${PROJECT_ROOT}"
    else
        "${CONTAINER_ENGINE}" build -f "${dockerfile}" -t "${image_name}" "${PROJECT_ROOT}"
    fi
}

# Run test in container
run_test() {
    local os_name="$1"
    local log_session="$2"
    local image_name="motif-test-${os_name}"
    local container_name="motif-test-${os_name}-$(date +%s)"
    local log_file="${log_session}/test_${os_name}.log"
    
    log_info "Running test for ${os_name}..."
    log_debug "Container: ${container_name}"
    log_debug "Log file: ${log_file}"
    
    # Ensure log directory exists
    if ! mkdir -p "$(dirname "${log_file}")"; then
        log_error "Failed to create log directory: $(dirname "${log_file}")"
        return 1
    fi
    
    # Create log file with header
    if ! cat > "${log_file}" << EOF
Motif Build Test Log
===================
OS: ${os_name}
Date: $(date)
Container Engine: ${CONTAINER_ENGINE}
Container: ${container_name}
Image: ${image_name}

EOF
    then
        log_error "Failed to create log file: ${log_file}"
        return 1
    fi
    
    # Run the container
    local exit_code=0
    if [[ "${VERBOSE}" == "true" ]]; then
        "${CONTAINER_ENGINE}" run \
            --name "${container_name}" \
            --rm \
            -v "${PROJECT_ROOT}:/motif:ro" \
            "${image_name}" \
            2>&1 | tee -a "${log_file}" || exit_code=$?
    else
        "${CONTAINER_ENGINE}" run \
            --name "${container_name}" \
            --rm \
            -v "${PROJECT_ROOT}:/motif:ro" \
            "${image_name}" \
            >> "${log_file}" 2>&1 || exit_code=$?
    fi
    
    # Log results
    if [[ -f "${log_file}" ]]; then
        echo "" >> "${log_file}"
        echo "Test completed at: $(date)" >> "${log_file}"
        echo "Exit code: ${exit_code}" >> "${log_file}"
        
        if [[ ${exit_code} -eq 0 ]]; then
            log_success "Test passed for ${os_name}"
            echo "RESULT: SUCCESS" >> "${log_file}"
        else
            log_error "Test failed for ${os_name} (exit code: ${exit_code})"
            echo "RESULT: FAILED" >> "${log_file}"
        fi
    else
        log_warning "Log file does not exist: ${log_file}"
        if [[ ${exit_code} -eq 0 ]]; then
            log_success "Test passed for ${os_name}"
        else
            log_error "Test failed for ${os_name} (exit code: ${exit_code})"
        fi
    fi
    
    return ${exit_code}
}

# Clean up containers and images
cleanup() {
    log_info "Cleaning up containers and images..."
    
    # Remove containers
    local containers=$("${CONTAINER_ENGINE}" ps -a --filter "name=motif-test-" --format "{{.Names}}" 2>/dev/null || true)
    if [[ -n "${containers}" ]]; then
        log_debug "Removing containers: ${containers}"
        echo "${containers}" | xargs -r "${CONTAINER_ENGINE}" rm -f
    fi
    
    # Remove images
    local images=$("${CONTAINER_ENGINE}" images --filter "reference=motif-test-*" --format "{{.Repository}}:{{.Tag}}" 2>/dev/null || true)
    if [[ -n "${images}" ]]; then
        log_debug "Removing images: ${images}"
        echo "${images}" | xargs -r "${CONTAINER_ENGINE}" rmi -f
    fi
    
    log_success "Cleanup completed"
}

# Show logs from previous runs
show_logs() {
    if [[ ! -d "${LOGS_DIR}" ]] || [[ -z "$(ls -A "${LOGS_DIR}" 2>/dev/null)" ]]; then
        log_warning "No previous test logs found"
        return 0
    fi
    
    log_info "Available test sessions:"
    echo ""
    
    for session_dir in "${LOGS_DIR}"/*/; do
        if [[ -d "${session_dir}" ]]; then
            local session_name=$(basename "${session_dir}")
            local session_date=$(echo "${session_name}" | sed 's/_/ /')
            echo -e "${CYAN}Session: ${session_name}${NC} (${session_date})"
            
            for log_file in "${session_dir}"/*.log; do
                if [[ -f "${log_file}" ]]; then
                    local os_name=$(basename "${log_file}" .log | sed 's/test_//')
                    local result=$(tail -1 "${log_file}" | grep "RESULT:" | cut -d: -f2 | tr -d ' ' || echo "UNKNOWN")
                    
                    case "${result}" in
                        "SUCCESS")
                            echo -e "  ${GREEN}✓${NC} ${os_name}"
                            ;;
                        "FAILED")
                            echo -e "  ${RED}✗${NC} ${os_name}"
                            ;;
                        *)
                            echo -e "  ${YELLOW}?${NC} ${os_name}"
                            ;;
                    esac
                fi
            done
            echo ""
        fi
    done
    
    log_info "To view detailed logs: less ${LOGS_DIR}/<session>/<test>.log"
}

# Test single OS
test_single_os() {
    local os_name="$1"
    local log_session="$2"
    
    if [[ ! -f "${CONTAINERS_DIR}/Dockerfile.${os_name}" ]]; then
        log_error "OS environment '${os_name}' not found"
        log_info "Available OS environments:"
        list_available_os | sed 's/^/  - /'
        return 1
    fi
    
    log_info "Testing Motif build on ${os_name}"
    
    # Build container
    if ! build_container "${os_name}"; then
        log_error "Failed to build container for ${os_name}"
        return 1
    fi
    
    # Run test
    if ! run_test "${os_name}" "${log_session}"; then
        log_error "Test failed for ${os_name}"
        return 1
    fi
    
    return 0
}

# Test all available OS
test_all_os() {
    local log_session="$1"
    local available_os=($(list_available_os))
    local failed_tests=()
    local passed_tests=()
    
    if [[ ${#available_os[@]} -eq 0 ]]; then
        log_error "No OS environments found"
        return 1
    fi
    
    log_info "Testing Motif build on all available OS: ${available_os[*]}"
    
    for os_name in "${available_os[@]}"; do
        log_info "Starting test for ${os_name} ($(date))"
        
        if test_single_os "${os_name}" "${log_session}"; then
            passed_tests+=("${os_name}")
        else
            failed_tests+=("${os_name}")
        fi
        
        echo ""  # Add spacing between tests
    done
    
    # Summary
    echo ""
    log_info "Test Summary:"
    echo ""
    
    if [[ ${#passed_tests[@]} -gt 0 ]]; then
        log_success "Passed tests (${#passed_tests[@]}):"
        for os_name in "${passed_tests[@]}"; do
            echo -e "  ${GREEN}✓${NC} ${os_name}"
        done
        echo ""
    fi
    
    if [[ ${#failed_tests[@]} -gt 0 ]]; then
        log_error "Failed tests (${#failed_tests[@]}):"
        for os_name in "${failed_tests[@]}"; do
            echo -e "  ${RED}✗${NC} ${os_name}"
        done
        echo ""
        return 1
    fi
    
    log_success "All tests passed!"
    return 0
}

# Main function
main() {
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -l|--list)
                log_info "Available OS environments:"
                list_available_os | sed 's/^/  - /'
                exit 0
                ;;
            -a|--all)
                TEST_ALL=true
                shift
                ;;
            -c|--clean)
                CLEAN_AFTER=true
                shift
                ;;
            -k|--keep)
                KEEP_RUNNING=true
                shift
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -r|--rebuild)
                REBUILD=true
                shift
                ;;
            -p|--podman)
                CONTAINER_ENGINE="podman"
                shift
                ;;
            --logs-only)
                LOGS_ONLY=true
                shift
                ;;
            -*)
                log_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
            *)
                if [[ -n "${OS_NAME:-}" ]]; then
                    log_error "Multiple OS names specified: ${OS_NAME} and $1"
                    exit 1
                fi
                OS_NAME="$1"
                shift
                ;;
        esac
    done
    
    # Handle logs-only option
    if [[ "${LOGS_ONLY}" == "true" ]]; then
        show_logs
        exit 0
    fi
    
    # Validate arguments
    if [[ "${TEST_ALL}" == "true" && -n "${OS_NAME:-}" ]]; then
        log_error "Cannot specify both --all and OS name"
        exit 1
    fi
    
    if [[ "${TEST_ALL}" == "false" && -z "${OS_NAME:-}" ]]; then
        log_error "Must specify OS name or use --all"
        echo "Use --list to see available OS environments"
        exit 1
    fi
    
    # Check prerequisites
    check_container_engine
    
    # Create directories
    mkdir -p "${LOGS_DIR}"
    
    # Create log session
    local log_session=$(create_log_session)
    log_info "Test session: $(basename "${log_session}")"
    log_info "Logs will be saved to: ${log_session}"
    
    # Run tests
    local exit_code=0
    if [[ "${TEST_ALL}" == "true" ]]; then
        test_all_os "${log_session}" || exit_code=$?
    else
        test_single_os "${OS_NAME}" "${log_session}" || exit_code=$?
    fi
    
    # Cleanup if requested
    if [[ "${CLEAN_AFTER}" == "true" ]]; then
        cleanup
    fi
    
    # Final message
    echo ""
    if [[ ${exit_code} -eq 0 ]]; then
        log_success "All tests completed successfully!"
        log_info "Logs available at: ${log_session}"
    else
        log_error "Some tests failed. Check logs at: ${log_session}"
    fi
    
    exit ${exit_code}
}

# Run main function with all arguments
main "$@"
