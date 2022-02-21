
list( APPEND _tools_tests
    put
    get
)

foreach( _test ${_tools_tests} )
    set(_wdir ${CMAKE_CURRENT_BINARY_DIR}/test_${_test})
    file(MAKE_DIRECTORY ${_wdir})
    set(TEST_ROOT ${_wdir}/shelf)
    configure_file(dasi-config.yml.in test_${_test}/dasi-config.yml)
    ecbuild_add_test(
            TARGET dasi_test_tool_${_test}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_${_test}.sh
            WORKING_DIRECTORY ${_wdir}
            ENVIRONMENT "DASI_BIN=${PROJECT_BINARY_DIR}/bin"
    )
endforeach()
