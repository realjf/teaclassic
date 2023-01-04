#ifndef _GL_MESH_H_
#define _GL_MESH_H_


#include "../tc_math.h"

struct vertex;

struct mesh{
    unsigned num_verts;
    GLuint   VBO;
    GLuint   VAO;
};


#endif /* _GL_MESH_H_ */
