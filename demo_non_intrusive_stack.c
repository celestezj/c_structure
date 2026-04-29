/*
 * 非侵入式泛型栈
 * 特点：
 *    1. 业务结构体无需嵌入链表节点
 *    2. 泛型存储任意指针类型
 *    3. 接口简单，不用关心底层实现
 */
#include <stdio.h>
#include <stdlib.h>
#include "list/non_intrusive_stack.h"

struct student {
    int id;
    char name[16];
};

int main() {
    stack_t stk = stack_new();

    // 结构体
    struct student *s1 = malloc(sizeof(*s1));
    s1->id = 101; sprintf(s1->name, "Tom");

    struct student *s2 = malloc(sizeof(*s2));
    s2->id = 102; sprintf(s2->name, "Jerry");

    stack_push(stk, s1);
    stack_push(stk, s2);

    // pop
    struct student *p;
    while ((p = stack_pop(stk)) != NULL) {
        printf("pop: id=%d name=%s\n", p->id, p->name);
        free(p);
    }

    stack_free(stk);
    return 0;
}