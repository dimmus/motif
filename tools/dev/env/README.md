# Motif Development Environment

This directory contains a comprehensive development environment for testing Motif builds across different operating systems using Docker/Podman containers. The environment provides automated testing, logging, and easy addition of new OS platforms.

## Overview

The development environment consists of:

- **Automated Testing**: Test Motif builds on multiple OS platforms
- **Container-based Isolation**: Each OS runs in its own container
- **Comprehensive Logging**: Detailed build logs and error reporting
- **Template System**: Easy addition of new OS environments
- **Cross-platform Support**: Works with both Docker and Podman

## Directory Structure

```
tools/dev/env/
├── README.md                # This documentation
├── test-motif.sh            # Main test runner script
├── add-os.sh                # Script to add new OS environments
├── containers/              # Container definitions
│   ├── Dockerfile.archlinux # Arch Linux container
│   └── Dockerfile.freebsd   # FreeBSD container
├── scripts/                 # Build scripts
│   └── build-motif.sh       # Container build script
├── templates/               # Template system
│   ├── Dockerfile.template  # Dockerfile template
│   └── os-config.yaml       # OS configuration definitions
└── logs/                    # Test execution logs
    └── YYYYMMDD_HHMMSS/     # Timestamped log sessions
```

## Quick Start

### Prerequisites

- Docker or Podman installed and running
- Sufficient disk space (containers can be 1-2GB each)
- Internet connection for downloading packages

### Basic Usage

```bash
# Navigate to the environment directory
cd tools/dev/env

# Test on Arch Linux (first run builds image, subsequent runs use cache)
./test-motif.sh archlinux

# Test on FreeBSD
./test-motif.sh freebsd

# Test on all available OS (uses cached images for speed)
./test-motif.sh --all

# List available OS environments
./test-motif.sh --list

# View previous test logs
./test-motif.sh --logs-only
```

**💡 Performance Tip**: Images are automatically cached! First build takes 5-15 minutes, subsequent runs take 5-30 seconds.

### Using Podman instead of Docker

```bash
# Use podman instead of docker
./test-motif.sh --podman archlinux

# Or set it as default
export CONTAINER_ENGINE=podman
./test-motif.sh archlinux
```

## Detailed Usage

### Test Runner Script (`test-motif.sh`)

The main test runner provides comprehensive testing capabilities:

#### Options

- `-h, --help`: Show help message
- `-l, --list`: List available OS environments
- `-a, --all`: Test all available OS environments
- `-c, --clean`: Clean containers and images after testing
- `-k, --keep`: Keep containers running after test (for debugging)
- `-v, --verbose`: Verbose output
- `-r, --rebuild`: Force rebuild of container images
- `-p, --podman`: Use podman instead of docker
- `--logs-only`: Show logs from previous runs

#### Examples

```bash
# Basic test with verbose output
./test-motif.sh -v archlinux

# Force rebuild and test
./test-motif.sh -r -v archlinux

# Test all OS and clean up afterwards
./test-motif.sh --all --clean

# Keep container running for debugging
./test-motif.sh -k archlinux
# Then connect to container: docker exec -it motif-test-archlinux-<timestamp> /bin/bash
```

### Adding New OS Environments

Use the `add-os.sh` script to add support for new operating systems:

#### Available Templates

The system includes pre-configured templates for:

- **archlinux**: Arch Linux (rolling release)
- **ubuntu**: Ubuntu 22.04 LTS
- **freebsd**: FreeBSD-like environment (Debian-based)
- **centos**: CentOS Stream 9
- **fedora**: Fedora 39

#### Adding a New OS

```bash
# List available templates
./add-os.sh --list

# Generate Dockerfile for Ubuntu
./add-os.sh ubuntu

# Preview what would be generated (dry run)
./add-os.sh --dry-run fedora

# Force overwrite existing Dockerfile
./add-os.sh --force archlinux
```

#### Custom OS Configuration

To add a completely new OS:

1. Edit `templates/os-config.yaml`
2. Add your OS configuration following the existing pattern
3. Run `./add-os.sh your-os-name`

Example configuration in `os-config.yaml`:

```yaml
myos:
  base_image: "myos:latest"
  description: "My Custom OS"
  package_manager: "mypm"
  commands:
    update: "mypm update"
    install: "mypm install"
    cleanup: "mypm clean"
  packages:
    base_tools:
      - build-essential
      - git
      # ... more packages
  environment:
    compiler: "gcc"
    cflags: "-O2 -g"
    # ... more environment variables
```

## Build Process

### Container Build Process

Each test follows these steps:

1. **Container Build**: Create OS-specific container with all dependencies
2. **Source Preparation**: Copy Motif source code to container
3. **Configuration**: Run autotools configuration
4. **Compilation**: Build Motif with parallel jobs
5. **Installation**: Install to temporary directory
6. **Testing**: Run available tests and verify installation
7. **Reporting**: Generate detailed build report

