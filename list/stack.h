#ifndef INTRUSIVE_STACK_H
#define INTRUSIVE_STACK_H

#include "list.h"

#define STACK_HEAD(name, type)    TAILQ_HEAD(name, type)
#define STACK_ENTRY(type)         TAILQ_ENTRY(type)
#define STACK_INIT(head)          TAILQ_INIT(head)
#define STACK_EMPTY(head)         TAILQ_EMPTY(head)

#define STACK_PUSH(head, elm, field) \
    TAILQ_INSERT_HEAD(head, elm, field)

#define STACK_POP(head, field) ({ \
    typeof((head)->tqh_first) __elm = NULL; \
    __elm = TAILQ_FIRST(head); \
    if (__elm) { \
        TAILQ_REMOVE(head, __elm, field); \
    } \
    __elm; \
})

#endif