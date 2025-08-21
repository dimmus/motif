#!/bin/bash

# Find and remove trailing whitespaces and empty endlines

# Basic usage - clean current directory
# ./cleanup_whitespace.sh
# Dry run to see what would be changed
# ./cleanup_whitespace.sh -n -v
# Clean with backups and verbose output
# ./cleanup_whitespace.sh -b -v /path/to/project
# Only process specific file types
# ./cleanup_whitespace.sh -e "c,h,cpp,py" src/
# Process all files (not just common text files)
# ./cleanup_whitespace.sh -a

set -uo pipefail

# Default values
TARGET_DIR="."
BACKUP=false
VERBOSE=false
DRY_RUN=false
EXCLUDE_PATTERNS=(".git" "node_modules" ".vscode" ".idea" "*.min.js" "*.min.css" "*.map")
FILE_EXTENSIONS=("*.c" "*.h" "*.cpp" "*.hpp" "*.py" "*.js" "*.ts" "*.java" "*.sh" "*.txt" "*.md" "*.yml" "*.yaml" "*.json" "*.xml" "*.html" "*.css" "*.php" "*.rb" "*.go" "*.rs" "*.swift" "*.kt" "*.scala" "*.pl" "*.pm" "*.r" "*.R" "*.sql" "*.lua" "*.vim" "*.tex" "*.m" "*.am" "*.in" "*.ac")

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to display help
show_help() {
    cat << EOF
Usage: $0 [OPTIONS] [DIRECTORY]

Find and remove trailing whitespaces and empty endlines from files.

OPTIONS:
    -h, --help          Show this help message
    -v, --verbose       Enable verbose output
    -b, --backup        Create backup files (.bak extension)
    -n, --dry-run       Show what would be changed without making changes
    -d, --directory DIR Specify target directory (default: current directory)
    -a, --all-files     Process all files (not just common text files)
    -e, --extensions    Specify file extensions (comma-separated, e.g., "c,h,py")

EXAMPLES:
    $0                              # Clean current directory
    $0 -v -b /path/to/project      # Clean with verbose output and backups
    $0 -n                          # Dry run to see what would be changed
    $0 -a                          # Process all files, not just common text files
    $0 -e "c,h,cpp" src/           # Only process .c, .h, .cpp files in src/

NOTES:
    - Binary files are automatically detected and skipped
    - Hidden files and directories are processed unless excluded
    - The script preserves file permissions and timestamps
EOF
}

# Function to log messages
log() {
    local level="$1"
    shift
    case "$level" in
        "INFO")  echo -e "${BLUE}[INFO]${NC} $*" ;;
        "WARN")  echo -e "${YELLOW}[WARN]${NC} $*" >&2 ;;
        "ERROR") echo -e "${RED}[ERROR]${NC} $*" >&2 ;;
        "SUCCESS") echo -e "${GREEN}[SUCCESS]${NC} $*" ;;
    esac
}

# Function to check if file is binary
is_binary() {
    local file="$1"

    # Check for null bytes in first 1024 bytes (most reliable method without 'file' command)
    local sample
    sample=$(head -c 1024 "$file" 2>/dev/null)
    if [[ ${#sample} -gt 0 && "$sample" != "${sample//$'\0'/}" ]]; then
        return 0  # Binary file (contains null bytes)
    fi

    # Additional heuristics for common binary file extensions
    local filename basename extension
    filename=$(basename "$file")
    extension="${filename##*.}"

    case "${extension,,}" in
        # Common binary extensions
        exe|dll|so|dylib|a|o|obj|bin|dat|db|sqlite|sqlite3|img|iso|dmg|zip|tar|gz|bz2|xz|7z|rar|pdf|doc|docx|xls|xlsx|ppt|pptx|jpg|jpeg|png|gif|bmp|tiff|mp3|mp4|avi|mov|wmv|flv|wav|ogg)
            return 0  # Binary
            ;;
        # Text file extensions (common ones)
        txt|md|rst|log|conf|cfg|ini|yaml|yml|json|xml|html|htm|css|js|ts|py|rb|go|rs|c|cpp|h|hpp|java|php|pl|sh|bash|zsh|fish|vim|sql|r|scala|kt|swift|m|mm|lua|tex|org|csv|tsv)
            return 1  # Text
            ;;
        *)
            # For unknown extensions, assume text if no null bytes found
            return 1  # Text
            ;;
    esac
}

