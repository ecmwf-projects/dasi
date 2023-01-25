
configure_file( lib/dasi_version.h.in lib/dasi_version.h )

ecbuild_add_library(
    TARGET dasi
    TYPE SHARED
    INSTALL_HEADERS LISTED
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/dasi

    PUBLIC_INCLUDES
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>

    SOURCES
        api/Dasi.cc
        api/Dasi.h
        api/Key.cc
        api/Key.h
        api/Query.cc
        api/Query.h

        api/detail/Generators.h
        api/detail/ListDetail.cc
        api/detail/ListDetail.h
        api/detail/PolicyDetail.cc
        api/detail/PolicyDetail.h
        api/detail/RetrieveDetail.cc
        api/detail/RetrieveDetail.h

        api/c/Dasi.h
        api/c/Dasi.cc

        impl/ListGeneratorImpl.cc
        impl/ListGeneratorImpl.h
        impl/PolicyStatusGeneratorImpl.cc
        impl/PolicyStatusGeneratorImpl.h
        impl/RetrieveResultImpl.cc
        impl/RetrieveResultImpl.h

        lib/LibDasi.cc
        lib/LibDasi.h

        tools/DASITool.cc
        tools/DASITool.h

    PUBLIC_LIBS
        eckit

    PRIVATE_LIBS
        fdb5
)

add_subdirectory( tools )
