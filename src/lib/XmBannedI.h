/*
 * Motif
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 */
/*
 * Ban the unbounded string functions from the library sources.
 *
 * The build passes this header with -include, together with
 * -Werror=deprecated-declarations, to every source file of libXm,
 * libMrm and libUil (see motif_ban_unsafe_string_functions in the top
 * level CMakeLists.txt); it is not installed and not meant for anything
 * else.  A new call of sprintf, vsprintf, strcpy or strcat then fails to
 * compile: use snprintf, vsnprintf, memcpy with a known length,
 * XtNewString or _XmConcatStrings instead.
 *
 * The functions are declared before <stdio.h> and <string.h> are
 * included: with _FORTIFY_SOURCE, glibc defines them there as inline
 * wrappers, and Clang ignores an attribute added after the definition.
 */
#ifndef _XmBannedI_h
#  define _XmBannedI_h

#  if defined(__GNUC__) || defined(__clang__)

#    include <stdarg.h>

#    define _XM_BANNED(instead) __attribute__((__deprecated__("unbounded, use " instead)))

/* The names are in parentheses in case they are function-like macros. */
extern int(sprintf)(char *, const char *, ...) _XM_BANNED("snprintf");
extern int(vsprintf)(char *, const char *, va_list) _XM_BANNED("vsnprintf");
extern char *(strcpy)(char *, const char *) _XM_BANNED("memcpy, snprintf or XtNewString");
extern char *(strcat)(char *, const char *) _XM_BANNED("memcpy, snprintf or _XmConcatStrings");

#    include <stdio.h>
#    include <string.h>
#    include <X11/Intrinsic.h>

/* For Clang, glibc's _FORTIFY_SOURCE makes sprintf a function-like macro
 * that calls the builtin directly: remove it so a call reaches the
 * declaration above. */
#    undef sprintf

/* Xt's XtNewString macro expands to strcpy: give it a bounded body. */
static inline char *_XmBannedNewString(const char *str)
{
  size_t size;
  if (str == NULL)
    return NULL;
  size = strlen(str) + 1;
  if (size != (Cardinal)size) { /* XtMalloc would truncate it */
    XtErrorMsg("allocError", "malloc", "XtToolkitError", "Cannot perform malloc", NULL, NULL);
    return NULL;
  }
  return (char *)memcpy(XtMalloc((Cardinal)size), str, size);
}
#    undef XtNewString
#    define XtNewString(str) _XmBannedNewString(str)

#  endif /* __GNUC__ || __clang__ */

#endif /* _XmBannedI_h */
