/*
 * non_intrusive_queue.h 是基于 list.h 实现的 非侵入式泛型队列
 * 功能：FIFO 队列，支持 push / pop
 * 底层：使用 list.h 中的 TAILQ（双向尾队列，O(1) 头尾操作）
 * 特点：
 *    1. 泛型设计，可存储任意类型数据（指针）
 *    2. 队列节点独立，不侵入业务结构体
 */

#include <stdio.h>
#include <stdlib.h>
#include "list/non_intrusive_queue.h"

struct student {
    int id;
    char name[16];
};

int main()
{
    // ==========================
    // int 必须 malloc 存指针
    // ==========================
    printf("=== int ===\n");
    queue_t q1 = queue_new();

    int *a = malloc(sizeof(int)); *a = 100; queue_push(q1, a);
    int *b = malloc(sizeof(int)); *b = 200; queue_push(q1, b);
    int *c = malloc(sizeof(int)); *c = 300; queue_push(q1, c);

    void *v;
    while (queue_pop(q1, &v) == 0) {
        int *p = (int*)v;
        printf("pop: %d\n", *p);
        free(p);
    }
    queue_free(q1);

    // ==========================
    // 字符串（正常）
    // ==========================
    printf("\n=== string ===\n");
    queue_t q2 = queue_new();
    queue_push(q2, "hello");
    queue_push(q2, "world");
    queue_push(q2, "generic queue");

    char *s;
    while (queue_pop(q2, (void**)&s) == 0) {
        printf("pop: %s\n", s);
    }
    queue_free(q2);

    // ==========================
    // 结构体（正常）
    // ==========================
    printf("\n=== struct ===\n");
    queue_t q3 = queue_new();

    struct student *st1 = malloc(sizeof(*st1));
    st1->id = 101;
    snprintf(st1->name, 16, "Tom");

    struct student *st2 = malloc(sizeof(*st2));
    st2->id = 102;
    snprintf(st2->name, 16, "Jerry");

    queue_push(q3, st1);
    queue_push(q3, st2);

    struct student *pst;
    while (queue_pop(q3, (void**)&pst) == 0) {
        printf("pop: id=%d name=%s\n", pst->id, pst->name);
        free(pst);
    }
    queue_free(q3);

    return 0;
}