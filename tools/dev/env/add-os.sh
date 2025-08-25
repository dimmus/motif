#!/bin/bash
#
# Add New OS Environment Script
# This script generates a new Dockerfile for testing Motif on a new operating system
# using the template system and OS configuration file.
#
# Usage: ./add-os.sh [OPTIONS] OS_NAME
#
# Options:
#   -h, --help          Show this help message
#   -l, --list          List available OS templates
#   -f, --force         Overwrite existing Dockerfile
#   -t, --template FILE Use custom template file
#   -c, --config FILE   Use custom config file
#   -v, --verbose       Verbose output
#   --dry-run           Show what would be generated without creating files
#
# Examples:
#   ./add-os.sh ubuntu          # Generate Dockerfile for Ubuntu
#   ./add-os.sh -f archlinux    # Overwrite existing Arch Linux Dockerfile
#   ./add-os.sh -l              # List available OS templates
#

set -euo pipefail

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEMPLATES_DIR="${SCRIPT_DIR}/templates"
CONTAINERS_DIR="${SCRIPT_DIR}/containers"
DEFAULT_TEMPLATE="${TEMPLATES_DIR}/Dockerfile.template"
DEFAULT_CONFIG="${TEMPLATES_DIR}/os-config.yaml"

# Default settings
FORCE=false
VERBOSE=false
DRY_RUN=false
TEMPLATE_FILE="${DEFAULT_TEMPLATE}"
CONFIG_FILE="${DEFAULT_CONFIG}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

log_debug() {
    if [[ "${VERBOSE}" == "true" ]]; then
        echo -e "${PURPLE}[DEBUG]${NC} $*"
    fi
}

# Help function
show_help() {
    cat << EOF
Add New OS Environment Script

This script generates a new Dockerfile for testing Motif on a new operating system
using the template system and OS configuration file.

Usage: $(basename "$0") [OPTIONS] OS_NAME

Options:
  -h, --help          Show this help message
  -l, --list          List available OS templates
  -f, --force         Overwrite existing Dockerfile
  -t, --template FILE Use custom template file (default: ${DEFAULT_TEMPLATE})
  -c, --config FILE   Use custom config file (default: ${DEFAULT_CONFIG})
  -v, --verbose       Verbose output
  --dry-run           Show what would be generated without creating files

Available OS templates:
$(list_available_templates | sed 's/^/  - /')

Examples:
  $(basename "$0") ubuntu              # Generate Dockerfile for Ubuntu
  $(basename "$0") -f archlinux        # Overwrite existing Arch Linux Dockerfile
  $(basename "$0") -l                  # List available OS templates
  $(basename "$0") --dry-run fedora    # Preview Fedora Dockerfile generation

Template file: ${DEFAULT_TEMPLATE}
Config file: ${DEFAULT_CONFIG}
Output directory: ${CONTAINERS_DIR}
EOF
}

# List available OS templates from config file
list_available_templates() {
    if [[ -f "${CONFIG_FILE}" ]]; then
        # Extract OS names from YAML config (simple parsing)
        grep '^[a-zA-Z][a-zA-Z0-9_-]*:' "${CONFIG_FILE}" | sed 's/:.*$//' | sort
    else
        echo "No configuration file found: ${CONFIG_FILE}"
    fi
}

# Check dependencies
check_dependencies() {
    local missing_deps=()
    local required_commands=("sed" "grep" "awk")
    
    for cmd in "${required_commands[@]}"; do
        if ! command -v "${cmd}" &> /dev/null; then
            missing_deps+=("${cmd}")
        fi
    done
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        return 1
    fi
    
    return 0
}

