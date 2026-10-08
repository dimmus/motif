/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * Locale helpers for the i18n suites.
 */
#ifndef I18N_UTIL_H
#define I18N_UTIL_H

/*
 * Select the locale a suite runs in, before init_xt() (whose language
 * procedure calls setlocale(LC_ALL, "")): $MOTIF_TEST_LOCALE when it is
 * set (CTest sets it, with LOCPATH, for the locales generated at build
 * time; a test fails if that locale cannot be set), else the first of
 * fallbacks (NULL terminated, may be NULL) that setlocale() accepts and
 * Xlib supports, else the environment's locale.  The choice is exported
 * as LC_ALL.  Returns the locale name, or NULL when none of them could
 * be set.
 */
const char *test_set_locale(const char *const *fallbacks);

/* The codeset of the current LC_CTYPE, e.g. "UTF-8", "EUC-JP" */
const char *test_codeset(void);

/*
 * Convert UTF-8 text to the codeset of the current locale.  Returns a
 * string to free with free(), or NULL when the text cannot be
 * represented there.
 */
char *test_from_utf8(const char *utf8);

/* The number of characters of a string in the current locale */
int test_mbslen(const char *s);

#endif /* I18N_UTIL_H */
