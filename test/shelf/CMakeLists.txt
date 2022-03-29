
list( APPEND _shelf_tests
    shelf
)

foreach( _test ${_shelf_tests} )
    ecbuild_add_test(
            TARGET dasi_test_shelf_${_test}
            SOURCES test_${_test}.cc
            LIBS dasi
    )
endforeach()
