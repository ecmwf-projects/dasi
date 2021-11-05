
ecbuild_add_library(
    TARGET dasi
    TYPE SHARED
    INSTALL_HEADERS LISTED
    DEPENDS yaml-cpp
    PRIVATE_LIBS
        "${YAML_CPP_LIBRARIES}"
    PRIVATE_INCLUDES
        "${YAML_CPP_INCLUDE_DIRS}"
    PUBLIC_INCLUDES
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    LINKER_LANGUAGE CXX
    SOURCES
        api/Dasi.cc
        api/Dasi.h
        api/Key.cc
        api/Key.h
        api/SplitKey.cc
        api/SplitKey.h
        api/Query.cc
        api/Query.h
        api/Query.cc
        api/Query.h

        core/ArchiveVisitor.cc
        core/ArchiveVisitor.h
        core/Schema.cc
        core/Schema.h

        util/Colour.cc
        util/Colour.h
        util/ContainerIostream.h
        util/Exceptions.cc
        util/Exceptions.h
        util/IndentStream.h
        util/Test.h
        util/CartesianProduct.h
)

ecbuild_add_executable(
    TARGET main
    SOURCES main.cc
    LIBS dasi
)
