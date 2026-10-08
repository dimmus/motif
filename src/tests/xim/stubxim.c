/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * stubxim: a minimal X input method server for the tests.
 *
 * It speaks the X Input Method Protocol (version 1.0, X transport 0.0)
 * to Xlib's XIM client, so that XOpenIM, XCreateIC and the preedit
 * callbacks run exactly as they do with a real input method, and XmIm's
 * on-the-spot, over-the-spot, off-the-spot and root paths can be tested
 * end to end.
 *
 *   stubxim NAME
 *
 * registers the server "@server=NAME" in the root window's XIM_SERVERS
 * and prints "ready" on stdout once a client can connect.  It exits when
 * stdin reaches end of file (the test that started it has gone), on
 * SelectionClear or when the X connection breaks.  It serves one client
 * at a time, in the client's own (native) byte order.
 *
 * Input: the client forwards KeyPress events (XIM_SET_EVENT_MASK).
 * Lower case letters are inserted into the preedit string at the caret,
 * Left and Right move the caret, BackSpace deletes before it, Escape
 * cancels and Return commits the string converted from romaji to
 * hiragana ("nihongo" commits "にほんご") as compound text.  Any other
 * key, and Return or BackSpace with no preedit, is sent back to the
 * client.  With the XIMPreeditCallbacks style the preedit is shown
 * through XIM_PREEDIT_START/DRAW/CARET/DONE; with the other styles only
 * the commit is sent.
 *
 * What the client asked for is appended, one line per request, to the
 * STRING property _STUBXIM_LOG of the root window (create_ic, spot,
 * area, focus, reset_ic, ...), for the tests to check.
 */
#include <errno.h>
#include <poll.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

/* XIM protocol opcodes and constants (X Input Method Protocol 1.0) */
enum {
	XIM_CONNECT = 1, XIM_CONNECT_REPLY, XIM_DISCONNECT, XIM_DISCONNECT_REPLY,
	XIM_ERROR = 20,
	XIM_OPEN = 30, XIM_OPEN_REPLY, XIM_CLOSE, XIM_CLOSE_REPLY,
	XIM_SET_EVENT_MASK = 37, XIM_ENCODING_NEGOTIATION,
	XIM_ENCODING_NEGOTIATION_REPLY, XIM_QUERY_EXTENSION,
	XIM_QUERY_EXTENSION_REPLY, XIM_SET_IM_VALUES, XIM_SET_IM_VALUES_REPLY,
	XIM_GET_IM_VALUES, XIM_GET_IM_VALUES_REPLY,
	XIM_CREATE_IC = 50, XIM_CREATE_IC_REPLY, XIM_DESTROY_IC,
	XIM_DESTROY_IC_REPLY, XIM_SET_IC_VALUES, XIM_SET_IC_VALUES_REPLY,
	XIM_GET_IC_VALUES, XIM_GET_IC_VALUES_REPLY, XIM_SET_IC_FOCUS,
	XIM_UNSET_IC_FOCUS, XIM_FORWARD_EVENT, XIM_SYNC, XIM_SYNC_REPLY,
	XIM_COMMIT, XIM_RESET_IC, XIM_RESET_IC_REPLY,
	XIM_PREEDIT_START = 73, XIM_PREEDIT_START_REPLY, XIM_PREEDIT_DRAW,
	XIM_PREEDIT_CARET, XIM_PREEDIT_CARET_REPLY, XIM_PREEDIT_DONE
};

enum {
	T_SEPARATOR = 0, T_CARD32 = 3, T_WINDOW = 5, T_STYLES = 10,
	T_RECTANGLE = 11, T_POINT = 12, T_FONTSET = 13, T_NEST = 0x7fff
};

#define XIM_SYNCHRONOUS 0x0001
#define XIM_LOOKUP_CHARS 0x0002
#define CM_DATA_SIZE 20

/* The IC attributes this server knows, by ID */
enum {
	A_INPUT_STYLE, A_CLIENT_WINDOW, A_FOCUS_WINDOW, A_FILTER_EVENTS,
	A_PREEDIT_ATTR, A_STATUS_ATTR, A_FONT_SET, A_AREA, A_AREA_NEEDED,
	A_COLORMAP, A_STD_COLORMAP, A_FOREGROUND, A_BACKGROUND,
	A_BACKGROUND_PIXMAP, A_SPOT_LOCATION, A_LINE_SPACE, A_CURSOR,
	A_SEPARATOR, A_RESET_STATE, N_IC_ATTRS
};