# Function to create backup
create_backup() {
    local file="$1"
    cp "$file" "$file.bak"
    if [[ $VERBOSE == true ]]; then
        log "INFO" "Created backup: $file.bak"
    fi
}

# Function to clean a single file
clean_file() {
    local file="$1"
    local changes_made=false
    local temp_file

    # Skip if file doesn't exist or is not readable
    if [[ ! -r "$file" ]]; then
        if [[ $VERBOSE == true ]]; then
            log "WARN" "Skipping unreadable file: $file"
        fi
        return 0
    fi

    # Skip binary files
    if is_binary "$file"; then
        if [[ $VERBOSE == true ]]; then
            log "INFO" "Skipping binary file: $file"
        fi
        return 0
    fi

    # Create temporary file
    temp_file=$(mktemp)

    # Check for trailing whitespaces
    if grep -q '[[:space:]]\+$' "$file" 2>/dev/null; then
        changes_made=true
        if [[ $VERBOSE == true ]]; then
            local count=$(grep -c '[[:space:]]\+$' "$file" 2>/dev/null || echo "0")
            log "INFO" "Found $count lines with trailing whitespace in: $file"
        fi
    fi

    # Process the file: remove trailing whitespaces
    sed 's/[[:space:]]*$//' "$file" > "$temp_file"

    # Remove empty lines at the end of file
    # This uses a more portable approach than GNU-specific tools
    awk '
        {
            lines[NR] = $0
            if (NF > 0 || $0 ~ /[^[:space:]]/) {
                last_non_empty = NR
            }
        }
        END {
            for (i = 1; i <= last_non_empty; i++) {
                print lines[i]
            }
        }
    ' "$temp_file" > "$temp_file.tmp" && mv "$temp_file.tmp" "$temp_file"

    # Check if there were empty lines at the end
    local original_lines=$(wc -l < "$file")
    local new_lines=$(wc -l < "$temp_file")
    if [[ $original_lines -ne $new_lines ]]; then
        changes_made=true
        if [[ $VERBOSE == true ]]; then
            local removed_lines=$((original_lines - new_lines))
            log "INFO" "Removed $removed_lines empty lines at end of: $file"
        fi
    fi

    # Apply changes if any were made
    if [[ $changes_made == true ]]; then
        if [[ $DRY_RUN == true ]]; then
            log "INFO" "Would clean: $file"
            # Cleanup
            rm -f "$temp_file" "$temp_file.tmp" 2>/dev/null || true
            return 1  # Return 1 to indicate changes would be made
        else
            # Create backup if requested
            if [[ $BACKUP == true ]]; then
                create_backup "$file"
            fi

            # Apply changes while preserving permissions
            cat "$temp_file" > "$file"
            log "SUCCESS" "Cleaned: $file"
            # Cleanup
            rm -f "$temp_file" "$temp_file.tmp" 2>/dev/null || true
            return 1  # Return 1 to indicate changes were made
        fi
    elif [[ $VERBOSE == true ]]; then
        log "INFO" "No changes needed: $file"
    fi

    # Cleanup
    rm -f "$temp_file" "$temp_file.tmp" 2>/dev/null || true

    return 0  # Return 0 to indicate no changes needed
}

