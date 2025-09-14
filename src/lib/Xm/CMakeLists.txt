# Xm library sources
set(XM_SRCS1
    XmStrDefs.c
    ArrowB.c
    ArrowBG.c
    BulletinB.c
    CallbackI.c
    CascadeB.c
    CascadeBG.c
    Command.c
    CutPaste.c
    Dest.c
    DialogS.c
    DialogSE.c
    DragBS.c
    DragC.c
    DragOverS.c
    DragICC.c
    DragIcon.c
    DragUnder.c
    DrawingA.c
    DrawnB.c
    DropSMgr.c
    DropSMgrI.c
    DropTrans.c
    ExtObject.c
    FileSB.c
    Form.c
    Frame.c
    Gadget.c
    GadgetUtil.c
    GeoUtils.c
    ImageCache.c
    Label.c
    LabelG.c
    List.c
    MainW.c
    MapEvents.c
    MenuShell.c
    MenuUtil.c
    MessageB.c
    PanedW.c
    PushB.c
    PushBG.c
    ReadImage.c
    Region.c
    ResConvert.c
    ResInd.c
    RowColumn.c
    Sash.c
    Scale.c
    ScrollBar.c
    ScrolledW.c
    SelectioB.c
    SeparatoG.c
    Separator.c
    Simple.c
    Svg.c
    Text.c
    TextIn.c
    TextOut.c
    TextStrSo.c
    TextSel.c
    ToggleB.c
    ToggleBG.c
    TrackLoc.c
    UniqueEvnt.c
    VaSimple.c
    VirtKeys.c
    XmString.c
    ResEncod.c
    TextF.c
    TextFSel.c
    AtomMgr.c
    Cache.c
    Protocols.c
    Primitive.c
    Manager.c
    Traversal.c
    TravAct.c
    TraversalI.c
    BaseClass.c
)

set(XM_SRCS2
    Desktop.c
    Messages.c
    Transltns.c
    RepType.c
    Xmos.c
    Draw.c
    TearOff.c
    TearOffB.c
    XmIm.c
    GMUtils.c
    Display.c
    Screen.c
    ShellE.c
    VendorS.c
    VendorSE.c
    RCMenu.c
    RCLayout.c
    GetPixData.c
    RCPopup.c
    Trait.c
    Synthetic.c
    XmFontList.c
    ClipWindow.c
    Transfer.c
    Hash.c
    ScrollFrameT.c
    XmTabList.c
    Resolve.c
    XmStringFunc.c
    XmRenderT.c
    PixConv.c
    Color.c
    Xm.c
    CareVisualT.c
    Direction.c
    XmStringObso.c
    IsMwmRun.c
    MenuProc.c
    MenuState.c
    TextFind.c
    TextFunc.c
    ColorObj.c
    IconFile.c
    RCHook.c
    GetRes.c
    BBUtil.c
    ChColor.c
    ColObjFunc.c
    DrTog.c
    DrArrow.c
    DrPoly.c
    DrHiDash.c
    ValTime.c
    FocusAct.c
    CvtUnit.c
    ScalTics.c
    ScroVis.c
    WidGet.c
    Log.c
)

set(XM_OBSO_SRCS
    Obso1_2.c
    ObsoStr.c
    ObsoXme.c
    Obso2_0.c
)

set(XM_XPM_SRCS
    XpmAttrib.c
    XpmCrIFrDat.c
    XpmInfo.c
    XpmWrFFrDat.c
    Xpmmisc.c
    XpmCrBufFrI.c
    XpmCrIFrP.c
    XpmRdFToBuf.c
    XpmWrFFrI.c
    Xpmparse.c
    XpmCrBufFrP.c
    XpmCrPFrBuf.c
    XpmRdFToDat.c
    XpmWrFFrP.c
    Xpmrgb.c
    XpmCrDatFrI.c
    XpmCrPFrDat.c
    XpmRdFToI.c
    Xpmcreate.c
    Xpmscan.c
    XpmCrDatFrP.c
    XpmCrPFrI.c
    XpmRdFToP.c
    Xpmdata.c
    XpmCrIFrBuf.c
    XpmImage.c
    XpmWrFFrBuf.c
    Xpmhashtab.c
)

set(XM_NEW_WID_SRCS
    IconH.c
    Container.c
    IconG.c
    Notebook.c
    ComboBox.c
    GrabShell.c
    SpinB.c
    SSpinB.c
    TxtPropCv.c
    TranslNew.c
)

