/*
 * (C) Copyright 2023 ECMWF.
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http: //www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "dasi_c.h"
#include "dasi/api/Dasi.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Optional.h"

#include <functional>

extern "C" {

// ---------------------------------------------------------------------------------------------------------------------

struct Dasi : public dasi::Dasi {
    using dasi::Dasi::Dasi;
};

struct Key : public dasi::Key {
    using dasi::Key::Key;
    Key(const dasi::Key& k) : dasi::Key(k) {}
};

struct Query : public dasi::Query {
    using dasi::Query::Query;
};

struct dasi_list_t {
    dasi_list_t(dasi::ListGenerator&& gen) :
        first(true),
        generator(std::move(gen)),
        iterator(generator.begin()) {}

    bool first;
    dasi::ListGenerator generator;
    dasi::ListGenerator::const_iterator iterator;
    std::string uri_cache;
};

struct dasi_retrieve_t {
    dasi_retrieve_t(dasi::RetrieveResult&& ret) : retrieve(std::move(ret)) {}
    dasi::RetrieveResult retrieve;
    std::unique_ptr<eckit::DataHandle> dh;
    eckit::Optional<eckit::AutoClose> closer;
};

// ---------------------------------------------------------------------------------------------------------------------
//                           ERROR HANDLING

} // extern "C"

// template can't have C linkage

static thread_local std::string g_current_error_string;

const char* dasi_get_error_string(int) {
    return g_current_error_string.c_str();
}

int innerWrapFn(std::function<int()> f) {
    return f();
}

int innerWrapFn(std::function<void()> f) {
    f();
    return DASI_SUCCESS;
}

/**
 * @brief Catch the exceptions and capture the detail in dasi_error_t.
 * @param error  Pointer to the error message and code
 * @param fn     Function that throws an exception
 * @return true  No exception caught
 * @return false Exception caught
 */
template <typename FN>
[[ nodiscard ]]
int tryCatch(FN&& fn) {
    try {
        return innerWrapFn(fn);
    }
    catch (const eckit::UserError& e) {
        eckit::Log::error() << "User Error: " << e.what() << std::endl;
        g_current_error_string = e.what();
        return DASI_ERROR_USER;
    }
    catch (const eckit::AssertionFailed& e) {
        eckit::Log::error() << "Assertion Failed: " << e.what() << std::endl;
        g_current_error_string = e.what();
        return DASI_ERROR_ASSERT;
    }
    catch (const eckit::Exception& e) {
        eckit::Log::error() << "DASI Error: " << e.what() << std::endl;
        g_current_error_string = e.what();
        return DASI_ERROR;
    }
    catch (const std::exception& e) {
        eckit::Log::error() << "Unknown Error: " << e.what() << std::endl;
        g_current_error_string = e.what();
        return DASI_ERROR_UNKNOWN;
    }
    catch (...) {
        eckit::Log::error() << "Unknown Error!" << std::endl;
        g_current_error_string = "<unknown>";
        return DASI_ERROR_UNKNOWN;
    }
}

