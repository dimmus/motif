/**
 * Motif
 *
 * Licensed under the LGPL 2.1 license.
 *
 * Requests and round trips of the startup paths that the profiles of
 * doc/profiling.md found: work that the server is asked to do again
 * although the client already has the answer.
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <Xm/XmP.h>
#include <check.h>

#include "suites.h"

static Widget shell;

static void _init_xt(void)
{
	shell = init_xt("check_RoundTrips");
	/* Creating the XmDisplay registers the virtual binding converter
	 * and converts the default bindings once. */
	(void)XmGetXmDisplay(XtDisplay(shell));
}

#define MAX_KEYS 16

static Cardinal convert(const char *str, XmKeyBindingRec *keys)
{
	XrmValue from, to;

	from.addr = (XPointer)str;
	from.size = strlen(str) + 1;
	to.addr = (XPointer)keys;
	to.size = MAX_KEYS * sizeof(XmKeyBindingRec);
	if (!XtConvertAndStore(shell, XmRString, &from, XmRVirtualBinding, &to))
		return 0;
	return to.size / sizeof(XmKeyBindingRec);
}

/*
 * The binding of a keysym the way the converter computed it before it
 * used Xt's copy of the keyboard mapping: with the key's own mapping
 * from the server.  If the keysym is on a shifted column of its key,
 * the converter translates the key with the modifier of that column.
 */
static KeySym expected(Display *dpy, KeySym ks, Modifiers mods)
{
	KeyCode code = XKeysymToKeycode(dpy, ks);
	unsigned int state = 0;
	KeySym *map, result = ks;
	Modifiers used;
	int per, j;
	KeyCode min;

	if (!code)
		return ks;
	(void)XtGetKeysymTable(dpy, &min, &per);
	map = XGetKeyboardMapping(dpy, code, 1, &j);
	if (j < per)
		per = j;
	if (map[0] != ks)
		for (j = 1; j < per; j++)
			if (map[j] == ks) {
				state = 1 << (j - 1);
				break;
			}
	XFree(map);
	XtTranslateKey(dpy, code, state | mods, &used, &result);
	return result;
}

/*
 * Converting a virtual binding string issues no request: it used to
 * fetch the keyboard mapping of every key with XGetKeyboardMapping, one
 * round trip per binding (34 of the 91 round trips of a small program's
 * startup), although Xt keeps a copy of the whole mapping.
 */
START_TEST(virtual_binding_no_requests)
{
	static const char *str = "<Key>BackSpace,Shift<Key>Tab,<Key>exclam,"
				 "Ctrl<Key>a,<Key>KP_Enter,<Key>F10";
	Display *dpy = XtDisplay(shell);
	XmKeyBindingRec keys[MAX_KEYS];
	unsigned long before;
	Cardinal n;

	/* The first conversion may load the XKB map for XKeysymToKeycode. */
	n = convert(str, keys);
	ck_assert_uint_eq(n, 6);
	XSync(dpy, False);

	before = NextRequest(dpy);
	n = convert(str, keys);
	ck_assert_uint_eq(NextRequest(dpy), before);
	ck_assert_uint_eq(n, 6);
}
END_TEST

/* The bindings are those that the server's keyboard mapping gives. */
START_TEST(virtual_binding_values)
{
	static const struct {
		KeySym keysym;
		Modifiers mods;
	} in[] = {
		{ XK_BackSpace, 0 },
		{ XK_Tab, ShiftMask },
		{ XK_exclam, 0 },
		{ XK_a, ControlMask },
		{ XK_KP_Enter, 0 },
		{ XK_F10, 0 },
	};
	static const char *str = "<Key>BackSpace,Shift<Key>Tab,<Key>exclam,"
				 "Ctrl<Key>a,<Key>KP_Enter,<Key>F10";
	Display *dpy = XtDisplay(shell);
	XmKeyBindingRec keys[MAX_KEYS];
	Cardinal n, i;

	n = convert(str, keys);
	ck_assert_uint_eq(n, XtNumber(in));
	for (i = 0; i < n; i++) {
		ck_assert_msg(keys[i].keysym == expected(dpy, in[i].keysym,
							  in[i].mods),
			      "binding %u: keysym 0x%lx, expected 0x%lx", i,
			      (unsigned long)keys[i].keysym,
			      (unsigned long)expected(dpy, in[i].keysym,
						      in[i].mods));
		ck_assert_uint_eq(keys[i].modifiers, in[i].mods);
	}
	/* "exclam" is on the shifted column of its key with the usual
	 * keymaps: the converter must still find the key. */
	ck_assert_uint_ne(XKeysymToKeycode(dpy, XK_exclam), 0);
}
END_TEST

void round_trips_suite(SRunner *runner)
{
	Suite *s = suite_create("RoundTrips");
	TCase *tc = tcase_create("RoundTrips");

	tcase_add_checked_fixture(tc, _init_xt, uninit_xt);
	tcase_add_test(tc, virtual_binding_no_requests);
	tcase_add_test(tc, virtual_binding_values);
	suite_add_tcase(s, tc);
	srunner_add_suite(runner, s);
}
