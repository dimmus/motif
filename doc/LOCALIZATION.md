# Localization Testing Guide for xde-motif

This document provides comprehensive guidance on how to test that localization works correctly in the xde-motif project.

## Overview

The xde-motif project supports internationalization (i18n) and localization (l10n) through several mechanisms:

- **UIL-based localization**: Different UIL files for different languages
- **Environment-based locale switching**: Using `LANG` and `XAPPLRESDIR` variables
- **X11 locale support**: Integration with X11's locale system
- **Font support**: Character set support for different languages

## Supported Languages

The project includes localization support for:

- **English** (default/C locale)
- **French** (`fr_FR.ISO8859-1`)
- **German** (`de_DE.ISO8859-1`)
- **Hebrew** (`hebrew`) (in examples)
- **Japanese** (`ja_JP.eucJP`, `ja_JP.dt-eucJP`)
- **Swedish** (`sv_SE.ISO8859-1`)
- **Spanish** (`es_ES.ISO8859-1`)  (in examples)
- **Italian** (`it_IT.ISO8859-1`)

## Testing Methods

### 1. Using Built-in Demo Programs

#### A. Hello World Internationalization Demo (`helloint`)

The simplest way to test localization:

```bash
# Navigate to the example directory
cd examples/programs/hellomotifi18n

# Test different languages
# English (default)
./helloint

# French
LANG=fr_FR.ISO8859-1 XAPPLRESDIR=./french ./helloint

# Hebrew  
LANG=hebrew XAPPLRESDIR=./hebrew ./helloint

# Japanese
LANG=ja_JP.eucJP XAPPLRESDIR=./japanese ./helloint

# Swedish
LANG=sv_SE.ISO8859-1 XAPPLRESDIR=./swedish ./helloint
```

**What to verify:**
- Button labels appear in target language
- Text displays correctly with proper character encoding
- Fonts support the required character set

#### B. File Viewer Demo (`fileview`)

A more complex application with full localization:

```bash
cd examples/programs/fileview

# Use the provided script for different languages
./xmfile french    # French interface
./xmfile english   # English interface  
./xmfile german    # German interface
```

**What to verify:**
- Menu items are translated
- Dialog boxes show localized text
- File operations work with localized text
- Error messages appear in target language

### 2. Using Built-in Test Scripts

#### A. I18N Test Script

The project includes a comprehensive test script in `.attic/tests/I18N/`:

```bash
cd .attic/tests/I18N

# Test text display with different locales
./i18n1 american a    # American locale, test case a
./i18n1 french a      # French locale, test case a
./i18n1 hebrew a      # Hebrew locale, test case a
./i18n1 japanese a    # Japanese locale, test case a

# Test text conversion (test case b)
./i18n1 american b
./i18n1 french b
```

**Test cases:**
- **Case a**: Basic text display testing
- **Case b**: Text conversion and wide character handling

#### B. Input Method Testing

For testing input methods and complex script handling:

```bash
# Test shared input context
./InputMethod1 -u a    # OVER_THE_SPOT Input Method
./InputMethod1 -u b    # OFF_THE_SPOT Input Method
./InputMethod1 -u c    # ROOT Input Method

# Test per-widget input contexts
./InputMethod2 -u a
./InputMethod2 -u b
./InputMethod2 -u c
```

### 3. Manual Testing Steps

#### A. Environment Setup

```bash
# Set locale environment variables
export LANG=fr_FR.ISO8859-1
export LC_ALL=fr_FR.ISO8859-1
export XAPPLRESDIR=/path/to/your/app/resources

# Verify locale support
locale -a | grep fr_FR
```

#### B. Font Verification

Check that appropriate fonts are available:

```bash
# List available fonts
xlsfonts | grep -i helvetica
xlsfonts | grep -i times
xlsfonts | grep -i iso8859

# Check font availability with fontconfig
fc-list | grep -i helvetica
fc-list | grep -i iso8859
```

#### C. X11 Locale Support

```bash
# Check X11 locale support
xdpyinfo | grep -i locale
xlsfonts | grep -i locale
```

### 4. Automated Testing Script

Create this script to automate localization testing:

