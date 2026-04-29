#ifndef __HASHTBL_WRAPPER_H__
#define __HASHTBL_WRAPPER_H__

#include "./hashtbl.h"

/* Universal HASH Table Encapsulation API
   Note: `type` struct must define `u_hashlink_##instance` field
   Author: muggledy
   Date: 2026.1.17
*/

#define HASHLINK(key) u_hashlink_##key

#define DECLARE_HASHTBL_TYPE(type)                                                                           \
	typedef struct hashtbl_##type##_s {                                                                      \
		hashtbl_t *ht;                                                                                       \
		int (*compute_hash)(type * obj);                                                                     \
		int (*compare)(type * item, type *target);                                                           \
		int (*insert)(struct hashtbl_##type##_s * ht_obj, type *obj);                                        \
		int (*del)(struct hashtbl_##type##_s * ht_obj, type *obj);                                           \
		type *(*find)(struct hashtbl_##type##_s * ht_obj, type *target);                                     \
		int (*traverse)(struct hashtbl_##type##_s * ht_obj, int (*func)(type * item, void *arg), void *arg); \
	} hashtbl_##type##_t;
#define HASHTBL_TYPE(type) hashtbl_##type##_t

#define DECLARE_HASHTBL_BASIC_FUNC(type)                                                                          \
	int hashtbl_##type##_insert(struct hashtbl_##type##_s *ht_obj, type *obj)                                     \
	{                                                                                                             \
		if (!ht_obj || !ht_obj->ht || !obj)                                                                       \
			return -1;                                                                                            \
		int key = ht_obj->compute_hash(obj);                                                                      \
		if (hashtbl_find(ht_obj->ht, (void *)obj, key, (int (*)(void *, void *))ht_obj->compare)) {               \
			return 0;                                                                                             \
		}                                                                                                         \
		return hashtbl_insert(ht_obj->ht, key, (void *)obj);                                                      \
	}                                                                                                             \
	int hashtbl_##type##_del(struct hashtbl_##type##_s *ht_obj, type *obj)                                        \
	{                                                                                                             \
		if (!ht_obj || !ht_obj->ht || !obj)                                                                       \
			return -1;                                                                                            \
		int key = ht_obj->compute_hash(obj);                                                                      \
		if (!hashtbl_find(ht_obj->ht, (void *)obj, key, (int (*)(void *, void *))ht_obj->compare)) {              \
			return 0;                                                                                             \
		}                                                                                                         \
		return hashtbl_remove(ht_obj->ht, key, (void *)obj);                                                      \
	}                                                                                                             \
	type *hashtbl_##type##_find(struct hashtbl_##type##_s *ht_obj, type *target)                                  \
	{                                                                                                             \
		if (!ht_obj || !ht_obj->ht || !target)                                                                    \
			return NULL;                                                                                          \
		int key = ht_obj->compute_hash(target);                                                                   \
		return (type *)hashtbl_find(ht_obj->ht, (void *)target, key, (int (*)(void *, void *))ht_obj->compare);   \
	}                                                                                                             \
	int hashtbl_##type##_traverse_wrapper(struct hashtbl_##type##_s *ht_obj, int (*func)(type * item, void *arg), \
										  void *arg)                                                              \
	{                                                                                                             \
		return hashtbl_traverse_each(ht_obj->ht, (int (*)(void *, void *))func, arg);                             \
	}

#define DECLARE_HASHTBL_HASH_CMP_FUNC_FOR_INSTANCE(type, instance, hash_func, cmp_func) \
	int hashtbl_##type##_##instance##_compute_hash_wrapper(type *obj)                   \
	{                                                                                   \
		return hash_func(obj);                                                          \
	}                                                                                   \
	int hashtbl_##type##_##instance##_compare_wrapper(type *item, type *target)         \
	{                                                                                   \
		return cmp_func(item, target);                                                  \
	}

#define DECLARE_HASHTBL_TEMPLATE(type) \
    DECLARE_HASHTBL_TYPE(type); \
    DECLARE_HASHTBL_BASIC_FUNC(type)

#define HASHTBL_INSTANCE(type, instance) hashtbl_##type##_##instance
#define CREATE_HASHTBL_INSTANCE(type, instance)                                                                        \
	HASHTBL_TYPE(type)                                                                                                 \
	HASHTBL_INSTANCE(type, instance) = {                                                                               \
		.ht = NULL, .compute_hash = NULL, .compare = NULL, .insert = NULL, .del = NULL, .find = NULL, .traverse = NULL \
	}
#define EXTERN_HASHTBL_INSTANCE(type, instance) extern HASHTBL_TYPE(type) HASHTBL_INSTANCE(type, instance)

#define CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(type, instance, hash_func, cmp_func) \
    DECLARE_HASHTBL_HASH_CMP_FUNC_FOR_INSTANCE(type, instance, hash_func, cmp_func); \
    CREATE_HASHTBL_INSTANCE(type, instance)

#define INIT_HASHTBL_INSTANCE(type, instance, log_size, auto_grow)                                                  \
	HASHTBL_INSTANCE(type, instance).ht = hashtbl_init(log_size, offsetof(type, u_hashlink_##instance), auto_grow); \
	HASHTBL_INSTANCE(type, instance).compute_hash = hashtbl_##type##_##instance##_compute_hash_wrapper;             \
	HASHTBL_INSTANCE(type, instance).compare = hashtbl_##type##_##instance##_compare_wrapper;                       \
	HASHTBL_INSTANCE(type, instance).insert = hashtbl_##type##_insert;                                              \
	HASHTBL_INSTANCE(type, instance).del = hashtbl_##type##_del;                                                    \
	HASHTBL_INSTANCE(type, instance).find = hashtbl_##type##_find;                                                  \
	HASHTBL_INSTANCE(type, instance).traverse = hashtbl_##type##_traverse_wrapper;
#define INVALID_HASHTBL_INSTANCE(type, instance) HASHTBL_INSTANCE(type, instance).ht = NULL

#define CHANGE_HASHTBL_INSTANCE_HASH_CMP_FUNC(type, instance, hash_func, cmp_func) \
	HASHTBL_INSTANCE(type, instance).compute_hash = hash_func;                     \
	HASHTBL_INSTANCE(type, instance).compare = cmp_func;
#define RESTORE_HASHTBL_INSTANCE_HASH_CMP_FUNC(type, instance)                                          \
	HASHTBL_INSTANCE(type, instance).compute_hash = hashtbl_##type##_##instance##_compute_hash_wrapper; \
	HASHTBL_INSTANCE(type, instance).compare = hashtbl_##type##_##instance##_compare_wrapper;

#define HASHTBL_INSTANCE_PTR(type, instance) (&HASHTBL_INSTANCE(type, instance))
#define HASHTBL_INSTANCE_SIZE(type, instance) \
	(HASHTBL_INSTANCE(type, instance).ht ? (HASHTBL_INSTANCE(type, instance).ht->num_items) : (-1))

#define HASHTBL_INSTANCE_INSERT_ITEM(type, instance, obj)                                            \
	do {                                                                                             \
		if (HASHTBL_INSTANCE(type, instance).ht && obj) {                                            \
			HASHTBL_INSTANCE_PTR(type, instance)->insert(HASHTBL_INSTANCE_PTR(type, instance), obj); \
		}                                                                                            \
	} while (0)
#define HASHTBL_INSTANCE_DEL_ITEM(type, instance, obj)                                            \
	do {                                                                                          \
		if (HASHTBL_INSTANCE(type, instance).ht && obj) {                                         \
			HASHTBL_INSTANCE_PTR(type, instance)->del(HASHTBL_INSTANCE_PTR(type, instance), obj); \
		}                                                                                         \
	} while (0)
#define HASHTBL_INSTANCE_FIND_ITEM(type, instance, obj)                                            \
	((HASHTBL_INSTANCE(type, instance).ht && obj) ?                                                \
		 (HASHTBL_INSTANCE_PTR(type, instance)->find(HASHTBL_INSTANCE_PTR(type, instance), obj)) : \
		 NULL)
#define HASHTBL_INSTANCE_PRINT_ALL_ITEM(type, instance, print_func, print_args, context) \
    hashtbl_print_all(HASHTBL_INSTANCE(type, instance).ht, print_func, print_args, context)

#endif