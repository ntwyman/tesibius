#pragma once

#ifdef DEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define INIT_ASSERT(x, msg)                                                    \
    printf("Assertion failed - %s", msg);                                      \
    for (;;)                                                                   \
        ;