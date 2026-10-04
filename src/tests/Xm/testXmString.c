/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Headless XmString tests: creation, comparison, concatenation, the
 * ASN.1 byte stream, XmStringToXmStringTable / XmStringTableToXmString,
 * parse tables and compound text.  None of this needs an X server.
 *
 * Most cases are regression tests for specific fixes and say which;
 * they are meant to be run under ASan and UBSan as well, where the
 * memory errors those fixes removed would be reported.
 */
#include <langinfo.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmStringI.h>
#include <check.h>

#include "leak.h"
#include "suites.h"

#define MAX_COMPONENTS 1024

/* A string's components as returned by XmStringGetNextTriple */
struct comps {
	int n;
	XmStringComponentType type[MAX_COMPONENTS];
	char *value[MAX_COMPONENTS];	/* text and tags as C strings */
};

static void get_comps(XmString s, struct comps *c)
{
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len;
	XtPointer val;

	memset(c, 0, sizeof *c);
	ck_assert(XmStringInitContext(&ctx, s));
	while ((t = XmStringGetNextTriple(ctx, &len, &val)) !=
	       XmSTRING_COMPONENT_END) {
		ck_assert_int_lt(c->n, MAX_COMPONENTS);
		c->type[c->n] = t;
		switch (t) {
		case XmSTRING_COMPONENT_TEXT:
		case XmSTRING_COMPONENT_LOCALE_TEXT:
		case XmSTRING_COMPONENT_TAG:
		case XmSTRING_COMPONENT_RENDITION_BEGIN:
		case XmSTRING_COMPONENT_RENDITION_END:
			c->value[c->n] = XtMalloc(len + 1);
			if (len)
				memcpy(c->value[c->n], val, len);
			c->value[c->n][len] = '\0';
			break;
		default:
			break;
		}
		XtFree((char *)val);
		c->n++;
	}
	XmStringFreeContext(ctx);
}

static void free_comps(struct comps *c)
{
	int i;

	for (i = 0; i < c->n; i++)
		XtFree(c->value[i]);
	c->n = 0;
}

/* Number of components of type t in s */
static int count_comps(XmString s, XmStringComponentType t)
{
	struct comps c;
	int i, n = 0;

	get_comps(s, &c);
	for (i = 0; i < c.n; i++)
		if (c.type[i] == t)
			n++;
	free_comps(&c);
	return n;
}

/* The text of s, without separators or tabs */
static char *text_of(XmString s)
{
	return (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       NULL, 0, XmOUTPUT_ALL);
}

static void assert_text(XmString s, const char *expect)
{
	char *t = text_of(s);

	ck_assert_ptr_nonnull(t);
	ck_assert_str_eq(t, expect);
	XtFree(t);
}

static XmString tagged(const char *text, const char *tag)
{
	return XmStringGenerate((XtPointer)text, (XmStringTag)tag,
				XmCHARSET_TEXT, NULL);
}

static XmString sep(void)
{
	return XmStringSeparatorCreate();
}

static XmString tab(void)
{
	return XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
}

/* a + b + ..., freeing the arguments; the list ends with NULL */
static XmString cat(XmString first, ...)
{
	va_list ap;
	XmString s = first, next;

	va_start(ap, first);
	while ((next = va_arg(ap, XmString)) != NULL)
		s = XmStringConcatAndFree(s, next);
	va_end(ap);
	return s;
}

/* Whether a and b have the same components with the same values */
static int same_comps(XmString a, XmString b)
{
	struct comps ca, cb;
	int i, same;

	get_comps(a, &ca);
	get_comps(b, &cb);
	same = (ca.n == cb.n);
	for (i = 0; same && i < ca.n; i++) {
		same = (ca.type[i] == cb.type[i]) &&
		       (!ca.value[i] == !cb.value[i]) &&
		       (!ca.value[i] || !strcmp(ca.value[i], cb.value[i]));
	}
	free_comps(&ca);
	free_comps(&cb);
	return same;
}

/* What assert_byte_stream_round_trip() checks of the result */
#define SAME_COMPS 1	/* the same components */
#define SAME_STRING 2	/* XmStringCompare() */

/* Round trip s through the byte stream and check the result */
static void assert_byte_stream_round_trip(XmString s, int check)
{
	unsigned char *stream = NULL;
	unsigned int len;
	XmString back;

	len = XmCvtXmStringToByteStream(s, &stream);
	ck_assert_uint_gt(len, 0);
	ck_assert_ptr_nonnull(stream);
	ck_assert_uint_eq(XmStringByteStreamLength(stream), len);
	ck_assert_uint_eq(_XmStringByteStreamValidLength(stream, len), len);
	ck_assert_uint_eq(_XmStringByteStreamValidLength(stream, len - 1), 0);

	back = XmCvtByteStreamToXmString(stream);
	ck_assert_ptr_nonnull(back);
	if (check & SAME_COMPS)
		ck_assert_msg(same_comps(s, back),
			      "components changed in the byte stream round trip");
	if (check & SAME_STRING)
		ck_assert_msg(XmStringCompare(s, back),
			      "XmStringCompare: string changed in the byte "
			      "stream round trip");
	XmStringFree(back);
	XtFree((char *)stream);
}

/* A byte stream from a body of components: header plus body */
static unsigned char *make_stream(const unsigned char *body, size_t len,
				  size_t *total)
{
	unsigned char *s = (unsigned char *)XtMalloc(len + 6);
	size_t h;

	s[0] = 0xdf;
	s[1] = 0x80;
	s[2] = 0x06;
	if (len < 128) {
		s[3] = (unsigned char)len;
		h = 4;
	} else {
		s[3] = 0x82;
		s[4] = (unsigned char)(len >> 8);
		s[5] = (unsigned char)len;
		h = 6;
	}
	memcpy(s + h, body, len);
	*total = h + len;
	return s;
}

/* Convert a copy of buf that is exactly len bytes, so that ASan sees
 * any read past it. */
static XmString cvt_exact(const unsigned char *buf, size_t len)
{
	unsigned char *copy = (unsigned char *)malloc(len ? len : 1);
	XmString s;

	memcpy(copy, buf, len);
	s = XmCvtByteStreamToXmString(copy);
	free(copy);
	return s;
}

/*
 * Basics
 */

START_TEST(create_and_compare)
{
	XmString a = XmStringCreateLocalized("hello");
	XmString b = XmStringCreateLocalized("hello");
	XmString c = XmStringCreateLocalized("world");
	XmString d = XmStringCopy(a);

	ck_assert(XmStringCompare(a, b));
	ck_assert(XmStringCompare(a, d));
	ck_assert(!XmStringCompare(a, c));
	ck_assert(!XmStringEmpty(a));
	ck_assert_int_eq(XmStringLength(a), XmStringLength(b));
	ck_assert(XmStringHasSubstring(a, b));
	assert_text(d, "hello");
	XmStringFree(a);
	XmStringFree(b);
	XmStringFree(c);
	XmStringFree(d);
}
END_TEST

START_TEST(empty_strings)
{
	XmString e = XmStringCreateLocalized("");
	XmString n = XmStringComponentCreate(XmSTRING_COMPONENT_TEXT, 0, NULL);

	ck_assert(XmStringEmpty(e));
	ck_assert(XmStringEmpty(NULL));
	ck_assert_ptr_nonnull(n);
	ck_assert(XmStringEmpty(n));
	XmStringFree(e);
	XmStringFree(n);
}
END_TEST

