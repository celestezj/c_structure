/*
 * SLIST 单向链表
 * 特点：单向、无尾指针、仅支持头部快速操作
 * 支持：头插、头删、遍历、安全遍历、判空、取首元素
 * 不支持：尾插、尾删、反向遍历、取最后一个元素
 * 适用场景：简单单向链表、栈、内存极小的场景
 */
#include <stdio.h>
#include <stdlib.h>
#include "list/list.h"

struct node {
    int data;
    SLIST_ENTRY(node) field;
};

SLIST_HEAD(slist_head, node);

int main() {
    struct slist_head head;
    struct node *elm, *tmp;

    SLIST_INIT(&head);

    // 判空
    printf("SLIST 初始是否为空：%d\n", SLIST_EMPTY(&head));

    // 头插
    for (int i = 1; i <= 4; i++) {
        elm = malloc(sizeof(*elm));
        elm->data = i;
        SLIST_INSERT_HEAD(&head, elm, field);
    }

    // 获取第一个元素
    elm = SLIST_FIRST(&head);
    printf("SLIST 第一个元素：%d\n", elm->data);

    // 普通遍历
    printf("SLIST 遍历：");
    SLIST_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // 安全遍历 + 删除所有
    printf("SLIST 安全遍历并删除：");
    SLIST_FOREACH_SAFE(elm, &head, field, tmp) {
        printf("%d ", elm->data);
        SLIST_REMOVE(&head, elm, node, field);
        free(elm);
    }
    printf("\n");

    printf("SLIST 最终是否为空：%d\n", SLIST_EMPTY(&head));
    return 0;
}