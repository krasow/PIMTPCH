#ifndef TABLES_H 
#define TABLES_H

#include "tpch.h"


#define COLUMN_BUFFER 128
#define ROW_BUFFER 1000

#define BIGINT_MEMSET(table, id)    (__BIGINT*) (table)->td.bigInts.items[id]
#define DOUBLE_MEMSET(table, id)    (__DOUBLE*) (table)->td.doubles.items[id]
#define CHAR_MEMSET(table, id)      (__CHAR*)   (table)->td.chars.items[id]
#define DATE_MEMSET(table, id)      (__DATE*)   (table)->td.dates.items[id]
#define STRING_MEMSET(table, id)    (__DBSTRING*)(table)->td.strings.items[id]

#define BIGINT_SET(val)             (__BIGINT)(atoi(val))
#define DOUBLE_SET(val, scale)      (__DOUBLE)ceil(strtod(val, &val + COLUMN_BUFFER - 1) * scale)
#define CHAR_SET(val)               val[0]
#define DATE_SET(val)               convert_date(val)

#define STRING_SIZE_SET(variable, count, sizes_list)            \
    size_t *variable = (size_t*)malloc(sizeof(size_t) * count); \
    size_t string_szs_set[count] = sizes_list;                  \
    memcpy(string_szs, string_szs_set, sizeof(size_t) * count)  \

#define DEFINE_DATABASE_RETRIEVE(table, file)                                      \
    void retrieve(table** tuples) {                                                \
        std::string db = DATABASE;                                                 \
        db += '/'; db += file;                                                     \
        std::string db_bin = db + ".bin";                                          \
        allocate(tuples);                                                          \
        /* check if there has been a binary file already produced from .tbl */     \
        /* only passes if table was loaded to the system before */                 \
        if(database_read_binary(tuples, db_bin)) {return;}                         \
        /* read DATABASE/table.tbl defined in common/tpch.h */                     \
        database_read(tuples, db);                                                 \
        /* write the table to a binary file to avoid conversions for reruns */     \
        database_write_binary(tuples, db_bin);                                     \
    }


#define DEFINE_DATABASE_READ(table, cols)                                          \
    void database_read(table** tuples, std::string db) {                           \
        std::cout << db << std::endl;                                              \
        if (access(db.c_str(), F_OK)) {                                            \
            std::cerr << "Error: no .tbl database folder found." << std::endl;     \
            exit(12);                                                              \
        }                                                                          \
        /* hold an entire row from fgets */                                        \
        char buffer[ROW_BUFFER];                                                   \
        /* hold an entire row separated by pipe delimiter & heap allocation */     \
        char **elems = (char**)malloc(cols * sizeof(char*));                       \
        for (uint16_t i = 0; i < cols; i++) {                                      \
            elems[i] = (char*)malloc(COLUMN_BUFFER);                               \
        }                                                                          \
        uint64_t curr_tuple = 0;                                                   \
        FILE* f = fopen(db.c_str(), "rb");                                         \
        /* goes thru all the tuples */                                             \
        while (fgets(buffer, ROW_BUFFER, f) && curr_tuple < MAX_TUPLES) {          \
            uint16_t curr_elem = 0;                                                \
            int bytes = 0;                                                         \
            /* store one column from the buffer ("STORE THIS"|"NOT THIS"|) */      \
            char tmp[COLUMN_BUFFER];                                               \
            /* used to increment the buffer */                                     \
            char* loc_buffer = buffer;                                             \
            /* for each element in each tuple */                                   \
            while (curr_elem < cols &&                                             \
                sscanf(loc_buffer, "%[^|]|%n", tmp, &bytes) == 1)                  \
            {                                                                      \
                tmp[bytes] = '\0';                                                 \
                strncpy(elems[curr_elem], tmp, bytes);                             \
                curr_elem++;                                                       \
                loc_buffer += bytes;                                               \
                memset(tmp, 0, COLUMN_BUFFER);                                     \
            }                                                                      \
            /* fill elements into tuples data struct */                            \
            fill(*tuples, elems, curr_tuple);                                      \
            curr_tuple++;                                                          \
        }                                                                          \
        for (uint16_t i = 0; i < cols; i++) {                                      \
            free(elems[i]);                                                        \
        }                                                                          \
        free(elems);                                                               \
        fclose(f);                                                                 \
        (*tuples)->elements = curr_tuple;                                          \
        /* reallocate based on elements read */                                    \
        td_reallocate(&(*tuples)->td, (*tuples)->elements);                        \
    }                                                                                                       


typedef struct td_elem {
    addr_t* items;
    uint16_t cnt;
    size_t   size;  // used if fixed size
    size_t* sizes; // used if non-fixed size
} td_elem;

typedef struct table_desc {
    td_elem doubles;
    td_elem bigInts;
    td_elem dates;
    td_elem chars;
    td_elem strings;
} table_desc;


void td_setup(table_desc* td,
    size_t bigInts,
    size_t doubles,
    size_t chars,
    size_t dates,
    size_t strings,
    size_t* string_szs);
void td_allocate(table_desc* td);
void td_reallocate(table_desc* td, uint64_t tuple_cnt);
void td_free(table_desc* td);

uint32_t convert_date(char* date);

#endif