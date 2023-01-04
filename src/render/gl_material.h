#ifndef _GL_MATERIAL_H_
#define _GL_MATERIAL_H_

#include "../tc_math.h"
#include "gl_texture.h"

struct material{
    GLfloat        ambient_intensity;
    vec3_t         diffuse_clr;
    vec3_t         specular_clr;
    struct texture texture;
    char           texname[64];
};


#endif /* _GL_MATERIAL_H_ */
