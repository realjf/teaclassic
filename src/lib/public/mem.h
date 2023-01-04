#ifndef _MEM_H_
#define _MEM_H_

#include <stdlib.h>
#include <stdint.h>

#define TC_FREE(...)                                  \
    do {                                              \
        free((void*)__VA_ARGS__);                     \
        __VA_ARGS__ = (void*)((uintptr_t)0xDEADBEEF); \
    } while (0)

#endif /* _MEM_H_ */
