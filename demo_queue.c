#include <stdio.h>
#include <stdlib.h>
#include "list/queue.h"

// 业务结构体（直接嵌入队列节点）
struct student {
    int id;
    char name[16];

    // 侵入式链表节点
    QUEUE_ENTRY(student) node;
};

// 定义队列
QUEUE_HEAD(student_queue, student);

int main() {
    struct student_queue q;
    QUEUE_INIT(&q);

    // 创建元素
    struct student *s1 = malloc(sizeof(*s1));
    s1->id = 1;
    snprintf(s1->name, sizeof(s1->name), "Tom");

    struct student *s2 = malloc(sizeof(*s2));
    s2->id = 2;
    snprintf(s2->name, sizeof(s2->name), "Jerry");

    // ==========================
    // push（泛型）
    // ==========================
    QUEUE_PUSH(&q, s1, node);
    QUEUE_PUSH(&q, s2, node);

    // ==========================
    // pop（泛型）
    // ==========================
    struct student *elm;
    while ((elm = QUEUE_POP(&q, node)) != NULL) {
        printf("pop: id=%d, name=%s\n", elm->id, elm->name);
        free(elm);
    }

    return 0;
}