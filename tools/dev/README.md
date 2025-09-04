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

### Required Dependencies

The script checks for and installs these essential build dependencies:

#### Build Tools
- `autoconf` - Configuration script generator
- `automake` - Makefile generator
- `autopoint` - Gettext infrastructure
- `pkg-config` - Package configuration tool
- `gcc` - C compiler (or equivalent)
- `make` - Build automation tool
- `flex` - Fast lexical analyzer
- `bison` - Parser generator

#### X11 Development Libraries
- `libX11-dev` - Core X11 library
- `libXt-dev` - X Toolkit Intrinsics
- `libXmu-dev` - X11 miscellaneous utilities
- `libXext-dev` - X11 extensions
- `libXpm-dev` - X11 pixmap library
- `libXft-dev` - X11 font rendering
- `libjpeg-dev` - JPEG image support
- `libpng-dev` - PNG image support

#### Optional Dependencies
- `libXp-dev` - X11 printing support (handled gracefully if not available)

#### Testing Framework
- `check` - Unit testing framework

### Usage

#### Basic Usage

#### Direct Script Execution
```bash
# Run from the project root directory
./tools/dev/scripts/deps_check.sh
```

#### Using Make Target
```bash
# Use the convenient make target (recommended)
make deps
```

The `make deps` target provides the same functionality as running the script directly, but integrates seamlessly with the build system.

#### What It Does
1. Detects your operating system
2. Checks which dependencies are already installed
3. Identifies missing packages
4. Automatically installs missing dependencies
5. Verifies successful installation
6. Provides build instructions

#### Example Output
```
==========================================
    Motif Dependency Checker & Installer
==========================================

[INFO] Detecting operating system...
[SUCCESS] Detected: ubuntu 24.04
[INFO] Package manager: apt

[INFO] Starting dependency check...
[INFO] Checking project dependencies...
[SUCCESS] ✓ autoconf (autoconf) - installed
[SUCCESS] ✓ pkg-config (pkg-config) - installed
[WARNING] ✗ libXp (libxp-dev) - not available (optional, not in repositories)
[INFO] Dependency check complete: 17/18 packages installed
[SUCCESS] All dependencies are installed!

[INFO] You can now build Motif with:
  ./autogen.sh
  ./configure
  make
```

### Requirements

- **Non-root user**: The script should not be run as root
- **sudo access**: Required for package installation
- **Internet connection**: For downloading packages

### Error Handling

#### Graceful Degradation
- **Required dependencies missing**: Script fails with clear error message
- **Optional dependencies missing**: Script continues with warning
- **Package not available**: Script notes unavailability and continues

#### Common Scenarios
- **libXp not available**: Common on newer Ubuntu/Debian versions, handled gracefully
- **Package installation fails**: Clear error messages and exit codes
- **Unsupported OS**: Helpful error message with suggestions

### Security Features

- **Non-root execution**: Script refuses to run as root
- **Sudo verification**: Checks for sudo availability before proceeding
- **Package verification**: Verifies successful installation after package management

### Integration with Build System

The dependency checker is fully integrated with the Motif build system:

- **`make deps`**: Automatically checks and installs dependencies
- **`make clean`**: Cleans build artifacts (dependencies remain installed)
- **`make distclean`**: Full cleanup (dependencies remain installed)

This integration ensures that dependencies are properly managed as part of the build workflow.

### Troubleshooting

#### Permission Denied
```bash
# Make sure the script is executable
chmod +x tools/dev/scripts/deps_check.sh
```

#### Sudo Not Available
```bash
# Install sudo first (if not available)
su -
# Then install sudo for your distribution
```

#### Package Installation Fails
- Check your internet connection
- Verify package repository configuration
- Some distributions may require additional repositories for development packages

#### Unsupported OS
The script will exit with an error if your operating system is not supported. You can:
- Check if your OS is a variant of a supported distribution
- Modify the script to add support for your OS
- Install dependencies manually using your package manager

### Manual Installation

If you prefer to install dependencies manually, here are the commands for each supported OS:

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential autoconf automake autopoint pkg-config \
                     flex bison libx11-dev libxt-dev libxmu-dev libxext-dev \
                     libxpm-dev libxft-dev libjpeg-dev libpng-dev libxp-dev check
```

#### Arch Linux
```bash
sudo pacman -Sy base-devel autoconf automake gettext pkgconf flex bison \
                libx11 libxt libxmu libxext libxpm libxft libjpeg-turbo libpng libxp check
```

#### RHEL/CentOS/Fedora
```bash
sudo dnf install gcc autoconf automake gettext pkgconfig flex bison \
                 libX11-devel libXt-devel libXmu-devel libXext-devel \
                 libXpm-devel libXft-devel libjpeg-devel libpng-devel libXp-devel check-devel
```

#### Alpine Linux
```bash
sudo apk add build-base autoconf automake gettext pkgconf flex bison \
            libx11-dev libxt-dev libxmu-dev libxext-dev \
            libxpm-dev libxft-dev libjpeg-turbo-dev libpng-dev check-dev
```

#### Void Linux
```bash
sudo xbps-install base-devel autoconf automake gettext pkg-config flex bison \
                  libX11-devel libXt-devel libXmu-devel libXext-devel \
                  libXpm-devel libXft-devel libjpeg-turbo-devel libpng-devel check-devel
```

#### FreeBSD
```bash
sudo pkg install gcc autoconf automake gettext pkgconf gmake flex bison \
                 libX11 libXt libXmu libXext libXpm libXft jpeg libpng libXp check
```

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

