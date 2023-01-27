
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dasi/api/c/Dasi.h"

#define ASSERT_SUCCESS(error) assert(dasi_error_get_code(error) == DASI_SUCCESS)

int main(int argc, char** argv) {
    const char* config_path = NULL;
    char type[]             = "ensemble";
    char version[5]         = "0001";
    char fc_date[9];
    char fc_time[5];
    int num_members           = 5;
    int num_steps             = 10;
    int num_levels            = 20;
    const char* param_names[] = {"p", "t", "u", "v"};
    const int num_params      = 4;

    const double init[] = {1010.0l, 300.0l, 5.0l, -3.0l};
    const double incr[] = {0.5l, 0.1l, -0.1l, 0.1l};

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

    dasi_error_t* err = NULL;
    dasi_t* dasi = dasi_new(config_path, &err);
    ASSERT_SUCCESS(err);

    dasi_key_t* key = dasi_key_new(&err);
    ASSERT_SUCCESS(err);
    dasi_key_set(key, "type", type, &err);
    dasi_key_set(key, "version", version, &err);
    dasi_key_set(key, "date", fc_date, &err);
    dasi_key_set(key, "time", fc_time, &err);

    char sbuf[5];
    char data[20];
    double vals[num_params];
    for (int number = 0; number < num_members; ++number) {
        for (int par = 0; par < num_params; ++par) {
            vals[par] = init[par] + number * incr[par];
        }

        printf("Member %d\n", number);
        snprintf(sbuf, sizeof(sbuf), "%d", number);
        dasi_key_set(key, "number", sbuf, &err);
        for (int step = 1; step <= num_steps; ++step) {
            printf("Step %d\n", step);
            snprintf(sbuf, sizeof(sbuf), "%d", step);
            dasi_key_set(key, "step", sbuf, &err);
            for (int level = 0; level < num_levels; ++level) {
                snprintf(sbuf, sizeof(sbuf), "%d", level);
                dasi_key_set(key, "level", sbuf, &err);
                for (int par = 0; par < num_params; ++par) {
                    dasi_key_set(key, "param", param_names[par], &err);
                    snprintf(data, sizeof(data), "%.10lg\n",
                             vals[par] - level * incr[par]);

                    dasi_archive(dasi, key, (void*)data, strlen(data), &err);
                    if (dasi_error_get_code(err) != DASI_SUCCESS) {
                        fprintf(stderr,
                                "Could not write data number=%d, step=%d, "
                                "level=%d, param=%s\n",
                                number, step, level, param_names[par]);
                        dasi_key_delete(key, &err);
                        dasi_delete(dasi, &err);
                        return 1;
                    }
                }
            }

            for (int par = 0; par < num_params; ++par) {
                vals[par] += incr[par];
            }
        }
    }

    dasi_key_delete(key, &err);
    dasi_delete(dasi, &err);

    return 0;
}
