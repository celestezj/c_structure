/*
 * 单向尾队列 STAILQ (Singly-Linked Tail Queue)
 * 特点：单向链表 + 尾指针，支持头部/尾部 O(1) 插入
 * 适用场景：FIFO 队列、消息队列、有序链表、简单缓存队列
 * 优点：尾插极快、结构简单、比 SLIST 更实用
 * 缺点：不支持反向遍历
 */
#include <stdio.h>
#include <stdlib.h>
// 加上这个头文件，彻底解决 offset 问题
#include <stddef.h>
#include "list/list.h"

// 定义队列节点
struct node {
    int data;
    STAILQ_ENTRY(node) field;
};

// 定义队列头
STAILQ_HEAD(stailq_head, node);

int main() {
    struct stailq_head head;
    struct node *elm, *tmp;
    int i;

    // 1. 初始化队列
    STAILQ_INIT(&head);

    // 2. 尾部插入（STAILQ 核心特性）
    for (i = 1; i <= 5; i++) {
        elm = (struct node *)malloc(sizeof(*elm));
        elm->data = i;
        STAILQ_INSERT_TAIL(&head, elm, field);
    }

    // 3. 头部插入
    elm = (struct node *)malloc(sizeof(*elm));
    elm->data = 99;
    STAILQ_INSERT_HEAD(&head, elm, field);

    // 4. 普通遍历
    printf("STAILQ 遍历: ");
    STAILQ_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // 5. 在指定元素后插入
    // 找到 data=2 的元素，在它后面插入 66
    STAILQ_FOREACH(elm, &head, field) {
        if (elm->data == 2) {
            struct node *new_elm = (struct node *)malloc(sizeof(*new_elm));
            new_elm->data = 66;
            STAILQ_INSERT_AFTER(&head, elm, new_elm, field);
            break;
        }
    }
    printf("在 2 后插入 66: ");
    STAILQ_FOREACH(elm, &head, field) {
        printf("%d ", elm->data);
    }
    printf("\n");

    // 6. SAFE 遍历 + 删除（遍历时删除必须用 SAFE 版本）
    printf("STAILQ 安全删除所有元素: ");
    STAILQ_FOREACH_SAFE(elm, &head, field, tmp) {
        printf("%d ", elm->data);
        // 使用你 queue.h 里的安全删除宏
        STAILQ_REMOVE_CHECK(&head, elm, node, field);
        free(elm);
    }
    printf("\n");

    // 7. 判断是否为空
    printf("队列是否为空: %d\n", STAILQ_EMPTY(&head));

    return 0;
}