static const struct attr {
	const char *name;
	unsigned short type;
} ic_attrs[N_IC_ATTRS] = {
	[A_INPUT_STYLE] = { XNInputStyle, T_CARD32 },
	[A_CLIENT_WINDOW] = { XNClientWindow, T_WINDOW },
	[A_FOCUS_WINDOW] = { XNFocusWindow, T_WINDOW },
	[A_FILTER_EVENTS] = { XNFilterEvents, T_CARD32 },
	[A_PREEDIT_ATTR] = { XNPreeditAttributes, T_NEST },
	[A_STATUS_ATTR] = { XNStatusAttributes, T_NEST },
	[A_FONT_SET] = { XNFontSet, T_FONTSET },
	[A_AREA] = { XNArea, T_RECTANGLE },
	[A_AREA_NEEDED] = { XNAreaNeeded, T_RECTANGLE },
	[A_COLORMAP] = { XNColormap, T_CARD32 },
	[A_STD_COLORMAP] = { XNStdColormap, T_CARD32 },
	[A_FOREGROUND] = { XNForeground, T_CARD32 },
	[A_BACKGROUND] = { XNBackground, T_CARD32 },
	[A_BACKGROUND_PIXMAP] = { XNBackgroundPixmap, T_CARD32 },
	[A_SPOT_LOCATION] = { XNSpotLocation, T_POINT },
	[A_LINE_SPACE] = { XNLineSpace, T_CARD32 },
	[A_CURSOR] = { XNCursor, T_CARD32 },
	[A_SEPARATOR] = { XNSeparatorofNestedList, T_SEPARATOR },
	[A_RESET_STATE] = { XNResetState, T_CARD32 },
};

/* The single IM attribute, XNQueryInputStyle */
#define IM_QUERY_INPUT_STYLE 0

static const unsigned long styles[] = {
	XIMPreeditCallbacks | XIMStatusNothing,
	XIMPreeditPosition | XIMStatusNothing,
	XIMPreeditArea | XIMStatusArea,
	XIMPreeditNothing | XIMStatusNothing,
	XIMPreeditNone | XIMStatusNone,
};

/* The status and preedit areas asked for with the off-the-spot style */
#define STATUS_WIDTH 60
#define AREA_HEIGHT 20

#define IMID 1
#define MAX_ICS 16
#define MAX_PREEDIT 64

struct area {
	short x, y;
	unsigned short width, height;
};

struct ic {
	int used;
	unsigned long style;
	unsigned long fg, bg;
	struct area preedit_area, status_area;
	/* The preedit string (ASCII), its caret, and whether it was started */
	char text[MAX_PREEDIT + 1];
	int len, caret, started;
};

static Display *dpy;
static Window server_window, comm_window, client_window;
static Atom server_atom, xim_servers, locales_atom, transport_atom;
static Atom xim_xconnect, xim_protocol, xim_moredata, log_atom;
static struct ic ics[MAX_ICS + 1]; /* icid 0 is unused */

/* Incoming bytes from the client not yet parsed */
static unsigned char *in_buf;
static size_t in_len, in_size;

static void die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fputs("stubxim: ", stderr);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);
	exit(1);
}

/* Append a line to the root window's _STUBXIM_LOG */
static void log_line(const char *fmt, ...)
{
	char line[256];
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(line, sizeof line - 1, fmt, ap);
	va_end(ap);
	if (n < 0)
		return;
	if (n > (int)sizeof line - 2)
		n = sizeof line - 2;
	line[n++] = '\n';
	XChangeProperty(dpy, DefaultRootWindow(dpy), log_atom, XA_STRING, 8,
			PropModeAppend, (unsigned char *)line, n);
	XFlush(dpy);
}

/* ---- Packet building ---- */

struct packet {
	unsigned char data[1024];
	size_t len;
};

static void put8(struct packet *p, unsigned v)
{
	if (p->len < sizeof p->data)
		p->data[p->len++] = (unsigned char)v;
}

static void put16(struct packet *p, unsigned v)
{
	unsigned short s = (unsigned short)v;

	if (p->len + 2 <= sizeof p->data) {
		memcpy(p->data + p->len, &s, 2);
		p->len += 2;
	}
}

static void put32(struct packet *p, unsigned long v)
{
	unsigned int l = (unsigned int)v;

	if (p->len + 4 <= sizeof p->data) {
		memcpy(p->data + p->len, &l, 4);
		p->len += 4;
	}
}

static void put_bytes(struct packet *p, const void *b, size_t n)
{
	if (p->len + n <= sizeof p->data) {
		memcpy(p->data + p->len, b, n);
		p->len += n;
	}
}

static void pad4(struct packet *p)
{
	while (p->len % 4)
		put8(p, 0);
}

static void begin(struct packet *p, int opcode)
{
	p->len = 0;
	put8(p, opcode);
	put8(p, 0);
	put16(p, 0); /* length, set by send_packet */
}

/*
 * Send a packet to the client: in one ClientMessage when it fits, else
 * through a property on the client's window (transport version 0.0).
 */
static void send_packet(struct packet *p)
{
	static int seq;
	unsigned short len16;
	XEvent ev;

	pad4(p);
	len16 = (unsigned short)((p->len - 4) / 4);
	memcpy(p->data + 2, &len16, 2);

	memset(&ev, 0, sizeof ev);
	ev.xclient.type = ClientMessage;
	ev.xclient.window = client_window;
	ev.xclient.message_type = xim_protocol;
	if (p->len <= CM_DATA_SIZE) {
		ev.xclient.format = 8;
		memcpy(ev.xclient.data.b, p->data, p->len);
	} else {
		char name[32];
		Atom prop;

		snprintf(name, sizeof name, "_stubxim%d", seq);
		seq = (seq + 1) % 20;
		prop = XInternAtom(dpy, name, False);
		XChangeProperty(dpy, client_window, prop, XA_STRING, 8,
				PropModeAppend, p->data, (int)p->len);
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = (long)p->len;
		ev.xclient.data.l[1] = (long)prop;
	}
	XSendEvent(dpy, client_window, False, NoEventMask, &ev);
	XFlush(dpy);
}

