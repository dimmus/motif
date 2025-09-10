#!/bin/bash

# Test Setup Verification Script
# This script verifies that the test environment is properly set up

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_HOME="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "Motif Test Setup Verification"
echo "=========================================="
echo "TESTS_HOME: $TESTS_HOME"
echo ""

# Check if test programs exist and are executable
echo "Checking test programs..."
cd "$SCRIPT_DIR/XmString"

if [ -x "./StringExt" ]; then
    echo "✅ StringExt executable found and executable"
    echo "   Size: $(ls -lh StringExt | awk '{print $5}')"
    echo "   Date: $(ls -l StringExt | awk '{print $6, $7, $8}')"
else
    echo "❌ StringExt not found or not executable"
fi

# Check if enhanced test library exists
if [ -f "./testlib_enhanced.c" ]; then
    echo "✅ Enhanced test library found"
    echo "   Size: $(ls -lh testlib_enhanced.c | awk '{print $5}')"
else
    echo "❌ Enhanced test library not found"
fi

# Check if Motif libraries are built
echo ""
echo "Checking Motif libraries..."
if [ -f "$TESTS_HOME/lib/Xm/.libs/libXm.so" ]; then
    echo "✅ libXm.so found"
    echo "   Size: $(ls -lh $TESTS_HOME/lib/Xm/.libs/libXm.so | awk '{print $5}')"
else
    echo "❌ libXm.so not found"
fi

if [ -f "$TESTS_HOME/lib/Mrm/.libs/libMrm.so" ]; then
    echo "✅ libMrm.so found"
    echo "   Size: $(ls -lh $TESTS_HOME/lib/Mrm/.libs/libMrm.so | awk '{print $5}')"
else
    echo "❌ libMrm.so not found"
fi

# Check environment variables
echo ""
echo "Checking environment variables..."
if [ -n "${MOTIF_HOME:-}" ]; then
    echo "✅ MOTIF_HOME set to: $MOTIF_HOME"
else
    echo "⚠️  MOTIF_HOME not set"
fi

if [ -n "${LIBRARY_PATH:-}" ]; then
    echo "✅ LIBRARY_PATH set (length: ${#LIBRARY_PATH})"
else
    echo "⚠️  LIBRARY_PATH not set"
fi

if [ -n "${LD_LIBRARY_PATH:-}" ]; then
    echo "✅ LD_LIBRARY_PATH set (length: ${#LD_LIBRARY_PATH})"
else
    echo "⚠️  LD_LIBRARY_PATH not set"
fi

# Check for X11 libraries
echo ""
echo "Checking X11 libraries..."
if ldconfig -p | grep -q libX11; then
    echo "✅ libX11 found in system libraries"
else
    echo "⚠️  libX11 not found in system libraries"
fi

if ldconfig -p | grep -q libXt; then
    echo "✅ libXt found in system libraries"
else
    echo "⚠️  libXt not found in system libraries"
fi

if ldconfig -p | grep -q libXm; then
    echo "✅ libXm found in system libraries"
else
    echo "⚠️  libXm not found in system libraries (using local build)"
fi

# Check for imake
echo ""
echo "Checking build tools..."
if command -v imake >/dev/null 2>&1; then
    echo "✅ imake found: $(which imake)"
else
    echo "⚠️  imake not found"
fi

# Check display environment
echo ""
echo "Checking display environment..."
if [ -n "${DISPLAY:-}" ]; then
    echo "✅ DISPLAY set to: $DISPLAY"
    if xset q >/dev/null 2>&1; then
        echo "✅ X11 server accessible"
    else
        echo "⚠️  X11 server not accessible"
    fi
else
    echo "⚠️  DISPLAY not set (required for graphical tests)"
fi

# Test compilation
echo ""
echo "Testing compilation..."
cd "$SCRIPT_DIR/XmString"
if make StringExt >/dev/null 2>&1; then
    echo "✅ StringExt compiles successfully"
else
    echo "❌ StringExt compilation failed"
fi

echo ""
echo "=========================================="
echo "Test Setup Verification Complete"
echo "=========================================="
echo ""
echo "Summary:"
echo "- Test programs: $(ls -1 *Ext *Comp *Draw *Width 2>/dev/null | wc -l) found"
echo "- Motif libraries: $(ls -1 $TESTS_HOME/lib/*/.libs/*.so 2>/dev/null | wc -l) built"
echo "- Environment: $(if [ -n "${MOTIF_HOME:-}" ] && [ -n "${LIBRARY_PATH:-}" ]; then echo "configured"; else echo "needs setup"; fi)"
echo "- Display: $(if [ -n "${DISPLAY:-}" ]; then echo "available"; else echo "not available"; fi)"
echo ""
echo "To run tests with display:"
echo "  export DISPLAY=:0.0"
echo "  ./run_graphical_tests.sh"
echo ""
echo "To run individual test:"
echo "  cd XmString && ./StringExt"
