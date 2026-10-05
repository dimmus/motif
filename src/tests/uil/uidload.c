/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * uidload: load .uid files with Mrm, fetch every widget and literal
 * they index, realize the widgets, and check resource values.
 *
 *   uidload [--buffer] [--procs file.uil]... file.uid... [check ...]
 *
 * --buffer maps the (single) file into memory, read-only, and opens it
 * with MrmOpenHierarchyFromBufferWithSize instead of from the file: Mrm
 * takes the buffer as const, and a write to it would fault.
 *
 * --procs registers every procedure that file.uil declares, so that
 * callbacks resolve; they do nothing.
 *
 * A check is name:resource=value.  The widget is looked up by name
 * among the fetched ones; a value in quotes ("text") is compared with
 * the text of an XmString resource (separators as \n), anything else
 * with an integer resource.
 *
 * Exits 0 on success, 1 on failure and 77 when there is no display.
 */
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Mrm/MrmPublic.h>
#include <Mrm/Mrm.h>

#include "leak.h"

#define EXIT_SKIP 77

static XtAppContext app;
static int x_errors;

static int x_error_handler(Display *dpy, XErrorEvent *ev)
{
	char text[256];

	XGetErrorText(dpy, ev->error_code, text, sizeof text);
	fprintf(stderr, "uidload: X error: %s (request %d)\n", text,
		ev->request_code);
	x_errors++;
	return 0;
}

/* Procedures named in UIL callbacks resolve to this */
static void dummy_proc(Widget w, XtPointer client, XtPointer call)
{
	(void)w;
	(void)client;
	(void)call;
}

static MrmRegisterArg *procs;
static int n_procs, max_procs;

static void add_proc(const char *name, size_t len)
{
	int i;

	for (i = 0; i < n_procs; i++)
		if (strlen(procs[i].name) == len &&
		    !strncmp(procs[i].name, name, len))
			return;
	if (n_procs == max_procs) {
		max_procs = max_procs ? 2 * max_procs : 32;
		procs = realloc(procs, max_procs * sizeof *procs);
	}
	procs[n_procs].name = strndup(name, len);
	procs[n_procs].value = (XtPointer)dummy_proc;
	n_procs++;
}

/* The next token of UIL source: a word, or one punctuation character */
static const char *next_token(const char *p, size_t *len)
{
	for (;;) {
		while (*p && isspace((unsigned char)*p))
			p++;
		if (*p == '!') {			/* comment to end of line */
			while (*p && *p != '\n')
				p++;
		} else if (p[0] == '/' && p[1] == '*') {
			const char *e = strstr(p + 2, "*/");
			p = e ? e + 2 : p + strlen(p);
		} else if (*p == '\'' || *p == '"') {	/* string literal */
			char q = *p++;
			while (*p && *p != q && *p != '\n')
				p += (*p == '\\' && p[1]) ? 2 : 1;
			if (*p)
				p++;
		} else {
			break;
		}
	}
	if (!*p)
		return NULL;
	if (isalpha((unsigned char)*p) || *p == '_' || *p == '$') {
		const char *s = p;
		while (isalnum((unsigned char)*p) || *p == '_' || *p == '$')
			p++;
		*len = p - s;
		return s;
	}
	*len = 1;
	return p;
}

static int is_word(const char *t, size_t len, const char *w)
{
	return t && len == strlen(w) && !strncasecmp(t, w, len);
}

static int is_name(const char *t)
{
	return t && (isalpha((unsigned char)*t) || *t == '_');
}

/*
 * Register the procedures a UIL file declares or refers to: the names
 * in its "procedure" sections, after "procedure" in callbacks, and in
 * "procedures { ... }" lists.  Widget creation procedures
 * (user_defined procedure X) are left out: they must return a widget.
 */