static void reply_empty(int opcode)
{
	struct packet p;

	begin(&p, opcode);
	send_packet(&p);
}

/* A reply that holds only the IM ID, or the IM and IC IDs */
static void reply_ids(int opcode, int icid)
{
	struct packet p;

	begin(&p, opcode);
	put16(&p, IMID);
	if (icid >= 0)
		put16(&p, icid);
	send_packet(&p);
}

/* ---- Parsing ---- */

static unsigned get16(const unsigned char *b)
{
	unsigned short s;

	memcpy(&s, b, 2);
	return s;
}

static unsigned long get32(const unsigned char *b)
{
	unsigned int l;

	memcpy(&l, b, 4);
	return l;
}

static struct ic *find_ic(unsigned icid)
{
	if (icid == 0 || icid > MAX_ICS || !ics[icid].used)
		return NULL;
	return &ics[icid];
}

/*
 * Parse a list of IC attributes (ID, length, value, padding), as sent
 * by XIM_CREATE_IC and XIM_SET_IC_VALUES.  nest is A_PREEDIT_ATTR or
 * A_STATUS_ATTR inside a nested list, -1 at the top level.
 */
static void parse_ic_attrs(struct ic *ic, unsigned icid, const unsigned char *b,
			   size_t n, int nest)
{
	const char *what = nest == A_STATUS_ATTR ? "status" : "preedit";

	while (n >= 4) {
		unsigned id = get16(b), len = get16(b + 2);
		const unsigned char *v = b + 4;
		size_t step = 4 + len + ((4 - len % 4) % 4);

		if (step > n)
			break;
		switch (id) {
		case A_INPUT_STYLE:
			if (len >= 4)
				ic->style = get32(v);
			break;
		case A_CLIENT_WINDOW:
			if (len >= 4)
				log_line("client_window %u 0x%lx", icid, get32(v));
			break;
		case A_FOCUS_WINDOW:
			if (len >= 4)
				log_line("focus_window %u 0x%lx", icid, get32(v));
			break;
		case A_PREEDIT_ATTR:
		case A_STATUS_ATTR:
			parse_ic_attrs(ic, icid, v, len, (int)id);
			break;
		case A_SPOT_LOCATION:
			if (len >= 4)
				log_line("spot %u %d %d", icid, (short)get16(v),
				     (short)get16(v + 2));
			break;
		case A_AREA:
			if (len >= 8) {
				struct area a = { (short)get16(v), (short)get16(v + 2),
						  (unsigned short)get16(v + 4),
						  (unsigned short)get16(v + 6) };

				if (nest == A_STATUS_ATTR)
					ic->status_area = a;
				else
					ic->preedit_area = a;
				log_line("area %u %s %d %d %u %u", icid, what, a.x, a.y,
				     a.width, a.height);
			}
			break;
		case A_AREA_NEEDED:
			if (len >= 8)
				log_line("area_needed %u %s %u %u", icid, what,
				     get16(v + 4), get16(v + 6));
			break;
		case A_FOREGROUND:
			if (len >= 4)
				ic->fg = get32(v);
			break;
		case A_BACKGROUND:
			if (len >= 4)
				ic->bg = get32(v);
			break;
		case A_FONT_SET:
			if (len >= 2 && get16(v) <= len - 2)
				log_line("font_set %u %s %.*s", icid, what, (int)get16(v),
				     (const char *)v + 2);
			break;
		default:
			break;
		}
		b += step;
		n -= step;
	}
}

/* Append one attribute (ID, length, value, padding) to a reply */
static void put_attr_header(struct packet *p, unsigned id, unsigned len)
{
	put16(p, id);
	put16(p, len);
}

/*
 * The value of a nested attribute for XIM_GET_IC_VALUES: the IDs from
 * ids[0] up to the separator, written into p.  Returns the number of
 * IDs consumed, the separator included.
 */
static size_t put_nested_values(struct packet *p, struct ic *ic, int nest,
				const unsigned char *ids, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		unsigned id = get16(ids + 2 * i);
		struct area *a = nest == A_STATUS_ATTR ? &ic->status_area :
							 &ic->preedit_area;

		if (id == A_SEPARATOR)
			return i + 1;
		switch (id) {
		case A_AREA_NEEDED: {
			int status = nest == A_STATUS_ATTR;

			put_attr_header(p, id, 8);
			put16(p, 0);
			put16(p, 0);
			put16(p, status ? STATUS_WIDTH : 200);
			put16(p, AREA_HEIGHT);
			break;
		}
		case A_AREA:
			put_attr_header(p, id, 8);
			put16(p, (unsigned short)a->x);
			put16(p, (unsigned short)a->y);
			put16(p, a->width);
			put16(p, a->height);
			break;
		case A_FOREGROUND:
		case A_BACKGROUND:
			put_attr_header(p, id, 4);
			put32(p, id == A_FOREGROUND ? ic->fg : ic->bg);
			break;
		default:
			/* Not known: leave it out, the client reports it */
			break;
		}
	}
	return n;
}

