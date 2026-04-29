#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <setjmp.h>
#include "../list/non_intrusive_stack.h"
#include "../global/dup.h"

typedef struct {
    jmp_buf env;
    int type;
} exception_t;

#define try(exception) if ((exception.type = setjmp(exception.env)) == 0)
#define catch(exception, exception_type) else if (exception.type == exception_type)
#define throw(exception, exception_type) longjmp(exception.env, exception_type)
#define finally

#define INIT_STK(stk) stk = stack_new()
#define PUSH_EX(stk, ex) stack_push(stk, OBJ_DUP(&(ex)))
#define POP_EX(stk, ex) ex = *(exception_t*)stack_pop(stk)

#endif