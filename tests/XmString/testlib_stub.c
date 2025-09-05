/*
 * Minimal stub implementation of testlib functions for StringExt
 */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawingA.h>
#include <stdlib.h>
#include <unistd.h>

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

/* Stub implementations */
void CommonTestInit(int argc, char **argv)
{
    Widget toplevel;
    Arg args[10];
    int n;

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    
    display = XtOpenDisplay(app_context, NULL, argv[0], "StringExt",
                           NULL, 0, &argc, argv);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    
    screen = DefaultScreenOfDisplay(display);
    rootWindow = RootWindowOfScreen(screen);
    
    n = 0;
    toplevel = XtAppCreateShell(argv[0], "StringExt",
                               applicationShellWidgetClass, display, args, n);
    Shell1 = toplevel;
}

Widget BulletinBoardCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateBulletinBoard(parent, name, args, nargs);
}

Widget DrawingAreaCreate(Widget parent, char *name, Arg *args, int nargs, WidgetClass xtclass)
{
    return XmCreateDrawingArea(parent, name, args, nargs);
}

XmFontList CommonGetFontList(char *fontstr)
{
    return XmFontListCreate(XLoadQueryFont(display, fontstr), XmSTRING_DEFAULT_CHARSET);
}

void CommonPause()
{
    /* Simple pause implementation */
    sleep(1);
}

void AppMainLoop(XtAppContext app)
{
    XtAppMainLoop(app);
}
