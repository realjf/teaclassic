#ifndef _TC_MALLOC_H_
#define _TC_MALLOC_H_PF

#include <stddef.h>
#include <stdbool.h>

/* Allows allocating variable-sized chunks
 * from a single fixed-sized slab. */

bool tc_malloc_init(void *slab, size_t size);
void *tc_malloc(void *slab, size_t size);
void tc_free(void *slab, void *ptr);

/* Same as above, except the actual memory slab is
 * stored separately. The allocation simply updates
 * the block metadata and returns an offset into the
 * slab buffer, or -1 if the allocation failed. */

void *tc_metamalloc_init(size_t size);
void tc_metamalloc_destroy(void *meta);
int tc_metamalloc(void *meta, size_t size);
/* Supports any alignment, not just powers of two */
int tc_metamemalign(void *meta, size_t alignment, size_t size);
void tc_metafree(void *meta, size_t offset);

#endif /* _TC_MALLOC_H_ */
