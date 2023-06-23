#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tables.h"

uint32_t convert_date(char* date){
    int YYYY = 0, MM = 0, DD = 0;
    struct tm ti;
    memset(&ti, 0, sizeof(ti));
    sscanf(date, "%d-%d-%d", &YYYY, &MM, &DD);
    ti.tm_year = YYYY - 1900;
    ti.tm_mon = MM;
    ti.tm_mday = DD;
    return (uint32_t)mktime(&ti);
}