static int add_procs_from(const char *path)
{
	static const char *const sections[] = {
		"value", "object", "identifier", "list", "end", "include",
		"procedure",
	};
	FILE *f = fopen(path, "r");
	const char *p, *t, *prev = NULL;
	size_t len, prev_len = 0, i;
	char *src;
	long size;
	int in_section = 0, in_list = 0, at_start = 0;

	if (!f)
		return 0;
	if (fseek(f, 0, SEEK_END) || (size = ftell(f)) < 0 ||
	    fseek(f, 0, SEEK_SET)) {
		fclose(f);
		return 0;
	}
	src = calloc(1, (size_t)size + 1);
	if (!src || fread(src, 1, (size_t)size, f) != (size_t)size) {
		fclose(f);
		free(src);
		return 0;
	}
	fclose(f);

	for (p = src; (t = next_token(p, &len)) != NULL; p = t + len) {
		if (is_word(t, len, "procedure")) {
			const char *n;
			size_t nlen;

			n = next_token(t + len, &nlen);
			if (is_word(prev, prev_len, "user_defined")) {
				/* skip the creation procedure's name */
				if (!n)
					break;
				t = n;
				len = nlen;
			} else if (!prev || *prev == ';' || is_name(prev)) {
				/* a section of declarations */
				in_section = 1;
				at_start = 1;
			} else if (is_name(n)) {
				/* a reference: "= procedure name (...)" */
				add_proc(n, nlen);
			}
		} else if (is_word(t, len, "procedures")) {
			in_list = 1;
			at_start = 0;
		} else if (in_list) {
			if (*t == '}')
				in_list = 0;
			else if (at_start && is_name(t))
				add_proc(t, len);
			at_start = (*t == '{' || *t == ';');
		} else if (in_section) {
			for (i = 0; i < sizeof sections / sizeof sections[0]; i++)
				if (is_word(t, len, sections[i]))
					in_section = 0;
			if (in_section && at_start && is_name(t))
				add_proc(t, len);
			at_start = in_section && *t == ';';
		}
		prev = t;
		prev_len = len;
	}
	free(src);
	return 1;
}

static void pump(Display *dpy)
{
	int idle = 0;

	XSync(dpy, False);
	while (idle < 3) {
		if (XtAppPending(app)) {
			XtAppProcessEvent(app, XtIMAll);
			idle = 0;
		} else {
			XSync(dpy, False);
			idle++;
		}
	}
}

static const unsigned char *map_file(const char *path, size_t *size)
{
	struct stat st;
	void *map;
	int fd = open(path, O_RDONLY);

	if (fd < 0)
		return NULL;
	if (fstat(fd, &st) || st.st_size <= 0) {
		close(fd);
		return NULL;
	}
	map = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (map == MAP_FAILED)
		return NULL;
	*size = (size_t)st.st_size;
	return map;
}

/* The names of the resources of one group indexed in the hierarchy */
static URMPointerListPtr index_names(MrmHierarchy h, MrmGroup group)
{
	URMPointerListPtr list = NULL;
	int i;

	if (UrmPlistInit(16, &list) != MrmSUCCESS)
		return NULL;
	for (i = 0; i < h->num_file; i++)
		UrmIdbFindIndexedResource(h->file_list[i], group, URMtNul, list);
	return list;
}

/* Fetch every literal; values are left to Mrm (see below) */
static int fetch_literals(MrmHierarchy h, Display *dpy)
{
	URMPointerListPtr names = index_names(h, URMgLiteral);
	int i, n = 0;

	if (!names)
		return 0;
	for (i = 0; i < UrmPlistNum(names); i++) {
		const char *name = (const char *)UrmPlistPtrN(names, i);
		XtPointer value = NULL;
		MrmCode type = 0;
		Cardinal rc;

		/* ">ClassTable" and the like are Mrm's own tables */
		if (name[0] == '>')
			continue;
		/*
		 * Who frees a literal's value depends on its type, and for
		 * some types the value shares storage with Mrm's caches;
		 * this test does not check that, only that fetching works.
		 */
		KNOWN_LEAK_BEGIN();
		rc = MrmFetchLiteral(h, name, dpy, &value, &type);
		KNOWN_LEAK_END();
		if (rc == MrmSUCCESS)
			n++;
	}
	UrmPlistFreeContents(names);
	UrmPlistFree(names);
	return n;
}

struct fetched {
	const char *name;
	Widget w;
};

static Widget find(struct fetched *f, int n, const char *name)
{
	int i;

	for (i = 0; i < n; i++)
		if (!strcmp(f[i].name, name))
			return f[i].w;
	return NULL;
}

