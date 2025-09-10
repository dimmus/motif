#!/bin/bash

# Motif Test Environment Revival Script
# This script revives the complete test setup for graphical display environment
# Based on analysis of .attic/tests and current tests directories

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_HOME="$SCRIPT_DIR"
ATTIC_TESTS="$TESTS_HOME/.attic/tests"
CURRENT_TESTS="$TESTS_HOME/tests"

echo "=========================================="
echo "Motif Test Environment Revival Script"
echo "=========================================="
echo "TESTS_HOME: $TESTS_HOME"
echo "ATTIC_TESTS: $ATTIC_TESTS"
echo "CURRENT_TESTS: $CURRENT_TESTS"
echo ""

# Check if we're in a graphical environment
if [ -z "${DISPLAY:-}" ]; then
    echo "WARNING: No DISPLAY environment variable set."
    echo "This script is designed for graphical test environments."
    echo "Please ensure you have X11 forwarding or are running on a desktop."
    echo ""
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Function to check if imake is available
check_imake() {
    if ! command -v imake >/dev/null 2>&1; then
        echo "ERROR: imake is not available. Please install xutils-dev or xorg-dev"
        echo "On Ubuntu/Debian: sudo apt-get install xutils-dev"
        echo "On CentOS/RHEL: sudo yum install xorg-x11-util-macros"
        exit 1
    fi
}

# Function to check if X11 libraries are available
check_x11_libs() {
    echo "Checking X11 libraries..."
    
    # Check for X11 headers
    if [ ! -d "/usr/include/X11" ] && [ ! -d "/usr/local/include/X11" ]; then
        echo "WARNING: X11 headers not found in standard locations"
        echo "Please install libx11-dev or xorg-dev"
    fi
    
    # Check for Motif libraries
    if [ ! -f "/usr/lib/libXm.so" ] && [ ! -f "/usr/local/lib/libXm.so" ]; then
        echo "WARNING: Motif library not found in standard locations"
        echo "Please ensure Motif is properly installed"
    fi
}

# Function to setup environment variables
setup_environment() {
    echo "Setting up environment variables..."
    
    export MOTIF_HOME="$TESTS_HOME"
    export LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:${LIBRARY_PATH:-}"
    export LD_LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:${LD_LIBRARY_PATH:-}"
    export C_INCLUDE_PATH="$TESTS_HOME/include:$TESTS_HOME/lib:${C_INCLUDE_PATH:-}"
    export C_PATH="$TESTS_HOME/lib:${C_PATH:-}"
    
    echo "Environment variables set:"
    echo "  MOTIF_HOME=$MOTIF_HOME"
    echo "  LIBRARY_PATH=$LIBRARY_PATH"
    echo "  LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
    echo ""
}

# Function to build Motif libraries
build_motif_libs() {
    echo "Building Motif libraries..."
    cd "$TESTS_HOME/lib"
    
    if [ -f "Makefile" ]; then
        echo "Building libXm..."
        make clean || true
        make -j$(nproc) || {
            echo "ERROR: Failed to build libXm"
            echo "This is required for the test environment"
            exit 1
        }
    else
        echo "ERROR: No Makefile found in lib directory"
        echo "Please run configure first: ./configure"
        exit 1
    fi
    
    echo "Motif libraries built successfully"
    echo ""
}

