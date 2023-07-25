#include <stdint.h>

uint16_t find_log2(uint16_t input) {
    if (input == 1) { return 0; } 
    int16_t ret = 0;   
    ret = (input && (!(input & (input - 1)))) ? -1 : 0;
    while (input >= 1){
        input >>= 1; 
        ret++;
    }
    return ret;
}