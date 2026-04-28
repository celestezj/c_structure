/*
 * TAILQ 双向尾队列
 * 特点：双向链表 + 头尾指针 + 反向遍历，功能最强大
 * 支持：O(1) 头尾插删、任意节点前后插入、任意删除、取首尾、正向/反向遍历、安全遍历
 * 适用场景：内核队列、消息链表、通用业务链表、工业级首选
 * gcc demo_tailq.c -o demo
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "list/list.h"

struct node {
    int data;
    TAILQ_ENTRY(node) field;
};

TAILQ_HEAD(tailq_head, node);

int main() {
    struct tailq_head head;
    struct node *elm, *tmp;

    // 1. 初始化队列
    TAILQ_INIT(&head);
    printf("TAILQ 初始是否为空：%d\n\n", TAILQ_EMPTY(&head));

    // 2. 尾部插入（最常用）
    for (int i = 1; i <= 4; i++) {
        elm = malloc(sizeof(*elm));
        elm->data = i;
        TAILQ_INSERT_TAIL(&head, elm, field);
    }

    // 3. 头部插入
    elm = malloc(sizeof(*elm));
    elm->data = 999;
    TAILQ_INSERT_HEAD(&head, elm, field);

    // 查看当前链表
    printf("插入 999 到头后：");
    TAILQ_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n\n");

    // ======================================================================
    // 🔥 4. TAILQ_INSERT_AFTER：在指定节点【后面】插入（O(1)）
    // ======================================================================
    // 找到 data=2 的节点，在后面插入 66
    TAILQ_FOREACH(elm, &head, field) {
        if (elm->data == 2) {
            struct node *new_elm = malloc(sizeof(*new_elm));
            new_elm->data = 66;
            TAILQ_INSERT_AFTER(&head, elm, new_elm, field);
            break;
        }
    }
    printf("在 2 后面插入 66：");
    TAILQ_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // ======================================================================
    // 🔥 5. TAILQ_INSERT_BEFORE：在指定节点【前面】插入（O(1)）
    // ======================================================================
    // 找到 data=3 的节点，在前面插入 88
    TAILQ_FOREACH(elm, &head, field) {
        if (elm->data == 3) {
            struct node *new_elm = malloc(sizeof(*new_elm));
            new_elm->data = 88;
            TAILQ_INSERT_BEFORE(elm, new_elm, field);
            break;
        }
    }
    printf("在 3 前面插入 88：");
    TAILQ_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n\n");

    // 6. 查看首尾元素
    printf("TAILQ 第一个元素：%d\n", TAILQ_FIRST(&head)->data);
    printf("TAILQ 最后一个元素：%d\n\n", TAILQ_LAST(&head, tailq_head)->data);

    // 7. 正向遍历
    printf("TAILQ 正向遍历：");
    TAILQ_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // 8. 反向遍历（TAILQ 独有！）
    printf("TAILQ 反向遍历：");
    TAILQ_FOREACH_REVERSE(elm, &head, tailq_head, field) {
        printf("%d ", elm->data);
    }
    printf("\n\n");

    // ======================================================================
    // 9. 安全遍历 + 删除（遍历时删除必须用 SAFE 版本）
    // ======================================================================
    printf("TAILQ 安全遍历并删除所有节点：");
    TAILQ_FOREACH_SAFE(elm, &head, field, tmp) {
        printf("%d ", elm->data);
        TAILQ_REMOVE(&head, elm, field);
        free(elm);
    }
    printf("\n\n");

    // 10. 最终判空
    printf("TAILQ 最终是否为空：%d\n", TAILQ_EMPTY(&head));

    return 0;
}