set(XM_EXTRA_SRCS
    XmExtUtil.c
    Column.c
    ButtonBox.c
    ColorS.c
    Picture.c
    DataFSel.c
    DataF.c
    MultiList.c
    I18List.c
    Paned.c
    FontS.c
    IconBox.c
    IconButton.c
    xmlist.c
    Hierarchy.c
    Outline.c
    TabBox.c
    TabList.c
    TabStack.c
    DrawUtils.c
    Tree.c
    DropDown.c
    XmStrDefs.ht
    XmStrDefs22.ht
    XmStrDefs23.ht
    XmStrDefsI.ht
)

set(XM_TIP_SRCS
    ToolTip.c
)

set(XM_SLIDE_SRCS
    SlideC.c
)

# Conditional sources
set(XM_JPEG_SRCS "")
set(XM_PNG_SRCS "")
set(XM_PRINTS_SRC "")

if(WITH_JPEG AND JPEG_FOUND)
    list(APPEND XM_JPEG_SRCS Jpeg.c)
endif()

if(WITH_PNG AND PNG_FOUND)
    list(APPEND XM_PNG_SRCS Png.c)
endif()

if(ENABLE_PRINTING AND XP_FOUND)
    list(APPEND XM_PRINTS_SRC PrintS.c)
endif()

# String definition files that need to be generated
set(XM_STRING_FILES
    XmStrDefs.h
    XmStrDefs22.h
    XmStrDefs23.h
    XmStrDefsI.h
    XmStrDefs.c
)

# Also create the include files that other targets need
set(XM_INCLUDE_FILES
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefs.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefs22.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefs23.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefsI.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h
)

# Generate xmstring.list from xmstring.list.in (equivalent to AC_CONFIG_FILES)
configure_file(
    ${CMAKE_SOURCE_DIR}/src/lib/Xm/xmstring.list.in
    ${CMAKE_BINARY_DIR}/src/lib/Xm/xmstring.list
    COPYONLY
)

# Generate string definition files using makestrs
add_custom_command(
    OUTPUT ${XM_STRING_FILES} ${XM_INCLUDE_FILES}
    COMMAND ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/generate_xm_strings.sh
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/src/bin/utils/makestrs
    DEPENDS ${CMAKE_BINARY_DIR}/src/bin/utils/makestrs ${CMAKE_BINARY_DIR}/src/bin/utils/mkcatdefs ${CMAKE_BINARY_DIR}/src/lib/Xm/xmstring.list
    COMMENT "Generating Xm string definition files"
)

# Generate message catalog header file
if(WITH_MESSAGE_CATALOG)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/Xm
        COMMAND ${CMAKE_BINARY_DIR}/src/bin/utils/mkcatdefs
            ${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h
            ${CMAKE_SOURCE_DIR}/localized/C/msg/Xm.msg
            > /dev/null 2>&1
        DEPENDS ${CMAKE_BINARY_DIR}/src/bin/utils/mkcatdefs ${CMAKE_SOURCE_DIR}/localized/C/msg/Xm.msg
        COMMENT "Generating Xm message catalog header"
    )
else()
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/Xm
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/src/lib/Xm/XmMsgCatI.h
            ${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h
        COMMENT "Copying Xm message catalog stub"
    )
endif()

# Create a custom target for the message catalog header
add_custom_target(xm_msgcat_header ALL DEPENDS ${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h)

# Create a custom target to ensure string files are generated
add_custom_target(xm_strings ALL DEPENDS ${XM_STRING_FILES} ${XM_INCLUDE_FILES})

# Create symlink for Xm.h in lib/Xm directory so ToolTipI.h can find it
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Xm.h
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/include/Xm/Xm.h ${CMAKE_CURRENT_SOURCE_DIR}/Xm.h
    DEPENDS ${CMAKE_BINARY_DIR}/include/Xm/Xm.h xm_strings
    COMMENT "Creating symlink for Xm.h in lib/Xm directory"
)

# Create a target for the Xm.h symlink
add_custom_target(xm_h_symlink ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Xm.h)

# All sources
set(XM_ALL_SRCS
    ${XM_SRCS1}
    ${XM_SRCS2}
    ${XM_OBSO_SRCS}
    ${XM_XPM_SRCS}
    ${XM_NEW_WID_SRCS}
    ${XM_EXTRA_SRCS}
    ${XM_TIP_SRCS}
    ${XM_SLIDE_SRCS}
    ${XM_JPEG_SRCS}
    ${XM_PNG_SRCS}
    ${XM_PRINTS_SRC}
)

