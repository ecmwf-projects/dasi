
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
        api/Key.h
        api/Query.cc
        api/Query.h
        api/Query.cc
        api/Query.h

        core/Archiver.cc
        core/Archiver.h
        core/Schema.cc
        core/Schema.h
        core/KeyT.h
        core/OrderedReferenceKey.h
        core/SchemaKeyIterator.h
        core/SplitReferenceKey.cc
        core/SplitReferenceKey.h

        util/CartesianProduct.h
        util/Colour.cc
        util/Colour.h
        util/ContainerIostream.h
        util/Exceptions.cc
        util/Exceptions.h
        util/IndentStream.h
        util/LRUMap.h
        util/OrderedMap.h
        util/ReverseAdapter.h
        util/StringBuilder.h
        util/Test.h
)

ecbuild_add_executable(
    TARGET main
    SOURCES main.cc
    LIBS dasi
)