START_TEST(concat_text)
{
	XmString s = cat(XmStringCreateLocalized("ab"),
			 XmStringCreateLocalized("cd"), NULL);
	XmString t = XmStringConcat(s, s);

	assert_text(s, "abcd");
	assert_text(t, "abcdabcd");
	XmStringFree(s);
	XmStringFree(t);
}
END_TEST

/* n copies of piece, as a C string */
static char *repeat(const char *piece, int n)
{
	size_t len = strlen(piece);
	char *s = XtMalloc(len * n + 1);
	int i;

	for (i = 0; i < n; i++)
		memcpy(s + i * len, piece, len);
	s[len * n] = '\0';
	return s;
}

/*
 * Concatenating text onto an optimized segment past 255 bytes wrapped
 * its 8-bit byte count: 33 pieces of 8 bytes came out as 8 bytes.  The
 * merged text must stay one text component, also on the last line of
 * a multi-line string and with a tag.
 */
START_TEST(concat_long_text)
{
	static const char *const tags[] = { NULL, "tag1" };
	int t, n, lines;

	for (t = 0; t < 2; t++)
		for (lines = 1; lines <= 2; lines++)
			for (n = 30; n <= 100; n += 7) {
				XmString s = NULL;
				char *expect = repeat("segment ", n);
				char *first = NULL, *all;
				int i;

				if (lines == 2) {
					s = cat(XmStringCreateLocalized("first"),
						sep(), NULL);
					first = "first"; /* text_of() drops separators */
				}
				for (i = 0; i < n; i++)
					s = XmStringConcatAndFree(s,
						tags[t] ? tagged("segment ", tags[t]) :
							  XmStringCreateLocalized("segment "));
				all = XtMalloc(strlen(expect) + 7);
				sprintf(all, "%s%s", first ? first : "", expect);
				assert_text(s, all);
				ck_assert_int_eq(XmStringLineCount(s), lines);
				ck_assert_int_eq(count_comps(s, XmSTRING_COMPONENT_TEXT) +
						 count_comps(s, XmSTRING_COMPONENT_LOCALE_TEXT),
						 lines);
				assert_byte_stream_round_trip(s, SAME_COMPS | SAME_STRING);
				XtFree(all);
				XtFree(expect);
				XmStringFree(s);
			}
}
END_TEST

/* The direction of the last segment of s whose direction is set */
static XmStringDirection scan_last_dir(XmString s)
{
	int i, j;

	for (i = _XmStrEntryCount(s); i > 0; i--) {
		_XmStringEntry line = _XmStrEntry(s)[i - 1];

		for (j = _XmEntrySegmentCountGet(line); j > 0; j--) {
			_XmStringEntry seg =
				(_XmStringEntry)_XmEntrySegmentGet(line)[j - 1];

			/* _XmEntryDirectionGet is not exported */
			unsigned int dir = _XmEntryOptimized(seg) ?
						   seg->single.str_dir :
						   seg->unopt_single.str_dir;

			if (dir != XmSTRING_DIRECTION_UNSET)
				return dir;
		}
	}
	return XmSTRING_DIRECTION_UNSET;
}

/*
 * XmStringConcatAndFree keeps the last direction set in the header of
 * the string it builds, rather than search the whole string for it
 * when a line is empty.  Whatever the pieces, it must be what that
 * search finds.
 */
START_TEST(concat_last_direction)
{
	static const XmStringDirection dirs[] = {
		XmSTRING_DIRECTION_L_TO_R, XmSTRING_DIRECTION_R_TO_L,
		XmSTRING_DIRECTION_UNSET
	};
	unsigned int rng = 12345;
	int round, i;

	for (round = 0; round < 200; round++) {
		XmString s = NULL, piece;

		for (i = 0; i < 40; i++) {
			rng = rng * 1103515245 + 12345;
			switch ((rng >> 16) % 7) {
			case 0:
				piece = XmStringDirectionCreate(
					dirs[(rng >> 8) % 3]);
				break;
			case 1:
			case 2:
				piece = sep();
				break;
			case 3:
				piece = XmStringCreate("x", "tagA");
				break;
			case 4:
				piece = cat(sep(), XmStringCreate("y", "tagB"),
					    sep(), NULL);
				break;
			case 5:
				piece = tab();
				break;
			default:
				piece = XmStringCreateLocalized("z");
				break;
			}
			if ((rng >> 24) % 5 == 0)
				s = XmStringConcatAndFree(piece, s);
			else
				s = XmStringConcatAndFree(s, piece);
			if (_XmStrMultiple(s) && _XmStrLastDirKnown(s))
				ck_assert_uint_eq(_XmStrLastDir(s),
						  scan_last_dir(s));
		}
		XmStringFree(s);
	}
}
END_TEST

/* The text of s with its separators as newlines */
static char *unparse_lines(XmString s)
{
	XmString nl = sep();
	XmParseMapping map;
	Arg args[3];
	char *text;

	XtSetArg(args[0], XmNpattern, "\n");
	XtSetArg(args[1], XmNsubstitute, nl);
	XtSetArg(args[2], XmNincludeStatus, XmINSERT);
	map = XmParseMappingCreate(args, 3);
	text = (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       &map, 1, XmOUTPUT_ALL);
	XmParseMappingFree(map);
	XmStringFree(nl);
	return text;
}

/*
 * Strings built from many pieces: the arrays and the text grow
 * geometrically.  Whichever way a string is built, it must come out
 * the same.
 */
START_TEST(build_many_pieces)
{
	enum { LINES = 20000 };
	char *text = XtMalloc(LINES * 8 + 1), *p = text, *got;
	XmString parsed, built = NULL, line;
	int i;

	for (i = 0; i < LINES; i++)
		p += sprintf(p, "l%05d%s", i, i + 1 < LINES ? "\n" : "");
	parsed = XmStringGenerate(text, NULL, XmCHARSET_TEXT, NULL);
	ck_assert_int_eq(XmStringLineCount(parsed), LINES);
	for (i = 0; i < LINES; i++) {
		char buf[8];

		snprintf(buf, sizeof buf, "l%05d", i);
		line = XmStringGenerate(buf, NULL, XmCHARSET_TEXT, NULL);
		built = XmStringConcatAndFree(built, line);
		if (i + 1 < LINES)
			built = XmStringConcatAndFree(built, sep());
	}
	ck_assert(XmStringCompare(parsed, built));
	got = unparse_lines(parsed);
	ck_assert_str_eq(got, text);
	XtFree(got);
	got = unparse_lines(built);
	ck_assert_str_eq(got, text);
	XtFree(got);
	XmStringFree(parsed);
	XmStringFree(built);

	/* One segment that text is appended to, 64 KiB of it */
	built = NULL;
	for (i = 0; i < 8192; i++)
		built = XmStringConcatAndFree(built,
			XmStringCreate("abcdefgh", "tagA"));
	ck_assert_int_eq(count_comps(built, XmSTRING_COMPONENT_TEXT), 1);
	got = text_of(built);
	ck_assert_uint_eq(strlen(got), 8192 * 8);
	for (i = 0; i < 8192; i++)
		ck_assert(!memcmp(got + i * 8, "abcdefgh", 8));
	XtFree(got);
	XmStringFree(built);
	XtFree(text);
}
END_TEST

/*
 * XmStringCopy shares the string.  The reference counts were 6 bits
 * (optimized strings) and 8 bits wide, so every 64th or 256th copy was
 * a full clone.
 */
