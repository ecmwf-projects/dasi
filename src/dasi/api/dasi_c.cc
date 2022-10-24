
#include "dasi/api/dasi_c.h"

#include "dasi/api/Dasi.h"
#include "dasi/api/ReadHandle.h"

#include "dasi/util/Exceptions.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>

using namespace dasi::api;

extern "C" {

struct dasi_t : public Dasi {
    using Dasi::Dasi;
};

struct dasi_key_t : public Key {
    using Key::Key;
    dasi_key_t(const Key& key) : Key(key) {}
};

struct dasi_query_t : public Query {
    using Query::Query;
};

struct dasi_retrieve_result_t : public RetrieveResult {
    dasi_retrieve_result_t(RetrieveResult&& result) : RetrieveResult(std::move(result)) {}
};

struct dasi_read_handle_t {
public:
    dasi_read_handle_t() : handle_(nullptr) {}
    dasi_read_handle_t(ReadHandle *handle) : dasi_read_handle_t() {
        set(handle);
    }

    void set(ReadHandle *handle) {
        if (handle_ != nullptr) {
            delete handle_;
        }
        handle_ = handle;
    }

    void open() {
        handle_->open();
    }

    void close() {
        handle_->close();
    }

    size_t read(void *buf, size_t len) {
        return handle_->read(buf, len);
    }

private:
    ReadHandle *handle_;
};

struct dasi_retrieve_iterator_t {
public:
    using wrapped = RetrieveResult::const_iterator;

    dasi_retrieve_iterator_t(wrapped&& begin, wrapped&& end) : begin_(std::move(begin)), end_(std::move(end)) {}

    bool next() {
        ++begin_;
        return begin_ != end_;
    }

    dasi_key_t *key() const {
        ASSERT(begin_ != end_);
        return new dasi_key_t((*begin_).first.toKey());
    }

    dasi_read_handle_t *handle() const {
        ASSERT(begin_ != end_);
        return new dasi_read_handle_t((*begin_).second.toHandle());
    }
    
private:
    wrapped begin_;
    wrapped end_;
};

}

//----------------------------------------------------------------------------------------------------------------------

static std::string g_current_error;

static dasi_error _wrapInner(std::function<void()> f) {
    f();
    return DASI_SUCCESS;
}

static dasi_error _wrapInner(std::function<dasi_error(int)> f) {
    return f(0);
}

template<typename F>
static dasi_error wrapFunc(F f) {
    try {
        return _wrapInner(f);
    }
    catch (dasi::util::ObjectNotFound& e) {
        std::cerr << e.what() << std::endl;
        g_current_error = e.what();
        return DASI_NOT_FOUND;
    }
    catch (dasi::util::Exception& e) {
        std::cerr << "Caught DASI error: " << e.what() << std::endl;
        g_current_error = e.what();
        return DASI_ERROR;
    }
    catch (std::exception& e) {
        std::cerr << "Caught unexpected error: " << e.what() << std::endl;
        g_current_error = e.what();
        return DASI_UNEXPECTED;
    }
    catch (...) {
        std::cerr << "Caught unknown error" << std::endl;
        g_current_error = "Unknown";
        return DASI_UNEXPECTED;
    }
}

