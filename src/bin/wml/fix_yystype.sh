#!/bin/bash
# Script to fix YYSTYPE definition in generated Uil.h file

if [ $# -ne 1 ]; then
    echo "Usage: $0 <Uil.h file path>"
    exit 1
fi

UIL_H_FILE="$1"

if [ ! -f "$UIL_H_FILE" ]; then
    echo "Error: File $UIL_H_FILE does not exist"
    exit 1
fi

# Remove any existing UIL includes (we'll just fix YYSTYPE)
sed -i '/#include "UilSarDef.h"/d' "$UIL_H_FILE"
sed -i '/#include "UilDefI.h"/d' "$UIL_H_FILE"
sed -i '/#include "UilSymDef.h"/d' "$UIL_H_FILE"
sed -i '/#include "UilSrcDef.h"/d' "$UIL_H_FILE"

# Replace typedef int YYSTYPE; with #define YYSTYPE yystype
sed -i 's/typedef int YYSTYPE;/#define YYSTYPE yystype/' "$UIL_H_FILE"

echo "Fixed YYSTYPE definition in $UIL_H_FILE"
