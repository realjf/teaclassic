#ifndef _STRING_INTERN_H_
#define _STRING_INTERN_H_

#include "mpool.h"
#include "khash.h"

#include <stdbool.h>
#include <stddef.h>

#ifndef STRBUFF_SZ
#define STRBUFF_SZ (256)
#endif

typedef char strbuff_t[STRBUFF_SZ];

MPOOL_TYPE(strbuff, strbuff_t)
__KHASH_TYPE(stridx, khint32_t, mp_ref_t)

bool si_init(mp_strbuff_t *pool, khash_t(stridx) * *index, size_t size);
const char *si_intern(const char *str, mp_strbuff_t *pool, khash_t(stridx) * index);
void si_shutdown(mp_strbuff_t *pool, khash_t(stridx) * index);
void si_clear(mp_strbuff_t *pool, khash_t(stridx) * index);

#endif /* _STRING_INTERN_H_ */