# Headers
set(XM_HEADERS
    ArrowB.h
    ArrowBG.h
    ArrowBGP.h
    ArrowBP.h
    BulletinB.h
    BulletinBP.h
    CacheP.h
    CascadeB.h
    CascadeBG.h
    CascadeBGP.h
    CascadeBP.h
    Command.h
    CommandP.h
    CutPaste.h
    DialogS.h
    DialogSP.h
    DialogSEP.h
    DragC.h
    DragCP.h
    DragDrop.h
    DragIcon.h
    DragIconP.h
    DragOverS.h
    DragOverSP.h
    DropTransP.h
    DropSMgr.h
    DropSMgrP.h
    DropTrans.h
    DrawingA.h
    DrawingAP.h
    DrawnB.h
    DrawnBP.h
    FileSB.h
    FileSBP.h
    Form.h
    FormP.h
    Frame.h
    FrameP.h
    Label.h
    LabelG.h
    LabelGP.h
    LabelP.h
    List.h
    ListP.h
    MainW.h
    MainWP.h
    MenuShell.h
    MenuShellP.h
    MessageB.h
    MessageBP.h
    PanedW.h
    PanedWP.h
    PushB.h
    PushBG.h
    PushBGP.h
    PushBP.h
    RCLayoutP.h
    RowColumn.h
    RowColumnP.h
    SashP.h
    Scale.h
    ScaleP.h
    ScrollBar.h
    ScrollBarP.h
    ScrolledW.h
    ScrolledWP.h
    SelectioB.h
    SelectioBP.h
    SeparatoG.h
    SeparatoGP.h
    Separator.h
    SeparatorP.h
    TextF.h
    TextFP.h
    TextFSelP.h
    TextSelP.h
    Text.h
    TextInP.h
    TextOutP.h
    TextP.h
    TextStrSoP.h
    ToggleB.h
    ToggleBG.h
    ToggleBGP.h
    ToggleBP.h
    XmP.h
    BaseClassP.h
    ManagerP.h
    PrimitiveP.h
    GadgetP.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefs.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefs22.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefs23.h
    VaSimpleP.h
    VirtKeys.h
    VirtKeysP.h
    ExtObjectP.h
    Display.h
    DisplayP.h
    Screen.h
    ScreenP.h
    ShellEP.h
    VendorS.h
    VendorSP.h
    VendorSEP.h
    DesktopP.h
    AtomMgr.h
    Protocols.h
    ProtocolsP.h
    MwmUtil.h
    TransltnsP.h
    RepType.h
    XmosP.h
    Xmos_r.h
    DrawP.h
    Xmpoll.h
    TearOffP.h
    TearOffBP.h
    MenuUtilP.h
    MenuT.h
    XmAll.h
    TraitP.h
    SpinB.h
    SpinBP.h
    SSpinB.h
    SSpinBP.h
    Log.h
    Container.h
    ContainerP.h
    IconG.h
    IconGP.h
    Notebook.h
    NotebookP.h
    ContItemT.h
    ContainerT.h
    RCMenuP.h
    ClipWindowP.h
    TransferT.h
    Transfer.h
    AccTextT.h
    NavigatorT.h
    IconFileP.h
    ScrollFrameT.h
    GrabShell.h
    GrabShellP.h
    ComboBox.h
    ComboBoxP.h
    XmIm.h
    IconFile.h
    AccColorT.h
    ActivatableT.h
    JoinSideT.h
    SpecRenderT.h
    CareVisualT.h
    DialogSavvyT.h
    TransferP.h
    TakesDefT.h
    ColorP.h
    Primitive.h
    Manager.h
    Gadget.h
    LayoutT.h
    IconH.h
    IconHP.h
    UnitTypeT.h
    TxtPropCv.h
    MenuProcP.h
    MenuStateP.h
    ColorObjP.h
    TravConT.h
    UnhighlightT.h
    PointInT.h
    Print.h
    PrintSP.h
    XpmP.h
    Xmfuncs.h
    deprecated.h
    obsolete.h
    ${CMAKE_BINARY_DIR}/src/lib/Xm/version.h
)

