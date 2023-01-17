cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

find_package(ecbuild 3.4 REQUIRED)

project(forecast VERSION 0.1 LANGUAGES C)

find_package(dasi REQUIRED)

ecbuild_find_package( NAME eckit  VERSION 1.20 REQUIRED )

ecbuild_add_executable(
    TARGET forecast
    SOURCES forecast.c
    LIBS dasi
)

ecbuild_add_executable(
    TARGET mean
    SOURCES mean.c
    LIBS dasi
)

ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