### Build Environment

Each container includes:

- **Development Tools**: GCC, Clang, Make, CMake, Autotools
- **X11 Libraries**: Complete X11 development stack
- **Additional Libraries**: PNG, JPEG, FreeType, FontConfig, etc.
- **Build Utilities**: Flex, Bison, pkg-config, etc.
- **Debugging Tools**: GDB, Valgrind, strace, ltrace

### Build Configuration

The build process uses optimized settings:

- **Parallel Builds**: Uses all available CPU cores
- **Standard Configuration**: `--enable-shared --with-xft --enable-jpeg --enable-png`
- **Debug Support**: Optional debug builds with `--enable-debug`
- **Comprehensive Testing**: Runs test suite when available

## Logging and Debugging

### Log Structure

Logs are organized by session timestamp:

```
logs/
└── 20240101_120000/          # Session timestamp
    ├── test_archlinux.log    # Arch Linux test log
    ├── test_freebsd.log      # FreeBSD test log
    └── ...
```

### Log Contents

Each log file contains:

- **System Information**: OS, kernel, CPU, memory details
- **Build Environment**: Compiler versions, environment variables
- **Configuration Output**: Autotools configuration results
- **Build Output**: Compilation messages, warnings, errors
- **Test Results**: Test execution and results
- **Installation Verification**: Installed files and libraries
- **Build Report**: Summary and timing information

### Debugging Failed Builds

```bash
# View latest logs
./test-motif.sh --logs-only

# Run with verbose output
./test-motif.sh -v archlinux

# Keep container running for investigation
./test-motif.sh -k archlinux

# Connect to running container
docker exec -it motif-test-archlinux-<timestamp> /bin/bash

# Check specific log file
less logs/20240101_120000/test_archlinux.log
```

### Common Issues and Solutions

1. **Missing Dependencies**
   - Check container Dockerfile for required packages
   - Verify X11 development libraries are installed
   - Update package lists in `templates/os-config.yaml`

2. **Build Failures**
   - Review build log for specific error messages
   - Check if autotools configuration succeeded
   - Verify all required libraries are available

3. **Container Build Failures**
   - Ensure base image is available
   - Check internet connectivity for package downloads
   - Verify package names are correct for the OS

## Performance Considerations

### Resource Usage

- **CPU**: Uses all available cores for parallel builds
- **Memory**: Each container may use 1-2GB during build
- **Disk**: Containers can be 1-2GB each, plus build artifacts
- **Network**: Downloads packages during container build

### Container Image Caching

**Images are automatically cached** - you don't need to rebuild unless you want to:

#### **Default Behavior (Efficient)**
```bash
# First run - builds image (5-15 minutes)
./test-motif.sh --podman archlinux

# Second run - uses cached image (5-30 seconds!)
./test-motif.sh --podman archlinux
```

#### **When Images Rebuild**
- **Manual**: Using `--rebuild` flag
- **Automatic**: When Dockerfile changes
- **Updates**: When base image updates (e.g., `archlinux:latest`)

#### **Image Management**
```bash
# Check saved images
podman images | grep motif-test

# See image sizes and creation dates
podman images --format "table {{.Repository}}\t{{.Tag}}\t{{.Size}}\t{{.Created}}"

# Remove specific image (forces rebuild next time)
podman rmi motif-test-archlinux

# Clean up unused images (frees disk space)
podman image prune
```

### Optimization Tips

```bash
# Build once per OS, then use cache
./test-motif.sh --podman archlinux  # Build once
./test-motif.sh --podman archlinux  # Fast subsequent runs (95% faster!)

# Weekly: Rebuild for package updates
./test-motif.sh --podman --rebuild archlinux

# Daily: Use cached images for fast testing
./test-motif.sh --podman --all

# Space management
podman system df                    # Check disk usage
podman system prune                 # Clean unused images
```

### Performance Benefits

| Operation | First Build | Cached Build | Time Savings |
|-----------|-------------|--------------|--------------|
| Single OS | 5-15 min | 5-30 sec | 95%+ faster |
| All OS | 15-45 min | 1-2 min | 90%+ faster |
| Daily testing | N/A | Seconds | Instant |

### Storage Requirements

- **Base images**: 100-500MB each
- **Built images**: 1-2GB each (with dev tools)
- **Total storage**: 2-10GB (depends on number of OS)
- **Build artifacts**: Cleaned automatically

## Integration with Development Workflow

### Continuous Integration

The environment can be integrated into CI/CD pipelines:

```bash
# Test all supported platforms
./tools/dev/env/test-motif.sh --all --clean

# Check exit code
if [ $? -eq 0 ]; then
    echo "All tests passed"
else
    echo "Some tests failed"
    exit 1
fi
```

