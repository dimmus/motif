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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Mrminit.c /main/14 1999/05/19 15:25:33 mgreess $"
#endif
#endif

/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *  ABSTRACT:
 *
 *	This contains only the top-level routine MrmIntialize. It can be
 *	modified by vendors as needed to add or remove widgets being \
 *	initialized for URM facilities. This routine is normally accessible to
 *	and used by an application at runtime to access URM facilities.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>
#include <X11/ConstrainP.h>
#include <X11/ShellP.h>
#include <X11/VendorP.h>
#include <X11/RectObjP.h>

#ifdef DXM_V11
#include <DXm/DXmHelpB.h>
#include <DXm/DXmHelpBp.h>
#endif

#include <Xm/XmP.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/LabelGP.h>
#include <Xm/LabelP.h>
#include <Xm/RowColumn.h>
#include <Xm/RowColumnP.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/ArrowBGP.h>
#include <Xm/ArrowBP.h>
#include <Xm/AtomMgr.h>
#include <Xm/BulletinB.h>
#include <Xm/BulletinBP.h>
#include <Xm/ButtonBox.h>
#include <Xm/ButtonBoxP.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/Command.h>
#include <Xm/CommandP.h>
#include <Xm/ColorS.h>
#include <Xm/ColorSP.h>
#include <Xm/CutPaste.h>
#include <Xm/DataF.h>
#include <Xm/DataFP.h>
#include <Xm/DialogS.h>
#include <Xm/DialogSP.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawingAP.h>
#include <Xm/DrawnB.h>
#include <Xm/DrawnBP.h>
#include <Xm/MultiList.h>
#include <Xm/MultiListP.h>
#include <Xm/DropDown.h>
#include <Xm/DropDownP.h>
#include <Xm/FileSB.h>
#include <Xm/FileSBP.h>
#include <Xm/FontS.h>
#include <Xm/FontSP.h>
#include <Xm/Form.h>
#include <Xm/FormP.h>
#include <Xm/Frame.h>
#include <Xm/FrameP.h>
#include <Xm/IconButton.h>
#include <Xm/IconButtonP.h>
#include <Xm/IconBox.h>
#include <Xm/IconBoxP.h>
#include <Xm/List.h>
#include <Xm/ListP.h>
#include <Xm/MainW.h>
#include <Xm/MainWP.h>
#include <Xm/MenuShell.h>
#include <Xm/MenuShellP.h>
#include <Xm/MessageB.h>
#include <Xm/MessageBP.h>
#include <Xm/Outline.h>
#include <Xm/OutlineP.h>
#include <Xm/Paned.h>
#include <Xm/PanedP.h>
#include <Xm/PanedW.h>
#include <Xm/PanedWP.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/PushBGP.h>
#include <Xm/PushBP.h>
#include <Xm/SashP.h>
#include <Xm/Scale.h>
#include <Xm/ScaleP.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrollBarP.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrolledWP.h>
#include <Xm/SelectioBP.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>
#include <Xm/SeparatoGP.h>
#include <Xm/Separator.h>
#include <Xm/SeparatorP.h>
#include <Xm/TabBox.h>
#include <Xm/TabBoxP.h>
#include <Xm/TabStack.h>
#include <Xm/TabStackP.h>
#include <Xm/Text.h>
#include <Xm/TextFP.h>
#include <Xm/TextP.h>
#include <Xm/TextOutP.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleBGP.h>
#include <Xm/ToggleBP.h>
#include <Xm/TreeP.h>
#include <Xm/Tree.h>
#include <Xm/ComboBoxP.h>
#include <Xm/ContainerP.h>
#include <Xm/NotebookP.h>
#include <Xm/SpinBP.h>
#include <Xm/SSpinBP.h>
#include <Xm/IconGP.h>

#include <Xm/Column.h>
#include <Xm/ColumnP.h>

#include <Xm/XmRenderTI.h>
#include <Xm/XmTabListI.h>


/*
 *
 *  TABLE OF CONTENTS
 *
 *	MrmInitialize			Initialize URM internals before use
 *
 */


/*
 *
 *  OWN VARIABLE DECLARATIONS
 *
 */

/*
 * The following flag is set to indicate successful URM initialization
 */
static	Boolean	urm__initialize_complete = FALSE;


/*
 *++
 *  PROCEDURE DESCRIPTION:
 *
 *	MrmInitialize must be called in order to prepare an application to
 *	use URM widget fetching facilities. It initializes the internal data
 *	structures (creating the mapping from class codes to the creation
 *	routine for each builtin widget class) which URM needs in order to
 *	successfully perform type conversion on arguments, and successfully
 *	access widget creation facilities. MrmInitialize must be called before
 *	any widgets are	created, whether by URM's fetch mechanisms or directly
 *	by the application. It may be called before or after XtInitialize, and
 *	multiple calls after the first one are benign (no-ops).
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

void MrmInitialize (void)
{

  /*
   *  Local variables
   */

  _MrmProcessLock();
  /*
   * Initialize only once
   */
  if ( urm__initialize_complete )
    {
      _MrmProcessUnlock();
      return ;
    }

  /*
   * Initialize the class descriptors for all the known widgets.
   */
