
#include "dasi/util/Test.h"

#include "dasi/api/Dasi.h"
#include "dasi/api/Query.h"
#include "dasi/api/ReadHandle.h"

#include "dasi/util/AutoCloser.h"

#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------

std::filesystem::path test_dir(const std::string& stem, const std::filesystem::path& root = ".") {
    static unsigned long long n =
        static_cast<unsigned long long>(std::time(nullptr)) << 32;

    std::filesystem::path rootabs = std::filesystem::canonical(root);
    std::filesystem::path path;

    do {
        std::ostringstream name;
        name << stem << "." << n++;
        path = rootabs / name.str();
    } while (std::filesystem::exists(path));

    std::filesystem::create_directories(path);
    return path;
}

constexpr const char* TEST_SCHEMA = R"(
schema:
 - [key1, key2, key3,
     [key1a, key2a, key3,
        [key1b, key2b, key3b],
        [key1B, key2B, key3B]],

     [key1A, key2A, key3,
        [key1b, key2b, key3b]],
     [Key1A, Key2A, Key3A,
        [key1b, key2b, key3b]]
   ]
 - [Key1, Key2, Key3,
     [key1a, key2a, key3,
        [key1b, key2b, key3b],
        [key1B, key2B, key3B]],

     [key1A, key2A, key3,
        [key1b, key2b, key3b]],
     [Key1A, Key2A, Key3A,
        [key1b, key2b, key3b]]
])";


CASE("Dasi simple archive") {
    std::filesystem::path root = test_dir("dasi_test_shelf");
    std::cout << "data root: " << root << std::endl;

    std::stringstream cfg;
    cfg << "engine: shelf" << "\n";
    cfg << "root: " << root << "\n";
    cfg << TEST_SCHEMA;

    dasi::api::Dasi dasi(cfg);

    dasi::api::Key key {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},   // n.b. in the sample schema, this key is used twice.
        {"key1a", "value1a"},
        {"key2a", "value2a"},
        {"key1b", "value1b"},
        {"key2b", "value2b"},
        {"key3b", "value3b"},
    };

    std::filesystem::path expected_path = root /
            "key1=value1,key2=value2,key3=value3" /
            "key1a=value1a,key2a=value2a,key3=value3" /
            "key1b=value1b,key2b=value2b,key3b=value3b";

    char test_data[] = "TESTING TESTING";
    dasi.archive(key, test_data, sizeof(test_data) - 1);

    EXPECT(std::filesystem::exists(expected_path));
    std::ifstream archive(expected_path, std::ios::in | std::ios::binary);
    EXPECT(archive.good());
    archive.seekg(0, std::ios::end);
    EXPECT(archive.tellg() == sizeof(test_data) - 1);
    archive.seekg(0);
    char archived_data[sizeof(test_data) - 1];
    archive.read(archived_data, sizeof(test_data) - 1);
    EXPECT(std::memcmp(archived_data, test_data, sizeof(test_data) - 1) == 0);

    std::filesystem::remove_all(root);
}

CASE("Dasi simple retrieve") {
    std::filesystem::path root = test_dir("dasi_test_shelf");
    std::cout << "data root: " << root << std::endl;

    std::stringstream cfg;
    cfg << "engine: shelf" << "\n";
    cfg << "root: " << root << "\n";
    cfg << TEST_SCHEMA;

    dasi::api::Dasi dasi(cfg);

    dasi::api::Key key {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},   // n.b. in the sample schema, this key is used twice.
        {"key1a", "value1a"},
        {"key2a", "value2a"},
        {"key1b", "value1b"},
        {"key2b", "value2b"},
        {"key3b", "value3b"},
    };

    dasi::api::Query query {
        {"key1", {"value1"}},
        {"key2", {"value2"}},
        {"key3", {"value3"}},
        {"key1a", {"value1a"}},
        {"key2a", {"value2a"}},
        {"key1b", {"value1b"}},
        {"key2b", {"value2b"}},
        {"key3b", {"value3b"}},
    };

    char test_data[] = "TESTING TESTING";
    dasi.archive(key, test_data, sizeof(test_data));

    dasi::api::RetrieveResult result(dasi.retrieve(query));
    std::unique_ptr<dasi::api::ReadHandle> handle(result.toHandle());
    char res[sizeof(test_data)];
    handle->open();
    dasi::util::AutoCloser closer(*handle);
    auto len = handle->read(res, sizeof(test_data));
    EXPECT(len == sizeof(test_data));
    EXPECT(::memcmp(res, test_data, len) == 0);

    std::filesystem::remove_all(root);
}

CASE("Dasi retrieve with no data matched") {
    std::filesystem::path root = test_dir("dasi_test_shelf");
    std::cout << "data root: " << root << std::endl;

    std::stringstream cfg;
    cfg << "engine: shelf" << "\n";
    cfg << "root: " << root << "\n";
    cfg << TEST_SCHEMA;

    dasi::api::Dasi dasi(cfg);

    dasi::api::Key key {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},   // n.b. in the sample schema, this key is used twice.
        {"key1a", "value1a"},
        {"key2a", "value2a"},
        {"key1b", "value1b"},
        {"key2b", "value2b"},
        {"key3b", "value3b"},
    };

    dasi::api::Query query {
        {"key1", {"value1"}},
        {"key2", {"value2"}},
        {"key3", {"value3"}},
        {"key1a", {"value1a"}},
        {"key2a", {"value2a"}},
        {"key1b", {"value1b"}},
        {"key2b", {"value2b"}},
        {"key3b", {"absent"}},
    };

    char test_data[] = "TESTING TESTING";
    dasi.archive(key, test_data, sizeof(test_data));

    EXPECT_THROWS_AS(dasi.retrieve(query), dasi::util::ObjectNotFound);

    std::filesystem::remove_all(root);
}

int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}
