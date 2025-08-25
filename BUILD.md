# Build Instructions

This document describes the available build targets and optimization options for the XDE Motif project.

## Quick Start

```bash
# Standard build
make

# Help - show all available targets
make help

# Optimized release build
make release

# Development build with debug symbols
make dev-build
```

## Build Targets

### Standard Targets

- **`make`** or **`make all`** - Standard build with default settings
- **`make clean`** - Clean build artifacts
- **`make distclean`** - Clean everything including configuration
- **`make install`** - Install the built software
- **`make uninstall`** - Uninstall the software

### Release Targets

- **`make release`** - Full optimized release build with CPU-specific optimizations and stripping
  - Uses `-O3 -march=native -mtune=native -flto` and other aggressive optimizations
  - Automatically strips debug symbols from binaries
  - Best performance for the current machine

- **`make release-portable`** - Portable optimized release build
  - Uses `-O3 -flto` without CPU-specific optimizations
  - Suitable for distribution packages
  - Automatically strips debug symbols from binaries

- **`make release-configure`** - Configure for release build only (without building)
- **`make release-build`** - Build release version (after configure)

### Development Targets

- **`make dev-build`** - Development build with debug symbols
  - Configures with `--enable-debug`
  - Includes debug symbols and assertions
  - Uses `-Og -ggdb` optimization level

### Utility Targets

- **`make help`** - Show all available build targets and their descriptions
- **`make format`** - Format source code with clang-format
- **`make deps`** - Check dependencies

## Optimization Details

### Release Build Optimizations

The release builds use the following GCC optimization flags:

- **`-O3`** - Maximum optimization level
- **`-march=native`** - Optimize for current CPU architecture (release only)
- **`-mtune=native`** - Tune for current CPU (release only)
- **`-flto`** - Link-time optimization
- **`-ffast-math`** - Fast floating-point math operations
- **`-funroll-loops`** - Unroll loops for better performance
- **`-fomit-frame-pointer`** - Omit frame pointer for smaller code
- **`-DNDEBUG`** - Disable debug assertions

### Binary Stripping

Release builds automatically strip debug symbols from:
- Shared libraries (`*.so*`)
- Executable files
- Uses `strip --strip-unneeded` for optimal size reduction

## Configuration Options

The build system supports standard autotools configure options:

```bash
# Configure manually with custom options
./configure --enable-debug --with-xft --with-png --with-jpeg

# Or use the predefined build targets
make dev-build    # Equivalent to: ./configure --enable-debug
make release      # Equivalent to: CFLAGS="-O3..." ./configure --disable-dependency-tracking
```

## Examples

```bash
# Build optimized version for current machine
make release

# Build portable version for distribution
make release-portable

# Build development version for debugging
make dev-build

# Clean and rebuild everything
make distclean
make release

# Install optimized version
make release
sudo make install
```

## Performance Notes

- **`make release`** provides the best performance but binaries only work on similar CPUs
- **`make release-portable`** provides good performance and works on most x86_64 systems
- Both release builds are significantly faster than the default build
- Debug builds (`make dev-build`) are slower but include debugging information
