
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

//----------------------------------------------------------------------------------------------------------------------

/// Error codes

enum dasi_error {
    DASI_SUCCESS = 0,
    DASI_ERROR = 1,
    DASI_UNEXPECTED = 2,
    DASI_NOT_FOUND = 3,
    DASI_ITERATOR_END = 4
};

//----------------------------------------------------------------------------------------------------------------------

/// Types

struct dasi_session_t;
typedef struct dasi_session_t dasi_session_t;

struct dasi_key_t;
typedef struct dasi_key_t dasi_key_t;

struct dasi_query_t;
typedef struct dasi_key_t dasi_key_t;

struct dasi_retrieve_result_t;
typedef struct dasi_retrieve_result_t dasi_retrieve_result_t;

struct dasi_retrieve_iterator_t;
typedef struct dasi_retrieve_iterator_t dasi_retrieve_iterator_t;

struct dasi_read_handle_t;
typedef struct dasi_read_handle_t dasi_read_handle_t;

//----------------------------------------------------------------------------------------------------------------------

/// Session

dasi_error dasi_open(const char *filename, dasi_session_t **session);
dasi_error dasi_open_str(const char *config, dasi_session_t **session);
dasi_error dasi_close(dasi_session_t *session);

dasi_error dasi_put(dasi_session_t *session, dasi_key_t *key, const void *data, size_t len);

dasi_error dasi_get(dasi_session_t *session, dasi_query_t *query, dasi_retrieve_result_t **result);

//----------------------------------------------------------------------------------------------------------------------

/// Key

dasi_error dasi_key_new(dasi_key_t **key);
dasi_error dasi_key_copy(const dasi_key_t *from, dasi_key_t **to);
dasi_error dasi_key_destroy(dasi_key_t *key);

dasi_error dasi_key_set(dasi_key_t *key, const char *keyword, const char *value);
dasi_error dasi_key_del(dasi_key_t *key, const char *keyword);
dasi_error dasi_key_get(dasi_key_t *key, const char *keyword, const char **value);

dasi_error dasi_key_cmp(dasi_key_t *lhs, dasi_key_t *rhs, int *result);

//----------------------------------------------------------------------------------------------------------------------

/// Query

dasi_error dasi_query_new(dasi_query_t **query);
dasi_error dasi_query_destroy(dasi_query_t *query);

dasi_error dasi_query_set(dasi_query_t *query, const char *keyword, const char *values[], size_t num);

//----------------------------------------------------------------------------------------------------------------------

/// Retrieve result

dasi_error dasi_retrieve_result_destroy(dasi_retrieve_result_t *result);

dasi_error dasi_retrieve_result_iterate(dasi_retrieve_result_t *result, dasi_retrieve_iterator_t **iterator);
dasi_error dasi_retrieve_result_get_handle(dasi_retrieve_result_t *result, dasi_read_handle_t **handle);

//----------------------------------------------------------------------------------------------------------------------

/// Retrieve result iterator

dasi_error dasi_retrieve_iterator_destroy(dasi_retrieve_iterator_t *iterator);

dasi_error dasi_retrieve_iterator_next(dasi_retrieve_iterator_t *iterator);
dasi_error dasi_retrieve_iterator_get_key(dasi_retrieve_iterator_t *iterator, dasi_key_t **key);
dasi_error dasi_retrieve_iterator_get_handle(dasi_retrieve_iterator_t *iterator, dasi_read_handle_t **handle);

//----------------------------------------------------------------------------------------------------------------------

/// Read handle

dasi_error dasi_read_handle_destroy(dasi_read_handle_t *handle);

dasi_error dasi_read_handle_open(dasi_read_handle_t *handle);
dasi_error dasi_read_handle_close(dasi_read_handle_t *handle);
dasi_error dasi_read_handle_read(dasi_read_handle_t *handle, void *buffer, size_t size, size_t *len);

#ifdef __cplusplus
}
#endif
