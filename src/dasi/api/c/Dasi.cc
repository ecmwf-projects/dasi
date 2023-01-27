/*
 * (C) Copyright 2023 ECMWF.
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http: //www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "dasi/api/c/Dasi.h"
#include "dasi/api/Dasi.h"

#include <eckit/exception/Exceptions.h>

struct Dasi : public dasi::Dasi {
    using dasi::Dasi::Dasi;
};

struct Key : public dasi::Key {
    using dasi::Key::Key;
    Key(const dasi::Key& other) : dasi::Key(other) {}
};

struct Query : public dasi::Query {};

struct ListGenerator : public dasi::ListGenerator {
    ListGenerator(dasi::ListGenerator&& iter) :
        dasi::ListGenerator(std::move(iter)) {}
};

struct ListElement : public dasi::ListElement {
    using dasi::ListElement::ListElement;
    ListElement(const dasi::ListElement& other) : dasi::ListElement(other) {}
};

// -----------------------------------------------------------------------------
//                           ERROR HANDLING
// -----------------------------------------------------------------------------

struct DasiError {
    void Set(const std::string& msg, const dasi_error_enum_t code) {
        this->Code    = code;
        this->Message = msg;
    }

public:
    std::string Message;
    dasi_error_enum_t Code{DASI_SUCCESS};
};

extern "C" const char* dasi_error_get_message(const dasi_error_t* error) {
    return error->Message.c_str();
}

extern "C" dasi_error_enum_t dasi_error_get_code(const dasi_error_t* error) {
    return error->Code;
}

DasiError* g_error = new DasiError();

/**
 * @brief Catch the exceptions and capture the detail in dasi_error_t.
 * @param error  Pointer to the error message and code
 * @param fn     Function that throws an exception
 * @return true  No exception caught
 * @return false Exception caught
 */
template <typename F>
bool tryCatch(dasi_error_t** error, F&& fn) {
    *error = g_error;
    try {
        fn();
        // eckit::Log::debug() << "[Success]" << std::endl;
        g_error->Set("Success", DASI_SUCCESS);
    }
    catch (const eckit::UserError& e) {
        eckit::Log::error() << "User Error: " << e.what() << std::endl;
        g_error->Set(e.what(), DASI_ERROR_USER);
        return false;
    }
    catch (const eckit::Exception& e) {
        eckit::Log::error() << "DASI Error: " << e.what() << std::endl;
        g_error->Set(e.what(), DASI_ERROR);
        return false;
    }
    catch (const std::exception& e) {
        eckit::Log::error() << "Unkown Error: " << e.what() << std::endl;
        g_error->Set(e.what(), DASI_ERROR_UNKNOWN);
        return false;
    }
    catch (...) {
        eckit::Log::error() << "Unkown Error!" << std::endl;
        g_error->Set("Unknown Error", DASI_ERROR_UNKNOWN);
        return false;
    }
    return true;
}

extern "C" {

// -----------------------------------------------------------------------------
//                           SESSION
// -----------------------------------------------------------------------------

dasi_t* dasi_new(const char* filename, dasi_error_t** error) {
    Dasi* result = nullptr;
    tryCatch(error, [filename, &result] {
        ASSERT(filename != nullptr);
        result = new Dasi(filename);
    });
    return result;
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

// -----------------------------------------------------------------------------
//                           KEY
// -----------------------------------------------------------------------------

dasi_key_t* dasi_key_new(dasi_error_t** error) {
    Key* result = nullptr;
    tryCatch(error, [&result] { result = new Key(); });
    return result;
}

void dasi_key_delete(dasi_key_t* key, dasi_error_t** error) {
    tryCatch(error, [&key] {
        ASSERT(key != nullptr);
        delete key;
        key = nullptr;
    });
}

void dasi_key_set(dasi_key_t* key, const char* keyword, const char* value,
                  dasi_error_t** error) {
    tryCatch(error, [key, keyword, value] {
        ASSERT(key != nullptr);
        ASSERT(keyword != nullptr);
        ASSERT(value != nullptr);
        key->set(keyword, value);
    });
}

void dasi_key_erase(dasi_key_t* key, const char* keyword,
                    dasi_error_t** error) {
    tryCatch(error, [key, keyword] {
        ASSERT(key != nullptr);
        ASSERT(keyword != nullptr);
        key->erase(keyword);
    });
}

// -----------------------------------------------------------------------------
//                           QUERY
// -----------------------------------------------------------------------------

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
}