# Function to create enhanced test library
create_enhanced_testlib() {
    echo "Creating enhanced test library..."
    
    # Create a comprehensive testlib implementation
    cat > "$CURRENT_TESTS/XmString/testlib_enhanced.c" << 'EOF'
/*
 * Enhanced testlib implementation for Motif tests
 * Provides comprehensive test framework functionality
 */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/Separator.h>
#include <Xm/ArrowB.h>
#include <Xm/CascadeB.h>
#include <Xm/Command.h>
#include <Xm/FileSB.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/ScrolledW.h>
#include <Xm/SpinB.h>
#include <Xm/TabStack.h>
#include <Xm/Notebook.h>
#include <Xm/Container.h>
#include <Xm/ComboBox.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/* Global variables */
XtAppContext app_context;
Display *display;
Window rootWindow;
Screen *screen;
Widget Shell1;
Widget InstructionBox;
Widget UTMShell;
Widget UTMDrawingArea;

/* Command line options */
Boolean instructions = False;
char *UserData = NULL;
char *instruct_file_name = NULL;
char *instruct_file_name_orig = NULL;
int pause_len = 0;
char test_font[64] = "fixed";
char default_font[64] = "fixed";

/* Automation variables */
Boolean Automation = False;
Boolean AutoRecord = False;
Boolean AutoBatch = False;
Boolean AutoManual = False;
Boolean AutoNoCheck = False;
Boolean AutoTrace = False;
Boolean AutoDelay = False;
Boolean AutoWindowDumps = False;
Boolean Command_In_Progress = False;

/* Enhanced CommonTestInit implementation */
void CommonTestInit(int argc, char **argv)
{
    Widget toplevel;
    Arg args[10];
    int n;
    char *display_name = NULL;

    printf("Initializing Motif test environment...\n");
    
    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    
    /* Try to open display */
    display = XtOpenDisplay(app_context, display_name, argv[0], "MotifTest",
                           NULL, 0, &argc, argv);
    if (display == NULL) {
        fprintf(stderr, "ERROR: Cannot open display\n");
        fprintf(stderr, "Please ensure you have X11 running and DISPLAY is set\n");
        fprintf(stderr, "Try: export DISPLAY=:0.0\n");
        exit(1);
    }
    
    screen = DefaultScreenOfDisplay(display);
    rootWindow = RootWindowOfScreen(screen);
    
    printf("Display opened successfully: %s\n", DisplayString(display));
    printf("Screen: %dx%d, depth: %d\n", 
           WidthOfScreen(screen), HeightOfScreen(screen), 
           DefaultDepthOfScreen(screen));
    
    /* Create main application shell */
    n = 0;
    XtSetArg(args[n], XmNtitle, "Motif Test Environment"); n++;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    toplevel = XtAppCreateShell(argv[0], "MotifTest",
                               applicationShellWidgetClass, display, args, n);
    Shell1 = toplevel;
    
    /* Create instruction box */
    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 10); n++;
    XtSetArg(args[n], XmNwidth, 400); n++;
    XtSetArg(args[n], XmNheight, 300); n++;
    InstructionBox = XtCreateWidget("instructionBox", 
                                   xmBulletinBoardWidgetClass, toplevel, args, n);
    
    printf("Test environment initialized successfully\n");
}

/* Widget creation functions */
Widget BulletinBoardCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateBulletinBoard(parent, name, args, nargs);
}

Widget DrawingAreaCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateDrawingArea(parent, name, args, nargs);
}

Widget LabelCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateLabel(parent, name, args, nargs);
}

Widget PushButtonCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreatePushButton(parent, name, args, nargs);
}

Widget RowColumnCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateRowColumn(parent, name, args, nargs);
}

Widget FormCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateForm(parent, name, args, nargs);
}

Widget FrameCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateFrame(parent, name, args, nargs);
}

Widget ListCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateList(parent, name, args, nargs);
}

Widget TextCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateText(parent, name, args, nargs);
}

Widget TextFieldCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateTextField(parent, name, args, nargs);
}

Widget ToggleButtonCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateToggleButton(parent, name, args, nargs);
}

Widget ScaleCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateScale(parent, name, args, nargs);
}

Widget ScrollBarCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateScrollBar(parent, name, args, nargs);
}

Widget SeparatorCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateSeparator(parent, name, args, nargs);
}

Widget ArrowButtonCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateArrowButton(parent, name, args, nargs);
}

Widget CascadeButtonCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateCascadeButton(parent, name, args, nargs);
}

Widget CommandCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateCommand(parent, name, args, nargs);
}

Widget FileSelectionBoxCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateFileSelectionBox(parent, name, args, nargs);
}

Widget MainWindowCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateMainWindow(parent, name, args, nargs);
}

Widget MessageBoxCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateMessageBox(parent, name, args, nargs);
}

Widget SelectionBoxCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateSelectionBox(parent, name, args, nargs);
}

Widget ScrolledWindowCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateScrolledWindow(parent, name, args, nargs);
}

Widget SpinBoxCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateSpinBox(parent, name, args, nargs);
}

Widget TabStackCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateTabStack(parent, name, args, nargs);
}

Widget NotebookCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateNotebook(parent, name, args, nargs);
}

Widget ContainerCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateContainer(parent, name, args, nargs);
}

Widget ComboBoxCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateComboBox(parent, name, args, nargs);
}

