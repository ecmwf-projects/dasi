/*
 * (C) Copyright 2022- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/testing/Test.h"
#include "eckit/filesystem/TmpDir.h"

#include "dasi/Dasi.h"

using eckit::testing::run_tests;

//----------------------------------------------------------------------------------------------------------------------

constexpr const char* TEST_CONFIG = R"(
schema: simple_schema
)";

//----------------------------------------------------------------------------------------------------------------------

CASE("simple archive") {



    dasi::Key key {
            {"key1", "value1"},
            {"key2", "123"},
            {"key3", "value1"},
            {"key1a", "value1"},
            {"key2a", "value1"},
            {"key3a", "321"},
            {"key1b", "value1"},
            {"key2b", "value1"},
            {"key3b", "value1"},
    };

    std::cout << key << std::endl;

    ASSERT(false);
}

CASE("Mismatched types in key") {

    // key2 and key3a are required to be integral according to the schema
    // so we should get exceptions raised if anything else is used

    dasi::Key key{
        {"key1", "value1"},
        { "key2", "123" },
        { "key3", "value1" },
        { "key1a", "value1" },
        { "key2a", "value1" },
        { "key3a", "321" },
        { "key1b", "value1" },
        { "key2b", "value1" },
        { "key3b", "value1" },
    };

    char test_data[] = "TESTING TESTING";
    //EXPECT_THROWS_AS(dasi.archive(key, test_data, sizeof(test_data)-1), SeriousBug);
    ASSERT(false);
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}