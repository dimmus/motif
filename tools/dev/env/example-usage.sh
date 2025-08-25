#!/bin/bash
#
# Example Usage Script for Motif Development Environment
# This script demonstrates how to use the development environment
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

echo "Motif Development Environment - Example Usage"
echo "============================================="
echo ""

# Check if docker/podman is available
if command -v docker &> /dev/null; then
    CONTAINER_ENGINE="docker"
elif command -v podman &> /dev/null; then
    CONTAINER_ENGINE="podman"
else
    echo "Error: Neither docker nor podman found. Please install one of them."
    exit 1
fi

echo "Using container engine: ${CONTAINER_ENGINE}"
echo ""

# Step 1: List available OS environments
echo "Step 1: List available OS environments"
echo "======================================"
./test-motif.sh --list
echo ""

# Step 2: Show available templates for adding new OS
echo "Step 2: Available OS templates"
echo "=============================="
./add-os.sh --list
echo ""

# Step 3: Generate a Dockerfile (dry run)
echo "Step 3: Generate Ubuntu Dockerfile (dry run)"
echo "============================================="
./add-os.sh --dry-run ubuntu | head -50
echo "... (truncated for brevity)"
echo ""

# Step 4: Actually generate Ubuntu Dockerfile if it doesn't exist
if [[ ! -f "containers/Dockerfile.ubuntu" ]]; then
    echo "Step 4: Generate Ubuntu Dockerfile"
    echo "=================================="
    ./add-os.sh ubuntu
    echo ""
else
    echo "Step 4: Ubuntu Dockerfile already exists"
    echo "========================================"
    echo "File: containers/Dockerfile.ubuntu"
    echo ""
fi

# Step 5: Show what a test run would look like (don't actually run it)
echo "Step 5: Example test commands"
echo "============================"
echo ""
echo "To test Motif build on Arch Linux:"
echo "  ./test-motif.sh archlinux"
echo ""
echo "To test with verbose output:"
echo "  ./test-motif.sh -v archlinux"
echo ""
echo "To test all available OS:"
echo "  ./test-motif.sh --all"
echo ""
echo "To use Podman instead of Docker:"
echo "  ./test-motif.sh --podman archlinux"
echo ""
echo "To view previous test logs:"
echo "  ./test-motif.sh --logs-only"
echo ""

# Step 6: Show directory structure
echo "Step 6: Directory structure"
echo "=========================="
echo ""
echo "Current directory structure:"
tree . 2>/dev/null || find . -type d | sed 's/^/  /' | head -20
echo ""

# Step 7: Show help information
echo "Step 7: Help information"
echo "======================="
echo ""
echo "Test runner help:"
./test-motif.sh --help | head -20
echo "... (use --help for complete information)"
echo ""

echo "Add OS script help:"
./add-os.sh --help | head -20
echo "... (use --help for complete information)"
echo ""

# Step 8: Final instructions
echo "Step 8: Next steps"
echo "================="
echo ""
echo "To start testing Motif builds:"
echo ""
echo "1. Choose an OS to test:"
echo "   ./test-motif.sh --list"
echo ""
echo "2. Run a test (this will take several minutes):"
echo "   ./test-motif.sh archlinux"
echo ""
echo "3. Check the results:"
echo "   ./test-motif.sh --logs-only"
echo ""
echo "4. Add more OS environments:"
echo "   ./add-os.sh fedora"
echo "   ./test-motif.sh fedora"
echo ""
echo "For detailed documentation, see README.md"
echo ""
echo "Example usage demonstration completed!"
