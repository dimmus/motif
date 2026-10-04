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
