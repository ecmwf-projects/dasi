
list( APPEND _util_tests
    IndentStream
    CartesianProduct
    OrderedMap
    StringBuilder
    LRUMap
)

foreach( _test ${_util_tests} )
    ecbuild_add_test(
            TARGET dasi_test_util_${_test}
            SOURCES test_${_test}.cc
            LIBS dasi
    )
endforeach()
