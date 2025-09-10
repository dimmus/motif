#!/bin/bash

# Script to generate Xm string definition files
set -e

SOURCE_DIR="$1"
BUILD_DIR="$2"
MAKESTRS="$3"

if [ -z "$SOURCE_DIR" ] || [ -z "$BUILD_DIR" ] || [ -z "$MAKESTRS" ]; then
    echo "Usage: $0 <source_dir> <build_dir> <makestrs_path>"
    exit 1
fi

# Create directories
mkdir -p "$BUILD_DIR/src/lib/Xm"
mkdir -p "$BUILD_DIR/include/Xm"

# Copy template files
cp "$SOURCE_DIR/src/lib/Xm/xmstring.list" "$BUILD_DIR/src/lib/Xm/"
cp "$SOURCE_DIR/src/lib/Xm/XmStrDefs.ht" "$BUILD_DIR/src/lib/Xm/"
cp "$SOURCE_DIR/src/lib/Xm/XmStrDefs22.ht" "$BUILD_DIR/src/lib/Xm/"
cp "$SOURCE_DIR/src/lib/Xm/XmStrDefs23.ht" "$BUILD_DIR/src/lib/Xm/"
cp "$SOURCE_DIR/src/lib/Xm/XmStrDefsI.ht" "$BUILD_DIR/src/lib/Xm/"
cp "$SOURCE_DIR/src/lib/Xm/XmStrDefs.ct" "$BUILD_DIR/src/lib/Xm/"

# Generate string files
cd "$BUILD_DIR/src/lib/Xm"
"$MAKESTRS" -f xmstring.list > XmStrDefs.c 2>/dev/null

# The makestrs tool should generate the header files directly
# Check if header files were generated, if not, create them from templates
if [ ! -f "XmStrDefs.h" ] || [ ! -s "XmStrDefs.h" ]; then
    sed 's/<<<STRING_TABLE_GOES_HERE>>>/extern char *XmStrings[];/' XmStrDefs.ht > XmStrDefs.h
fi
if [ ! -f "XmStrDefs22.h" ] || [ ! -s "XmStrDefs22.h" ]; then
    sed 's/<<<STRING_TABLE_GOES_HERE>>>/extern char *XmStrings22[];/' XmStrDefs22.ht > XmStrDefs22.h
fi
if [ ! -f "XmStrDefs23.h" ] || [ ! -s "XmStrDefs23.h" ]; then
    sed 's/<<<STRING_TABLE_GOES_HERE>>>/extern char *XmStrings23[];/' XmStrDefs23.ht > XmStrDefs23.h
fi
if [ ! -f "XmStrDefsI.h" ] || [ ! -s "XmStrDefsI.h" ]; then
    sed 's/<<<STRING_TABLE_GOES_HERE>>>/extern char *XmStringsI[];/' XmStrDefsI.ht > XmStrDefsI.h
fi

# Copy generated files to include directory
cp XmStrDefs.h "$BUILD_DIR/include/Xm/"
cp XmStrDefs22.h "$BUILD_DIR/include/Xm/"
cp XmStrDefs23.h "$BUILD_DIR/include/Xm/"
cp XmStrDefsI.h "$BUILD_DIR/include/Xm/"

# Copy generated .c file to lib directory (it stays in lib/Xm)
# The .c file is needed for compilation and should remain in the lib directory

# Generate message catalog header file
if [ -f "$BUILD_DIR/src/bin/utils/mkcatdefs" ]; then
    "$BUILD_DIR/src/bin/utils/mkcatdefs" "$BUILD_DIR/include/Xm/XmMsgCatI.h" "$SOURCE_DIR/localized/C/msg/Xm.msg" >/dev/null 2>&1
elif [ -f "$SOURCE_DIR/src/bin/utils/mkcatdefs" ]; then
    "$SOURCE_DIR/src/bin/utils/mkcatdefs" "$BUILD_DIR/include/Xm/XmMsgCatI.h" "$SOURCE_DIR/localized/C/msg/Xm.msg" >/dev/null 2>&1
else
    # Fallback to stub file if mkcatdefs is not available
    cp "$SOURCE_DIR/src/lib/Xm/XmMsgCatI.h" "$BUILD_DIR/include/Xm/"
fi

# Generated Xm string definition files successfully
