#!/bin/bash

# Generate all WML tools and files
# This script handles the complete WML tool generation process

set -e

# Get the source and build directories
SOURCE_DIR="$1"
BUILD_DIR="$2"

if [ -z "$SOURCE_DIR" ] || [ -z "$BUILD_DIR" ]; then
    echo "Usage: $0 <source_dir> <build_dir>"
    exit 1
fi

# Set up paths
WML_DIR="$SOURCE_DIR/tools/wml"
OUTPUT_DIR="$BUILD_DIR/tools/wml"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Check if required tools are available
if ! command -v bison >/dev/null 2>&1; then
    echo "Error: bison is required but not installed"
    exit 1
fi

if ! command -v flex >/dev/null 2>&1; then
    echo "Error: flex is required but not installed"
    exit 1
fi

echo "Generating WML tools..."

cd "$OUTPUT_DIR"

# Generate WML parser using Bison
echo "Generating WML parser from $WML_DIR/wmlparse.y"
bison -d -o wmlparse.c "$WML_DIR/wmlparse.y"

# Generate WML lexer using Flex
echo "Generating WML lexer from $WML_DIR/wmllex.l"
flex -o wmllex.c "$WML_DIR/wmllex.l"

# Generate wmluiltok using Flex
echo "Generating wmluiltok from $WML_DIR/wmluiltok.l"
flex -o wmluiltok.c "$WML_DIR/wmluiltok.l"

echo "WML tools generated successfully:"
echo "  - $OUTPUT_DIR/wmlparse.c"
echo "  - $OUTPUT_DIR/wmlparse.h"
echo "  - $OUTPUT_DIR/wmllex.c"
echo "  - $OUTPUT_DIR/wmluiltok.c"

echo "WML tools generation completed"
