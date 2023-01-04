#ifndef _GL_VERTEX_H_
#define _GL_VERTEX_H_

#include "../tc_math.h"
#include <stdint.h>
#include <GL/glew.h>

#define PACK_32(name, ...) \
    union {                \
        struct {           \
            __VA_ARGS__    \
        };                 \
        uint32_t name;     \
    }

#define VERTEX_BASE \
    vec3_t pos;     \
    vec2_t uv;      \
    vec3_t normal;  \
    GLint material_idx;

struct vertex {
    VERTEX_BASE
};

struct anim_vert {
    VERTEX_BASE
    GLubyte joint_indices[6];
    GLfloat weights[6];
};

struct terrain_vert {
    VERTEX_BASE
    uint16_t blend_mode;
    uint16_t middle_indices;
    /* Each uint32_t holds 4 8-bit indices */
    uint32_t c1_indices[2]; /* corner 1 */
    uint32_t c2_indices[2]; /* corner 2 */
    uint32_t tb_indices;
    uint32_t lr_indices;
};

struct colored_vert {
    vec3_t pos;
    vec4_t color;
};

struct textured_vert {
    vec3_t pos;
    vec2_t uv;
};

#endif /* _GL_VERTEX_H_ */
