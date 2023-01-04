#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "tc_math.h"
#include <stdbool.h>

struct camera;
struct frustum;
extern const unsigned g_sizeof_camera;

struct bound_box {
    GLfloat x, z;
    GLfloat w, h;
};

#define DECL_CAMERA_STACK(_name) \
    char _name[g_sizeof_camera]

#define CAM_Z_NEAR_DIST (5.0f)
#define CAM_FOV_RAD (M_PI / 4.0f)

struct camera *Camera_New(void);
void Camera_Free(struct camera *cam);

void Camera_SetPos(struct camera *cam, vec3_t pos);
void Camera_SetDir(struct camera *cam, vec3_t dir);
void Camera_SetPitchAndYaw(struct camera *cam, float pitch, float yaw);
void Camera_SetSpeed(struct camera *cam, float speed);
void Camera_SetSens(struct camera *cam, float sensitivity);

float Camera_GetSpeed(const struct camera *cam);
float Camera_GetSens(const struct camera *cam);
float Camera_GetYaw(const struct camera *cam);
float Camera_GetPitch(const struct camera *cam);
float Camera_GetHeight(const struct camera *cam);
vec3_t Camera_GetDir(const struct camera *cam);
vec3_t Camera_GetPos(const struct camera *cam);

void Camera_MakeViewMat(const struct camera *cam, mat4x4_t *out);
void Camera_MakeProjMat(const struct camera *cam, mat4x4_t *out);

void Camera_RestrictPosWithBox(struct camera *cam, struct bound_box box);
void Camera_UnrestrictPos(struct camera *cam);
bool Camera_PosIsRestricted(const struct camera *cam);

/* These should be called once per tick, at most. The amount moved depends
 * on the camera speed.
 */
void Camera_MoveLeftTick(struct camera *cam);
void Camera_MoveRightTick(struct camera *cam);
void Camera_MoveFrontTick(struct camera *cam);
void Camera_MoveBackTick(struct camera *cam);
void Camera_MoveDirectionTick(struct camera *cam, vec3_t dir);
void Camera_ChangeDirection(struct camera *cam, int dx, int dy);

/* Should be called once per frame, after all movements have been set, but
 * prior to rendering.
 */
void Camera_TickFinishPerspective(struct camera *cam);
void Camera_TickFinishOrthographic(struct camera *cam, vec2_t bot_left, vec2_t top_right);

void Camera_MakeFrustum(const struct camera *cam, struct frustum *out);

#endif /* _CAMERA_H_ */
