#ifndef _GL_RINGBUFFER_H_
#define _GL_RINGBUFFER_H_

#include <stdbool.h>
#include <stddef.h>
#include <GL/glew.h>

/* The ringbuffer is used for efficient submission of streamed data
 * to the GPU. The key principle is using a manually synchronized buffer,
 * (or Persistent Mapped Buffer, if available) and filling up 1 section
 * of it every frame. The data is exposed to a shader via a pair of uniforms:
 *
 *     1. uname ((u)samplerBuffer)
 *     2. uname_offset (int)
 *
 * So long as there is sufficient room in the buffer, this should allow
 * for the GPU to use one section of the buffer, while the CPU is filling
 * another with the next frame's data, all without implicit synchronization
 * and minimal state changes.
 *
 * Usage:
 *
 *   ring = R_GL_RingbufferInit(...);
 *   for each frame:
 *       R_GL_RingbufferPush(ring, ...);
 *       R_GL_RingbufferBindLast(ring, ...);
 *       // queue the GL draw commands touching buffered data
 *       R_GL_RingbufferSyncLast(ring, ...);
 *   R_GL_RingbufferDestroy(ring);
 *
 */
struct gl_ring;

enum ring_format {
    RING_UBYTE,
    RING_FLOAT
};

struct gl_ring *R_GL_RingbufferInit(size_t size, enum ring_format fmt);
void R_GL_RingbufferDestroy(struct gl_ring *ring);
bool R_GL_RingbufferPush(struct gl_ring *ring, const void *data, size_t size);
bool R_GL_RingbufferAppendLast(struct gl_ring *ring, const void *data, size_t size);
bool R_GL_RingbufferExtendLast(struct gl_ring *ring, size_t size);
bool R_GL_RingbufferGetLastRange(struct gl_ring *ring, size_t *out_begin, size_t *out_end);
void R_GL_RingbufferBindLast(struct gl_ring *ring, GLuint tunit, GLuint shader_prog, const char *uname);
void R_GL_RingbufferSyncLast(struct gl_ring *ring);
GLuint R_GL_RingbufferGetVBO(struct gl_ring *ring);

#endif /* _GL_RINGBUFFER_H_ */
