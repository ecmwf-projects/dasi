
list( APPEND _api_tests
    dasi
    key
    query
    SplitKey
    dasi_c
)

foreach( _test ${_api_tests} )
    ecbuild_add_test(
        TARGET dasi_test_api_${_test}
        SOURCES test_${_test}.cc
        LIBS dasi
    )
endforeach()