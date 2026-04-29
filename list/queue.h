#ifndef INTRUSIVE_QUEUE_H
#define INTRUSIVE_QUEUE_H

#include "list.h"
#include <stddef.h>

/*
================================================================================
  侵入式泛型队列（基于 TAILQ）
  特点：
      1. 侵入式，无额外节点包装
      2. 泛型宏，支持任意结构体
      3. push/pop 全 O(1)
      4. 内核级最佳实践
================================================================================
*/

#define QUEUE_HEAD(name, type)     TAILQ_HEAD(name, type)
#define QUEUE_ENTRY(type)          TAILQ_ENTRY(type)

#define QUEUE_INIT(head)           TAILQ_INIT(head)
#define QUEUE_EMPTY(head)          TAILQ_EMPTY(head)

/*
 * 泛型入队（尾插）
 * queue: 队列头
 * elm:   你的业务结构体指针
 * field: 结构体里 TAILQ_ENTRY 的字段名
 */
#define QUEUE_PUSH(queue, elm, field) \
    TAILQ_INSERT_TAIL(queue, elm, field)

/*
 * 泛型出队（头删）
 * queue: 队列头
 * type:  你的结构体类型
 * field: 结构体里 TAILQ_ENTRY 的字段名
 */
#if 0
#define QUEUE_POP(queue, type, field) \
    ({ \
        type *__elm = TAILQ_FIRST(queue); \
        if (__elm) { \
            TAILQ_REMOVE(queue, __elm, field); \
        } \
        __elm; \
    })
#endif
#define QUEUE_POP(head, field) ({                 \
    typeof((head)->tqh_first) __elm = NULL;       \
    __elm = TAILQ_FIRST(head);                    \
    if (__elm) {                                  \
        TAILQ_REMOVE(head, __elm, field);         \
    }                                             \
    __elm;                                        \
})

#endif