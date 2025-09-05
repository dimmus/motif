#!/bin/bash
# Generated automation script for XmString tests

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Starting automated XmString tests..."

echo "Running StringGLR..."
if [ -x "$SCRIPT_DIR/StringGLR" ]; then
    "$SCRIPT_DIR/StringGLR"
    echo "StringGLR completed"
else
    echo "ERROR: StringGLR not executable"
fi

echo "Running StringDisp..."
if [ -x "$SCRIPT_DIR/StringDisp" ]; then
    "$SCRIPT_DIR/StringDisp"
    echo "StringDisp completed"
else
    echo "ERROR: StringDisp not executable"
fi

echo "Running StringDraw..."
if [ -x "$SCRIPT_DIR/StringDraw" ]; then
    "$SCRIPT_DIR/StringDraw"
    echo "StringDraw completed"
else
    echo "ERROR: StringDraw not executable"
fi

echo "Running StringToByte..."
if [ -x "$SCRIPT_DIR/StringToByte" ]; then
    "$SCRIPT_DIR/StringToByte"
    echo "StringToByte completed"
else
    echo "ERROR: StringToByte not executable"
fi

echo "Running StringToCT..."
if [ -x "$SCRIPT_DIR/StringToCT" ]; then
    "$SCRIPT_DIR/StringToCT"
    echo "StringToCT completed"
else
    echo "ERROR: StringToCT not executable"
fi

echo "Running StringConcat..."
if [ -x "$SCRIPT_DIR/StringConcat" ]; then
    "$SCRIPT_DIR/StringConcat"
    echo "StringConcat completed"
else
    echo "ERROR: StringConcat not executable"
fi

echo "Running RendToProp..."
if [ -x "$SCRIPT_DIR/RendToProp" ]; then
    "$SCRIPT_DIR/RendToProp"
    echo "RendToProp completed"
else
    echo "ERROR: RendToProp not executable"
fi

echo "Running StringNext..."
if [ -x "$SCRIPT_DIR/StringNext" ]; then
    "$SCRIPT_DIR/StringNext"
    echo "StringNext completed"
else
    echo "ERROR: StringNext not executable"
fi

echo "Running FontListA..."
if [ -x "$SCRIPT_DIR/FontListA" ]; then
    "$SCRIPT_DIR/FontListA"
    echo "FontListA completed"
else
    echo "ERROR: FontListA not executable"
fi

echo "Running StringRend..."
if [ -x "$SCRIPT_DIR/StringRend" ]; then
    "$SCRIPT_DIR/StringRend"
    echo "StringRend completed"
else
    echo "ERROR: StringRend not executable"
fi

echo "Running StringComp..."
if [ -x "$SCRIPT_DIR/StringComp" ]; then
    "$SCRIPT_DIR/StringComp"
    echo "StringComp completed"
else
    echo "ERROR: StringComp not executable"
fi

echo "Running StringEmpty..."
if [ -x "$SCRIPT_DIR/StringEmpty" ]; then
    "$SCRIPT_DIR/StringEmpty"
    echo "StringEmpty completed"
else
    echo "ERROR: StringEmpty not executable"
fi

echo "Running FontListC..."
if [ -x "$SCRIPT_DIR/FontListC" ]; then
    "$SCRIPT_DIR/FontListC"
    echo "FontListC completed"
else
    echo "ERROR: FontListC not executable"
fi

echo "Running StringTab..."
if [ -x "$SCRIPT_DIR/StringTab" ]; then
    "$SCRIPT_DIR/StringTab"
    echo "StringTab completed"
else
    echo "ERROR: StringTab not executable"
fi

echo "Running StringWidth..."
if [ -x "$SCRIPT_DIR/StringWidth" ]; then
    "$SCRIPT_DIR/StringWidth"
    echo "StringWidth completed"
else
    echo "ERROR: StringWidth not executable"
fi

echo "Running StringExt..."
if [ -x "$SCRIPT_DIR/StringExt" ]; then
    "$SCRIPT_DIR/StringExt"
    echo "StringExt completed"
else
    echo "ERROR: StringExt not executable"
fi

echo "Running StringCopy..."
if [ -x "$SCRIPT_DIR/StringCopy" ]; then
    "$SCRIPT_DIR/StringCopy"
    echo "StringCopy completed"
else
    echo "ERROR: StringCopy not executable"
fi

echo "Running StringParse..."
if [ -x "$SCRIPT_DIR/StringParse" ]; then
    "$SCRIPT_DIR/StringParse"
    echo "StringParse completed"
else
    echo "ERROR: StringParse not executable"
fi

echo "All tests completed"