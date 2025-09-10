#!/usr/bin/env python3

"""
XmString Test Data Processor
Processes .dat and .scr files for automated testing
"""

import os
import sys
import re
import json
from pathlib import Path
from typing import Dict, List, Any, Optional

class TestDataProcessor:
    def __init__(self, test_dir: str):
        self.test_dir = Path(test_dir)
        self.test_data = {}
        
    def parse_dat_file(self, dat_file: Path) -> Dict[str, Any]:
        """Parse a .dat file to extract test description and expected behavior"""
        data = {
            'description': '',
            'steps': [],
            'expected': [],
            'continue': False,
            'exit': False
        }
        
        if not dat_file.exists():
            return data
            
        with open(dat_file, 'r') as f:
            content = f.read()
            
        # Extract description (lines starting with @))
        desc_match = re.search(r'@\)\s*(.+?)(?=\n#\)|$)', content, re.DOTALL)
        if desc_match:
            data['description'] = desc_match.group(1).strip()
            
        # Extract test steps (lines starting with #))
        step_matches = re.findall(r'#\)\s*(.+?)(?=\n#\)|$)', content, re.DOTALL)
        for step in step_matches:
            data['steps'].append(step.strip())
            
        # Check for continue/exit flags
        if 'C)' in content:
            data['continue'] = True
        if 'E)' in content:
            data['exit'] = True
            
        return data
        
    def parse_scr_file(self, scr_file: Path) -> Dict[str, Any]:
        """Parse a .scr file to extract automated test script commands"""
        script = {
            'commands': [],
            'description': '',
            'continue': False,
            'exit': False
        }
        
        if not scr_file.exists():
            return script
            
        with open(scr_file, 'r') as f:
            content = f.read()
            
        # Extract description (lines starting with # @))
        desc_match = re.search(r'# @\)\s*(.+?)(?=\n# #\)|$)', content, re.DOTALL)
        if desc_match:
            script['description'] = desc_match.group(1).strip()
            
        # Extract commands (non-comment lines)
        lines = content.split('\n')
        for line in lines:
            line = line.strip()
            if line and not line.startswith('#'):
                script['commands'].append(line)
                
        # Check for continue/exit flags
        if 'Continue' in content:
            script['continue'] = True
        if 'Exit' in content:
            script['exit'] = True
            
        return script
        
    def process_test_files(self) -> Dict[str, Any]:
        """Process all .dat and .scr files in the test directory"""
        test_files = {}
        
        # Find all test programs
        for file_path in self.test_dir.glob('*'):
            if file_path.is_file() and file_path.suffix == '' and file_path.name.isalpha():
                test_name = file_path.name
                if test_name.startswith(('FontList', 'String', 'RendTo')):
                    test_files[test_name] = {
                        'executable': file_path,
                        'dat_file': self.test_dir / f"{test_name}.dat",
                        'scr_file': self.test_dir / f"{test_name}.scr",
                        'data': self.parse_dat_file(self.test_dir / f"{test_name}.dat"),
                        'script': self.parse_scr_file(self.test_dir / f"{test_name}.scr")
                    }
                    
        return test_files
        
    def generate_test_report(self, test_files: Dict[str, Any]) -> str:
        """Generate a comprehensive test report"""
        report = []
        report.append("# XmString Test Suite Report")
        report.append("=" * 50)
        report.append("")
        
        for test_name, test_info in test_files.items():
            report.append(f"## {test_name}")
            report.append("")
            
            # Test description
            if test_info['data']['description']:
                report.append("**Description:**")
                report.append(test_info['data']['description'])
                report.append("")
                
            # Test steps
            if test_info['data']['steps']:
                report.append("**Test Steps:**")
                for i, step in enumerate(test_info['data']['steps'], 1):
                    report.append(f"{i}. {step}")
                report.append("")
                
            # Script commands
            if test_info['script']['commands']:
                report.append("**Automated Commands:**")
                for cmd in test_info['script']['commands']:
                    report.append(f"- `{cmd}`")
                report.append("")
                
            # File status
            report.append("**File Status:**")
            report.append(f"- Executable: {'✓' if test_info['executable'].exists() else '✗'}")
            report.append(f"- Data file: {'✓' if test_info['dat_file'].exists() else '✗'}")
            report.append(f"- Script file: {'✓' if test_info['scr_file'].exists() else '✗'}")
            report.append("")
            
        return '\n'.join(report)
        
    def generate_automation_script(self, test_files: Dict[str, Any]) -> str:
        """Generate a shell script for automated test execution"""
        script = []
        script.append("#!/bin/bash")
        script.append("# Generated automation script for XmString tests")
        script.append("")
        script.append("set -euo pipefail")
        script.append("")
        script.append("SCRIPT_DIR=\"$(cd \"$(dirname \"${BASH_SOURCE[0]}\")\" && pwd)\"")
        script.append("")
        script.append("echo \"Starting automated XmString tests...\"")
        script.append("")
        
        for test_name, test_info in test_files.items():
            if test_info['executable'].exists():
                script.append(f"echo \"Running {test_name}...\"")
                script.append(f"if [ -x \"$SCRIPT_DIR/{test_name}\" ]; then")
                script.append(f"    \"$SCRIPT_DIR/{test_name}\"")
                script.append(f"    echo \"{test_name} completed\"")
                script.append("else")
                script.append(f"    echo \"ERROR: {test_name} not executable\"")
                script.append("fi")
                script.append("")
            else:
                script.append(f"echo \"SKIP: {test_name} not found\"")
                script.append("")
                
        script.append("echo \"All tests completed\"")
        
        return '\n'.join(script)
        
    def export_json(self, test_files: Dict[str, Any], output_file: str):
        """Export test data to JSON format"""
        json_data = {}
        for test_name, test_info in test_files.items():
            json_data[test_name] = {
                'executable': str(test_info['executable']),
                'dat_file': str(test_info['dat_file']),
                'scr_file': str(test_info['scr_file']),
                'data': test_info['data'],
                'script': test_info['script']
            }
            
        with open(output_file, 'w') as f:
            json.dump(json_data, f, indent=2)

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 process_test_data.py <test_directory>")
        sys.exit(1)
        
    test_dir = sys.argv[1]
    processor = TestDataProcessor(test_dir)
    
    print(f"Processing test files in: {test_dir}")
    
    # Process all test files
    test_files = processor.process_test_files()
    
    print(f"Found {len(test_files)} test programs")
    
    # Generate report
    report = processor.generate_test_report(test_files)
    with open(os.path.join(test_dir, 'test_report.md'), 'w') as f:
        f.write(report)
    print("Generated test_report.md")
    
    # Generate automation script
    automation_script = processor.generate_automation_script(test_files)
    with open(os.path.join(test_dir, 'automated_tests.sh'), 'w') as f:
        f.write(automation_script)
    os.chmod(os.path.join(test_dir, 'automated_tests.sh'), 0o755)
    print("Generated automated_tests.sh")
    
    # Export JSON data
    processor.export_json(test_files, os.path.join(test_dir, 'test_data.json'))
    print("Generated test_data.json")
    
    print("\nTest data processing complete!")

if __name__ == "__main__":
    main()
