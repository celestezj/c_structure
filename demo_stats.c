#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashtbl/hashtbl.h"
#include "hashtbl/hashtbl_stats.h"

// 测试数据结构
typedef struct test_node_ {
    hashtbl_link_t link;
    uint32_t key;
    uint32_t value;
} test_node_t;

// 哈希函数
int test_hash(test_node_t *node) {
    return hashtbl_hash_uint32(node->key, 1 << 12);
}

// 比较函数
int test_cmp(test_node_t *a, test_node_t *b) {
    return a->key == b->key ? 1 : 0;
}

// 场景1：均匀分布测试
void test_uniform_distribution(hashtbl_t *ht) {
    printf("\n\n========================================\n");
    printf("Scenario 1: Uniform Distribution\n");
    printf("========================================\n");
    
    // 插入随机数据
    srand(12345);  // 固定种子以便复现
    for (int i = 0; i < 5000; i++) {
        test_node_t *node = malloc(sizeof(test_node_t));
        node->key = rand() % 100000;
        node->value = i;
        node->link.prev = NULL;
        node->link.next = NULL;
        
        int key = test_hash(node);
        if (!hashtbl_find(ht, node, key, (hashtbl_comp_func)test_cmp)) {
            hashtbl_insert(ht, key, node);
        } else {
            free(node);
        }
    }
    
    hashtbl_print_stats_report(ht);
}

// 场景2：冲突测试 - 故意制造哈希冲突
void test_collision_scenario(hashtbl_t *ht) {
    printf("\n\n========================================\n");
    printf("Scenario 2: Collision Scenario\n");
    printf("========================================\n");
    
    // 插入大量具有相同低位的数据（导致冲突）
    for (int i = 0; i < 2000; i++) {
        test_node_t *node = malloc(sizeof(test_node_t));
        // 故意让key的低12位相同，制造冲突
        node->key = ((i / 10) << 12) | (i % 10);  // 每10个一组，组内冲突
        node->value = i;
        node->link.prev = NULL;
        node->link.next = NULL;
        
        int key = test_hash(node);
        if (!hashtbl_find(ht, node, key, (hashtbl_comp_func)test_cmp)) {
            hashtbl_insert(ht, key, node);
        } else {
            free(node);
        }
    }
    
    hashtbl_print_stats_report(ht);
}

// 场景3：稀疏分布测试
void test_sparse_distribution(hashtbl_t *ht) {
    printf("\n\n========================================\n");
    printf("Scenario 3: Sparse Distribution\n");
    printf("========================================\n");
    
    // 只插入少量数据到大量桶中
    for (int i = 0; i < 100; i++) {
        test_node_t *node = malloc(sizeof(test_node_t));
        node->key = i * 1000;  // 分散的key
        node->value = i;
        node->link.prev = NULL;
        node->link.next = NULL;
        
        int key = test_hash(node);
        if (!hashtbl_find(ht, node, key, (hashtbl_comp_func)test_cmp)) {
            hashtbl_insert(ht, key, node);
        } else {
            free(node);
        }
    }
    
    hashtbl_print_stats_report(ht);
}

// 清理哈希表
void cleanup_hashtbl(hashtbl_t *ht) {
    if (!ht) return;
    
    uint32_t bucket_count = 1 << ht->log_size;
    for (uint32_t i = 0; i < bucket_count; i++) {
        hashtbl_link_t *head = ht->tbl[i];
        while (head) {
            hashtbl_link_t *next = head->next;
            test_node_t *node = (test_node_t *)LINK2OBJ(ht, head);
            free(node);
            head = next;
        }
        ht->tbl[i] = NULL;
    }
    ht->num_items = 0;
}

int main() {
    printf("Hash Table Distribution Analysis Demo\n");
    printf("=====================================\n");
    
    // 创建哈希表
    hashtbl_t *ht = hashtbl_init(12, offsetof(test_node_t, link), 0);
    if (!ht) {
        printf("Failed to create hash table\n");
        return 1;
    }
    
    // 场景1：均匀分布
    test_uniform_distribution(ht);
    cleanup_hashtbl(ht);
    
    // 场景2：冲突场景
    test_collision_scenario(ht);
    cleanup_hashtbl(ht);
    
    // 场景3：稀疏分布
    test_sparse_distribution(ht);
    
    // 导出CSV（可选）
    // hashtbl_export_distribution_csv(ht, "distribution.csv");
    
    // 清理
    cleanup_hashtbl(ht);
    hashtbl_destroy(ht);
    
    printf("\n\nDemo completed!\n");
    return 0;
}
