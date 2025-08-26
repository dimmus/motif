#!/bin/sh

# Git Repository Mass Update Script
# Updates all git repositories in a directory with parallel processing
# Handles uncommitted changes with rebase and recursive submodule updates

# Default values
REPOS_DIR="."
MAX_JOBS=4
VERBOSE=0
DRY_RUN=0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print usage information
usage() {
    cat << EOF
Usage: $0 [OPTIONS] [DIRECTORY]

Update all git repositories in the specified directory (default: current directory).

OPTIONS:
    -j, --jobs NUM      Maximum number of parallel jobs (default: 4)
    -v, --verbose       Enable verbose output
    -n, --dry-run       Show what would be done without executing
    -h, --help          Show this help message

DIRECTORY:
    Path to directory containing git repositories (default: current directory)

FEATURES:
    - Parallel processing of multiple repositories
    - Handles uncommitted changes with automatic rebase
    - Recursive submodule updates
    - Detailed progress reporting
    - Error handling and logging

EXAMPLES:
    $0                          # Update repos in current directory
    $0 /path/to/repos           # Update repos in specific directory
    $0 -j 8 -v /path/to/repos   # Use 8 parallel jobs with verbose output
    $0 -n                       # Dry run to see what would be updated
EOF
}

# Logging functions
log_info() {
    printf "${BLUE}[INFO]${NC} %s\n" "$1"
}

log_success() {
    printf "${GREEN}[SUCCESS]${NC} %s\n" "$1"
}

log_warning() {
    printf "${YELLOW}[WARNING]${NC} %s\n" "$1"
}

log_error() {
    printf "${RED}[ERROR]${NC} %s\n" "$1" >&2
}

log_verbose() {
    if [ "$VERBOSE" -eq 1 ]; then
        printf "${BLUE}[VERBOSE]${NC} %s\n" "$1"
    fi
}

# Check if directory is a git repository
is_git_repo() {
    # Check if directory has .git folder (is a repository root)
    [ -d "$1/.git" ]
}

# Get current branch name
get_current_branch() {
    git -C "$1" symbolic-ref --short HEAD 2>/dev/null || git -C "$1" rev-parse --short HEAD 2>/dev/null
}

# Check if repository has uncommitted changes
has_uncommitted_changes() {
    ! git -C "$1" diff-index --quiet HEAD -- 2>/dev/null
}

# Check if repository has untracked files
has_untracked_files() {
    [ -n "$(git -C "$1" ls-files --others --exclude-standard 2>/dev/null)" ]
}

# Get remote tracking branch
get_remote_branch() {
    git -C "$1" rev-parse --abbrev-ref --symbolic-full-name @{u} 2>/dev/null
}

# Update a single repository
update_repo() {
    local repo_path="$1"
    local repo_name
    repo_name=$(basename "$repo_path")
    
    log_info "Processing repository: $repo_name"
    
    # Check if it's actually a git repository
    if ! is_git_repo "$repo_path"; then
        log_warning "$repo_name: Not a git repository, skipping"
        return 1
    fi
    
    # Change to repository directory
    cd "$repo_path" || {
        log_error "$repo_name: Cannot access directory"
        return 1
    }
    
    # Get current branch
    local current_branch
    current_branch=$(get_current_branch "$repo_path")
    log_verbose "$repo_name: Current branch/commit: $current_branch"
    
    # Get remote tracking branch
    local remote_branch
    remote_branch=$(get_remote_branch "$repo_path")
    
    if [ -z "$remote_branch" ]; then
        log_warning "$repo_name: No remote tracking branch found, skipping"
        return 1
    fi
    
    log_verbose "$repo_name: Remote tracking branch: $remote_branch"
    
    # Check for uncommitted changes
    local has_changes=0
    local has_untracked=0
    
    if has_uncommitted_changes "$repo_path"; then
        has_changes=1
        log_verbose "$repo_name: Has uncommitted changes"
    fi
    
    if has_untracked_files "$repo_path"; then
        has_untracked=1
        log_verbose "$repo_name: Has untracked files"
    fi
    
    # Dry run mode
    if [ "$DRY_RUN" -eq 1 ]; then
        if [ "$has_changes" -eq 1 ] || [ "$has_untracked" -eq 1 ]; then
            log_info "$repo_name: [DRY RUN] Would stash changes and rebase"
        else
            log_info "$repo_name: [DRY RUN] Would pull changes"
        fi
        log_info "$repo_name: [DRY RUN] Would update submodules recursively"
        return 0
    fi
    
    # Fetch latest changes
    log_verbose "$repo_name: Fetching from remote..."
    if ! git fetch --all --prune 2>/dev/null; then
        log_error "$repo_name: Failed to fetch from remote"
        return 1
    fi
    
    # Handle uncommitted changes
    local stashed=0
    if [ "$has_changes" -eq 1 ] || [ "$has_untracked" -eq 1 ]; then
        log_verbose "$repo_name: Stashing uncommitted changes..."
        if git stash push -u -m "Auto-stash before update $(date)" >/dev/null 2>&1; then
            stashed=1
            log_verbose "$repo_name: Changes stashed successfully"
        else
            log_error "$repo_name: Failed to stash changes"
            return 1
        fi
    fi
    
    # Check if we're ahead, behind, or diverged
    local local_commit remote_commit base_commit
    local_commit=$(git rev-parse HEAD)
    remote_commit=$(git rev-parse "$remote_branch")
    base_commit=$(git merge-base HEAD "$remote_branch" 2>/dev/null)
    
    if [ "$local_commit" = "$remote_commit" ]; then
        log_verbose "$repo_name: Already up to date"
    elif [ "$local_commit" = "$base_commit" ]; then
        # We're behind, can fast-forward
        log_verbose "$repo_name: Fast-forwarding..."
        if ! git merge --ff-only "$remote_branch" >/dev/null 2>&1; then
            log_error "$repo_name: Fast-forward failed"
            if [ "$stashed" -eq 1 ]; then
                git stash pop >/dev/null 2>&1
            fi
            return 1
        fi
    elif [ "$remote_commit" = "$base_commit" ]; then
        # We're ahead, nothing to do
        log_verbose "$repo_name: Local branch is ahead of remote"
    else
        # We've diverged, need to rebase
        log_verbose "$repo_name: Rebasing onto $remote_branch..."
        if ! git rebase "$remote_branch" >/dev/null 2>&1; then
            log_error "$repo_name: Rebase failed, aborting..."
            git rebase --abort >/dev/null 2>&1
            if [ "$stashed" -eq 1 ]; then
                git stash pop >/dev/null 2>&1
            fi
            return 1
        fi
    fi
    
    # Restore stashed changes
    if [ "$stashed" -eq 1 ]; then
        log_verbose "$repo_name: Restoring stashed changes..."
        if ! git stash pop >/dev/null 2>&1; then
            log_warning "$repo_name: Failed to restore stashed changes (may have conflicts)"
        fi
    fi
    
    # Update submodules recursively
    if [ -f .gitmodules ]; then
        log_verbose "$repo_name: Updating submodules recursively..."
        if ! git submodule update --init --recursive --remote >/dev/null 2>&1; then
            log_warning "$repo_name: Failed to update some submodules"
        else
            log_verbose "$repo_name: Submodules updated successfully"
        fi
    fi
    
    log_success "$repo_name: Updated successfully"
    return 0
}

