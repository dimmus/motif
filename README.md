# Motif - The Industrial-Class UI Toolkit

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/dimmus/motif)
[![License](https://img.shields.io/badge/license-LGPL--2.1-blue.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-2.4.0-orange.svg)](https://github.com/dimmus/motif)

## Overview

**Motif** is the legendary user interface toolkit that defined the Unix desktop era and powered countless industrial, enterprise, and scientific applications throughout the 1990s and 2000s. Originally developed by the Open Software Foundation (OSF) in 1988, Motif became the de facto standard for professional Unix workstations and mission-critical applications.

This is a modern, actively maintained implementation of the Motif toolkit, preserving its industrial-strength reliability while adding contemporary features like UTF-8 support, Xft font rendering, and modern image format support. The project uses a modern CMake-based build system for improved cross-platform compatibility and developer experience.

## Build Status

**🚀 Modern C Standard Support**: This project now fully supports the **C23 standard** and is compatible with **GCC 15**, ensuring cutting-edge compiler features and future-proof development.

This project is continuously tested on multiple Linux distributions with both **GCC** and **Clang** compilers to ensure maximum compatibility and reliability across different build environments:

### GitHub Actions (GitHub)
- **🏔️ Alpine Linux + GCC**: Tests compilation with GNU Compiler Collection on minimal Alpine Linux
- **🏔️ Alpine Linux + Clang**: Tests compilation with LLVM Clang compiler on Alpine Linux

### Gitverse Workflows (Gitverse)  
- **🐧 Ubuntu Linux + GCC**: Tests compilation with GNU Compiler Collection on Ubuntu Linux
- **🐧 Ubuntu Linux + Clang**: Tests compilation with LLVM Clang compiler on Ubuntu Linux

This comprehensive testing approach ensures robust compatibility:
- **Alpine Linux** provides minimal base system testing with musl libc and strict compiler settings
- **Ubuntu Linux** provides mainstream glibc testing with comprehensive development libraries
- **Dual Compiler Testing** verifies compatibility with both GNU and LLVM toolchains
- **Container Ready** builds work correctly in constrained environments and Docker containers

### Historical Significance

Motif was born from the collaboration of industry giants including Digital Equipment Corporation, Hewlett-Packard, and IBM. It combined:
- **DEC's Widget Technology**: Robust, enterprise-grade UI components
- **HP's 3D Visual Style**: The iconic beveled, professional appearance
- **Microsoft's Presentation Manager Behavior**: Consistent interaction patterns

This fusion created a toolkit that powered:
- **CAD/CAM Applications**: AutoCAD, Pro/ENGINEER, CATIA
- **Scientific Computing**: MATLAB, Mathematica, LabVIEW
- **Enterprise Software**: Oracle databases, SAP systems
- **Unix Workstations**: Sun, SGI, HP, DEC, IBM AIX systems
- **Common Desktop Environment (CDE)**: The standard Unix desktop

## Features

### Core Capabilities
- **Industrial-Strength Widgets**: Over 50 production-ready UI components
- **Motif Window Manager (MWM)**: Professional window management
- **User Interface Language (UIL)**: Declarative UI development
- **Motif Resource Manager (MRM)**: Dynamic resource management
- **Internationalization**: Full i18n support including UTF-8
- **Accessibility**: Built-in keyboard navigation and screen reader support

### Modern Enhancements
- **C23 Standard Support**: Built with the latest C language standard for modern development
- **GCC 15 Compatibility**: Future-ready compiler support with automatic fallback
- **Xft Font Rendering**: Anti-aliased text with modern font support
- **PNG/JPEG Support**: Modern image format integration
- **UTF-8 Support**: Full Unicode text handling
- **Printing Support**: Direct print integration
- **Message Catalogs**: X/Open compliant localization

### Widget Set
The comprehensive Motif widget set includes:

**Primitive Widgets**:
- `XmArrowButton`, `XmLabel`, `XmPushButton`, `XmToggleButton`
- `XmText`, `XmTextField`, `XmList`, `XmScale`, `XmScrollBar`

**Manager Widgets**:
- `XmBulletinBoard`, `XmForm`, `XmFrame`, `XmPanedWindow`
- `XmRowColumn`, `XmScrolledWindow`, `XmMainWindow`

**Dialog Widgets**:
- `XmMessageBox`, `XmFileSelectionBox`, `XmSelectionBox`
- `XmCommand`, `XmPromptDialog`

**Advanced Widgets** (Motif 2.x):
- `XmNotebook`, `XmContainer`, `XmSpinBox`, `XmComboBox`
- `XmIconBox`, `XmOutline`, `XmTree`, `XmColumn`

## System Requirements

### Supported Platforms
- **Linux**: 2.6+ (tested on modern distributions)
- **Solaris**: 10+
- **FreeBSD**: 10+
- **Other Unix**: Any POSIX-compliant system with X11

### Dependencies

#### Build Dependencies
```bash
# Essential build tools
cmake >= 3.16             # Primary build system
pkg-config                # Package configuration
gcc >= 13.0 (supports C23 standard and GCC 15 compatibility)
make (GNU Make required)  # Build orchestration
flex/lex                  # Lexical analysis
yacc/bison                # Parser generation

# Optional but recommended
ninja                     # Faster build tool
ccache                    # Compiler cache for faster rebuilds
```

**Compiler Support**:
- **GCC 13.0+**: Full C23 standard support with `-std=c23` flag
- **GCC 15**: Future-ready compatibility (uses `-std=c2x` fallback for current GCC versions)
- **Clang**: Compatible with modern C standards
- **Legacy Compilers**: Graceful fallback to C99/C11 standards

#### Runtime Dependencies
```bash
# Core X11 libraries
libX11-dev
libXt-dev
libXmu-dev
libXext-dev
libXpm-dev

# Optional but recommended
libXft-dev >= 2.0    # Anti-aliased fonts
libjpeg-dev          # JPEG image support
libpng-dev           # PNG image support
libXp-dev            # Printing support (if available)
```

#### Ubuntu/Debian Installation
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config flex bison \
                     libx11-dev libxt-dev libxmu-dev libxext-dev \
                     libxpm-dev libxft-dev libjpeg-dev libpng-dev \
                     ninja-build ccache
```

#### RHEL/CentOS/Fedora Installation
```bash
sudo yum install gcc cmake pkgconfig flex bison ninja-build ccache \
                 libX11-devel libXt-devel libXmu-devel libXext-devel \
                 libXpm-devel libXft-devel libjpeg-devel libpng-devel
```

#### Automatic Dependency Management
For convenience, the project includes an automated dependency checker that supports multiple operating systems:

```bash
# Automatic dependency checking and installation
make deps

# Or run the script directly
./tools/dev/scripts/deps_check.sh
```

**Supported Operating Systems:**
- Ubuntu, Debian, Linux Mint (apt)
- Arch Linux, Manjaro (pacman)
- RHEL, CentOS, Fedora, Rocky, Alma (dnf)
- Alpine Linux (apk)
- Void Linux (xbps)
- OpenIndiana, OmniOS (pkg)
- FreeBSD (pkg)

The dependency checker automatically:
- Detects your operating system
- Identifies missing packages
- Installs required dependencies
- Handles optional dependencies gracefully
- Provides clear feedback and next steps

### Troubleshooting

#### CMake Build Issues
If you encounter issues during the build process:

**Missing Dependencies**: Ensure all required packages are installed:
```bash
# Check dependencies
make deps

# Or install manually for your OS (see installation commands above)
```

**Build Directory Issues**: Clean and rebuild if you encounter strange build errors:
```bash
make clean
make build
```

**Permission Issues**: Ensure you have proper permissions for installation:
```bash
# Use sudo for system installation
sudo make install

# Or install to user directory
cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=$HOME/.local
make build
make install
```

## Building and Installation

### Quick Start
```bash
git clone https://github.com/dimmus/motif.git
cd motif

# Check and install dependencies (recommended)
make deps

# Or manually install dependencies for your OS
# (see manual installation commands above)

# Build Motif (CMake-based build system)
make build

# Install to system
sudo make install

# Uninstall if needed
sudo make uninstall
```

### Build System

This project uses **CMake** as the primary build system, providing modern dependency management and cross-platform compatibility.

#### Available Build Targets
```bash
# Core build targets
make build          # Configure and build Motif (no installation)
make all            # Build and install Motif (legacy behavior)
make install        # Install to system (requires sudo)
make uninstall      # Uninstall from system (requires sudo)

# Build variants
make debug          # Build debug version with symbols
make release        # Build optimized release version
make lite           # Build minimal version (faster, smaller)
make full           # Build with all optional features
make developer      # Build with development tools and checks

# Build tools
make ninja          # Use Ninja build tool for faster builds
make ccache         # Use ccache for faster rebuilds
make config         # Run CMake configuration tool
```

#### Feature Control
```bash
# Configure with specific features
make config

# Or use CMake directly with feature flags
cmake -H. -Bbuild \
    -DWITH_UTF8=ON                    # UTF-8 text support (recommended)
    -DWITH_PRINTING=ON                # Print support (if libXp available)
    -DWITH_MESSAGE_CATALOG=ON         # X/Open message catalogs
    -DWITH_XFT=ON                     # Xft font rendering (recommended)
    -DWITH_JPEG=ON                    # JPEG image support
    -DWITH_PNG=ON                     # PNG image support
    -DWITH_DEMOS=ON                   # Build demonstration programs
    -DWITH_TESTS=ON                   # Build automated tests
    -DCMAKE_BUILD_TYPE=Debug          # Debug build with symbols
```

#### Installation Paths
```bash
# Configure installation paths
cmake -H. -Bbuild \
    -DCMAKE_INSTALL_PREFIX=/usr/local    # Installation prefix
    -DCMAKE_INSTALL_SYSCONFDIR=/etc      # Configuration files
    -DCMAKE_INSTALL_LIBDIR=lib64         # Library directory
```

### Complete Build Example
```bash
# Build with all modern features using CMake
make full release ninja ccache

# Or configure manually with specific features
cmake -H. -Bbuild \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DWITH_UTF8=ON \
    -DWITH_DEMOS=ON \
    -DWITH_XFT=ON \
    -DWITH_JPEG=ON \
    -DWITH_PNG=ON \
    -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make build

# Install
sudo make install

# Update library cache
sudo ldconfig

# Uninstall if needed
sudo make uninstall
```

## Development and Usage

### Basic Application Structure
```c
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>

int main(int argc, char *argv[])
{
    XtAppContext app_context;
    Widget toplevel, main_window, form, button;
    
    // Initialize toolkit
    toplevel = XtVaAppInitialize(&app_context, "MyApp", 
                                 NULL, 0, &argc, argv, NULL, NULL);
    
    // Create main window
    main_window = XmCreateMainWindow(toplevel, "main", NULL, 0);
    XtManageChild(main_window);
    
    // Create form container
    form = XmCreateForm(main_window, "form", NULL, 0);
    XtManageChild(form);
    
    // Create push button
    button = XmCreatePushButton(form, "Hello World", NULL, 0);
    XtManageChild(button);
    
    // Realize and run
    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);
    
    return 0;
}
```

### Compilation
```bash
# Using pkg-config (recommended) - automatically uses C23 standard
gcc -o myapp myapp.c `pkg-config --cflags --libs motif`

# Manual compilation with C23 standard
gcc -std=c23 -o myapp myapp.c -I/usr/local/include -L/usr/local/lib -lXm -lXt -lX11

# For GCC 15 compatibility (automatic fallback to c2x if c23 not supported)
gcc -std=c2x -o myapp myapp.c -I/usr/local/include -L/usr/local/lib -lXm -lXt -lX11
```

### UIL Development
```uil
! MyApp.uil - User Interface Language file
module MyApp
    version = 'v1.0'
    names = case_sensitive

object root_window : XmMainWindow {
    controls {
        XmMenuBar menu_bar;
        XmForm work_area;
    };
};

object menu_bar : XmMenuBar {
    controls {
        XmCascadeButton file_menu;
    };
};

object work_area : XmForm {
    controls {
        XmPushButton hello_button;
    };
};

object hello_button : XmPushButton {
    arguments {
        XmNlabelString = "Hello, Motif World!";
    };
};

end module;
```

Compile UIL:
```bash
uil -o MyApp.uid MyApp.uil
```

### Demo Programs

The distribution includes comprehensive demonstration programs:

```bash
# After building with --enable-examples
cd examples/programs

# Classic examples
./hellomotif/hellomotif          # Basic Motif application
./draw/draw                      # Drawing application
./animate/animate                # Animation demo
./filemanager/filemanager        # File manager
./periodic/periodic              # Periodic table

# Advanced widget examples  
./Notebook/notebook              # Notebook widget
./Container/container            # Container widget
./Tree/tree                      # Tree widget
./ComboBox/combo                 # Combo box widget
```

## Applications and Legacy

### Historic Applications
Motif powered numerous industry-defining applications:

- **CAD/Engineering**: AutoCAD, Pro/ENGINEER, CATIA, I-DEAS
- **Scientific Computing**: MATLAB, Mathematica, LabVIEW, AVS
- **Databases**: Oracle Forms, Informix, Sybase
- **Development Tools**: Sun Workshop, SGI Workshop, DEC FUSE
- **System Administration**: HP OpenView, IBM Tivoli, Sun AdminSuite

### Modern Usage
Today, Motif continues to serve:

- **Legacy Application Maintenance**: Keeping critical systems operational
- **Industrial Control Systems**: Manufacturing and process control
- **Scientific Instruments**: Laboratory and research equipment
- **Embedded Systems**: Specialized Unix-based devices
- **Educational Projects**: Learning classic Unix GUI development

## Documentation

### Official Resources
- **Motif Programmer's Guide**: Complete development documentation
- **Motif Reference Manual**: Widget and function reference
- **Style Guide**: User interface design principles
- **Man Pages**: Comprehensive API documentation

### Online Documentation
- [OpenGroup Motif Documentation](http://www.opengroup.org/openmotif/docs/)
- [Motif Programming Manual](http://www.motifzone.net/docs/)
- [Widget Gallery](http://www.motifzone.net/gallery/)

### Quick References
```bash
# View widget man pages
man XmPushButton
man XmText
man XmMainWindow

# UIL documentation
man uil
man Mrm

# Window manager
man mwm
```

## Project Structure

```
motif/
├── lib/Xm/          # Core Motif widget library
├── lib/Mrm/         # Motif Resource Manager
├── clients/         # Motif applications
│   ├── mwm/         # Motif Window Manager
│   ├── uil/         # UIL compiler
│   └── xmbind/      # Key binding utility
├── include/         # Header files
├── examples/         # Example programs
├── doc/             # Documentation and man pages
├── data/            # Resource files and bitmaps
└── tools/           # Development utilities
```

## Contributing

We welcome contributions to keep this historic toolkit alive and relevant:

### Areas of Interest
- **Bug Fixes**: Addressing compatibility issues
- **Modern Platform Support**: New OS and compiler support  
- **Performance Improvements**: Optimizations and memory management
- **Documentation**: Examples and tutorials
- **Testing**: Automated test coverage

### Development Setup
```bash
git clone https://github.com/dimmus/motif.git
cd motif

# Development build with all features
make developer debug full ninja ccache

# Or configure manually for development
cmake -H. -Bbuild \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_DEMOS=ON \
    -DWITH_TESTS=ON \
    -DWITH_UTF8=ON \
    -DWITH_XFT=ON

# Build
make build

# Run tests
make test

# Generate code coverage reports
make gcov
```

### Code Coverage

When tests are enabled (`-DWITH_TESTS=ON`), you can generate code coverage reports:

```bash
# Generate coverage reports
make gcov

# Clean up coverage files
make clean-gcov

# View available targets
make help
```

The coverage system automatically detects your compiler:
- **GCC**: Uses `gcov` for coverage analysis
- **Clang**: Uses `llvm-cov gcov` for coverage analysis

Coverage reports are generated as `.gcov` files in the source directories.

### Additional Build Commands

```bash
# View all available targets
make help

# Clean build directory
make clean

# Clean everything including build directory
make clean_all

# Package the build
make package_archive

# Format source code
make format PATHS="lib/Xm clients"

# Run static analysis
make check_cppcheck
make check_clang_array
make check_struct_comments
```

## License

This implementation of Motif is released under the **LGPL 2.1** license, ensuring it remains free and open source while allowing commercial usage.

## Support and Community

- **Issue Tracker**: [GitHub Issues](https://github.com/dimmus/motif/issues)
- **Discussions**: [GitHub Discussions](https://github.com/dimmus/motif/discussions)
<!-- - **Mailing List**: [motif-developer@lists.openmotif.org](mailto:motif-developer@lists.openmotif.org) -->

## Acknowledgments

This project builds upon decades of development by:
- **Open Software Foundation (OSF)**: Original Motif creators
- **The Open Group**: Motif stewardship and standards
- **ICS/Integrated Computer Solutions**: Commercial Motif development
- **OpenMotif Community**: Open source maintenance and evolution

Special thanks to:
- [Tim Hentenaar](https://github.com/thentenaar)
- [Olivier Fourdan](https://github.com/ofourdan)
- [Alexander Pampuchin](https://github.com/alx210)
---

*"Motif: Where industrial-strength meets elegant design. Powering mission-critical applications since 1988."*
