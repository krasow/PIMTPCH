#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <iostream>

#include <common/timer.h>
#include <common/tables.h>

void td_elem_allocate(td_elem* elem);
void td_elem_reallocate(td_elem* elem, uint64_t tuple_cnt);
void td_elem_free(td_elem* elem);

uint32_t convert_date(char* date) {
    struct tm ti;
    memset(&ti, 0, sizeof(ti));
    strptime(date, "%Y-%m-%d", &ti);

    uint32_t ret = (int)mktime(&ti);
    return (uint32_t)ret;
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

    td->bigInts.size = sizeof(__BIGINT);
    td->doubles.size = sizeof(__DOUBLE);
    td->chars.size   = sizeof(__CHAR);
    td->dates.size   = sizeof(__DATE);
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
            std::cerr << "td_elem_allocate:: Could not allocate space for tuples." << std::endl;
            std::cerr << "num_tuples:: " << MAX_TUPLES << " elem_size:: " << elemsize << std::endl;
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
        if(elem->items[i] == 0) {
            std::cerr << "td_elem_reallocate:: Could not allocate space for tuples." << std::endl;
            std::cerr << "num_tuples:: " << tuple_cnt << " elem_size:: " << elemsize << std::endl;
            exit(-1);
        }
    }
}
void td_free(table_desc* td) {
    td_elem_free(&td->bigInts);
    td_elem_free(&td->doubles);
    td_elem_free(&td->chars);
    td_elem_free(&td->dates);
    td_elem_free(&td->strings);
    free(td);
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
