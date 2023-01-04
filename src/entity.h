#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "tc_math.h"
#include "lib/public/vec.h"
#include "map/public/tile.h"
#include "phys/public/collision.h"

#include <stdbool.h>

#define MAX_JOINTS (96)
#define MAX_TAGS (127)

enum {
    ENTITY_FLAG_ANIMATED = (1 << 0),
    ENTITY_FLAG_COLLISION = (1 << 1),
    ENTITY_FLAG_SELECTABLE = (1 << 2),
    ENTITY_FLAG_MOVABLE = (1 << 3),
    ENTITY_FLAG_COMBATABLE = (1 << 4),
    ENTITY_FLAG_INVISIBLE = (1 << 5),
    /* zombie entities are those that have died in the game simulation,
     * but are still retained by some scripting variable */
    ENTITY_FLAG_ZOMBIE = (1 << 6),
    ENTITY_FLAG_MARKER = (1 << 7),
    ENTITY_FLAG_BUILDING = (1 << 8),
    ENTITY_FLAG_BUILDER = (1 << 9),
    ENTITY_FLAG_TRANSLUCENT = (1 << 10),
    ENTITY_FLAG_RESOURCE = (1 << 11),
    ENTITY_FLAG_HARVESTER = (1 << 12),
    ENTITY_FLAG_STORAGE_SITE = (1 << 13),
};

struct entity {
    uint32_t uid;
    uint32_t flags;
    const char *name;
    const char *basedir;
    const char *filename;
    void *render_private;
    void *anim_private;
    struct aabb identity_aabb; /* Bind-pose AABB */
};

/* State needed for rendering a static entity */
struct ent_stat_rstate {
    void *render_private;
    mat4x4_t model;
    bool translucent;
    struct tile_desc td; /* For binning to a chunk batch */
};

/* State needed for rendering an animated entity */
struct ent_anim_rstate {
    void *render_private;
    mat4x4_t model;
    bool translucent;
    size_t njoints;
    const mat4x4_t *inv_bind_pose; /* static, use shallow copy */
    mat4x4_t curr_pose[MAX_JOINTS];
};

VEC_TYPE(rstat, struct ent_stat_rstate)
VEC_IMPL(static inline, rstat, struct ent_stat_rstate)

VEC_TYPE(ranim, struct ent_anim_rstate)
VEC_IMPL(static inline, ranim, struct ent_anim_rstate)

struct map;

bool Entity_Init(void);
void Entity_Shutdown(void);
void Entity_ClearState(void);

void Entity_ModelMatrix(const struct entity *ent, mat4x4_t *out);
uint32_t Entity_NewUID(void);
void Entity_SetNextUID(uint32_t uid);
void Entity_CurrentOBB(const struct entity *ent, struct obb *out, bool identity);
vec3_t Entity_CenterPos(const struct entity *ent);
vec3_t Entity_TopCenterPointWS(const struct entity *ent);
void Entity_FaceTowards(struct entity *ent, vec2_t point);
void Entity_Ping(const struct entity *ent);
vec2_t Entity_TopScreenPos(const struct entity *ent, int screenw, int screenh);
/* Coarse-grained test that can give false positives. Use the check to get
 * positives, but confirm positive results with a more precise check */
bool Entity_MaybeAdjacentFast(const struct entity *a, const struct entity *b, float buffer);
bool Entity_AddTag(uint32_t uid, const char *tag);
void Entity_RemoveTag(uint32_t uid, const char *tag);
bool Entity_HasTag(uint32_t uid, const char *tag);
void Entity_ClearTags(uint32_t uid);
size_t Entity_EntsForTag(const char *tag, size_t maxout, uint32_t out[static maxout]);
size_t Entity_TagsForEnt(uint32_t uid, size_t maxout, const char *out[static maxout]);
void Entity_DisappearAnimated(struct entity *ent, const struct map *map, void (*on_finish)(void *), void *arg);
int Entity_NavLayer(const struct entity *ent);

quat_t Entity_GetRot(uint32_t uid);
void Entity_SetRot(uint32_t uid, quat_t rot);
vec3_t Entity_GetScale(uint32_t uid);
void Entity_SetScale(uint32_t uid, vec3_t scale);
void Entity_Remove(uint32_t uid);

#endif /* _ENTITY_H_ */
