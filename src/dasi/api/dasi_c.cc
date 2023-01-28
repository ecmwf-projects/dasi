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

#include <functional>

extern "C" {

// ---------------------------------------------------------------------------------------------------------------------

struct Dasi : public dasi::Dasi {
    using dasi::Dasi::Dasi;
};

struct Key : public dasi::Key {
    using dasi::Key::Key;

    Key(const dasi::Key& other) : dasi::Key(other) {}
};

struct Query : public dasi::Query {
};

struct ListGenerator : public dasi::ListGenerator {
    ListGenerator(dasi::ListGenerator&& iter) :
            dasi::ListGenerator(std::move(iter)) {}
};

struct ListElement : public dasi::ListElement {
    using dasi::ListElement::ListElement;

    ListElement(const dasi::ListElement& other) : dasi::ListElement(other) {}
};

// ---------------------------------------------------------------------------------------------------------------------
//                           ERROR HANDLING

} // extern "C"

// template can't have C linkage

static thread_local std::string g_current_error_string;

const char* dasi_error_get_message(int) {
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

#if 0
dasi_t* dasi_new(const char* filename, dasi_error_t** error) {
    Dasi* result = nullptr;
    return tryCatch([filename, &result] {
        ASSERT(filename != nullptr);
        result = new Dasi(filename);
    });
}

void dasi_delete(dasi_t* session, dasi_error_t** error) {
    tryCatch(error, [&session] {
        ASSERT(session != nullptr);
        delete session;
        session = nullptr;
    });
}

void dasi_archive(dasi_t* session, const dasi_key_t* key, const void* data,
                  size_t length, dasi_error_t** error) {
    tryCatch(error, [session, key, data, length] {
        ASSERT(session != nullptr);
        ASSERT(key != nullptr);
        ASSERT(data != nullptr);
        session->archive(*key, data, length);
    });
}

dasi_list_t* dasi_list(dasi_t* session, const dasi_query_t* query,
                       dasi_error_t** error) {
    ListGenerator* result = nullptr;
    tryCatch(error, [session, query, &result] {
        ASSERT(session != nullptr);
        ASSERT(query != nullptr);
        auto&& list = session->list(*query);
        result      = new ListGenerator(std::move(list));
    });
    return result;
}

void dasi_list_delete(dasi_list_t* list, dasi_error_t** error) {
    tryCatch(error, [&list] {
        ASSERT(list != nullptr);
        delete list;
        list = nullptr;
    });
}

void dasi_flush(dasi_t* session, dasi_error_t** error) {
    tryCatch(error, [session] {
        ASSERT(session != nullptr);
        session->flush();
    });
}

#endif
// -----------------------------------------------------------------------------
//                           KEY
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
//                           QUERY
// -----------------------------------------------------------------------------

#if 0
dasi_query_t* dasi_query_new(dasi_error_t** error) {
    Query* result = nullptr;
    tryCatch(error, [&result] { result = new Query(); });
    return result;
}

void dasi_query_delete(dasi_query_t* query, dasi_error_t** error) {
    tryCatch(error, [&query] {
        ASSERT(query != nullptr);
        delete query;
        query = nullptr;
    });
}

void dasi_query_set(dasi_query_t* query, const char* keyword,
                    const char* values[], const size_t num,
                    dasi_error_t** error) {
    tryCatch(error, [query, keyword, values, num] {
        ASSERT(query != nullptr);
        ASSERT(keyword != nullptr);
        ASSERT(values != nullptr);
        query->set(keyword, Query::value_type{values, values + num});
    });
}

void dasi_query_append(dasi_query_t* query, const char* keyword,
                       const char* value, dasi_error_t** error) {
    tryCatch(error, [query, keyword, value] {
        ASSERT(query != nullptr);
        ASSERT(keyword != nullptr);
        ASSERT(value != nullptr);
        query->append(keyword, value);
    });
}

// -----------------------------------------------------------------------------
//                           LIST
// -----------------------------------------------------------------------------

dasi_key_t* dasi_list_elem_get_key(dasi_list_elem_t* element) {
    ASSERT(element != nullptr);
    return new Key(std::move(element->key));
}

// List: Triple-iterator
// -----------------------------------------------------------------------------

dasi_list_elem_t* dasi_list_first(dasi_list_t* list) {
    ASSERT(list != nullptr);
    const auto& current = *list->begin();
    return new ListElement(current);
}

dasi_list_elem_t* dasi_list_next(dasi_list_t* list, dasi_list_elem_t* element) {
    ASSERT(element != nullptr);
    delete element;
    ASSERT(list != nullptr);
    ++list->begin();
    return dasi_list_first(list);
}

int dasi_list_done(dasi_list_t* p_list) {
    auto* list = reinterpret_cast<dasi::ListGenerator*>(p_list);
    ASSERT(list != nullptr);
    return static_cast<int>(list->begin().done());
}
#endif

// ---------------------------------------------------------------------------------------------------------------------

}  // extern "C"
