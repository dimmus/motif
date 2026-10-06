# Sample ~/.motifbind files.  Motif does not search this directory; users
# copy a file to ~/.motifbind or load it with xmbind.
install(FILES
    CDE
    pc
    DESTINATION ${CMAKE_INSTALL_DATADIR}/X11/bindings
)
