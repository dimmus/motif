/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <check.h>
#include "suites.h"

/* Exit status that CTest (SKIP_RETURN_CODE) and automake report as skipped */
#define EXIT_SKIP 77

XtAppContext app;
static Widget app_shell;

static const struct suite_entry {
	const char *name;
	void (*add)(SRunner *runner);
	int needs_display;
} suite_table[] = {
	{ "FontListEntry", xmfontlistentry_suite, 1 },
	{ "FontList",      xmfontlist_suite,      1 },
#ifdef XM_WITH_PNG
	{ "Png",           png_suite,             0 },
#endif
#ifdef XM_WITH_JPEG
	{ "Jpeg",          jpeg_suite,            0 },
#endif
	{ "Svg",           svg_suite,             0 },
	{ "Log",           log_suite,             0 },
	{ "LogConfig",     log_config_suite,      0 },
	{ "XmString",      xmstring_suite,        0 },
	{ "XmStringCT",    xmstring_ct_suite,     1 },
	{ "XmStringExtent", xmstring_extent_suite, 1 },
	{ "Widgets",       widgets_suite,         1 },
	{ "Text",          text_suite,            1 },
	{ "I18n",          i18n_suite,            0 },
	{ "Layout",        layout_suite,          1 },
	{ "Cache",         cache_suite,           0 },
	{ "GadgetCache",   gadget_cache_suite,    1 },
	{ "Draw",          draw_suite,            1 },
};

#define N_SUITES (sizeof suite_table / sizeof suite_table[0])

/**
 * Xt fixture
 */
Widget init_xt(const char *klass)
{
	Widget shell;
	int argc = 0;

	XtSetLanguageProc(NULL, NULL, NULL);
	XtToolkitInitialize();
	shell = XtAppInitialize(&app, klass, NULL, 0, &argc, NULL, NULL, NULL, 0);
	ck_assert_msg(app, "Failed to initialize app context");
	app_shell = shell;
	return shell;
}

void uninit_xt(void)
{
	/* Destroy the shell first: closing the display does not destroy
	 * widgets, so whatever they keep would show up as leaks. */
	if (app_shell)
		XtDestroyWidget(app_shell);
	app_shell = NULL;
	if (app)
		XtDestroyApplicationContext(app);
	app = NULL;
}

static const struct suite_entry *find_suite(const char *name)
{
	size_t i;

	for (i = 0; i < N_SUITES; i++)
		if (!strcmp(suite_table[i].name, name))
			return &suite_table[i];
	return NULL;
}

static void usage(const char *prog)
{
	size_t i;

	fprintf(stderr, "usage: %s [--xfail] [suite ...]\n"
		"  --xfail  run only the test cases tagged \"xfail\" (known failures)\n"
		"available suites:", prog);
	for (i = 0; i < N_SUITES; i++)
		fprintf(stderr, " %s", suite_table[i].name);
	fputc('\n', stderr);
}

/**
 * Add a suite to the runner, unless it needs an X server and DISPLAY
 * is not set.  Returns 1 if the suite was added, 0 if it was skipped.
 */
static int add_suite(SRunner *runner, const struct suite_entry *e)
{
	const char *display = getenv("DISPLAY");

	if (e->needs_display && (!display || !*display)) {
		printf("# SKIP %s: DISPLAY is not set\n", e->name);
		return 0;
	}
	e->add(runner);
	return 1;
}

/**
 * Run the suites named on the command line, or all of them if none are
 * named.  If every requested suite had to be skipped, exit with status
 * 77 so that the harness reports a skip rather than a pass.
 *
 * Test cases tagged "xfail" document known library bugs.  They are left
 * out of normal runs; with --xfail only they are run, and the run passes
 * only if every one of them still fails, so that a fix is noticed.
 */
int main(int argc, char *argv[])
{
	const struct suite_entry *e;
	int failed, run, i, first = 1, n_added = 0, xfail = 0;
	SRunner *runner;

	if (argc > 1 && !strcmp(argv[1], "--xfail")) {
		xfail = 1;
		first = 2;
	}

	runner = srunner_create(NULL);
	if (argc > first) {
		for (i = first; i < argc; i++) {
			if (!(e = find_suite(argv[i]))) {
				fprintf(stderr, "%s: unknown suite '%s'\n", argv[0], argv[i]);
				usage(argv[0]);
				srunner_free(runner);
				return EXIT_FAILURE;
			}
			n_added += add_suite(runner, e);
		}
	} else {
		for (i = 0; i < (int)N_SUITES; i++)
			n_added += add_suite(runner, &suite_table[i]);
	}

	if (!n_added) {
		srunner_free(runner);
		return EXIT_SKIP;
	}

	srunner_set_tap(runner, "-");

	/**
	 * Given that some things in Motif / Xt rely on static initialization
	 * at runtime, the tests NEED to fork.
	 */
	srunner_set_fork_status(runner, CK_FORK);
	srunner_run_tagged(runner, NULL, NULL, xfail ? "xfail" : NULL,
			   xfail ? NULL : "xfail", CK_SILENT);
	failed = srunner_ntests_failed(runner);
	run = srunner_ntests_run(runner);
	srunner_free(runner);

	if (xfail) {
		if (!run) {
			printf("# no test cases tagged xfail were run\n");
			return EXIT_FAILURE;
		}
		if (failed != run) {
			printf("# %d of %d expected failures passed unexpectedly\n",
			       run - failed, run);
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
