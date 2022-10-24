
#include "dasi/util/Test.h"

#include "dasi/api/ObjectLocation.h"
#include "dasi/api/ReadHandle.h"
#include "dasi/api/SplitKey.h"
#include "dasi/api/dasi_c.h"

#include "dasi/core/Catalogue.h"
#include "dasi/core/HandleFactory.h"

#include "dasi/util/Buffer.h"

#include <cstdio>
#include <cstring>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------

namespace {

using namespace dasi;

class BufferReadHandle : public dasi::api::ReadHandle {

public: // members

    constexpr static const char* type = "buffer";

public: // methods

    BufferReadHandle(const util::Buffer& buffer) :
        buffer_(buffer),
        pos_(0) {}

    BufferReadHandle(const std::string& location, offset_type offset, length_type length) :
        buffer_(deserialise(location)),
        pos_(offset) {}

    size_t read(void* buf, size_t len) override {
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

    void open() override {}
    void close() override {}

    static api::ObjectLocation toLocation(const util::Buffer& buffer) {
        std::string loc = util::StringBuilder() << reinterpret_cast<const void*>(&buffer);
        return api::ObjectLocation{type, loc, 0, length_type(buffer.size())};
    }

private: // methods

    void print(std::ostream& s) const override {
        s << "BufferReadHandle";
    }

    static const util::Buffer& deserialise(const std::string& location) {
        std::istringstream iss(location);
        void* buf;
        iss >> buf;
        ASSERT(buf != nullptr);
        return *reinterpret_cast<const util::Buffer*>(buf);
    }

private: // members

    const util::Buffer& buffer_;
    size_t pos_;

};

core::HandleBuilder<BufferReadHandle> bufferBuilder;

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