# Extra headers
set(XM_EXTRA_HEADERS
    Ext.h
    ExtP.h
    Column.h
    ColumnP.h
    ButtonBox.h
    ButtonBoxP.h
    ColorS.h
    ColorSP.h
    Picture.h
    PictureP.h
    DataF.h
    DataFP.h
    DataFSelP.h
    Ext18List.h
    Ext18ListP.h
    MultiList.h
    MultiListP.h
    Paned.h
    PanedP.h
    FontS.h
    FontSP.h
    IconBox.h
    IconBoxP.h
    IconButton.h
    IconButtonP.h
    xmlist.h
    Hierarchy.h
    HierarchyP.h
    Outline.h
    OutlineP.h
    TabBox.h
    TabBoxP.h
    TabStack.h
    TabStackP.h
    TabList.h
    DrawUtils.h
    Tree.h
    TreeP.h
    ComboBox2.h
    ComboBox2P.h
    DropDown.h
    DropDownP.h
    ToolTipT.h
    ToolTipCT.h
)

# Slide headers
set(XM_SLIDE_HEADERS
    SlideC.h
    SlideCP.h
)

# Internal headers
set(XM_INTERNAL_HEADERS
    BaseClassI.h
    BitmapsI.h
    BulletinBI.h
    CacheI.h
    CallbackI.h
    CareVisualTI.h
    CascadeBGI.h
    CascadeBI.h
    ClipWindTI.h
    CntrbmI.h
    ColorI.h
    ColorObjI.h
    CommandI.h
    CutPasteI.h
    DestI.h
    DisplayI.h
    DragBSI.h
    DragCI.h
    DragICCI.h
    DragIconI.h
    DragOverSI.h
    DragUnderI.h
    DrawI.h
    DrawingAI.h
    DropSMgrI.h
    ExtObjectI.h
    GMUtilsI.h
    GadgetI.h
    GadgetUtiI.h
    GeoUtilsI.h
    HashI.h
    IconGI.h
    ImageCachI.h
    LabelGI.h
    LabelI.h
    ManagerI.h
    MapEventsI.h
    MenuProcI.h
    MenuShellI.h
    MenuStateI.h
    MenuUtilI.h
    MessageBI.h
    MessagesI.h
    nanosvg.h
    nanosvgrast.h
    PixConvI.h
    PrimitiveI.h
    PrintSI.h
    ProtocolsI.h
    RCHookI.h
    RCLayoutI.h
    RCMenuI.h
    ReadImageI.h
    RegionI.h
    RepTypeI.h
    ResConverI.h
    ResEncodI.h
    ResIndI.h
    RowColumnI.h
    ScreenI.h
    ScrollFramTI.h
    ScrolledWI.h
    SelectioBI.h
    SeparatoGI.h
    SvgI.h
    SyntheticI.h
    TearOffI.h
    TextDIconI.h
    TextFI.h
    TextFSelI.h
    TextI.h
    TextInI.h
    TextOutI.h
    TextSelI.h
    TextStrSoI.h
    ToggleBGI.h
    TraitI.h
    TransferI.h
    TravActI.h
    TraversalI.h
    UniqueEvnI.h
    VaSimpleI.h
    VendorSEI.h
    VendorSI.h
    VirtKeysI.h
    XmI.h
    XmImI.h
    XmMsgI.h
    XmRenderTI.h
    ${CMAKE_BINARY_DIR}/include/Xm/XmStrDefsI.h
    XmStringI.h
    XmTabListI.h
    XmosI.h
    XpmI.h
)

# Add conditional internal headers
if(WITH_JPEG AND JPEG_FOUND)
    list(APPEND XM_INTERNAL_HEADERS JpegI.h)
endif()

if(WITH_PNG AND PNG_FOUND)
    list(APPEND XM_INTERNAL_HEADERS PngI.h)
endif()

list(APPEND XM_INTERNAL_HEADERS ToolTipI.h)

# Create the library
add_library(Xm ${XM_ALL_SRCS} ${XM_STRING_FILES})

# Ensure message catalog header, string files, and Xm.h symlink are generated before building this target
add_dependencies(Xm xm_msgcat_header xm_strings xm_h_symlink)

# XM_MSGCAT is now defined globally in the main CMakeLists.txt

# Set library properties
set_target_properties(Xm PROPERTIES
    VERSION ${ABI_CURRENT}.${ABI_REVISION}.${ABI_AGE}
    SOVERSION ${ABI_CURRENT}
    OUTPUT_NAME "Xm"
)

