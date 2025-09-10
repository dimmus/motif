#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

"""
Source archive creator for Motif.
This script creates a compressed archive of the source code.
"""

import os
import sys
import tarfile
import zipfile
import argparse
from datetime import datetime


def create_tar_archive(source_dir, output_file):
    """Create a tar.gz archive of the source directory."""
    with tarfile.open(output_file, "w:gz") as tar:
        tar.add(source_dir, arcname=os.path.basename(source_dir))


def create_zip_archive(source_dir, output_file):
    """Create a zip archive of the source directory."""
    with zipfile.ZipFile(output_file, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for root, dirs, files in os.walk(source_dir):
            for file in files:
                file_path = os.path.join(root, file)
                arc_path = os.path.relpath(file_path, source_dir)
                zipf.write(file_path, arc_path)


def main():
    parser = argparse.ArgumentParser(description='Create source archive for Motif')
    parser.add_argument('--format', choices=['tar', 'zip'], default='tar', 
                       help='Archive format (default: tar)')
    parser.add_argument('--output', help='Output file name')
    parser.add_argument('--exclude', action='append', default=[], 
                       help='Patterns to exclude from archive')
    args = parser.parse_args()

    # Get source directory (parent of this script)
    source_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    
    # Generate output filename if not provided
    if not args.output:
        timestamp = datetime.now().strftime("%Y%m%d")
        if args.format == 'tar':
            args.output = f"motif-source-{timestamp}.tar.gz"
        else:
            args.output = f"motif-source-{timestamp}.zip"

    print(f"Creating {args.format} archive of {source_dir}")
    print(f"Output file: {args.output}")

    try:
        if args.format == 'tar':
            create_tar_archive(source_dir, args.output)
        else:
            create_zip_archive(source_dir, args.output)
        
        print(f"Archive created successfully: {args.output}")
        return 0
    except Exception as e:
        print(f"Error creating archive: {e}")
        return 1


if __name__ == '__main__':
    sys.exit(main())
