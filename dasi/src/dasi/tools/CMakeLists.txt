# Tool wrapper script

configure_file(dasi.in ${CMAKE_BINARY_DIR}/bin/dasi @ONLY)
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/dasi DESTINATION ${INSTALL_BIN_DIR} )

foreach( _tool info list put get policy )
    ecbuild_add_executable( TARGET     dasi-${_tool}
                            SOURCES    DASITool.cc
                                       DASITool.h
                                       dasi-${_tool}.cc
                            LIBS       dasi
                                       eckit_option )
endforeach()