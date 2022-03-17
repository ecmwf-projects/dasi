
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/** DASI C API
 *
 * Conventions:
 *
 * - Every function in the API returns a `dasi_error` value (see below).
 * - Every 'object' is represented as a pointer to an opaque structure.
 * - If a function returns a value, it is stored into the pointer passed as the
 *   last argument of said function.
 * - If the function creates a new object, it has to be released using the
 *   appropriate `<obj>_destroy` call.
 *
 */

//----------------------------------------------------------------------------------------------------------------------

/// Error codes

enum dasi_error {
    DASI_SUCCESS = 0,
    DASI_ERROR = 1,
    DASI_UNEXPECTED = 2,
    DASI_NOT_FOUND = 3,
    DASI_ITERATOR_END = 4
};
typedef enum dasi_error dasi_error;

//----------------------------------------------------------------------------------------------------------------------

/// Types

struct dasi_session_t;
typedef struct dasi_session_t dasi_session_t;

struct dasi_key_t;
typedef struct dasi_key_t dasi_key_t;

struct dasi_query_t;
typedef struct dasi_query_t dasi_query_t;

struct dasi_retrieve_result_t;
typedef struct dasi_retrieve_result_t dasi_retrieve_result_t;

struct dasi_retrieve_iterator_t;
typedef struct dasi_retrieve_iterator_t dasi_retrieve_iterator_t;

struct dasi_read_handle_t;
typedef struct dasi_read_handle_t dasi_read_handle_t;

struct dasi_list_iterator_t;
typedef struct dasi_list_iterator_t dasi_list_iterator_t;

//----------------------------------------------------------------------------------------------------------------------

/// Session

/// Open a session using the given configuration file. Returns a new session object.
dasi_error dasi_open(const char *filename, dasi_session_t **session);
/// Open a session using the given configuration string. Returns a new session object.
dasi_error dasi_open_str(const char *config, dasi_session_t **session);
/// Close the session and release the object.
dasi_error dasi_close(dasi_session_t *session);

/// Store an object.
dasi_error dasi_put(dasi_session_t *session, dasi_key_t *key, const void *data, size_t len);

/// Make a query to retrieve objects. Returns a new result object.
dasi_error dasi_get(dasi_session_t *session, dasi_query_t *query, dasi_retrieve_result_t **result);

dasi_error dasi_list(dasi_session_t *session, dasi_query_t *query, dasi_list_iterator_t **result);

//----------------------------------------------------------------------------------------------------------------------

/// Key

/// Create a new empty key object.
dasi_error dasi_key_new(dasi_key_t **key);
/// Create a new key object by copying an existing one. Returns a new key object.
dasi_error dasi_key_copy(const dasi_key_t *from, dasi_key_t **to);
/// Release a key object.
dasi_error dasi_key_destroy(dasi_key_t *key);

/// Set a keyword-value pair.
dasi_error dasi_key_set(dasi_key_t *key, const char *keyword, const char *value);
/// Delete the given keyword and its value.
dasi_error dasi_key_del(dasi_key_t *key, const char *keyword);
/// Get the value of a keyword. Returns a pointer to an internal buffer,
/// valid until the next call to `dasi_key_del` on this keyword or
/// `dasi_key_destroy` on the key.
dasi_error dasi_key_get(dasi_key_t *key, const char *keyword, const char **value);

/// Compare the given keys according to lexicographical order (like `strcmp`).
dasi_error dasi_key_cmp(dasi_key_t *lhs, dasi_key_t *rhs, int *result);

//----------------------------------------------------------------------------------------------------------------------

/// Query

/// Create a new empty query object.
dasi_error dasi_query_new(dasi_query_t **query);
/// Release a query object.
dasi_error dasi_query_destroy(dasi_query_t *query);

/// Set all values for a keyword at once. The keyword is added if it does not exist.
dasi_error dasi_query_set(dasi_query_t *query, const char *keyword, const char *values[], size_t num);
/// Append one value to the set for the given keyword. The keyword is added if it does not exist.
dasi_error dasi_query_append(dasi_query_t *query, const char *keyword, const char *value);

//----------------------------------------------------------------------------------------------------------------------

/// Retrieve result

/// Release a result object.
dasi_error dasi_retrieve_result_destroy(dasi_retrieve_result_t *result);

/// Start iterating over the result. Returns a new iterator object.
dasi_error dasi_retrieve_result_iterate(dasi_retrieve_result_t *result, dasi_retrieve_iterator_t **iterator);
/// Read the result as a single stream. Returns a new handle object.
dasi_error dasi_retrieve_result_get_handle(dasi_retrieve_result_t *result, dasi_read_handle_t **handle);

//----------------------------------------------------------------------------------------------------------------------

/// Retrieve result iterator

/// Release an iterator object.
dasi_error dasi_retrieve_iterator_destroy(dasi_retrieve_iterator_t *iterator);

/// Advance to the next item. Error code is `DASI_ITERATOR_END` if there is none.
dasi_error dasi_retrieve_iterator_next(dasi_retrieve_iterator_t *iterator);
/// Get the key associated with the current item. Returns a new key object.
dasi_error dasi_retrieve_iterator_get_key(dasi_retrieve_iterator_t *iterator, dasi_key_t **key);
/// Get the data handle associated to the current item. Returns a new handle object.
dasi_error dasi_retrieve_iterator_get_handle(dasi_retrieve_iterator_t *iterator, dasi_read_handle_t **handle);

//----------------------------------------------------------------------------------------------------------------------

/// Read handle

/// Release a handle object.
dasi_error dasi_read_handle_destroy(dasi_read_handle_t *handle);

/// Open the underlying data stream. MUST be called before reading.
dasi_error dasi_read_handle_open(dasi_read_handle_t *handle);
/// Close the underlying data stream. MUST be called before releasing.
dasi_error dasi_read_handle_close(dasi_read_handle_t *handle);
/// Reads `size` bytes into the given buffer. Returns the number of bytes
/// actually read. End of stream has been reached if `*len < size`.
dasi_error dasi_read_handle_read(dasi_read_handle_t *handle, void *buffer, size_t size, size_t *len);

//----------------------------------------------------------------------------------------------------------------------

/// List iterator

dasi_error dasi_list_iterator_destroy(dasi_list_iterator_t *iterator);

dasi_error dasi_list_iterator_next(dasi_list_iterator_t *iterator);
dasi_error dasi_list_iterator_get(dasi_list_iterator_t *iterator, dasi_key_t **key);

#ifdef __cplusplus
}
#endif
