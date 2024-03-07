
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
    const char* config_path    = NULL;
    const char type[]          = "ensemble";
    const int n_params         = 4;
    const char* param_names[4] = {"p", "t", "u", "v"};
    int num_members            = 3;
    int num_steps              = 5;
    int num_levels             = 10;
    int stat                   = DASI_ERROR_UNKNOWN;
    char version[5]            = "0001";
    char fc_date[9]            = "12345678";
    char fc_time[5]            = "0000";

    char** argp;
    const char* arg;

    dasi_t* dasi;
    dasi_key_t* key;

    int step;
    int level;
    int par;
    int number;
    char sbuf[5];

    double vals[n_params];
    const double init[] = {1010.0l, 300.0l, 5.0l, -3.0l};
    const double incr[] = {0.5l, 0.1l, -0.1l, 0.1l};

    (void)argc;

    set_time(fc_date, fc_time);

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
        else if (strcmp(*argp, "-s") == 0 || strcmp(*argp, "--steps") == 0) {
            arg = *(++argp);
            if (arg == NULL) {
                fprintf(stderr, "Argument needed for -s/--steps\n");
                return 1;
            }
            if (sscanf(arg, "%d", &num_steps) != 1 || num_steps <= 0) {
                fprintf(stderr, "Invalid value for -s/--steps\n");
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
        else {
            fprintf(stderr,
                    "Usage: forecast [--config PATH] [--version VERS] [--date "
                    "YYYYMMDD] [--time HHMM] [--members NUM] [--steps NUM] "
                    "[--levels NUM]\n");
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

    ASSERT_SUCCESS(stat, dasi_open(&dasi, config_path));
    ASSERT_SUCCESS(stat, dasi_new_key(&key));
    ASSERT_SUCCESS(stat, dasi_key_set(key, "type", type));
    ASSERT_SUCCESS(stat, dasi_key_set(key, "version", version));
    ASSERT_SUCCESS(stat, dasi_key_set(key, "date", fc_date));
    ASSERT_SUCCESS(stat, dasi_key_set(key, "time", fc_time));

    for (number = 0; number < num_members; ++number) {
        /* generate parameter values */

        for (par = 0; par < n_params; ++par) {
            vals[par] = init[par] + number * incr[par];
        }
        printf("Member %d", number);
        snprintf(sbuf, sizeof(sbuf), "%d", number);
        ASSERT_SUCCESS(stat, dasi_key_set(key, "number", sbuf));

        /* STEPS */
        for (step = 1; step <= num_steps; ++step) {
            printf("\tStep %d", step);
            snprintf(sbuf, sizeof(sbuf), "%d", step);
            ASSERT_SUCCESS(stat, dasi_key_set(key, "step", sbuf));

            /* LEVELS */
            for (level = 0; level < num_levels; ++level) {
                snprintf(sbuf, sizeof(sbuf), "%d", level);
                ASSERT_SUCCESS(stat, dasi_key_set(key, "level", sbuf));

                /* PARAMETERS */
                for (par = 0; par < n_params; ++par) {
                    char data[20];
                    ASSERT_SUCCESS(
                        stat, dasi_key_set(key, "param", param_names[par]));
                    snprintf(data, sizeof(data), "%.10lg\n",
                             vals[par] - level * incr[par]);

                    /* ARCHIVE */
                    stat = dasi_archive(dasi, key, (void*)data,
                                        (long)strlen(data));
                    if (stat != DASI_SUCCESS) {
                        fprintf(stderr,
                                "Could not write data! Level: %d Param: %s\n",
                                level, param_names[par]);
                        ASSERT_SUCCESS(stat, dasi_free_key(key));
                        ASSERT_SUCCESS(stat, dasi_close(dasi));
                        return 1;
                    }

                } /* parameters */
            }     /* levels */
            /* increment values */
            for (par = 0; par < n_params; ++par) {
                vals[par] += incr[par];
            }
        } /* steps */
        printf("\n");
    } /* numbers */

    ASSERT_SUCCESS(stat, dasi_free_key(key));
    ASSERT_SUCCESS(stat, dasi_close(dasi));

    printf("Finished successfully!\n");

    return 0;
}
