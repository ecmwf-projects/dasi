#ifndef __WEATHER_UTILS_H_
#define __WEATHER_UTILS_H_

#include <time.h>

void set_time(char* p_date, char* p_time) {
    time_t t      = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(p_date, sizeof(p_date), "%Y%m%d", tm);
    strftime(p_time, sizeof(p_time), "%H%M", tm);
    printf("Date: %s Time: %s\n", p_date, p_time);
}

#endif  // __WEATHER_UTILS_H_