static void get_ic_values(unsigned icid, const unsigned char *ids, size_t n)
{
	struct ic *ic = find_ic(icid);
	struct packet p;
	size_t i, list_start;
	unsigned short list_len;

	begin(&p, XIM_GET_IC_VALUES_REPLY);
	put16(&p, IMID);
	put16(&p, icid);
	put16(&p, 0); /* byte length of the list, set below */
	put16(&p, 0);
	list_start = p.len;
	for (i = 0; ic && i < n; i++) {
		unsigned id = get16(ids + 2 * i);

		switch (id) {
		case A_FILTER_EVENTS:
			put_attr_header(&p, id, 4);
			put32(&p, KeyPressMask);
			break;
		case A_RESET_STATE:
			/* A reset ends the preedit */
			put_attr_header(&p, id, 4);
			put32(&p, XIMInitialState);
			break;
		case A_INPUT_STYLE:
			put_attr_header(&p, id, 4);
			put32(&p, ic->style);
			break;
		case A_PREEDIT_ATTR:
		case A_STATUS_ATTR: {
			size_t head = p.len, used;
			unsigned short len;

			put_attr_header(&p, id, 0);
			used = put_nested_values(&p, ic, (int)id, ids + 2 * (i + 1),
						 n - i - 1);
			len = (unsigned short)(p.len - head - 4);
			memcpy(p.data + head + 2, &len, 2);
			i += used;
			break;
		}
		default:
			break;
		}
	}
	list_len = (unsigned short)(p.len - list_start);
	memcpy(p.data + 8, &list_len, 2);
	send_packet(&p);
}

/* ---- Preedit and commit ---- */

/* Romaji to hiragana: JIS X 0208 row 4 and U+3041.. are in step */
static const struct kana {
	const char *romaji;
	int index; /* from U+3041 / JIS 0x2421 */
} kana_table[] = {
	{ "ka", 0x0a }, { "ki", 0x0c }, { "ku", 0x0e }, { "ke", 0x10 }, { "ko", 0x12 },
	{ "ga", 0x0b }, { "gi", 0x0d }, { "gu", 0x0f }, { "ge", 0x11 }, { "go", 0x13 },
	{ "sa", 0x14 }, { "si", 0x16 }, { "su", 0x18 }, { "se", 0x1a }, { "so", 0x1c },
	{ "ta", 0x1e }, { "ti", 0x20 }, { "tu", 0x23 }, { "te", 0x25 }, { "to", 0x27 },
	{ "na", 0x29 }, { "ni", 0x2a }, { "nu", 0x2b }, { "ne", 0x2c }, { "no", 0x2d },
	{ "ha", 0x2e }, { "hi", 0x31 }, { "hu", 0x34 }, { "he", 0x37 }, { "ho", 0x3a },
	{ "ma", 0x3d }, { "mi", 0x3e }, { "mu", 0x3f }, { "me", 0x40 }, { "mo", 0x41 },
	{ "ya", 0x43 }, { "yu", 0x45 }, { "yo", 0x47 },
	{ "ra", 0x48 }, { "ri", 0x49 }, { "ru", 0x4a }, { "re", 0x4b }, { "ro", 0x4c },
	{ "wa", 0x4e }, { "wo", 0x51 },
	{ "a", 0x01 }, { "i", 0x03 }, { "u", 0x05 }, { "e", 0x07 }, { "o", 0x09 },
	{ "n", 0x52 },
};

/*
 * Convert ASCII romaji to compound text: hiragana in JIS X 0208 (GR),
 * anything that does not convert stays ASCII (GL).
 */
static size_t romaji_to_ct(const char *s, int n, unsigned char *out, size_t size)
{
	static const unsigned char jisx0208_gr[] = { 0x1b, '$', ')', 'B' };
	size_t o = 0;
	int designated = 0, i = 0;

	while (i < n && o + 6 < size) {
		const struct kana *k = NULL;
		size_t j;

		for (j = 0; j < sizeof kana_table / sizeof kana_table[0]; j++) {
			size_t l = strlen(kana_table[j].romaji);

			if (i + (int)l <= n && !strncmp(s + i, kana_table[j].romaji, l)) {
				k = &kana_table[j];
				break;
			}
		}
		if (!k) {
			out[o++] = (unsigned char)s[i++];
			continue;
		}
		if (!designated) {
			memcpy(out + o, jisx0208_gr, sizeof jisx0208_gr);
			o += sizeof jisx0208_gr;
			designated = 1;
		}
		out[o++] = 0x80 | 0x24;
		out[o++] = (unsigned char)(0x80 | (0x21 + k->index));
		i += (int)strlen(k->romaji);
	}
	return o;
}

/*
 * XIM_PREEDIT_DRAW: replace chg_length characters at chg_first with
 * len characters of text (none: delete), the caret at caret.
 */
static void preedit_draw(unsigned icid, int caret, int chg_first, int chg_length,
			 const char *text, int len)
{
	struct packet p;
	int i;

	begin(&p, XIM_PREEDIT_DRAW);
	put16(&p, IMID);
	put16(&p, icid);
	put32(&p, (unsigned long)caret);
	put32(&p, (unsigned long)chg_first);
	put32(&p, (unsigned long)chg_length);
	if (!text || !len) {
		put32(&p, 0x3); /* no string, no feedback */
		put16(&p, 0);
		put16(&p, 0);
		put16(&p, 0);
		put16(&p, 0);
	} else {
		put32(&p, 0);
		put16(&p, len);
		put_bytes(&p, text, len);
		pad4(&p); /* pad(2 + string length): the string starts at 26 */
		put16(&p, len * 4);
		put16(&p, 0);
		for (i = 0; i < len; i++)
			put32(&p, XIMUnderline);
	}
	send_packet(&p);
}