START_TEST(copy_shares)
{
	XmString s[2], copies[1000];
	int i, j;

	s[0] = XmStringCreateLocalized("The quick brown fox");
	s[1] = cat(XmStringCreateLocalized("one"), sep(),
		   XmStringCreateLocalized("two"), NULL);
	ck_assert(_XmStrOptimized(s[0]));
	ck_assert(_XmStrMultiple(s[1]));
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 1000; i++) {
			copies[i] = XmStringCopy(s[j]);
			ck_assert_ptr_eq(copies[i], s[j]);
		}
		ck_assert_uint_eq(_XmStrRefCountGet(s[j]), 1001);
		for (i = 0; i < 1000; i++)
			XmStringFree(copies[i]);
		ck_assert_uint_eq(_XmStrRefCountGet(s[j]), 1);
	}
	assert_text(s[0], "The quick brown fox");
	assert_text(s[1], "onetwo");
	XmStringFree(s[0]);
	XmStringFree(s[1]);
}
END_TEST

START_TEST(line_count)
{
	XmString s = cat(XmStringCreateLocalized("one"), sep(),
			 XmStringCreateLocalized("two"), sep(),
			 XmStringCreateLocalized("three"), NULL);

	ck_assert_int_eq(XmStringLineCount(s), 3);
	ck_assert_int_eq(count_comps(s, XmSTRING_COMPONENT_SEPARATOR), 2);
	XmStringFree(s);
}
END_TEST

/*
 * 57011cc7: "a" followed by a tab must stay text, tab; the optimized
 * concatenation merged the tab in front of the text.
 */
START_TEST(concat_keeps_tab_after_text)
{
	XmString s = cat(XmStringCreateLocalized("a"), tab(), NULL);
	struct comps c;
	int i, text = -1, tabpos = -1;

	get_comps(s, &c);
	for (i = 0; i < c.n; i++) {
		if (c.type[i] == XmSTRING_COMPONENT_TEXT ||
		    c.type[i] == XmSTRING_COMPONENT_LOCALE_TEXT)
			text = i;
		else if (c.type[i] == XmSTRING_COMPONENT_TAB)
			tabpos = i;
	}
	ck_assert_int_ge(text, 0);
	ck_assert_int_ge(tabpos, 0);
	ck_assert_int_lt(text, tabpos);
	free_comps(&c);
	XmStringFree(s);
}
END_TEST

/*
 * 14bd06da, 562299d2: empty text has a NULL value; copying,
 * comparing and concatenating it must not pass NULL to memcpy or
 * strncmp (UBSan reports that).
 */
START_TEST(empty_text_component)
{
	XmString e1 = XmStringComponentCreate(XmSTRING_COMPONENT_TEXT, 0, NULL);
	XmString e2 = XmStringComponentCreate(XmSTRING_COMPONENT_TEXT, 0, NULL);
	XmString a = XmStringCreateLocalized("a");
	XmString s, t;
	struct comps c;

	ck_assert(XmStringCompare(e1, e2));
	s = XmStringConcat(e1, a);
	t = XmStringConcat(a, e1);
	assert_text(s, "a");
	assert_text(t, "a");
	get_comps(e1, &c);
	free_comps(&c);
	/* Reading it back adds the default tag */
	assert_byte_stream_round_trip(e1, SAME_STRING);
	XmStringFree(e1);
	XmStringFree(e2);
	XmStringFree(a);
	XmStringFree(s);
	XmStringFree(t);
}
END_TEST

/*
 * dae6c9a0: the same text held as a single segment line and as an
 * array line of one segment compares equal.
 */
START_TEST(compare_single_and_array_lines)
{
	XmString plain = tagged("x", "T");
	XmString built, pieces_str, brk;
	XmStringTable table;
	Cardinal n;

	/* A multi-segment first line, so the string is not optimized */
	built = cat(tagged("x", "T"), sep(), tagged("y", "U"), NULL);
	brk = sep();
	n = XmStringToXmStringTable(built, brk, &table);
	ck_assert_uint_eq(n, 2);
	ck_assert(XmStringCompare(table[0], plain));
	pieces_str = XmStringTableToXmString(table, n, brk);
	ck_assert(XmStringCompare(pieces_str, built));

	while (n--)
		XmStringFree(table[n]);
	XtFree((char *)table);
	XmStringFree(pieces_str);
	XmStringFree(built);
	XmStringFree(brk);
	XmStringFree(plain);
}
END_TEST

/*
 * The byte stream
 */

START_TEST(byte_stream_round_trip)
{
	char long_text[300];
	XmString s;

	memset(long_text, 'x', sizeof long_text - 1);
	long_text[sizeof long_text - 1] = '\0';

	s = XmStringCreateLocalized("hello");
	assert_byte_stream_round_trip(s, SAME_COMPS | SAME_STRING);
	XmStringFree(s);

	/* Long-form lengths (>= 128) for the component and the stream */
	s = XmStringCreateLocalized(long_text);
	assert_byte_stream_round_trip(s, SAME_COMPS | SAME_STRING);
	XmStringFree(s);

	/* Several lines, tags and tabs */
	s = cat(tagged("one", "A"), tab(), tagged("two", "B"), sep(),
		tagged("three", "A"), NULL);
	assert_byte_stream_round_trip(s, SAME_COMPS | SAME_STRING);
	XmStringFree(s);

	/* Direction and layout push/pop */
	s = cat(XmStringDirectionCreate(XmSTRING_DIRECTION_R_TO_L),
		XmStringCreateLocalized("rtl"),
		XmStringComponentCreate(XmSTRING_COMPONENT_LAYOUT_POP, 0, NULL),
		NULL);
	assert_byte_stream_round_trip(s, SAME_COMPS | SAME_STRING);
	XmStringFree(s);
}
END_TEST

/*
 * e41107cc: a byte stream with several rendition begin and end tags
 * on one segment goes through _XmStringNonOptCreate, which sized the
 * tag arrays in bytes rather than pointers.  XmStringUnparse on such
 * a string double freed the context's rendition list.
 */
START_TEST(byte_stream_renditions)
{
	XmString s;
	char *t;

	s = cat(XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
					2, "r1"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
					2, "r2"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
					2, "r3"),
		XmStringCreateLocalized("text"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
					2, "r3"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
					2, "r2"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
					2, "r1"),
		NULL);
	/* XmStringCompare() would fail, see compare_ignores_segmentation */
	assert_byte_stream_round_trip(s, SAME_COMPS);
	ck_assert_int_eq(count_comps(s, XmSTRING_COMPONENT_RENDITION_BEGIN), 3);
	t = text_of(s);
	ck_assert_str_eq(t, "text");
	XtFree(t);
	XmStringFree(s);
}
END_TEST

/*
 * 1029786d: the parser must stay inside the stream.  Each of these is
 * malformed and must give NULL without reading past the buffer.
 */
