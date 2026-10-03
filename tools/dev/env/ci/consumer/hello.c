/*
 * Minimal Motif client used by CI to check that an installed Motif can be
 * consumed: it must compile and link against the installed headers and
 * libraries only, and run.  It creates a push button, realizes it and
 * exits from a timer.  Without an X server it exits with status 77.
 */

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/PushB.h>
#ifdef HELLO_MRM
#include <Mrm/MrmPublic.h>
#endif

static void
quit(XtPointer client_data, XtIntervalId *id)
{
  (void) client_data;
  (void) id;
  exit(0);
}

int
main(int argc, char *argv[])
{
  XtAppContext app;
  Display *dpy;
  Widget top, button;

  printf("Motif %d.%d.%d\n", XmVERSION, XmREVISION, XmUPDATE_LEVEL);

  XtToolkitInitialize();
#ifdef HELLO_MRM
  MrmInitialize();
#endif
  app = XtCreateApplicationContext();
  dpy = XtOpenDisplay(app, NULL, "hello", "Hello", NULL, 0, &argc, argv);
  if (dpy == NULL) {
    fprintf(stderr, "hello: cannot open display, skipping\n");
    return 77;
  }

  top = XtVaAppCreateShell("hello", "Hello", applicationShellWidgetClass,
                           dpy, NULL);
  button = XmCreatePushButton(top, "Hello, world", NULL, 0);
  XtManageChild(button);
  XtRealizeWidget(top);

  XtAppAddTimeOut(app, 200, quit, NULL);
  XtAppMainLoop(app);
  return 1;
}
