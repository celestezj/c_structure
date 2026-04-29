/*
 * 侵入式栈
 * 特点：
 *    1. 无额外节点包装，结构体直接嵌入链表节点
 *    2. 高性能
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list/stack.h"

// 业务结构体
struct student {
    int id;
    char name[16];

    // 侵入式栈节点（位置任意）
    STACK_ENTRY(student) node;
};

// 定义栈
STACK_HEAD(student_stack, student);

int main() {
    struct student_stack stk;
    STACK_INIT(&stk);

    // 创建元素
    struct student *s1 = malloc(sizeof(*s1));
    s1->id = 1; strcpy(s1->name, "Tom");

    struct student *s2 = malloc(sizeof(*s2));
    s2->id = 2; strcpy(s2->name, "Jerry");

    struct student *s3 = malloc(sizeof(*s3));
    s3->id = 3; strcpy(s3->name, "Alice");

    // push
    STACK_PUSH(&stk, s1, node);
    STACK_PUSH(&stk, s2, node);
    STACK_PUSH(&stk, s3, node);

    // pop（后进先出）
    struct student *elm;
    while ((elm = STACK_POP(&stk, node)) != NULL) {
        printf("pop: id=%d name=%s\n", elm->id, elm->name);
        free(elm);
    }

    return 0;
}