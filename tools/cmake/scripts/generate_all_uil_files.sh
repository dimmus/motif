#!/bin/bash

# Generate all UIL-related files (parser, lexer, and WML targets)
# This script handles the complete UIL file generation process

set -e

# Get the source and build directories
SOURCE_DIR="$1"
BUILD_DIR="$2"

if [ -z "$SOURCE_DIR" ] || [ -z "$BUILD_DIR" ]; then
    echo "Usage: $0 <source_dir> <build_dir>"
    exit 1
fi

# Set up paths
WML_DIR="$SOURCE_DIR/src/bin/wml"
UIL_YACC_FILE="$WML_DIR/Uil.y"
UIL_LEX_FILE="$WML_DIR/wmluiltok.l"
OUTPUT_DIR="$BUILD_DIR/src/bin/wml"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Copy stub UilSymGen.h first to break circular dependency
echo "Copying stub UilSymGen.h to break circular dependency"
cp "${SOURCE_DIR}/src/bin/wml/UilSymGen.h.stub" "${OUTPUT_DIR}/src/lib/Uil/UilSymGen.h"

# Check if required tools are available
if ! command -v bison >/dev/null 2>&1; then
    echo "Error: bison is required but not installed"
    exit 1
fi

if ! command -v flex >/dev/null 2>&1; then
    echo "Error: flex is required but not installed"
    exit 1
fi

# Now generate WML target files if wml and wmluiltok are available
if [ -f "$OUTPUT_DIR/wml" ] && [ -f "$OUTPUT_DIR/wmluiltok" ]; then
    echo "Generating WML target files..."
    
    # Generate tokens.dat
    "$OUTPUT_DIR/wmluiltok" < "$UIL_YACC_FILE" > "$OUTPUT_DIR/tokens.dat"
    
    # Generate WML target files
    cd "$OUTPUT_DIR"
    "$OUTPUT_DIR/wml" "$WML_DIR/motif.wml"
    
    echo "WML target files generated successfully"
else
    echo "Warning: wml or wmluiltok not found, skipping WML target generation"
fi
