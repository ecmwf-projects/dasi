
list( APPEND _util_tests
    IndentStream
    CartesianProduct
    OrderedMap
    StringBuilder
    LRUMap
    ReverseAdapter
    AsyncGenerator
)

foreach( _test ${_util_tests} )
    ecbuild_add_test(
            TARGET dasi_test_util_${_test}
            SOURCES test_${_test}.cc
            LIBS dasi
    )
endforeach()

set_tests_properties(dasi_test_util_AsyncGenerator PROPERTIES TIMEOUT 10)