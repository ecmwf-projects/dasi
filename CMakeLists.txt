cmake_minimum_required( VERSION 3.1 FATAL_ERROR )

find_package( ecbuild 3.4 REQUIRED )

project( dasi LANGUAGES CXX C )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We can do find-package, and test, or build ourselves switch here.

include(ExternalProject)
ExternalProject_Add(
    yaml-cpp
    GIT_REPOSITORY    "https://github.com/jbeder/yaml-cpp"
    GIT_TAG           "yaml-cpp-0.6.0"
    CONFIGURE_COMMAND cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp -DCMAKE_POSITION_INDEPENDENT_CODE=ON ${CMAKE_CURRENT_SOURCE_DIR}/contrib/yaml-cpp
    SOURCE_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/contrib/yaml-cpp"
    BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/contrib-build/yaml-cpp"
    INSTALL_DIR       "${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp"
    BUILD_IN_SOURCE   OFF
)
set( YAML_CPP_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp/include" )
set( YAML_CPP_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/contrib/yaml-cpp/lib/libyaml-cpp.a" )

add_subdirectory( src )
add_subdirectory( test )

ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
