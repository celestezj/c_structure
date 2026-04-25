#ifndef __HASH_TBL_H__
#define __HASH_TBL_H__

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct hashtbl_link_ {
	struct hashtbl_link_ *prev;
	struct hashtbl_link_ *next;
	int		hash_value;
} hashtbl_link_t;

#define OBJ2LINK(hashtbl, obj)	((hashtbl_link_t *)(((char *)obj) + hashtbl->obj_offset))
#define LINK2OBJ(hashtbl, link) ((void *)(((char *)link) - hashtbl->obj_offset))
#ifndef get16bits
#define get16bits(d) ((((const uint8_t *)(d))[1] << UINT32_C(8))\
                      +((const uint8_t *)(d))[0])
#endif

#define HASHTBL_SIZE(hashtbl)    (1 << (hashtbl)->log_size)

typedef struct hashtbl_ {
	unsigned char log_size;
	unsigned char auto_grow;
	unsigned short obj_offset;
	int num_items;
	hashtbl_link_t **tbl;
} hashtbl_t;

typedef int (*hashtbl_comp_func)(void *item, void *target_obj);
typedef int (*hashtbl_comp_func2)(void *item, void *target_obj, void *arg);
typedef int (*hashtbl_traverse_func)(void *item, void *arg);
typedef int (*hashtbl_traverse_func2)(void *item);

extern uint32_t hashtbl_hash_uint32(uint32_t a, int32_t hash_size);
extern uint32_t hashtbl_hash_data(const uint8_t *data, int32_t len, int32_t hash_size);
extern unsigned int hashtbl_get_log2(unsigned int val);
extern unsigned int hashtbl_hash_string(const char *data, int len);
extern hashtbl_t* hashtbl_init(int log_size, int obj_offset, int auto_grow);
extern int hashtbl_insert(hashtbl_t *tbl, int key, void *obj);
extern int hashtbl_remove(hashtbl_t *tbl, int key, void *obj);
extern void *hashtbl_find(hashtbl_t *hashtbl, void *target_obj, int key, hashtbl_comp_func func);
extern void *hashtbl_find2(hashtbl_t *hashtbl, void *target_obj, int key, hashtbl_comp_func2 func, void* arg);
extern int hashtbl_grow(hashtbl_t *tbl);
extern int hashtbl_destroy(hashtbl_t *tbl);
extern int hashtbl_reset(hashtbl_t *tbl);
extern int hashtbl_traverse_each(hashtbl_t *hashtbl, hashtbl_traverse_func func, void *arg);
extern int hashtbl_free_all_objects(hashtbl_t *hashtbl, hashtbl_traverse_func func, void *args); 
extern int hashtbl_traverse_each_safe(hashtbl_t *hashtbl, hashtbl_traverse_func func, void *args);

#endif