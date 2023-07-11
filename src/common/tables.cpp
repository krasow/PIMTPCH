#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <common/tables.h>
#include <common/tpch.h>

void td_elem_allocate(td_elem* elem);
void td_elem_reallocate(td_elem* elem, uint64_t tuple_cnt);
void td_elem_free(td_elem* elem);

uint32_t convert_date(char* date) {
    int YYYY = 0, MM = 0, DD = 0;
    struct tm ti;
    memset(&ti, 0, sizeof(ti));
    sscanf(date, "%d-%d-%d", &YYYY, &MM, &DD);
    ti.tm_year = YYYY - 1900;
    ti.tm_mon = MM;
    ti.tm_mday = DD;
    return (uint32_t)mktime(&ti);
}

void td_setup(table_desc* td,
    size_t bigInts,
    size_t doubles,
    size_t chars,
    size_t dates,
    size_t strings,
    size_t* string_szs){

    td->bigInts.cnt  = bigInts;
    td->doubles.cnt  = doubles;
    td->chars.cnt    = chars;
    td->dates.cnt    = dates;
    td->strings.cnt  = strings;

    td->bigInts.size = sizeof(uint64_t);
    td->doubles.size = sizeof(uint64_t);
    td->chars.size   = sizeof(uchar_t);
    td->dates.size   = sizeof(uint32_t);
    td->strings.size = 0;
    td->strings.sizes = string_szs;

    td->bigInts.items   = (addr_t*)malloc(bigInts * sizeof(addr_t*));
    td->doubles.items   = (addr_t*)malloc(doubles * sizeof(addr_t*));
    td->chars.items     = (addr_t*)malloc(chars * sizeof(addr_t*));
    td->dates.items     = (addr_t*)malloc(dates * sizeof(addr_t*));
    td->strings.items   = (addr_t*)malloc(strings * sizeof(addr_t*));
}

void td_allocate(table_desc* td) {
    td_elem_allocate(&td->bigInts);
    td_elem_allocate(&td->doubles);
    td_elem_allocate(&td->chars);
    td_elem_allocate(&td->dates);
    td_elem_allocate(&td->strings);
}


void td_elem_allocate(td_elem* elem) {
    uint32_t elemsize = 0;
    // if not string
    if (elem->size != 0) {
        elemsize = elem->size;
    }
    for (unsigned int i = 0; i < elem->cnt; i++) {
        if (elem->size == 0) {
            elemsize = elem->sizes[i];
        }
        elem->items[i] = (addr_t)malloc(elemsize * MAX_TUPLES);
        if(elem->items[i] == 0) {
            std::cerr << "Could not allocate space for tuples." << std::endl;
            exit(-1);
        }
    }
}



void td_reallocate(table_desc* td, uint64_t tuple_cnt) {
    td_elem_reallocate(&td->bigInts, tuple_cnt);
    td_elem_reallocate(&td->doubles, tuple_cnt);
    td_elem_reallocate(&td->chars,   tuple_cnt);
    td_elem_reallocate(&td->dates,   tuple_cnt);
    td_elem_reallocate(&td->strings, tuple_cnt);
}


void td_elem_reallocate(td_elem* elem, uint64_t tuple_cnt) {
    uint32_t elemsize = 0;
    // if not string
    if (elem->size != 0) {
        elemsize = elem->size;
    }
    for (unsigned int i = 0; i < elem->cnt; i++) {
        if (elem->size == 0) {
            elemsize = elem->sizes[i];
        }
        elem->items[i] = (addr_t)realloc((void*)elem->items[i], elemsize * tuple_cnt);
    }
}


void td_free(table_desc* td) {
    td_elem_free(&td->bigInts);
    td_elem_free(&td->doubles);
    td_elem_free(&td->chars);
    td_elem_free(&td->dates);
    td_elem_free(&td->strings);
}

void td_elem_free(td_elem* elem) {
    for (unsigned int i = 0; i < elem->cnt; i++) {
        free((void*)elem->items[i]);
    }
    free(elem->items);
    // if string
    if(elem->size == 0) {
        free(elem->sizes);
    }
}

