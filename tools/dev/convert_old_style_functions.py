#!/usr/bin/env python3
"""
Script to convert old-style C function definitions to modern C syntax.

This script finds and converts function definitions like:
    int main(argc, argv)
         int    argc;
         String argv[];

Or:
    static Boolean fstep(client_data)
    /************************/
         XtPointer client_data ;       /* scalespeed */

To:
    int main(int argc, String argv[])
    static Boolean fstep(XtPointer client_data)

USAGE EXAMPLES:
    # Dry run on a single file (shows what would be changed)
    python3 convert_old_style_functions.py file.c
    
    # Apply changes to a single file
    python3 convert_old_style_functions.py file.c --apply
    
    # Process all C files in a directory (dry run)
    python3 convert_old_style_functions.py /path/to/directory/
    
    # Process all C files in a directory and apply changes
    python3 convert_old_style_functions.py /path/to/directory/ --apply
    
    # Process all C files in current directory recursively
    python3 convert_old_style_functions.py . --apply
    
    # Process specific file types
    python3 convert_old_style_functions.py . --apply --extensions .c,.h
"""

import re
import sys
import os
import argparse
from typing import List, Tuple, Optional

def parse_old_style_function(content: str, start_line: int) -> Optional[Tuple[str, str, List[str], int]]:
    """
    Parse an old-style function definition starting at the given line.
    
    Returns:
        Tuple of (function_name, return_type, parameters, end_line) or None if not found
    """
    lines = content.split('\n')
    
    # Look for function definition pattern
    # Pattern: return_type function_name(param1, param2, ...)
    # Simple pattern that matches most C function declarations
    
    if start_line >= len(lines):
        return None
        
    line = lines[start_line].strip()
    
    func_pattern = r'^(\w+(?:\s+\w+)*)\s+(\w+)\s*\(([^)]*)\)\s*$'
    match = re.match(func_pattern, line)
    
    if not match:
        return None
        
    return_type = match.group(1).strip()
    function_name = match.group(2).strip()
    param_list = match.group(3).strip()
    
    # Skip if this looks like a conditional statement (else if, if, while, for, etc.)
    if function_name in ['if', 'else', 'while', 'for', 'switch']:
        return None
    
    # Skip if the line starts with control flow keywords
    if line.strip().startswith(('else if', 'if (', 'while (', 'for (', 'switch (')):
        return None
    
    # If the parameter list contains type information (like "int argc, char *argv[]"),
    # this is already a modern C function, so skip it
    # Look for common C type patterns and custom type names (starting with capital letters)
    if (re.search(r'\b(int|char|void|float|double|long|short|unsigned|signed|const|static|extern|auto|register|volatile|restrict|_Bool|_Complex|_Imaginary|Widget|XtPointer|XmString|Boolean|String|XtAppContext|FILE|ViewPtr|MrmCode|XmString|Arg|Cardinal|Dimension|Position|Pixel|Boolean|XtCallbackProc|XmFileSelectionBoxCallbackStruct|MrmCode)\b', param_list) or
        re.search(r'\b[A-Z][a-zA-Z0-9_]*\s+[a-zA-Z_][a-zA-Z0-9_]*\b', param_list) or
        re.search(r'\b[A-Z][a-zA-Z0-9_]*\s*\*\s*[a-zA-Z_][a-zA-Z0-9_]*\b', param_list) or
        param_list.strip() == ''):  # Functions with no parameters are modern C
        return None
    
    # This is an old-style function - extract just the parameter names
    param_names = [p.strip() for p in param_list.split(',') if p.strip()]
    
    # Now look for parameter type declarations on subsequent lines
    parameters = []
    current_line = start_line + 1
    
    while current_line < len(lines):
        line = lines[current_line].strip()
        
        # Skip empty lines, comments, and separator lines like /********/
        if (not line or line.startswith('/*') or line.startswith('//') or 
            re.match(r'^/+$', line) or re.match(r'^/+.*/+$', line)):
            current_line += 1
            continue
            
        # Stop if we hit the opening brace of the function body
        if line == '{':
            break
            
        # More robust parsing that handles various C type declarations
        # Split the line by semicolon and process each parameter declaration
        if ';' in line:
            # Split by semicolon to handle multiple declarations on one line
            declarations = line.split(';')
            for declaration in declarations:
                declaration = declaration.strip()
                if not declaration:  # Skip empty parts
                    continue
                    
                # Remove any inline comment
                if '/*' in declaration:
                    declaration = declaration.split('/*')[0].strip()
                
                # Split by whitespace to separate type and name
                parts = declaration.split()
                if len(parts) >= 2:
                    # Check if this declaration has multiple parameters (e.g., "Pixel fg, bg")
                    if ',' in declaration:
                        # Split by comma to get individual parameters
                        param_declarations = declaration.split(',')
                        # Extract the base type from the original declaration
                        # For "Pixel fg, bg", the type is "Pixel" and names are "fg" and "bg"
                        # The first part "Pixel fg" contains the type, so we need to extract just "Pixel"
                        first_param = param_declarations[0].strip()
                        first_parts = first_param.split()
                        if len(first_parts) >= 2:
                            base_type = ' '.join(first_parts[:-1])  # Everything except the last part (parameter name)
                        else:
                            base_type = ' '.join(parts[:-1])  # Fallback to original logic
                        
                        for param_decl in param_declarations:
                            param_decl = param_decl.strip()
                            if not param_decl:
                                continue
                                
                            # Extract the parameter name (last part after stripping)
                            param_parts = param_decl.split()
                            if param_parts:
                                param_name = param_parts[-1]
                                # Remove * from the beginning of the parameter name if present
                                if param_name.startswith('*'):
                                    param_name = param_name[1:]
                                # Remove [] from the end of the parameter name if present
                                if param_name.endswith('[]'):
                                    param_name = param_name[:-2]
                                
                                # Check if this parameter name matches one we're looking for
                                base_param_name = param_name.lstrip('*')
                                if base_param_name in param_names:
                                    # Reconstruct the full type including the * if it was present
                                    full_type = base_type
                                    # Count the * characters in the original declaration
                                    star_count = param_parts[-1].count('*')
                                    if star_count > 0:
                                        full_type += ' ' + '*' * star_count
                                    # Add [] back if it was present - but after the variable name, not after the type
                                    if param_parts[-1].endswith('[]'):
                                        parameters.append(f"{full_type} {base_param_name}[]")
                                    else:
                                        parameters.append(f"{full_type} {base_param_name}")
                    else:
                        # Single parameter declaration (original logic)
                        # The last part is the parameter name (might include * for pointers)
                        param_name = parts[-1]
                        # Remove * from the beginning of the parameter name if present
                        if param_name.startswith('*'):
                            param_name = param_name[1:]
                        # Remove [] from the end of the parameter name if present
                        if param_name.endswith('[]'):
                            param_name = param_name[:-2]
                        # Everything before the last part is the type
                        param_type = ' '.join(parts[:-1])
                        
                        # Check if this parameter name matches one we're looking for
                        # Handle cases where the parameter name might have multiple * characters
                        base_param_name = param_name.lstrip('*')
                        if base_param_name in param_names:
                            # Reconstruct the full type including the * if it was present
                            full_type = param_type
                            # Count the * characters in the original declaration
                            star_count = parts[-1].count('*')
                            if star_count > 0:
                                full_type += ' ' + '*' * star_count
                            # Add [] back if it was present - but after the variable name, not after the type
                            if parts[-1].endswith('[]'):
                                parameters.append(f"{full_type} {base_param_name}[]")
                            else:
                                parameters.append(f"{full_type} {base_param_name}")
                        else:
                            pass
            
            current_line += 1
            continue
        else:
            current_line += 1
            continue
    
    # Check if we have the right number of parameters
    if len(parameters) != len(param_names):
        print(f"Warning: Parameter count mismatch for {function_name}: expected {len(param_names)}, got {len(parameters)}")
        print(f"  Param names: {param_names}")
        print(f"  Parameters: {parameters}")
        print(f"  Function line: {lines[start_line]}")
        print(f"  Parameter lines: {lines[start_line+1:current_line]}")
        
        # Try to match parameters by name instead of just count
        matched_parameters = []
        for param_name in param_names:
            # Find the parameter with this name
            for param in parameters:
                if param.endswith(f" {param_name}"):
                    matched_parameters.append(param)
                    break
            else:
                # If we can't find a match, use the original parameter name
                print(f"  Warning: Could not find type for parameter '{param_name}'")
                matched_parameters.append(f"int {param_name}")
        
        if len(matched_parameters) == len(param_names):
            print(f"  -> Recovered parameters by name matching: {matched_parameters}")
            parameters = matched_parameters
        else:
            print(f"  -> Failed to recover parameters, skipping function")
            return None
    
    return function_name, return_type, parameters, current_line

