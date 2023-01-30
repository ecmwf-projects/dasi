
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dasi/api/dasi_c.h"

#define ASSERT_SUCCESS(stat, dasi_fn) \
    stat = dasi_fn;                   \
    assert(stat == DASI_SUCCESS)

int main(int argc, char** argv) {
    int stat                = DASI_ERROR_UNKNOWN;
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
    char** argp;
    for (argp = argv + 1; *argp != NULL; ++argp) {
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

    dasi_t* dasi;
    ASSERT_SUCCESS(stat, dasi_open(&dasi, config_path));

    dasi_query_t* query;
    ASSERT_SUCCESS(stat, dasi_new_query(&query));
    ASSERT_SUCCESS(stat, dasi_query_append(query, "type", in_type));
    ASSERT_SUCCESS(stat, dasi_query_append(query, "version", version));
    ASSERT_SUCCESS(stat, dasi_query_append(query, "date", fc_date));
    ASSERT_SUCCESS(stat, dasi_query_append(query, "time", fc_time));

    char sbuf[5];
    snprintf(sbuf, sizeof(sbuf), "%d", step);
    ASSERT_SUCCESS(stat, dasi_query_append(query, "step", sbuf));
    int number = 0;
    for (number = 0; number < num_members; ++number) {
        snprintf(sbuf, sizeof(sbuf), "%d", number);
        ASSERT_SUCCESS(stat, dasi_query_append(query, "number", sbuf));
    }

    int count    = 0;
    double mean  = 0;
    long length  = 0;
    double value = 0;
    char buffer[128];
    dasi_retrieve_t* ret;

    const char* kv;
    dasi_key_t* key;
    ASSERT_SUCCESS(stat, dasi_new_key(&key));

    const char* sbuf_p[] = {sbuf};
    int level            = 0;
    for (level = 0; level < num_levels; ++level) {
        snprintf(sbuf, sizeof(sbuf), "%d", level);
        ASSERT_SUCCESS(stat, dasi_query_set(query, "level", sbuf_p, 1));
        int par = 0;
        for (par = 0; par < num_params; ++par) {
            ASSERT_SUCCESS(
                stat, dasi_query_set(query, "param", &param_names[par], 1));

            if (dasi_retrieve(dasi, query, &ret) != DASI_SUCCESS) {
                fprintf(stderr, "Could not find step=%d, level=%d, param=%s\n",
                        step, level, param_names[par]);
                ASSERT_SUCCESS(stat, dasi_free_query(query));
                dasi_close(dasi);
                return 1;
            }
            assert(ret != NULL);

            count  = 0;
            mean   = 0;
            length = 0;
            do {
                ASSERT_SUCCESS(
                    stat, dasi_retrieve_attrs(ret, &key, NULL, NULL, NULL));
                ASSERT_SUCCESS(stat, dasi_key_get(key, "number", &kv));
                sscanf(kv, "number: %d", &number);

                buffer[length] = 0;
                if (sscanf(buffer, "%lg", &value) != 1) {
                    fprintf(stderr,
                            "Could not read data number=%d, step=%d, level = % "
                            "d, param = %s\n ",
                            number, step, level, param_names[par]);
                }
                if (dasi_retrieve_read(ret, buffer, &length) != DASI_SUCCESS) {
                    fprintf(stderr,
                            "Could not read data number=%d, step=%d, level = % "
                            "d, param = %s\n ",
                            number, step, level, param_names[par]);
                    ASSERT_SUCCESS(stat, dasi_free_retrieve(ret));
                    ASSERT_SUCCESS(stat, dasi_free_key(key));
                    ASSERT_SUCCESS(stat, dasi_free_query(query));
                    dasi_close(dasi);
                    return 1;
                }
                mean += value;
                count++;
            } while ((stat = dasi_retrieve_next(ret)) == DASI_SUCCESS);

            ASSERT_SUCCESS(stat, dasi_free_retrieve(ret));

            mean /= count;

            ASSERT_SUCCESS(stat, dasi_key_set(key, "type", out_type));
            ASSERT_SUCCESS(stat, dasi_key_erase(key, "number"));

            snprintf(buffer, sizeof(buffer), "%.10lg\n", mean);

            if (dasi_archive(dasi, key, (void*)buffer, strlen(buffer)) !=
                DASI_SUCCESS) {
                fprintf(stderr,
                        "Could not write data step=%d, level=%d, param=%s\n",
                        step, level, param_names[par]);
                ASSERT_SUCCESS(stat, dasi_free_key(key));
                ASSERT_SUCCESS(stat, dasi_free_query(query));
                ASSERT_SUCCESS(stat, dasi_close(dasi));
                return 1;
            }
            ASSERT_SUCCESS(stat, dasi_free_key(key));
        }
    }

    ASSERT_SUCCESS(stat, dasi_free_query(query));
    ASSERT_SUCCESS(stat, dasi_close(dasi));

    return 0;
}
