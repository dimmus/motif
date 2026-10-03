/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * wmclient: a small top-level client for the mwm tests.  It maps a
 * shell, prints its window id, and prints a line whenever the window
 * manager reparents it, maps or unmaps it (iconify), or configures it,
 * so the driver can see what mwm does.  With --hostile it first sets
 * malformed WM_HINTS, WM_NORMAL_HINTS, _MOTIF_WM_HINTS,
 * WM_COLORMAP_WINDOWS and _MOTIF_WM_MENU, to check that mwm survives
 * them.  Exits 77 without a display.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

static Atom prop(Display *d, const char *n)
{
	return XInternAtom(d, n, False);
}

/* Set deliberately malformed versions of the properties mwm reads. */
static void set_hostile(Display *dpy, Window w)
{
	unsigned char small[1] = { 0xff };
	long huge[4] = { 0x7fffffff, 0x7fffffff, -123456, 0x40000000 };
	long hints[4] = { 0x7fffffff, 0, 0, 0 };	/* every flag set */
	Window cmap_windows[3];

	/* WM_HINTS and WM_NORMAL_HINTS far too short */
	XChangeProperty(dpy, w, XA_WM_HINTS, XA_WM_HINTS, 32,
			PropModeReplace, small, 1);
	XChangeProperty(dpy, w, XA_WM_NORMAL_HINTS, XA_WM_SIZE_HINTS, 32,
			PropModeReplace, (unsigned char *)huge, 2);
	/* _MOTIF_WM_HINTS with all flags and nonsense values */
	XChangeProperty(dpy, w, prop(dpy, "_MOTIF_WM_HINTS"),
			prop(dpy, "_MOTIF_WM_HINTS"), 32, PropModeReplace,
			(unsigned char *)hints, 4);
	/* WM_COLORMAP_WINDOWS listing bogus windows */
	cmap_windows[0] = w;
	cmap_windows[1] = 0x12345678;
	cmap_windows[2] = 0;
	XChangeProperty(dpy, w, prop(dpy, "WM_COLORMAP_WINDOWS"), XA_WINDOW, 32,
			PropModeReplace, (unsigned char *)cmap_windows, 3);
	/* a _MOTIF_WM_MENU naming functions that are not allowed from a
	 * client, overlong */
	{
		char menu[4096];

		memset(menu, 'A', sizeof menu - 1);
		menu[sizeof menu - 1] = '\0';
		memcpy(menu, "f.exec \"x\" f.restart f.set_behavior ", 36);
		XChangeProperty(dpy, w, prop(dpy, "_MOTIF_WM_MENU"), XA_STRING,
				8, PropModeReplace, (unsigned char *)menu,
				(int)strlen(menu));
	}
}

int main(int argc, char **argv)
{
	Display *dpy;
	Window w, root;
	XSetWindowAttributes attr;
	XSizeHints hints;
	XTextProperty name;
	char *title = "mwmclient";
	int hostile = 0, i;
	XEvent ev;

	for (i = 1; i < argc; i++)
		if (!strcmp(argv[i], "--hostile"))
			hostile = 1;
		else if (!strcmp(argv[i], "--title") && i + 1 < argc)
			title = argv[++i];

	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		printf("wmclient: SKIP: no display\n");
		return 77;
	}
	root = DefaultRootWindow(dpy);
	attr.event_mask = StructureNotifyMask;
	attr.background_pixel = WhitePixel(dpy, DefaultScreen(dpy));
	w = XCreateWindow(dpy, root, 50, 50, 120, 80, 0, CopyFromParent,
			  InputOutput, CopyFromParent,
			  CWEventMask | CWBackPixel, &attr);
	XStoreName(dpy, w, title);
	XStringListToTextProperty(&title, 1, &name);
	hints.flags = PPosition | PSize | PMinSize;
	hints.x = 50;
	hints.y = 50;
	hints.width = 120;
	hints.height = 80;
	hints.min_width = 40;
	hints.min_height = 30;
	XSetWMProperties(dpy, w, &name, &name, argv, argc, &hints, NULL, NULL);

	if (hostile)
		set_hostile(dpy, w);

	XMapWindow(dpy, w);
	printf("window 0x%lx\n", (unsigned long)w);
	fflush(stdout);

	for (;;) {
		XNextEvent(dpy, &ev);
		switch (ev.type) {
		case ReparentNotify:
			printf("reparent parent=0x%lx\n",
			       (unsigned long)ev.xreparent.parent);
			break;
		case MapNotify:
			printf("map\n");
			break;
		case UnmapNotify:
			printf("unmap\n");
			break;
		case ConfigureNotify:
			printf("configure %dx%d+%d+%d\n", ev.xconfigure.width,
			       ev.xconfigure.height, ev.xconfigure.x,
			       ev.xconfigure.y);
			break;
		case DestroyNotify:
			printf("destroy\n");
			fflush(stdout);
			return 0;
		default:
			break;
		}
		fflush(stdout);
	}
}
