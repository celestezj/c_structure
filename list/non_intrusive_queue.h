#ifndef GENERIC_QUEUE_H
#define GENERIC_QUEUE_H

#include "list.h"
#include <stdlib.h>

/*
 * 非侵入式 泛型队列
 * 基于 list.h TAILQ
 */

typedef struct queue_node {
    void *data;
    TAILQ_ENTRY(queue_node) entry;
} queue_node_t;

TAILQ_HEAD(queue_head, queue_node);
typedef struct queue_head *queue_t;

// 创建队列
static inline queue_t queue_new(void)
{
    queue_t q = (queue_t)malloc(sizeof(*q));
    if (q)
        TAILQ_INIT(q);
    return q;
}

// 入队：传递 指针/值 都可以
#define queue_push(q, item) \
do { \
    queue_node_t *__n = (queue_node_t*)malloc(sizeof(*__n)); \
    __n->data = (void*)(item); \
    TAILQ_INSERT_TAIL(q, __n, entry); \
} while(0)

// 出队
static inline int queue_pop(queue_t q, void **out)
{
    if (!q || TAILQ_EMPTY(q))
        return -1;

    queue_node_t *n = TAILQ_FIRST(q);
    *out = n->data;

    TAILQ_REMOVE(q, n, entry);
    free(n);
    return 0;
}

static inline int queue_empty(queue_t q)
{
    return !q || TAILQ_EMPTY(q);
}

static inline void queue_free(queue_t q)
{
    if (!q) return;

    queue_node_t *n, *tmp;
    TAILQ_FOREACH_SAFE(n, q, entry, tmp) {
        TAILQ_REMOVE(q, n, entry);
        free(n);
    }
    free(q);
}

#endif