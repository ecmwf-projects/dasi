/*
 * (C) Copyright 2022- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/testing/Test.h"

#include "dasi/api/Dasi.h"

#include <tuple>
#include <string.h>

using eckit::testing::run_tests;

//----------------------------------------------------------------------------------------------------------------------

constexpr const char SIMPLE_SCHEMA[] = R"(
# Keys are by default Strings, unless otherwise specified

key2:  Integer;
key3a: Integer;

[ key1, key2, key3
    [ key1a, key2a, key3a
       [ key1b, key2b, key3b ]
    ]
]
)";


std::string simple_config(const eckit::PathName& basedir) {
    return std::string(R"(
        schema: )") + (basedir / "simple_schema") + R"(
        catalogue: toc
        store: file
        spaces:
         - roots:
           - path: )" + (basedir / "root") + R"(
        )";
}

//----------------------------------------------------------------------------------------------------------------------

class ListResultChecker {

public:
    ListResultChecker(std::initializer_list<dasi::Key> init) : expectedValues_(init), validated_(false) {}

    ~ListResultChecker() noexcept(false) {
        validate();
    }

    void found(const dasi::Key& k) {
        ASSERT(!validated_);
        EXPECT(expectedValues_.erase(k) == 1);
    }

    void validate() {
        if (!validated_) {
            validated_ = true;
            EXPECT(expectedValues_.empty());
        }
    }

    template <typename GENERATOR>
    void iterate(GENERATOR&& generator) {
        for (const auto& elem : generator) {
            found(elem.key);
        }
        validate();
    }

private:
    std::set<dasi::Key> expectedValues_;
    bool validated_;
};

//----------------------------------------------------------------------------------------------------------------------

CASE("simple archive") {

    eckit::TmpDir test_wd(eckit::LocalPathName::cwd().c_str());
    std::cout << "Working directory: " << test_wd << std::endl;

    test_wd.mkdir();
    (test_wd / "root").mkdir();

    {
        eckit::FileHandle dh(test_wd / "simple_schema");
        dh.openForWrite(0);
        eckit::AutoClose close(dh);
        dh.write(SIMPLE_SCHEMA, sizeof(SIMPLE_SCHEMA));
    }

    dasi::Dasi dasi(simple_config(test_wd).c_str());

    SECTION("simple archive") {

        dasi::Key key{
                {"key1",  "value1"},
                {"key2",  "123"},
                {"key3",  "value1"},
                {"key1a", "value1"},
                {"key2a", "value1"},
                {"key3a", "321"},
                {"key1b", "value1"},
                {"key2b", "value1"},
                {"key3b", "value1"},
        };

        constexpr const char test_data[] = "TESTING SIMPLE ARCHIVE";
        dasi.archive(key, test_data, sizeof(test_data) - 1);
    }

    //SECTION("mismatched types in key") {

    //    // key2 and key3a are required to be integral according to the schema
    //    // so we should get exceptions raised if anything else is used

    //    dasi::Key key{
    //            {"key1",  "value1"},
    //            {"key2",  "123"},
    //            {"key3",  "value1"},
    //            {"key1a", "value1"},
    //            {"key2a", "value1"},
    //            {"key3a", "blah-blah-nonintegral"},
    //            {"key1b", "value1"},
    //            {"key2b", "value1"},
    //            {"key3b", "value1"},
    //    };

    //    constexpr const char test_data[] = "TESTING SIMPLE ARCHIVE";
    //    EXPECT_THROWS_AS(dasi.archive(key, test_data, sizeof(test_data) - 1), eckit::SeriousBug);
    //}
}


CASE("Accessing data that has been archived") {

    eckit::TmpDir test_wd(eckit::LocalPathName::cwd().c_str());
    std::cout << "Working directory: " << test_wd << std::endl;

    test_wd.mkdir();
    (test_wd / "root").mkdir();

    {
        eckit::FileHandle dh(test_wd / "simple_schema");
        dh.openForWrite(0);
        eckit::AutoClose close(dh);
        dh.write(SIMPLE_SCHEMA, sizeof(SIMPLE_SCHEMA));
    }

    dasi::Dasi dasi(simple_config(test_wd).c_str());

    constexpr const char test_data1[] = "TESTING SIMPLE ARCHIVE";
    constexpr const char test_data2[] = "TESTING SIMPLE ARCHIVE 2222222222";
    constexpr const char test_data3[] = "TESTING SIMPLE ARCHIVE 3333333333";

    for (const auto& elems : std::vector<std::tuple<const char*, int, const char*>> {
                {test_data1, sizeof(test_data1)-1, "value1"},
                {test_data2, sizeof(test_data2)-1, "value2"},
                {test_data3, sizeof(test_data3)-1, "value3"}}) {

            dasi::Key key{
                {"key1",  "value1"},
                {"key2",  "123"},
                {"key3",  "value1"},
                {"key1a", "value1"},
                {"key2a", "value1"},
                {"key3a", "321"},
                {"key1b", "value1"},
                {"key2b", "value1"},
                {"key3b", std::get<2>(elems)},
            };

            dasi.archive(key, std::get<0>(elems), std::get<1>(elems));
    }
    dasi.flush();

    SECTION("We can list all of the data correctly") {

        dasi::Query query {
            {"key1",  {"value1"}},
            {"key2",  {"123"}},
            {"key3",  {"value1"}},
        };

        ListResultChecker checker{
            {{"key1",  "value1"}, {"key2",  "123"}, {"key3",  "value1"}, {"key1a", "value1"}, {"key2a", "value1"},
             {"key3a", "321"}, {"key1b", "value1"}, {"key2b", "value1"}, {"key3b", "value1"}},
            {{"key1",  "value1"}, {"key2",  "123"}, {"key3",  "value1"}, {"key1a", "value1"}, {"key2a", "value1"},
             {"key3a", "321"}, {"key1b", "value1"}, {"key2b", "value1"}, {"key3b", "value2"}},
            {{"key1",  "value1"}, {"key2",  "123"}, {"key3",  "value1"}, {"key1a", "value1"}, {"key2a", "value1"},
             {"key3a", "321"}, {"key1b", "value1"}, {"key2b", "value1"}, {"key3b", "value3"}},
        };

        checker.iterate(dasi.list(query));
    }

    SECTION("We can list a subset of the data") {

        dasi::Query query {
                {"key1",  {"value1"}},
                {"key2",  {"123"}},
                {"key3",  {"value1"}},
                {"key3b",  {"value1", "value3"}},
        };

        ListResultChecker checker{
                {{"key1",  "value1"}, {"key2",  "123"}, {"key3",  "value1"}, {"key1a", "value1"}, {"key2a", "value1"},
                 {"key3a", "321"}, {"key1b", "value1"}, {"key2b", "value1"}, {"key3b", "value1"}},
                {{"key1",  "value1"}, {"key2",  "123"}, {"key3",  "value1"}, {"key1a", "value1"}, {"key2a", "value1"},
                 {"key3a", "321"}, {"key1b", "value1"}, {"key2b", "value1"}, {"key3b", "value3"}},
        };

        checker.iterate(dasi.list(query));
    }

    SECTION("We can match no data") {

        dasi::Query query {
                {"key1",  {"value1"}},
                {"key2",  {"123"}},
                {"key3",  {"value1"}},
                {"key3b",  {"value4", "value5"}},
        };

        ListResultChecker checker{};
        checker.iterate(dasi.list(query));
    }

    SECTION("We can have only some of the query matched") {

        dasi::Query query {
                {"key1",  {"value1"}},
                {"key2",  {"123", "321"}},
                {"key3",  {"value1"}},
                {"key3b",  {"value1", "value3", "value4", "value5"}},
        };

        ListResultChecker checker{
                {{"key1",  "value1"}, {"key2",  "123"}, {"key3",  "value1"}, {"key1a", "value1"}, {"key2a", "value1"},
                 {"key3a", "321"}, {"key1b", "value1"}, {"key2b", "value1"}, {"key3b", "value1"}},
                {{"key1",  "value1"}, {"key2",  "123"}, {"key3",  "value1"}, {"key1a", "value1"}, {"key2a", "value1"},
                 {"key3a", "321"}, {"key1b", "value1"}, {"key2b", "value1"}, {"key3b", "value3"}},
        };

        checker.iterate(dasi.list(query));
    }

    SECTION("We get no results from a query with no hits") {

        dasi::Query query {
            {"key1",  {"value1"}},
            {"key2",  {"123"}},
            {"key3",  {"value1"}},
            {"key3b",  {"unknown", "values"}},
        };

        int cnt = 0;
        for (const auto& elem : dasi.list(query)) {
            ++cnt;
        }
        EXPECT(cnt == 0);
    }

    SECTION("simple retrieve") {
        dasi::Query query {
            {"key1",  {"value1"}},
            {"key2",  {"123"}},
            {"key3",  {"value1"}},
            {"key1a", {"value1"}},
            {"key2a", {"value1"}},
            {"key3a", {"321"}},
            {"key1b", {"value1"}},
            {"key2b", {"value1"}},
            {"key3b",  {"value3", "value1"}},
        };

        dasi::RetrieveResult ret = dasi.retrieve(query);
        EXPECT(ret.count() == 2);

        std::unique_ptr<eckit::DataHandle> dh = ret.dataHandle();
        eckit::MemoryHandle mh;
        auto len = dh->saveInto(mh);

        EXPECT(len == eckit::Length(55));
        EXPECT(memcmp(mh.data(), "TESTING SIMPLE ARCHIVE 3333333333TESTING SIMPLE ARCHIVE", 55) == 0);
    }

    /*
    SECTION("Retrieval fails if not fully qualified") {
        EXPECT(false);
    }

    SECTION("Retrieval fails if not all keys in query satisfied") {
        EXPECT(false);
    }*/
}

//CASE("Arhive data that should be masked...") {
//    EXPECT(false);
//}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
