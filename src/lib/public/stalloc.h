#ifndef _STALLOC_H_
#define _STALLOC_H_

#include <stddef.h>
#include <stdbool.h>

#define STATIC_BUFF_SZ (512 * 1024)
#define MEMBLOCK_SZ (8 * 1024 * 1024)

/* The memstack allows variable-sized allocations from larger pre-allocated
 * blocks. The point is to reduce ovehead of 'malloc' and 'free' when wanting
 * to make many small allocations.
 *
 * The memblocks are chained in a linked list. When one memblock is exhausted,
 * another one is allocated from the OS and appended to it. The purpose is to
 * allow arbitrary many allocations without needing to invalidate pointers to
 * prior allocations, which would be required with a 'realloc'-based approach.
 *
 * The allocations cannot be freed in arbitrary order. The API provides only a
 * means to clear all the allocations at once. Hence, this allocator is good
 * for cases where all allocations will have the same lifetime (ex. a single
 * frame).
 */

struct st_mem {
    struct st_mem *next;
    unsigned char raw[MEMBLOCK_SZ];
};

struct memstack {
    struct st_mem *head;
    struct st_mem *tail;
    void *top; /* Empty Ascending stack */
};

bool stalloc_init(struct memstack *st);
void stalloc_destroy(struct memstack *st);

void *stalloc(struct memstack *st, size_t size);
void stalloc_clear(struct memstack *st);

/* The smemstack is just like the memstack, except that the first 'STATIC_BUFF_SZ'
 * bytes of allocations will be from the local 'mem' buffer, which can be declared
 * on the stack or in static storage.
 */

struct smemstack {
    unsigned char mem[STATIC_BUFF_SZ];
    void *top; /* Empty Ascending stack; when NULL, extra.top is the TOS */
    struct memstack extra;
};

bool sstalloc_init(struct smemstack *st);
void sstalloc_destroy(struct smemstack *st);

void *sstalloc(struct smemstack *st, size_t size);
void sstalloc_clear(struct smemstack *st);


#endif /* _STALLOC_H_ */