static void preedit_caret(unsigned icid, int position, XIMCaretDirection dir)
{
	struct packet p;

	begin(&p, XIM_PREEDIT_CARET);
	put16(&p, IMID);
	put16(&p, icid);
	put32(&p, (unsigned long)position);
	put32(&p, dir);
	put32(&p, XIMIsPrimary);
	send_packet(&p);
}

static int on_the_spot(const struct ic *ic)
{
	return (ic->style & XIMPreeditCallbacks) != 0;
}

/* End the preedit: erase it on the client and send XIM_PREEDIT_DONE */
static void preedit_end(unsigned icid, struct ic *ic)
{
	if (on_the_spot(ic) && ic->started) {
		if (ic->len)
			preedit_draw(icid, 0, 0, ic->len, NULL, 0);
		reply_ids(XIM_PREEDIT_DONE, (int)icid);
	}
	ic->started = ic->len = ic->caret = 0;
	ic->text[0] = '\0';
}

static void commit(unsigned icid, struct ic *ic)
{
	unsigned char ct[MAX_PREEDIT * 4 + 8];
	size_t n = romaji_to_ct(ic->text, ic->len, ct, sizeof ct);
	struct packet p;

	log_line("commit %u %s", icid, ic->text);
	preedit_end(icid, ic);
	begin(&p, XIM_COMMIT);
	put16(&p, IMID);
	put16(&p, icid);
	put16(&p, XIM_LOOKUP_CHARS);
	put16(&p, (unsigned)n);
	put_bytes(&p, ct, n);
	send_packet(&p);
}

/* Handle a key; returns 0 when it is not for the input method */
static int key(unsigned icid, struct ic *ic, KeySym sym)
{
	if (sym >= XK_a && sym <= XK_z) {
		char c = (char)('a' + (sym - XK_a));

		if (ic->len == MAX_PREEDIT)
			return 1;
		if (on_the_spot(ic) && !ic->started)
			reply_ids(XIM_PREEDIT_START, (int)icid);
		ic->started = 1;
		memmove(ic->text + ic->caret + 1, ic->text + ic->caret,
			(size_t)(ic->len - ic->caret));
		ic->text[ic->caret] = c;
		ic->text[++ic->len] = '\0';
		ic->caret++;
		if (on_the_spot(ic))
			preedit_draw(icid, ic->caret, ic->caret - 1, 0, &c, 1);
		return 1;
	}
	if (!ic->len)
		return 0;
	switch (sym) {
	case XK_Return:
	case XK_KP_Enter:
		commit(icid, ic);
		return 1;
	case XK_Escape:
		preedit_end(icid, ic);
		return 1;
	case XK_BackSpace:
		if (!ic->caret)
			return 1;
		memmove(ic->text + ic->caret - 1, ic->text + ic->caret,
			(size_t)(ic->len - ic->caret + 1));
		ic->len--;
		ic->caret--;
		if (on_the_spot(ic))
			preedit_draw(icid, ic->caret, ic->caret, 1, NULL, 0);
		if (!ic->len)
			preedit_end(icid, ic);
		return 1;
	case XK_Left:
	case XK_Right:
		if (sym == XK_Left && ic->caret > 0)
			ic->caret--;
		else if (sym == XK_Right && ic->caret < ic->len)
			ic->caret++;
		else
			return 1;
		if (on_the_spot(ic))
			preedit_caret(icid, ic->caret,
				      sym == XK_Left ? XIMBackwardChar : XIMForwardChar);
		return 1;
	default:
		/* Keep the preedit, let the client have the key */
		return 0;
	}
}

static void forward_event(unsigned icid, unsigned flag, const unsigned char *body,
			  size_t n)
{
	struct ic *ic = find_ic(icid);
	xEvent wire;
	XKeyEvent ev;
	KeySym sym = NoSymbol;
	char buf[8];
	int handled;

	if (n < 4 + sizeof wire)
		return;
	memcpy(&wire, body + 4, sizeof wire);
	if (ic && (wire.u.u.type & 0x7f) == KeyPress) {
		memset(&ev, 0, sizeof ev);
		ev.type = KeyPress;
		ev.display = dpy;
		ev.keycode = wire.u.u.detail;
		ev.state = wire.u.keyButtonPointer.state;
		XLookupString(&ev, buf, sizeof buf, &sym, NULL);
	}
	handled = ic && key(icid, ic, sym);
	if (!handled) {
		/* Not ours: send it back as it came */
		struct packet p;

		begin(&p, XIM_FORWARD_EVENT);
		put16(&p, IMID);
		put16(&p, icid);
		put16(&p, 0);
		put16(&p, get16(body + 2)); /* serial */
		put_bytes(&p, &wire, sizeof wire);
		send_packet(&p);
	}
	if (flag & XIM_SYNCHRONOUS)
		reply_ids(XIM_SYNC_REPLY, (int)icid);
	/* Logged last, so that a client which sees the line has been sent
	 * everything the key caused. */
	if (ic)
		log_line("key %u %s %s [%s] %d", icid, XKeysymToString(sym) ?
			 XKeysymToString(sym) : "NoSymbol",
			 handled ? "taken" : "returned", ic->text, ic->caret);
}

