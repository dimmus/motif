/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * The drag and drop receiver info: the input is stored as the
 * _MOTIF_DRAG_RECEIVER_INFO property of a window, as a drop site
 * client would, and read back the way a drag source does when the
 * pointer enters that window (_XmGetDragReceiverInfo), then the drop
 * site tree is parsed from it (the drop site manager's changeRoot,
 * ReadTree) and freed again.  Needs an X server (run under xvfb-run).
 */
#include <X11/Xatom.h>
#include <Xm/XmP.h>
#include <Xm/DragC.h>
#include <Xm/DragCP.h>
#include <Xm/DropSMgrP.h>
#include <Xm/DisplayP.h>
#include <Xm/DisplayI.h>
#include <Xm/DragCI.h>
#include <Xm/DragICCI.h>

#include "fuzz_common.h"

static Window target;
static Atom receiver_info;
static XmDropSiteManagerObject dsm;

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	Display *dpy;

	(void)argc;
	dpy = XtDisplay(fuzz_open_display((*argv)[0]));
	target = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 100, 100,
				     0, 0, 0);
	receiver_info = XInternAtom(dpy, "_MOTIF_DRAG_RECEIVER_INFO", False);
	dsm = _XmGetDropSiteManagerObject((XmDisplay)XmGetXmDisplay(dpy));
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	Display *dpy = XtDisplay(fuzz_top);
	XmDragReceiverInfoStruct info;
	XmDragTopLevelClientDataStruct cd;
	XmTopLevelEnterCallbackStruct enter;
	XmTopLevelLeaveCallbackStruct leave;

	if (size > 64 * 1024)
		return 0;
	XChangeProperty(dpy, target, receiver_info, receiver_info, 8,
			PropModeReplace, data, (int)size);
	memset(&info, 0, sizeof info);
	if (!_XmGetDragReceiverInfo(dpy, target, &info) || !info.iccInfo)
		return 0;

	memset(&cd, 0, sizeof cd);
	cd.destShell = NULL;		/* a remote client: read the stream */
	cd.iccInfo = info.iccInfo;
	cd.window = target;
	cd.xOrigin = info.xOrigin;
	cd.yOrigin = info.yOrigin;
	cd.width = info.width;
	cd.height = info.height;
	memset(&enter, 0, sizeof enter);
	enter.reason = XmCR_TOP_LEVEL_ENTER;
	enter.timeStamp = CurrentTime;
	enter.screen = DefaultScreenOfDisplay(dpy);
	enter.window = target;
	DSMChangeRoot(dsm, (XtPointer)&cd, (XtPointer)&enter);

	memset(&leave, 0, sizeof leave);
	leave.reason = XmCR_TOP_LEVEL_LEAVE;
	leave.timeStamp = CurrentTime;
	leave.screen = DefaultScreenOfDisplay(dpy);
	leave.window = target;
	DSMChangeRoot(dsm, (XtPointer)&cd, (XtPointer)&leave);

	_XmFreeDragReceiverInfo(info.iccInfo);
	return 0;
}
