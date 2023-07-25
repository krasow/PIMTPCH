#ifndef Q3_H 
#define Q3_H

#include "tpch.h"
#include "common/lineitem.h"
#include "common/orders.h"
#include "common/customer.h"
#include "common/hmap.h"
#include "common/tables.h"

#define Q3_DATE1    912470400 //1998-12-01
#define Q3_SEGMENT  "BUILDING"

// if using CPU
#if (!DPU) 
__DOUBLE* q3(const lineitem* l_tups, const orders* o_tups, const customer* c_tups);
__DOUBLE* q3_naive(const lineitem* l_tups, const orders* o_tups, const customer* c_tups);
#endif



#endif