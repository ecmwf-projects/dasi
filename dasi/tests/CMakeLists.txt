
list( APPEND _dasi_tests
    simple_archive
    key
    query
)

foreach( _test ${_dasi_tests} )
    ecbuild_add_test(
        TARGET dasi_test_${_test}
        SOURCES test_${_test}.cc
        LIBS dasi
    )
endforeach()

# TODO: Include some tests that don't pull in eckit, to prove that the API doesn't require it.