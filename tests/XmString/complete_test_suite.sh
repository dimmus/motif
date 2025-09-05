#!/bin/bash

# Complete XmString Test Suite Runner
# Integrates all test automation components

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_HOME="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
VERBOSE=false
BUILD_TESTS=true
RUN_TESTS=true
PROCESS_DATA=true
GENERATE_REPORTS=true
DISPLAY_SET=""

print_header() {
    echo -e "${BLUE}==========================================${NC}"
    echo -e "${BLUE}Complete XmString Test Suite Runner${NC}"
    echo -e "${BLUE}==========================================${NC}"
    echo "Test Directory: $SCRIPT_DIR"
    echo "Motif Home: $TESTS_HOME"
    echo ""
}

print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -v, --verbose           Enable verbose output"
    echo "  --no-build              Skip building tests"
    echo "  --no-run                Skip running tests"
    echo "  --no-process            Skip processing .dat/.scr files"
    echo "  --no-reports            Skip generating reports"
    echo "  -d, --display DISP      Set DISPLAY variable"
    echo "  --build-only            Only build tests, don't run"
    echo "  --process-only          Only process test data"
    echo "  --run-only              Only run tests (assume already built)"
    echo ""
    echo "Examples:"
    echo "  $0                      # Complete test suite"
    echo "  $0 --build-only         # Only build tests"
    echo "  $0 --run-only           # Only run tests"
    echo "  $0 -d :0.0 -v           # Run with specific display, verbose"
}

setup_environment() {
    echo -e "${CYAN}Setting up environment...${NC}"
    
    # Set up Motif environment
    export MOTIF_HOME="$TESTS_HOME"
    export LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:${LIBRARY_PATH:-}"
    export LD_LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:${LD_LIBRARY_PATH:-}"
    export C_INCLUDE_PATH="$TESTS_HOME/include:$TESTS_HOME/lib:${C_INCLUDE_PATH:-}"
    export C_PATH="$TESTS_HOME/lib:${C_PATH:-}"
    
    if [ -n "$DISPLAY_SET" ]; then
        export DISPLAY="$DISPLAY_SET"
    fi
    
    echo -e "${GREEN}✓ Environment configured${NC}"
}

build_tests() {
    if [ "$BUILD_TESTS" = false ]; then
        echo -e "${YELLOW}Skipping test build${NC}"
        return 0
    fi
    
    echo -e "${CYAN}Building XmString tests...${NC}"
    
    cd "$SCRIPT_DIR"
    
    # Build all test programs
    local build_errors=0
    
    for test in FontListA FontListC StringComp StringCopy StringEmpty StringGLR StringConcat StringDisp StringDraw StringExt StringNext StringParse StringRend StringTab StringToByte StringToCT StringWidth RendToProp; do
        echo -n "Building $test... "
        if make "$test" >/dev/null 2>&1; then
            echo -e "${GREEN}✓${NC}"
        else
            echo -e "${RED}✗${NC}"
            ((build_errors++))
        fi
    done
    
    if [ $build_errors -eq 0 ]; then
        echo -e "${GREEN}✓ All tests built successfully${NC}"
    else
        echo -e "${RED}✗ $build_errors tests failed to build${NC}"
        return 1
    fi
}

process_test_data() {
    if [ "$PROCESS_DATA" = false ]; then
        echo -e "${YELLOW}Skipping test data processing${NC}"
        return 0
    fi
    
    echo -e "${CYAN}Processing test data files...${NC}"
    
    cd "$SCRIPT_DIR"
    
    # Run the Python data processor
    if command -v python3 >/dev/null 2>&1; then
        python3 process_test_data.py "$SCRIPT_DIR"
        echo -e "${GREEN}✓ Test data processed${NC}"
    else
        echo -e "${YELLOW}Python3 not found, skipping data processing${NC}"
    fi
}

run_tests() {
    if [ "$RUN_TESTS" = false ]; then
        echo -e "${YELLOW}Skipping test execution${NC}"
        return 0
    fi
    
    echo -e "${CYAN}Running XmString tests...${NC}"
    
    cd "$SCRIPT_DIR"
    
    # Use our test runner
    if [ -x "./run_xmstring_tests.sh" ]; then
        ./run_xmstring_tests.sh
    else
        echo -e "${RED}Test runner not found or not executable${NC}"
        return 1
    fi
}