```bash
#!/bin/bash
# localization_test.sh

echo "Testing xde-motif Localization..."

# Test locales
LOCALES=("en_US.UTF-8" "fr_FR.ISO8859-1" "de_DE.ISO8859-1" "ja_JP.eucJP")

for locale in "${LOCALES[@]}"; do
    echo "Testing locale: $locale"
    
    # Set environment
    export LANG=$locale
    export LC_ALL=$locale
    
    # Test helloint example
    cd examples/programs/hellomotifi18n
    timeout 10s ./helloint &
    sleep 2
    pkill helloint
    
    # Test fileview demo  
    cd ../fileview
    timeout 10s ./fileview -ok &
    sleep 2
    pkill fileview
    
    echo "Completed test for $locale"
    echo "---"
done

echo "Localization testing complete!"
```

## What to Look For When Testing

### Text Display
- **Correct Language**: Verify that text appears in the target language
- **Character Encoding**: Check that special characters (accents, umlauts, etc.) display correctly
- **Font Rendering**: Ensure fonts support the required character set
- **Text Direction**: For RTL languages like Hebrew, verify text flows right-to-left

### User Interface Elements
- **Menu Items**: All menu text should be translated
- **Button Labels**: Button text should appear in target language
- **Dialog Boxes**: All dialog text should be localized
- **Error Messages**: System messages should appear in target language

### Input Handling
- **Keyboard Input**: Test typing in the target language
- **Input Methods**: For complex scripts (Japanese, Chinese), test input methods
- **Character Conversion**: Verify text conversion between different character sets

## Troubleshooting Common Issues

### Locale Not Supported

```bash
# Check available locales
locale -a

# Install additional locales (Ubuntu/Debian)
sudo locale-gen fr_FR.ISO8859-1
sudo dpkg-reconfigure locales

# Install additional locales (Red Hat/Fedora)
sudo localedef -i fr_FR -f ISO8859-1 fr_FR.ISO8859-1
```

### Font Issues

```bash
# Install additional fonts
sudo apt-get install fonts-dejavu fonts-liberation

# Check font availability
fc-list | grep -i helvetica
fc-list | grep -i iso8859
```

### X11 Locale Issues

```bash
# Check X11 locale support
xlsfonts | grep -i locale
xdpyinfo | grep -i locale

# Restart X server if needed
sudo systemctl restart display-manager
```

### UIL Compilation Issues

```bash
# Recompile UIL files for different locales
cd examples/programs/hellomotifi18n
make clean
make

# Check UIL compilation with specific locale
LANG=fr_FR.ISO8859-1 uil -o test.uid test.uil
```

## Verification Checklist

- [ ] Text displays in correct language
- [ ] Special characters render properly
- [ ] Fonts support required character sets
- [ ] Menu items are translated
- [ ] Button labels are localized
- [ ] Dialog boxes show translated text
- [ ] Input methods work for complex scripts
- [ ] Text direction is correct for RTL languages
- [ ] Error messages appear in target language
- [ ] Application doesn't crash with different locales
- [ ] UIL files compile correctly for each locale
- [ ] Resource files load properly for each language

## File Structure

The localization files are organized as follows:

```
examples/programs/hellomotifi18n/
├── C/                    # Default locale (English)
├── english/              # English localization
├── french/               # French localization
├── hebrew/               # Hebrew localization
├── japanese/             # Japanese localization
└── swedish/              # Swedish localization

examples/programs/fileview/
├── English.uil           # English UIL file
├── French.uil            # French UIL file
├── German.uil            # German UIL file
└── xmfile                # Localization test script

localized/
├── C/                    # Default locale resources
├── fr_FR.ISO8859-1/      # French resources
├── de_DE.ISO8859-1/      # German resources
└── ja_JP.dt-eucJP/       # Japanese resources
```

## Environment Variables

Key environment variables for localization testing:

- `LANG`: Primary locale setting
- `LC_ALL`: Override for all locale categories
- `XAPPLRESDIR`: Directory for application resources
- `XFILESEARCHPATH`: X11 file search path
- `XNLSPATH`: X11 NLS (National Language Support) path

## Additional Resources

- [X11 Internationalization](https://www.x.org/releases/X11R7.7/doc/libX11/i18n.html)
- [Motif Internationalization](https://www.opengroup.org/motif/)
- [UIL Language Reference](https://www.opengroup.org/motif/uil.html)

## Contributing

When adding new localizations:

1. Create appropriate UIL files for the new language
2. Add locale-specific resource files
3. Update the test scripts to include the new locale
4. Verify font support for the target character set
5. Test input methods if applicable
6. Update this documentation

---

For questions or issues with localization testing, please refer to the project's issue tracker or documentation.