START_TEST(byte_stream_malformed)
{
	static const struct {
		const char *what;
		unsigned char bytes[16];
		size_t len;
	} bad[] = {
		{ "component longer than the stream",
		  { 0xdf, 0x80, 0x06, 0x04, XmSTRING_COMPONENT_TEXT, 0x7f,
		    'a', 'b' }, 8 },
		{ "long component length past the end",
		  { 0xdf, 0x80, 0x06, 0x05, XmSTRING_COMPONENT_TEXT, 0x82,
		    0xff, 0xff, 'a' }, 9 },
		{ "truncated component header",
		  { 0xdf, 0x80, 0x06, 0x02, XmSTRING_COMPONENT_TEXT, 0x82 },
		  6 },
		{ "empty direction",
		  { 0xdf, 0x80, 0x06, 0x02, XmSTRING_COMPONENT_DIRECTION,
		    0x00 }, 6 },
		{ "empty layout push",
		  { 0xdf, 0x80, 0x06, 0x02, XmSTRING_COMPONENT_LAYOUT_PUSH,
		    0x00 }, 6 },
	};
	size_t i;

	for (i = 0; i < sizeof bad / sizeof bad[0]; i++) {
		XmString s = cvt_exact(bad[i].bytes, bad[i].len);

		ck_assert_msg(s == NULL, "accepted: %s", bad[i].what);
		ck_assert_uint_eq(_XmStringByteStreamValidLength(
				  (unsigned char *)bad[i].bytes, bad[i].len), 0);
	}

	/* Bad header magic */
	ck_assert_uint_eq(_XmStringByteStreamValidLength(
			  (unsigned char *)"\x12\x34\x56\x00", 4), 0);
	ck_assert_uint_eq(_XmStringByteStreamValidLength(NULL, 100), 0);
}
END_TEST

/*
 * 1029786d: a long-form length below 128 (0x82 0x00 n) is valid and
 * must not desynchronise the walk.
 */
START_TEST(byte_stream_long_form_short_length)
{
	static const unsigned char body[] = {
		XmSTRING_COMPONENT_TAG, 0x82, 0x00, 0x01, 'T',
		XmSTRING_COMPONENT_TEXT, 0x82, 0x00, 0x03, 'a', 'b', 'c',
	};
	unsigned char *stream;
	size_t total;
	XmString s;

	stream = make_stream(body, sizeof body, &total);
	ck_assert_uint_eq(_XmStringByteStreamValidLength(stream, total), total);
	ck_assert_uint_eq(XmStringByteStreamLength(stream), total);
	s = cvt_exact(stream, total);
	ck_assert_ptr_nonnull(s);
	assert_text(s, "abc");
	XmStringFree(s);
	XtFree((char *)stream);
}
END_TEST

/*
 * Mutate every byte of a valid stream; nothing may read out of bounds
 * (run under ASan).  The results are not checked.
 */
START_TEST(byte_stream_mutations)
{
	XmString s, r;
	unsigned char *stream = NULL, *copy;
	unsigned int len, i, v;
	static const unsigned char vals[] = { 0x00, 0x01, 0x7f, 0x80, 0x82,
					      0xff };

	s = cat(tagged("one", "A"), tab(), tagged("two", "B"), sep(),
		XmStringDirectionCreate(XmSTRING_DIRECTION_R_TO_L),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
					1, "r"),
		tagged("three", "A"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
					1, "r"),
		NULL);
	len = XmCvtXmStringToByteStream(s, &stream);
	ck_assert_uint_gt(len, 0);
	copy = (unsigned char *)malloc(len);
	for (i = 0; i < len; i++) {
		for (v = 0; v < sizeof vals; v++) {
			memcpy(copy, stream, len);
			copy[i] = vals[v];
			if (!_XmStringByteStreamValidLength(copy, len))
				continue;
			r = XmCvtByteStreamToXmString(copy);
			if (r) {
				XtFree(text_of(r));
				XmStringFree(r);
			}
		}
	}
	free(copy);
	XtFree((char *)stream);
	XmStringFree(s);
}
END_TEST

/*
 * 2d1d4cec: the byte stream holds lengths in 16 bits; a longer string
 * cannot be encoded and must be refused rather than written with
 * wrapped lengths.
 */
START_TEST(byte_stream_too_long)
{
	size_t n = 70000;
	char *big = malloc(n + 1);
	unsigned char *stream = (unsigned char *)"not touched";
	XmString s;

	memset(big, 'y', n);
	big[n] = '\0';
	s = XmStringCreateLocalized(big);
	ck_assert_uint_eq(XmCvtXmStringToByteStream(s, &stream), 0);
	ck_assert_ptr_null(stream);
	XmStringFree(s);
	free(big);
}
END_TEST

/*
 * a9a49b6f: more than 255 rendition begin tags on one segment wrapped
 * the unsigned char counter and stored a tag at index -1.
 */
START_TEST(byte_stream_many_renditions)
{
	enum { N = 300 };
	unsigned char *body, *stream;
	size_t i, blen = 0, total;
	XmString s;

	body = (unsigned char *)XtMalloc(N * 3 + 8);
	for (i = 0; i < N; i++) {
		body[blen++] = XmSTRING_COMPONENT_RENDITION_BEGIN;
		body[blen++] = 1;
		body[blen++] = 'r';
	}
	body[blen++] = XmSTRING_COMPONENT_TEXT;
	body[blen++] = 2;
	body[blen++] = 'h';
	body[blen++] = 'i';
	stream = make_stream(body, blen, &total);
	s = cvt_exact(stream, total);
	ck_assert_ptr_nonnull(s);
	assert_text(s, "hi");
	XmStringFree(s);
	XtFree((char *)stream);
	XtFree((char *)body);
}
END_TEST

/*
 * 6612cc83, c7a6f58d: a tag holding a NUL must neither read past a
 * shorter cached tag nor grow the tag cache on every lookup.
 * 06cd5c8d: out of range cache indices give NULL.
 */
START_TEST(byte_stream_tag_with_nul)
{
	static const unsigned char body[] = {
		XmSTRING_COMPONENT_TAG, 3, 'a', '\0', 'b',
		XmSTRING_COMPONENT_TEXT, 1, 'x',
	};
	unsigned char *stream;
	size_t total;
	XmString s1, s2, ref;
	int i;

	/* Cache a one-letter tag "a" first */
	ref = tagged("x", "a");
	stream = make_stream(body, sizeof body, &total);
	s1 = cvt_exact(stream, total);
	ck_assert_ptr_nonnull(s1);
	for (i = 0; i < 100; i++) {
		s2 = cvt_exact(stream, total);
		ck_assert_ptr_nonnull(s2);
		ck_assert(XmStringCompare(s1, s2));
		XmStringFree(s2);
	}
	/* The tag is stored as the part before the NUL */
	ck_assert(XmStringCompare(s1, ref));
	XmStringFree(s1);
	XmStringFree(ref);
	XtFree((char *)stream);

	ck_assert_ptr_null(_XmStringIndexGetTag(-1));
	ck_assert_ptr_null(_XmStringIndexGetTag(1 << 20));
}
END_TEST

/*
 * 4f53fec9: a line of 256 or more segments lost all but count % 256 of
 * them in an 8-bit segment count.
 */
START_TEST(many_segments_on_one_line)
{
	enum { N = 300 };
	XmString s = NULL, c;
	char tag[16];
	int i;

	for (i = 0; i < N; i++) {
		snprintf(tag, sizeof tag, "t%d", i % 2);
		s = s ? XmStringConcatAndFree(s, tagged("z", tag))
		      : tagged("z", tag);
	}
	ck_assert_int_eq(count_comps(s, XmSTRING_COMPONENT_TEXT), N);
	ck_assert_int_eq(XmStringLineCount(s), 1);
	c = XmStringCopy(s);
	ck_assert(XmStringCompare(s, c));
	ck_assert_int_eq(count_comps(c, XmSTRING_COMPONENT_TEXT), N);
	assert_byte_stream_round_trip(s, SAME_COMPS | SAME_STRING);
	XmStringFree(c);
	XmStringFree(s);
}
END_TEST

/*
 * XmStringToXmStringTable / XmStringTableToXmString
 */

static Cardinal split(XmString s, XmString brk, XmStringTable *table)
{
	Cardinal n = XmStringToXmStringTable(s, brk, table);
	Cardinal i;

	for (i = 0; i < n; i++)
		ck_assert_msg((*table)[i] != NULL, "piece %u is NULL", i);
	return n;
}

