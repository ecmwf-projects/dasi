ecbuild_add_test(
    TARGET dasi_test_python
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.sh
    ENVIRONMENT
        "DASI_SRC_DIR=${PROJECT_SOURCE_DIR}"
        "DASI_LIB_DIR=${PROJECT_BINARY_DIR}/lib"
        "TESTS_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
        "PYTHON_INTERP=${Python3_EXECUTABLE}"
)
