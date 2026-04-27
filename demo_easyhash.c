#include "hashtbl/easyhash.h"
#include "hashtbl/hashtbl_stats.h" //gcc demo_easyhash.c hashtbl/hashtbl.c hashtbl/hashtbl_stats.c -o demo_stats -lm

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

char* print_data_node(void *_node, void *_args) {
    static char buffer[8];
    data_node_t *node = (data_node_t*)_node;
    snprintf(buffer, sizeof(buffer), "%u", node->value);
    return buffer;
}

CREATE_HASHTBL_INSTANCE_WITH_HASH_CMP(data_node_t, integer, 
    hashtbl_compute_data_node_hash, hashtbl_compare_data_node); //基于哈希表模板创建出一个哈希表实例，并绑定了哈希表的核心参数：hash和cmp函数

int main() {
    INIT_HASHTBL_INSTANCE(data_node_t, integer, 12, 0); //初始化哈希表实例，并指定容量大小
    data_node_t node1 = {.value = 111};
    data_node_t node2 = {.value = 222};
    data_node_t node3 = {.value = 333};
    hashtbl_printer_context_t context;
    HASHTBL_INSTANCE_INSERT_ITEM(data_node_t, integer, &node1);
    HASHTBL_INSTANCE_INSERT_ITEM(data_node_t, integer, &node2);
    HASHTBL_INSTANCE_INSERT_ITEM(data_node_t, integer, &node3);
    HASHTBL_INSTANCE_PRINT_ALL_ITEM(data_node_t, integer, print_data_node, NULL, &context);
    hashtbl_print_stats_report(HASHTBL_INSTANCE(data_node_t, integer).ht);
    // hashtbl_export_distribution_csv(HASHTBL_INSTANCE(data_node_t, integer).ht, "distribution.csv");
    HASHTBL_INSTANCE_DEL_ITEM(data_node_t, integer, &node1);
    HASHTBL_INSTANCE_DEL_ITEM(data_node_t, integer, &node2);
    HASHTBL_INSTANCE_DEL_ITEM(data_node_t, integer, &node3);
    HASHTBL_INSTANCE_PRINT_ALL_ITEM(data_node_t, integer, print_data_node, NULL, &context);
    return 0;
}