static void free_table(XmStringTable table, Cardinal n)
{
	while (n--)
		XmStringFree(table[n]);
	XtFree((char *)table);
}

/* 6e4722c4: n breaks give n + 1 pieces, and the table round-trips */
START_TEST(table_pieces_match_man_page)
{
	XmString brk = sep(), s, back;
	XmStringTable table;
	Cardinal n;

	/* "a<sep>b" gives "a" and "b" */
	s = cat(XmStringCreateLocalized("a"), sep(),
		XmStringCreateLocalized("b"), NULL);
	n = split(s, brk, &table);
	ck_assert_uint_eq(n, 2);
	assert_text(table[0], "a");
	assert_text(table[1], "b");
	back = XmStringTableToXmString(table, n, brk);
	ck_assert(XmStringCompare(back, s));
	ck_assert_int_eq(count_comps(back, XmSTRING_COMPONENT_SEPARATOR), 1);
	XmStringFree(back);
	free_table(table, n);
	XmStringFree(s);

	/* No break: one piece equal to the string */
	s = XmStringCreateLocalized("abc");
	n = split(s, brk, &table);
	ck_assert_uint_eq(n, 1);
	ck_assert(XmStringCompare(table[0], s));
	free_table(table, n);
	XmStringFree(s);

	/* A trailing break gives an empty last piece, not NULL */
	s = cat(XmStringCreateLocalized("a"), sep(), NULL);
	n = split(s, brk, &table);
	ck_assert_uint_eq(n, 2);
	assert_text(table[0], "a");
	ck_assert(XmStringEmpty(table[1]));
	back = XmStringTableToXmString(table, n, brk);
	ck_assert(XmStringCompare(back, s));
	XmStringFree(back);
	free_table(table, n);
	XmStringFree(s);

	XmStringFree(brk);
}
END_TEST

/*
 * d7c5c844: lines of several segments made MakeStrFromSeg() index the
 * segment array with the count of another line (heap over-read under
 * ASan), and the pieces must round-trip.
 */
START_TEST(table_multi_segment_lines)
{
	XmString brk = sep(), s, back;
	XmStringTable table;
	Cardinal n;

	s = cat(tagged("a1", "A"), tagged("a2", "B"), tagged("a3", "A"), sep(),
		tagged("b1", "B"), sep(),
		tagged("c1", "A"), tagged("c2", "B"), NULL);
	n = split(s, brk, &table);
	ck_assert_uint_eq(n, 3);
	assert_text(table[0], "a1a2a3");
	assert_text(table[1], "b1");
	assert_text(table[2], "c1c2");
	back = XmStringTableToXmString(table, n, brk);
	ck_assert(XmStringCompare(back, s));
	XmStringFree(back);
	free_table(table, n);
	XmStringFree(s);
	XmStringFree(brk);
}
END_TEST

/*
 * 51675b14: a piece that starts after a break inside a segment keeps
 * the tag of its text.
 */
START_TEST(table_mid_segment_keeps_tag)
{
	XmString brk = tab(), s, back, expect;
	XmStringTable table;
	Cardinal n;

	/* The second segment is tab, tab, "right": the third piece starts
	 * after the first tab of that segment. */
	s = cat(tagged("left", "T0"), tab(), tab(), tagged("right", "T1"),
		NULL);
	n = split(s, brk, &table);
	ck_assert_uint_eq(n, 3);
	expect = tagged("left", "T0");
	ck_assert(XmStringCompare(table[0], expect));
	XmStringFree(expect);
	ck_assert(XmStringEmpty(table[1]));
	expect = tagged("right", "T1");
	ck_assert_msg(XmStringCompare(table[2], expect),
		      "the piece after the tab lost its tag");
	XmStringFree(expect);
	back = XmStringTableToXmString(table, n, brk);
	ck_assert(same_comps(back, s));
	XmStringFree(back);
	free_table(table, n);
	XmStringFree(s);
	XmStringFree(brk);
}
END_TEST

/*
 * 97547c53: XmStringConcatAndFree() of multi-line strings read b's
 * freed first entry through a stale aliased copy (heap corruption).
 */
START_TEST(concat_and_free_multi_line)
{
	XmString a, b, s, expect;
	int i;

	expect = cat(tagged("a1", "A"), sep(), tagged("a2", "A"),
		     tagged("a3", "B"), tagged("b1", "A"), tagged("b2", "C"),
		     sep(), tagged("b3", "A"), sep(), tagged("b4", "B"), NULL);

	for (i = 0; i < 50; i++) {
		a = cat(tagged("a1", "A"), sep(), tagged("a2", "A"),
			tagged("a3", "B"), NULL);
		b = cat(tagged("b1", "A"), tagged("b2", "C"), sep(),
			tagged("b3", "A"), sep(), tagged("b4", "B"), NULL);
		s = XmStringConcatAndFree(a, b);
		ck_assert_int_eq(XmStringLineCount(s), 4);
		ck_assert(same_comps(s, expect));
		ck_assert(XmStringCompare(s, expect));
		XmStringFree(s);
	}
	XmStringFree(expect);
}
END_TEST

/*
 * Known library bug: a string built by concatenating two or more
 * rendition components holds them in empty segments of their own, while the
 * byte stream reader (and any other path building the same components
 * at once) puts them on the text's segment.  XmStringCompare() compares
 * segment counts first, so it reports such strings as different even
 * though they have the same components.
 */
START_TEST(compare_ignores_segmentation)
{
	XmString a, b;
	unsigned char *stream = NULL;

	a = cat(XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
					2, "r1"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
					2, "r2"),
		XmStringCreateLocalized("text"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
					2, "r2"),
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
					2, "r1"), NULL);
	ck_assert_uint_gt(XmCvtXmStringToByteStream(a, &stream), 0);
	b = XmCvtByteStreamToXmString(stream);
	ck_assert(same_comps(a, b));
	ck_assert_msg(XmStringCompare(a, b),
		      "same components, but XmStringCompare() says different");
	XmStringFree(a);
	XmStringFree(b);
	XtFree((char *)stream);
}
END_TEST

#ifdef HAVE_LSAN
/*
 * _XmStringNonOptCreate() builds each segment in a stack record, and
 * finish_segment() used to reset it without freeing its rendition tag
 * arrays, which it had copied.
 */
START_TEST(byte_stream_rendition_leak)
{
	static const unsigned char body[] = {
		XmSTRING_COMPONENT_RENDITION_BEGIN, 1, 'r',
		XmSTRING_COMPONENT_TEXT, 1, 'x',
		XmSTRING_COMPONENT_RENDITION_END, 1, 'r',
	};
	unsigned char *stream;
	size_t total;
	XmString s;

	stream = make_stream(body, sizeof body, &total);
	s = XmCvtByteStreamToXmString(stream);
	ck_assert_ptr_nonnull(s);
	XmStringFree(s);
	XtFree((char *)stream);
	ck_assert_msg(!leaks_found(), "XmCvtByteStreamToXmString leaked");
}
END_TEST

/*
 * XmStringToXmStringTable() counts the pieces with one string context
 * and then starts it again; the renditions still active at the end of
 * the count used to be lost with it.
 */
