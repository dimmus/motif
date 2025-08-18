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
 *
 */
/*
 * HISTORY
 */
/* $XConsortium: ImageCachI.h /main/7 1996/01/29 13:19:43 daniel $ */
#ifndef _XmImageCacheI_h
#define _XmImageCacheI_h

#include <Xm/XmP.h>

/* this name is used by XmeGetPixmapData to cache a pixmap in the
   pixmap cache with no associated name. _XmCachePixmap knows about it
   and will not add this one in the pixmap_data cache because it is a
   _name_ based pixmap cache used during conversion */
#define DIRECT_PIXMAP_CACHED ""

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for ImageCache.c    ********/

extern Boolean _XmInstallImage(
                        XImage *image,
                        char *image_name,
                        int hot_x,
                        int hot_y) ;
extern Boolean _XmGetImage(
                        Screen *screen,
                        char *image_name,
                        XImage **image) ;
extern Boolean _XmCachePixmap(
			      Pixmap pixmap,
			      Screen *screen,
			      char *image_name,
			      Pixel foreground,
			      Pixel background,
			      int depth,
			      Dimension width,
			      Dimension height) ;
extern Pixmap _XmGetColoredPixmap(Screen *screen,
				  char *image_name,
				  XmAccessColorData acc_color,
				  int depth,
#if NeedWidePrototypes
				  int only_if_exists) ;
#else
				  Boolean only_if_exists) ;
#endif /* NeedWidePrototypes */

extern Boolean _XmGetPixmapData(
		   Screen *screen,
		   Pixmap pixmap,
		   char **image_name,
		   int *depth,
		   Pixel *foreground,
		   Pixel *background,
		   int *hot_x,
		   int *hot_y,
		   unsigned int *width,
		   unsigned int *height) ;
extern Boolean _XmInImageCache(
			       String image_name);

extern Pixmap _XmGetScaledPixmap(
    Screen *screen,
    Widget widget,
    char *image_name,
    XmAccessColorData acc_color,
    int depth,
#if NeedWidePrototypes
    int only_if_exists,
#else
    Boolean only_if_exists,
#endif /* NeedWidePrototypes */
    double scaling_ratio);

extern void _XmPutScaledImage (
    Display*		 display ,
    Drawable		 d ,
    GC			 gc ,
    XImage*		 src_image ,
    int			 src_x ,
    int			 src_y ,
    int			 dest_x ,
    int			 dest_y ,
    unsigned int	 src_width ,
    unsigned int	 src_height,
    unsigned int	 dest_width ,
    unsigned int	 dest_height);

extern void _XmCleanPixmapCache(Screen * screen, Widget shell);

/* for Xm.h */
extern Pixmap XmGetScaledPixmap(
    Widget widget,
    char *image_name,
    Pixel foreground,
    Pixel background,
    int depth,
    double scaling_ratio);

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmImageCacheI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
