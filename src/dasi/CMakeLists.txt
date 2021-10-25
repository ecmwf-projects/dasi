
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
        api/Key.cc
        api/Key.h
        api/Query.cc
        api/Query.h
        api/Schema.cc
        api/Schema.h

        core/Colour.cc
        core/Colour.h
        core/ContainerIostream.h
        core/Exceptions.cc
        core/Exceptions.h
        core/IndentStream.h
        core/Test.h
)

ecbuild_add_executable(
    TARGET main
    SOURCES main.cc
    LIBS dasi
)
