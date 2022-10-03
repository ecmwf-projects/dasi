
ecbuild_add_library(
    TARGET dasi
    TYPE SHARED
    INSTALL_HEADERS LISTED
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/dasi

    PUBLIC_INCLUDES
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>

    SOURCES
        Dasi.cc
        Dasi.h
        Key.cc
        Key.h
        Query.cc
        Query.h

    PUBLIC_LIBS
        eckit
)