generate_reports() {
    if [ "$GENERATE_REPORTS" = false ]; then
        echo -e "${YELLOW}Skipping report generation${NC}"
        return 0
    fi
    
    echo -e "${CYAN}Generating test reports...${NC}"
    
    cd "$SCRIPT_DIR"
    
    # Generate summary report
    cat > test_summary.md << EOF
# XmString Test Suite Summary

Generated: $(date)

## Test Status

EOF
    
    # Check which tests are built
    local built_tests=0
    local total_tests=0
    
    for test in FontListA FontListC StringComp StringCopy StringEmpty StringGLR StringConcat StringDisp StringDraw StringExt StringNext StringParse StringRend StringTab StringToByte StringToCT StringWidth RendToProp; do
        ((total_tests++))
        if [ -x "./$test" ]; then
            echo "- ✅ $test" >> test_summary.md
            ((built_tests++))
        else
            echo "- ❌ $test" >> test_summary.md
        fi
    done
    
    cat >> test_summary.md << EOF

## Statistics

- Total tests: $total_tests
- Built tests: $built_tests
- Success rate: $(( built_tests * 100 / total_tests ))%

## Files

- Test executables: \`./*\`
- Test data files: \`*.dat\`
- Test scripts: \`*.scr\`
- Test outputs: \`test_outputs/*.prt\`
- Test results: \`test_results/*.log\`

## Usage

To run individual tests:
\`\`\`bash
./StringExt
./StringComp
# etc.
\`\`\`

To run all tests:
\`\`\`bash
./run_xmstring_tests.sh
\`\`\`

To run with specific display:
\`\`\`bash
DISPLAY=:0.0 ./run_xmstring_tests.sh
\`\`\`
EOF
    
    echo -e "${GREEN}✓ Reports generated${NC}"
}

print_final_summary() {
    echo ""
    echo -e "${BLUE}==========================================${NC}"
    echo -e "${BLUE}Test Suite Complete${NC}"
    echo -e "${BLUE}==========================================${NC}"
    echo ""
    echo -e "${GREEN}Available test programs:${NC}"
    ls -la *Ext *Comp *Draw *Width *Concat *Copy *Empty *GLR *Next *Parse *Rend *Tab *ToByte *ToCT *RendToProp FontListA FontListC 2>/dev/null | while read line; do
        echo "  $line"
    done
    echo ""
    echo -e "${GREEN}Test automation files:${NC}"
    echo "  - run_xmstring_tests.sh (main test runner)"
    echo "  - process_test_data.py (data processor)"
    echo "  - test_summary.md (summary report)"
    echo "  - test_data.json (structured test data)"
    echo ""
    echo -e "${GREEN}To run tests:${NC}"
    echo "  ./run_xmstring_tests.sh"
    echo "  ./run_xmstring_tests.sh --list"
    echo "  ./run_xmstring_tests.sh -t StringExt"
    echo ""
}

# Parse command line arguments
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
        --no-build)
            BUILD_TESTS=false
            shift
            ;;
        --no-run)
            RUN_TESTS=false
            shift
            ;;
        --no-process)
            PROCESS_DATA=false
            shift
            ;;
        --no-reports)
            GENERATE_REPORTS=false
            shift
            ;;
        -d|--display)
            DISPLAY_SET="$2"
            shift 2
            ;;
        --build-only)
            RUN_TESTS=false
            PROCESS_DATA=false
            GENERATE_REPORTS=false
            shift
            ;;
        --process-only)
            BUILD_TESTS=false
            RUN_TESTS=false
            GENERATE_REPORTS=false
            shift
            ;;
        --run-only)
            BUILD_TESTS=false
            PROCESS_DATA=false
            GENERATE_REPORTS=false
            shift
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

# Setup environment
setup_environment

# Build tests
build_tests

# Process test data
process_test_data

# Run tests
run_tests

# Generate reports
generate_reports

# Print final summary
print_final_summary
