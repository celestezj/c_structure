#include "hashtbl/hashtbl.h"

#define LOG_SIZE 12
uint32_t hash_size = 1 << LOG_SIZE;

typedef struct data_node_ {
	hashtbl_link_t hashlink;
    uint32_t value;
} data_node_t;

int hashtbl_compute_data_node_hash(data_node_t *node) {
    return hashtbl_hash_uint32(node->value, hash_size);
}

int hashtbl_compare_data_node(data_node_t *node1, data_node_t *node2) {
    if (node1->value == node2->value) {
        return 1;
    }
    return 0;
}

int hashtbl_insert_data_node(hashtbl_t *hashtbl, data_node_t *node) {
    int key = hashtbl_compute_data_node_hash(node);
    if (hashtbl_find(hashtbl, node, key, (hashtbl_comp_func)hashtbl_compare_data_node)) {
        return 0;
    }
    return hashtbl_insert(hashtbl, key, node);
}

data_node_t* hashtbl_find_data_node(hashtbl_t *hashtbl, uint32_t target_value) {
    data_node_t target_node;
    target_node.value = target_value;
    int key = hashtbl_compute_data_node_hash(&target_node);
    return hashtbl_find(hashtbl, &target_node, key, (hashtbl_comp_func)hashtbl_compare_data_node);
}

int hashtbl_del_data_node(hashtbl_t *hashtbl, data_node_t *node) {
    int key = hashtbl_compute_data_node_hash(node);
    if (!hashtbl_find(hashtbl, node, key, (hashtbl_comp_func)hashtbl_compare_data_node)) {
        return 0;
    }
    return hashtbl_remove(hashtbl, key, node);
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

int main() {
    hashtbl_t* my_hashtbl = hashtbl_init(LOG_SIZE, offsetof(struct data_node_, hashlink), 0);
    data_node_t node1 = {.value = 111};
    data_node_t node2 = {.value = 222};
    data_node_t node3 = {.value = 333};
    hashtbl_insert_data_node(my_hashtbl, &node1);
    hashtbl_insert_data_node(my_hashtbl, &node2);
    hashtbl_insert_data_node(my_hashtbl, &node3);
    hashtbl_print_all(my_hashtbl);
    hashtbl_del_data_node(my_hashtbl, &node1);
    hashtbl_del_data_node(my_hashtbl, &node2);
    hashtbl_del_data_node(my_hashtbl, &node3);
    hashtbl_print_all(my_hashtbl);
    return 0;
}