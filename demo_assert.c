#include <stdio.h>
#include "exception/assert.h"

int main(){
    int a = 666;
    ASSERT(a > 1000, "a小于等于1000！"); //(assert error)demo_assert.c(main:6): "a小于等于1000！"
    printf("OVER\n");
    return 0;
}