/* Font and color functions */
XmFontList CommonGetFontList(char *fontstr)
{
    XFontStruct *font;
    XmFontList fontlist;
    
    if (fontstr == NULL || strlen(fontstr) == 0) {
        fontstr = "fixed";
    }
    
    font = XLoadQueryFont(display, fontstr);
    if (font == NULL) {
        printf("Warning: Could not load font '%s', using default\n", fontstr);
        font = XLoadQueryFont(display, "fixed");
        if (font == NULL) {
            printf("Error: Could not load any font\n");
            return NULL;
        }
    }
    
    fontlist = XmFontListCreate(font, XmSTRING_DEFAULT_CHARSET);
    return fontlist;
}

Pixel CommonGetColor(char *colorstr)
{
    XColor color, exact;
    Status status;
    
    if (colorstr == NULL || strlen(colorstr) == 0) {
        return BlackPixelOfScreen(screen);
    }
    
    status = XAllocNamedColor(display, DefaultColormapOfScreen(screen), 
                             colorstr, &color, &exact);
    if (status == 0) {
        printf("Warning: Could not allocate color '%s', using black\n", colorstr);
        return BlackPixelOfScreen(screen);
    }
    
    return color.pixel;
}

/* Utility functions */
void CommonPause()
{
    printf("Pausing... (press Enter to continue)\n");
    getchar();
}

void CommonUsage(char *test_name)
{
    printf("Usage: %s [options]\n", test_name);
    printf("Options:\n");
    printf("  -display <display>  Specify X11 display\n");
    printf("  -help              Show this help\n");
    printf("  -instructions      Show instructions\n");
    printf("  -pause <seconds>   Pause duration\n");
}

void CommonGetOptions(int *argc, char **argv)
{
    int i, j;
    
    for (i = 1; i < *argc; i++) {
        if (strcmp(argv[i], "-instructions") == 0) {
            instructions = True;
            for (j = i; j < *argc - 1; j++) {
                argv[j] = argv[j + 1];
            }
            (*argc)--;
            i--;
        } else if (strcmp(argv[i], "-pause") == 0 && i + 1 < *argc) {
            pause_len = atoi(argv[i + 1]);
            for (j = i; j < *argc - 2; j++) {
                argv[j] = argv[j + 2];
            }
            (*argc) -= 2;
            i--;
        } else if (strcmp(argv[i], "-help") == 0) {
            CommonUsage(argv[0]);
            exit(0);
        }
    }
}

void CommonGenericCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    printf("Generic callback called\n");
}

void CommonDumpHierarchy(Widget w, FILE *file)
{
    fprintf(file, "Widget hierarchy dump not implemented\n");
}

char *CommonCsToRs(XmString cs)
{
    char *str;
    if (XmStringGetLtoR(cs, XmSTRING_DEFAULT_CHARSET, &str)) {
        return str;
    }
    return NULL;
}

void CommonExtraResources(Arg args[], Cardinal n)
{
    /* No extra resources by default */
}

void MallocInit()
{
    /* Memory initialization - no-op for now */
}

void MallocExit()
{
    /* Memory cleanup - no-op for now */
}

void AppMainLoop(XtAppContext app)
{
    printf("Starting main event loop...\n");
    printf("Close the window or press Ctrl+C to exit\n");
    XtAppMainLoop(app);
}

/* Test execution functions */
void run_test_with_display(char *test_name, char **args, int argc)
{
    printf("\n==========================================\n");
    printf("Running test: %s\n", test_name);
    printf("==========================================\n");
    
    if (access(test_name, X_OK) != 0) {
        printf("ERROR: Test executable '%s' not found or not executable\n", test_name);
        return;
    }
    
    /* Set up environment for test */
    char *test_args[argc + 1];
    test_args[0] = test_name;
    for (int i = 0; i < argc; i++) {
        test_args[i + 1] = args[i];
    }
    test_args[argc + 1] = NULL;
    
    /* Execute test */
    int result = execv(test_name, test_args);
    if (result == -1) {
        perror("execv failed");
    }
}

