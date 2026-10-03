/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * .uid files: open the input with MrmOpenHierarchyFromBufferWithSize,
 * read every indexed widget and literal record, and, when there is a
 * display (run under xvfb-run), fetch the widgets and literals as an
 * application would.
 */
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Mrm/MrmPublic.h>
#include <Mrm/Mrm.h>

#include "fuzz_common.h"
#include "leak.h"

static Widget parent;

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	(void)argc;
	MrmInitialize();
	if (getenv("DISPLAY") && *getenv("DISPLAY")) {
		fuzz_open_display((*argv)[0]);
		parent = XmCreateBulletinBoard(fuzz_top, "parent", NULL, 0);
		XtManageChild(parent);
		XtRealizeWidget(fuzz_top);
	}
	return 0;
}

static void each_index(MrmHierarchy h, MrmGroup group, URMResourceContextPtr ctx)
{
	URMPointerListPtr names = NULL;
	int i, j;

	if (UrmPlistInit(16, &names) != MrmSUCCESS)
		return;
	for (i = 0; i < h->num_file; i++)
		UrmIdbFindIndexedResource(h->file_list[i], group, URMtNul, names);
	for (j = 0; j < UrmPlistNum(names) && j < 256; j++) {
		String name = (String)UrmPlistPtrN(names, j);
		IDBFile file = NULL;

		if (group == URMgWidget) {
			Widget w = NULL;
			MrmType cls;

			UrmHGetIndexedResource(h, name, URMgWidget, URMtNul, ctx,
					       &file);
			if (parent &&
			    MrmFetchWidget(h, name, parent, &w, &cls) ==
			    MrmSUCCESS && w)
				XtDestroyWidget(w);
		} else if (name[0] != '>') {
			XtPointer value = NULL;
			MrmCode type;

			UrmHGetIndexedResource(h, name, URMgLiteral, URMtNul,
					       ctx, &file);
			/* Who frees a literal depends on its type; not
			 * checked here (see uidload.c). */
			if (parent) {
				KNOWN_LEAK_BEGIN();
				MrmFetchLiteral(h, name, XtDisplay(parent),
						&value, &type);
				KNOWN_LEAK_END();
			}
		}
	}
	UrmPlistFreeContents(names);
	UrmPlistFree(names);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	unsigned char *buf;
	URMResourceContextPtr ctx;
	MrmHierarchy h;

	if (!size)
		return 0;
	buf = malloc(size);
	memcpy(buf, data, size);
	if (MrmOpenHierarchyFromBufferWithSize(buf, size, &h) == MrmSUCCESS) {
		if (UrmGetResourceContext(NULL, NULL, 0, &ctx) == MrmSUCCESS) {
			each_index(h, URMgWidget, ctx);
			each_index(h, URMgLiteral, ctx);
			UrmFreeResourceContext(ctx);
		}
		MrmCloseHierarchy(h);
		if (parent)
			fuzz_pump();
	}
	free(buf);
	return 0;
}