def convert_function_definition(content: str, start_line: int, function_name: str, 
                              return_type: str, parameters: List[str], end_line: int) -> str:
    """
    Convert the old-style function definition to modern C syntax.
    """
    lines = content.split('\n')
    
    # Create the new function signature
    param_str = ', '.join(parameters)
    new_signature = f"{return_type} {function_name}({param_str})"
    
    # Replace the old definition lines
    new_lines = lines[:start_line]
    new_lines.append(new_signature)
    new_lines.extend(lines[end_line:])
    
    return '\n'.join(new_lines)

def process_file(filepath: str, dry_run: bool = True) -> bool:
    """
    Process a single C file to convert old-style function definitions.
    
    Args:
        filepath: Path to the C file
        dry_run: If True, only show what would be changed without modifying the file
        
    Returns:
        True if changes were made or would be made, False otherwise
    """
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False
    
    original_content = content
    changes_made = False
    line_offset = 0
    
    # Find all old-style function definitions
    current_line = 0
    functions_found = 0
    while current_line < len(content.split('\n')):
        result = parse_old_style_function(content, current_line)
        
        if result is None:
            current_line += 1
            continue
            
        function_name, return_type, parameters, end_line = result
        functions_found += 1
        
        print(f"Found old-style function: {function_name}")
        print(f"  Return type: {return_type}")
        print(f"  Parameters: {parameters}")
        print(f"  Lines: {current_line + 1}-{end_line + 1}")
        
        if not dry_run:
            # Convert the function
            content = convert_function_definition(content, current_line, function_name, 
                                               return_type, parameters, end_line)
            changes_made = True
            print(f"  -> Converted to: {return_type} {function_name}({', '.join(parameters)})")
        else:
            print(f"  -> Would convert to: {return_type} {function_name}({', '.join(parameters)})")
        
        # Move to next line after this function
        current_line = end_line + 1
    
    if functions_found > 0:
        print(f"\nTotal functions found: {functions_found}")
        if dry_run:
            print("Run with --apply to convert these functions to modern C syntax")
    else:
        print("No old-style functions found in this file")
    
    if changes_made and not dry_run:
        # Write the modified content back to the file
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"Updated {filepath}")
            return True
        except Exception as e:
            print(f"Error writing {filepath}: {e}")
            return False
    
    return changes_made

