#ifndef Q6_H 
#define Q6_H

#include <stdlib.h>
#include <stdint.h>

#include "../tpch.h"
#include "../tables/lineitem.h"

// Q6 query selectivity 
#define Q6_DATE1        757404000  // "1994-01-01"
#define Q6_DATE2        788940000  // "1995-01-01"
#define Q6_DISCOUNT1    (6 - 1)   // 0.06 - 0.01
#define Q6_DISCOUNT2    (6 + 1)   // 0.06 + 0.01
#define Q6_QUANTITY     24


#endif