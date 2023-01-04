#ifndef _PHYS_H_
#define _PHYS_H_

struct obb;
struct entity;
struct SDL_RWops;

#include "../../tc_math.h"

#include <stdint.h>
#include <stdbool.h>

#define PROJ_ONLY_HIT_COMBATABLE (1 << 0)
#define PROJ_ONLY_HIT_ENEMIES (1 << 1)

struct proj_hit {
    uint32_t ent_uid;
    uint32_t proj_uid;
    uint32_t parent_uid;
    uint32_t cookie;
};

struct proj_desc {
    const char *basedir;
    const char *tcobj;
    vec3_t scale;
    float speed;
};

bool P_Projectile_Init(void);
void P_Projectile_Shutdown(void);

uint32_t P_Projectile_Add(vec3_t origin, vec3_t velocity, uint32_t ent_parent, int faction_id,
                          uint32_t cookie, int flags, struct proj_desc pd);
void P_Projectile_Update(void);
bool P_Projectile_VelocityForTarget(vec3_t src, vec3_t dst, float init_speed, vec3_t *out);

bool P_Projectile_SaveState(struct SDL_RWops *stream);
bool P_Projectile_LoadState(struct SDL_RWops *stream);
void P_Projectile_ClearState(void);

#endif /* _PHYS_H_ */
