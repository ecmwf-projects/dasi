
#include "dasi/util/Test.h"

#include "dasi/api/Dasi.h"
#include "dasi/api/Handle.h"
#include "dasi/api/Query.h"
#include "dasi/api/SplitKey.h"

#include "dasi/core/Catalogue.h"
#include "dasi/core/SplitReferenceKey.h"

#include "dasi/util/Buffer.h"

#include <cstring>

//----------------------------------------------------------------------------------------------------------------------

namespace {

using namespace dasi;

class BufferHandle : public dasi::api::Handle {

public: // methods

    BufferHandle(const api::Key& key, const util::Buffer& buffer) :
        key_(key),
        buffer_(buffer),
        pos_(0) {}

    size_t read(void* buf, size_t len, bool stream = false) override {
        if (pos_ >= buffer_.size()) {
            return 0;
        }

        size_t toread = buffer_.size() - pos_;
        if (len < toread) {
            toread = len;
        }

        const char* start = static_cast<const char*>(buffer_.data()) + pos_;
        std::memcpy(buf, start, toread);
        pos_ += toread;
        return toread;
    }

    const api::Key& currentKey() const override {
        return key_;
    }

    void open() override {}
    void close() override {}

private: // methods

    void print(std::ostream& s) const override {
        s << "BufferHandle";
    }

private: // members

    const api::Key key_;
    const util::Buffer& buffer_;
    size_t pos_;

};

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

    api::Handle* retrieve(const core::SplitReferenceKey& key) override {
        EXPECT(key[0] == dbkey());
        std::cout << "DBKEY: " << dbkey() << std::endl;
        std::cout << "RETRIEVE: " << key << std::endl;
        auto matches_key = [&key](const auto& el){
            return el.first == key;
        };
        auto it = std::find_if(ARCHIVED_DATA.begin(), ARCHIVED_DATA.end(), matches_key);
        ASSERT(it != ARCHIVED_DATA.end());
        api::Key key2;
        for (int level = 0; level < 3; ++level) {
            for (const auto& elem : key[level]) {
                key2.set(elem.first, std::string{elem.second});
            }
        }
        return new BufferHandle(key2, it->second);
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
    ARCHIVED_DATA.clear();

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
}

CASE("Dasi simple retrieve") {
    ARCHIVED_DATA.clear();

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

    std::unique_ptr<api::Handle> handle(dasi.retrieve(query));
    char res[sizeof(test_data)];
    handle->open();
    dasi::api::AutoCloser closer(*handle);
    auto len = handle->read(res, sizeof(test_data));
    EXPECT(len == sizeof(test_data));
    EXPECT(::memcmp(res, test_data, len) == 0);
}

CASE("Dasi archive with invalid key") {
    ARCHIVED_DATA.clear();

    dasi::api::Dasi dasi(TEST_CONFIG);

    dasi::api::Key valid_key {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"},   // n.b. in the sample schema, this key is used twice.
        {"key1a", "value1a"},
        {"key2a", "value2a"},
        {"key1b", "value1b"},
        {"key2b", "value2b"},
        {"key3b", "value3b"},
    };

    // key1a not provided

    dasi::api::Key key1 {valid_key};
    key1.erase("key1a");

    // Extra keys provided

    dasi::api::Key key2 {valid_key};
    key2.set("invalid", "key");

    // Key provided from another part of the schema

    dasi::api::Key key3 {valid_key};
    key3.set("key1A", "value1A");

    dasi::api::Key key4 {valid_key};
    key4.erase("key1a");
    key4.set("key1A", "value1A");

    char test_data[] = "TESTING TESTING";
    dasi.archive(valid_key, test_data, sizeof(test_data)-1);

    for (const auto& key : {key1, key2, key3}) {
        EXPECT_THROWS_AS(dasi.archive(key, test_data, sizeof(test_data)-1), util::SeriousBug);
    }
}


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}