extern "C" {

// -----------------------------------------------------------------------------
//                           SESSION
// -----------------------------------------------------------------------------

int dasi_open(dasi_t** dasi, const char* filename) {
    return tryCatch([dasi, filename] {
        ASSERT(dasi);
        ASSERT(filename);
        *dasi = new Dasi(filename);
    });
}

int dasi_close(const dasi_t* dasi) {
    return tryCatch([dasi] {
        ASSERT(dasi);
        delete dasi;
    });
}

int dasi_archive(dasi_t* dasi, const dasi_key_t* key, const void* data, long length) {
    return tryCatch([dasi, key, data, length] {
        ASSERT(dasi);
        ASSERT(key);
        ASSERT(data);
        ASSERT(length >= 0);
        dasi->archive(*key, data, length);
    });
}

int dasi_flush(dasi_t* dasi) {
    return tryCatch([dasi] {
        ASSERT(dasi);
        dasi->flush();
    });
}

int dasi_list(dasi_t* dasi, const dasi_query_t* query, dasi_list_t** list) {
    return tryCatch([dasi, query, list] {
        ASSERT(dasi);
        ASSERT(query);
        ASSERT(list);
        *list = new dasi_list_t(dasi->list(*query));
    });
}

int dasi_free_list(const dasi_list_t* list) {
    return tryCatch([list] {
        ASSERT(list);
        delete list;
    });
}

int dasi_list_next(dasi_list_t* list) {
    return tryCatch(std::function<int()> {[list] {
        if (list->iterator == list->generator.end()) {
            if (list->first) list->uri_cache = list->iterator->location.uri.asRawString();
            return DASI_ITERATION_COMPLETE;
        }
        if (!list->first) ++list->iterator;
        if (list->iterator == list->generator.end()) return DASI_ITERATION_COMPLETE;
        list->uri_cache = list->iterator->location.uri.asRawString();
        list->first = false;
        return DASI_SUCCESS;
    }});
}

int dasi_list_attrs(const dasi_list_t* list,
                    dasi_key_t** key,
                    time_t* timestamp,
                    const char** uri,
                    long* offset,
                    long* length) {
    return tryCatch([list, key, timestamp, uri, offset, length] {
        ASSERT(list);
        ASSERT(list->iterator != list->generator.end());
        if (key) {
            *key = new Key(list->iterator->key);
        }
        if (timestamp) *timestamp = list->iterator->timestamp;
        if (uri) {
            *uri = list->uri_cache.c_str();
        }
        if (offset) *offset = list->iterator->location.offset;
        if (length) *length = list->iterator->location.length;
    });
}

int dasi_retrieve(dasi_t* dasi, const dasi_query_t* query, dasi_retrieve_t** retrieve) {
    return tryCatch([dasi, query, retrieve] {
        ASSERT(dasi);
        ASSERT(query);
        ASSERT(retrieve);
        *retrieve = new dasi_retrieve_t(dasi->retrieve(*query));
    });
}

int dasi_free_retrieve(const dasi_retrieve_t* retrieve) {
    return tryCatch([retrieve] {
        ASSERT(retrieve);
        delete retrieve;
    });
}

int dasi_retrieve_read(dasi_retrieve_t* retrieve, void* data, long* length) {
    return tryCatch(std::function<int()> {[retrieve, data, length] {
        ASSERT(retrieve);
        ASSERT(data);
        ASSERT(length);
        ASSERT(*length > 0);

        if (!retrieve->dh) {
            retrieve->dh = retrieve->retrieve.dataHandle();
            retrieve->dh->openForRead();
            retrieve->closer.emplace(*retrieve->dh);
        }

        *length = retrieve->dh->read(data, *length);
        if (*length == 0) return DASI_ITERATION_COMPLETE;
        return DASI_SUCCESS;
    }});
}

int dasi_retrieve_count(const dasi_retrieve_t* retrieve, long* count) {
    return tryCatch([retrieve, count] {
        ASSERT(retrieve);
        ASSERT(count);
        *count = retrieve->retrieve.count();
    });
}

// ---------------------------------------------------------------------------------------------------------------------
// KEY

int dasi_new_key(dasi_key_t** key) {
    return tryCatch([key] {
        *key = new Key();
    });
}

int dasi_new_key_from_string(dasi_key_t** key, const char* str) {
    return tryCatch([key, str] {
        *key = new Key(str);
    });
}

int dasi_free_key(const dasi_key_t* key) {
    return tryCatch([key] {
        ASSERT(key);
        delete key;
    });
}

int dasi_key_set(dasi_key_t* key, const char* keyword, const char* value) {
    return tryCatch([key, keyword, value] {
        ASSERT(key);
        ASSERT(keyword);
        ASSERT(value);
        key->set(keyword, value);
    });
}

int dasi_key_get_index(dasi_key_t* key, int n, const char** keyword, const char** value) {
    return tryCatch([key, n, keyword, value] {
        ASSERT(key);
        ASSERT(n >= 0);
        auto it = key->begin();
        std::advance(it, n);
        if (keyword) *keyword = it->first.c_str();
        if (value) *value = it->second.c_str();
    });
}

int dasi_key_get(dasi_key_t* key, const char* keyword, const char** value) {
    return tryCatch([key, keyword, value] {
        ASSERT(key);
        ASSERT(keyword);
        ASSERT(value);
        *value = key->get(keyword).c_str();
    });
}

int dasi_key_has(dasi_key_t* key, const char* keyword, bool* has) {
    return tryCatch([key, keyword, has] {
        ASSERT(key);
        ASSERT(keyword);
        ASSERT(has);
        *has = key->has(keyword);
    });
}

int dasi_key_count(dasi_key_t* key, long* count) {
    return tryCatch([key, count] {
        ASSERT(key);
        ASSERT(count);
        *count = key->size();
    });
}

int dasi_key_erase(dasi_key_t* key, const char* keyword) {
    return tryCatch([key, keyword] {
        ASSERT(key != nullptr);
        ASSERT(keyword != nullptr);
        key->erase(keyword);
    });
}

int dasi_key_clear(dasi_key_t* key) {
    return tryCatch([key] {
        ASSERT(key != nullptr);
        key->clear();
    });
}

// ---------------------------------------------------------------------------------------------------------------------
// QUERY

int dasi_new_query(dasi_query_t** query) {
    return tryCatch([query] {
        *query = new Query();
    });
}

int dasi_new_query_from_string(dasi_query_t** query, const char* str) {
    return tryCatch([query, str] {
        *query = new Query(str);
    });
}

int dasi_free_query(const dasi_query_t* query) {
    return tryCatch([query] {
        ASSERT(query);
        delete query;
    });
}

int dasi_query_set(dasi_query_t* query, const char* keyword, const char* values[], int num) {
    return tryCatch([query, keyword, values, num] {
        ASSERT(query);
        ASSERT(keyword);
        ASSERT(values);
        ASSERT(num >= 0);
        std::vector<std::string> vals;
        for (int i = 0; i < num; i++) {
            vals.push_back(values[i]);
        }
        query->set(keyword, vals);
    });
}

int dasi_query_append(dasi_query_t* query, const char* keyword, const char* value) {
    return tryCatch([query, keyword, value] {
        ASSERT(query);
        ASSERT(keyword);
        ASSERT(value);
        query->append(keyword, value);
    });
}

int dasi_query_get(dasi_query_t* query, const char* keyword, int num, const char** value) {
    return tryCatch([query, keyword, num, value] {
        ASSERT(query);
        ASSERT(keyword);
        ASSERT(value);
        ASSERT(num >= 0);
        const auto& values(query->get(keyword));
        ASSERT( num < values.size());
        *value = values[num].c_str();
    });
}

int dasi_query_has(dasi_query_t* query, const char* keyword, bool* has) {
    return tryCatch([query, keyword, has] {
        ASSERT(query);
        ASSERT(keyword);
        ASSERT(has);
        *has = query->has(keyword);
    });
}

int dasi_query_keyword_count(dasi_query_t* query, long* count) {
    return tryCatch([query, count] {
        ASSERT(query);
        ASSERT(count);
        *count = query->size();
    });
}

int dasi_query_value_count(dasi_query_t* query, const char* keyword, long* count) {
    return tryCatch([query, keyword, count] {
        ASSERT(query);
        ASSERT(count);
        *count = query->get(keyword).size();
    });
}

int dasi_query_erase(dasi_query_t* query, const char* keyword) {
    return tryCatch([query, keyword] {
        ASSERT(query != nullptr);
        ASSERT(keyword != nullptr);
        query->erase(keyword);
    });
}

int dasi_query_clear(dasi_query_t* query) {
    return tryCatch([query] {
        ASSERT(query != nullptr);
        query->clear();
    });
}

// ---------------------------------------------------------------------------------------------------------------------

}  // extern "C"
