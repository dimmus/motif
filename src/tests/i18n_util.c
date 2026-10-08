/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Locale helpers for the i18n suites.
 */
#include <errno.h>
#include <iconv.h>
#include <langinfo.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <check.h>

#include "i18n_util.h"

static int try_locale(const char *name)
{
	if (!setlocale(LC_ALL, name) || !XSupportsLocale()) {
		setlocale(LC_ALL, "C");
		return 0;
	}
	setenv("LC_ALL", name, 1);
	return 1;
}

const char *test_set_locale(const char *const *fallbacks)
{
	static char env_name[256];
	const char *name = getenv("MOTIF_TEST_LOCALE");
	int i;

	if (name && *name) {
		ck_assert_msg(try_locale(name),
			      "cannot set the locale %s (LOCPATH=%s)", name,
			      getenv("LOCPATH") ? getenv("LOCPATH") : "");
		return name;
	}
	for (i = 0; fallbacks && fallbacks[i]; i++)
		if (try_locale(fallbacks[i]))
			return fallbacks[i];
	/* LC_CTYPE's: with categories set apart LC_ALL's name is a list */
	if (!(name = setlocale(LC_CTYPE, "")))
		return NULL;
	snprintf(env_name, sizeof env_name, "%s", name);
	return try_locale(env_name) ? env_name : NULL;
}

const char *test_codeset(void)
{
	return nl_langinfo(CODESET);
}

char *test_from_utf8(const char *utf8)
{
	size_t in_left = strlen(utf8), out_left = 4 * in_left + 4;
	char *out = malloc(out_left), *in = (char *)utf8, *o = out;
	iconv_t cd;

	ck_assert_ptr_nonnull(out);
	cd = iconv_open(test_codeset(), "UTF-8");
	if (cd == (iconv_t)-1) {
		free(out);
		return NULL;
	}
	if (iconv(cd, &in, &in_left, &o, &out_left) == (size_t)-1 ||
	    iconv(cd, NULL, NULL, &o, &out_left) == (size_t)-1) {
		iconv_close(cd);
		free(out);
		return NULL;
	}
	iconv_close(cd);
	*o = '\0';
	return out;
}

int test_mbslen(const char *s)
{
	size_t n = mbstowcs(NULL, s, 0);

	ck_assert_msg(n != (size_t)-1, "invalid multibyte string");
	return (int)n;
}
