#include "gl_ringbuffer.h"
#include "gl_assert.h"
#include "gl_perf.h"
#include "gl_shader.h"
#include "gl_state.h"
#include "../lib/public/tc_string.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* How many discrete sets of data (guarded by fences) the buffer can hold */
#define NMAXMARKERS     (16)
#define TIMEOUT_NSEC    (((uint64_t)10) * 1000 * 1000 * 1000)

/* On some hardware persistent mapped buffers are faster. However, they
 * are not part of OpenGL 3.3 core which we are targeting. So, fallback
 * to unsynchronized VBOs if the extension isn't present. */
enum mode{
    MODE_UNSYNCHRONIZED_VBO,
    MODE_PERSISTENT_MAPPED_BUFFER,
};

struct marker{
    size_t begin;
    size_t end;
};

struct buffer_ops{
    void  (*init) (struct gl_ring*);
    void *(*map)  (struct gl_ring*, size_t offset, size_t size);
    void  (*unmap)(struct gl_ring*);
};

struct gl_ring{
    enum mode         mode;
    struct buffer_ops ops;
    void             *user;
    size_t            pos;
    size_t            size;
    /* The buffer object backing the ringbuffer */
    GLuint            VBO;
    /* The texture buffer object associated with the VBO -
     * for exposing the buffer to shaders. */
    GLuint            tex_buff;
    /* Fences are to make sure we don't overwrite the next
     * part of the buffer before it's consumed by the GPU. */
    GLsync            fences[NMAXMARKERS];
    /* The markers hold the buffer positions guarded by the fences */
    size_t            nmarkers;
    size_t            imark_head, imark_tail;
    struct marker     markers[NMAXMARKERS];
};

/*****************************************************************************/
/* STATIC FUNCTIONS                                                          */
/*****************************************************************************/

static bool ring_wait_one(struct gl_ring *ring)
{
    GL_PERF_ENTER();
    assert(ring->fences[ring->imark_tail] > 0);

    if(ring->nmarkers == 0)
        GL_PERF_RETURN(false);

    GLenum result = glClientWaitSync(ring->fences[ring->imark_tail], 0, TIMEOUT_NSEC);
    glDeleteSync(ring->fences[ring->imark_tail]);
    ring->fences[ring->imark_tail] = 0;
    ring->imark_tail = (ring->imark_tail + 1) % NMAXMARKERS;
    ring->nmarkers--;

    if(result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED)
        GL_PERF_RETURN(false);

    GL_PERF_RETURN(true);
}

static bool ring_section_free(const struct gl_ring *ring, size_t size)
{
    if(ring->nmarkers == NMAXMARKERS)
        return false;
    if(ring->nmarkers == 0)
        return true;

    assert((ring->imark_head - ring->imark_tail + 1) % NMAXMARKERS == ring->nmarkers);
    size_t begin = ring->markers[ring->imark_tail].begin;
    size_t end = ring->markers[ring->imark_head].end;

    if(end == begin) /* entire buffer is used up */
        return false;

    if(end < begin) { /* wrap around */
        return begin - end >= size;
    }else{
        size_t end_size = ring->size - end;
        size_t start_size = begin;
        return (end_size + start_size) >= size;
    }
}

static void pmb_init(struct gl_ring *ring)
{
    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    glBindBuffer(GL_TEXTURE_BUFFER, ring->VBO);
    glBufferStorage(GL_TEXTURE_BUFFER, ring->size, NULL, flags);
    ring->user = glMapBufferRange(GL_TEXTURE_BUFFER, 0, ring->size, flags);
}

static void *pmb_map(struct gl_ring *ring, size_t offset, size_t size)
{
    return ((unsigned char*)ring->user) + offset;
}

static void pmb_unmap(struct gl_ring *ring)
{
    /* no-op */
}

static void unsynch_vbo_init(struct gl_ring *ring)
{
    glBindBuffer(GL_TEXTURE_BUFFER, ring->VBO);
    glBufferData(GL_TEXTURE_BUFFER, ring->size, NULL, GL_STREAM_DRAW);
}

static void *unsynch_vbo_map(struct gl_ring *ring, size_t offset, size_t size)
{
    glBindBuffer(GL_TEXTURE_BUFFER, ring->VBO);
    return glMapBufferRange(GL_TEXTURE_BUFFER, offset, size,
        GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
}

static void unsynch_vbo_unmap(struct gl_ring *ring)
{
    glUnmapBuffer(GL_TEXTURE_BUFFER);
}

/*****************************************************************************/
/* STATIC VARIABLES                                                          */
/*****************************************************************************/

struct gl_ring *R_GL_RingbufferInit(size_t size, enum ring_format fmt)
{
    struct gl_ring *ret = malloc(sizeof(struct gl_ring));
    if(!ret)
        return NULL;

    glGenBuffers(1, &ret->VBO);
    glGenTextures(1, &ret->tex_buff);

    ret->pos = 0;
    ret->size = size;
    ret->imark_head = 0;
    ret->imark_tail = 0;
    ret->nmarkers = 0;
    memset(&ret->fences, 0, sizeof(ret->fences));
    memset(&ret->markers, 0, sizeof(ret->fences));

    if(GLEW_ARB_buffer_storage) {
        ret->mode = MODE_PERSISTENT_MAPPED_BUFFER;
        ret->ops = (struct buffer_ops){
            pmb_init,
            pmb_map,
            pmb_unmap
        };
    }else{
        ret->mode = MODE_UNSYNCHRONIZED_VBO;
        ret->ops = (struct buffer_ops){
            unsynch_vbo_init,
            unsynch_vbo_map,
            unsynch_vbo_unmap
        };
    }
    ret->ops.init(ret);

    glBindTexture(GL_TEXTURE_BUFFER, ret->tex_buff);
    if(fmt == RING_UBYTE) {
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R8UI, ret->VBO);
    }else if(fmt == RING_FLOAT) {
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, ret->VBO);
    }

    GL_ASSERT_OK();
    return ret;
}