START_TEST(table_open_rendition_leak)
{
	XmString s, sep_str = sep();
	XmStringTable table;
	Cardinal i, n;

	s = cat(XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
					1, "r"),
		XmStringCreateLocalized("a"), sep(),
		XmStringCreateLocalized("b"), sep(),
		XmStringCreateLocalized("c"), NULL);
	n = XmStringToXmStringTable(s, sep_str, &table);
	ck_assert_uint_eq(n, 3);
	for (i = 0; i < n; i++)
		XmStringFree(table[i]);
	XtFree((char *)table);
	XmStringFree(s);
	XmStringFree(sep_str);
	ck_assert_msg(!leaks_found(), "XmStringToXmStringTable leaked");
}
END_TEST
#endif

/*
 * Parse tables
 */

START_TEST(parse_table_round_trip)
{
	XmParseTable table;
	XmString tab_str = tab(), sep_str = sep(), s;
	struct comps c;
	char *text;
	Arg args[4];
	Cardinal n;

	/* XmParseTableFree() frees the array as well */
	table = (XmParseTable)XtMalloc(2 * sizeof(XmParseMapping));
	n = 0;
	XtSetArg(args[n], XmNpattern, "\t"); n++;
	XtSetArg(args[n], XmNpatternType, XmCHARSET_TEXT); n++;
	XtSetArg(args[n], XmNsubstitute, tab_str); n++;
	XtSetArg(args[n], XmNincludeStatus, XmINSERT); n++;
	table[0] = XmParseMappingCreate(args, n);
	n = 0;
	XtSetArg(args[n], XmNpattern, "\n"); n++;
	XtSetArg(args[n], XmNpatternType, XmCHARSET_TEXT); n++;
	XtSetArg(args[n], XmNsubstitute, sep_str); n++;
	XtSetArg(args[n], XmNincludeStatus, XmINSERT); n++;
	table[1] = XmParseMappingCreate(args, n);

	s = XmStringParseText("a\tb\nc", NULL, NULL, XmCHARSET_TEXT,
			      table, 2, NULL);
	ck_assert_ptr_nonnull(s);
	ck_assert_int_eq(count_comps(s, XmSTRING_COMPONENT_TAB), 1);
	ck_assert_int_eq(count_comps(s, XmSTRING_COMPONENT_SEPARATOR), 1);
	get_comps(s, &c);
	free_comps(&c);

	text = (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       table, 2, XmOUTPUT_ALL);
	ck_assert_str_eq(text, "a\tb\nc");
	XtFree(text);

	XmStringFree(s);
	XmParseTableFree(table, 2);
	XmStringFree(tab_str);
	XmStringFree(sep_str);
}
END_TEST

/*
 * Compound text
 */

START_TEST(ct_round_trip)
{
	/* "cafe" with an e acute, in the encoding of the suite's locale */
	const char *cafe = strcmp(nl_langinfo(CODESET), "UTF-8") == 0 ?
			   "caf\xc3\xa9" : "caf\xe9";
	XmString s = cat(XmStringCreate("plain", XmFONTLIST_DEFAULT_TAG), sep(),
			 XmStringCreate((char *)cafe, XmFONTLIST_DEFAULT_TAG), NULL);
	XmString back;
	char *ct, *t;
	char expect[16];

	ct = XmCvtXmStringToCT(s);
	ck_assert_ptr_nonnull(ct);
	back = XmCvtCTToXmString(ct);
	ck_assert_ptr_nonnull(back);
	ck_assert_int_eq(XmStringLineCount(back), 2);
	t = text_of(back);
	snprintf(expect, sizeof(expect), "plain%s", cafe);
	ck_assert_str_eq(t, expect);
	XtFree(t);
	XmStringFree(back);
	XtFree(ct);
	XmStringFree(s);
}
END_TEST

/*
 * 3ec877bf, 3c897ed3: malformed compound text must not run past the
 * input.  The strings are copied to exactly sized buffers for ASan.
 */
START_TEST(ct_malformed)
{
	static const char *bad[] = {
		/* extended segment (ESC % / 1 M L) without the STX */
		"\x1b%/1\x80\x85" "abcde" "trailing text without stx",
		"\x1b%/2\x80\x83" "abc",
		/* extended segment length past the end */
		"\x1b%/1\xff\xff" "x\x02y",
		/* two-octet charset, character cut short by the end */
		"\x1b$)A\xb0",
		"\x1b$(B\x30",
		/* CSI at the end, and a lone ESC */
		"abc\x9b",
		"abc\x9b" "1",
		"\x1b",
		"\x1b%",
		/* version sequence only */
		"\x1b#!0",
	};
	size_t i;

	for (i = 0; i < sizeof bad / sizeof bad[0]; i++) {
		size_t len = strlen(bad[i]) + 1;
		char *copy = malloc(len);
		XmString s;

		memcpy(copy, bad[i], len);
		s = XmCvtCTToXmString(copy);
		if (s)
			XmStringFree(s);
		free(copy);
	}
}
END_TEST

#if XM_UTF8
START_TEST(utf8_round_trip)
{
	XmString s = XmStringGenerate("gr\xc3\xbc\xc3\x9f" "e", "UTF-8",
				      XmCHARSET_TEXT, NULL);
	char *u = XmCvtXmStringToUTF8String(s);

	ck_assert_ptr_nonnull(u);
	ck_assert_str_eq(u, "gr\xc3\xbc\xc3\x9f" "e");
	XtFree(u);
	XmStringFree(s);
}
END_TEST
#endif

static float tab_value(XmTabList tl, int position)
{
	unsigned char units, alignment;
	XmOffsetModel model;
	char *decimal;
	XmTab tab = XmTabListGetTab(tl, (Cardinal)position);
	float value = XmTabGetValues(tab, &units, &model, &alignment, &decimal);

	XmTabFree(tab);
	return value;
}

/*
 * Negative positions count from the end of a tab list.  After one, the
 * next position in the same call was resolved relative to a wrongly
 * computed (unsigned) remainder, so {-1, 1} removed the first tab instead
 * of the second.
 */
START_TEST(tab_list_negative_positions)
{
	XmTab tabs[3];
	Cardinal positions[2];
	XmTabList tl;
	int i;

	for (i = 0; i < 3; i++)
		tabs[i] = XmTabCreate((float)(i + 1), XmCENTIMETERS, XmABSOLUTE,
				      XmALIGNMENT_BEGINNING, ".");
	tl = XmTabListInsertTabs(NULL, tabs, 3, 0);
	for (i = 0; i < 3; i++)
		XmTabFree(tabs[i]);
	ck_assert_uint_eq(XmTabListTabCount(tl), 3);

	positions[0] = (Cardinal)-1;
	positions[1] = 1;
	tl = XmTabListRemoveTabs(tl, positions, 2);
	ck_assert_uint_eq(XmTabListTabCount(tl), 1);
	ck_assert(tab_value(tl, 0) == 1.0f);
	XmTabListFree(tl);
}
END_TEST

void xmstring_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmString");

	t = tcase_create("Basics");
	tcase_add_test(t, create_and_compare);
	tcase_add_test(t, empty_strings);
	tcase_add_test(t, concat_text);
	tcase_add_test(t, concat_long_text);
	tcase_add_test(t, copy_shares);
	tcase_add_test(t, concat_last_direction);
	tcase_add_test(t, build_many_pieces);
	tcase_add_test(t, line_count);
	tcase_add_test(t, concat_keeps_tab_after_text);
	tcase_add_test(t, empty_text_component);
	tcase_add_test(t, compare_single_and_array_lines);
	tcase_add_test(t, concat_and_free_multi_line);
	tcase_add_test(t, many_segments_on_one_line);
	suite_add_tcase(s, t);

	t = tcase_create("Byte stream");
	tcase_add_test(t, byte_stream_round_trip);
	tcase_add_test(t, byte_stream_renditions);
	tcase_add_test(t, byte_stream_malformed);
	tcase_add_test(t, byte_stream_long_form_short_length);
	tcase_add_test(t, byte_stream_mutations);
	tcase_add_test(t, byte_stream_too_long);
	tcase_add_test(t, byte_stream_many_renditions);
	tcase_add_test(t, byte_stream_tag_with_nul);
