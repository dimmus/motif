/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Message catalogs: a Motif warning comes from the catalog that NLSPATH
 * finds for the locale (libXm opens "Xm" with catopen() when the
 * VendorShell class is initialized).
 *
 * $MOTIF_TEST_MSGCAT_SOURCE names the symbolic catalog source whose
 * message is expected; CTest sets it, with NLSPATH and the locale, for
 * Xm.MsgCat.C (a test catalog in the C locale) and Xm.MsgCat.de_DE.UTF-8
 * (the German catalog of localized/de).  Without it the catalog is
 * kept out of reach and the built-in English message is expected.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <check.h>

#include "i18n_util.h"
#include "suites.h"

/* The message XmForm warns with when XmNfractionBase is 0 */
#define MESSAGE_ID "MSG_Form_0000"
#define BUILT_IN "Fraction base cannot be zero."

static Widget top;
static char warning[512];
static int warnings;

static void warning_handler(String name, String type, String klass, String defaultp,
			    String *params, Cardinal *num_params)
{
	(void)name;
	(void)type;
	(void)klass;
	(void)params;
	(void)num_params;
	snprintf(warning, sizeof warning, "%s", defaultp ? defaultp : "");
	warnings++;
}

/*
 * The text of message id in a symbolic catalog source: the rest of the
 * line, or the quoted string when the catalog sets $quote ".
 */
static char *catalog_message(const char *path, const char *id)
{
	FILE *f = fopen(path, "r");
	char line[1024], *out = NULL;
	size_t len = strlen(id);
	int quoted = 0;

	ck_assert_msg(f != NULL, "cannot open %s", path);
	while (!out && fgets(line, sizeof line, f)) {
		char *p, *o;

		line[strcspn(line, "\n")] = '\0';
		if (!strncmp(line, "$quote \"", 8))
			quoted = 1;
		if (strncmp(line, id, len) || (line[len] != ' ' && line[len] != '\t'))
			continue;
		p = line + len + strspn(line + len, " \t");
		if (quoted && *p == '"')
			p++;
		out = o = malloc(strlen(p) + 1);
		ck_assert_ptr_nonnull(out);
		for (; *p && !(quoted && *p == '"'); p++) {
			if (*p == '\\' && p[1]) {
				p++;
				*o++ = *p == 'n' ? '\n' : *p == 't' ? '\t' : *p;
			} else {
				*o++ = *p;
			}
		}
		*o = '\0';
	}
	fclose(f);
	ck_assert_msg(out != NULL, "no %s in %s", id, path);
	return out;
}

static void setup(void)
{
	/* Keep an installed catalog out of the built-in message's way */
	if (!getenv("MOTIF_TEST_MSGCAT_SOURCE"))
		setenv("NLSPATH", "/nonexistent/%N", 1);
	ck_assert_ptr_nonnull(test_set_locale(NULL));
	top = init_xt("check_MsgCat");
	XtAppSetWarningMsgHandler(app, warning_handler);
	warnings = 0;
}

static void teardown(void)
{
	uninit_xt();
}

static void assert_warning(void)
{
	const char *source = getenv("MOTIF_TEST_MSGCAT_SOURCE");
	char *expect = source ? catalog_message(source, MESSAGE_ID) : strdup(BUILT_IN);

	ck_assert_int_eq(warnings, 1);
	ck_assert_str_eq(warning, expect);
	if (source)
		ck_assert_str_ne(warning, BUILT_IN);
	free(expect);
}

/* At creation */
START_TEST(warning_at_create)
{
	Arg args[1];

	XtSetArg(args[0], XmNfractionBase, 0);
	XmCreateForm(top, "form", args, 1);
	assert_warning();
}
END_TEST

/* From XtSetValues */
START_TEST(warning_at_set_values)
{
	Widget form = XmCreateForm(top, "form", NULL, 0);
	int base = 0;

	ck_assert_int_eq(warnings, 0);
	XtVaSetValues(form, XmNfractionBase, 0, NULL);
	assert_warning();
	/* The value was refused */
	XtVaGetValues(form, XmNfractionBase, &base, NULL);
	ck_assert_int_eq(base, 100);
}
END_TEST

void msgcat_suite(SRunner *runner)
{
	Suite *s = suite_create("MsgCat");
	TCase *t = tcase_create("Message catalogs");

	tcase_add_checked_fixture(t, setup, teardown);
	tcase_add_test(t, warning_at_create);
	tcase_add_test(t, warning_at_set_values);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}