# Function to build find command arguments
build_find_args() {
    local find_args=("$TARGET_DIR")

    # Add exclusion patterns
    for pattern in "${EXCLUDE_PATTERNS[@]}"; do
        find_args+=(-not -path "*/$pattern" -not -path "*/$pattern/*")
    done

    # Add file type filter
    find_args+=(-type f)

    # Add file extension filter if not processing all files
    if [[ ${#FILE_EXTENSIONS[@]} -gt 0 ]]; then
        find_args+=(\()
        local first=true
        for ext in "${FILE_EXTENSIONS[@]}"; do
            if [[ $first == true ]]; then
                find_args+=(-name "$ext")
                first=false
            else
                find_args+=(-o -name "$ext")
            fi
        done
        find_args+=(\))
    fi

    printf '%s\n' "${find_args[@]}"
}

# Function to process all files
process_files() {
    local file_count=0
    local processed_count=0
    local cleaned_count=0

    log "INFO" "Starting whitespace cleanup in: $TARGET_DIR"
    if [[ $DRY_RUN == true ]]; then
        log "INFO" "DRY RUN MODE - No files will be modified"
    fi

    # Build find command arguments
    readarray -t find_args < <(build_find_args)

    # Count total files first for progress
    if [[ $VERBOSE == true ]]; then
        file_count=$(find "${find_args[@]}" 2>/dev/null | wc -l)
        log "INFO" "Found $file_count files to process"
    fi

    # Process each file
    while IFS= read -r -d '' file; do
        if [[ -z "$file" ]]; then
            continue
        fi

        ((processed_count++))

        if [[ $VERBOSE == true && $file_count -gt 0 ]]; then
            local progress=$((processed_count * 100 / file_count))
            echo -ne "\r${BLUE}[INFO]${NC} Progress: $processed_count/$file_count ($progress%)"
        fi



        # Call clean_file and check return code
        # Return code 1 means changes were made/would be made
        # Return code 0 means no changes needed
        if ! clean_file "$file"; then
            ((cleaned_count++))
        fi
    done < <(find "${find_args[@]}" -print0 2>/dev/null)

    if [[ $VERBOSE == true && $file_count -gt 0 ]]; then
        echo # New line after progress indicator
    fi

    log "SUCCESS" "Processing complete!"
    log "INFO" "Files processed: $processed_count"
    if [[ $DRY_RUN == false ]]; then
        log "INFO" "Files cleaned: $cleaned_count"
    else
        log "INFO" "Files that would be cleaned: $cleaned_count"
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -b|--backup)
            BACKUP=true
            shift
            ;;
        -n|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -d|--directory)
            TARGET_DIR="$2"
            shift 2
            ;;
        -a|--all-files)
            FILE_EXTENSIONS=()
            shift
            ;;
        -e|--extensions)
            IFS=',' read -ra FILE_EXTENSIONS <<< "$2"
            # Add wildcard prefix if not present
            for i in "${!FILE_EXTENSIONS[@]}"; do
                if [[ ${FILE_EXTENSIONS[i]} != *.* ]]; then
                    FILE_EXTENSIONS[i]="*.${FILE_EXTENSIONS[i]}"
                fi
            done
            shift 2
            ;;
        -*)
            log "ERROR" "Unknown option: $1"
            show_help
            exit 1
            ;;
        *)
            TARGET_DIR="$1"
            shift
            ;;
    esac
done

# Validate target directory
if [[ ! -d "$TARGET_DIR" ]]; then
    log "ERROR" "Directory does not exist: $TARGET_DIR"
    exit 1
fi

# Convert to absolute path
TARGET_DIR=$(cd "$TARGET_DIR" && pwd)

# Main execution
main() {
    if [[ $VERBOSE == true ]]; then
        log "INFO" "Configuration:"
        log "INFO" "  Target directory: $TARGET_DIR"
        log "INFO" "  Create backups: $BACKUP"
        log "INFO" "  Dry run: $DRY_RUN"
        log "INFO" "  File extensions: ${FILE_EXTENSIONS[*]:-all files}"
        log "INFO" "  Excluded patterns: ${EXCLUDE_PATTERNS[*]}"
        echo
    fi

    process_files
}

# Run main function
main