#ifdef HAVE_LSAN
	tcase_add_test(t, byte_stream_rendition_leak);
#endif
	suite_add_tcase(s, t);

	t = tcase_create("String tables");
	tcase_add_test(t, table_pieces_match_man_page);
	tcase_add_test(t, table_multi_segment_lines);
	tcase_add_test(t, table_mid_segment_keeps_tag);
#ifdef HAVE_LSAN
	tcase_add_test(t, table_open_rendition_leak);
#endif
	suite_add_tcase(s, t);

	t = tcase_create("Parse tables");
	tcase_add_test(t, parse_table_round_trip);
	suite_add_tcase(s, t);

	t = tcase_create("Tab lists");
	tcase_add_test(t, tab_list_negative_positions);
	suite_add_tcase(s, t);

#if XM_UTF8
	t = tcase_create("UTF-8");
	tcase_add_test(t, utf8_round_trip);
	suite_add_tcase(s, t);
#endif

	t = tcase_create("Known bugs");
	tcase_add_test(t, compare_ignores_segmentation);
	tcase_set_tags(t, "xfail");
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}

static void _init_xt(void)
{
	init_xt("check_XmStringCT");
}

/*
 * Compound text conversion goes through Xlib's XmbTextListToTextProperty
 * and friends, which intern atoms on the default display.
 */
/*
 * XmCvtXmStringTableToTextProperty joins the text of all the segments of
 * a string; GetUseableText used to restart the buffer for every segment
 * and keep only the last one.
 */
START_TEST(text_property_joins_segments)
{
	XmString s = cat(XmStringCreateLocalized("abc"), tab(),
			 XmStringCreateLocalized("def"), NULL);
	Display **dpys;
	Cardinal ndpys;
	XTextProperty prop;

	XtGetDisplays(app, &dpys, &ndpys);
	ck_assert_uint_gt(ndpys, 0);
	ck_assert_int_eq(XmCvtXmStringTableToTextProperty(dpys[0], &s, 1,
							  XmSTYLE_LOCALE,
							  &prop), Success);
	ck_assert_ptr_nonnull(prop.value);
	ck_assert_str_eq((char *)prop.value, "abc\tdef");
	XFree(prop.value);
	XtFree((char *)dpys);
	XmStringFree(s);
}
END_TEST

void xmstring_ct_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmStringCT");

	t = tcase_create("Compound text");
	tcase_add_test(t, ct_round_trip);
	tcase_add_test(t, ct_malformed);
	tcase_add_test(t, text_property_joins_segments);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}

/*
 * Extents, which need fonts and so a display.
 */
static Widget ext_shell;

static void _init_xt_extent(void)
{
	ext_shell = init_xt("check_XmStringExtent");
}

static void _uninit_xt_extent(void)
{
	ext_shell = NULL;
	uninit_xt();
}

/* A rendition, with no font if font is NULL */
static XmRendition make_rend(const char *tag, const char *font,
			     XmFontType type)
{
	Arg args[2];

	XtSetArg(args[0], XmNfontName, font);
	XtSetArg(args[1], XmNfontType, type);
	return XmRenditionCreate(ext_shell, (XmStringTag)tag, args,
				 font ? 2 : 0);
}

/* Add a rendition to rt, or replace the one with its tag */
static XmRenderTable add_rend(XmRenderTable rt, const char *tag,
			      const char *font, XmMergeMode mode)
{
	XmRendition rend = make_rend(tag, font, XmFONT_IS_FONT);

	rt = XmRenderTableAddRenditions(rt, &rend, 1, mode);
	XmRenditionFree(rend);
	return rt;
}

/* A render table of one rendition, with no font if font is NULL */
static XmRenderTable make_rt(const char *tag, const char *font,
			     XmFontType type)
{
	XmRendition rend = make_rend(tag, font, type);
	XmRenderTable rt;

	rt = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_NEW);
	XmRenditionFree(rend);
	return rt;
}

static void quiet_warning(String msg)
{
	(void)msg;
}

/* Leave garbage where the next call's locals will be */
static void __attribute__((noinline)) dirty_stack(void)
{
	volatile unsigned char junk[8192];
	size_t i;

	for (i = 0; i < sizeof junk; i++)
		junk[i] = 0x5a;
}

/*
 * With no font for an optimized string, XmStringBaseline returned the
 * ascent OptLineMetrics never set: whatever was on the stack.
 */
START_TEST(baseline_without_font)
{
	XmRenderTable rt;
	XmString s = XmStringCreateLocalized("abc");
	Dimension w = 1, h = 1;
	int i;

	ck_assert(_XmStrOptimized(s));
	XtAppSetWarningHandler(app, quiet_warning);
	rt = make_rt(XmFONTLIST_DEFAULT_TAG, NULL, XmFONT_IS_FONT);
	for (i = 0; i < 3; i++) {
		dirty_stack();
		ck_assert_uint_eq(XmStringBaseline(rt, s), 0);
		XmStringExtent(rt, s, &w, &h);
		ck_assert_uint_eq(w, 0);
		ck_assert_uint_eq(h, 0);
	}
	XmRenderTableFree(rt);
	XmStringFree(s);
}
END_TEST

#ifdef HAVE_LSAN
/*
 * XmStringBaseline of a multi-segment string did not free the rendition
 * tags that measuring its first line collected.
 */
START_TEST(baseline_rendition_leak)
{
	XmRenderTable rt = make_rt("r1", "fixed", XmFONT_IS_FONT);
	XmString s = XmStringGenerate("abc\ndef", NULL, XmCHARSET_TEXT, "r1");

	ck_assert(!_XmStrOptimized(s));
	ck_assert_uint_gt(XmStringBaseline(rt, s), 0);
	XmStringFree(s);
	XmRenderTableFree(rt);
	ck_assert_msg(!leaks_found(), "XmStringBaseline leaked");
}
END_TEST
#endif

/* The extent and baseline of s with rt */
struct extent {
	Dimension w, h, base;
};

static struct extent extent_of(XmRenderTable rt, XmString s)
{
	struct extent e;

	XmStringExtent(rt, s, &e.w, &e.h);
	e.base = XmStringBaseline(rt, s);
	return e;
}

static void assert_extent_eq(struct extent a, struct extent b)
{
	ck_assert_uint_eq(a.w, b.w);
	ck_assert_uint_eq(a.h, b.h);
	ck_assert_uint_eq(a.base, b.base);
}

/* What a string that was never measured measures, with rt */
static struct extent fresh_extent(XmRenderTable rt, const char *text,
				  const char *tag)
{
	XmString s = tag ? XmStringCreate((char *)text, (XmStringTag)tag) :
			   XmStringCreateLocalized((char *)text);
	struct extent e;

	ck_assert(_XmStrOptimized(s));
	ck_assert(((_XmStringOpt)s)->extent_stamp == 0);
	e = extent_of(rt, s);
	XmStringFree(s);
	return e;
}

/*
 * Optimized strings cache their extent under the render table's stamp:
 * the cached extent is the computed one, with a core font, a font set
 * and Xft.
 */
