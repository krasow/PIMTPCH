#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

#include "tables.h"

uint32_t convert_date(char* date){
    int YYYY = 0, MM = 0, DD = 0;
    struct tm ti = { 0 };
    sscanf(date, "%d-%d-%d", &YYYY, &MM, &DD);
    ti.tm_year = YYYY;
    ti.tm_mon = MM;
    ti.tm_mday = DD;
    return (uint32_t)mktime(&ti);
}