/*
 * XIM_RESET_IC returns the preedit string, unconverted, and clears the
 * preedit.  The client queues preedit callbacks while it waits for the
 * reply, so the preedit is erased after the reply.
 */
static void reset_ic(unsigned icid)
{
	struct ic *ic = find_ic(icid);
	struct packet p;
	size_t n = ic ? (size_t)ic->len : 0;

	begin(&p, XIM_RESET_IC_REPLY);
	put16(&p, IMID);
	put16(&p, icid);
	put16(&p, (unsigned)n);
	if (ic)
		put_bytes(&p, ic->text, n);
	send_packet(&p);
	if (ic) {
		log_line("reset_ic %u %s", icid, ic->text);
		preedit_end(icid, ic);
	}
}

/* ---- Requests ---- */

static void open_reply(void)
{
	struct packet p;
	size_t head;
	unsigned short len;
	int i;

	begin(&p, XIM_OPEN_REPLY);
	put16(&p, IMID);

	/* IM attributes */
	head = p.len;
	put16(&p, 0);
	put16(&p, IM_QUERY_INPUT_STYLE);
	put16(&p, T_STYLES);
	put16(&p, (unsigned)strlen(XNQueryInputStyle));
	put_bytes(&p, XNQueryInputStyle, strlen(XNQueryInputStyle));
	while ((p.len - head - 2) % 4) /* pad(2 + name length) */
		put8(&p, 0);
	len = (unsigned short)(p.len - head - 2);
	memcpy(p.data + head, &len, 2);

	/* IC attributes */
	head = p.len;
	put16(&p, 0);
	put16(&p, 0);
	for (i = 0; i < N_IC_ATTRS; i++) {
		size_t start = p.len;
		size_t name_len = strlen(ic_attrs[i].name);

		put16(&p, (unsigned)i);
		put16(&p, ic_attrs[i].type);
		put16(&p, (unsigned)name_len);
		put_bytes(&p, ic_attrs[i].name, name_len);
		while ((p.len - start - 4) % 4)
			put8(&p, 0);
	}
	len = (unsigned short)(p.len - head - 4);
	memcpy(p.data + head, &len, 2);
	send_packet(&p);

	/* Static event flow: the client forwards every KeyPress */
	begin(&p, XIM_SET_EVENT_MASK);
	put16(&p, IMID);
	put16(&p, 0);
	put32(&p, KeyPressMask);
	put32(&p, 0);
	send_packet(&p);
}

static void get_im_values(void)
{
	struct packet p;
	size_t i, n = sizeof styles / sizeof styles[0];

	begin(&p, XIM_GET_IM_VALUES_REPLY);
	put16(&p, IMID);
	put16(&p, (unsigned)(4 + 4 + 4 * n));
	put16(&p, IM_QUERY_INPUT_STYLE);
	put16(&p, (unsigned)(4 + 4 * n));
	put16(&p, (unsigned)n);
	put16(&p, 0);
	for (i = 0; i < n; i++)
		put32(&p, styles[i]);
	send_packet(&p);
}

static void create_ic(const unsigned char *body, size_t n)
{
	struct packet p;
	unsigned icid;

	for (icid = 1; icid <= MAX_ICS && ics[icid].used; icid++)
		;
	if (icid > MAX_ICS || n < 4) {
		begin(&p, XIM_ERROR);
		put16(&p, IMID);
		put16(&p, 0);
		put16(&p, 1); /* imid valid */
		put16(&p, 1); /* BadAlloc */
		put16(&p, 0);
		put16(&p, 0);
		send_packet(&p);
		return;
	}
	memset(&ics[icid], 0, sizeof ics[icid]);
	ics[icid].used = 1;
	parse_ic_attrs(&ics[icid], icid, body + 4,
		       n - 4 < get16(body + 2) ? n - 4 : get16(body + 2), -1);
	log_line("create_ic %u 0x%lx", icid, ics[icid].style);
	reply_ids(XIM_CREATE_IC_REPLY, (int)icid);
}