/* Display information functions */
void print_display_info()
{
    printf("\n==========================================\n");
    printf("Display Information\n");
    printf("==========================================\n");
    printf("Display: %s\n", DisplayString(display));
    printf("Screen: %dx%d, depth: %d\n", 
           WidthOfScreen(screen), HeightOfScreen(screen), 
           DefaultDepthOfScreen(screen));
    printf("Visual: %s\n", 
           (DefaultVisualOfScreen(screen)->class == TrueColor) ? "TrueColor" :
           (DefaultVisualOfScreen(screen)->class == PseudoColor) ? "PseudoColor" :
           (DefaultVisualOfScreen(screen)->class == DirectColor) ? "DirectColor" :
           (DefaultVisualOfScreen(screen)->class == StaticColor) ? "StaticColor" :
           (DefaultVisualOfScreen(screen)->class == StaticGray) ? "StaticGray" :
           (DefaultVisualOfScreen(screen)->class == GrayScale) ? "GrayScale" : "Unknown");
    printf("==========================================\n\n");
}
EOF

    echo "Enhanced test library created: $CURRENT_TESTS/XmString/testlib_enhanced.c"
    echo ""
}

# Function to update StringExt to use enhanced testlib
update_stringext() {
    echo "Updating StringExt to use enhanced test library..."
    
    # Backup original
    cp "$CURRENT_TESTS/XmString/StringExt.c" "$CURRENT_TESTS/XmString/StringExt.c.backup"
    
    # Update to use enhanced testlib
    sed -i 's/#include "testlib_stub.c"/#include "testlib_enhanced.c"/' "$CURRENT_TESTS/XmString/StringExt.c"
    
    echo "StringExt updated to use enhanced test library"
    echo ""
}

# Function to build test programs
build_test_programs() {
    echo "Building test programs..."
    
    cd "$CURRENT_TESTS/XmString"
    
    # Build StringExt with enhanced testlib
    echo "Building StringExt..."
    make clean || true
    make StringExt || {
        echo "ERROR: Failed to build StringExt"
        return 1
    }
    
    echo "Test programs built successfully"
    echo ""
}

# Function to create test runner script
create_test_runner() {
    echo "Creating test runner script..."
    
    cat > "$CURRENT_TESTS/run_graphical_tests.sh" << 'EOF'
#!/bin/bash

# Motif Graphical Test Runner
# This script runs Motif tests in a graphical environment

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_HOME="$(dirname "$SCRIPT_DIR")"

# Source the enhanced test library
source "$SCRIPT_DIR/XmString/testlib_enhanced.c" 2>/dev/null || true

# Set up environment
export MOTIF_HOME="$TESTS_HOME"
export LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:$LIBRARY_PATH"
export LD_LIBRARY_PATH="/usr/X11R6/lib:$TESTS_HOME/lib/Xm/.libs:$TESTS_HOME/tests/auto/lib/common:$TESTS_HOME/tests/auto/lib/Create:$TESTS_HOME/tests/auto/lib/Script:$TESTS_HOME/tests/auto/lib/Synth:$TESTS_HOME/tests/auto/lib/Util:$TESTS_HOME/tests/auto/lib/Visual:$LD_LIBRARY_PATH"

echo "=========================================="
echo "Motif Graphical Test Runner"
echo "=========================================="
echo "TESTS_HOME: $TESTS_HOME"
echo "DISPLAY: ${DISPLAY:-'not set'}"
echo ""

# Check if we have a display
if [ -z "${DISPLAY:-}" ]; then
    echo "ERROR: No DISPLAY environment variable set."
    echo "Please set DISPLAY (e.g., export DISPLAY=:0.0)"
    exit 1
fi

# Check if X11 is running
if ! xset q >/dev/null 2>&1; then
    echo "ERROR: Cannot connect to X11 server."
    echo "Please ensure X11 is running and accessible."
    exit 1
fi

echo "X11 connection successful"
echo ""

# Run available tests
cd "$SCRIPT_DIR/XmString"

echo "Available test programs:"
ls -la *Ext *Comp *Draw *Width 2>/dev/null || echo "No test programs found"

echo ""
echo "Running StringExt test..."
if [ -x "./StringExt" ]; then
    echo "Executing StringExt..."
    ./StringExt
    echo "StringExt completed"
else
    echo "StringExt not found or not executable"
fi

echo ""
echo "Test run completed"
EOF

    chmod +x "$CURRENT_TESTS/run_graphical_tests.sh"
    echo "Test runner created: $CURRENT_TESTS/run_graphical_tests.sh"
    echo ""
}