### Local Development

```bash
# Initial setup (builds images once)
./add-os.sh archlinux
./add-os.sh ubuntu
./add-os.sh fedora

# Quick test on primary platform (uses cache)
./test-motif.sh archlinux

# Full compatibility test before release (uses cache)
./test-motif.sh --all

# Debug specific build issue
./test-motif.sh -v -k problematic-os

# Weekly refresh for updated packages
./test-motif.sh --rebuild --all
```

### Efficient Development Workflow

```bash
# Day 1: Initial setup
./test-motif.sh --podman archlinux     # Build: 10 min
./test-motif.sh --podman ubuntu        # Build: 8 min

# Day 2-7: Fast daily testing
./test-motif.sh --podman archlinux     # Cached: 30 sec
./test-motif.sh --podman --all         # Cached: 1 min

# Week 2: Refresh images
./test-motif.sh --podman --rebuild --all  # Rebuild: 20 min
# Then back to fast cached testing...
```

### Release Testing

```bash
# Comprehensive release testing
./test-motif.sh --all --verbose --clean > release-test.log 2>&1

# Generate test report
echo "Release Test Report" > report.txt
echo "==================" >> report.txt
./test-motif.sh --logs-only >> report.txt
```

## Customization

### Custom Build Scripts

Modify `scripts/build-motif.sh` to customize the build process:

- Add custom configure options
- Include additional tests
- Modify build flags
- Add custom verification steps

### Custom Container Images

Create custom Dockerfiles in `containers/`:

- Add specific development tools
- Include custom libraries
- Configure specific environments
- Add debugging utilities

### Environment Variables

Control build behavior with environment variables:

```bash
# Use specific compiler
CC=clang ./test-motif.sh archlinux

# Custom build flags
CFLAGS="-O3 -march=native" ./test-motif.sh archlinux

# Debug build
BUILD_TYPE=debug ./test-motif.sh archlinux
```

## Troubleshooting

### Common Problems

1. **"Container engine not found"**
   - Install Docker or Podman
   - Ensure it's in PATH
   - Use `--podman` flag if using Podman

2. **"Permission denied" (Docker socket error)**
   
   **Error message example:**
   ```
   permission denied while trying to connect to the Docker daemon socket at 
   unix:///var/run/docker.sock: Post "http://%2Fvar%2Frun%2Fdocker.sock/...": 
   dial unix /var/run/docker.sock: connect: permission denied
   ```
   
   This is the most common issue when Docker is installed but your user lacks permissions.
   
   **Quick Diagnosis:**
   ```bash
   # Check if you're in the docker group
   groups $USER | grep docker
   
   # Check if Docker service is running
   sudo systemctl status docker
   
   # Check if Podman is available as alternative
   command -v podman
   ```
   
   **Solution 1 - Add user to docker group (recommended):**
   ```bash
   # Add your user to the docker group
   sudo usermod -aG docker $USER
   
   # Apply the group change immediately (choose one):
   # Option A: Log out and log back in
   # Option B: Use newgrp to apply immediately
   newgrp docker
   
   # Verify it works
   docker run hello-world
   
   # Now you can run tests normally
   ./test-motif.sh archlinux
   ```
   
   **Solution 2 - Use Podman (rootless, recommended for security):**
   ```bash
   # Install podman if not available
   sudo apt install podman     # Ubuntu/Debian
   sudo pacman -S podman        # Arch Linux
   sudo dnf install podman      # Fedora/CentOS
   
   # Use podman instead (no permissions needed)
   ./test-motif.sh --podman archlinux
   
   # Make podman default (optional)
   export CONTAINER_ENGINE=podman
   ./test-motif.sh archlinux
   
   # Or add to your shell profile
   echo 'export CONTAINER_ENGINE=podman' >> ~/.bashrc
   ```
   
   **Solution 3 - Use sudo (quick fix):**
   ```bash
   sudo ./test-motif.sh archlinux
   ```
   
   **Solution 4 - Start Docker service:**
   ```bash
   # Check if Docker service is running
   sudo systemctl status docker
   
   # Start Docker service if needed
   sudo systemctl start docker
   sudo systemctl enable docker
   ```
   
   **Note:** Podman is often preferred because:
   - Runs rootless (more secure)
   - No daemon required
   - Drop-in replacement for Docker
   - Available on most Linux distributions

3. **"Dockerfile not found"**
   - Generate it: `./add-os.sh os-name`
   - Check `containers/` directory
   - Verify OS name is correct

4. **"Build failed"**
   - Check logs in `logs/` directory
   - Run with `-v` for verbose output
   - Use `-k` to keep container for debugging

