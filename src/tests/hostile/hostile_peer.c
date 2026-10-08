/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * hostile_peer: a deliberately malformed X client, using Xlib only, for
 * the end-to-end hostile-client test (hostile.sh).  It plays the part
 * other clients play in Motif's inter-client protocols -- drop site,
 * drag source, clipboard owner and the owner of the root-window drag
 * and bindings properties -- and answers every request with data a
 * well-behaved peer would never send: truncated and oversized
 * properties, the wrong byte order, wrong type and format, huge counts
 * and out-of-range indices.  A Motif application built with ASan+UBSan
 * must parse all of it without crashing or reporting an error.
 *
 *   hostile_peer --dropsite  [--geometry WxH+X+Y]   (also sets root)
 *   hostile_peer --source    --target <victim window>
 *   hostile_peer --clipboard
 *   hostile_peer --root
 *
 * In --dropsite mode it creates one window per malformed receiver-info
 * variant in a horizontal row, so a single drag sweep crosses them all,
 * and prints "window <id> ... <id>" and "ready".  In --source mode it
 * injects the Motif/Xdnd ClientMessages of a drag toward the victim and
 * answers its selection conversions with garbage.  In --clipboard mode
 * it owns CLIPBOARD and answers TARGETS and data conversions with
 * garbage, and writes malformed _MOTIF_CLIP_* records on the root.  In
 * --root mode it writes malformed _MOTIF_DRAG_WINDOW,
 * _MOTIF_DRAG_TARGETS, _MOTIF_DRAG_ATOMS and _MOTIF_BINDINGS and exits.
 *
 * Exits 77 without a display.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define NELEM(a) ((int)(sizeof(a) / sizeof((a)[0])))

static Display *dpy;
static Window root;

static Atom A(const char *name)
{
	return XInternAtom(dpy, name, False);
}

/* --- malformed root-window drag and bindings properties ---------------- */

/*
 * The real _MOTIF_DRAG_WINDOW names the hidden window that holds the
 * target and atom tables; _MOTIF_DRAG_TARGETS/_MOTIF_DRAG_ATOMS live on
 * that window.  We point _MOTIF_DRAG_WINDOW at a window of our own and
 * fill the table properties with lies: counts far larger than the data,
 * the wrong byte order, the wrong format and type, and heap offsets past
 * the end.  _XmInitTargetsTable()/ReadAtomsTable()/ReadTargetsTable()
 * read these on the first drag.
 */
static void set_hostile_root(void)
{
	Window drag_win;
	unsigned char targets[64];
	unsigned char atoms[64];
	long badwin[2];
	const char bindings[] =
		/* a long, malformed _MOTIF_BINDINGS: osfkeysyms with no key,
		 * duplicated specifiers and an unterminated entry */
		"<Key>osfADQWERTY:ident\n"
		":bogus\n"
		"<Btn1Down>,<Btn1Down>,<Btn1Down>:again again again\n"
		"osffoo<Key>:";
	int i;

	/* a window we own, so the reader's XGetWindowProperty succeeds but the
	 * table properties on it are malformed */
	drag_win = XCreateSimpleWindow(dpy, root, -200, -200, 10, 10, 0, 0, 0);

	/* _MOTIF_DRAG_WINDOW: correct type and format but naming our window */
	XChangeProperty(dpy, root, A("_MOTIF_DRAG_WINDOW"), XA_WINDOW, 32,
			PropModeReplace, (unsigned char *)&drag_win, 1);

	/* a second variant: also leave a _MOTIF_DRAG_WINDOW of the wrong type
	 * on a sibling atom path is not possible (single atom), so additionally
	 * publish a bogus window id via a short property to exercise the
	 * BadWindow recovery in ReadMotifWindow()/StartProtectedSection() */
	badwin[0] = 0x3fffffff;
	badwin[1] = 0;

	/*
	 * _MOTIF_DRAG_TARGETS header is
	 *   byte_order(1) protocol_version(1) num_target_lists(2) heap_offset(4)
	 * Claim many target lists but give almost no data, and flip the byte
	 * order so the reader byte-swaps the (already huge) count.
	 */
	memset(targets, 0, sizeof targets);
	targets[0] = (char)('l' ^ 'B' ^ 'l'); /* force != this client's order */
	targets[0] = 'B';
	targets[1] = 0;      /* protocol version */
	targets[2] = 0xff;   /* num_target_lists high byte (big-endian) */
	targets[3] = 0xff;
	targets[4] = 0x7f;   /* heap_offset */
	targets[5] = 0xff;
	targets[6] = 0xff;
	targets[7] = 0xff;
	/* a partial first list: a count that overruns */
	targets[8] = 0xff;
	targets[9] = 0xff;
	XChangeProperty(dpy, drag_win, A("_MOTIF_DRAG_TARGETS"),
			A("_MOTIF_DRAG_TARGETS"), 8, PropModeReplace, targets, 12);

	/*
	 * _MOTIF_DRAG_ATOMS header is
	 *   byte_order(1) protocol_version(1) num_atoms(2) heap_offset(4)
	 * Claim a huge atom count with no entries behind it.
	 */
	memset(atoms, 0, sizeof atoms);
	atoms[0] = 'l';
	atoms[1] = 0;
	atoms[2] = 0xff; /* num_atoms low (little-endian) */
	atoms[3] = 0xff;
	atoms[4] = 0x08;
	atoms[5] = 0;
	atoms[6] = 0;
	atoms[7] = 0;
	XChangeProperty(dpy, drag_win, A("_MOTIF_DRAG_ATOMS"),
			A("_MOTIF_DRAG_ATOMS"), 8, PropModeReplace, atoms, 8);

	/* _MOTIF_BINDINGS on the root, STRING/8 as the reader expects, plus a
	 * copy with a bad type and format to make sure the checked reader
	 * rejects them */
	XChangeProperty(dpy, root, A("_MOTIF_BINDINGS"), XA_STRING, 8,
			PropModeReplace, (const unsigned char *)bindings,
			(int)sizeof bindings - 1);
	/* also a wrong-format sibling on _MOTIF_DEFAULT_BINDINGS */
	XChangeProperty(dpy, root, A("_MOTIF_DEFAULT_BINDINGS"), XA_STRING, 32,
			PropModeReplace, (unsigned char *)badwin, 2);

	XFlush(dpy);
	for (i = 0; i < 0; i++)
		;
}

