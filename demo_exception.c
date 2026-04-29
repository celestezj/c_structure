#include <stdio.h>
#include "exception/exception.h"

stack_t stk;
exception_t ex;

void do_something_inner(){
    PUSH_EX(stk, ex);
    try(ex) {
        printf("inner error\n");
        throw(ex, 1);
    } catch(ex, 1) { //don't throw directly in catch
        printf("catch inner exception type 1\n");
    } catch(ex, 2) {
        printf("catch inner exception type 2\n");
    } finally {
        printf("inner finally\n");
        POP_EX(stk, ex);
    }
}

int main(){
    INIT_STK(stk);
    try(ex) {
        do_something_inner();
        printf("main(outer) error\n");
        throw(ex, 4);
    } catch(ex, 3) {
        printf("catch main exception type 3\n");
    } catch(ex, 4) {
        printf("catch main exception type 4\n");
    } finally {
        printf("main over\n");
    }
    printf("stk empty: %d\n", stack_empty(stk));
    return 0;
}