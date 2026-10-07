# WmWsmLib moved to src/lib/Wsm

# Mwm sources
set(MWM_SRCS
    WmCDInfo.c
    WmCDecor.c
    WmCEvent.c
    WmCPlace.c
    WmColormap.c
    WmError.c
    WmEvent.c
    WmFeedback.c
    WmFunction.c
    WmGraphics.c
    WmIDecor.c
    WmIPlace.c
    WmIconBox.c
    WmKeyFocus.c
    WmMain.c
    WmManage.c
    WmProperty.c
    WmResCvt.c
    WmResParse.c
    WmResource.c
    WmSignal.c
    WmWinConf.c
    WmWinInfo.c
    WmWinList.c
    WmWinState.c
    WmWsm.c
    WmXSMP.c
    version.c
    WmCmd.c
    WmImage.c
    WmInitWs.c
    WmMenu.c
    WmProtocol.c
)

set_source_files_properties(version.c
    PROPERTIES COMPILE_DEFINITIONS MWM_VERSION="${PROJECT_VERSION}")

# Create mwm executable
add_executable(mwm ${MWM_SRCS})

target_include_directories(mwm PRIVATE
    ${CMAKE_SOURCE_DIR}/src/lib/Wsm
)

target_compile_definitions(mwm PRIVATE
    LARGECURSORS
    R2_COMPAT
    UNMAP_ON_RESTART
    CDE_INSTALLATION_TOP="${CMAKE_INSTALL_PREFIX}/dt"
    CDE_CONFIGURATION_TOP="${CMAKE_INSTALL_PREFIX}/etc/dt"
    # Where data/bitmaps installs the bitmaps; the trailing slash is
    # expected by WmResource.c, as in its built-in default.
    BITMAPDIR="${CMAKE_INSTALL_FULL_INCLUDEDIR}/X11/bitmaps/"
    # Where system.mwmrc is installed, below
    MWMRCDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}/X11"
)

target_link_libraries(mwm
    Wsm
    Xm
    PkgConfig::MOTIF_XEXT
)

install(TARGETS mwm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install system.mwmrc where mwm looks for it (MWMRCDIR above).
# GNUInstallDirs maps the sysconfdir of the /usr prefix to /etc, not
# /usr/etc, so the full path is used when the sysconfdir is outside the
# prefix.  Otherwise the destination stays relative, so that
# "cmake --install --prefix <dir>" can still stage it below <dir>.
if(CMAKE_INSTALL_FULL_SYSCONFDIR STREQUAL
   "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}")
    set(MWM_RC_INSTALL_DIR ${CMAKE_INSTALL_SYSCONFDIR}/X11)
else()
    set(MWM_RC_INSTALL_DIR ${CMAKE_INSTALL_FULL_SYSCONFDIR}/X11)
endif()
install(FILES system.mwmrc
    DESTINATION ${MWM_RC_INSTALL_DIR}
)
