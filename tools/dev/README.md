# Development Tools

This directory contains development tools for the Motif project.

## Dependency Checker (`deps_check.sh`)

A comprehensive script that automatically detects your operating system, checks for required build dependencies, and installs missing packages.

### Supported Operating Systems

- **Ubuntu/Debian/Linux Mint** - Uses `apt` package manager
- **Arch Linux/Manjaro** - Uses `pacman` package manager  
- **RHEL/CentOS/Fedora/Rocky/Alma** - Uses `dnf` package manager
- **Alpine Linux** - Uses `apk` package manager
- **Void Linux** - Uses `xbps` package manager
- **OpenIndiana** - Uses `pkg` package manager
- **OmniOS** - Uses `pkg` package manager
- **FreeBSD** - Uses `pkg` package manager

### Features

1. **Automatic OS Detection** - Identifies your operating system and package manager
2. **Dependency Checking** - Verifies all required build dependencies are installed
3. **Automatic Installation** - Installs missing packages using the appropriate package manager
4. **Cross-Platform Support** - Maps dependencies to correct package names for each OS
5. **Verification** - Confirms successful installation of all dependencies

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
- `libXp-dev` - X11 printing support

#### Testing Framework
- `check` - Unit testing framework

### Usage

#### Basic Usage

#### Direct Script Execution
```bash
# Run from the project root directory
./tools/dev/deps_check.sh
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
[SUCCESS] Detected: ubuntu 22.04
[INFO] Package manager: apt
[INFO] Starting dependency check...
[INFO] Checking project dependencies...
[SUCCESS] ✓ autoconf (autoconf) - installed
[SUCCESS] ✓ automake (automake) - installed
[WARNING] ✗ libXft (libxft-dev) - not installed
...
[INFO] Some dependencies are missing. Installing them now...
[INFO] Installing packages: libxft-dev libjpeg-dev libpng-dev
[SUCCESS] All dependencies installed successfully!
[INFO] You can now build Motif with:
  ./autogen.sh
  ./configure
  make
```

### Requirements

- **Non-root user**: The script should not be run as root
- **sudo access**: Required for package installation
- **Internet connection**: For downloading packages

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
chmod +x tools/dev/deps_check.sh
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

### Contributing

To add support for additional operating systems:
1. Add OS detection logic in `detect_os()`
2. Add package name mappings in `get_package_names()`
3. Add package installation commands in `install_packages()`
4. Test on the target OS

### License

This script is part of the Motif project and follows the same license terms.
