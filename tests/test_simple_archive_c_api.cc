
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dasi/api/Dasi.h"
#include "dasi/api/c/Dasi.h"

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/testing/Test.h"

#define ASSERT_SUCCESS(error) EXPECT(dasi_error_get_code(error) == DASI_SUCCESS)
#define LOG_I(msg) eckit::Log::info() << msg << std::endl
#define LOG_D(msg) eckit::Log::debug() << msg << std::endl

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

class TestDasi {
public:
    TestDasi();
    ~TestDasi() = default;

    const char* GetConfig() const;

private:
    std::string Config;
    std::unique_ptr<eckit::TmpDir> TmpDir;
};

TestDasi::TestDasi() {
    const auto cwd = eckit::LocalPathName::cwd();
    this->TmpDir   = std::make_unique<eckit::TmpDir>(cwd.c_str());

    LOG_I("- Working directory: " << this->TmpDir->path());

    this->TmpDir->mkdir();
    (*TmpDir / "root").mkdir();

    {  // Schema
        eckit::FileHandle dh(*TmpDir + "/simple_schema");
        dh.openForWrite(0);
        eckit::AutoClose close(dh);
        dh.write(SIMPLE_SCHEMA, sizeof(SIMPLE_SCHEMA));
        LOG_I("- Schema: " << dh.path());
        LOG_D(SIMPLE_SCHEMA);
    }

    this->Config = "schema: " + *TmpDir + "/simple_schema\n" +
                   "catalogue: toc\nstore: file\nspaces:\n - roots:" +
                   "\n   - path: " + *TmpDir + "/root";

    LOG_I("- Configuration ...");
    LOG_D(Config);
}

const char* TestDasi::GetConfig() const {
    return this->Config.c_str();
}

// -----------------------------------------------------------------------------

CASE("[C API] Archive Test: session + key + archive") {
    TestDasi test;
    dasi_error_t err;

    dasi_t dasi = dasi_new(test.GetConfig(), &err);
    ASSERT_SUCCESS(err);

    dasi_key_t key = dasi_key_new(&err);
    ASSERT_SUCCESS(err);

    dasi_key_set(key, "key1", "value1", &err);
    dasi_key_set(key, "key2", "123", &err);
    dasi_key_set(key, "key3", "value1", &err);
    dasi_key_set(key, "key1a", "value1", &err);
    dasi_key_set(key, "key2a", "value1", &err);
    dasi_key_set(key, "key3a", "321", &err);
    dasi_key_set(key, "key1b", "value1", &err);
    dasi_key_set(key, "key2b", "value1", &err);
    dasi_key_set(key, "key3b", "value1", &err);

    constexpr const char test_data[] = "TESTING C API - ARCHIVE";
    dasi_archive(dasi, key, test_data, sizeof(test_data) - 1, &err);
    ASSERT_SUCCESS(err);

    dasi_key_delete(key, &err);
    ASSERT_SUCCESS(err);

    dasi_delete(dasi, &err);
    ASSERT_SUCCESS(err);
}

CASE("[C API] Query Test: session + key + archive + query") {
    TestDasi test;
    dasi_error_t err;

    dasi_t dasi = dasi_new(test.GetConfig(), &err);
    ASSERT_SUCCESS(err);

    constexpr const char data1[] = "TESTING ARCHIVE 1111111111";
    constexpr const char data2[] = "TESTING ARCHIVE 2222222222";
    constexpr const char data3[] = "TESTING ARCHIVE 3333333333";

    for (const auto& elem :
         std::vector<std::tuple<const char*, int, const char*>>{
             {data1, sizeof(data1) - 1, "value1"},
             {data2, sizeof(data2) - 1, "value2"},
             {data3, sizeof(data3) - 1, "value3"}}) {

        LOG_D("Element: " << std::get<0>(elem));

        dasi_key_t key = dasi_key_new(&err);
        ASSERT_SUCCESS(err);
        dasi_key_set(key, "key1", "value1", &err);
        dasi_key_set(key, "key2", "123", &err);
        dasi_key_set(key, "key3", "value1", &err);
        dasi_key_set(key, "key1a", "value1", &err);
        dasi_key_set(key, "key2a", "value1", &err);
        dasi_key_set(key, "key3a", "321", &err);
        dasi_key_set(key, "key1b", "value1", &err);
        dasi_key_set(key, "key2b", "value1", &err);
        dasi_key_set(key, "key3b", std::get<2>(elem), &err);

        constexpr const char test_data[] = "TESTING C API - ARCHIVE";
        dasi_archive(dasi, key, std::get<0>(elem), std::get<1>(elem), &err);
        ASSERT_SUCCESS(err);
        dasi_key_delete(key, &err);
        ASSERT_SUCCESS(err);
    }
    dasi_flush(dasi, &err);
    ASSERT_SUCCESS(err);

    LOG_I("Let's check if we can list correctly...");

    dasi_query_t* query = dasi_query_new(&err);
    ASSERT_SUCCESS(err);
    dasi_query_append(query, "key1", "value1", &err);
    dasi_query_append(query, "key2", "123", &err);
    dasi_query_append(query, "key3", "value1", &err);

    // checklist to compare the list
    std::set<dasi::Key> checklist{{{"key1", "value1"},
                                   {"key2", "123"},
                                   {"key3", "value1"},
                                   {"key1a", "value1"},
                                   {"key2a", "value1"},
                                   {"key3a", "321"},
                                   {"key1b", "value1"},
                                   {"key2b", "value1"},
                                   {"key3b", "value1"}},
                                  {{"key1", "value1"},
                                   {"key2", "123"},
                                   {"key3", "value1"},
                                   {"key1a", "value1"},
                                   {"key2a", "value1"},
                                   {"key3a", "321"},
                                   {"key1b", "value1"},
                                   {"key2b", "value1"},
                                   {"key3b", "value3"}}};

    // Loop over the list elements (C API)
    dasi_list_t* list   = dasi_list(dasi, query, &err);
    dasi_list_elem_t* e = dasi_list_first(list);
    for (; dasi_list_done(list) == 0; e = dasi_list_next(list)) {
        dasi_key_t p_key = dasi_list_elem_get_key(e);
        /// @note C++ API
        auto key = *reinterpret_cast<dasi::Key*>(p_key);
        checklist.erase(key);
    }
    EXPECT(checklist.empty());
    LOG_D("Checklist Finished!");
}

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
