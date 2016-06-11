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
#ifndef _XmBaseClassP_h
#define _XmBaseClassP_h

#ifndef _XmNO_BC_INCL
#define _XmNO_BC_INCL
#endif

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif


#define _XmBCEPTR(wc)	((XmBaseClassExt *)(&(((WidgetClass)(wc))\
					      ->core_class.extension)))
#define _XmBCE(wc)	((XmBaseClassExt)(((WidgetClass)(wc))\
					  ->core_class.extension))

#define _XmGetBaseClassExtPtr(wc, owner) \
    ((_XmBCE(wc) && (((_XmBCE(wc))->record_type) == owner)) ? \
     _XmBCEPTR(wc) :  \
     ((XmBaseClassExt *) _XmGetClassExtensionPtr( \
						 ((XmGenericClassExt *)  \
						  _XmBCEPTR( wc)),  \
						 owner)))

/* defines for 256 bit (at least) bit field
 */
#define _XmGetFlagsBit(field, bit) \
	(field[ (bit >> 3) ]) & (1 << (bit & 0x07))

#define _XmSetFlagsBit(field, bit) \
	    (field[ (bit >> 3) ] |= (1 << (bit & 0x07)))


#ifndef XTHREADS
#define _XmFastSubclassInit(wc, bit_field) { \
	if((_Xm_fastPtr = _XmGetBaseClassExtPtr( wc, XmQmotif)) && \
	   (*_Xm_fastPtr)) \
		_XmSetFlagsBit((*_Xm_fastPtr)->flags, bit_field) ; \
   }

/* _XmGetBaseClassExtPtr can return NULL or a pointer to a NULL extension,
 * for non Motif classes in particular, so we check that up front.
 * We use the global _Xm_fastPtr for that purpose, this variable exists
 * already in BaseClass.c for apparently no other use.
 */

#define _XmIsFastSubclass(wc, bit) \
	((_Xm_fastPtr = _XmGetBaseClassExtPtr((wc),XmQmotif)) && \
         (*_Xm_fastPtr)) ? \
	     (_XmGetFlagsBit(((*_Xm_fastPtr)->flags), bit) ? TRUE : FALSE) \
		 : FALSE

#else
extern void _XmFastSubclassInit(WidgetClass, unsigned int);
extern Boolean _XmIsFastSubclass(WidgetClass, unsigned int);
#endif  /* XTHREADS */

#define XmBaseClassExtVersion 2L
#define XmBaseClassExtVersion 2L


typedef Cardinal (*XmGetSecResDataFunc)( WidgetClass,
					    XmSecondaryResourceData **);

typedef struct _XmObjectClassExtRec{
    XtPointer 		next_extension;	
    XrmQuark 		record_type;	
    long 		version;	
    Cardinal 		record_size;	
} XmObjectClassExtRec, *XmObjectClassExt;

typedef struct _XmGenericClassExtRec{
    XtPointer 		next_extension;	
    XrmQuark 		record_type;	
    long 		version;	
    Cardinal 		record_size;	
} XmGenericClassExtRec, *XmGenericClassExt;

typedef struct _XmWrapperDataRec{
    struct _XmWrapperDataRec *next;
    WidgetClass		widgetClass;
    XtInitProc		initializeLeaf;
    XtSetValuesFunc	setValuesLeaf;
    XtArgsProc		getValuesLeaf;
    XtRealizeProc	realize;
    XtWidgetClassProc	classPartInitLeaf;
    XtWidgetProc	resize;
    XtGeometryHandler   geometry_manager;

    /* init_depth is obselete now .. */
    Cardinal		init_depth;

    int                 initializeLeafCount;
    int                 setValuesLeafCount;
    int                 getValuesLeafCount;
    XtInitProc          constraintInitializeLeaf;
    XtSetValuesFunc     constraintSetValuesLeaf;
    int                 constraintInitializeLeafCount;
    int 		constraintSetValuesLeafCount;
} XmWrapperDataRec, *XmWrapperData;

typedef struct _XmBaseClassExtRec{
    XtPointer 		next_extension;	
    XrmQuark 		record_type;	
    long 		version;	
    Cardinal 		record_size;	
    XtInitProc		initializePrehook;
    XtSetValuesFunc 	setValuesPrehook;
    XtInitProc		initializePosthook;
    XtSetValuesFunc 	setValuesPosthook;
    WidgetClass		secondaryObjectClass;
    XtInitProc		secondaryObjectCreate;
    XmGetSecResDataFunc	getSecResData;
    unsigned char	flags[32];
    XtArgsProc		getValuesPrehook;
    XtArgsProc		getValuesPosthook;
    XtWidgetClassProc	classPartInitPrehook;
    XtWidgetClassProc	classPartInitPosthook;
    XtResourceList	ext_resources;
    XtResourceList	compiled_ext_resources;
    Cardinal		num_ext_resources;
    Boolean		use_sub_resources;
    XmWidgetNavigableProc widgetNavigable;
    XmFocusChangeProc	focusChange;
    XmWrapperData	wrapperData;
} XmBaseClassExtRec, *XmBaseClassExt;


typedef struct _XmWidgetExtDataRec{
    Widget		widget;
    Widget		reqWidget;
    Widget		oldWidget;
} XmWidgetExtDataRec, *XmWidgetExtData;

externalref XrmQuark	     XmQmotif;
externalref int		     _XmInheritClass;
externalref XmBaseClassExt * _Xm_fastPtr;
  
/********    Private Function Declarations    ********/


extern XmGenericClassExt * _XmGetClassExtensionPtr( 
                        XmGenericClassExt *listHeadPtr,
                        XrmQuark owner) ;
extern Boolean _XmIsSubclassOf(WidgetClass wc, WidgetClass sc);


/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmBaseClassP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
