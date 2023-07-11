/*
  Copyright (c) 2002, 2004, Christopher Clark
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  
  * Neither the name of the original author; nor the names of any contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.
  
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/* Modifications made by Jack Lange <jarusl@cs.northwestern.edu> 
   Further modifications by David Krasowska <krasow@u.northwestern.edu>
*/


#ifndef HMAP_H 
#define HMAP_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tpch.h"

#if DPU
#include <alloc.h>
#define malloc(s) mem_alloc(s)
#define free(p) 
#endif


typedef struct values {
    __BIGINT* ints;
    __DOUBLE* doubles;

    uint16_t int_cnt;
    uint16_t double_cnt;  
} values;

#define val_malloc(val, num_ints, num_doubles) \
    val = (values*) malloc(sizeof(values)); \
    val->ints = (__BIGINT*) calloc(sizeof(__BIGINT), num_ints); \
    val->doubles = (__DOUBLE*) calloc(sizeof(__DOUBLE), num_doubles); \
    val->int_cnt = num_ints; \
    val->double_cnt = num_doubles; \


#define val_free(val) \
    free(val->ints); \
    free(val->doubles); \
    free(val); \


#define val_i64(val, offset)  val->ints[offset]
#define val_f64(val, offset)  val->doubles[offset]


struct tpch_hashtable;

/* Example of use:
 *
 *      struct hashtable  *h;
 *      struct some_key   *k;
 *      struct some_value *v;
 *
 *      static uint32_t         hash_from_key_fn( void *k );
 *      static int              keys_equal_fn ( void *key1, void *key2 );
 *
 *      h = create_hashtable(16, hash_from_key_fn, keys_equal_fn);
 *      k = (struct some_key *)     malloc(sizeof(struct some_key));
 *      v = (struct some_value *)   malloc(sizeof(struct some_value));
 *
 *      (initialise k and v to suitable values)
 * 
 *      if (! hashtable_insert(h,k,v) )
 *      {     exit(-1);               }
 *
 *      if (NULL == (found = hashtable_search(h,k) ))
 *      {    printf("not found!");                  }
 *
 *      if (NULL == (found = hashtable_remove(h,k) ))
 *      {    printf("Not found\n");                 }
 *
 */

/* Macros may be used to define type-safe(r) hashtable access functions, with
 * methods specialized to take known key and value types as parameters.
 * 
 * Example:
 *
 * Insert this at the start of your file:
 *
 * DEFINE_HASHTABLE_INSERT(insert_some, struct some_key, struct some_value);
 * DEFINE_HASHTABLE_SEARCH(search_some, struct some_key, struct some_value);
 * DEFINE_HASHTABLE_REMOVE(remove_some, struct some_key, struct some_value);
 *
 * This defines the functions 'insert_some', 'search_some' and 'remove_some'.
 * These operate just like hashtable_insert etc., with the same parameters,
 * but their function signatures have 'struct some_key *' rather than
 * 'void *', and hence can generate compile time errors if your program is
 * supplying incorrect data as a key (and similarly for value).
 *
 * Note that the hash and key equality functions passed to create_hashtable
 * still take 'void *' parameters instead of 'some key *'. This shouldn't be
 * a difficult issue as they're only defined and passed once, and the other
 * functions will ensure that only valid keys are supplied to them.
 *
 * The cost for this checking is increased code size and runtime overhead
 * - if performance is important, it may be worth switching back to the
 * unsafe methods once your program has been debugged with the safe methods.
 * This just requires switching to some simple alternative defines - eg:
 * #define insert_some hashtable_insert
 *
 */

#define DEFINE_HASHTABLE_INSERT(fnname, keytype, valuetype)		\
    static int fnname (struct tpch_hashtable * htable, keytype key, valuetype value) { \
	return tpch_htable_insert(htable, (addr_t)key, (addr_t)value);	\
    }

#define DEFINE_HASHTABLE_SEARCH(fnname, keytype, valuetype)		\
    static valuetype * fnname (struct tpch_hashtable * htable, keytype  key) { \
	return (valuetype *) (tpch_htable_search(htable, (addr_t)key));	\
    }