# Parse YAML config for specific OS
parse_os_config() {
    local os_name="$1"
    local config_file="$2"
    
    if [[ ! -f "${config_file}" ]]; then
        log_error "Config file not found: ${config_file}"
        return 1
    fi
    
    # Check if OS exists in config
    if ! grep -q "^${os_name}:" "${config_file}"; then
        log_error "OS '${os_name}' not found in config file"
        log_info "Available OS templates:"
        list_available_templates | sed 's/^/  - /'
        return 1
    fi
    
    log_debug "Parsing configuration for ${os_name}"
    
    # Extract OS configuration using awk
    awk -v os="${os_name}" '
    BEGIN { in_os = 0; indent_level = 0 }
    /^[a-zA-Z][a-zA-Z0-9_-]*:/ {
        if ($0 ~ "^" os ":") {
            in_os = 1
            indent_level = 0
        } else {
            in_os = 0
        }
        next
    }
    in_os && /^  / {
        if (match($0, /^( +)/)) {
            current_indent = RLENGTH
            if (indent_level == 0) indent_level = current_indent
            if (current_indent == indent_level) {
                print $0
            } else if (current_indent > indent_level) {
                print $0
            }
        }
    }
    in_os && /^[^ ]/ && !/^[a-zA-Z][a-zA-Z0-9_-]*:/ {
        in_os = 0
    }
    ' "${config_file}"
}

# Extract specific value from OS config
get_config_value() {
    local os_name="$1"
    local key_path="$2"
    local config_data="$3"
    
    # Simple key extraction for nested YAML
    echo "${config_data}" | awk -v key="${key_path}" '
    BEGIN { found = 0; in_section = 0 }
    $0 ~ "^  " key ":" {
        found = 1
        gsub(/^  [^:]+: *"?/, "")
        gsub(/"$/, "")
        print $0
        exit
    }
    '
}

# Extract package list from config
get_package_list() {
    local os_name="$1"
    local section="$2"
    local config_data="$3"
    
    echo "${config_data}" | awk -v section="${section}" '
    BEGIN { in_section = 0 }
    $0 ~ "^    " section ":" { in_section = 1; next }
    in_section && /^    [a-zA-Z]/ { in_section = 0 }
    in_section && /^      - / {
        gsub(/^      - /, "")
        packages[++count] = $0
    }
    END {
        for (i = 1; i <= count; i++) {
            if (i < count) printf "%s \\\n        ", packages[i]
            else printf "%s", packages[i]
        }
    }
    '
}

# Generate package installation commands
generate_install_commands() {
    local os_name="$1"
    local config_data="$2"
    local package_manager=$(get_config_value "${os_name}" "package_manager" "${config_data}")
    local install_cmd=$(get_config_value "${os_name}" "commands.*install" "${config_data}")
    
    # Get install command from config
    local install_command=$(echo "${config_data}" | awk '
    /^    commands:/ { in_commands = 1; next }
    in_commands && /^    [a-zA-Z]/ { in_commands = 0 }
    in_commands && /^      install:/ {
        gsub(/^      install: *"?/, "")
        gsub(/"$/, "")
        print $0
        exit
    }
    ')
    
    if [[ -z "${install_command}" ]]; then
        case "${package_manager}" in
            "apt")
                install_command="apt-get install -y"
                ;;
            "yum"|"dnf")
                install_command="dnf install -y"
                ;;
            "pacman")
                install_command="pacman -S --noconfirm"
                ;;
            "zypper")
                install_command="zypper install -y"
                ;;
            *)
                install_command="# Unknown package manager: ${package_manager}"
                ;;
        esac
    fi
    
    echo "${install_command}"
}

