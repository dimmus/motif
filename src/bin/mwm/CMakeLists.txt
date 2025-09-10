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

# Create mwm executable
add_executable(mwm ${MWM_SRCS})

target_include_directories(mwm PRIVATE
    ${CMAKE_BINARY_DIR}/lib
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/src/lib/Wsm
    ${X_INCLUDE_DIRS}
)

target_compile_definitions(mwm PRIVATE
    LARGECURSORS
    R2_COMPAT
    UNMAP_ON_RESTART
    CDE_INSTALLATION_TOP="${CMAKE_INSTALL_PREFIX}/dt"
    CDE_CONFIGURATION_TOP="${CMAKE_INSTALL_PREFIX}/etc/dt"
    BITMAPDIR="${CMAKE_INSTALL_INCLUDEDIR}/bitmaps"
    MWMRCDIR="${CMAKE_INSTALL_SYSCONFDIR}/X11"
)

target_link_libraries(mwm
    Wsm
    Xm
    ${X_LIBRARIES}
)

install(TARGETS mwm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install system.mwmrc
install(FILES system.mwmrc
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/X11
)