# Include directories
target_include_directories(Xm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/lib/Xm>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/lib>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/lib/Xm>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/lib/Xm
        ${CMAKE_BINARY_DIR}/src/lib/Xm
        ${CMAKE_BINARY_DIR}/include
        ${X_INCLUDE_DIRS}
)

# Add system includes for compilation
target_compile_definitions(Xm PRIVATE
    _GNU_SOURCE
    _DEFAULT_SOURCE
    _POSIX_C_SOURCE=200809L
    _BSD_SOURCE
    _XOPEN_SOURCE=600
    _XOPEN_SOURCE_EXTENDED=1
    _SVID_SOURCE
    _ATFILE_SOURCE
)

# Add system includes
target_compile_options(Xm PRIVATE
    -include ${CMAKE_SOURCE_DIR}/include/motif_system.h
)

# Compile definitions
target_compile_definitions(Xm
    PRIVATE
        XMBINDDIR_FALLBACK="${CMAKE_INSTALL_DATADIR}/X11/bindings"
        INCDIR="${CMAKE_INSTALL_INCLUDEDIR}/X11"
        LIBDIR="${CMAKE_INSTALL_LIBDIR}/X11"
        DATADIR="${CMAKE_INSTALL_DATADIR}"
)

# Link libraries
target_link_libraries(Xm
    PUBLIC
        ${X_LIBRARIES}
    PRIVATE
        ${ICONV_LIBRARIES}
        m
        pthread
)

# Add system libraries for missing functions
find_library(REGEX_LIB regex)
if(REGEX_LIB)
    target_link_libraries(Xm PRIVATE ${REGEX_LIB})
endif()

# Check for re_comp/re_exec library (some systems have this instead of regex)
find_library(REGEX_COMP_LIB regex)
if(NOT REGEX_COMP_LIB)
    find_library(REGEX_COMP_LIB re)
endif()
if(REGEX_COMP_LIB)
    target_link_libraries(Xm PRIVATE ${REGEX_COMP_LIB})
endif()

# Add conditional libraries
if(XFT_FOUND)
    target_link_libraries(Xm PRIVATE ${XFT_LIBRARIES})
    target_include_directories(Xm PRIVATE ${XFT_INCLUDE_DIRS})
endif()

# Add FontConfig library (required by Xft)
if(FONTCONFIG_FOUND)
    target_link_libraries(Xm PRIVATE ${FONTCONFIG_LIBRARIES})
    target_include_directories(Xm PRIVATE ${FONTCONFIG_INCLUDE_DIRS})
endif()

if(JPEG_FOUND)
    target_link_libraries(Xm PRIVATE ${JPEG_LIBRARIES})
    target_include_directories(Xm PRIVATE ${JPEG_INCLUDE_DIRS})
endif()

if(PNG_FOUND)
    target_link_libraries(Xm PRIVATE ${PNG_LIBRARIES})
    target_include_directories(Xm PRIVATE ${PNG_INCLUDE_DIRS})
endif()

if(XP_FOUND)
    target_link_libraries(Xm PRIVATE ${XP_LIBRARIES})
    target_include_directories(Xm PRIVATE ${XP_INCLUDE_DIRS})
endif()

# Install the library
install(TARGETS Xm
    EXPORT XmTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES ${XM_HEADERS} ${XM_EXTRA_HEADERS} ${XM_SLIDE_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Xm
)

# Install internal headers (for development)
install(FILES ${XM_INTERNAL_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Xm
)

# Create Xm.h from template
configure_file(
    ${CMAKE_SOURCE_DIR}/src/lib/Xm/Xm.h.in
    ${CMAKE_BINARY_DIR}/include/Xm/Xm.h
    @ONLY
)

# Install Xm.h
install(FILES ${CMAKE_BINARY_DIR}/include/Xm/Xm.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Xm
)

# Generate version.h
configure_file(
    ${CMAKE_SOURCE_DIR}/src/lib/Xm/version.h.in
    ${CMAKE_BINARY_DIR}/src/lib/Xm/version.h
    @ONLY
)

# Install version.h
install(FILES ${CMAKE_BINARY_DIR}/src/lib/Xm/version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Xm
)

# Export targets
install(EXPORT XmTargets
    FILE XmTargets.cmake
    NAMESPACE Xm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Xm
)