# Generate Dockerfile from template
generate_dockerfile() {
    local os_name="$1"
    local template_file="$2"
    local config_file="$3"
    local output_file="$4"
    
    log_info "Generating Dockerfile for ${os_name}..."
    
    # Parse OS configuration
    local config_data=$(parse_os_config "${os_name}" "${config_file}")
    if [[ -z "${config_data}" ]]; then
        log_error "Failed to parse configuration for ${os_name}"
        return 1
    fi
    
    log_debug "Configuration data extracted"
    
    # Extract configuration values
    local base_image=$(get_config_value "${os_name}" "base_image" "${config_data}")
    local description=$(get_config_value "${os_name}" "description" "${config_data}")
    local package_manager=$(get_config_value "${os_name}" "package_manager" "${config_data}")
    local compiler=$(get_config_value "${os_name}" "compiler" "${config_data}")
    local cflags=$(get_config_value "${os_name}" "cflags" "${config_data}")
    local cxxflags=$(get_config_value "${os_name}" "cxxflags" "${config_data}")
    local ldflags=$(get_config_value "${os_name}" "ldflags" "${config_data}")
    
    log_debug "Base image: ${base_image}"
    log_debug "Description: ${description}"
    log_debug "Package manager: ${package_manager}"
    
    # Get package lists
    local base_tools=$(get_package_list "${os_name}" "base_tools" "${config_data}")
    local x11_deps=$(get_package_list "${os_name}" "x11_deps" "${config_data}")
    local additional_libs=$(get_package_list "${os_name}" "additional_libs" "${config_data}")
    local build_tools=$(get_package_list "${os_name}" "build_tools" "${config_data}")
    local doc_tools=$(get_package_list "${os_name}" "doc_tools" "${config_data}")
    
    # Get commands
    local update_cmd=$(echo "${config_data}" | awk '/^    commands:/ { in_commands = 1; next } in_commands && /^    [a-zA-Z]/ { in_commands = 0 } in_commands && /^      update:/ { gsub(/^      update: *"?/, ""); gsub(/"$/, ""); print $0; exit }')
    local install_cmd=$(generate_install_commands "${os_name}" "${config_data}")
    local cleanup_cmd=$(echo "${config_data}" | awk '/^    commands:/ { in_commands = 1; next } in_commands && /^    [a-zA-Z]/ { in_commands = 0 } in_commands && /^      cleanup:/ { gsub(/^      cleanup: *"?/, ""); gsub(/"$/, ""); print $0; exit }')
    
    # Generate Dockerfile content
    local dockerfile_content=$(cat "${template_file}")
    
    # Replace template variables
    dockerfile_content="${dockerfile_content//\{\{OS_NAME\}\}/${os_name}}"
    dockerfile_content="${dockerfile_content//\{\{BASE_IMAGE\}\}/${base_image}}"
    dockerfile_content="${dockerfile_content//\{\{OS_DESCRIPTION\}\}/${description}}"
    
    # Replace package update commands
    if [[ -n "${update_cmd}" ]]; then
        dockerfile_content="${dockerfile_content//\{\{PACKAGE_UPDATE_COMMANDS\}\}/RUN ${update_cmd}}"
    else
        dockerfile_content="${dockerfile_content//\{\{PACKAGE_UPDATE_COMMANDS\}\}/# No update command specified}"
    fi
    
    # Replace package installation commands
    if [[ -n "${base_tools}" ]]; then
        dockerfile_content="${dockerfile_content//\{\{INSTALL_BASE_TOOLS\}\}/RUN ${install_cmd} \\
        ${base_tools}}"
    else
        dockerfile_content="${dockerfile_content//\{\{INSTALL_BASE_TOOLS\}\}/# No base tools specified}"
    fi
    
    if [[ -n "${x11_deps}" ]]; then
        dockerfile_content="${dockerfile_content//\{\{INSTALL_X11_DEPS\}\}/RUN ${install_cmd} \\
        ${x11_deps}}"
    else
        dockerfile_content="${dockerfile_content//\{\{INSTALL_X11_DEPS\}\}/# No X11 dependencies specified}"
    fi
    
    if [[ -n "${additional_libs}" ]]; then
        dockerfile_content="${dockerfile_content//\{\{INSTALL_ADDITIONAL_LIBS\}\}/RUN ${install_cmd} \\
        ${additional_libs}}"
    else
        dockerfile_content="${dockerfile_content//\{\{INSTALL_ADDITIONAL_LIBS\}\}/# No additional libraries specified}"
    fi
    
    if [[ -n "${build_tools}" ]]; then
        dockerfile_content="${dockerfile_content//\{\{INSTALL_BUILD_TOOLS\}\}/RUN ${install_cmd} \\
        ${build_tools}}"
    else
        dockerfile_content="${dockerfile_content//\{\{INSTALL_BUILD_TOOLS\}\}/# No build tools specified}"
    fi
    
    if [[ -n "${doc_tools}" ]]; then
        dockerfile_content="${dockerfile_content//\{\{INSTALL_DOC_TOOLS\}\}/RUN ${install_cmd} \\
        ${doc_tools}}"
    else
        dockerfile_content="${dockerfile_content//\{\{INSTALL_DOC_TOOLS\}\}/# No documentation tools specified}"
    fi
    
    # Replace cleanup command
    if [[ -n "${cleanup_cmd}" ]]; then
        dockerfile_content="${dockerfile_content//\{\{PACKAGE_CLEANUP\}\}/RUN ${cleanup_cmd}}"
    else
        dockerfile_content="${dockerfile_content//\{\{PACKAGE_CLEANUP\}\}/# No cleanup command specified}"
    fi
    
    # Replace build environment
    local build_env="ENV CC=${compiler:-gcc}
