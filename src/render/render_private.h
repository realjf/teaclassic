#ifndef _RENDER_PRIVATE_H_
#define _RENDER_PRIVATE_H_

#include "gl_mesh.h"
#include "gl_texture.h"
#include "../map/public/tile.h"

struct terrain_vert;
struct map;

struct render_private {
    struct mesh mesh;
    size_t num_materials;
    struct material *materials;
    struct texture_arr material_arr;
    GLuint shader_prog;
    GLuint shader_prog_dp; /* for the depth pass */
    GLuint vertex_stride;
};

/* Tile */
void R_TileGetVertices(const struct map *map, struct tile_desc td, struct terrain_vert *out);

#endif /* _RENDER_PRIVATE_H_ */
