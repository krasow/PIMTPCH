#ifndef Q6_TEST_H 
#define Q6_TEST_H

// #define DEBUG
// #define PRINT


#define PAGE_SIZE       4096
#define NUM_TUPLES      (1<<24)


#define NUM_DPUS        64
#define NUM_TASKLETS    16


#define divceil(n, m)   (((n)-1) / (m) + 1)
#define roundup(n, m)   ((n / m) * m + m)

#define BLOCK_SIZE_LOG2 4
#define BLOCK_SIZE      (1 << BLOCK_SIZE_LOG2)


#define Q6_DATE1        757404000  // "1994-01-01"
#define Q6_DATE2        788940000  // "1995-01-01"

#define Q6_DISCOUNT1    (6 - 1)   // 0.06 - 0.01
#define Q6_DISCOUNT2    (6 + 1)   // 0.06 + 0.01
#define Q6_QUANTITY     24

// size is 32 bytes
typedef struct data {
    uint32_t     l_shipdate;
    uint64_t  	 l_discount;
    uint64_t     l_quantity;
    uint64_t     l_extendedprice;
    char         junk[4];
} data;



typedef struct {
    uint32_t size;
    uint32_t transfer_size;
} dpu_arguments_t;

uint16_t find_log2(uint16_t input);
#endif