# Install binding files
install(FILES
    xmbind.alias
    CDE
    dec
    doubleclick
    hal
    hp
    ibm
    pc
    sgi
    sony
    sun
    sun_at
    DESTINATION ${CMAKE_INSTALL_DATADIR}/X11/bindings
)
