
ecbuild_add_library(
    TARGET dasi
    TYPE SHARED
    INSTALL_HEADERS LISTED
    DEPENDS yaml-cpp
    PRIVATE_LIBS
        "${YAML_CPP_LIBRARIES}"
    PRIVATE_INCLUDES
        "${YAML_CPP_INCLUDE_DIR}"
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
        api/Config.h
        api/Config.cc
        api/SplitKey.h
        api/Handle.h

        core/Archiver.cc
        core/Archiver.h
        core/AggregatedHandle.cc
        core/AggregatedHandle.h
        core/Catalogue.cc
        core/Catalogue.h
        core/DB.cc
        core/DB.h
        core/KeyT.h
        core/OrderedReferenceKey.h
        core/Retriever.cc
        core/Retriever.h
        core/Schema.cc
        core/Schema.h
        core/SchemaKeyIterator.h
        core/SplitKeyT.h
        core/SplitReferenceKey.h

        util/Buffer.cc
        util/Buffer.h
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
