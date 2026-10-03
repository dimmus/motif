/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * The XmString ASN.1 byte stream, as read from _MOTIF_COMPOUND_STRING
 * selections, clipboard items and .uid files: validate it the way
 * callers holding a sized buffer do, convert it, and exercise the
 * result (components, copies, comparison, string tables, the byte
 * stream back).  No display is needed.
 */
#include <Xm/Xm.h>
#include <Xm/XmStringI.h>

#include "fuzz_common.h"

static void walk(XmString s)
{
	XmStringContext ctx;
	XmStringComponentType t;
	unsigned int len;
	XtPointer val;
	int n = 0;

	if (!XmStringInitContext(&ctx, s))
		return;
	while ((t = XmStringGetNextTriple(ctx, &len, &val)) !=
	       XmSTRING_COMPONENT_END && n++ < 100000)
		XtFree((char *)val);
	XmStringFreeContext(ctx);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	unsigned char *buf, *stream = NULL;
	XmString s, copy, brk, back;
	XmStringTable table = NULL;
	Cardinal n;
	char *text;

	/* An exactly sized copy, so that ASan sees reads past the end */
	buf = malloc(size ? size : 1);
	memcpy(buf, data, size);

	if (!_XmStringByteStreamValidLength(buf, size)) {
		free(buf);
		return 0;
	}
	s = XmCvtByteStreamToXmString(buf);
	free(buf);
	if (!s)
		return 0;

	walk(s);
	copy = XmStringCopy(s);
	(void)XmStringCompare(s, copy);
	(void)XmStringLineCount(s);
	(void)XmStringEmpty(s);
	text = (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       NULL, 0, XmOUTPUT_ALL);
	XtFree(text);

	brk = XmStringSeparatorCreate();
	n = XmStringToXmStringTable(s, brk, &table);
	if (table) {
		back = XmStringTableToXmString(table, n, brk);
		XmStringFree(back);
		while (n--)
			XmStringFree(table[n]);
		XtFree((char *)table);
	}
	XmStringFree(brk);

	if (XmCvtXmStringToByteStream(copy, &stream))
		XtFree((char *)stream);
	XmStringFree(copy);
	XmStringFree(s);
	return 0;
}