#define DEFINE_HASHTABLE_REMOVE(fnname, keytype, valuetype, free_key)	\
    static valuetype * fnname (struct tpch_hashtable * htable, keytype key) { \
	return (valuetype *) (tpch_htable_remove(htable, (addr_t)key, free_key)); \
    }

uint32_t hash_fn(addr_t k); 
int int_keys_equal_fn(addr_t key1, addr_t key2); 


/* These cannot be inlined because they are referenced as fn ptrs */
uint64_t tpch_hash_long(uint64_t val, uint32_t bits);
uint64_t tpch_hash_buffer(uchar_t * msg, uint32_t length);



struct tpch_hashtable * tpch_create_htable(uint32_t min_size,
				    uint32_t (*hashfunction) (addr_t key),
				    int (*key_eq_fn) (addr_t key1, addr_t key2));

void tpch_free_htable(struct tpch_hashtable * htable, int free_values, int free_keys);

/*
 * returns non-zero for successful insertion
 *
 * This function will cause the table to expand if the insertion would take
 * the ratio of entries to table size over the maximum load factor.
 *
 * This function does not check for repeated insertions with a duplicate key.
 * The value returned when using a duplicate key is undefined -- when
 * the hashtable changes size, the order of retrieval of duplicate key
 * entries is reversed.
 * If in doubt, remove before insert.
 */
int tpch_htable_insert(struct tpch_hashtable * htable, addr_t key, addr_t value);

int tpch_htable_change(struct tpch_hashtable * htable, addr_t key, addr_t value, int free_value);


// returns the value associated with the key, or NULL if none found
addr_t tpch_htable_search(struct tpch_hashtable * htable, addr_t key);

// returns the value associated with the key, or NULL if none found
addr_t tpch_htable_remove(struct tpch_hashtable * htable, addr_t key, int free_key);

uint32_t tpch_htable_count(struct tpch_hashtable * htable);

// Specialty functions for a counting hashtable 
int tpch_htable_inc(struct tpch_hashtable * htable, addr_t key, addr_t value);
int tpch_htable_dec(struct tpch_hashtable * htable, addr_t key, addr_t value);


/* ************ */
/* ITERATOR API */
/* ************ */

#define DEFINE_HASHTABLE_ITERATOR_SEARCH(fnname, keytype)		\
   static int fnname (struct tpch_hashtable_itr * iter, struct tpch_hashtable * htable, keytype * key) { \
	return (tpch_htable_iter_search(iter, htable, key));		\
    }



/*****************************************************************************/
/* This struct is only concrete here to allow the inlining of two of the
 * accessor functions. */
struct tpch_hashtable_iter {
    struct tpch_hashtable * htable;
    struct tpch_hash_entry * entry;
    struct tpch_hash_entry * parent;
    uint32_t index;
};


struct tpch_hashtable_iter * tpch_create_htable_iter(struct tpch_hashtable * htable);
void tpch_destroy_htable_iter(struct tpch_hashtable_iter * iter);

/* - return the value of the (key,value) pair at the current position */
//extern inline 
addr_t tpch_htable_get_iter_key(struct tpch_hashtable_iter * iter);
/* {
   return iter->entry->key;
   }
*/


/* value - return the value of the (key,value) pair at the current position */
//extern inline 
addr_t tpch_htable_get_iter_value(struct tpch_hashtable_iter * iter);
/* {
   return iter->entry->value;
   }
*/



/* returns zero if advanced to end of table */
int tpch_htable_iter_advance(struct tpch_hashtable_iter * iter);

/* remove current element and advance the iterator to the next element
 *          NB: if you need the value to free it, read it before
 *          removing. ie: beware memory leaks!
 *          returns zero if advanced to end of table 
 */
int tpch_htable_iter_remove(struct tpch_hashtable_iter * iter, int free_key);


/* search - overwrite the supplied iterator, to point to the entry
 *          matching the supplied key.
 *          returns zero if not found. */
int tpch_htable_iter_search(struct tpch_hashtable_iter * iter, struct tpch_hashtable * htable, addr_t key);



#endif