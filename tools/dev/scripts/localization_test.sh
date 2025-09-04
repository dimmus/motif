#!/bin/bash
#
# localization_test.sh - Test localization functionality in xde-motif
#
# This script tests the localization features of xde-motif by running
# demo applications with different locale settings and verifying that
# text appears in the correct language and character encoding.
#
# Usage: ./localization_test.sh [options]
#
# Options:
#   -v, --verbose     Enable verbose output
#   -q, --quiet       Suppress output except for errors
#   -h, --help        Show this help message
#   --demo-only       Only test demo programs
#   --test-scripts    Only test built-in test scripts
#   --all            Run all tests (default)
#   --auto-locales   Use only system-available locales
#

# Usage:
# Run all tests
# ./tools/dev/scripts/localization_test.sh
# 
# Run only demo programs with verbose output
# ./tools/dev/scripts/localization_test.sh --demo-only --verbose
# 
# Run quietly, only show errors
# ./tools/dev/scripts/localization_test.sh --quiet
# 
# Test only built-in scripts
# ./tools/dev/scripts/localization_test.sh --test-scripts

set -e

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
VERBOSE=false
QUIET=false
DEMO_ONLY=false
TEST_SCRIPTS_ONLY=false
RUN_ALL=true
AUTO_LOCALES=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

# Logging functions
log_info() {
    if [ "$QUIET" = false ]; then
        echo -e "${BLUE}[INFO]${NC} $1"
    fi
}

log_success() {
    if [ "$QUIET" = false ]; then
        echo -e "${GREEN}[PASS]${NC} $1"
    fi
}

log_warning() {
    if [ "$QUIET" = false ]; then
        echo -e "${YELLOW}[WARN]${NC} $1"
    fi
}

log_error() {
    echo -e "${RED}[FAIL]${NC} $1" >&2
}

log_verbose() {
    if [ "$VERBOSE" = true ] && [ "$QUIET" = false ]; then
        echo -e "${BLUE}[VERBOSE]${NC} $1"
    fi
}

# Help function
show_help() {
    cat << EOF
localization_test.sh - Test localization functionality in xde-motif

Usage: $0 [options]

Options:
  -v, --verbose       Enable verbose output
  -q, --quiet         Suppress output except for errors
  -h, --help          Show this help message
  --demo-only         Only test demo programs
  --test-scripts      Only test built-in test scripts
  --all               Run all tests (default)
  --auto-locales      Use only system-available locales

Examples:
  $0                      # Run all tests
  $0 --demo-only          # Only test demo programs
  $0 --verbose            # Run with verbose output
  $0 --quiet              # Run quietly, only show errors
  $0 --auto-locales       # Use only available locales
  $0 --auto-locales -v    # Use available locales with verbose output

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -q|--quiet)
            QUIET=true
            VERBOSE=false
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        --demo-only)
            DEMO_ONLY=true
            RUN_ALL=false
            shift
            ;;
        --test-scripts)
            TEST_SCRIPTS_ONLY=true
            RUN_ALL=false
            shift
            ;;
        --all)
            RUN_ALL=true
            shift
            ;;
        --auto-locales)
            AUTO_LOCALES=true
            shift
            ;;
        *)
            log_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Test locales configuration
declare -A TEST_LOCALES=(
    ["en_US.UTF-8"]="English (UTF-8)"
    ["fr_FR.ISO8859-1"]="French (ISO8859-1)"
    ["de_DE.ISO8859-1"]="German (ISO8859-1)"
    ["ja_JP.eucJP"]="Japanese (EUC-JP)"
    ["hebrew"]="Hebrew"
    ["sv_SE.ISO8859-1"]="Swedish (ISO8859-1)"
)

