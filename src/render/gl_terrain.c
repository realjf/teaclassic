#include "gl_render.h"
#include "gl_texture.h"
#include "gl_shader.h"
#include "gl_ringbuffer.h"
#include "gl_assert.h"
#include "gl_state.h"
#include "gl_perf.h"
#include "render_private.h"
#include "../main.h"
#include "../map/public/tile.h"

#include <assert.h>
#include <string.h>

#define ARR_SIZE(a)     (sizeof(a)/sizeof(a[0]))

/*****************************************************************************/
/* STATIC VARIABLES                                                          */
/*****************************************************************************/

static struct texture_arr     s_map_textures;
static bool                   s_map_ctx_active = false;
static struct gl_ring        *s_fog_ring;
static struct map_resolution  s_res;

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

void R_GL_MapInit(const char map_texfiles[][256], const size_t *num_textures,
                  const struct map_resolution *res)
{
    GL_PERF_ENTER();
    ASSERT_IN_RENDER_THREAD();

    size_t nchunks = res->chunk_w * res->chunk_h;
    s_fog_ring = R_GL_RingbufferInit(nchunks * TILES_PER_CHUNK_WIDTH * TILES_PER_CHUNK_HEIGHT * 3, RING_UBYTE);
    assert(s_fog_ring);

    R_GL_Texture_ArrayMakeMap(map_texfiles, *num_textures, &s_map_textures, GL_TEXTURE0);

    R_GL_StateSet(GL_U_MAP_RES, (struct uval){
        .type = UTYPE_IVEC4,
        .val.as_ivec4[0] = res->chunk_w,
        .val.as_ivec4[1] = res->chunk_h,
        .val.as_ivec4[2] = res->tile_w,
        .val.as_ivec4[3] = res->tile_h
    });

    s_res = *res;
    GL_ASSERT_OK();
    GL_PERF_RETURN_VOID();
}

void R_GL_MapUpdateFog(void *buff, const size_t *size)
{
    GL_PERF_ENTER();
    R_GL_RingbufferPush(s_fog_ring, buff, *size);
    GL_ASSERT_OK();
    GL_PERF_RETURN_VOID();
}

void R_GL_MapShutdown(void)
{
    R_GL_Texture_ArrayFree(s_map_textures);
    R_GL_RingbufferDestroy(s_fog_ring);
}

/* Push a fully 'visible' field into the ringbuffer. Must be followed
 * with a matching R_GL_MapInvalidate to consume the fence. */
void R_GL_MapUpdateFogClear(void)
{
    size_t size = s_res.chunk_w * s_res.chunk_h * s_res.tile_w * s_res.tile_h;
    void *buff = malloc(size);
    memset(buff, 0x2, size);
    R_GL_RingbufferPush(s_fog_ring, buff, size);
    free(buff);
}

void R_GL_MapBegin(const bool *shadows, const vec2_t *pos)
{
    GL_PERF_ENTER();
    ASSERT_IN_RENDER_THREAD();
    assert(!s_map_ctx_active);

    GLuint shader_prog;
    if(*shadows) {
        shader_prog = R_GL_Shader_GetProgForName("terrain-shadowed");
    }else {
        shader_prog = R_GL_Shader_GetProgForName("terrain");
    }
    assert(shader_prog != -1);
    R_GL_Shader_InstallProg(shader_prog);

    R_GL_Texture_BindArray(&s_map_textures, shader_prog);
    R_GL_RingbufferBindLast(s_fog_ring, GL_TEXTURE1, shader_prog, "visbuff");

    R_GL_StateSet(GL_U_MAP_POS, (struct uval){
        .type = UTYPE_VEC2,
        .val.as_vec2 = *pos
    });

    s_map_ctx_active = true;
    GL_PERF_RETURN_VOID();
}

void R_GL_MapEnd(void)
{
    GL_PERF_ENTER();
    ASSERT_IN_RENDER_THREAD();

    assert(s_map_ctx_active);
    s_map_ctx_active = false;

    GL_PERF_RETURN_VOID();
}

void R_GL_MapInvalidate(void)
{
    GL_PERF_ENTER();
    R_GL_RingbufferSyncLast(s_fog_ring);
    GL_PERF_RETURN_VOID();
}

void R_GL_MapFogBindLast(GLuint tunit, GLuint shader_prog, const char *uname)
{
    R_GL_RingbufferBindLast(s_fog_ring, tunit, shader_prog, uname);
}