# Function to create comprehensive test setup
create_comprehensive_setup() {
    echo "Creating comprehensive test setup..."
    
    # Create a master test setup script
    cat > "$TESTS_HOME/setup_graphical_tests.sh" << 'EOF'
#!/bin/bash

# Comprehensive Motif Test Setup Script
# Sets up the complete graphical test environment

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=========================================="
echo "Comprehensive Motif Test Setup"
echo "=========================================="

# Check prerequisites
echo "Checking prerequisites..."

# Check for X11
if [ -z "${DISPLAY:-}" ]; then
    echo "WARNING: No DISPLAY set. Tests require graphical environment."
fi

# Check for required libraries
echo "Checking for required libraries..."

if ! ldconfig -p | grep -q libXm; then
    echo "WARNING: libXm not found in system libraries"
fi

if ! ldconfig -p | grep -q libXt; then
    echo "WARNING: libXt not found in system libraries"
fi

if ! ldconfig -p | grep -q libX11; then
    echo "WARNING: libX11 not found in system libraries"
fi

echo "Prerequisites check completed"
echo ""

# Set up environment
echo "Setting up environment..."

export MOTIF_HOME="$SCRIPT_DIR"
export LIBRARY_PATH="/usr/X11R6/lib:$SCRIPT_DIR/lib/Xm/.libs:$SCRIPT_DIR/tests/auto/lib/common:$SCRIPT_DIR/tests/auto/lib/Create:$SCRIPT_DIR/tests/auto/lib/Script:$SCRIPT_DIR/tests/auto/lib/Synth:$SCRIPT_DIR/tests/auto/lib/Util:$SCRIPT_DIR/tests/auto/lib/Visual:$LIBRARY_PATH"
export LD_LIBRARY_PATH="/usr/X11R6/lib:$SCRIPT_DIR/lib/Xm/.libs:$SCRIPT_DIR/tests/auto/lib/common:$SCRIPT_DIR/tests/auto/lib/Create:$SCRIPT_DIR/tests/auto/lib/Script:$SCRIPT_DIR/tests/auto/lib/Synth:$SCRIPT_DIR/tests/auto/lib/Util:$SCRIPT_DIR/tests/auto/lib/Visual:$LD_LIBRARY_PATH"
export C_INCLUDE_PATH="$SCRIPT_DIR/include:$SCRIPT_DIR/lib:$C_INCLUDE_PATH"
export C_PATH="$SCRIPT_DIR/lib:$C_PATH"

echo "Environment variables set"
echo ""

# Build Motif libraries if needed
if [ ! -f "$SCRIPT_DIR/lib/Xm/.libs/libXm.so" ]; then
    echo "Building Motif libraries..."
    cd "$SCRIPT_DIR/lib"
    make clean || true
    make -j$(nproc) || {
        echo "ERROR: Failed to build Motif libraries"
        exit 1
    }
    echo "Motif libraries built"
else
    echo "Motif libraries already built"
fi

echo ""
echo "=========================================="
echo "Test Environment Setup Complete"
echo "=========================================="
echo ""
echo "To run tests:"
echo "  cd tests"
echo "  ./run_graphical_tests.sh"
echo ""
echo "To run individual tests:"
echo "  cd tests/XmString"
echo "  ./StringExt"
echo ""
EOF

    chmod +x "$TESTS_HOME/setup_graphical_tests.sh"
    echo "Comprehensive setup script created: $TESTS_HOME/setup_graphical_tests.sh"
    echo ""
}

# Main execution
main() {
    echo "Starting Motif test environment revival..."
    echo ""
    
    # Check prerequisites
    check_imake
    check_x11_libs
    
    # Setup environment
    setup_environment
    
    # Build Motif libraries
    build_motif_libs
    
    # Create enhanced test library
    create_enhanced_testlib
    
    # Update StringExt
    update_stringext
    
    # Build test programs
    build_test_programs
    
    # Create test runner
    create_test_runner
    
    # Create comprehensive setup
    create_comprehensive_setup
    
    echo "=========================================="
    echo "Test Environment Revival Complete!"
    echo "=========================================="
    echo ""
    echo "Next steps:"
    echo "1. Run the comprehensive setup:"
    echo "   ./setup_graphical_tests.sh"
    echo ""
    echo "2. Run the graphical tests:"
    echo "   cd tests && ./run_graphical_tests.sh"
    echo ""
    echo "3. Run individual tests:"
    echo "   cd tests/XmString && ./StringExt"
    echo ""
    echo "Make sure you have a graphical environment running!"
    echo "If running remotely, use X11 forwarding:"
    echo "   ssh -X username@hostname"
    echo ""
}

# Run main function
main "$@"