static void request(const unsigned char *pkt, size_t n)
{
	const unsigned char *body = pkt + 4;
	size_t blen = n - 4;
	unsigned icid = blen >= 4 ? get16(body + 2) : 0;
	struct ic *ic;

	switch (pkt[0]) {
	case XIM_CONNECT: {
		struct packet p;
		unsigned short probe = 1;
		unsigned char native = *(unsigned char *)&probe ? 0x6c : 0x42;

		if (blen < 1 || body[0] != native)
			die("the client's byte order is not this server's");
		log_line("connect");
		begin(&p, XIM_CONNECT_REPLY);
		put16(&p, 1);
		put16(&p, 0);
		send_packet(&p);
		break;
	}
	case XIM_DISCONNECT:
		log_line("disconnect");
		memset(ics, 0, sizeof ics);
		reply_empty(XIM_DISCONNECT_REPLY);
		break;
	case XIM_OPEN:
		if (blen >= 1 && body[0] < blen)
			log_line("open %.*s", (int)body[0], (const char *)body + 1);
		open_reply();
		break;
	case XIM_CLOSE:
		log_line("close");
		reply_ids(XIM_CLOSE_REPLY, 0);
		break;
	case XIM_QUERY_EXTENSION: {
		struct packet p;

		begin(&p, XIM_QUERY_EXTENSION_REPLY);
		put16(&p, IMID);
		put16(&p, 0); /* no extensions */
		send_packet(&p);
		break;
	}
	case XIM_ENCODING_NEGOTIATION: {
		/* Pick COMPOUND_TEXT, the only one Xlib can use */
		struct packet p;
		unsigned i = 0, idx = 0;
		size_t names = blen >= 4 ? get16(body + 2) : 0;
		const unsigned char *s = body + 4;

		while (names > 0 && names <= blen - 4) {
			unsigned l = s[0];

			if (l + 1u > names)
				break;
			if (l == 13 && !memcmp(s + 1, "COMPOUND_TEXT", 13)) {
				idx = i;
				break;
			}
			names -= l + 1;
			s += l + 1;
			i++;
		}
		begin(&p, XIM_ENCODING_NEGOTIATION_REPLY);
		put16(&p, IMID);
		put16(&p, 0); /* name category */
		put16(&p, idx);
		put16(&p, 0);
		send_packet(&p);
		break;
	}
	case XIM_GET_IM_VALUES:
		get_im_values();
		break;
	case XIM_SET_IM_VALUES:
		reply_ids(XIM_SET_IM_VALUES_REPLY, 0);
		break;
	case XIM_CREATE_IC:
		create_ic(body, blen);
		break;
	case XIM_DESTROY_IC:
		log_line("destroy_ic %u", icid);
		if (find_ic(icid))
			ics[icid].used = 0;
		reply_ids(XIM_DESTROY_IC_REPLY, (int)icid);
		break;
	case XIM_SET_IC_VALUES:
		if ((ic = find_ic(icid)) && blen >= 8) {
			size_t len = get16(body + 4);

			log_line("set_ic_values %u", icid);
			parse_ic_attrs(ic, icid, body + 8,
				       len < blen - 8 ? len : blen - 8, -1);
		}
		reply_ids(XIM_SET_IC_VALUES_REPLY, (int)icid);
		break;
	case XIM_GET_IC_VALUES:
		if (blen >= 6) {
			size_t len = get16(body + 4);

			if (len > blen - 6)
				len = blen - 6;
			get_ic_values(icid, body + 6, len / 2);
		}
		break;
	case XIM_SET_IC_FOCUS:
		log_line("focus %u", icid);
		break;
	case XIM_UNSET_IC_FOCUS:
		log_line("unfocus %u", icid);
		break;
	case XIM_FORWARD_EVENT:
		if (blen >= 8)
			forward_event(icid, get16(body + 4), body + 4, blen - 4);
		break;
	case XIM_SYNC:
		reply_ids(XIM_SYNC_REPLY, (int)icid);
		break;
	case XIM_RESET_IC:
		reset_ic(icid);
		break;
	case XIM_PREEDIT_START_REPLY:
		if (blen >= 8)
			log_line("start_reply %u %ld", icid, (long)(int)get32(body + 4));
		break;
	case XIM_PREEDIT_CARET_REPLY:
		if (blen >= 8)
			log_line("caret_reply %u %lu", icid, get32(body + 4));
		break;
	case XIM_ERROR:
		log_line("error %u", blen >= 8 ? get16(body + 6) : 0);
		break;
	default:
		break;
	}
}

/* Parse the complete packets in in_buf */
static void parse_input(void)
{
	size_t off = 0;

	for (;;) {
		size_t plen;

		/* A short packet arrives zero padded to 20 bytes */
		while (off < in_len && in_buf[off] == 0)
			off++;
		if (in_len - off < 4)
			break;
		plen = 4 + 4 * (size_t)get16(in_buf + off + 2);
		if (in_len - off < plen)
			break;
		request(in_buf + off, plen);
		off += plen;
	}
	memmove(in_buf, in_buf + off, in_len - off);
	in_len -= off;
}

static void add_input(const unsigned char *b, size_t n)
{
	if (in_len + n > in_size) {
		in_size = (in_len + n) * 2;
		if (!(in_buf = realloc(in_buf, in_size)))
			die("out of memory");
	}
	memcpy(in_buf + in_len, b, n);
	in_len += n;
}

static void client_message(XClientMessageEvent *cm)
{
	if (cm->window == server_window && cm->message_type == xim_xconnect) {
		XEvent ev;

		/* One client at a time: a new one replaces the old */
		if (comm_window)
			XDestroyWindow(dpy, comm_window);
		memset(ics, 0, sizeof ics);
		in_len = 0;
		client_window = (Window)cm->data.l[0];
		comm_window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
						  1, 1, 0, 0, 0);
		memset(&ev, 0, sizeof ev);
		ev.xclient.type = ClientMessage;
		ev.xclient.window = client_window;
		ev.xclient.message_type = xim_xconnect;
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = (long)comm_window;
		ev.xclient.data.l[1] = 0; /* transport version 0.0 */
		ev.xclient.data.l[2] = 0;
		XSendEvent(dpy, client_window, False, NoEventMask, &ev);
		XFlush(dpy);
		return;
	}
	if (cm->window != comm_window ||
	    (cm->message_type != xim_protocol && cm->message_type != xim_moredata))
		return;
	if (cm->format == 8) {
		add_input((const unsigned char *)cm->data.b, CM_DATA_SIZE);
	} else if (cm->format == 32) {
		Atom type;
		int format;
		unsigned long nitems, after;
		unsigned char *data = NULL;

		if (XGetWindowProperty(dpy, comm_window, (Atom)cm->data.l[1], 0,
				       0x10000, True, AnyPropertyType, &type, &format,
				       &nitems, &after, &data) == Success && data) {
			if (format == 8)
				add_input(data, nitems);
			XFree(data);
		}
	}
	if (cm->message_type == xim_protocol)
		parse_input();
}

