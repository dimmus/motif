# XM Logging System Configuration

## Overview

The XM Logging System provides configurable logging output destinations that can be set at both build time and runtime. This document describes how to configure and use the logging system.

## Build-Time Configuration

### Configure Options

The logging system can be configured during the build process using the following configure options:

#### Logging System
- **Status**: Always enabled
- **Description**: The enhanced logging system is always compiled in and available
- **Note**: Logging cannot be disabled at build time

#### `--with-log-level=LEVEL`
- **Default**: `INFO`
- **Description**: Set the default logging level
- **Valid values**: `DEBUG`, `INFO`, `WARN`, `ERROR`, `CRITICAL`
- **Example**: `./configure --with-log-level=DEBUG`

#### `--with-log-output=DEST`
- **Default**: `stderr`
- **Description**: Set the default log output destination
- **Valid values**: `stderr`, `stdout`, `file`
- **Example**: `./configure --with-log-output=stdout`

### Configuration Examples

```bash
# Development build with debug logging to stdout
./configure --with-log-level=DEBUG --with-log-output=stdout

# Production build with error logging to stderr
./configure --with-log-level=ERROR --with-log-output=stderr

# Default build (logging always enabled with INFO level to stderr)
./configure
```

## Runtime Configuration

### Initialization

The logging system must be initialized before use:

```c
#include "Log.h"

int main() {
    // Initialize logging system with build-time defaults
    if (!XmLogInit()) {
        fprintf(stderr, "Failed to initialize logging system\n");
        return 1;
    }
    
    // Your application code here
    return 0;
}
```

### Changing Output Destination

You can change the log output destination at runtime:

```c
// Switch to stdout
XmLogSetOutput("stdout", NULL);

// Switch to stderr
XmLogSetOutput("stderr", NULL);

// Switch to file output
XmLogSetOutput("file", "myapp.log");

// Switch to file with default name (motif.log)
XmLogSetOutput("file", NULL);
```

## Output Destinations

### stderr
- **Use case**: Development, debugging, error reporting
- **Behavior**: Logs are written to standard error stream
- **Color support**: Yes (if terminal supports it)

### stdout
- **Use case**: Integration with logging systems, system logs
- **Behavior**: Logs are written to standard output stream
- **Color support**: Yes (if terminal supports it)

### file
- **Use case**: Production logging, log analysis, debugging
- **Behavior**: Logs are written to a specified file
- **Color support**: No (raw text file)
- **Default filename**: `motif.log` (if no filename specified)

## Log Levels

The logging system supports five levels (in order of severity):

1. **DEBUG** - Detailed debugging information
2. **INFO** - General information messages
3. **WARN** - Warning messages
4. **ERROR** - Error conditions
5. **CRITICAL** - Critical error conditions

Only messages at or above the configured level are output.

## Configuration Defines

The following preprocessor defines are set based on the configuration:

- Logging is always enabled (no conditional compilation needed)
- `XM_DEFAULT_LOG_LEVEL` - Set to the configured log level constant
- `XM_DEFAULT_LOG_OUTPUT` - Set to the configured output destination string

## Example Usage

```c
#include "Log.h"

int main() {
    // Initialize logging
    XmLogInit();
    
    // Register a custom domain
    int my_domain = XmLogDomainRegister("MyApp", LOG_COLOR_GREEN);
    
    // Log messages
    XmLogPrint(my_domain, XM_LOG_INFO, __FILE__, __FUNCTION__, __LINE__,
               "Application started");
    
    // Change output to file
    XmLogSetOutput("file", "myapp.log");
    
    XmLogPrint(my_domain, XM_LOG_WARN, __FILE__, __FUNCTION__, __LINE__,
               "This will be written to myapp.log");
    
    return 0;
}
```

## Integration with Existing Code

The logging system is designed to integrate seamlessly with existing Motif code. When logging is enabled, existing debug and error messages are automatically converted to use the new logging system.

### Automatic Integration

The following files have been updated to use the new logging system:
- `lib/Xm/Paned.c` - Error messages
- `lib/Xm/Xmos.c` - Debug process locking messages
- `lib/Xm/FontS.c` - Debug macro
- `lib/Xm/DataF.c` - Debug printf statements
- `lib/Xm/TabStack.c` - Debug geometry messages

### Conditional Compilation

The logging system is always available and compiled in:

```c
// Logging is always available
XmLogPrint(domain, level, file, func, line, fmt, ...);
```

## Troubleshooting

### Logs Not Appearing
1. Logging is always enabled - no need to check
2. Verify log level: `grep XM_DEFAULT_LOG_LEVEL config.h`
3. Check output destination: `grep XM_DEFAULT_LOG_OUTPUT config.h`

### File Output Issues
1. Ensure the application has write permissions to the log file directory
2. Check if the log file is being created in the expected location
3. Verify the filename is valid and not already in use

### Performance Considerations
- File output may be slower than stderr/stdout
- DEBUG level logging can generate significant output
- Consider using ERROR or WARN level in production builds

## Migration Guide

### From Traditional printf/fprintf
```c
// Old way
printf("Debug: value = %d\n", value);
fprintf(stderr, "Error: %s\n", error_msg);

// New way
XmLogPrint(domain, XM_LOG_DEBUG, __FILE__, __FUNCTION__, __LINE__,
           "Debug: value = %d", value);
XmLogPrint(domain, XM_LOG_ERROR, __FILE__, __FUNCTION__, __LINE__,
           "Error: %s", error_msg);
```

### From Debug Macros
```c
// Old way
#define DEBUG_PRINT(fmt, ...) printf("DEBUG: " fmt "\n", ##__VA_ARGS__)

// New way
#define DEBUG_PRINT(fmt, ...) \
    XmLogPrint(domain, XM_LOG_DEBUG, __FILE__, __FUNCTION__, __LINE__, \
               fmt, ##__VA_ARGS__)
```
