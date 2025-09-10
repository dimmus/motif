#ifndef MOTIF_SYSTEM_H
#define MOTIF_SYSTEM_H

/* Feature test macros must be defined before any includes */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED 1
#endif
#ifndef _SVID_SOURCE
#define _SVID_SOURCE
#endif
#ifndef _ATFILE_SOURCE
#define _ATFILE_SOURCE
#endif

/* Standard C includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

/* POSIX and system includes for user/group functions */
#include <pwd.h>
#include <grp.h>

/* System includes needed for Motif compilation */
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#else
#include <alloca.h>
#endif

#ifdef HAVE_ICONV_H
#include <iconv.h>
#else
#include <iconv.h>
#endif

#ifdef HAVE_WCTYPE_H
#include <wctype.h>
#else
#include <wctype.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_POLL_H
#include <sys/poll.h>
#else
#include <sys/poll.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <sys/time.h>
#endif

#ifdef HAVE_X11_XOS_R_H
#include <X11/Xos_r.h>
#endif

#ifdef HAVE_X11_XPOLL_H
#include <X11/Xpoll.h>
#endif

/* Regular expression support */
#ifdef HAVE_REGEX
#include <regex.h>
#elif defined(HAVE_RE_COMP)
/* Some systems have re_comp/re_exec instead of regex */
extern int re_comp(const char *);
extern int re_exec(const char *);
#else
#include <regex.h>
#endif

/* X11 includes - these must come after system includes */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>

/* X11 compatibility fixes for missing internal APIs */
/* Note: _Xos_processLock and _Xos_processUnlock are defined globally in CMakeLists.txt */

#endif /* MOTIF_SYSTEM_H */