/* The size of resource name of w, or 0 */
static Cardinal resource_size(Widget w, const char *name)
{
	XtResourceList res;
	Cardinal n, i, size = 0;

	XtGetResourceList(XtClass(w), &res, &n);
	for (i = 0; i < n; i++)
		if (!strcmp(res[i].resource_name, name))
			size = res[i].resource_size;
	XtFree((char *)res);
	if (!size && XtParent(w) && XtIsConstraint(XtParent(w))) {
		XtGetConstraintResourceList(XtClass(XtParent(w)), &res, &n);
		for (i = 0; res && i < n; i++)
			if (!strcmp(res[i].resource_name, name))
				size = res[i].resource_size;
		XtFree((char *)res);
	}
	return size;
}

/* The text of an XmString, lines separated by \n */
static char *xmstring_text(XmString s)
{
	XmParseMapping map[1];
	XmString nl = XmStringSeparatorCreate();
	Arg args[3];
	char *text;

	XtSetArg(args[0], XmNpattern, "\n");
	XtSetArg(args[1], XmNsubstitute, nl);
	XtSetArg(args[2], XmNincludeStatus, XmINSERT);
	map[0] = XmParseMappingCreate(args, 3);
	text = (char *)XmStringUnparse(s, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
				       map, 1, XmOUTPUT_ALL);
	XmParseMappingFree(map[0]);
	XmStringFree(nl);
	return text;
}

static int check(struct fetched *f, int n, const char *spec)
{
	char name[256], resource[256];
	const char *colon = strchr(spec, ':'), *eq = strchr(spec, '=');
	Widget w;

	if (!colon || !eq || eq < colon ||
	    (size_t)(colon - spec) >= sizeof name ||
	    (size_t)(eq - colon - 1) >= sizeof resource) {
		fprintf(stderr, "uidload: bad check '%s'\n", spec);
		return 0;
	}
	memcpy(name, spec, colon - spec);
	name[colon - spec] = '\0';
	memcpy(resource, colon + 1, eq - colon - 1);
	resource[eq - colon - 1] = '\0';

	if (!(w = find(f, n, name))) {
		fprintf(stderr, "uidload: %s: widget not fetched\n", spec);
		return 0;
	}
	if (eq[1] == '"') {
		XmString s = NULL;
		char *text;
		size_t len = strlen(eq + 2);
		int ok;

		XtVaGetValues(w, resource, &s, NULL);
		text = s ? xmstring_text(s) : NULL;
		ok = text && len && eq[2 + len - 1] == '"' &&
		     strlen(text) == len - 1 && !strncmp(text, eq + 2, len - 1);
		if (!ok)
			fprintf(stderr, "uidload: %s: got \"%s\"\n", spec,
				text ? text : "(null)");
		XtFree(text);
		XmStringFree(s);
		return ok;
	} else {
		union { XtArgVal v; char c[sizeof(XtArgVal)]; } buf;
		Cardinal size = resource_size(w, resource);
		long got, want = strtol(eq + 1, NULL, 0);

		memset(&buf, 0, sizeof buf);
		XtVaGetValues(w, resource, &buf, NULL);
		switch (size) {
		case 1: got = *(signed char *)&buf; break;
		case 2: got = *(short *)&buf; break;
		case 4: got = *(int *)&buf; break;
		default: got = (long)buf.v; break;
		}
		if (got != want) {
			fprintf(stderr, "uidload: %s: got %ld\n", spec, got);
			return 0;
		}
		return 1;
	}
}

