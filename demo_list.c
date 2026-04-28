/*
 * LIST 双向链表
 * 特点：双向、无尾指针、O(1) 任意节点删除
 * 支持：双向插入、任意删除、判空、取首元素
 * 不支持：尾插优化、取最后一个元素、反向遍历
 * 适用场景：需要快速任意删除、但不需要尾操作
 */
#include <stdio.h>
#include <stdlib.h>
#include "list/list.h"

struct node {
    int data;
    LIST_ENTRY(node) field;
};

LIST_HEAD(list_head, node);

int main() {
    struct list_head head;
    struct node *elm, *tmp;

    LIST_INIT(&head);

    // 头插 1 2 3 4
    for (int i = 1; i <= 4; i++) {
        elm = malloc(sizeof(*elm));
        elm->data = i;
        LIST_INSERT_HEAD(&head, elm, field);
    }

    printf("原始链表：");
    LIST_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // ==============================================
    // 🔥 找到元素 2，执行 INSERT AFTER（在后面插入 99）
    // ==============================================
    LIST_FOREACH(elm, &head, field) {
        if (elm->data == 2) {
            struct node *new_elm = malloc(sizeof(*new_elm));
            new_elm->data = 99;
            LIST_INSERT_AFTER(elm, new_elm, field);
            break;
        }
    }
    printf("在 2 后插入 99：");
    LIST_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // ==============================================
    // 🔥 找到元素 3，执行 INSERT BEFORE（在前面插入 88）
    // ==============================================
    LIST_FOREACH(elm, &head, field) {
        if (elm->data == 3) {
            struct node *new_elm = malloc(sizeof(*new_elm));
            new_elm->data = 88;
            LIST_INSERT_BEFORE(elm, new_elm, field);
            break;
        }
    }
    printf("在 3 前插入 88：");
    LIST_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // 安全遍历删除
    printf("遍历并删除：");
    LIST_FOREACH_SAFE(elm, &head, field, tmp) {
        printf("%d ", elm->data);
        LIST_REMOVE(elm, field);
        free(elm);
    }
    printf("\n");

    printf("LIST 最终是否为空：%d\n", LIST_EMPTY(&head));
    return 0;
}