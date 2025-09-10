#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

"""
Test runner for Motif build system.
This script runs automated tests using ctest.
"""

import os
import sys
import subprocess
import argparse


def main():
    parser = argparse.ArgumentParser(description='Run Motif tests')
    parser.add_argument('build_dir', help='Build directory path')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    parser.add_argument('--parallel', '-j', type=int, help='Number of parallel jobs')
    args = parser.parse_args()

    if not os.path.exists(args.build_dir):
        print(f"Error: Build directory '{args.build_dir}' does not exist")
        return 1

    # Change to build directory
    os.chdir(args.build_dir)

    # Build ctest command
    ctest_cmd = ['ctest']
    
    if args.verbose:
        ctest_cmd.append('--verbose')
    
    if args.parallel:
        ctest_cmd.extend(['--parallel', str(args.parallel)])
    else:
        # Auto-detect number of cores
        try:
            import multiprocessing
            cores = multiprocessing.cpu_count()
            ctest_cmd.extend(['--parallel', str(cores)])
        except:
            pass

    print(f"Running tests in {args.build_dir}")
    print(f"Command: {' '.join(ctest_cmd)}")
    
    # Run ctest
    try:
        result = subprocess.run(ctest_cmd, check=True)
        print("All tests passed!")
        return 0
    except subprocess.CalledProcessError as e:
        print(f"Tests failed with exit code {e.returncode}")
        return e.returncode
    except FileNotFoundError:
        print("Error: ctest not found. Make sure CMake is installed and tests are configured.")
        return 1


if __name__ == '__main__':
    sys.exit(main())
