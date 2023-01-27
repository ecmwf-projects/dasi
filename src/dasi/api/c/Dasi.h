/*
 * (C) Copyright 2023- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http: //www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/**
 * @file DASI C API Conventions
 *
 * - #define and Enum constants should be all CAPS.
 * - Function, variable, typedef, struct, enum tag names should be lower case.
 *   Separate the words by underscore "_" for clarity. Typedefs end with "_t".
 * - Functions that are PUBLIC should start with "dasi_".
 * - Each object is represented as a pointer to an opaque structure.
 * - If the function creates a new object, it has to be released using the
 *   appropriate "<obj>_delete" call.
 *
 */

#pragma once

#include <stddef.h>

// -----------------------------------------------------------------------------
//                           TYPES
// -----------------------------------------------------------------------------

typedef struct Dasi* dasi_t;
typedef struct Key dasi_key_t;
typedef struct Query dasi_query_t;
typedef struct DasiError* dasi_error_t;
typedef struct ListGenerator dasi_list_t;
typedef struct ListElement dasi_list_elem_t;

/// DASI Error Codes
typedef enum dasi_error_enum_t
{
    DASI_SUCCESS        = 0, /* Operation succeded. */
    DASI_ERROR          = 1, /* Operation failed. */
    DASI_ERROR_UNKNOWN  = 2, /* Failed with an unknown error. */
    DASI_ERROR_USER     = 3, /* Failed with an user error. */
    DASI_ERROR_ITERATOR = 4  /* Failed with an iterator error. */
} dasi_error_enum_t;

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                           ERROR HANDLING
// -----------------------------------------------------------------------------

/**
 * @brief Get the error message from the error object.
 * @param error Error object.
 * @return const char* Error message.
 */
const char* dasi_error_get_message(const dasi_error_t error);

/**
 * @brief Get the error code from the error object.
 * @param error Error object.
 * @return dasi_error_enum_t Error code.
 */
dasi_error_enum_t dasi_error_get_code(const dasi_error_t error);

// -----------------------------------------------------------------------------
//                           SESSION
// -----------------------------------------------------------------------------

/// Create a new session object using the given configuration file.
dasi_t dasi_new(const char* filename, dasi_error_t* error);

/// Release the session and delete the object.
void dasi_delete(dasi_t p_session, dasi_error_t* error);

/**
 * @brief Writes data to the object store.
 *
 * @note Data is not guaranteed accessible nor persisted (wrt. failure),
 * until dasi_flush() is called.
 *
 * @param p_session Dasi session
 * @param p_key Metadata description of the data to store and index
 * @param data Pointer to the read-only data
 * @param length Length of "data" in bytes
 * @param error Error object
 */
void dasi_archive(dasi_t p_session, const dasi_key_t* p_key, const void* data,
                  size_t length, dasi_error_t* error);

/**
 * @brief Returns the (meta)list of the data described by the query.
 *
 * @param p_session Dasi session that has the archive
 * @param p_query Description of the metadata to be listed
 * @param error Error object
 * @return dasi_list_t List object
 */
dasi_list_t* dasi_list(dasi_t p_session, const dasi_query_t* p_query,
                       dasi_error_t* error);

/// Flushes all buffers and ensures the internal state is safe.
void dasi_flush(dasi_t p_session, dasi_error_t* error);

// -----------------------------------------------------------------------------
//                           KEY
// -----------------------------------------------------------------------------

/// Create a new key object.
dasi_key_t* dasi_key_new(dasi_error_t* error);

/// Release the key and delete the object.
void dasi_key_delete(dasi_key_t* p_key, dasi_error_t* error);

/// Set the value of the specified keyword.
/// @note The keyword is added if it's missing.
void dasi_key_set(dasi_key_t* p_key, const char* keyword, const char* value,
                  dasi_error_t* error);

/// Erase the keyword:value pair specified by its keyword.
void dasi_key_erase(dasi_key_t* p_key, const char* keyword, dasi_error_t* error);

// -----------------------------------------------------------------------------
//                           QUERY
// -----------------------------------------------------------------------------

/// Create a new query object.
dasi_query_t* dasi_query_new(dasi_error_t* error);

/// Release a query object.
void dasi_query_delete(dasi_query_t* p_query, dasi_error_t* error);

/// Set all values for a keyword at once.
/// @note The keyword is added if it's missing.
void dasi_query_set(dasi_query_t* p_query, const char* keyword,
                    const char* values[], const size_t num,
                    dasi_error_t* error);

/// Append one value to the set for the given keyword.
/// @note The keyword is added if it's missing.
void dasi_query_append(dasi_query_t* p_query, const char* keyword,
                       const char* value, dasi_error_t* error);

// -----------------------------------------------------------------------------
//                           LIST
// -----------------------------------------------------------------------------

dasi_key_t* dasi_list_elem_get_key(dasi_list_elem_t* p_element);

dasi_list_elem_t* dasi_list_first(dasi_list_t* p_list);

dasi_list_elem_t* dasi_list_next(dasi_list_t* p_list);

int dasi_list_done(dasi_list_t* p_list);

#ifdef __cplusplus
}
#endif
