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

/* ---------------------------------------------------------------------------------------------------------------------
 * TYPES
 * -----*/

struct Dasi;
typedef struct Dasi dasi_t;

struct Key;
typedef struct Key dasi_key_t;

struct Query;
typedef struct Query dasi_query_t;

/*struc DasiError;
typedef struct DasiError dasi_error_t;


typedef struct ListGenerator dasi_list_t;
typedef struct ListElement dasi_list_elem_t;
 */

/// DASI Error Codes
typedef enum dasi_error_values_t
{
    DASI_SUCCESS             = 0, /* Operation succeded. */
    DASI_ITERATION_COMPLETE  = 1, /* All elements have been returned */
    DASI_ERROR               = 2, /* Operation failed. */
    DASI_ERROR_UNKNOWN       = 3, /* Failed with an unknown error. */
    DASI_ERROR_USER          = 4, /* Failed with an user error. */
    DASI_ERROR_ITERATOR      = 5, /* Failed with an iterator error. */
    DASI_ERROR_ASSERT        = 6, /* Failed with an assert() */
} dasi_error_enum_t;

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                           ERROR HANDLING
// -----------------------------------------------------------------------------

#if 0
/**
 * @brief Get the error message from the error object.
 * @param error Error object.
 * @return const char* Error message.
 */
const char* dasi_error_get_message(const dasi_error_t* error);

/**
 * @brief Get the error code from the error object.
 * @param error Error object.
 * @return dasi_error_enum_t Error code.
 */
dasi_error_enum_t dasi_error_get_code(const dasi_error_t* error);

#endif
/* ---------------------------------------------------------------------------------------------------------------------
 * DASI SESSION
 * ------------*/

/// Create a new session object using the given configuration file.
int dasi_open(dasi_t** dasi, const char* filename);

/// Release the session and delete the object.
int dasi_close(const dasi_t* dasi);

/**
 * @brief Writes data to the object store.
 *
 * @note Data is not guaranteed accessible nor persisted (wrt. failure),
 * until dasi_flush() is called.
 *
 * @param dasi Dasi session
 * @param key Metadata description of the data to store and index
 * @param data Pointer to the read-only data
 * @param length Length of "data" in bytes
 */
int dasi_archive(dasi_t* dasi, const dasi_key_t* key, const void* data, long length);

int dasi_flush(dasi_t* dasi);

#if 0
dasi_list_t* dasi_list(dasi_t* p_session, const dasi_query_t* query,
                       dasi_error_t** error);

void dasi_list_delete(dasi_list_t* list, dasi_error_t** error);


#endif
/* ---------------------------------------------------------------------------------------------------------------------
 * KEY
 * ---*/

/**
 * @brief Construct a new Dasi key object
 * @param Key instance. Returned value must be freed using dasi_free_key
 * @return Return code (#dasi_error_values_t)
 */
int dasi_new_key(dasi_key_t** key);

/**
 * Construct a Dasi key from a string
 */
int dasi_new_key_from_string(dasi_key_t** key, const char* str);

/** Release the key and delete the object. */
int dasi_free_key(const dasi_key_t* key);

/**
 * Set the value of the specified keyword.
 * @note The keyword is added if it's missing.
 */
int dasi_key_set(dasi_key_t* key, const char* keyword, const char* value);

/**
 * Get the value of a specified keyword in a key
 * @param value The value to be returned. This will point towards an internal character buffer with a lifetime equal to that of the key
 */
int dasi_key_get(dasi_key_t* k, const char* keyword, const char** value);

/** Does the key have a specified keyword */
int dasi_key_has(dasi_key_t* key, const char* keyword, bool* has);

/** How many keys have been set */
int dasi_key_count(dasi_key_t* key, long* count);

/** Erase the keyword:value pair specified by its keyword. */
int dasi_key_erase(dasi_key_t* key, const char* keyword);

/** Erase all elements in the key */
int dasi_key_clear(dasi_key_t* key);

/* ---------------------------------------------------------------------------------------------------------------------
 * QUERY
 * -----*/

int dasi_new_query(dasi_query_t** query);

int dasi_new_query_from_string(dasi_query_t** query, const char* str);

int dasi_free_query(const dasi_query_t* query);

int dasi_query_set(dasi_query_t* query, const char* keyword, const char* values[], int num);

int dasi_query_append(dasi_query_t* query, const char* keyword, const char* value);

int dasi_query_keyword_count(dasi_query_t* query, long* count);

int dasi_query_value_count(dasi_query_t* query, const char* keyword, long* count);

int dasi_query_get(dasi_query_t* query, const char* keyword, int num, const char** value);

int dasi_query_has(dasi_query_t* query, const char* keyword, bool* has);

int dasi_query_erase(dasi_query_t* query, const char* keyword);

int dasi_query_clear(dasi_query_t* query);

#if 0
// -----------------------------------------------------------------------------
//                           LIST
// -----------------------------------------------------------------------------

dasi_key_t* dasi_list_elem_get_key(dasi_list_elem_t* element);

dasi_list_elem_t* dasi_list_first(dasi_list_t* list);

dasi_list_elem_t* dasi_list_next(dasi_list_t* list, dasi_list_elem_t* element);

int dasi_list_done(dasi_list_t* list);
#endif

#ifdef __cplusplus
}
#endif