#ifdef DXM_V11
  MrmRegisterClass
    (0, NULL, "DXmCreateHelpWidget", DXmCreateHelp,
     (WidgetClass)&dxmhelpwidgetclassrec);
#endif

  MrmRegisterClass
    (0, NULL, "XmCreateArrowButton", (Widget (*)())XmCreateArrowButton,
     (WidgetClass)&xmArrowButtonClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateArrowButtonGadget", (Widget (*)())XmCreateArrowButtonGadget,
     (WidgetClass)&xmArrowButtonGadgetClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateBulletinBoard", (Widget (*)())XmCreateBulletinBoard,
     (WidgetClass)&xmBulletinBoardClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateBulletinBoardDialog", (Widget (*)())XmCreateBulletinBoardDialog,
     (WidgetClass)&xmBulletinBoardClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateButtonBox", (Widget (*)())XmCreateButtonBox,
     (WidgetClass)&xmButtonBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateCascadeButton", (Widget (*)())XmCreateCascadeButton,
     (WidgetClass)&xmCascadeButtonClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateCascadeButtonGadget", (Widget (*)())XmCreateCascadeButtonGadget,
     (WidgetClass)&xmCascadeButtonGadgetClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateColumn", (Widget (*)())XmCreateColumn,
     (WidgetClass)&xmColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateCommand", (Widget (*)())XmCreateCommand,
     (WidgetClass)&xmCommandClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateCommandDialog", (Widget (*)())XmCreateCommandDialog,
     (WidgetClass)&xmCommandClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateColorSelector", (Widget (*)())XmCreateColorSelector,
     (WidgetClass)&xmColorSelectorClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateDataField", (Widget (*)())XmCreateDataField,
     (WidgetClass)&xmDataFieldClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateDialogShell", (Widget (*)())XmCreateDialogShell,
     (WidgetClass)&xmDialogShellClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateDrawingArea", (Widget (*)())XmCreateDrawingArea,
     (WidgetClass)&xmDrawingAreaClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateMultiList", (Widget (*)())XmCreateMultiList,
     (WidgetClass)&xmMultiListClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateDrawnButton", (Widget (*)())XmCreateDrawnButton,
     (WidgetClass)&xmDrawnButtonClassRec);

   MrmRegisterClass
    (0, NULL, "XmCreateFileSelectionBox", (Widget (*)())XmCreateFileSelectionBox,
     (WidgetClass)&xmFileSelectionBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateFileSelectionDialog", (Widget (*)())XmCreateFileSelectionDialog,
     (WidgetClass)&xmFileSelectionBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateForm", (Widget (*)())XmCreateForm,
     (WidgetClass)&xmFormClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateFontSelector", (Widget (*)())XmCreateFontSelector,
     (WidgetClass)&xmFontSelectorClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateFormDialog", (Widget (*)())XmCreateFormDialog,
     (WidgetClass)&xmFormClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateFrame", (Widget (*)())XmCreateFrame,
     (WidgetClass)&xmFrameClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateIconButton", (Widget (*)())XmCreateIconButton,
     (WidgetClass)&xmIconButtonClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateIconBox", (Widget (*)())XmCreateIconBox,
     (WidgetClass)&xmIconBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateLabel", (Widget (*)())XmCreateLabel,
     (WidgetClass)&xmLabelClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateLabelGadget", (Widget (*)())XmCreateLabelGadget,
     (WidgetClass)&xmLabelGadgetClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateList", (Widget (*)())XmCreateList,
     (WidgetClass)&xmListClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateScrolledList", (Widget (*)())XmCreateScrolledList,
     (WidgetClass)&xmListClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateMainWindow", (Widget (*)())XmCreateMainWindow,
     (WidgetClass)&xmMainWindowClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateMenuShell", (Widget (*)())XmCreateMenuShell,
     (WidgetClass)&xmMenuShellClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateMessageBox", (Widget (*)())XmCreateMessageBox,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateOutline", (Widget (*)())XmCreateOutline,
     (WidgetClass)&xmOutlineClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateMessageDialog", (Widget (*)())XmCreateMessageDialog,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateErrorDialog", (Widget (*)())XmCreateErrorDialog,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateInformationDialog", (Widget (*)())XmCreateInformationDialog,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateQuestionDialog", (Widget (*)())XmCreateQuestionDialog,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateTemplateDialog", (Widget (*)())XmCreateTemplateDialog,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateWarningDialog", (Widget (*)())XmCreateWarningDialog,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateWorkingDialog", (Widget (*)())XmCreateWorkingDialog,
     (WidgetClass)&xmMessageBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreatePushButton", (Widget (*)())XmCreatePushButton,
     (WidgetClass)&xmPushButtonClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreatePushButtonGadget", (Widget (*)())XmCreatePushButtonGadget,
     (WidgetClass)&xmPushButtonGadgetClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateRowColumn", (Widget (*)())XmCreateRowColumn,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateWorkArea", (Widget (*)())XmCreateWorkArea,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateRadioBox", (Widget (*)())XmCreateRadioBox,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateMenuBar", (Widget (*)())XmCreateMenuBar,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateOptionMenu", (Widget (*)())XmCreateOptionMenu,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreatePopupMenu", (Widget (*)())XmCreatePopupMenu,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreatePulldownMenu", (Widget (*)())XmCreatePulldownMenu,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateSimpleCheckBox", (Widget (*)())XmCreateSimpleCheckBox,
     (WidgetClass)&xmRowColumnClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateScale", (Widget (*)())XmCreateScale,
     (WidgetClass)&xmScaleClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateScrolledWindow", (Widget (*)())XmCreateScrolledWindow,
     (WidgetClass)&xmScrolledWindowClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateScrollBar", (Widget (*)())XmCreateScrollBar,
     (WidgetClass)&xmScrollBarClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateSelectionBox", (Widget (*)())XmCreateSelectionBox,
     (WidgetClass)&xmSelectionBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateSelectionDialog", (Widget (*)())XmCreateSelectionDialog,
     (WidgetClass)&xmSelectionBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreatePromptDialog", (Widget (*)())XmCreatePromptDialog,
     (WidgetClass)&xmSelectionBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateSeparator", (Widget (*)())XmCreateSeparator,
     (WidgetClass)&xmSeparatorClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateSeparatorGadget", (Widget (*)())XmCreateSeparatorGadget,
     (WidgetClass)&xmSeparatorGadgetClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateTabBox", (Widget (*)())XmCreateTabBox,
     (WidgetClass)&xmTabBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateTabStack", (Widget (*)())XmCreateTabStack,
     (WidgetClass)&xmTabStackClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateText", (Widget (*)())XmCreateText,
     (WidgetClass)&xmTextClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateTextField", (Widget (*)())XmCreateTextField,
     (WidgetClass)&xmTextFieldClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateScrolledText", (Widget (*)())XmCreateScrolledText,
     (WidgetClass)&xmTextClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateToggleButton", (Widget (*)())XmCreateToggleButton,
     (WidgetClass)&xmToggleButtonClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateToggleButtonGadget", (Widget (*)())XmCreateToggleButtonGadget,
     (WidgetClass)&xmToggleButtonGadgetClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateTree", (Widget (*)())XmCreateTree,
     (WidgetClass)&xmTreeClassRec);

    MrmRegisterClass
    (0, NULL, "XmCreatePaned", (Widget (*)())XmCreatePaned,
     (WidgetClass)&xmPanedClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreatePanedWindow", (Widget (*)())XmCreatePanedWindow,
     (WidgetClass)&xmPanedWindowClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateComboBox", (Widget (*)())XmCreateComboBox,
     (WidgetClass)&xmComboBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateContainer", (Widget (*)())XmCreateContainer,
     (WidgetClass)&xmContainerClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateNotebook", (Widget (*)())XmCreateNotebook,
     (WidgetClass)&xmNotebookClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateSpinBox", (Widget (*)())XmCreateSpinBox,
     (WidgetClass)&xmSpinBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateSimpleSpinBox", (Widget (*)())XmCreateSimpleSpinBox,
     (WidgetClass)&xmSimpleSpinBoxClassRec);

  MrmRegisterClass
    (0, NULL, "XmCreateIconGadget", (Widget (*)())XmCreateIconGadget,
     (WidgetClass)&xmIconGadgetClassRec);

  MrmRegisterClassWithCleanup
    (0, NULL, "_XmCreateRenderTable", (Widget (*)())_XmCreateRenderTable,
     (WidgetClass)NULL, (void (*)())XmRenderTableFree);

  MrmRegisterClassWithCleanup
    (0, NULL, "_XmCreateRendition", (Widget (*)())_XmCreateRendition,
     (WidgetClass)NULL, (void (*)())XmRenditionFree);

  MrmRegisterClass
    (0, NULL, "_XmCreateTabList", (Widget (*)())_XmCreateTabList,
     (WidgetClass)NULL);

  MrmRegisterClass
    (0, NULL, "_XmCreateTab", (Widget (*)())_XmCreateTab,
     (WidgetClass)NULL);

  MrmRegisterClass
    (0, NULL, "XmCreateDropDown", (Widget (*)())XmCreateDropDown,
     (WidgetClass)&xmDropDownClassRec);

  /*
   * Initialization complete
   */
  urm__initialize_complete = TRUE ;
  _MrmProcessUnlock();

}
