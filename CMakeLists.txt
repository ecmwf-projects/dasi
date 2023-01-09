cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.4 REQUIRED HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild)

project( dasi LANGUAGES C CXX )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
### dependencies and options

ecbuild_find_package( NAME eckit  VERSION 1.12 REQUIRED )

ecbuild_find_package( NAME metkit VERSION 1.9 REQUIRED )

ecbuild_find_package( NAME fdb5 VERSION 5.11 REQUIRED )

add_subdirectory( src )
add_subdirectory( tests )

ecbuild_print_summary()