# Process repositories in parallel
process_repositories() {
    local repos_dir="$1"
    local job_count=0
    local pids=""
    local total_repos=0
    local successful_repos=0
    local failed_repos=0
    
    # Count total repositories
    for dir in "$repos_dir"/*; do
        if [ -d "$dir" ] && is_git_repo "$dir"; then
            total_repos=$((total_repos + 1))
        fi
    done
    
    if [ "$total_repos" -eq 0 ]; then
        log_warning "No git repositories found in $repos_dir"
        return 1
    fi
    
    log_info "Found $total_repos git repositories to update"
    
    # Process each repository
    for dir in "$repos_dir"/*; do
        if [ -d "$dir" ] && is_git_repo "$dir"; then
            # Wait if we've reached max jobs
            while [ "$job_count" -ge "$MAX_JOBS" ]; do
                wait_for_job
                job_count=$((job_count - 1))
            done
            
            # Start update in background
            (
                if update_repo "$dir"; then
                    echo "SUCCESS:$(basename "$dir")"
                else
                    echo "FAILED:$(basename "$dir")"
                fi
            ) &
            
            pids="$pids $!"
            job_count=$((job_count + 1))
        fi
    done
    
    # Wait for remaining jobs
    for pid in $pids; do
        if wait "$pid"; then
            successful_repos=$((successful_repos + 1))
        else
            failed_repos=$((failed_repos + 1))
        fi
    done
    
    # Print summary
    echo
    log_info "Update Summary:"
    log_success "Successfully updated: $successful_repos repositories"
    if [ "$failed_repos" -gt 0 ]; then
        log_error "Failed to update: $failed_repos repositories"
    fi
    
    return "$failed_repos"
}

# Wait for any background job to complete
wait_for_job() {
    wait -n 2>/dev/null || {
        # Fallback for shells that don't support 'wait -n'
        sleep 0.1
    }
}

# Parse command line arguments
parse_args() {
    while [ $# -gt 0 ]; do
        case $1 in
            -j|--jobs)
                if [ -n "$2" ] && [ "$2" -gt 0 ] 2>/dev/null; then
                    MAX_JOBS="$2"
                    shift 2
                else
                    log_error "Invalid number of jobs: $2"
                    exit 1
                fi
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            -n|--dry-run)
                DRY_RUN=1
                shift
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            -*)
                log_error "Unknown option: $1"
                usage
                exit 1
                ;;
            *)
                if [ -d "$1" ]; then
                    REPOS_DIR="$1"
                else
                    log_error "Directory does not exist: $1"
                    exit 1
                fi
                shift
                ;;
        esac
    done
}

# Main function
main() {
    # Parse command line arguments
    parse_args "$@"
    
    # Convert to absolute path
    REPOS_DIR=$(cd "$REPOS_DIR" && pwd)
    
    log_info "Starting git repository updates"
    log_info "Target directory: $REPOS_DIR"
    log_info "Maximum parallel jobs: $MAX_JOBS"
    
    if [ "$DRY_RUN" -eq 1 ]; then
        log_info "DRY RUN MODE - No changes will be made"
    fi
    
    if [ "$VERBOSE" -eq 1 ]; then
        log_info "Verbose mode enabled"
    fi
    
    echo
    
    # Check if git is available
    if ! command -v git >/dev/null 2>&1; then
        log_error "Git is not installed or not in PATH"
        exit 1
    fi
    
    # Process repositories
    if process_repositories "$REPOS_DIR"; then
        log_success "All repositories updated successfully!"
        exit 0
    else
        log_error "Some repositories failed to update"
        exit 1
    fi
}

# Run main function with all arguments
main "$@"