extern "C" {

const char *dasi_error_str(dasi_error err) {
    switch(err) {
    case DASI_SUCCESS:
        return "Success";
    case DASI_ERROR:
    case DASI_UNEXPECTED:
    case DASI_NOT_FOUND:
        return g_current_error.c_str();
    case DASI_ITERATOR_END:
        return "No more items";
    default:
        return "<unknown>";
    }
    ASSERT(false);
}

//----------------------------------------------------------------------------------------------------------------------

dasi_error dasi_open(const char *filename, dasi_t **session) {
    return wrapFunc([filename, session] {
        ASSERT(filename != nullptr);
        ASSERT(session != nullptr);
        std::ifstream in(filename);
        *session = new dasi_t(in);
    });
}

dasi_error dasi_open_str(const char *config, dasi_t **session) {
    return wrapFunc([config, session] {
        ASSERT(config != nullptr);
        ASSERT(session != nullptr);
        std::istringstream in(config);
        *session = new dasi_t(in);
    });
}

dasi_error dasi_close(dasi_t *session) {
    return wrapFunc([session] {
        ASSERT(session != nullptr);
        delete session;
    });
}

dasi_error dasi_put(dasi_t *session, dasi_key_t *key, const void *data, size_t len) {
    return wrapFunc([session, key, data, len] {
        ASSERT(session != nullptr);
        ASSERT(key != nullptr);
        ASSERT(data != nullptr);
        session->archive(*key, data, len);
    });
}

dasi_error dasi_get(dasi_t *session, dasi_query_t *query, dasi_retrieve_result_t **result) {
    return wrapFunc([session, query, result] {
        ASSERT(session != nullptr);
        ASSERT(query != nullptr);
        ASSERT(result != nullptr);
        *result = new dasi_retrieve_result_t(std::move(session->retrieve(*query)));
    });
}

//----------------------------------------------------------------------------------------------------------------------

dasi_error dasi_key_new(dasi_key_t **key) {
    return wrapFunc([key] {
        ASSERT(key != nullptr);
        *key = new dasi_key_t;
    });
}

dasi_error dasi_key_copy(const dasi_key_t *from, dasi_key_t **to) {
    return wrapFunc([from, to] {
        ASSERT(from != nullptr);
        ASSERT(to != nullptr);
        *to = new dasi_key_t(*from);
    });
}

dasi_error dasi_key_destroy(dasi_key_t *key) {
    return wrapFunc([key] {
        ASSERT(key != nullptr);
        delete key;
    });
}

dasi_error dasi_key_set(dasi_key_t *key, const char *keyword, const char *value) {
    return wrapFunc([key, keyword, value] {
        ASSERT(key != nullptr);
        ASSERT(keyword != nullptr);
        ASSERT(value != nullptr);
        key->set(keyword, value);
    });
}

dasi_error dasi_key_del(dasi_key_t *key, const char *keyword) {
    return wrapFunc([key, keyword] {
        ASSERT(key != nullptr);
        ASSERT(keyword != nullptr);
        key->erase(keyword);
    });
}

dasi_error dasi_key_get(dasi_key_t *key, const char *keyword, const char **value) {
    return wrapFunc([key, keyword, value] {
        ASSERT(key != nullptr);
        ASSERT(keyword != nullptr);
        ASSERT(value != nullptr);
        *value = key->get(keyword).c_str();
    });
}

dasi_error dasi_key_cmp(dasi_key_t *lhs, dasi_key_t *rhs, int *result) {
    return wrapFunc([lhs, rhs, result] {
        ASSERT(lhs != nullptr);
        ASSERT(rhs != nullptr);
        ASSERT(result != nullptr);
        if (*lhs == *rhs) {
            *result = 0;
        }
        else if (*lhs < *rhs) {
            *result = -1;
        }
        else {
            *result = 1;
        }
    });
}

//----------------------------------------------------------------------------------------------------------------------

dasi_error dasi_query_new(dasi_query_t **query) {
    return wrapFunc([query] {
        ASSERT(query != nullptr);
        *query = new dasi_query_t;
    });
}

dasi_error dasi_query_destroy(dasi_query_t *query) {
    return wrapFunc([query] {
        ASSERT(query != nullptr);
        delete query;
    });
}

dasi_error dasi_query_set(dasi_query_t *query, const char *keyword, const char *values[], size_t num) {
    return wrapFunc([query, keyword, values, num] {
        ASSERT(query != nullptr);
        ASSERT(keyword != nullptr);
        ASSERT(values != nullptr);
        query->set(keyword, Query::value_type{values, values + num});
    });
}

dasi_error dasi_query_append(dasi_query_t *query, const char *keyword, const char *value) {
    return wrapFunc([query, keyword, value] {
        ASSERT(query != nullptr);
        ASSERT(keyword != nullptr);
        ASSERT(value != nullptr);
        query->append(keyword, value);
    });
}

//----------------------------------------------------------------------------------------------------------------------

dasi_error dasi_retrieve_result_destroy(dasi_retrieve_result_t *result) {
    return wrapFunc([result] {
        ASSERT(result != nullptr);
        delete result;
    });
}

dasi_error dasi_retrieve_result_iterate(dasi_retrieve_result_t *result, dasi_retrieve_iterator_t **iterator) {
    return wrapFunc([result, iterator] {
        ASSERT(result != nullptr);
        ASSERT(iterator != nullptr);
        *iterator = new dasi_retrieve_iterator_t(result->begin(), result->end());
    });
}

dasi_error dasi_retrieve_result_get_handle(dasi_retrieve_result_t *result, dasi_read_handle_t **handle) {
    return wrapFunc([result, handle] {
        ASSERT(result != nullptr);
        ASSERT(handle != nullptr);
        *handle = new dasi_read_handle_t(result->toHandle());
    });
}

//----------------------------------------------------------------------------------------------------------------------

dasi_error dasi_retrieve_iterator_destroy(dasi_retrieve_iterator_t *iterator) {
    return wrapFunc([iterator] {
        ASSERT(iterator != nullptr);
        delete iterator;
    });
}

dasi_error dasi_retrieve_iterator_next(dasi_retrieve_iterator_t *iterator) {
    return wrapFunc([iterator] (int) {
        ASSERT(iterator != nullptr);
        return iterator->next()? DASI_SUCCESS : DASI_ITERATOR_END;
    });
}

dasi_error dasi_retrieve_iterator_get_key(dasi_retrieve_iterator_t *iterator, dasi_key_t **key) {
    return wrapFunc([iterator, key] {
        ASSERT(iterator != nullptr);
        ASSERT(key != nullptr);
        *key = iterator->key();
    });
}

dasi_error dasi_retrieve_iterator_get_handle(dasi_retrieve_iterator_t *iterator, dasi_read_handle_t **handle) {
    return wrapFunc([iterator, handle] {
        ASSERT(iterator != nullptr);
        ASSERT(handle != nullptr);
        *handle = iterator->handle();
    });
}

//----------------------------------------------------------------------------------------------------------------------

dasi_error dasi_read_handle_destroy(dasi_read_handle_t *handle) {
    return wrapFunc([handle] {
        ASSERT(handle != nullptr);
        handle->set(nullptr);
        delete handle;
    });
}

dasi_error dasi_read_handle_open(dasi_read_handle_t *handle) {
    return wrapFunc([handle] {
        ASSERT(handle != nullptr);
        handle->open();
    });
}

dasi_error dasi_read_handle_close(dasi_read_handle_t *handle) {
    return wrapFunc([handle] {
        ASSERT(handle != nullptr);
        handle->close();
    });
}

dasi_error dasi_read_handle_read(dasi_read_handle_t *handle, void *buffer, size_t size, size_t *len) {
    return wrapFunc([handle, buffer, size, len] {
        ASSERT(handle != nullptr);
        ASSERT(buffer != nullptr);
        ASSERT(len != nullptr);
        *len = handle->read(buffer, size);
    });
}

}