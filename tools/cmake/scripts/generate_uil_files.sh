#!/bin/bash

# Generate UIL parser and lexer files
# This script generates UilLexPars.c, UilLexPars.h, and UilLexAna.c from the WML grammar files

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

# Check if required tools are available
if ! command -v bison >/dev/null 2>&1; then
    echo "Error: bison is required but not installed"
    exit 1
fi

if ! command -v flex >/dev/null 2>&1; then
    echo "Error: flex is required but not installed"
    exit 1
fi

# Generate UIL parser using Bison
echo "Generating UIL parser from $UIL_YACC_FILE"
cd "$OUTPUT_DIR"
bison -d -o UilLexPars.c "$UIL_YACC_FILE"

echo "UIL files generated successfully:"
echo "  - $OUTPUT_DIR/UilLexPars.c"
echo "  - $OUTPUT_DIR/UilLexPars.h"