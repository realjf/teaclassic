#ifndef _GL_SHADER_H_
#define _GL_SHADER_H_

#include <GL/glew.h>

#include <stdbool.h>

bool R_GL_Shader_InitAll(const char *base_path);
GLint R_GL_Shader_GetProgForName(const char *name);
const char *R_GL_Shader_GetName(GLuint prog);
void R_GL_Shader_Install(const char *name);
void R_GL_Shader_InstallProg(GLuint prog);
GLuint R_GL_Shader_GetCurrActive(void);

#endif /* _GL_SHADER_H_ */
