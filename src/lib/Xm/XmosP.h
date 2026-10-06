/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
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
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
#ifndef _XmosP_h
#  define _XmosP_h
/* For bzero, bcopy and bcmp. */
#  include <X11/Xfuncs.h>
/*
 * Modern systems use USER environment variable
 */
#  ifndef USER_VAR
#    define USER_VAR "USER"
#  endif
#  include <stdlib.h> /* Needed for MB_CUR_MAX, mbtowc, mbstowcs and mblen */
#  include <limits.h> /* for MB_LEN_MAX et al */
#  ifndef INT_MAX
#    define INT_MAX 2147483647
#  endif
#  ifndef LONG_MAX
#    define LONG_MAX 2147483647
#  endif
/**********************************************************************/
/* here we duplicate Xtos.h, since we can't include this private file */
#  if defined(INCLUDE_ALLOCA_H) || defined(HAVE_ALLOCA_H)
#    include <alloca.h>
#  endif
/*
 * Local allocation and deallocation: alloca() with GCC-compatible
 * compilers, XtMalloc() and XtFree() otherwise, on Solaris, or when
 * NO_ALLOCA is defined.
 */
#  if defined(__GNUC__) && !defined(sun) && !defined(NO_ALLOCA)
#    ifndef alloca /* gnu itself might have done that already */
#      define alloca __builtin_alloca
#    endif
#    define ALLOCATE_LOCAL(size) alloca(size)
#    define DEALLOCATE_LOCAL(ptr) /* as nothing */
#  endif
#  ifndef ALLOCATE_LOCAL
#    define ALLOCATE_LOCAL(size) XtMalloc(size)
#    define DEALLOCATE_LOCAL(ptr) XtFree(ptr)
#  endif /* ALLOCATE_LOCAL */
/* End of Xtos.h */
/*****************/
#  include <Xm/XmP.h>
/* For padding structures in Mrm we need to know how big pointers are. */
#  if !defined(__alpha)
#    define MrmShortPtr
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  define MATCH_CHAR \
    'P' /* referenced in InitPath strings and in the files \
    that uses it (ImageCache.c and Mrmhier.c) */
/* OS-dependent file info for VirtKeys */
#  define MOTIFBIND ".motifbind"

typedef enum { XmOS_METHOD_NULL, XmOS_METHOD_DEFAULTED, XmOS_METHOD_REPLACED } XmOSMethodStatus;

typedef XmDirection (*XmCharDirectionProc)(XtPointer /* char */,
                                           XmTextType /* type */,
                                           XmStringTag /* locale */);
typedef Status (*XmInitialDirectionProc)(XtPointer /* chars */,
                                         XmTextType /* type */,
                                         XmStringTag /* locale */,
                                         unsigned int * /* num_bytes */,
                                         XmDirection * /* direction */);
/********    Private Function Declarations    ********/
extern XmOSMethodStatus XmOSGetMethod(Widget w,
                                      String method_name,
                                      XtPointer *method,
                                      XtPointer *os_data);
/********    End Private Function Declarations    ********/
#  ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration which encloses file. */
#  endif
#endif /* _XmosP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
