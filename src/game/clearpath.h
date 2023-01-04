#ifndef _CLEARPATH_H_
#define _CLEARPATH_H_

#include "../lib/public/vec.h"
#include "../tc_math.h"

#define CLEARPATH_NEIGHBOUR_RADIUS (10.0f)
/* This is added to the entity's radius so that it will take wider turns
 * and leave this as a buffer between it and the obstacle.
 */
#define CLEARPATH_BUFFER_RADIUS (0.0f)

struct map;

struct cp_ent {
  vec2_t xz_pos;
  vec2_t xz_vel; /* specified per pathfinding tick */
  float radius;
};

VEC_TYPE(cp_ent, struct cp_ent)
VEC_IMPL(static inline, cp_ent, struct cp_ent)

void G_ClearPath_Init(const struct map *map);
void G_ClearPath_Shutdown(void);
bool G_ClearPath_ShouldSaveDebug(uint32_t ent_uid);

vec2_t G_ClearPath_NewVelocity(struct cp_ent ent, uint32_t ent_uid,
                               vec2_t ent_des_v, vec_cp_ent_t dyn_neighbs,
                               vec_cp_ent_t stat_neighbs, bool save_debug);

#endif /* _CLEARPATH_H_ */