/* --- malformed _MOTIF_DRAG_RECEIVER_INFO drop sites -------------------- */

/*
 * The receiver-info header on the wire (xmDragReceiverInfoStruct) is 16
 * bytes:
 *   byte_order(1) protocol_version(1) drag_protocol_style(1) pad(1)
 *   proxy_window(4) num_drop_sites(2) pad(2) heap_offset(4)
 * followed by the drop-site records (between the header and heap_offset)
 * and the target heap.  Each variant below is a different way of lying
 * about those fields.
 */
static int make_receiver_info(int variant, unsigned char *buf, int cap)
{
	memset(buf, 0, (size_t)cap);

	switch (variant) {
	case 0: /* truncated: shorter than the fixed header */
		buf[0] = 'l';
		buf[1] = 0;
		return 5;
	case 1: /* heap_offset past the end of the property */
		buf[0] = 'l';
		buf[1] = 0;
		buf[2] = 3;              /* XmDRAG_PREREGISTER */
		buf[12] = 0xff;         /* heap_offset = 0x7fffffff */
		buf[13] = 0xff;
		buf[14] = 0xff;
		buf[15] = 0x7f;
		buf[8] = 0xff;          /* num_drop_sites = 0xffff */
		buf[9] = 0xff;
		return 24;
	case 2: /* wrong byte order, huge swapped num_drop_sites/heap_offset */
		buf[0] = 'B';
		buf[1] = 0;
		buf[2] = 3;
		buf[8] = 0xff;          /* num_drop_sites */
		buf[9] = 0xff;
		buf[12] = 0x7f;         /* heap_offset big-endian -> huge */
		buf[13] = 0xff;
		buf[14] = 0xff;
		buf[15] = 0xff;
		return 20;
	case 3: /* valid header, but a drop-site stream that lies about its
		 * region box count (huge) */
		buf[0] = 'l';
		buf[1] = 0;
		buf[2] = 3;             /* XmDRAG_PREREGISTER */
		buf[8] = 1;             /* num_drop_sites = 1 */
		buf[9] = 0;
		buf[12] = 16;           /* heap_offset: header is 16 bytes */
		buf[13] = 0;
		buf[14] = 0;
		buf[15] = 0;
		/* ... but no drop-site data follows (data area is empty), so the
		 * stream reader must fail the short read, not read past the end */
		return 16;
	case 4: { /* valid header + a drop-site header claiming a huge box count */
		int off = 16;
		buf[0] = 'l';
		buf[1] = 0;
		buf[2] = 3;
		buf[8] = 1;             /* num_drop_sites = 1 */
		buf[9] = 0;
		/* data area holds the drop-site records; put the ds header here
		 * and a large heap_offset so the data area is non-empty */
		buf[12] = 40;           /* heap_offset */
		buf[13] = 0;
		buf[14] = 0;
		buf[15] = 0;
		/* xmDSHeaderStruct: flags(2) import_targets_id(2) numBoxes(4) */
		buf[off + 0] = 0x00;    /* flags: animationStyle NONE */
		buf[off + 1] = 0x00;
		buf[off + 2] = 0;
		buf[off + 3] = 0;
		buf[off + 4] = 0xff;    /* dsRegionNumBoxes = 0xffffffff */
		buf[off + 5] = 0xff;
		buf[off + 6] = 0xff;
		buf[off + 7] = 0xff;
		return 40;
	}
	case 5: /* protocol style out of range (used as a table index) */
		buf[0] = 'l';
		buf[1] = 7;             /* bogus protocol version */
		buf[2] = 0xff;          /* drag_protocol_style way out of range */
		buf[12] = 16;
		return 16;
	case 6: /* all 0xff garbage */
		memset(buf, 0xff, 32);
		return 32;
	default:
		return 0;
	}
}

