# Git Repository Mass Update Script

A comprehensive POSIX shell script for updating multiple git repositories in parallel with advanced features for handling uncommitted changes and recursive submodule updates.

## Features

- **Parallel Processing**: Updates multiple repositories simultaneously for faster execution
- **Uncommitted Changes Handling**: Automatically stashes uncommitted changes, performs updates, then restores changes
- **Smart Rebase Strategy**: Handles diverged branches by rebasing onto remote tracking branch
- **Recursive Submodule Updates**: Updates all submodules recursively when present
- **Comprehensive Logging**: Color-coded output with verbose mode support
- **Dry Run Mode**: Preview what would be done without making changes
- **Error Handling**: Robust error handling with detailed reporting
- **POSIX Compatible**: Works on all POSIX-compliant shells (bash, dash, zsh, etc.)

## Usage

```bash
./update_git_repos.sh [OPTIONS] [DIRECTORY]
```

### Options

- `-j, --jobs NUM`: Maximum number of parallel jobs (default: 4)
- `-v, --verbose`: Enable verbose output
- `-n, --dry-run`: Show what would be done without executing
- `-h, --help`: Show help message

### Examples

```bash
# Update repos in current directory
./update_git_repos.sh

# Update repos in specific directory
./update_git_repos.sh /path/to/repos

# Use 8 parallel jobs with verbose output
./update_git_repos.sh -j 8 -v /path/to/repos

# Dry run to see what would be updated
./update_git_repos.sh -n
```

## How It Works

### Repository Detection
The script scans the specified directory for subdirectories containing a `.git` folder, identifying them as git repository roots.

### Update Process
For each repository, the script:

1. **Fetches** latest changes from all remotes
2. **Stashes** uncommitted changes (if any) including untracked files
3. **Analyzes** the relationship between local and remote branches:
   - **Up to date**: No action needed
   - **Behind**: Fast-forward merge
   - **Ahead**: No action needed (local changes preserved)
   - **Diverged**: Rebase local commits onto remote branch
4. **Restores** stashed changes after successful update
5. **Updates** submodules recursively (if `.gitmodules` exists)

### Parallel Execution
The script uses background processes to update multiple repositories simultaneously, with configurable job limits to prevent system overload.

## Error Handling

The script handles various error scenarios:

- **No remote tracking branch**: Skips repository with warning
- **Fetch failures**: Reports error and continues with other repos
- **Rebase conflicts**: Aborts rebase and restores stashed changes
- **Stash failures**: Reports error and skips repository
- **Access issues**: Reports permission or path errors

## Output

The script provides color-coded output:
- **Blue [INFO]**: General information
- **Green [SUCCESS]**: Successful operations
- **Yellow [WARNING]**: Non-critical issues
- **Red [ERROR]**: Critical errors

### Verbose Mode
Enable with `-v` flag for detailed operation logging:
- Current branch information
- Remote tracking branch details
- Stash operations
- Rebase/merge operations
- Submodule updates

## Requirements

- **Git**: Version 2.0 or later recommended
- **POSIX Shell**: bash, dash, zsh, or any POSIX-compliant shell
- **Standard Unix Tools**: Basic utilities like `basename`, `cd`, `printf`

## Directory Structure Example

```
/path/to/repos/
├── project1/
│   ├── .git/
│   └── ...
├── project2/
│   ├── .git/
│   ├── .gitmodules
│   └── ...
└── project3/
    ├── .git/
    └── ...
```

## Best Practices

1. **Test First**: Use dry run mode (`-n`) before actual updates
2. **Backup Important Work**: Ensure critical changes are committed or backed up
3. **Check Results**: Review the summary output for any failures
4. **Adjust Parallelism**: Use fewer jobs (`-j`) on resource-constrained systems
5. **Use Verbose Mode**: Enable verbose output (`-v`) for troubleshooting

## Troubleshooting

### Common Issues

**"No remote tracking branch found"**
- Repository doesn't have a remote configured
- Current branch isn't tracking a remote branch
- Solution: Set up remote tracking with `git branch --set-upstream-to=origin/main`

**"Rebase failed"**
- Conflicts between local and remote changes
- Script automatically aborts rebase and restores stashed changes
- Solution: Manually resolve conflicts in the repository

**"Failed to stash changes"**
- Unusual git state or permissions issue
- Solution: Check repository status manually with `git status`

**"Cannot access directory"**
- Permission issues or directory doesn't exist
- Solution: Check directory permissions and existence

### Performance Tuning

- **CPU Cores**: Set jobs (`-j`) to number of CPU cores for optimal performance
- **I/O Bound**: For network-heavy operations, more jobs than cores may help
- **Memory**: Reduce jobs if running out of memory during parallel operations

## Exit Codes

- `0`: All repositories updated successfully
- `1`: Some repositories failed to update or other errors occurred

## Security Considerations

- Script only performs read and standard git operations
- No destructive operations (force push, branch deletion, etc.)
- Stashed changes are always preserved and restored
- Failed operations are safely aborted

## License

This script is provided as-is for educational and practical use. Modify as needed for your specific requirements.