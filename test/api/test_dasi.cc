
#include "dasi/util/Test.h"

#include "dasi/api/Dasi.h"
#include "dasi/api/SplitKey.h"

#include "dasi/core/Catalogue.h"
#include "dasi/core/SplitReferenceKey.h"

#include "dasi/util/Buffer.h"

#include <cstring>

//----------------------------------------------------------------------------------------------------------------------

namespace {

using namespace dasi;

std::vector<std::pair<api::SplitKey, util::Buffer>> ARCHIVED_DATA;

class PlayCatalogue : public dasi::core::Catalogue {

public: // types

    constexpr static const char* name = "play";

public: // methods

    using Catalogue::Catalogue;

private: // methods

    void archive(const core::SplitReferenceKey& key, const void* data, size_t length) override {
        EXPECT(key[0] == dbkey());
        std::cout << "DBKEY: " << dbkey() << std::endl;
        std::cout << "ARCHIVE: " << key << std::endl;
        ARCHIVED_DATA.emplace_back(std::make_pair(api::SplitKey{key}, util::Buffer{data, length}));
    }

    void print(std::ostream& s) const override {
        s << "PlayCatalogue[]";
    }
};

core::CatalogueBuilder<PlayCatalogue> playBuilder;
}

//----------------------------------------------------------------------------------------------------------------------

constexpr const char* TEST_CONFIG = R"(
engine: play
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
    dasi::api::Dasi dasi(TEST_CONFIG);

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

    dasi::api::SplitKey expected_archive_key {
        {
            {"key1", "value1"},
            {"key2", "value2"},
            {"key3", "value3"},
        }, {
            {"key1a", "value1a"},
            {"key2a", "value2a"},
            {"key3", "value3"},
        },
        {
            {"key1b", "value1b"},
            {"key2b", "value2b"},
            {"key3b", "value3b"},
        }
    };

    char test_data[] = "TESTING TESTING";
    dasi.archive(key, test_data, sizeof(test_data)-1);

    EXPECT(ARCHIVED_DATA.size() == 1);
    EXPECT(ARCHIVED_DATA[0].first == expected_archive_key);
    EXPECT(ARCHIVED_DATA[0].second.size() == sizeof(test_data)-1);
    EXPECT(::memcmp(ARCHIVED_DATA[0].second.data(), test_data, sizeof(test_data)-1) == 0);
    ARCHIVED_DATA.clear();
}

CASE("Dasi archive with invalid key") {

    dasi::api::Dasi dasi(TEST_CONFIG);

    // n.b. ey1a not provided
    dasi::api::Key key {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},   // n.b. in the sample schema, this key is used twice.
        {"key2a", "value2a"},
        {"key1b", "value1b"},
        {"key2b", "value2b"},
        {"key3b", "value3b"},
    };

    char test_data[] = "TESTING TESTING";
    EXPECT_THROWS_AS(dasi.archive(key, test_data, sizeof(test_data)-1), util::SeriousBug);
}


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}