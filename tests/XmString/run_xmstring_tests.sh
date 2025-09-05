#!/bin/bash

# XmString Test Automation Runner
# Modern replacement for the original RUN script

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_HOME="$(dirname "$SCRIPT_DIR")"

# Colors for output - detect if terminal supports colors
if [ -t 1 ] && command -v tput >/dev/null 2>&1 && [ "$(tput colors 2>/dev/null || echo 0)" -ge 8 ]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    BLUE='\033[0;34m'
    NC='\033[0m' # No Color
else
    RED=''
    GREEN=''
    YELLOW=''
    BLUE=''
    NC=''
fi

# Test configuration
MODE="compare"  # compare, record, nocompare
VERBOSE=false
DISPLAY_SET=""
OUTPUT_DIR="$SCRIPT_DIR/test_outputs"
RESULTS_DIR="$SCRIPT_DIR/test_results"

# Test programs list
TEST_PROGRAMS=(
    "FontListA"
    "FontListC" 
    "StringComp"
    "StringCopy"
    "StringEmpty"
    "StringGLR"
    "StringConcat"
    "StringDisp"
    "StringDraw"
    "StringExt"
    "StringNext"
    "StringParse"
    "StringRend"
    "StringTab"
    "StringToByte"
    "StringToCT"
    "StringWidth"
    "RendToProp"
)

# Statistics
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0

# Functions
print_header() {
    echo -e "${BLUE}==========================================${NC}"
    echo -e "${BLUE}XmString Test Automation Runner${NC}"
    echo -e "${BLUE}==========================================${NC}"
    echo "Mode: $MODE"
    echo "Tests Directory: $SCRIPT_DIR"
    echo "Output Directory: $OUTPUT_DIR"
    echo "Results Directory: $RESULTS_DIR"
    echo ""
}

print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -v, --verbose       Enable verbose output"
    echo "  -r, --record        Record mode (generate .prt files)"
    echo "  -c, --compare       Compare mode (default)"
    echo "  -n, --nocompare     No comparison mode"
    echo "  -d, --display DISP  Set DISPLAY variable"
    echo "  -o, --output DIR    Set output directory"
    echo "  -t, --test NAME     Run specific test only"
    echo "  --list              List available tests"
    echo ""
    echo "Examples:"
    echo "  $0                          # Run all tests in compare mode"
    echo "  $0 -r                       # Record mode (generate reference outputs)"
    echo "  $0 -t StringExt             # Run only StringExt test"
    echo "  $0 -d :0.0 -v               # Run with specific display, verbose"
}