#define NUM_DROP_VARIANTS 7

static void run_dropsite(const char *geom)
{
	Window win[NUM_DROP_VARIANTS];
	Atom recv = A("_MOTIF_DRAG_RECEIVER_INFO");
	Atom xdndAware = A("XdndAware");
	unsigned char buf[256];
	int i, x = 0, y = 0, w = 80, h = 120;
	XEvent ev;

	(void)geom;
	set_hostile_root();

	for (i = 0; i < NUM_DROP_VARIANTS; i++) {
		int len;
		long aware = 0x7fffffff; /* a bogus XdndAware version */

		win[i] = XCreateSimpleWindow(dpy, root, x + i * (w + 4), y, w, h,
					     0, 0, WhitePixel(dpy, 0));
		XSelectInput(dpy, win[i], ButtonPressMask | ExposureMask |
			     StructureNotifyMask | PropertyChangeMask);
		len = make_receiver_info(i, buf, sizeof buf);
		XChangeProperty(dpy, win[i], recv, recv, 8, PropModeReplace,
				buf, len);
		/* also publish a malformed XdndAware so the Xdnd fallback path
		 * in _XmGetDragReceiverInfo runs too */
		XChangeProperty(dpy, win[i], xdndAware, XA_ATOM, 32,
				PropModeReplace, (unsigned char *)&aware, 1);
		XMapWindow(dpy, win[i]);
	}
	XFlush(dpy);

	printf("window");
	for (i = 0; i < NUM_DROP_VARIANTS; i++)
		printf(" 0x%lx", (unsigned long)win[i]);
	printf("\n");
	printf("ready\n");
	fflush(stdout);

	for (;;) {
		XNextEvent(dpy, &ev);
		if (ev.type == SelectionRequest) {
			/* a drop may ask us to convert; refuse with None */
			XSelectionEvent se;
			XSelectionRequestEvent *re = &ev.xselectionrequest;

			memset(&se, 0, sizeof se);
			se.type = SelectionNotify;
			se.display = dpy;
			se.requestor = re->requestor;
			se.selection = re->selection;
			se.target = re->target;
			se.property = None; /* refuse */
			se.time = re->time;
			XSendEvent(dpy, re->requestor, False, 0, (XEvent *)&se);
			XFlush(dpy);
		}
	}
}

/* --- hostile drag source ----------------------------------------------- */

/*
 * Send the Motif drag-and-drop ClientMessages the victim's receiver
 * handler consumes, with malformed payloads, and answer the selection
 * conversions it makes (TARGETS, _MOTIF_DRAG_TARGETS, data) with garbage.
 */
static void send_motif_dnd(Window target, int message_type,
			   unsigned char *data16)
{
	XClientMessageEvent ev;

	memset(&ev, 0, sizeof ev);
	ev.type = ClientMessage;
	ev.display = dpy;
	ev.window = target;
	ev.message_type = A("_MOTIF_DRAG_AND_DROP_MESSAGE");
	ev.format = 8;
	memcpy(ev.data.b, data16, 16);
	ev.data.b[0] = (char)message_type;
	ev.data.b[1] = (char)'l';
	XSendEvent(dpy, target, False, 0, (XEvent *)&ev);
}

