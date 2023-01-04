#include "public/SDL_vec_rwops.h"
#include "public/vec.h"

#include <stdlib.h>
#include <assert.h>

VEC_TYPE(uchar, unsigned char)
VEC_IMPL(static inline, uchar, unsigned char)

#define VEC(rwops)          ((vec_uchar_t*)((rwops)->hidden.unknown.data1))
#define SEEK_IDX(rwops)     ((uintptr_t)((rwops)->hidden.unknown.data2))
#define SDL_RWOPS_VEC       (0xffff)

/*****************************************************************************/
/* STATIC FUNCTIONS                                                          */
/*****************************************************************************/

static Sint64 rw_vec_size(SDL_RWops *ctx)
{
    assert(ctx->type == SDL_RWOPS_VEC);
    return vec_size(VEC(ctx));
}

static Sint64 rw_vec_seek(SDL_RWops *ctx, Sint64 offset, int whence)
{
    assert(ctx->type == SDL_RWOPS_VEC);
    switch (whence) {
    case RW_SEEK_SET:
        ctx->hidden.unknown.data2 = (void*)offset;
        break;
    case RW_SEEK_CUR:
        ctx->hidden.unknown.data2 = (void*)(SEEK_IDX(ctx) + offset);
        break;
    case RW_SEEK_END:
        ctx->hidden.unknown.data2 = (void*)(rw_vec_size(ctx)-1 + offset);
        break;
    default:
        return SDL_SetError("rw_vec_seek: Unknown value for 'whence'");
    }
    return SEEK_IDX(ctx);
}

static size_t rw_vec_write(SDL_RWops *ctx, const void *ptr, size_t size, size_t num)
{
    assert(ctx->type == SDL_RWOPS_VEC);

    if(rw_vec_size(ctx) <= SEEK_IDX(ctx) + size * num
    && !vec_uchar_resize(VEC(ctx), SEEK_IDX(ctx) + size * num)) {

        SDL_Error(SDL_EFWRITE);
        return 0;
    }

    for(int i = 0; i < size * num; i++) {
        vec_uchar_push(VEC(ctx), *((unsigned char*)ptr++));
    }

    ctx->hidden.unknown.data2 = (void*)(SEEK_IDX(ctx) + size * num);
    return num;
}

static size_t rw_vec_read(SDL_RWops *ctx, void *ptr, size_t size, size_t num)
{
    assert(ctx->type == SDL_RWOPS_VEC);

    if(rw_vec_size(ctx) < SEEK_IDX(ctx) + size * num) {

        SDL_Error(SDL_EFREAD);
        return 0;
    }

    memcpy(ptr, &vec_AT(VEC(ctx), SEEK_IDX(ctx)), size * num);
    ctx->hidden.unknown.data2 = (void*)(SEEK_IDX(ctx) + size * num);
    return num;
}

static int rw_vec_close(SDL_RWops *ctx)
{
    assert(ctx->type == SDL_RWOPS_VEC);
    vec_uchar_destroy(ctx->hidden.unknown.data1);
    free(ctx);
    return 0;
}

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

SDL_RWops *TCSDL_VectorRWOps(void)
{
    SDL_RWops *ret = malloc(sizeof(SDL_RWops) + sizeof(vec_uchar_t));
    if(!ret)
        return ret;

    ret->size = rw_vec_size;
    ret->seek = rw_vec_seek;
    ret->read = rw_vec_read;
    ret->write = rw_vec_write;
    ret->close = rw_vec_close;
    ret->type = SDL_RWOPS_VEC;

    ret->hidden.unknown.data1 = ret + 1;
    vec_uchar_init(VEC(ret));
    ret->hidden.unknown.data2 = (void*)0; /* This is the seek index */

    return ret;
}

const char *TCSDL_VectorRWOpsRaw(SDL_RWops *ctx)
{
    return (const char*)VEC(ctx)->array;
}

