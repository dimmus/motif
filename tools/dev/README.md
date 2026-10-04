# Development Tools

This directory contains development tools for the Motif project.

## Dependency Checker (`deps_check.sh`)

A comprehensive script that automatically detects your operating system, checks for required build dependencies, and installs missing packages.

### Supported Operating Systems

| OS Family | Package Manager | Detection Method |
|-----------|----------------|------------------|
| Ubuntu/Debian/Linux Mint | `apt` | `/etc/os-release` |
| Arch Linux/Manjaro | `pacman` | `/etc/os-release` |
| RHEL/CentOS/Fedora/Rocky/Alma | `dnf` | `/etc/os-release` |
| Alpine Linux | `apk` | `/etc/os-release` |
| Void Linux | `xbps` | `/etc/os-release` |
| OpenIndiana | `pkg` | `/etc/release` |
| OmniOS | `pkg` | `/etc/release` |
| FreeBSD | `pkg` | `/etc/freebsd-update.conf` |

### Features

1. **Automatic OS Detection** - Identifies your operating system and package manager
2. **Dependency Checking** - Verifies all required build dependencies are installed
3. **Smart Installation** - Only installs missing required dependencies, attempts optional ones
4. **Cross-Platform Support** - Maps dependencies to correct package names for each OS
5. **Verification** - Confirms successful installation of all dependencies
6. **Graceful Degradation** - Continues successfully even if optional packages fail
7. **Security Features** - Non-root execution, sudo verification, package verification
8. **Clear Feedback** - Provides detailed status messages and next steps

### Dependencies Checked

The lists are `REQUIRED_DEPS` and `OPTIONAL_DEPS`, defined just before
`get_package_names()` in the script, which maps each name to the package of every
supported OS.

#### Required
- Build tools: `cmake`, `ninja`, `pkg-config`, `gcc`, `make`, `flex`,
  `bison`
- X11 and image libraries (development packages): `libX11`, `libXt`,
  `libXmu`, `libXext`, `libXpm`, `libXft`, `libjpeg`, `libpng`
- `check` (libcheck), for the tests

#### Optional
- `libXp`, for printing support (`WITH_PRINTING`); missing on newer
  distributions, which the script handles
- `Xvfb` and `xvfb-run`, to run the X11 test suites without a display
- `Xephyr`, a nested X server, used by the mwm test and to watch or
  debug the tests in a window
- `xdotool`, which the Text and mwm tests use to drive real input
- `abidiff` (libabigail), for `tools/dev/env/ci/abi-check.sh`

Optional dependencies that are not packaged for an OS are skipped.  The
script does not install the core X fonts that the X11 test suites use
(`xfonts-base` on Debian and Ubuntu, `xorg-x11-fonts-misc` on Fedora,
`xorg-fonts-misc` on Arch Linux, `font-misc-misc` on Alpine);
`tools/dev/env/ci/deps.sh` installs the complete set CI uses.

### Usage

Run it from the project root:

```bash
./tools/dev/scripts/deps_check.sh
```

It detects the OS and package manager, reports which dependencies are
installed, installs the missing required ones, tries the missing
optional ones, and checks the result.  When everything is present it
suggests `make build` (see the `GNUmakefile`) to build Motif.

### Requirements

- **Non-root user** on the host: the script refuses to run as root,
  except inside a container
- **sudo access**, to install packages
- **Network access**, to download packages

### Error Handling

- **Required dependency missing**: the script fails with an error.
- **Optional dependency missing or not available**: the script warns and
  continues.
- **Unsupported OS**: the script exits with an error; add the OS to
  `detect_os()` and `get_package_names()`, or install the packages by
  hand.

### Manual Installation

