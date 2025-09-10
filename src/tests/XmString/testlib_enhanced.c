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

/* definitions used in all test modules */
#define MAX_ARGS  	100
#define MAX_FILE        256
#define MAX_FONT_LEN    64

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

/* Error and summary handling functions are provided by convenience.c */

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