setup_directories() {
    mkdir -p "$OUTPUT_DIR"
    mkdir -p "$RESULTS_DIR"
    
    # Clean up previous runs
    rm -f "$OUTPUT_DIR"/*.prt
    rm -f "$OUTPUT_DIR"/*.err
    rm -f "$RESULTS_DIR"/*.log
}

check_display() {
    if [ -z "${DISPLAY:-}" ] && [ -z "$DISPLAY_SET" ]; then
        echo -e "${YELLOW}WARNING: No DISPLAY set. Tests require graphical environment.${NC}"
        echo "Set DISPLAY with: export DISPLAY=:0.0"
        echo "Or use: $0 -d :0.0"
        return 1
    fi
    
    if [ -n "$DISPLAY_SET" ]; then
        export DISPLAY="$DISPLAY_SET"
    fi
    
    return 0
}

check_test_executable() {
    local test_name="$1"
    local test_path="$SCRIPT_DIR/$test_name"
    
    if [ ! -f "$test_path" ]; then
        echo -e "${RED}ERROR: Test executable '$test_name' not found${NC}"
        return 1
    fi
    
    if [ ! -x "$test_path" ]; then
        echo -e "${RED}ERROR: Test executable '$test_name' is not executable${NC}"
        return 1
    fi
    
    return 0
}

run_single_test() {
    local test_name="$1"
    local test_path="$SCRIPT_DIR/$test_name"
    local output_file="$OUTPUT_DIR/${test_name}.prt"
    local error_file="$OUTPUT_DIR/${test_name}.err"
    local log_file="$RESULTS_DIR/${test_name}.log"
    
    echo -e "${BLUE}Running test: $test_name${NC}"
    
    # Check if test exists and is executable
    if ! check_test_executable "$test_name"; then
        ((SKIPPED_TESTS++))
        return 1
    fi
    
    # Run the test
    local start_time=$(date +%s)
    
    if [ "$VERBOSE" = true ]; then
        echo "Command: $test_path"
        echo "Output: $output_file"
        echo "Error: $error_file"
    fi
    
    # Run test and capture output
    # Use xvfb-run for virtual display if no DISPLAY is set
    local test_command="$test_path"
    if [ -z "${DISPLAY:-}" ]; then
        test_command="xvfb-run -a $test_path"
    fi
    
    # Provide input to handle pause prompts
    if echo -e "\n\n\n\n\n" | timeout 30 $test_command > "$output_file" 2> "$error_file"; then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        
        echo -e "${GREEN}✓ $test_name passed (${duration}s)${NC}"
        ((PASSED_TESTS++))
        
        # Log success
        echo "$(date): $test_name PASSED (${duration}s)" >> "$log_file"
        
        return 0
    else
        local exit_code=$?
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        
        echo -e "${RED}✗ $test_name failed (exit code: $exit_code, ${duration}s)${NC}"
        ((FAILED_TESTS++))
        
        # Log failure
        echo "$(date): $test_name FAILED (exit code: $exit_code, ${duration}s)" >> "$log_file"
        
        # Show error output if verbose
        if [ "$VERBOSE" = true ] && [ -s "$error_file" ]; then
            echo "Error output:"
            cat "$error_file"
        fi
        
        return 1
    fi
}

compare_outputs() {
    local test_name="$1"
    local current_output="$OUTPUT_DIR/${test_name}.prt"
    local reference_output="$SCRIPT_DIR/${test_name}.prt"
    local diff_file="$OUTPUT_DIR/${test_name}.diff"
    
    if [ ! -f "$reference_output" ]; then
        echo -e "${YELLOW}No reference output found for $test_name${NC}"
        return 1
    fi
    
    if diff "$reference_output" "$current_output" > "$diff_file" 2>&1; then
        echo -e "${GREEN}✓ $test_name output matches reference${NC}"
        rm -f "$diff_file"
        return 0
    else
        echo -e "${RED}✗ $test_name output differs from reference${NC}"
        if [ "$VERBOSE" = true ]; then
            echo "Differences:"
            cat "$diff_file"
        fi
        return 1
    fi
}

process_test() {
    local test_name="$1"
    ((TOTAL_TESTS++))
    
    case "$MODE" in
        "record")
            run_single_test "$test_name"
            ;;
        "compare")
            run_single_test "$test_name"
            if [ $? -eq 0 ]; then
                compare_outputs "$test_name"
            fi
            ;;
        "nocompare")
            run_single_test "$test_name"
            ;;
    esac
}

run_all_tests() {
    echo -e "${BLUE}Running all XmString tests...${NC}"
    echo ""
    
    for test_name in "${TEST_PROGRAMS[@]}"; do
        process_test "$test_name"
        echo ""
    done
}

run_specific_test() {
    local test_name="$1"
    
    # Check if test exists in our list
    local found=false
    for t in "${TEST_PROGRAMS[@]}"; do
        if [ "$t" = "$test_name" ]; then
            found=true
            break
        fi
    done
    
    if [ "$found" = false ]; then
        echo -e "${RED}ERROR: Unknown test '$test_name'${NC}"
        echo "Available tests: ${TEST_PROGRAMS[*]}"
        exit 1
    fi
    
    echo -e "${BLUE}Running specific test: $test_name${NC}"
    echo ""
    
    process_test "$test_name"
}

list_tests() {
    echo -e "${BLUE}Available XmString tests:${NC}"
    echo ""
    for test_name in "${TEST_PROGRAMS[@]}"; do
        local status=""
        if check_test_executable "$test_name"; then
            status="${GREEN}[READY]${NC}"
        else
            status="${RED}[MISSING]${NC}"
        fi
        echo "  $test_name $status"
    done
}

print_summary() {
    echo ""
    echo -e "${BLUE}==========================================${NC}"
    echo -e "${BLUE}Test Summary${NC}"
    echo -e "${BLUE}==========================================${NC}"
    echo "Total tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    echo -e "Skipped: ${YELLOW}$SKIPPED_TESTS${NC}"
    echo ""
    
    if [ $FAILED_TESTS -eq 0 ] && [ $SKIPPED_TESTS -eq 0 ]; then
        echo -e "${GREEN}All tests completed successfully!${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed or were skipped.${NC}"
        exit 1
    fi
}

# Parse command line arguments
SPECIFIC_TEST=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_usage
            exit 0
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -r|--record)
            MODE="record"
            shift
            ;;
        -c|--compare)
            MODE="compare"
            shift
            ;;
        -n|--nocompare)
            MODE="nocompare"
            shift
            ;;
        -d|--display)
            DISPLAY_SET="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -t|--test)
            SPECIFIC_TEST="$2"
            shift 2
            ;;
        --list)
            list_tests
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Main execution
print_header

# Setup
setup_directories

# Check display
if ! check_display; then
    echo "Continuing without display check..."
fi

# Run tests
if [ -n "$SPECIFIC_TEST" ]; then
    run_specific_test "$SPECIFIC_TEST"
else
    run_all_tests
fi

# Print summary
print_summary