int main(int argc, char **argv)
{
	const char *display = getenv("DISPLAY");
	const unsigned char *buffer = NULL;
	String paths[16];
	int n_paths = 0;
	struct fetched *fetched;
	URMPointerListPtr names;
	MrmHierarchy h;
	MrmType cls;
	Widget top;
	Display *dpy;
	int use_buffer = 0, first, i, n_fetched = 0, n_failed = 0;
	int n_literals, ok = 1, xargc = 1;
	static char argv0[] = "uidload";
	char *xargv[] = { argv0, NULL };
	size_t size = 0;
	Widget parent;

	for (first = 1; first < argc; first++) {
		size_t len = strlen(argv[first]);

		if (!strcmp(argv[first], "--buffer")) {
			use_buffer = 1;
		} else if (!strcmp(argv[first], "--procs") && first + 1 < argc) {
			if (!add_procs_from(argv[++first])) {
				fprintf(stderr, "uidload: cannot read %s\n",
					argv[first]);
				return 1;
			}
		} else if (len > 4 && !strcmp(argv[first] + len - 4, ".uid") &&
			   n_paths < (int)(sizeof paths / sizeof paths[0])) {
			paths[n_paths++] = argv[first];
		} else {
			break;
		}
	}
	if (!n_paths || (use_buffer && n_paths > 1)) {
		fprintf(stderr, "usage: %s [--buffer] [--procs file.uil]... "
			"file.uid... [name:resource=value ...]\n", argv[0]);
		return 1;
	}
	if (!display || !*display) {
		printf("uidload: SKIP: DISPLAY is not set\n");
		return EXIT_SKIP;
	}

	MrmInitialize();
	XtSetLanguageProc(NULL, NULL, NULL);
	top = XtAppInitialize(&app, "UidLoad", NULL, 0, &xargc, xargv, NULL,
			      NULL, 0);
	dpy = XtDisplay(top);
	XSetErrorHandler(x_error_handler);

	if (use_buffer) {
		buffer = map_file(paths[0], &size);
		if (!buffer) {
			fprintf(stderr, "uidload: cannot read %s\n", paths[0]);
			return 1;
		}
		if (MrmOpenHierarchyFromBufferWithSize(buffer, size, &h) !=
		    MrmSUCCESS) {
			fprintf(stderr, "uidload: cannot open %s from a buffer\n",
				paths[0]);
			return 1;
		}
	} else if (MrmOpenHierarchyPerDisplay(dpy, n_paths, paths, NULL, &h) !=
		   MrmSUCCESS) {
		fprintf(stderr, "uidload: cannot open %s\n", paths[0]);
		return 1;
	}
	if (n_procs && MrmRegisterNamesInHierarchy(h, procs, n_procs) !=
	    MrmSUCCESS) {
		fprintf(stderr, "uidload: cannot register procedures\n");
		return 1;
	}

	/* A manager to fetch into, so that gadgets have a valid parent */
	parent = XmVaCreateManagedBulletinBoard(top, "uidload_parent", NULL);

	names = index_names(h, URMgWidget);
	if (!names)
		return 1;

	fetched = calloc((size_t)UrmPlistNum(names) + 1, sizeof *fetched);
	for (i = 0; i < UrmPlistNum(names); i++) {
		const char *name = (const char *)UrmPlistPtrN(names, i);
		Widget w = NULL;
		Cardinal rc;

		rc = MrmFetchWidget(h, name, parent, &w, &cls);
		if (rc != MrmSUCCESS || !w) {
			fprintf(stderr, "uidload: cannot fetch %s (%u)\n",
				name, rc);
			n_failed++;
			continue;
		}
		fetched[n_fetched].name = name;
		fetched[n_fetched].w = w;
		n_fetched++;
		/* Not menu panes or dialog contents: their parent is a shell */
		if (XtParent(w) == parent)
			XtManageChild(w);
	}

	n_literals = fetch_literals(h, dpy);

	XtRealizeWidget(top);
	pump(dpy);

	for (i = first; i < argc; i++)
		if (!check(fetched, n_fetched, argv[i]))
			ok = 0;

	printf("uidload: %s: fetched %d widgets (%d failed), %d literals\n",
	       paths[0], n_fetched, n_failed, n_literals);

	XtDestroyWidget(top);
	pump(dpy);
	UrmPlistFreeContents(names);
	UrmPlistFree(names);
	free(fetched);
	MrmCloseHierarchy(h);
	if (buffer)
		munmap((void *)buffer, size);
	XtDestroyApplicationContext(app);
	for (i = 0; i < n_procs; i++)
		free(procs[i].name);
	free(procs);

	if (n_failed || x_errors)
		ok = 0;
	return ok ? 0 : 1;
}
