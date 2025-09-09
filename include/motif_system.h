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

/* Additional system includes */
#include <alloca.h>
#include <iconv.h>
#include <wctype.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/time.h>

/* Regular expression support */
#include <regex.h>

/* X11 includes - these must come after system includes */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>

/* X11 compatibility fixes for missing internal APIs */
/* Note: _Xos_processLock and _Xos_processUnlock are defined globally in CMakeLists.txt */

#endif /* MOTIF_SYSTEM_H */