static void run_source(Window target)
{
	Window src;
	Atom init = A("_MOTIF_DRAG_INITIATOR_INFO");
	Atom typelist = A("XdndTypeList");
	unsigned char info[16];
	unsigned char msg[16];
	long bogus_types[4];
	XEvent ev;
	int fired;

	src = XCreateSimpleWindow(dpy, root, 0, 0, 10, 10, 0, 0, 0);
	XSelectInput(dpy, src, PropertyChangeMask);

	/* malformed initiator info: too short, wrong byte order, a targets
	 * index far out of range.  _XmReadInitiatorInfo() reads it. */
	memset(info, 0, sizeof info);
	info[0] = 'B';            /* wrong byte order */
	info[1] = 9;              /* bad protocol version */
	info[2] = 0xff;           /* targets_index high */
	info[3] = 0xff;
	XChangeProperty(dpy, src, init, init, 8, PropModeReplace, info, 4);

	/* a malformed XdndTypeList: wrong type/format and bogus atoms */
	bogus_types[0] = 0x7fffffff;
	bogus_types[1] = 0;
	bogus_types[2] = 0xdeadbeef;
	bogus_types[3] = 0x10000;
	XChangeProperty(dpy, src, typelist, XA_ATOM, 32, PropModeReplace,
			(unsigned char *)bogus_types, 4);
	/* and a wrong-format copy to exercise the checked reader's reject */
	XChangeProperty(dpy, src, typelist, XA_ATOM, 8, PropModeAppend,
			(unsigned char *)bogus_types, 1);

	memset(msg, 0xff, sizeof msg); /* huge x/y/flags/time */

	printf("source 0x%lx\n", (unsigned long)src);
	printf("ready\n");
	fflush(stdout);

	/* TOP_LEVEL_ENTER (0), DRAG_MOTION (2), DROP_START (5), with the icc
	 * handle field pointing at our src window */
	{
		long sw = (long)src;

		memcpy(msg + 8, &sw, 4);  /* src_window for TOP_LEVEL_ENTER */
	}
	send_motif_dnd(target, 0, msg);  /* XmTOP_LEVEL_ENTER */
	XFlush(dpy);
	send_motif_dnd(target, 2, msg);  /* XmDRAG_MOTION */
	XFlush(dpy);
	send_motif_dnd(target, 5, msg);  /* XmDROP_START */
	XFlush(dpy);
	/* an out-of-range message type (parser must reject, not index) */
	send_motif_dnd(target, 0x7f, msg);
	XFlush(dpy);

	/* answer any selection request the victim's drop transfer makes */
	fired = 0;
	for (;;) {
		struct timeval tv = { 2, 0 };
		fd_set fds;
		int fd = ConnectionNumber(dpy);

		while (XPending(dpy)) {
			XNextEvent(dpy, &ev);
			if (ev.type == SelectionRequest) {
				XSelectionRequestEvent *re =
					&ev.xselectionrequest;
				XSelectionEvent se;
				long garbage[8];
				int i;

				for (i = 0; i < 8; i++)
					garbage[i] = 0x41414141 + i;

				/* Answer with the wrong type and format and a
				 * huge-looking length (the toolkit must check
				 * type==XA_ATOM/format==32 before trusting a
				 * TARGETS reply). */
				XChangeProperty(dpy, re->requestor,
						re->property == None ?
							re->target :
							re->property,
						XA_STRING, 8, PropModeReplace,
						(unsigned char *)garbage,
						(int)sizeof garbage);
				memset(&se, 0, sizeof se);
				se.type = SelectionNotify;
				se.display = dpy;
				se.requestor = re->requestor;
				se.selection = re->selection;
				se.target = re->target;
				se.property = re->property == None ?
					re->target : re->property;
				se.time = re->time;
				XSendEvent(dpy, re->requestor, False, 0,
					   (XEvent *)&se);
				XFlush(dpy);
				fired = 1;
			}
		}
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		if (select(fd + 1, &fds, NULL, NULL, &tv) <= 0) {
			if (fired)
				break; /* answered at least one, then quiet */
			break;
		}
	}
}

/* --- hostile clipboard owner ------------------------------------------- */

/*
 * Own CLIPBOARD, answer TARGETS and data conversions with the wrong
 * type/format and overlong data, and leave malformed _MOTIF_CLIP_* records
 * on the root so the Motif clipboard code's record reader meets them too.
 */
