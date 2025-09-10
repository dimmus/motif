#!/bin/bash

# Motif Graphical Test Runner
# This script runs Motif tests in a graphical environment

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_HOME="$(dirname "$SCRIPT_DIR")"

# Source the enhanced test library
source "$SCRIPT_DIR/XmString/testlib_enhanced.c" 2>/dev/null || true

# Set up environment
export MOTIF_HOME="$TESTS_HOME"
export LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:${LIBRARY_PATH:-}"
export LD_LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:${LD_LIBRARY_PATH:-}"

echo "=========================================="
echo "Motif Graphical Test Runner"
echo "=========================================="
echo "TESTS_HOME: $TESTS_HOME"
echo "DISPLAY: ${DISPLAY:-'not set'}"
echo ""

# Check if we have a display
if [ -z "${DISPLAY:-}" ]; then
    echo "ERROR: No DISPLAY environment variable set."
    echo "Please set DISPLAY (e.g., export DISPLAY=:0.0)"
    exit 1
fi

# Check if X11 is running
if ! xset q >/dev/null 2>&1; then
    echo "ERROR: Cannot connect to X11 server."
    echo "Please ensure X11 is running and accessible."
    exit 1
fi

echo "X11 connection successful"
echo ""

# Run available tests
cd "$SCRIPT_DIR/XmString"

echo "Available test programs:"
ls -la *Ext *Comp *Draw *Width 2>/dev/null || echo "No test programs found"

echo ""
echo "Running StringExt test..."
if [ -x "./StringExt" ]; then
    echo "Executing StringExt..."
    ./StringExt
    echo "StringExt completed"
else
    echo "StringExt not found or not executable"
fi

echo ""
echo "Test run completed"
