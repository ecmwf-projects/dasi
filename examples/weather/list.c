
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

    int stat = DASI_ERROR_UNKNOWN;

    char**      argp;
    const char* kv;

    int    count  = 0;
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

    ASSERT_SUCCESS(stat, dasi_free_key(key));
    ASSERT_SUCCESS(stat, dasi_free_query(query));
    ASSERT_SUCCESS(stat, dasi_close(dasi));

    return 0;
}