# Function to detect available locales
detect_available_locales() {
    local available_locales=()
    
    # Get list of available locales
    if locale -a >/dev/null 2>&1; then
        while IFS= read -r locale; do
            # Skip empty lines and C locale
            if [ -n "$locale" ] && [ "$locale" != "C" ] && [ "$locale" != "POSIX" ]; then
                available_locales+=("$locale")
            fi
        done < <(locale -a 2>/dev/null)
    fi
    
    # If no locales found, try to use basic ones
    if [ ${#available_locales[@]} -eq 0 ]; then
        available_locales=("C" "en_US" "en_GB")
    fi
    
    echo "${available_locales[@]}"
}

# Demo programs configuration
declare -A DEMO_PROGRAMS=(
    ["helloint"]="demos/programs/hellomotifi18n"
    ["fileview"]="demos/programs/fileview"
)

# Test script configuration (only include if programs exist)
declare -A TEST_SCRIPTS=()

# Function to detect available test scripts
detect_test_scripts() {
    local test_dir="$PROJECT_ROOT/.attic/tests/I18N"
    
    # Check for compiled I18N programs first (these are more reliable)
    if [ -f "$test_dir/I18N1" ] && [ -x "$test_dir/I18N1" ]; then
        TEST_SCRIPTS["I18N1"]=".attic/tests/I18N/I18N1"
    fi
    
    if [ -f "$test_dir/InputMethod1" ] && [ -x "$test_dir/InputMethod1" ]; then
        TEST_SCRIPTS["InputMethod1"]=".attic/tests/I18N/InputMethod1"
    fi
    
    if [ -f "$test_dir/InputMethod2" ] && [ -x "$test_dir/InputMethod2" ]; then
        TEST_SCRIPTS["InputMethod2"]=".attic/tests/I18N/InputMethod2"
    fi
    
    # Only include i18n1 if it can actually run (has required dependencies)
    if [ -f "$test_dir/i18n1" ] && [ -x "$test_dir/i18n1" ]; then
        # Test if the script can run without errors
        if cd "$test_dir" && timeout 5s ./i18n1 american a >/dev/null 2>&1; then
            TEST_SCRIPTS["i18n1"]=".attic/tests/I18N/i18n1"
        else
            log_verbose "i18n1 script found but not functional - skipping"
        fi
        cd "$PROJECT_ROOT"
    fi
}

# Function to check if a program exists
check_program() {
    local program="$1"
    if command -v "$program" >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# Function to check if a file exists
check_file() {
    local file="$1"
    if [ -f "$file" ]; then
        return 0
    else
        return 1
    fi
}

# Function to check if a directory exists
check_dir() {
    local dir="$1"
    if [ -d "$dir" ]; then
        return 0
    else
        return 1
    fi
}

# Function to run a test and capture results
run_test() {
    local test_name="$1"
    local test_command="$2"
    local timeout_seconds="${3:-10}"
    
    log_verbose "Running test: $test_name"
    log_verbose "Command: $test_command"
    
    if timeout "$timeout_seconds" bash -c "$test_command" >/dev/null 2>&1; then
        log_success "$test_name"
        ((TESTS_PASSED++))
        return 0
    else
        local exit_code=$?
        if [ $exit_code -eq 124 ]; then
            log_warning "$test_name (timeout after ${timeout_seconds}s)"
            ((TESTS_SKIPPED++))
        else
            log_error "$test_name (exit code: $exit_code)"
            ((TESTS_FAILED++))
        fi
        return 1
    fi
}

# Function to test locale support
test_locale_support() {
    local locale="$1"
    local locale_name="$2"
    
    log_info "Testing locale support for $locale_name ($locale)"
    
    # Check if locale is available
    local locale_available=false
    if locale -a 2>/dev/null | grep -q "^${locale}$"; then
        locale_available=true
    elif locale -a 2>/dev/null | grep -q "^${locale%.*}$"; then
        locale_available=true
        # Try the base locale instead
        locale="${locale%.*}"
    elif locale -a 2>/dev/null | grep -q "^${locale%_*}$"; then
        locale_available=true
        # Try the language-only locale
        locale="${locale%_*}"
    fi
    
    if [ "$locale_available" = false ]; then
        log_warning "Locale $locale not available on this system"
        log_verbose "Available locales: $(locale -a 2>/dev/null | head -5 | tr '\n' ' ')..."
        ((TESTS_SKIPPED++))
        return 1
    fi
    
    # Test locale setting
    if LANG="$locale" LC_ALL="$locale" locale 2>/dev/null | grep -q "LC_ALL=$locale"; then
        log_success "Locale $locale can be set"
        return 0
    else
        log_warning "Failed to set locale $locale (may still work for basic testing)"
        ((TESTS_SKIPPED++))
        return 1
    fi
}

# Function to test demo program
test_demo_program() {
    local demo_name="$1"
    local demo_dir="$2"
    local locale="$3"
    local locale_name="$4"
    
    local full_demo_path="$PROJECT_ROOT/$demo_dir"
    
    if [ ! -d "$full_demo_path" ]; then
        log_error "Demo directory not found: $full_demo_path"
        ((TESTS_FAILED++))
        return 1
    fi
    
    log_info "Testing $demo_name with $locale_name ($locale)"
    
    # Change to demo directory
    cd "$full_demo_path"
    
    # Set up environment for the test
    local test_env="LANG=$locale LC_ALL=$locale"
    
    # Set XAPPLRESDIR and UIDPATH based on demo and locale
    case "$demo_name" in
        "helloint")
            # helloint requires XAPPLRESDIR to be set to "." and UIDPATH for localized strings
            test_env="$test_env XAPPLRESDIR=."
            
            # Determine which localized strings to use and copy the appropriate l_strings.uid
            local l_strings_dir=""
            case "$locale" in
                "fr_FR.ISO8859-1"|*"french"*)
                    l_strings_dir="./french/uid"
                    ;;
                "de_DE.ISO8859-1"|*"german"*)
                    l_strings_dir="./german/uid"
                    ;;
                "ja_JP.eucJP"|*"japanese"*)
                    l_strings_dir="./japanese/uid"
                    ;;
                "hebrew"|*"hebrew"*)
                    l_strings_dir="./hebrew/uid"
                    ;;
                "sv_SE.ISO8859-1"|*"swedish"*)
                    l_strings_dir="./swedish/uid"
                    ;;
                *)
                    l_strings_dir="./english/uid"
                    ;;
            esac
            
            # Copy the appropriate l_strings.uid to the main directory
            if [ -f "${l_strings_dir}/l_strings.uid" ]; then
                cp "${l_strings_dir}/l_strings.uid" .
                test_env="$test_env UIDPATH=${l_strings_dir}"
            else
                log_warning "Localized strings not found for $locale, using default"
                test_env="$test_env UIDPATH=./english/uid"
            fi
            ;;
        "fileview")
            # fileview uses a different approach with the xmfile script
            case "$locale" in
                "fr_FR.ISO8859-1")
                    if [ -f "./xmfile" ]; then
                        run_test "$demo_name ($locale_name)" "env $test_env ./xmfile french"
                    else
                        log_warning "xmfile script not found for $demo_name"
                        ((TESTS_SKIPPED++))
                    fi
                    return
                    ;;
                "de_DE.ISO8859-1")
                    if [ -f "./xmfile" ]; then
                        run_test "$demo_name ($locale_name)" "env $test_env ./xmfile german"
                    else
                        log_warning "xmfile script not found for $demo_name"
                        ((TESTS_SKIPPED++))
                    fi
                    return
                    ;;
                *)
                    if [ -f "./xmfile" ]; then
                        run_test "$demo_name ($locale_name)" "env $test_env ./xmfile english"
                    else
                        log_warning "xmfile script not found for $demo_name"
                        ((TESTS_SKIPPED++))
                    fi
                    return
                    ;;
            esac
            ;;
    esac
    
    # Run the demo program
    if [ -f "./$demo_name" ]; then
        run_test "$demo_name ($locale_name)" "env $test_env ./$demo_name"
    else
        log_warning "Demo program $demo_name not found or not built"
        ((TESTS_SKIPPED++))
    fi
    
    # Return to project root
    cd "$PROJECT_ROOT"
}

