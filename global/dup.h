#ifndef __DUP_H__
#define __DUP_H__

#include <stdlib.h>
#include <string.h>

#define OBJ_DUP(ptr) \
({                  \
    typeof(*(ptr)) *__new = malloc(sizeof(*(ptr))); \
    if (__new) {     \
        memcpy(__new, (ptr), sizeof(*(ptr)));        \
    }                \
    __new;           \
})

#endif