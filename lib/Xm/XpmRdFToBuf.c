/*
 * Copyright (C) 1989-95 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */
/*****************************************************************************\
* RdFToBuf.c:                                                                 *
*                                                                             *
*  XPM library                                                                *
*  Copy a file to a malloc'ed buffer, provided as a convenience.              *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/
/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */
/* October 2004, source code review by Thomas Biege <thomas@suse.de> */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "XpmI.h"
#include <sys/stat.h>
#if !defined(FOR_MSW) && !defined(WIN32)
#include <unistd.h>
#endif
#ifndef VAX11C
#include <fcntl.h>
#endif
#if defined(FOR_MSW) || defined(WIN32)
#include <io.h>
#define stat _stat
#define fstat _fstat
#define fdopen _fdopen
#define O_RDONLY _O_RDONLY
#endif
int
XpmReadFileToBuffer(
    const char	 *filename,
    char	**buffer_return)
{
    int fd, fcheck;
    off_t len;
    char *ptr;
    struct stat stats;
    FILE *fp;
    *buffer_return = NULL;
#ifndef VAX11C
    fd = open(filename, O_RDONLY);
#else
    fd = open(filename, O_RDONLY, NULL);
#endif
    if (fd < 0)
	return XpmOpenFailed;
    if (fstat(fd, &stats)) {
	close(fd);
	return XpmOpenFailed;
    }
    fp = fdopen(fd, "r");
    if (!fp) {
	close(fd);
	return XpmOpenFailed;
    }
    len = stats.st_size;
    if (len < 0 || len >= SIZE_MAX) {
	close(fd);
	return XpmOpenFailed;
    }
    ptr = (char *) XpmMalloc(len + 1);
    if (!ptr) {
	fclose(fp);
	return XpmNoMemory;
    }
    fcheck = fread(ptr, 1, len, fp);
    fclose(fp);
    if (fcheck != len) {
	XpmFree(ptr);
	return XpmOpenFailed;
    }
    ptr[len] = '\0';
    *buffer_return = ptr;
    return XpmSuccess;
}