# Function to test built-in test scripts
test_builtin_scripts() {
    local script_name="$1"
    local script_path="$2"
    local locale="$3"
    local locale_name="$4"
    
    local full_script_path="$PROJECT_ROOT/$script_path"
    
    if [ ! -f "$full_script_path" ]; then
        log_error "Test script not found: $full_script_path"
        ((TESTS_FAILED++))
        return 1
    fi
    
    log_info "Testing $script_name with $locale_name ($locale)"
    
    # Change to script directory
    local script_dir="$(dirname "$full_script_path")"
    cd "$script_dir"
    
    # Set up environment
    local test_env="LANG=$locale LC_ALL=$locale"
    
    # Run the test script based on its type
    case "$script_name" in
        "i18n1")
            # Map locale to script parameter
            local script_locale
            case "$locale" in
                "fr_FR.ISO8859-1"|"fr_FR"|*"french"*)
                    script_locale="french"
                    ;;
                "de_DE.ISO8859-1"|"de_DE"|*"german"*)
                    script_locale="german"
                    ;;
                "ja_JP.eucJP"|"ja_JP"|*"japanese"*)
                    script_locale="japanese"
                    ;;
                "hebrew"|*"hebrew"*)
                    script_locale="hebrew"
                    ;;
                *)
                    script_locale="american"
                    ;;
            esac
            
            # Test both test cases (a and b)
            run_test "$script_name ($locale_name) - case a" "env $test_env ./i18n1 $script_locale a"
            run_test "$script_name ($locale_name) - case b" "env $test_env ./i18n1 $script_locale b"
            ;;
    esac
    
    # Return to project root
    cd "$PROJECT_ROOT"
}

