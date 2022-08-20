cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.4 REQUIRED )

project( dasi LANGUAGES CXX C )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(yaml-cpp 0.6.0 QUIET)

if(yaml-cpp_FOUND)
    message(STATUS "Found yaml-cpp (${yaml-cpp_CONFIG})")
else()
    include(ExternalProject)
    set( YAML_CPP_STATIC_LIB  "${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp/lib/libyaml-cpp.a" )
    set( YAML_CPP_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp/include" )
    ExternalProject_Add(
        yaml-cpp-builder
        GIT_REPOSITORY    "https://github.com/jbeder/yaml-cpp"
        GIT_TAG           "yaml-cpp-0.6.0"
        CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        SOURCE_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/contrib/yaml-cpp"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/contrib-build/yaml-cpp"
        INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp"
        BUILD_IN_SOURCE   OFF
        BUILD_BYPRODUCTS  "${YAML_CPP_STATIC_LIB}"
    )
    # Keep INTERFACE_INCLUDE_DIRECTORIES happy, by ensuring this dir exists at configure time
    file(MAKE_DIRECTORY "${YAML_CPP_INCLUDE_DIR}")
    add_library( yaml-cpp STATIC IMPORTED )
    set_target_properties(
        yaml-cpp PROPERTIES
        IMPORTED_LOCATION "${YAML_CPP_STATIC_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp/include"
    )
    add_dependencies( yaml-cpp yaml-cpp-builder )
endif()

ecbuild_add_option(FEATURE PYTHON REQUIRED_PACKAGES "Python3 COMPONENTS Interpreter")

add_subdirectory( src )
add_subdirectory( test )

ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
