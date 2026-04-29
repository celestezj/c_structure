#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <stdlib.h>

/* 
 * Signature: ASSERT(assertion, [message]); 
 */
#define ASSERT(assertion, ...) \
    {\
        if(!(assertion)){\
            printf("(assert error)%s(%s:%d)", __FILE__, __FUNCTION__, __LINE__);\
                if(#__VA_ARGS__[0]){\
                    printf(": %s\n", #__VA_ARGS__);\
                }else{\
              	    printf(": 断言\"%s\"不成立\n", #assertion);\
                }\
                abort();\
        }\
    }

#endif