void R_GL_RingbufferDestroy(struct gl_ring *ring)
{
    while(ring->nmarkers) {
        ring_wait_one(ring);
    }
    glDeleteBuffers(1, &ring->VBO);
    glDeleteTextures(1, &ring->tex_buff);
    free(ring);
}

bool R_GL_RingbufferPush(struct gl_ring *ring, const void *data, size_t size)
{
    if(size > ring->size) {
        return false;
    }

    while(!ring_section_free(ring, size)) {
        if(!ring_wait_one(ring))
            return false;
    }

    size_t left = ring->size - ring->pos;
    size_t old_pos = ring->pos;

    if(size <= left) {
        void *ptr = ring->ops.map(ring, ring->pos, size);
        memcpy(ptr, data, size);
        ring->pos = (ring->pos + size) % ring->size;
    }else{
        size_t start = size - left;
        if(left > 0) {
            void *ptr = ring->ops.map(ring, ring->pos, left);
            memcpy(ptr, data, left);
            ring->ops.unmap(ring);
        }

        /* wrap around */
        void *ptr = ring->ops.map(ring, 0, start);
        memcpy(ptr, ((char*)data) + left, start);
        ring->pos = start;
    }

    ring->ops.unmap(ring);
    ring->imark_head = (ring->imark_head + 1) % NMAXMARKERS;
    ring->markers[ring->imark_head] = (struct marker){old_pos, ring->pos};

    if(!ring->nmarkers)
        ring->imark_tail = ring->imark_head;

    ring->nmarkers++;

    GL_ASSERT_OK();
    return true;
}

bool R_GL_RingbufferAppendLast(struct gl_ring *ring, const void *data, size_t size)
{
    assert(ring->nmarkers);
    assert(ring->fences[ring->imark_head] == 0);

    if(size > ring->size) {
        return false;
    }

    while(!ring_section_free(ring, size)) {
        if(!ring_wait_one(ring))
            return false;
    }

    size_t left = ring->size - ring->pos;
    size_t old_pos = ring->pos;

    if(size <= left) {
        void *ptr = ring->ops.map(ring, ring->pos, size);
        memcpy(ptr, data, size);
        ring->pos = (ring->pos + size) % ring->size;
    }else{
        size_t start = size - left;
        if(left > 0) {
            void *ptr = ring->ops.map(ring, ring->pos, left);
            memcpy(ptr, data, left);
            ring->ops.unmap(ring);
        }

        /* wrap around */
        void *ptr = ring->ops.map(ring, 0, start);
        memcpy(ptr, ((char*)data) + left, start);
        ring->pos = start;
    }

    ring->ops.unmap(ring);
    ring->markers[ring->imark_head].end = ring->pos;

    GL_ASSERT_OK();
    return true;
}

bool R_GL_RingbufferExtendLast(struct gl_ring *ring, size_t size)
{
    assert(ring->nmarkers);
    assert(ring->fences[ring->imark_head] == 0);

    if(size > ring->size) {
        return false;
    }

    while(!ring_section_free(ring, size)) {
        if(!ring_wait_one(ring))
            return false;
    }

    size_t left = ring->size - ring->pos;
    size_t old_pos = ring->pos;

    if(size <= left) {
        ring->pos = (ring->pos + size) % ring->size;
    }else{
        size_t start = size - left;
        ring->pos = start;
    }

    ring->markers[ring->imark_head].end = ring->pos;
    return true;
}

bool R_GL_RingbufferGetLastRange(struct gl_ring *ring, size_t *out_begin, size_t *out_end)
{
    if(ring->nmarkers == 0)
        return false;

    *out_begin = ring->markers[ring->imark_head].begin;
    *out_end = ring->markers[ring->imark_head].end;
    return true;
}

void R_GL_RingbufferBindLast(struct gl_ring *ring, GLuint tunit, GLuint shader_prog, const char *uname)
{
    assert(ring->nmarkers);
    assert(ring->fences[ring->imark_head] == 0);
    size_t bpos = ring->markers[ring->imark_head].begin;

    char uname_offset[128];
    tc_snprintf(uname_offset, sizeof(uname_offset), "%s_offset", uname);

    glActiveTexture(tunit);
    glBindTexture(GL_TEXTURE_BUFFER, ring->tex_buff);
    R_GL_Shader_InstallProg(shader_prog);

    R_GL_StateSet(uname, (struct uval){
        .type = UTYPE_INT,
        .val.as_int = tunit - GL_TEXTURE0
    });
    R_GL_StateInstall(uname, shader_prog);

    R_GL_StateSet(uname_offset, (struct uval){
        .type = UTYPE_INT,
        .val.as_int = bpos
    });
    R_GL_StateInstall(uname_offset, shader_prog);
}

void R_GL_RingbufferSyncLast(struct gl_ring *ring)
{
    assert(ring->nmarkers);
    assert(ring->fences[ring->imark_head] == 0);
    ring->fences[ring->imark_head] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

GLuint R_GL_RingbufferGetVBO(struct gl_ring *ring)
{
    return ring->VBO;
}