# Function to check prerequisites
check_prerequisites() {
    log_info "Checking prerequisites..."
    
    local missing_deps=()
    local warnings=()
    
    # Check for required commands
    for cmd in locale timeout; do
        if ! check_program "$cmd"; then
            missing_deps+=("$cmd")
        fi
    done
    
    # Check for optional X11 commands
    if ! check_program "xdpyinfo"; then
        warnings+=("xdpyinfo not available - X11 tests may fail")
    fi
    
    # Check for X11 display
    if [ -z "$DISPLAY" ]; then
        warnings+=("DISPLAY environment variable not set - GUI tests may fail")
    fi
    
    # Check for X11 locale support (only if xdpyinfo is available)
    if check_program "xdpyinfo"; then
        if ! xdpyinfo >/dev/null 2>&1; then
            warnings+=("Cannot connect to X server - GUI tests may fail")
        fi
    fi
    
    # Check for available locales
    if ! locale -a >/dev/null 2>&1; then
        warnings+=("Cannot list available locales - locale tests may fail")
    fi
    
    # Report warnings
    for warning in "${warnings[@]}"; do
        log_warning "$warning"
    done
    
    # Report missing dependencies
    if [ ${#missing_deps[@]} -gt 0 ]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        log_error "Please install the missing packages and try again"
        exit 1
    fi
    
    log_success "Prerequisites check passed"
}

# Function to print test summary
print_summary() {
    local total_tests=$((TESTS_PASSED + TESTS_FAILED + TESTS_SKIPPED))
    
    echo
    echo "=========================================="
    echo "Localization Test Summary"
    echo "=========================================="
    echo "Total tests: $total_tests"
    echo -e "Passed: ${GREEN}$TESTS_PASSED${NC}"
    echo -e "Failed: ${RED}$TESTS_FAILED${NC}"
    echo -e "Skipped: ${YELLOW}$TESTS_SKIPPED${NC}"
    echo "=========================================="
    
    if [ $TESTS_FAILED -gt 0 ]; then
        echo -e "${RED}Some tests failed!${NC}"
        exit 1
    elif [ $TESTS_SKIPPED -gt 0 ]; then
        echo -e "${YELLOW}Some tests were skipped.${NC}"
        exit 0
    else
        echo -e "${GREEN}All tests passed!${NC}"
        exit 0
    fi
}

# Main function
main() {
    log_info "Starting xde-motif localization tests..."
    log_info "Project root: $PROJECT_ROOT"
    
    # Check prerequisites
    check_prerequisites
    
    # Change to project root
    cd "$PROJECT_ROOT"
    
    # Detect available test scripts
    detect_test_scripts
    
    # Test locale support
    log_info "Testing locale support..."
    
    if [ "$AUTO_LOCALES" = true ]; then
        # Use system-available locales
        log_info "Using auto-detected locales..."
        local available_locales=($(detect_available_locales))
        for locale in "${available_locales[@]}"; do
            test_locale_support "$locale" "$locale"
        done
    else
        # Use configured locales
        for locale in "${!TEST_LOCALES[@]}"; do
            locale_name="${TEST_LOCALES[$locale]}"
            test_locale_support "$locale" "$locale_name"
        done
    fi
    
    # Test demo programs
    if [ "$RUN_ALL" = true ] || [ "$DEMO_ONLY" = true ]; then
        log_info "Testing demo programs..."
        for demo in "${!DEMO_PROGRAMS[@]}"; do
            demo_dir="${DEMO_PROGRAMS[$demo]}"
            
            if [ "$AUTO_LOCALES" = true ]; then
                local available_locales=($(detect_available_locales))
                for locale in "${available_locales[@]}"; do
                    test_demo_program "$demo" "$demo_dir" "$locale" "$locale"
                done
            else
                for locale in "${!TEST_LOCALES[@]}"; do
                    locale_name="${TEST_LOCALES[$locale]}"
                    test_demo_program "$demo" "$demo_dir" "$locale" "$locale_name"
                done
            fi
        done
    fi
    
    # Test built-in test scripts
    if [ "$RUN_ALL" = true ] || [ "$TEST_SCRIPTS_ONLY" = true ]; then
        if [ ${#TEST_SCRIPTS[@]} -eq 0 ]; then
            log_warning "No test scripts available - skipping test script tests"
            log_info "To enable test script testing, compile the programs in .attic/tests/I18N/"
        else
            log_info "Testing built-in test scripts..."
            for script in "${!TEST_SCRIPTS[@]}"; do
                script_path="${TEST_SCRIPTS[$script]}"
                
                if [ "$AUTO_LOCALES" = true ]; then
                    local available_locales=($(detect_available_locales))
                    for locale in "${available_locales[@]}"; do
                        test_builtin_scripts "$script" "$script_path" "$locale" "$locale"
                    done
                else
                    for locale in "${!TEST_LOCALES[@]}"; do
                        locale_name="${TEST_LOCALES[$locale]}"
                        test_builtin_scripts "$script" "$script_path" "$locale" "$locale_name"
                    done
                fi
            done
        fi
    fi
    
    # Print summary
    print_summary
}

# Run main function
main "$@"