static void write_hostile_clip_records(void)
{
	long header[12];
	long item[8];
	long nextid[1];
	int i;

	for (i = 0; i < 12; i++)
		header[i] = 0x7fffffff;
	/* a header whose counts/offsets point out of bounds */
	XChangeProperty(dpy, root, A("_MOTIF_CLIP_HEADER"), XA_INTEGER, 32,
			PropModeReplace, (unsigned char *)header, 12);
	for (i = 0; i < 8; i++)
		item[i] = 0x41414141;
	XChangeProperty(dpy, root, A("_MOTIF_CLIP_ITEM_1"), XA_INTEGER, 32,
			PropModeReplace, (unsigned char *)item, 8);
	/* a wrong-type, wrong-format next id */
	nextid[0] = 0x10000;
	XChangeProperty(dpy, root, A("_MOTIF_CLIP_NEXT_ID"), XA_STRING, 8,
			PropModeReplace, (unsigned char *)nextid, 2);
	XFlush(dpy);
}

static void run_clipboard(void)
{
	Window owner;
	Atom clipboard = A("CLIPBOARD");
	Atom targets = A("TARGETS");
	XEvent ev;

	owner = XCreateSimpleWindow(dpy, root, 0, 0, 10, 10, 0, 0, 0);
	XSetSelectionOwner(dpy, clipboard, owner, CurrentTime);
	write_hostile_clip_records();

	printf("owner 0x%lx\n", (unsigned long)owner);
	printf("ready\n");
	fflush(stdout);

	for (;;) {
		XNextEvent(dpy, &ev);
		if (ev.type == SelectionRequest) {
			XSelectionRequestEvent *re = &ev.xselectionrequest;
			XSelectionEvent se;
			Atom prop = re->property == None ? re->target :
							   re->property;

			if (re->target == targets) {
				/* a TARGETS reply of the wrong type and format,
				 * and a huge item count */
				long atoms[4];
				atoms[0] = (long)XA_STRING;
				atoms[1] = (long)clipboard;
				atoms[2] = 0x7fffffff;
				atoms[3] = 0;
				XChangeProperty(dpy, re->requestor, prop,
						XA_INTEGER, 8, PropModeReplace,
						(unsigned char *)atoms,
						(int)sizeof atoms);
			} else {
				/* data of the wrong format and overlong */
				char junk[512];
				memset(junk, 'Z', sizeof junk);
				XChangeProperty(dpy, re->requestor, prop,
						XA_STRING, 32, PropModeReplace,
						(unsigned char *)junk,
						(int)(sizeof junk / 4));
			}
			memset(&se, 0, sizeof se);
			se.type = SelectionNotify;
			se.display = dpy;
			se.requestor = re->requestor;
			se.selection = re->selection;
			se.target = re->target;
			se.property = prop;
			se.time = re->time;
			XSendEvent(dpy, re->requestor, False, 0, (XEvent *)&se);
			XFlush(dpy);
		}
		if (ev.type == SelectionClear)
			; /* keep running; the test drives several pastes */
	}
}

int main(int argc, char **argv)
{
	const char *mode = NULL;
	const char *geom = NULL;
	Window target = 0;
	int i;

	if (!getenv("DISPLAY") || !*getenv("DISPLAY")) {
		fprintf(stderr, "hostile_peer: SKIP: no DISPLAY\n");
		return 77;
	}
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--dropsite"))
			mode = "dropsite";
		else if (!strcmp(argv[i], "--source"))
			mode = "source";
		else if (!strcmp(argv[i], "--clipboard"))
			mode = "clipboard";
		else if (!strcmp(argv[i], "--root"))
			mode = "root";
		else if (!strcmp(argv[i], "--geometry") && i + 1 < argc)
			geom = argv[++i];
		else if (!strcmp(argv[i], "--target") && i + 1 < argc)
			target = (Window)strtoul(argv[++i], NULL, 0);
	}
	if (!mode) {
		fprintf(stderr, "usage: %s --dropsite|--source|--clipboard|--root"
			" [--target W] [--geometry G]\n", argv[0]);
		return 2;
	}
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "hostile_peer: cannot open display\n");
		return 1;
	}
	root = RootWindow(dpy, DefaultScreen(dpy));

	if (!strcmp(mode, "root")) {
		set_hostile_root();
		XSync(dpy, False);
		printf("ready\n");
		fflush(stdout);
		/* keep the override properties and our drag window alive */
		for (;;)
			pause();
	} else if (!strcmp(mode, "dropsite")) {
		run_dropsite(geom);
	} else if (!strcmp(mode, "source")) {
		if (!target) {
			fprintf(stderr, "--source needs --target\n");
			return 2;
		}
		run_source(target);
	} else if (!strcmp(mode, "clipboard")) {
		run_clipboard();
	}
	XCloseDisplay(dpy);
	return 0;
}
