/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#ifndef SUITES_H
#define SUITES_H

#include <X11/Intrinsic.h>
#include <check.h>

extern XtAppContext app;

/* Xt fixture */
Widget init_xt(const char *klass);
void uninit_xt(void);

/* Suites */
void xmfontlistentry_suite(SRunner *runner);
void xmfontlist_suite(SRunner *runner);
void png_suite(SRunner *runner);
void jpeg_suite(SRunner *runner);
void svg_suite(SRunner *runner);
void log_suite(SRunner *runner);
void log_config_suite(SRunner *runner);
void xmstring_suite(SRunner *runner);
void xmstring_ct_suite(SRunner *runner);
void xmstring_extent_suite(SRunner *runner);
void widgets_suite(SRunner *runner);
void text_suite(SRunner *runner);
void clipboard_suite(SRunner *runner);
void i18n_suite(SRunner *runner);
void i18n_locale_suite(SRunner *runner);
void layout_suite(SRunner *runner);
void xerrors_suite(SRunner *runner);
void list_suite(SRunner *runner);
void msgcat_suite(SRunner *runner);
void rtl_suite(SRunner *runner);
void xim_suite(SRunner *runner);
void round_trips_suite(SRunner *runner);
void const_api_suite(SRunner *runner);
void rendertable_suite(SRunner *runner);
void traits_suite(SRunner *runner);
void gadgets_suite(SRunner *runner);
void xmim_suite(SRunner *runner);

#endif /* SUITES_H */
