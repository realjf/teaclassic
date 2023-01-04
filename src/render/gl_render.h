#ifndef _GL_RENDER_H_
#define _GL_RENDER_H_

#include "public/render.h"
#include "../map/public/tile.h"
#include "../tc_math.h"

#include <GL/glew.h>

#include <stddef.h>
#include <stdbool.h>

#define SHADOW_MAP_TUNIT (GL_TEXTURE16)

struct render_private;
struct vertex;
struct tile;
struct tile_desc;
struct map;

/* General */

void R_GL_Init(struct render_private *priv, const char *shader, const struct vertex *vbuff);
void R_GL_GlobalConfig(void);
void R_GL_SetViewport(int *x, int *y, int *w, int *h);

/* Shadows */

void R_GL_InitShadows(void);
vec3_t R_GL_GetLightPos(void);
void R_GL_SetLightSpaceTrans(const mat4x4_t *trans);
void R_GL_ShadowMapBind(void);

/* Water */

void R_GL_SetClipPlane(vec4_t plane_eq);

/* Terrain */
void R_GL_MapFogBindLast(GLuint tunit, GLuint shader_prog, const char *uname);
void R_GL_MapUpdateFogClear(void);


#endif /* _GL_RENDER_H_ */