def main():
    """Main function to run the conversion script."""
    parser = argparse.ArgumentParser(description="Convert old-style C function definitions to modern C syntax.")
    parser.add_argument("target", help="Path to a file or directory to process.")
    parser.add_argument("--apply", action="store_true", help="Actually apply the changes (default is dry-run).")
    parser.add_argument("--extensions", default=".c,.h", help="Comma-separated list of file extensions to process (e.g., .c,.h).")
    
    args = parser.parse_args()
    
    dry_run = not args.apply
    
    if dry_run:
        print("DRY RUN MODE - No files will be modified")
        print("Use --apply to actually make changes")
        print()
    
    if os.path.isfile(args.target):
        # Process single file
        if args.target.endswith(tuple(args.extensions.split(','))):
            print(f"Processing file: {args.target}")
            process_file(args.target, dry_run)
        else:
            print(f"Skipping non-C file: {args.target}")
    
    elif os.path.isdir(args.target):
        # Process directory
        print(f"Processing directory: {args.target}")
        for root, dirs, files in os.walk(args.target):
            for file in files:
                if file.endswith(tuple(args.extensions.split(','))):
                    filepath = os.path.join(root, file)
                    print(f"\nProcessing: {filepath}")
                    process_file(filepath, dry_run)
    
    else:
        print(f"Target not found: {args.target}")
        sys.exit(1)
    
    if dry_run:
        print("\nDry run completed. Use --apply to make actual changes.")

if __name__ == "__main__":
    main()
