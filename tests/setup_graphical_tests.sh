#!/bin/bash

# Comprehensive Motif Test Setup Script
# Sets up the complete graphical test environment

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=========================================="
echo "Comprehensive Motif Test Setup"
echo "=========================================="

# Check prerequisites
echo "Checking prerequisites..."

# Check for X11
if [ -z "${DISPLAY:-}" ]; then
    echo "WARNING: No DISPLAY set. Tests require graphical environment."
fi

# Check for required libraries
echo "Checking for required libraries..."

if ! ldconfig -p | grep -q libXm; then
    echo "WARNING: libXm not found in system libraries"
fi

if ! ldconfig -p | grep -q libXt; then
    echo "WARNING: libXt not found in system libraries"
fi

if ! ldconfig -p | grep -q libX11; then
    echo "WARNING: libX11 not found in system libraries"
fi

echo "Prerequisites check completed"
echo ""

# Set up environment
echo "Setting up environment..."

export MOTIF_HOME="$SCRIPT_DIR"
export LIBRARY_PATH="/usr/X11R6/lib:$SCRIPT_DIR/lib/Xm/.libs:$SCRIPT_DIR/tests/auto/lib/common:$SCRIPT_DIR/tests/auto/lib/Create:$SCRIPT_DIR/tests/auto/lib/Script:$SCRIPT_DIR/tests/auto/lib/Synth:$SCRIPT_DIR/tests/auto/lib/Util:$SCRIPT_DIR/tests/auto/lib/Visual:${LIBRARY_PATH:-}"
export LD_LIBRARY_PATH="/usr/X11R6/lib:$SCRIPT_DIR/lib/Xm/.libs:$SCRIPT_DIR/tests/auto/lib/common:$SCRIPT_DIR/tests/auto/lib/Create:$SCRIPT_DIR/tests/auto/lib/Script:$SCRIPT_DIR/tests/auto/lib/Synth:$SCRIPT_DIR/tests/auto/lib/Util:$SCRIPT_DIR/tests/auto/lib/Visual:${LD_LIBRARY_PATH:-}"
export C_INCLUDE_PATH="$SCRIPT_DIR/include:$SCRIPT_DIR/lib:${C_INCLUDE_PATH:-}"
export C_PATH="$SCRIPT_DIR/lib:${C_PATH:-}"

echo "Environment variables set"
echo ""

# Build Motif libraries if needed
if [ ! -f "$SCRIPT_DIR/lib/Xm/.libs/libXm.so" ]; then
    echo "Building Motif libraries..."
    cd "$SCRIPT_DIR/lib"
    make clean || true
    make -j$(nproc) || {
        echo "ERROR: Failed to build Motif libraries"
        exit 1
    }
    echo "Motif libraries built"
else
    echo "Motif libraries already built"
fi

echo ""
echo "=========================================="
echo "Test Environment Setup Complete"
echo "=========================================="
echo ""
echo "To run tests:"
echo "  cd tests"
echo "  ./run_graphical_tests.sh"
echo ""
echo "To run individual tests:"
echo "  cd tests/XmString"
echo "  ./StringExt"
echo ""