    api::ObjectLocation retrieve(const core::SplitReferenceKey& key) override {
        EXPECT(key[0] == dbkey());
        std::cout << "DBKEY: " << dbkey() << std::endl;
        std::cout << "RETRIEVE: " << key << std::endl;
        auto matches_key = [&key](const auto& el){
            return el.first == key;
        };
        auto it = std::find_if(ARCHIVED_DATA.begin(), ARCHIVED_DATA.end(), matches_key);
        if (it == ARCHIVED_DATA.end()) {
            std::ostringstream oss;
            oss << key;
            throw dasi::util::ObjectNotFound(oss.str(), Here());
        }
        return BufferReadHandle::toLocation(it->second);
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


#define DASI_CHECK(x) EXPECT((x) == DASI_SUCCESS)

namespace detail {
    template <typename T>
    void cleanup(T*);

    template<>
    void cleanup(dasi_t* ptr) { dasi_close(ptr); }

    template<>
    void cleanup(dasi_key_t* ptr) { dasi_key_destroy(ptr); }

    template<>
    void cleanup(dasi_query_t* ptr) { dasi_query_destroy(ptr); }

    template<>
    void cleanup(dasi_retrieve_result_t* ptr) { dasi_retrieve_result_destroy(ptr); }
    
    template<>
    void cleanup(dasi_read_handle_t* ptr) { dasi_read_handle_destroy(ptr); }

    template<>
    void cleanup(dasi_retrieve_iterator_t* ptr) { dasi_retrieve_iterator_destroy(ptr); }
}

template<typename T>
struct Destructor {
    T* ptr_;
    Destructor(T* ptr) : ptr_(ptr) {}
    ~Destructor() { detail::cleanup(ptr_); }
};

struct HandleCloser {
    dasi_read_handle_t* handle_;
    HandleCloser(dasi_read_handle_t* handle) : handle_(handle) {}
    ~HandleCloser() { dasi_read_handle_close(handle_); }
};


CASE("Dasi simple archive") {
    ARCHIVED_DATA.clear();

    dasi_t* session;
    DASI_CHECK(dasi_open_str(TEST_CONFIG, &session));
    Destructor sessionD(session);

    dasi_key_t* key;
    DASI_CHECK(dasi_key_new(&key));
    Destructor keyD(key);
    DASI_CHECK(dasi_key_set(key, "key1", "value1"));
    DASI_CHECK(dasi_key_set(key, "key2", "value2"));
    DASI_CHECK(dasi_key_set(key, "key3", "value3"));
    DASI_CHECK(dasi_key_set(key, "key1a", "value1a"));
    DASI_CHECK(dasi_key_set(key, "key2a", "value2a"));
    DASI_CHECK(dasi_key_set(key, "key1b", "value1b"));
    DASI_CHECK(dasi_key_set(key, "key2b", "value2b"));
    DASI_CHECK(dasi_key_set(key, "key3b", "value3b"));

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
    DASI_CHECK(dasi_put(session, key, test_data, sizeof(test_data)-1));

    EXPECT(ARCHIVED_DATA.size() == 1);
    EXPECT(ARCHIVED_DATA[0].first == expected_archive_key);
    EXPECT(ARCHIVED_DATA[0].second.size() == sizeof(test_data)-1);
    EXPECT(::memcmp(ARCHIVED_DATA[0].second.data(), test_data, sizeof(test_data)-1) == 0);
}

#if 0
CASE("Dasi archive with invalid key") {
    ARCHIVED_DATA.clear();

    dasi_t* session;
    DASI_CHECK(dasi_open_str(TEST_CONFIG, &session));
    Destructor sessionD(session);

    dasi_key_t* valid_key;
    DASI_CHECK(dasi_key_new(&valid_key));
    Destructor valid_keyD(valid_key);
    DASI_CHECK(dasi_key_set(valid_key, "key1", "value1"));
    DASI_CHECK(dasi_key_set(valid_key, "key2", "value2"));
    DASI_CHECK(dasi_key_set(valid_key, "key3", "value3"));
    DASI_CHECK(dasi_key_set(valid_key, "key1a", "value1a"));
    DASI_CHECK(dasi_key_set(valid_key, "key2a", "value2a"));
    DASI_CHECK(dasi_key_set(valid_key, "key1b", "value1b"));
    DASI_CHECK(dasi_key_set(valid_key, "key2b", "value2b"));
    DASI_CHECK(dasi_key_set(valid_key, "key3b", "value3b"));

    // key1a not provided

    dasi_key_t* key1;
    DASI_CHECK(dasi_key_copy(valid_key, &key1));
    Destructor key1D(key1);
    DASI_CHECK(dasi_key_del(key1, "key1a"));

    // Extra keys provided

    dasi_key_t* key2;
    DASI_CHECK(dasi_key_copy(valid_key, &key2));
    Destructor key2D(key2);
    DASI_CHECK(dasi_key_set(key2, "invalid", "key"));

    // Key provided from another part of the schema

    dasi_key_t* key3;
    DASI_CHECK(dasi_key_copy(valid_key, &key3));
    Destructor key3D(key3);
    DASI_CHECK(dasi_key_set(key3, "key1A", "value1A"));

    dasi_key_t* key4;
    DASI_CHECK(dasi_key_copy(valid_key, &key4));
    Destructor key4D(key4);
    DASI_CHECK(dasi_key_del(key4, "key1a"));
    DASI_CHECK(dasi_key_set(key4, "key1A", "value1A"));

    char test_data[] = "TESTING TESTING";
    DASI_CHECK(dasi_put(session, valid_key, test_data, sizeof(test_data)-1));

    for (const auto& key : {key1, key2, key3}) {
        dasi_error err = dasi_put(session, key, test_data, sizeof(test_data)-1);
        EXPECT(err == DASI_ERROR);
    }
}
#endif

CASE("Dasi simple retrieve") {
    ARCHIVED_DATA.clear();

    dasi_t* session;
    DASI_CHECK(dasi_open_str(TEST_CONFIG, &session));
    Destructor sessionD(session);

    dasi_key_t* key;
    DASI_CHECK(dasi_key_new(&key));
    Destructor keyD(key);
    DASI_CHECK(dasi_key_set(key, "key1", "value1"));
    DASI_CHECK(dasi_key_set(key, "key2", "value2"));
    DASI_CHECK(dasi_key_set(key, "key3", "value3"));
    DASI_CHECK(dasi_key_set(key, "key1a", "value1a"));
    DASI_CHECK(dasi_key_set(key, "key2a", "value2a"));
    DASI_CHECK(dasi_key_set(key, "key1b", "value1b"));
    DASI_CHECK(dasi_key_set(key, "key2b", "value2b"));
    DASI_CHECK(dasi_key_set(key, "key3b", "value3b"));

    dasi_query_t* query;
    DASI_CHECK(dasi_query_new(&query));
    Destructor queryD(query);
    DASI_CHECK(dasi_query_append(query, "key1", "value1"));
    DASI_CHECK(dasi_query_append(query, "key2", "value2"));
    DASI_CHECK(dasi_query_append(query, "key3", "value3"));
    DASI_CHECK(dasi_query_append(query, "key1a", "value1a"));
    DASI_CHECK(dasi_query_append(query, "key2a", "value2a"));
    DASI_CHECK(dasi_query_append(query, "key1b", "value1b"));
    DASI_CHECK(dasi_query_append(query, "key2b", "value2b"));
    DASI_CHECK(dasi_query_append(query, "key3b", "value3b"));

    char test_data[] = "TESTING TESTING";
    DASI_CHECK(dasi_put(session, key, test_data, sizeof(test_data)));

    char extra_data[] = "NEW DATA";
    DASI_CHECK(dasi_key_set(key, "key3b", "new3b"));
    DASI_CHECK(dasi_put(session, key, extra_data, sizeof(extra_data)));

    dasi_retrieve_result_t* result;
    DASI_CHECK(dasi_get(session, query, &result));
    Destructor resultD(result);
    dasi_read_handle_t* handle;
    DASI_CHECK(dasi_retrieve_result_get_handle(result, &handle));
    Destructor handleD(handle);
    char res[sizeof(test_data)];
    DASI_CHECK(dasi_read_handle_open(handle));
    HandleCloser closer(handle);
    size_t len;
    DASI_CHECK(dasi_read_handle_read(handle, res, sizeof(test_data), &len));
    EXPECT(len == sizeof(test_data));
    EXPECT(::memcmp(res, test_data, len) == 0);
}

CASE("Dasi retrieve multiple objects") {
    ARCHIVED_DATA.clear();

    dasi_t* session;
    DASI_CHECK(dasi_open_str(TEST_CONFIG, &session));
    Destructor sessionD(session);

    dasi_key_t* key;
    DASI_CHECK(dasi_key_new(&key));
    Destructor keyD(key);
    DASI_CHECK(dasi_key_set(key, "key1", "value1"));
    DASI_CHECK(dasi_key_set(key, "key2", "value2"));
    DASI_CHECK(dasi_key_set(key, "key3", "value3"));
    DASI_CHECK(dasi_key_set(key, "key1a", "value1a"));
    DASI_CHECK(dasi_key_set(key, "key2a", "value2a"));
    DASI_CHECK(dasi_key_set(key, "key1b", "value1b"));
    DASI_CHECK(dasi_key_set(key, "key2b", "value2b"));
    DASI_CHECK(dasi_key_set(key, "key3b", "value3b"));

    dasi_query_t* query;
    DASI_CHECK(dasi_query_new(&query));
    Destructor queryD(query);
    DASI_CHECK(dasi_query_append(query, "key1", "value1"));
    DASI_CHECK(dasi_query_append(query, "key2", "value2"));
    DASI_CHECK(dasi_query_append(query, "key3", "value3"));
    DASI_CHECK(dasi_query_append(query, "key1a", "value1a"));
    DASI_CHECK(dasi_query_append(query, "key2a", "value2a"));
    DASI_CHECK(dasi_query_append(query, "key1b", "value1b"));
    DASI_CHECK(dasi_query_append(query, "key2b", "value2b"));
    DASI_CHECK(dasi_query_append(query, "key3b", "value3b"));
    DASI_CHECK(dasi_query_append(query, "key3b", "new3b"));

    char test_data[] = "TESTING TESTING";
    DASI_CHECK(dasi_put(session, key, test_data, sizeof(test_data)));

    char extra_data[] = "NEW DATA";
    DASI_CHECK(dasi_key_set(key, "key3b", "new3b"));
    DASI_CHECK(dasi_put(session, key, extra_data, sizeof(extra_data)));

    dasi_retrieve_result_t* result;
    DASI_CHECK(dasi_get(session, query, &result));
    Destructor resultD(result);
    dasi_read_handle_t* handle;
    DASI_CHECK(dasi_retrieve_result_get_handle(result, &handle));
    Destructor handleD(handle);
    DASI_CHECK(dasi_read_handle_open(handle));
    HandleCloser closer(handle);

    char res[sizeof(test_data)];
    size_t len;
    DASI_CHECK(dasi_read_handle_read(handle, res, sizeof(test_data), &len));
    EXPECT(len == sizeof(test_data));
    EXPECT(::memcmp(res, test_data, len) == 0);

    char res2[sizeof(extra_data)];
    size_t len2;
    DASI_CHECK(dasi_read_handle_read(handle, res2, sizeof(extra_data), &len2));
    EXPECT(len2 == sizeof(extra_data));
    EXPECT(::memcmp(res2, extra_data, len2) == 0);
}

CASE("Dasi retrieve with iterator") {
    ARCHIVED_DATA.clear();

    dasi_t* session;
    DASI_CHECK(dasi_open_str(TEST_CONFIG, &session));
    Destructor sessionD(session);

    dasi_key_t* kbase;
    DASI_CHECK(dasi_key_new(&kbase));
    Destructor kbaseD(kbase);
    DASI_CHECK(dasi_key_set(kbase, "key1", "value1"));
    DASI_CHECK(dasi_key_set(kbase, "key2", "value2"));
    DASI_CHECK(dasi_key_set(kbase, "key3", "value3"));
    DASI_CHECK(dasi_key_set(kbase, "key1a", "value1a"));
    DASI_CHECK(dasi_key_set(kbase, "key2a", "value2a"));
    DASI_CHECK(dasi_key_set(kbase, "key1b", "value1b"));
    DASI_CHECK(dasi_key_set(kbase, "key2b", "value2b"));
    DASI_CHECK(dasi_key_set(kbase, "key3b", "value3b"));

    size_t num_keys = 5;
    std::vector<std::string> vals;
    std::map<std::string, std::pair<dasi_key_t*, std::string>> expected;
    vals.reserve(num_keys);
    char data[] = "TEST   ";
    size_t data_len = sizeof(data) - 1;
    for (size_t i = 0; i < num_keys; ++i) {
        dasi_key_t* key;
        DASI_CHECK(dasi_key_copy(kbase, &key));
        std::string kv = std::to_string(i);
        DASI_CHECK(dasi_key_set(key, "key2b", kv.c_str()));
        std::snprintf(data, data_len + 1, "TEST %02ld", i);
        DASI_CHECK(dasi_put(session, key, data, data_len));
        if (i % 2 == 0) {
            vals.push_back(kv);
            expected.emplace(kv, std::make_pair(key, data));
        }
    }

    dasi_query_t* query;
    DASI_CHECK(dasi_query_new(&query));
    Destructor queryD(query);
    DASI_CHECK(dasi_query_append(query, "key1", "value1"));
    DASI_CHECK(dasi_query_append(query, "key2", "value2"));
    DASI_CHECK(dasi_query_append(query, "key3", "value3"));
    DASI_CHECK(dasi_query_append(query, "key1a", "value1a"));
    DASI_CHECK(dasi_query_append(query, "key2a", "value2a"));
    DASI_CHECK(dasi_query_append(query, "key1b", "value1b"));
    const char* cvals[vals.size()];
    for (size_t i = 0; i < vals.size(); ++i) {
        cvals[i] = vals[i].c_str();
    }
    DASI_CHECK(dasi_query_set(query, "key2b", cvals, vals.size()));
    DASI_CHECK(dasi_query_append(query, "key3b", "value3b"));

    dasi_retrieve_result_t* result;
    DASI_CHECK(dasi_get(session, query, &result));
    Destructor resultD(result);

    dasi_retrieve_iterator_t* iter;
    DASI_CHECK(dasi_retrieve_result_iterate(result, &iter));
    Destructor iterD(iter);
    dasi_error err;
    do {
        dasi_key_t* key;
        DASI_CHECK(dasi_retrieve_iterator_get_key(iter, &key));
        const char* kv;
        DASI_CHECK(dasi_key_get(key, "key2b", &kv));
        auto it = expected.find(std::string{kv});
        EXPECT(it != expected.end());
        auto [ekey, edata] = it->second;

        int cmp;
        DASI_CHECK(dasi_key_cmp(ekey, key, &cmp));
        EXPECT(cmp == 0);

        dasi_read_handle_t* handle;
        DASI_CHECK(dasi_retrieve_iterator_get_handle(iter, &handle));
        Destructor handleD(handle);
        DASI_CHECK(dasi_read_handle_open(handle));
        HandleCloser closer(handle);
        char res[data_len];
        size_t len;
        DASI_CHECK(dasi_read_handle_read(handle, res, data_len, &len));
        EXPECT(len == data_len);
        EXPECT(::memcmp(res, edata.c_str(), len) == 0);

        expected.erase(it);
    } while((err = dasi_retrieve_iterator_next(iter)) == DASI_SUCCESS);
    EXPECT(err == DASI_ITERATOR_END);
    EXPECT(expected.empty());
}

CASE("Dasi retrieve with no data matched") {
    ARCHIVED_DATA.clear();

    dasi_t* session;
    DASI_CHECK(dasi_open_str(TEST_CONFIG, &session));
    Destructor sessionD(session);

    dasi_key_t* key;
    DASI_CHECK(dasi_key_new(&key));
    Destructor keyD(key);
    DASI_CHECK(dasi_key_set(key, "key1", "value1"));
    DASI_CHECK(dasi_key_set(key, "key2", "value2"));
    DASI_CHECK(dasi_key_set(key, "key3", "value3"));
    DASI_CHECK(dasi_key_set(key, "key1a", "value1a"));
    DASI_CHECK(dasi_key_set(key, "key2a", "value2a"));
    DASI_CHECK(dasi_key_set(key, "key1b", "value1b"));
    DASI_CHECK(dasi_key_set(key, "key2b", "value2b"));
    DASI_CHECK(dasi_key_set(key, "key3b", "value3b"));

    dasi_query_t* query;
    DASI_CHECK(dasi_query_new(&query));
    Destructor queryD(query);
    DASI_CHECK(dasi_query_append(query, "key1", "value1"));
    DASI_CHECK(dasi_query_append(query, "key2", "value2"));
    DASI_CHECK(dasi_query_append(query, "key3", "value3"));
    DASI_CHECK(dasi_query_append(query, "key1a", "value1a"));
    DASI_CHECK(dasi_query_append(query, "key2a", "value2a"));
    DASI_CHECK(dasi_query_append(query, "key1b", "value1b"));
    DASI_CHECK(dasi_query_append(query, "key2b", "value2b"));
    DASI_CHECK(dasi_query_append(query, "key3b", "absent"));

    char test_data[] = "TESTING TESTING";
    DASI_CHECK(dasi_put(session, key, test_data, sizeof(test_data)));

    dasi_retrieve_result_t* result;
    EXPECT(dasi_get(session, query, &result) == DASI_NOT_FOUND);
}

CASE("Dasi retrieve with partial data") {
    ARCHIVED_DATA.clear();

    dasi_t* session;
    DASI_CHECK(dasi_open_str(TEST_CONFIG, &session));
    Destructor sessionD(session);

    dasi_key_t* key;
    DASI_CHECK(dasi_key_new(&key));
    Destructor keyD(key);
    DASI_CHECK(dasi_key_set(key, "key1", "value1"));
    DASI_CHECK(dasi_key_set(key, "key2", "value2"));
    DASI_CHECK(dasi_key_set(key, "key3", "value3"));
    DASI_CHECK(dasi_key_set(key, "key1a", "value1a"));
    DASI_CHECK(dasi_key_set(key, "key2a", "value2a"));
    DASI_CHECK(dasi_key_set(key, "key1b", "value1b"));
    DASI_CHECK(dasi_key_set(key, "key2b", "value2b"));
    DASI_CHECK(dasi_key_set(key, "key3b", "value3b"));

    dasi_query_t* query;
    DASI_CHECK(dasi_query_new(&query));
    Destructor queryD(query);
    DASI_CHECK(dasi_query_append(query, "key1", "value1"));
    DASI_CHECK(dasi_query_append(query, "key2", "value2"));
    DASI_CHECK(dasi_query_append(query, "key3", "value3"));
    DASI_CHECK(dasi_query_append(query, "key1a", "value1a"));
    DASI_CHECK(dasi_query_append(query, "key2a", "value2a"));
    DASI_CHECK(dasi_query_append(query, "key1b", "value1b"));
    DASI_CHECK(dasi_query_append(query, "key1b", "absent"));
    DASI_CHECK(dasi_query_append(query, "key2b", "value2b"));
    DASI_CHECK(dasi_query_append(query, "key3b", "value3b"));

    char test_data[] = "TESTING TESTING";
    DASI_CHECK(dasi_put(session, key, test_data, sizeof(test_data)));

    dasi_retrieve_result_t* result;
    EXPECT(dasi_get(session, query, &result) == DASI_NOT_FOUND);
}


int main(int argc, char** argv) {
    return ::dasi::util::run_tests();
}
