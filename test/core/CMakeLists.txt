
list( APPEND _core_tests
    Schema
    OrderedKey
    OrderedReferenceKey
    SchemaKeyIterator
    CatalogueFactory
)

foreach( _test ${_core_tests} )
    ecbuild_add_test(
        TARGET dasi_test_core_${_test}
        SOURCES test_${_test}.cc
        LIBS dasi
    )
endforeach()