static void selection_request(XSelectionRequestEvent *req)
{
	XEvent ev;
	char buf[256];
	const char *value = NULL;

	if (req->target == locales_atom) {
		/* Xlib matches language_territory.codeset, language.codeset,
		 * language_territory and language against this list. */
		const char *lc = getenv("STUBXIM_LOCALES");

		snprintf(buf, sizeof buf, "@locale=%s",
			 lc && *lc ? lc : "C,POSIX,en,ja,de,he,ar,ko,zh");
		value = buf;
	} else if (req->target == transport_atom) {
		value = "@transport=X/";
	}
	memset(&ev, 0, sizeof ev);
	ev.xselection.type = SelectionNotify;
	ev.xselection.requestor = req->requestor;
	ev.xselection.selection = req->selection;
	ev.xselection.target = req->target;
	ev.xselection.time = req->time;
	ev.xselection.property = None;
	if (value && req->property != None) {
		XChangeProperty(dpy, req->requestor, req->property, req->target, 8,
				PropModeReplace, (const unsigned char *)value,
				(int)strlen(value));
		ev.xselection.property = req->property;
	}
	XSendEvent(dpy, req->requestor, False, NoEventMask, &ev);
	XFlush(dpy);
}

/* Add or remove server_atom in the root window's XIM_SERVERS */
static void register_server(int add)
{
	Atom type, *atoms = NULL, *list;
	int format;
	unsigned long n = 0, after, i, j = 0;
	unsigned char *data = NULL;

	XGrabServer(dpy);
	if (XGetWindowProperty(dpy, DefaultRootWindow(dpy), xim_servers, 0, 1024,
			       False, XA_ATOM, &type, &format, &n, &after,
			       &data) != Success || type != XA_ATOM || format != 32)
		n = 0;
	atoms = (Atom *)data;
	if (!(list = calloc(n + 1, sizeof *list)))
		die("out of memory");
	for (i = 0; i < n; i++)
		if (atoms[i] != server_atom)
			list[j++] = atoms[i];
	if (add)
		list[j++] = server_atom;
	XChangeProperty(dpy, DefaultRootWindow(dpy), xim_servers, XA_ATOM, 32,
			PropModeReplace, (unsigned char *)list, (int)j);
	XUngrabServer(dpy);
	XFlush(dpy);
	free(list);
	if (data)
		XFree(data);
}

int main(int argc, char *argv[])
{
	char name[128];
	struct pollfd fds[2];

	if (argc != 2)
		die("usage: stubxim NAME");
	if (!(dpy = XOpenDisplay(NULL)))
		die("cannot open the display");

	snprintf(name, sizeof name, "@server=%s", argv[1]);
	server_atom = XInternAtom(dpy, name, False);
	xim_servers = XInternAtom(dpy, "XIM_SERVERS", False);
	locales_atom = XInternAtom(dpy, "LOCALES", False);
	transport_atom = XInternAtom(dpy, "TRANSPORT", False);
	xim_xconnect = XInternAtom(dpy, "_XIM_XCONNECT", False);
	xim_protocol = XInternAtom(dpy, "_XIM_PROTOCOL", False);
	xim_moredata = XInternAtom(dpy, "_XIM_MOREDATA", False);
	log_atom = XInternAtom(dpy, "_STUBXIM_LOG", False);

	XDeleteProperty(dpy, DefaultRootWindow(dpy), log_atom);
	server_window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1, 1,
					    0, 0, 0);
	XSetSelectionOwner(dpy, server_atom, server_window, CurrentTime);
	if (XGetSelectionOwner(dpy, server_atom) != server_window)
		die("cannot own %s", name);
	register_server(1);

	printf("ready\n");
	fflush(stdout);

	fds[0].fd = ConnectionNumber(dpy);
	fds[0].events = POLLIN;
	fds[1].fd = STDIN_FILENO;
	fds[1].events = POLLIN;
	for (;;) {
		XEvent ev;

		while (XPending(dpy)) {
			XNextEvent(dpy, &ev);
			switch (ev.type) {
			case ClientMessage:
				client_message(&ev.xclient);
				break;
			case SelectionRequest:
				selection_request(&ev.xselectionrequest);
				break;
			case SelectionClear:
				register_server(0);
				return 0;
			default:
				break;
			}
		}
		if (poll(fds, 2, -1) < 0 && errno != EINTR)
			die("poll: %s", strerror(errno));
		if (fds[1].revents & (POLLIN | POLLHUP | POLLERR)) {
			char c;

			/* End of file: whoever started the server is gone */
			if (read(STDIN_FILENO, &c, 1) <= 0) {
				register_server(0);
				XCloseDisplay(dpy);
				return 0;
			}
		}
	}
}
