#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

"""
Update script for Motif.
This script updates the Motif repository and optionally libraries.
"""

import os
import sys
import subprocess
import argparse


def run_command(cmd, cwd=None):
    """Run a command and return the result."""
    try:
        result = subprocess.run(cmd, shell=True, cwd=cwd, check=True, 
                              capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        return e.returncode, e.stdout, e.stderr


def update_git_repo(repo_dir):
    """Update git repository."""
    print(f"Updating git repository in {repo_dir}")
    
    # Check if it's a git repository
    if not os.path.exists(os.path.join(repo_dir, '.git')):
        print("Not a git repository, skipping git update")
        return True
    
    # Fetch latest changes
    returncode, stdout, stderr = run_command("git fetch", cwd=repo_dir)
    if returncode != 0:
        print(f"Error fetching: {stderr}")
        return False
    
    # Check for updates
    returncode, stdout, stderr = run_command("git status -uno", cwd=repo_dir)
    if "Your branch is up to date" in stdout:
        print("Repository is already up to date")
        return True
    
    # Pull latest changes
    returncode, stdout, stderr = run_command("git pull", cwd=repo_dir)
    if returncode != 0:
        print(f"Error pulling: {stderr}")
        return False
    
    print("Repository updated successfully")
    return True


def update_libraries():
    """Update libraries (placeholder for future implementation)."""
    print("Library update not implemented yet")
    return True


def main():
    parser = argparse.ArgumentParser(description='Update Motif repository and libraries')
    parser.add_argument('--no-libraries', action='store_true', 
                       help='Skip library updates')
    args = parser.parse_args()

    # Get source directory
    source_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    
    print("Updating Motif...")
    
    # Update git repository
    if not update_git_repo(source_dir):
        return 1
    
    # Update libraries if requested
    if not args.no_libraries:
        if not update_libraries():
            return 1
    
    print("Update completed successfully")
    return 0


if __name__ == '__main__':
    sys.exit(main())
