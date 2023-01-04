#ifndef _GL_TEXTURE_H_
#define _GL_TEXTURE_H_

#include <GL/glew.h>
#include <stdbool.h>

struct material;

struct texture {
    GLuint id;
    GLuint tunit;
};

struct texture_arr {
    GLuint id;
    GLuint tunit;
};

bool R_GL_Texture_Init(void);
void R_GL_Texture_Shutdown(void);

void R_GL_Texture_ArrayAlloc(size_t num_elems, struct texture_arr *out, GLuint tunit);
void R_GL_Texture_ArrayFree(struct texture_arr array);
void R_GL_Texture_ArrayCopyElem(struct texture_arr *dst, int dst_idx, struct texture_arr *src, int src_idx);

void R_GL_Texture_ArrayMake(const struct material *mats, size_t num_mats,
                            struct texture_arr *out, GLuint tunit);
void R_GL_Texture_ArrayMakeMap(const char texnames[][256], size_t num_textures,
                               struct texture_arr *out, GLuint tunit);

void R_GL_Texture_Bind(const struct texture *text, GLuint shader_prog);
void R_GL_Texture_BindArray(const struct texture_arr *arr, GLuint shader_prog);

bool R_GL_Texture_Load(const char *basedir, const char *name, GLuint *out);
void R_GL_Texture_Free(const char *basedir, const char *name);
void R_GL_Texture_GetOrLoad(const char *basedir, const char *name, GLuint *out);
bool R_GL_Texture_GetForName(const char *basedir, const char *name, GLuint *out);
void R_GL_Texture_GetSize(GLuint texid, int *out_w, int *out_h, int *out_d);
bool R_GL_Texture_AddExisting(const char *name, GLuint id);

#endif /* _GL_TEXTURE_H_ */