The package commands for Debian/Ubuntu and Fedora are in the
[top-level README](../../README.md#requirements), and
`tools/dev/env/ci/deps.sh` has the lists for Debian, Ubuntu, Fedora,
Alpine, Arch Linux and FreeBSD.

## Development Environment (`env/`)

A comprehensive containerized development environment for testing Motif builds across multiple operating systems using Docker/Podman.

### Features

- **Multi-OS Testing**: Test Motif builds on Arch Linux, FreeBSD, Ubuntu, CentOS, Fedora
- **Automated Testing**: Complete build, test, and verification pipeline
- **Container Isolation**: Each OS runs in its own isolated container
- **Comprehensive Logging**: Detailed build logs with error reporting
- **Template System**: Easy addition of new OS environments
- **Cross-platform Support**: Works with both Docker and Podman

### Quick Start

```bash
# Navigate to the environment directory
cd tools/dev/env

# Test on Arch Linux
./test-motif.sh archlinux

# Test on all available OS
./test-motif.sh --all

# List available OS environments
./test-motif.sh --list

# Add new OS environment
./add-os.sh ubuntu

# Use Makefile targets
make test                    # Test on default OS
make test-all               # Test on all OS
make add-os OS=fedora       # Add Fedora support
```

### Available Scripts

- **`test-motif.sh`**: Main test runner with comprehensive options
- **`add-os.sh`**: Generate new OS environments from templates
- **`build-motif.sh`**: Container build script (runs inside containers)
- **`example-usage.sh`**: Demonstration of environment usage
- **`Makefile`**: Convenient make targets for common tasks

### Directory Structure

```
env/
├── README.md                 # Comprehensive documentation
├── test-motif.sh            # Main test runner
├── add-os.sh                # OS environment generator
├── example-usage.sh         # Usage examples
├── Makefile                 # Make targets
├── containers/              # Container definitions
│   ├── Dockerfile.archlinux # Arch Linux container
│   └── Dockerfile.freebsd   # FreeBSD container
├── scripts/                 # Build scripts
│   └── build-motif.sh      # Container build script
├── templates/               # Template system
│   ├── Dockerfile.template  # Dockerfile template
│   └── os-config.yaml      # OS configuration definitions
└── logs/                   # Test execution logs
```

### Usage Examples

```bash
# Basic testing
./test-motif.sh archlinux

# Verbose testing with container rebuild
./test-motif.sh -v -r archlinux

# Test all OS with cleanup
./test-motif.sh --all --clean

# Use Podman instead of Docker
./test-motif.sh --podman archlinux

# Keep container running for debugging
./test-motif.sh -k archlinux

# Generate new OS environment
./add-os.sh ubuntu

# Preview OS generation
./add-os.sh --dry-run fedora

# View previous test logs
./test-motif.sh --logs-only
```

### Integration with CI/CD

The environment can be easily integrated into continuous integration pipelines:

```bash
# Test all supported platforms
./tools/dev/env/test-motif.sh --all --clean

# Check exit code for CI
if [ $? -eq 0 ]; then
    echo "All tests passed"
else
    echo "Some tests failed"
    exit 1
fi
```

For complete documentation, see `env/README.md`.

### Maintenance

#### Adding New OS Support
1. Add OS detection logic in `detect_os()`
2. Add package name mappings in `get_package_names()`
3. Add package installation commands in `install_packages()`
4. Test on target OS

#### Adding New Dependencies
1. Add to `required_deps` array if essential
2. Add to `optional_deps` array if optional
3. Add package name mappings for all supported OSes
4. Update documentation

### Testing

The script has been tested on:
- ✅ Ubuntu 24.04 (current system)
- ✅ Handles missing optional dependencies gracefully
- ✅ Properly installs missing required dependencies
- ✅ Provides clear feedback and next steps

### Benefits

- **Developer Experience**: One command to get all dependencies
- **Cross-Platform**: Works consistently across different Unix-like systems
- **Maintenance**: Reduces manual dependency installation steps
- **Documentation**: Clear guidance for users on any supported OS
- **Reliability**: Handles edge cases and provides helpful error messages

### Contributing

To add support for additional operating systems:
1. Add OS detection logic in `detect_os()`
2. Add package name mappings in `get_package_names()`
3. Add package installation commands in `install_packages()`
4. Test on the target OS

For the development environment:
1. Add OS configuration to `env/templates/os-config.yaml`
2. Generate Dockerfile with `env/add-os.sh os-name`
3. Test with `env/test-motif.sh os-name`
4. Submit changes with test results

