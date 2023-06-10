#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint16_t find_log2(uint16_t input) {
    if (input == 1) { return 0; } 
    uint16_t ret = 0;   
    while (input > 1){
        input >>= 1; 
        ret++;
    }
    return ret;
}