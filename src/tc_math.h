#ifndef _TC_MATH_H_
#define _TC_MATH_H_

#include <GL/glew.h> /* GLfloat definition */
#include <stdio.h>   /* FILE definition    */
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#ifndef __USE_MISC
#define __USE_MISC
#endif
#include <math.h> /* M_PI definition    */

#define DEG_TO_RAD(_deg) ((_deg) * (M_PI / 180.0f))
#define RAD_TO_DEG(_rad) ((_rad) * (180.0f / M_PI))

typedef union vec2 {
    GLfloat raw[2];
    struct {
        GLfloat x, y;
    };
    struct {
        GLfloat _x, z;
    };
} vec2_t;

typedef union vec3 {
    GLfloat raw[3];
    struct {
        GLfloat x, y, z;
    };
} vec3_t;

typedef union vec4 {
    GLfloat raw[4];
    struct {
        GLfloat x, y, z, w;
    };
} vec4_t;

typedef vec4_t quat_t;

typedef union mat3x3 {
    GLfloat raw[9];
    GLfloat cols[3][3];
    struct {
        GLfloat m0, m1, m2,
            m3, m4, m5,
            m6, m7, m8;
    };
} mat3x3_t;

typedef union mat4x4 {
    GLfloat raw[16];
    GLfloat cols[4][4];
    struct {
        GLfloat m0, m1, m2, m3,
            m4, m5, m6, m7,
            m8, m9, m10, m11,
            m12, m13, m14, m15;
    };
} mat4x4_t;

/*****************************************************************************/
/* vec2                                                                      */
/*****************************************************************************/

GLfloat TCM_Vec2_Dot(vec2_t *op1, vec2_t *op2);
void TCM_Vec2_Add(vec2_t *op1, vec2_t *op2, vec2_t *out);
void TCM_Vec2_Sub(vec2_t *op1, vec2_t *op2, vec2_t *out);
void TCM_Vec2_Scale(vec2_t *op1, GLfloat scale, vec2_t *out);
GLfloat TCM_Vec2_Len(const vec2_t *op1);
void TCM_Vec2_Normal(vec2_t *op1, vec2_t *out);
void TCM_Vec2_Dump(vec2_t *vec, FILE *dumpfile);

/*****************************************************************************/
/* vec3                                                                      */
/*****************************************************************************/

void TCM_Vec3_Cross(vec3_t *a, vec3_t *b, vec3_t *out);
GLfloat TCM_Vec3_Dot(vec3_t *op1, vec3_t *op2);
void TCM_Vec3_Add(vec3_t *op1, vec3_t *op2, vec3_t *out);
void TCM_Vec3_Sub(vec3_t *op1, vec3_t *op2, vec3_t *out);
void TCM_Vec3_Scale(vec3_t *op1, GLfloat scale, vec3_t *out);
GLfloat TCM_Vec3_Len(const vec3_t *op1);
void TCM_Vec3_Normal(vec3_t *op1, vec3_t *out);
void TCM_Vec3_Dump(vec3_t *vec, FILE *dumpfile);

/*****************************************************************************/
/* vec4                                                                      */
/*****************************************************************************/

GLfloat TCM_Vec4_Dot(vec4_t *op1, vec4_t *op2, vec4_t *out);
void TCM_Vec4_Add(vec4_t *op1, vec4_t *op2, vec4_t *out);
void TCM_Vec4_Sub(vec4_t *op1, vec4_t *op2, vec4_t *out);
void TCM_Vec4_Scale(vec4_t *op1, GLfloat scale, vec4_t *out);
GLfloat TCM_Vec4_Len(const vec4_t *op1);
void TCM_Vec4_Normal(vec4_t *op1, vec4_t *out);
void TCM_Vec4_Dump(vec4_t *vec, FILE *dumpfile);

/*****************************************************************************/
/* mat3x3                                                                    */
/*****************************************************************************/

void TCM_Mat3x3_Scale(mat3x3_t *op1, GLfloat scale, mat3x3_t *out);
void TCM_Mat3x3_Mult3x3(mat3x3_t *op1, mat3x3_t *op2, mat3x3_t *out);
void TCM_Mat3x3_Mult3x1(mat3x3_t *op1, vec3_t *op2, vec3_t *out);
void TCM_Mat3x3_Identity(mat3x3_t *out);

/*****************************************************************************/
/* mat4x4                                                                    */
/*****************************************************************************/

void TCM_Mat4x4_Scale(mat4x4_t *op1, GLfloat scale, mat4x4_t *out);
void TCM_Mat4x4_Mult4x4(mat4x4_t *op1, mat4x4_t *op2, mat4x4_t *out);
void TCM_Mat4x4_Mult4x1(mat4x4_t *op1, vec4_t *op2, vec4_t *out);
void TCM_Mat4x4_Identity(mat4x4_t *out);

void TCM_Mat4x4_MakeScale(GLfloat s1, GLfloat s2, GLfloat s3, mat4x4_t *out);
void TCM_Mat4x4_MakeTrans(GLfloat tx, GLfloat ty, GLfloat tz, mat4x4_t *out);
void TCM_Mat4x4_MakeRotX(GLfloat radians, mat4x4_t *out);
void TCM_Mat4x4_MakeRotY(GLfloat radians, mat4x4_t *out);
void TCM_Mat4x4_MakeRotZ(GLfloat radians, mat4x4_t *out);
void TCM_Mat4x4_RotFromQuat(const quat_t *quat, mat4x4_t *out);
void TCM_Mat4x4_RotFromEuler(GLfloat deg_x, GLfloat deg_y, GLfloat deg_z, mat4x4_t *out);
void TCM_Mat4x4_Inverse(mat4x4_t *in, mat4x4_t *out);
void TCM_Mat4x4_Transpose(mat4x4_t *in, mat4x4_t *out);

void TCM_Mat4x4_MakePerspective(GLfloat fov_radians, GLfloat aspect_ratio,
                                GLfloat z_near, GLfloat z_far, mat4x4_t *out);
void TCM_Mat4x4_MakeOrthographic(GLfloat left, GLfloat right,
                                 GLfloat top, GLfloat bot,
                                 GLfloat near, GLfloat far, mat4x4_t *out);
void TCM_Mat4x4_MakeLookAt(vec3_t *camera_pos, vec3_t *target_pos,
                           vec3_t *up_dir, mat4x4_t *out);

/*****************************************************************************/
/* quat                                                                      */
/*****************************************************************************/

void TCM_Quat_FromRotMat(mat4x4_t *mat, quat_t *out);
void TCM_Quat_ToEuler(quat_t *q, float *out_roll, float *out_pitch, float *out_yaw);
void TCM_Quat_MultQuat(quat_t *op1, quat_t *op2, quat_t *out);
void TCM_Quat_Normal(quat_t *op1, quat_t *out);
void TCM_Quat_Inverse(quat_t *op1, quat_t *out);
/* The angle of rotation from op1 to op2 around the Y axis, in radians */
GLfloat TCM_Quat_PitchDiff(quat_t *op1, quat_t *op2);

/*****************************************************************************/
/* Other                                                                     */
/*****************************************************************************/

GLfloat TCM_BilinearInterp(GLfloat q11, GLfloat q12, GLfloat q21, GLfloat q22,
                           GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2,
                           GLfloat x, GLfloat y);

#endif /* _TC_MATH_H_ */
