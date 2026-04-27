#include "hashtbl.h"

uint32_t hashtbl_get_log2(uint32_t val)
{
	uint32_t size = 0;

	do {
		size++;
	} while (val >>= 1);

	if (size > 5 && size <= 10) {
		size -= 1;
	} else if (size > 10 && size <= 15) {
		size -= 2;
	} else if (size > 15 && size <= 20) {
		size -= 3;
	} else if (size > 20 && size <= 25) {
		size -= 5;
	} else if (size > 25) {
		assert(0);
	}

	return size;
}

#ifndef get16bits
#define get16bits(d) ((((const uint8_t *)(d))[1] << UINT32_C(8)) + ((const uint8_t *)(d))[0])
#endif

uint32_t hashtbl_hash_string(const char *data, int len)
{
	uint32_t hash = len, tmp;
	int rem;

	if (len <= 0 || data == NULL)
		return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--) {
		hash += get16bits(data);
		tmp = (get16bits(data + 2) << 11) ^ hash;
		hash = (hash << 16) ^ tmp;
		data += 2 * sizeof(uint16_t);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
	case 3:
		hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= data[sizeof(uint16_t)] << 18;
		hash += hash >> 11;
		break;
	case 2:
		hash += get16bits(data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

hashtbl_t *hashtbl_init(int log_size, int obj_offset, int auto_grow)
{
	hashtbl_t *hashtbl;

	if (log_size < 0 || log_size > 20) {
		printf("log size of the table must be between 1 and 2^20\n");
		return NULL;
	}

	if (obj_offset < 0 || obj_offset > 65535) {
		printf("offset of hash table link must be between 0 and 65535\n");
		return NULL;
	}

	hashtbl = (hashtbl_t *)malloc(sizeof(hashtbl_t));
	if (!hashtbl) {
		printf("memory allocation error, size=%lu\r\n", sizeof(hashtbl_t));
		return NULL;
	}

	hashtbl->tbl = (hashtbl_link_t **)malloc(sizeof(hashtbl_link_t *) * (1 << log_size));
	if (!hashtbl->tbl) {
		printf("memory allocation error,tbl size=%lu,log_size=%d\n", sizeof(hashtbl_link_t *), log_size);
		free(hashtbl);
		return NULL;
	}

	memset(hashtbl->tbl, 0, sizeof(hashtbl_link_t *) * (1 << log_size));
	hashtbl->log_size = log_size;
	hashtbl->auto_grow = (auto_grow) ? 1 : 0;
	hashtbl->obj_offset = obj_offset;
	hashtbl->num_items = 0;

	return hashtbl;
}

int hashtbl_insert(hashtbl_t *hashtbl, int key, void *obj)
{
	int idx = key & ((1 << hashtbl->log_size) - 1);
	hashtbl_link_t *head = hashtbl->tbl[idx];
	hashtbl_link_t *link = OBJ2LINK(hashtbl, obj);

	assert(link->prev == NULL);
	assert(link->next == NULL);

	link->next = head;
	if (head)
		head->prev = link;
	link->prev = NULL;
	link->hash_value = key;
	hashtbl->tbl[idx] = link;
	hashtbl->num_items++;

	return 0;
}

int hashtbl_remove(hashtbl_t *hashtbl, int key, void *obj)
{
	int idx = key & ((1 << hashtbl->log_size) - 1);
	hashtbl_link_t *head = hashtbl->tbl[idx];
	hashtbl_link_t *link = OBJ2LINK(hashtbl, obj);
	hashtbl_link_t *iter;

	/*Check if it's in hash table*/
	for (iter = head; iter; iter = iter->next) {
		if (iter == link)
			break;
	}
	if (iter == NULL)
		return 0;

	if (link->next) {
		link->next->prev = link->prev;
	}
	if (link->prev) {
		link->prev->next = link->next;
	}
	if (link == head) {
		hashtbl->tbl[idx] = link->next;
	}

	link->next = link->prev = NULL;
	hashtbl->num_items--;
	return 0;
}

void *hashtbl_find(hashtbl_t *hashtbl, void *target_obj, int key, hashtbl_comp_func func)
{
	int idx = key & ((1 << hashtbl->log_size) - 1);
	hashtbl_link_t *head = hashtbl->tbl[idx];
	hashtbl_link_t *iter;

	for (iter = head; iter; iter = iter->next) {
		if (iter->hash_value != key)
			continue;
		if ((func)(LINK2OBJ(hashtbl, iter), target_obj))
			return LINK2OBJ(hashtbl, iter);
	}
	return NULL;
}

void *hashtbl_find2(hashtbl_t *hashtbl, void *target_obj, int key, hashtbl_comp_func2 func, void *arg)
{
	int idx = key & ((1 << hashtbl->log_size) - 1);
	hashtbl_link_t *head = hashtbl->tbl[idx];
	hashtbl_link_t *iter;

	for (iter = head; iter; iter = iter->next) {
		if (iter->hash_value != key)
			continue;
		if ((func)(LINK2OBJ(hashtbl, iter), target_obj, arg) == 0)
			return LINK2OBJ(hashtbl, iter);
	}
	return NULL;
}

int hashtbl_grow(hashtbl_t *tbl)
{
	return 0;
}

int hashtbl_free_all_objects(hashtbl_t *hashtbl, hashtbl_traverse_func func, void *args)
{
	if (NULL == hashtbl || (hashtbl->num_items == 0)) {
		return 0;
	}

	hashtbl_link_t *head = NULL;
	hashtbl_link_t *next = NULL;
	uint32_t bucket_num = (1 << hashtbl->log_size);
	uint32_t i = 0;

	for (i = 0; i < bucket_num; i++) {
		head = hashtbl->tbl[i];
		if (NULL == head) {
			continue;
		}

		while (head) {
			next = head->next;
			if (func) {
				(func)(LINK2OBJ(hashtbl, head), args);
			}
			head = next;
		}
		hashtbl->tbl[i] = NULL;
	}

	hashtbl->num_items = 0;

	return 0;
}

int hashtbl_destroy(hashtbl_t *hashtbl)
{
	free(hashtbl->tbl);
	free(hashtbl);
	return 0;
}

int hashtbl_reset(hashtbl_t *hashtbl)
{
	memset(hashtbl->tbl, 0, sizeof(hashtbl_link_t *) * (1 << hashtbl->log_size));
	hashtbl->num_items = 0;
	return 0;
}

int hashtbl_traverse_each(hashtbl_t *hashtbl, hashtbl_traverse_func func, void *args)
{
	if (NULL == hashtbl || NULL == func || (hashtbl->num_items == 0)) {
		return 0;
	}

	hashtbl_link_t *head = NULL;
	hashtbl_link_t *iter = NULL;
	uint32_t bucket_num = (1 << hashtbl->log_size);
	uint32_t i = 0;

	for (i = 0; i < bucket_num; i++) {
		head = hashtbl->tbl[i];
		if (NULL == head) {
			continue;
		}

		for (iter = head; iter; iter = iter->next) {
			if (func) {
				if ((func)(LINK2OBJ(hashtbl, iter), args))
					return -1;
			}
		}
	}

	return 0;
}

uint32_t hashtbl_hash_uint32(uint32_t a, int32_t hash_size)
{
	a = (a + 0x7ed55d16) + (a << 12);
	a = (a ^ 0xc761c23c) ^ (a >> 19);
	a = (a + 0x165667b1) + (a << 5);
	a = (a + 0xd3a2646c) ^ (a << 9);
	a = (a + 0xfd7046c5) + (a << 3);
	a = (a ^ 0xb55a4f09) ^ (a >> 16);

	return a % hash_size;
}

#ifndef get16bits
#define get16bits(data) \
	((((uint32_t)(((const uint8_t *)(data))[1])) << 8) + (uint32_t)(((const uint8_t *)(data))[0]))
#endif

uint32_t hashtbl_hash_data(const uint8_t *data, int32_t len, int32_t hash_size)
{
	uint32_t hash = len;
	uint32_t tmp;
	int32_t rem;

	if (len <= 0 || data == NULL) {
		return 0;
	}

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--) {
		hash += get16bits(data);
		tmp = (get16bits(data + 2) << 11) ^ hash;
		hash = (hash << 16) ^ tmp;
		data += 2 * sizeof(uint16_t);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
	case 3:
		hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= data[sizeof(uint16_t)] << 18;
		hash += hash >> 11;
		break;
	case 2:
		hash += get16bits(data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash % hash_size;
}

int hashtbl_traverse_each_safe(hashtbl_t *hashtbl, hashtbl_traverse_func func, void *args)
{
	if (NULL == hashtbl || NULL == func || (hashtbl->num_items == 0)) {
		return 0;
	}

	hashtbl_link_t *head = NULL;
	hashtbl_link_t *next = NULL;
	uint32_t bucket_num = (1 << hashtbl->log_size);
	uint32_t i = 0;

	for (i = 0; i < bucket_num; i++) {
		head = hashtbl->tbl[i];
		if (NULL == head) {
			continue;
		}

		while (head) {
			next = head->next;
			if (func) {
				(func)(LINK2OBJ(hashtbl, head), args);
			}
			head = next;
		}
	}

	return 0;
}

int hashtbl_print_one(void *item, void *args) {
	if (!item) {
		return -1;
	}
    hashtbl_printer_context_t *context = (hashtbl_printer_context_t *)args;
    context->iter_num++;
    printf("(%u:%s)", context->iter_num, context->print_one_func(item, context->args));
    if (context->iter_num != context->total_num) { //非最后一个节点
        printf("->");
    }
    return 0;
}

void hashtbl_print_all(hashtbl_t *hashtbl, hashtbl_traverse_print_func print_one_func, void *args, 
                       hashtbl_printer_context_t *context) {
	if (!hashtbl || !print_one_func || !context) {
		return;
	}
    context->iter_num = 0;
    context->total_num = hashtbl->num_items;
    context->print_one_func = print_one_func;
    context->args = args;
    printf("哈希表：");
    if (!(context->total_num)) {
        printf("空\n");
        return;
    }
    hashtbl_traverse_each(hashtbl, (hashtbl_traverse_func)hashtbl_print_one, context);
    printf("\n");
}