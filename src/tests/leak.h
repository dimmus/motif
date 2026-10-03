/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * LeakSanitizer helpers for the tests.
 *
 * KNOWN_LEAK_BEGIN() / KNOWN_LEAK_END() exclude the allocations made
 * between them from leak checking.  Use them only around a call that
 * leaks because of a documented library bug, and say which in a
 * comment, so that the rest of the test is still checked.
 *
 * HAVE_LSAN is defined when the tests are built with LeakSanitizer;
 * leaks_found() then runs a leak check on the spot and returns the
 * number of leaked objects reported (it prints them as usual).
 */
#ifndef TESTS_LEAK_H
#define TESTS_LEAK_H

/* LeakSanitizer is part of ASan with GCC (__SANITIZE_ADDRESS__) and Clang. */
#if defined(__SANITIZE_ADDRESS__)
#define HAVE_LSAN 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define HAVE_LSAN 1
#endif
#endif

#ifdef HAVE_LSAN
#include <sanitizer/lsan_interface.h>
#define KNOWN_LEAK_BEGIN() __lsan_disable()
#define KNOWN_LEAK_END() __lsan_enable()
#define leaks_found() __lsan_do_recoverable_leak_check()
#else
#define KNOWN_LEAK_BEGIN() ((void)0)
#define KNOWN_LEAK_END() ((void)0)
#endif

#endif /* TESTS_LEAK_H */
