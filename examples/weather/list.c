
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dasi/api/dasi_c.h"
#include "utils.h"

#define ASSERT_SUCCESS(stat, dasi_fn) \
    stat = dasi_fn;                   \
    assert(stat == DASI_SUCCESS)

int main(int argc, char** argv) {
    const char* config_path = NULL;
    const char* str_query   = NULL;

    // const int   n_params       = 0;
    // const char* param_names[4] = {"p", "t", "u", "v"};
    // int         num_nums       = 0;
    // int         num_steps      = 0;
    // int         num_levels     = 0;
    int stat = DASI_ERROR_UNKNOWN;

    char**      argp;
    // const char* arg;
    const char* kv;

    // int    number;
    // int    level;
    // int    par;
    // char   sbuf[5];
    int    count = 0;
    // double mean   = 0;
    long   length = 0;
    double value  = 0;
    char   buffer[128];

    dasi_t*          dasi;
    dasi_key_t*      key;
    dasi_query_t*    query;
    dasi_retrieve_t* ret;
    dasi_list_t*     list;

    (void)argc;

    for (argp = argv + 1; *argp != NULL; ++argp) {
        if (strcmp(*argp, "-c") == 0 || strcmp(*argp, "--config") == 0) {
            config_path = *(++argp);
            if (config_path == NULL) {
                fprintf(stderr, "Argument needed for -c/--config\n");
                return 1;
            }
        } else if (strcmp(*argp, "-q") == 0 || strcmp(*argp, "--query") == 0) {
            str_query = *(++argp);
            if (str_query == NULL) {
                fprintf(stderr, "Argument needed for -q/--query\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Usage: query [--config PATH] [--query QUERY]\n");
            return 1;
        }
    }

    if (config_path == NULL) {
        config_path = getenv("DASI_CONFIG_FILE");
        if (config_path == NULL) {
            fprintf(stderr, "No configuration file. Please set DASI_CONFIG_FILE or "
                            "pass -c/--config\n");
            return 1;
        }
    }

    ASSERT_SUCCESS(stat, dasi_open(&dasi, config_path));

    ASSERT_SUCCESS(stat, dasi_new_query_from_string(&query, str_query));

    printf("Listing ...\n");

    ASSERT_SUCCESS(stat, dasi_list(dasi, query, &list));

    assert(list != NULL);
    // ASSERT_SUCCESS(stat, dasi_list_count(list, &length));
    // if (length < 1) {
    //     fprintf(stderr, "Could not find anything for query=%s\n", str_query);
    //     ASSERT_SUCCESS(stat, dasi_free_query(query));
    //     ASSERT_SUCCESS(stat, dasi_close(dasi));
    //     return 1;
    // }

    // fprintf(stdout, "Retrieve count=%ld\n", length);

    count  = 0;
    length = 0;

    const char* uri = NULL;

    int  step      = 0;
    int  number    = 0;
    long timestamp = 0;
    long offset    = 0;
    // key used for retrieved items
    ASSERT_SUCCESS(stat, dasi_new_key(&key));
    while ((stat = dasi_list_next(list)) == DASI_SUCCESS) {
        // dasi_key_t** key,dasi_time_t *timestamp, const char **uri, long *offset, long* length
        ASSERT_SUCCESS(stat, dasi_list_attrs(list, &key, &timestamp, &uri, &offset, &length));
        assert(length > 0);
        ASSERT_SUCCESS(stat, dasi_key_get(key, "number", &kv));
        sscanf(kv, "%d", &number);
        ASSERT_SUCCESS(stat, dasi_key_get(key, "step", &kv));
        sscanf(kv, "%d", &step);
        fprintf(stdout, "List #%d - Key: number=%d, step:%d\n", count, number, step);

        fprintf(stdout, "time: %li, uri: %s, offset: %li, length: %li \n", timestamp, uri, offset, length);

        count++;
    }
    assert(stat == DASI_ITERATION_COMPLETE);

    ASSERT_SUCCESS(stat, dasi_free_list(list));

    //     printf("Querying ...\n");
    //
    //     ASSERT_SUCCESS(stat, dasi_retrieve(dasi, query, &ret));
    //
    //     assert(ret != NULL);
    //     ASSERT_SUCCESS(stat, dasi_retrieve_count(ret, &length));
    //     if (length < 1) {
    //         fprintf(stderr, "Could not find anything for query=%s\n", str_query);
    //         ASSERT_SUCCESS(stat, dasi_free_query(query));
    //         ASSERT_SUCCESS(stat, dasi_close(dasi));
    //         return 1;
    //     }
    //
    //     fprintf(stdout, "Retrieve count=%ld\n", length);
    //
    //     count  = 0;
    //     length = 0;
    //     // key used for retrieved items
    //     ASSERT_SUCCESS(stat, dasi_new_key(&key));
    //     while ((stat = dasi_retrieve_next(ret)) == DASI_SUCCESS) {
    //         ASSERT_SUCCESS(stat, dasi_retrieve_attrs(ret, &key, NULL, NULL, &length));
    //         assert(length > 0);
    //         // ASSERT_SUCCESS(stat, dasi_key_get(key, "number", &kv));
    //         // sscanf(kv, "%d", &number);
    //
    //         buffer[length] = 0;
    //         ASSERT_SUCCESS(stat, dasi_retrieve_read(ret, buffer, &length));
    //
    //         stat = sscanf(buffer, "%lg", &value);
    //         if (stat != 1) { fprintf(stderr, "Could not read data!"); }
    //
    //         count++;
    //         fprintf(stdout, "#%d Retrieved: %f\n", count, value);
    //     }
    //     assert(stat == DASI_ITERATION_COMPLETE);

    ASSERT_SUCCESS(stat, dasi_free_key(key));
    ASSERT_SUCCESS(stat, dasi_free_query(query));
    // ASSERT_SUCCESS(stat, dasi_free_retrieve(ret));
    ASSERT_SUCCESS(stat, dasi_close(dasi));

    return 0;
}