ENV CXX=\${CC%cc}++
ENV CFLAGS=\"${cflags:-\-O2 \-g}\"
ENV CXXFLAGS=\"${cxxflags:-\-O2 \-g}\"
ENV LDFLAGS=\"${ldflags:-}\"
ENV MAKEFLAGS=\"-j\$(nproc)\""
    
    dockerfile_content="${dockerfile_content//\{\{BUILD_ENVIRONMENT\}\}/${build_env}}"
    
    # Replace user creation
    local create_user="RUN useradd -m -s /bin/bash builder && \\
    echo 'builder ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers"
    dockerfile_content="${dockerfile_content//\{\{CREATE_BUILD_USER\}\}/${create_user}}"
    
    # Output result
    if [[ "${DRY_RUN}" == "true" ]]; then
        log_info "Dry run - would create: ${output_file}"
        echo ""
        echo "Generated Dockerfile content:"
        echo "============================="
        echo "${dockerfile_content}"
        echo "============================="
    else
        echo "${dockerfile_content}" > "${output_file}"
        log_success "Dockerfile created: ${output_file}"
    fi
    
    return 0
}

# Main function
main() {
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -l|--list)
                log_info "Available OS templates:"
                list_available_templates | sed 's/^/  - /'
                exit 0
                ;;
            -f|--force)
                FORCE=true
                shift
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            --dry-run)
                DRY_RUN=true
                shift
                ;;
            -t|--template)
                TEMPLATE_FILE="$2"
                shift 2
                ;;
            -c|--config)
                CONFIG_FILE="$2"
                shift 2
                ;;
            -*)
                log_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
            *)
                if [[ -n "${OS_NAME:-}" ]]; then
                    log_error "Multiple OS names specified: ${OS_NAME} and $1"
                    exit 1
                fi
                OS_NAME="$1"
                shift
                ;;
        esac
    done
    
    # Validate arguments
    if [[ -z "${OS_NAME:-}" ]]; then
        log_error "OS name is required"
        echo "Use --list to see available OS templates"
        echo "Use --help for usage information"
        exit 1
    fi
    
    # Check prerequisites
    check_dependencies || exit 1
    
    # Validate files
    if [[ ! -f "${TEMPLATE_FILE}" ]]; then
        log_error "Template file not found: ${TEMPLATE_FILE}"
        exit 1
    fi
    
    if [[ ! -f "${CONFIG_FILE}" ]]; then
        log_error "Config file not found: ${CONFIG_FILE}"
        exit 1
    fi
    
    # Create containers directory if it doesn't exist
    if [[ "${DRY_RUN}" == "false" ]]; then
        mkdir -p "${CONTAINERS_DIR}"
    fi
    
    # Determine output file
    local output_file="${CONTAINERS_DIR}/Dockerfile.${OS_NAME}"
    
    # Check if file exists and force flag
    if [[ -f "${output_file}" && "${FORCE}" == "false" && "${DRY_RUN}" == "false" ]]; then
        log_error "Dockerfile already exists: ${output_file}"
        log_info "Use --force to overwrite existing file"
        exit 1
    fi
    
    # Generate Dockerfile
    if generate_dockerfile "${OS_NAME}" "${TEMPLATE_FILE}" "${CONFIG_FILE}" "${output_file}"; then
        if [[ "${DRY_RUN}" == "false" ]]; then
            log_success "Successfully generated Dockerfile for ${OS_NAME}"
            log_info "You can now test it with: ./test-motif.sh ${OS_NAME}"
        else
            log_info "Dry run completed successfully"
        fi
    else
        log_error "Failed to generate Dockerfile for ${OS_NAME}"
        exit 1
    fi
}

# Run main function with all arguments
main "$@"
