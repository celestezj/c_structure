/*
 * 非侵入式泛型栈
 * 基于 list.h TAILQ
 * 无侵入业务结构体
 */
#ifndef __GENERIC_STACK_H__
#define __GENERIC_STACK_H__

#include "list.h"
#include <stdlib.h>

typedef struct stack_node {
    void *data;
    TAILQ_ENTRY(stack_node) entry;
} stack_node_t;

TAILQ_HEAD(stack_head, stack_node);
typedef struct stack_head *stack_t;

static inline stack_t stack_new(void) {
    stack_t s = malloc(sizeof(*s));
    if (s) TAILQ_INIT(s);
    return s;
}

static inline void stack_push(stack_t s, void *data) {
    stack_node_t *n = malloc(sizeof(*n));
    n->data = data;
    TAILQ_INSERT_HEAD(s, n, entry);
}

static inline void *stack_pop(stack_t s) {
    if (TAILQ_EMPTY(s)) return NULL;
    stack_node_t *n = TAILQ_FIRST(s);
    void *data = n->data;
    TAILQ_REMOVE(s, n, entry);
    free(n);
    return data;
}

static inline int stack_empty(stack_t s) {
    return TAILQ_EMPTY(s);
}

static inline void stack_free(stack_t s) {
    stack_node_t *n, *tmp;
    TAILQ_FOREACH_SAFE(n, s, entry, tmp) {
        TAILQ_REMOVE(s, n, entry);
        free(n);
    }
    free(s);
}

#endif