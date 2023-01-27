
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dasi/api/c/Dasi.h"

#define ASSERT_SUCCESS(error) assert(dasi_error_get_code(error) == DASI_SUCCESS)


int main(int argc, char** argv) {
    const char* config_path = NULL;
    char in_type[]          = "ensemble";
    char out_type[]         = "ens_mean";
    char version[5]         = "0001";
    char fc_date[9];
    char fc_time[5];
    int num_members           = 5;
    int step                  = 0;
    int num_levels            = 20;
    const char* param_names[] = {"p", "t", "u", "v"};
    const int num_params      = 4;

    time_t t      = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(fc_date, sizeof(fc_date), "%Y%m%d", tm);
    strftime(fc_time, sizeof(fc_time), "%H%M", tm);

    const char* arg;
    for (char** argp = argv + 1; *argp != NULL; ++argp) {
        if (strcmp(*argp, "-c") == 0 || strcmp(*argp, "--config") == 0) {
            config_path = *(++argp);
            if (config_path == NULL) {
                fprintf(stderr, "Argument needed for -c/--config\n");
                return 1;
            }
        }
        else if (strcmp(*argp, "-v") == 0 || strcmp(*argp, "--version") == 0) {
            arg = *(++argp);
            if (arg == NULL) {
                fprintf(stderr, "Argument needed for -v/--version\n");
                return 1;
            }
            strncpy(version, arg, sizeof(version) - 1);
            version[sizeof(version) - 1] = 0;
        }
        else if (strcmp(*argp, "-d") == 0 || strcmp(*argp, "--date") == 0) {
            arg = *(++argp);
            if (arg == NULL) {
                fprintf(stderr, "Argument needed for -d/--date\n");
                return 1;
            }
            strncpy(fc_date, arg, sizeof(fc_date) - 1);
            fc_date[sizeof(fc_date) - 1] = 0;
        }
        else if (strcmp(*argp, "-t") == 0 || strcmp(*argp, "--time") == 0) {
            arg = *(++argp);
            if (arg == NULL) {
                fprintf(stderr, "Argument needed for -t/--time\n");
                return 1;
            }
            strncpy(fc_time, arg, sizeof(fc_time) - 1);
            fc_time[sizeof(fc_time) - 1] = 0;
        }
        else if (strcmp(*argp, "-m") == 0 || strcmp(*argp, "--members") == 0) {
            arg = *(++argp);
            if (arg == NULL) {
                fprintf(stderr, "Argument needed for -m/--members\n");
                return 1;
            }
            if (sscanf(arg, "%d", &num_members) != 1 || num_members <= 0) {
                fprintf(stderr, "Invalid value for -m/--members\n");
                return 1;
            }
        }
        else if (strcmp(*argp, "-l") == 0 || strcmp(*argp, "--levels") == 0) {
            arg = *(++argp);
            if (arg == NULL) {
                fprintf(stderr, "Argument needed for -l/--levels\n");
                return 1;
            }
            if (sscanf(arg, "%d", &num_levels) != 1 || num_levels <= 0) {
                fprintf(stderr, "Invalid value for -l/--levels\n");
                return 1;
            }
        }
        else if (strcmp(*argp, "-s") == 0 || strcmp(*argp, "--step") == 0) {
            arg = *(++argp);
            if (arg == NULL) {
                fprintf(stderr, "Argument needed for -s/--step\n");
                return 1;
            }
            if (sscanf(arg, "%d", &step) != 1 || step <= 0) {
                fprintf(stderr, "Invalid value for -s/--step\n");
                return 1;
            }
        }
        else {
            fprintf(stderr,
                    "Usage: mean [--config PATH] [--version VERS] [--date "
                    "YYYYMMDD] [--time HHMM] [--members NUM] "
                    "[--levels NUM] [--step NUM]\n");
            return 1;
        }
    }

    if (config_path == NULL) {
        config_path = getenv("DASI_CONFIG_FILE");
        if (config_path == NULL) {
            fprintf(stderr,
                    "No configuration file. Please set DASI_CONFIG_FILE or "
                    "pass -c/--config\n");
            return 1;
        }
    }

    dasi_error_t err;
    dasi_t dasi = dasi_new(config_path, &err);
    ASSERT_SUCCESS(err);

    dasi_query_t* query = dasi_query_new(&err);
    ASSERT_SUCCESS(err);
    dasi_query_append(query, "type", in_type, &err);
    dasi_query_append(query, "version", version, &err);
    dasi_query_append(query, "date", fc_date, &err);
    dasi_query_append(query, "time", fc_time, &err);

    char sbuf[5];
    snprintf(sbuf, sizeof(sbuf), "%d", step);
    dasi_query_append(query, "step", sbuf, &err);
    ASSERT_SUCCESS(err);
    for (int number = 0; number < num_members; ++number) {
        snprintf(sbuf, sizeof(sbuf), "%d", number);
        dasi_query_append(query, "number", sbuf, &err);
        ASSERT_SUCCESS(err);
    }

    // dasi_retrieve_result_t* result;
    // dasi_retrieve_iterator_t* iterator;
    // dasi_read_handle_t* handle;

    size_t len;
    char data[20];
    double val;
    const char* kv;
    int number;
    double mean;
    int count;

    dasi_key_t* key = dasi_key_new(&err);
    ASSERT_SUCCESS(err);

    const char* sbuf_p[] = {sbuf};
    for (int level = 0; level < num_levels; ++level) {
        snprintf(sbuf, sizeof(sbuf), "%d", level);
        dasi_query_set(query, "level", sbuf_p, 1, &err);
        for (int par = 0; par < num_params; ++par) {
            dasi_query_set(query, "param", &param_names[par], 1, &err);
            ASSERT_SUCCESS(err);

            // stat = dasi_get(dasi, query, &result);
            // if (stat != DASI_SUCCESS) {
            //     fprintf(stderr, "Could not find step=%d, level=%d,
            //     param=%s\n", step, level, param_names[par]);
            //     dasi_query_destroy(query);
            //     dasi_close(dasi);
            //     return 1;
            // }
            // dasi_retrieve_result_iterate(result, &iterator);
            // mean  = 0;
            // count = 0;
            // do {
            //     dasi_retrieve_iterator_get_key(iterator, &key);
            //     dasi_key_get(key, "number", &kv);
            //     sscanf(kv, "%d", &number);
            //     dasi_retrieve_iterator_get_handle(iterator, &handle);
            //     dasi_read_handle_open(handle);
            //     dasi_read_handle_read(handle, data, sizeof(data), &len);
            //     data[len] = 0;
            //     if (sscanf(data, "%lg", &val) != 1) {
            //         fprintf(stderr, "Could not read data number=%d, step=%d,
            //         level=%d, param=%s\n", number, step, level,
            //                 param_names[par]);
            //         dasi_read_handle_close(handle);
            //         dasi_read_handle_destroy(handle);
            //         dasi_retrieve_iterator_destroy(iterator);
            //         dasi_retrieve_result_destroy(result);
            //         dasi_key_destroy(key);
            //         dasi_query_destroy(query);
            //         dasi_close(dasi);
            //         return 1;
            //     }
            //     mean += val;
            //     ++count;
            // } while ((err = dasi_retrieve_iterator_next(iterator)) ==
            // DASI_SUCCESS); dasi_retrieve_iterator_destroy(iterator);
            // dasi_retrieve_result_destroy(result);
            //
            // mean /= count;

            dasi_key_set(key, "type", out_type, &err);
            ASSERT_SUCCESS(err);
            dasi_key_erase(key, "number", &err);
            ASSERT_SUCCESS(err);
            snprintf(data, sizeof(data), "%.10lg\n", mean);
            dasi_archive(dasi, key, (void*)data, strlen(data), &err);
            if (dasi_error_get_code(err) != DASI_SUCCESS) {
                fprintf(stderr,
                        "Could not write data step=%d, level=%d, param=%s\n",
                        step, level, param_names[par]);
                dasi_key_delete(key, &err);
                ASSERT_SUCCESS(err);
                dasi_query_delete(query, &err);
                ASSERT_SUCCESS(err);
                dasi_delete(dasi, &err);
                ASSERT_SUCCESS(err);
                return 1;
            }
            dasi_key_delete(key, &err);
            ASSERT_SUCCESS(err);
        }
    }

    dasi_query_delete(query, &err);
    ASSERT_SUCCESS(err);
    dasi_delete(dasi, &err);
    ASSERT_SUCCESS(err);

    return 0;
}
