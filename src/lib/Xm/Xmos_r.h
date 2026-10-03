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

/*
 * <Xm/Xmos_r.h> used to be a private copy of <X11/Xos_r.h>.  Motif no
 * longer uses it; it is kept so that programs that include it still
 * build, and now simply includes the X.Org header, which every supported
 * libX11 provides.
 *
 * As with <X11/Xos_r.h>, define the X_INCLUDE_*_H macros for the
 * interfaces wanted before including it.  There is deliberately no
 * include guard: <X11/Xos_r.h> may be included several times with
 * different X_INCLUDE_*_H macros.
 */
#include <X11/Xos_r.h>
