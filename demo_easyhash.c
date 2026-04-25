#include "hashtbl/easyhash.h"

#define LOG_SIZE 12
uint32_t hash_size = 1 << LOG_SIZE;

typedef struct data_node_ {
	hashtbl_link_t HASHLINK(integer); //“integer”是要创建的哈希表实例的关键标识，可以针对data_node_t创建多个哈希表实例，不同的哈希表实例可以绑定不同的hash/cmp函数
    uint32_t value;
} data_node_t;

DECLARE_HASHTBL_TEMPLATE(data_node_t); //声明一个存储data_node_t对象的哈希表模板

int hashtbl_compute_data_node_hash(data_node_t *node) {
    return hashtbl_hash_uint32(node->value, hash_size);
}

int hashtbl_compare_data_node(data_node_t *node1, data_node_t *node2) {
    if (node1->value == node2->value) {
        return 1;
    }
    return 0;
}

uint32_t iter_num = 0; //当前已迭代节点数量
uint32_t total_num = 0; //哈希表中全部节点数量

int print_data_node(data_node_t *node, void *args) {
    iter_num++;
    printf("(%u:%u)", iter_num, node->value);
    if (iter_num != total_num) { //非最后一个节点
        printf("->");
    }
    return 0;
}

void hashtbl_print_all(hashtbl_t *hashtbl) {
    iter_num = 0;
    total_num = hashtbl->num_items;
    printf("哈希表：");
    if (!total_num) {
        printf("空\n");
        return;
    }
    hashtbl_traverse_each(hashtbl, (hashtbl_traverse_func)print_data_node, NULL);
    printf("\n");
}

CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(data_node_t, integer, 
    hashtbl_compute_data_node_hash, hashtbl_compare_data_node); //基于哈希表模板创建出一个哈希表实例，并绑定了哈希表的核心参数：hash和cmp函数

int main() {
    INIT_HASHTBL_INSTANCE(data_node_t, integer, 12, 0); //初始化哈希表实例，并指定容量大小
    data_node_t node1 = {.value = 11111};
    data_node_t node2 = {.value = 22222};
    data_node_t node3 = {.value = 33333};
    HASHTBL_INSTANCE_INSERT_ITEM(data_node_t, integer, &node1);
    HASHTBL_INSTANCE_INSERT_ITEM(data_node_t, integer, &node2);
    HASHTBL_INSTANCE_INSERT_ITEM(data_node_t, integer, &node3);
    hashtbl_print_all(HASHTBL_INSTANCE(data_node_t, integer).ht);
    HASHTBL_INSTANCE_DEL_ITEM(data_node_t, integer, &node1);
    HASHTBL_INSTANCE_DEL_ITEM(data_node_t, integer, &node2);
    HASHTBL_INSTANCE_DEL_ITEM(data_node_t, integer, &node3);
    hashtbl_print_all(HASHTBL_INSTANCE(data_node_t, integer).ht);
    return 0;
}