START_TEST(extent_cache_hits)
{
	static const XmFontType types[] = {
		XmFONT_IS_FONT, XmFONT_IS_FONTSET,
#if USE_XFT
		XmFONT_IS_XFT
#endif
	};
	static const char *const texts[] = { "", "a", "The quick brown fox" };
	unsigned int i, j;

	for (i = 0; i < XtNumber(types); i++) {
		XmRenderTable rt = make_rt(XmFONTLIST_DEFAULT_TAG,
					   types[i] == XmFONT_IS_XFT ?
						   "Sans-10" : "fixed",
					   types[i]);

		for (j = 0; j < XtNumber(texts); j++) {
			XmString s = XmStringCreateLocalized((char *)texts[j]);
			struct extent first = extent_of(rt, s);

			ck_assert(((_XmStringOpt)s)->extent_stamp != 0);
			assert_extent_eq(extent_of(rt, s), first);
			assert_extent_eq(fresh_extent(rt, texts[j], NULL), first);
			if (j > 0)
				ck_assert_uint_gt(first.w, 0);
			XmStringFree(s);
		}
		XmRenderTableFree(rt);
	}
}
END_TEST

/*
 * A cached extent must not survive a change of the render table: one
 * replaced in place, one added, a table removed and another one
 * allocated (likely at the same address), and a copy.
 */
START_TEST(extent_cache_follows_table)
{
	XmRenderTable rt = make_rt(XmFONTLIST_DEFAULT_TAG, "fixed",
				   XmFONT_IS_FONT), copy;
	XmString s = XmStringCreateLocalized("The quick brown fox");
	XmString t = XmStringCreate("jumps over", "tagA");
	XmStringTag tags[1] = { "tagA" };
	struct extent before = extent_of(rt, s), after;

	(void)extent_of(rt, t);

	/* Replace the default rendition, in place since rt is not shared */
	rt = add_rend(rt, XmFONTLIST_DEFAULT_TAG, "9x15", XmMERGE_REPLACE);
	after = extent_of(rt, s);
	ck_assert_uint_ne(after.w, before.w);
	assert_extent_eq(after, fresh_extent(rt, "The quick brown fox", NULL));
	assert_extent_eq(extent_of(rt, t), fresh_extent(rt, "jumps over", "tagA"));

	/* Add a rendition for t's tag */
	rt = add_rend(rt, "tagA", "fixed", XmMERGE_NEW);
	assert_extent_eq(extent_of(rt, t), fresh_extent(rt, "jumps over", "tagA"));

	/* And remove it */
	rt = XmRenderTableRemoveRenditions(rt, tags, 1);
	assert_extent_eq(extent_of(rt, t), fresh_extent(rt, "jumps over", "tagA"));

	/* A copy measures the same, and so does a table that replaced it */
	copy = XmRenderTableCopy(rt, NULL, 0);
	assert_extent_eq(extent_of(copy, s), after);
	XmRenderTableFree(copy);
	XmRenderTableFree(rt);
	rt = make_rt(XmFONTLIST_DEFAULT_TAG, "fixed", XmFONT_IS_FONT);
	assert_extent_eq(extent_of(rt, s), before);
	assert_extent_eq(extent_of(rt, t), fresh_extent(rt, "jumps over", "tagA"));

	XmRenderTableFree(rt);
	XmStringFree(s);
	XmStringFree(t);
}
END_TEST

/*
 * XmStringConcatAndFree reuses an optimized string it owns when the
 * other one adds no text: its cached extent must go, as a tab or a
 * rendition in front of the text changes it.
 */
START_TEST(extent_cache_string_changed)
{
	XmRendition rend[2];
	XmRenderTable rt;
	XmTabList tabs;
	XmTab xtab;
	XmString s, reused;
	struct extent e;
	Arg args[3];

	xtab = XmTabCreate(100.0, XmPIXELS, XmABSOLUTE, XmALIGNMENT_BEGINNING,
			  NULL);
	tabs = XmTabListInsertTabs(NULL, &xtab, 1, 0);
	XmTabFree(xtab);
	XtSetArg(args[0], XmNfontName, "fixed");
	XtSetArg(args[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(args[2], XmNtabList, tabs);
	rend[0] = XmRenditionCreate(ext_shell, XmFONTLIST_DEFAULT_TAG, args, 3);
	XtSetArg(args[0], XmNfontName, "9x15");
	rend[1] = XmRenditionCreate(ext_shell, "r1", args, 2);
	rt = XmRenderTableAddRenditions(NULL, rend, 2, XmMERGE_NEW);
	XmRenditionFree(rend[0]);
	XmRenditionFree(rend[1]);
	XmTabListFree(tabs);

	/* A tab */
	s = XmStringCreateLocalized("abc");
	e = extent_of(rt, s);
	reused = XmStringConcatAndFree(tab(), s);
	ck_assert_ptr_eq(reused, s);
	ck_assert_uint_eq(extent_of(rt, reused).w, 100 + e.w);
	XmStringFree(reused);

	/* A rendition */
	s = XmStringCreateLocalized("abc");
	e = extent_of(rt, s);
	reused = XmStringConcatAndFree(
		XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN, 2,
					"r1"), s);
	ck_assert_ptr_eq(reused, s);
	ck_assert_uint_gt(extent_of(rt, reused).w, e.w);
	ck_assert_uint_gt(extent_of(rt, reused).h, e.h);
	XmStringFree(reused);

	XmRenderTableFree(rt);
}
END_TEST

/*
 * Tabs in units other than pixels depend on the screen (font units on
 * the XmScreen's resources), so their extent is not cached.
 */
START_TEST(extent_cache_skips_tab_units)
{
	XmRendition rend;
	XmRenderTable rt;
	XmTabList tabs;
	XmTab xtab;
	XmString s;
	Arg args[3];
	Dimension w1, w2, h;

	xtab = XmTabCreate(2.0, XmFONT_UNITS, XmABSOLUTE, XmALIGNMENT_BEGINNING,
			  NULL);
	tabs = XmTabListInsertTabs(NULL, &xtab, 1, 0);
	XmTabFree(xtab);
	XtSetArg(args[0], XmNfontName, "fixed");
	XtSetArg(args[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(args[2], XmNtabList, tabs);
	rend = XmRenditionCreate(ext_shell, XmFONTLIST_DEFAULT_TAG, args, 3);
	rt = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_NEW);
	XmRenditionFree(rend);
	XmTabListFree(tabs);

	s = XmStringConcatAndFree(tab(), XmStringCreateLocalized("abc"));
	ck_assert(_XmStrOptimized(s));
	XmStringExtent(rt, s, &w1, &h);
	ck_assert(((_XmStringOpt)s)->extent_stamp == 0);
	XmStringExtent(rt, s, &w2, &h);
	ck_assert_uint_eq(w1, w2);
	XmStringFree(s);
	XmRenderTableFree(rt);
}
END_TEST

void xmstring_extent_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmStringExtent");

	t = tcase_create("Extents");
	tcase_add_test(t, baseline_without_font);
	tcase_add_test(t, extent_cache_hits);
	tcase_add_test(t, extent_cache_follows_table);
	tcase_add_test(t, extent_cache_string_changed);
	tcase_add_test(t, extent_cache_skips_tab_units);
#ifdef HAVE_LSAN
	tcase_add_test(t, baseline_rendition_leak);
#endif
	tcase_add_checked_fixture(t, _init_xt_extent, _uninit_xt_extent);
	suite_add_tcase(s, t);

	srunner_add_suite(runner, s);
}