5. **"Legacy builder deprecated" warning**
   
   **Warning message:**
   ```
   DEPRECATED: The legacy builder is deprecated and will be removed in a future release.
               Install the buildx component to build images with BuildKit:
               https://docs.docker.com/go/buildx/
   ```
   
   This is just a warning and doesn't prevent builds from working.
   
   **Solutions:**
   ```bash
   # Option 1: Install buildx (Ubuntu/Debian)
   sudo apt install docker-buildx-plugin
   
   # Option 2: Install buildx (Arch Linux)
   sudo pacman -S docker-buildx
   
   # Option 3: Use Podman (no warning)
   ./test-motif.sh --podman archlinux
   
   # Option 4: Ignore the warning - builds still work fine
   ```

6. **"Can't add file to tar: io: read/write on closed pipe"**
   
   This error often accompanies permission issues and indicates the build process
   was interrupted due to Docker socket permission problems.
   
   **Solution:** Fix the underlying permission issue (see #2 above), then retry:
   ```bash
   # After fixing permissions, clean and retry
   ./test-motif.sh --clean archlinux
   ./test-motif.sh archlinux
   ```

7. **"short-name did not resolve to an alias" (Podman registry error)**
   
   **Error message example:**
   ```
   Error: creating build container: short-name "archlinux:latest" did not resolve 
   to an alias and no unqualified-search registries are defined in 
   "/etc/containers/registries.conf"
   ```
   
   This happens when Podman can't resolve short image names because no default 
   registries are configured.
   
   **Solution 1 - Configure registries (system-wide):**
   ```bash
   # Edit registries configuration
   sudo nano /etc/containers/registries.conf
   
   # Add these lines:
   [registries.search]
   registries = ['docker.io', 'registry.fedoraproject.org', 'quay.io', 'registry.access.redhat.com']
   
   # Or for user-specific config:
   mkdir -p ~/.config/containers
   cp /etc/containers/registries.conf ~/.config/containers/
   # Then edit ~/.config/containers/registries.conf
   ```
   
   **Solution 2 - Use fully qualified names (already implemented):**
   ```bash
   # The Dockerfiles have been updated to use fully qualified registry names
   # This should work without additional configuration
   ./test-motif.sh --podman archlinux
   ```
   
   **Solution 3 - Configure registries for current session:**
   ```bash
   # Set environment variable for current session
   export CONTAINERS_REGISTRIES_CONF_PATH=/dev/null
   
   # Or create temporary config
   mkdir -p ~/.config/containers
   cat > ~/.config/containers/registries.conf << 'EOF'
   [registries.search]
   registries = ['docker.io', 'registry.fedoraproject.org', 'quay.io']
   EOF
   ```

### Quick Error Reference

| Error Pattern | Likely Cause | Quick Fix |
|---------------|--------------|-----------|
| `permission denied...docker.sock` | User not in docker group | `sudo usermod -aG docker $USER && newgrp docker` |
| `short-name did not resolve to an alias` | Podman registry config missing | Use updated Dockerfiles (fixed) or configure registries |
| `DEPRECATED: The legacy builder` | Old Docker version | Use `--podman` or install `docker-buildx-plugin` |
| `Can't add file to tar` | Permission/build interrupted | Fix permissions, then `./test-motif.sh --clean os-name` |
| `Container engine not found` | Docker/Podman not installed | Install Docker/Podman or check PATH |
| `Dockerfile not found` | OS environment missing | Run `./add-os.sh os-name` first |

### Getting Help

1. **Check logs**: `./test-motif.sh --logs-only`
2. **Verbose output**: `./test-motif.sh -v os-name`
3. **List available options**: `./test-motif.sh --help`
4. **Check container status**: `docker ps -a | grep motif-test` or `podman ps -a | grep motif-test`
5. **Diagnostic commands**:
   ```bash
   # Check Docker permissions
   groups $USER | grep docker
   
   # Check available container engines
   command -v docker && echo "Docker available"
   command -v podman && echo "Podman available"
   
   # Check Docker service
   sudo systemctl status docker
   ```

## Contributing

### Adding New OS Support

1. Research OS package names and build requirements
2. Add configuration to `templates/os-config.yaml`
3. Test with `./add-os.sh --dry-run os-name`
4. Generate and test: `./add-os.sh os-name && ./test-motif.sh os-name`
5. Submit changes with test results

### Improving Build Scripts

1. Modify `scripts/build-motif.sh`
2. Test on multiple OS platforms
3. Ensure backward compatibility
4. Document changes

### Enhancing Templates

1. Update `templates/Dockerfile.template`
2. Test template generation: `./add-os.sh --dry-run existing-os`
3. Verify all OS configurations work
4. Update documentation

## License

This development environment is part of the XDE Motif project and follows the same license terms.

## Support

For issues and questions:

1. Check this documentation
2. Review existing logs and error messages
3. Test with verbose output
4. Report issues with detailed logs and system information
