#ifndef _MOVEMENT_H_
#define _MOVEMENT_H_

#include "../tc_math.h"
#include <stdbool.h>

#define MOVE_TICK_RES (20)

struct map;
struct entity;
struct SDL_RWops;

bool G_Move_Init(const struct map *map);
void G_Move_Shutdown(void);

void G_Move_AddEntity(const struct entity *ent);
void G_Move_RemoveEntity(const struct entity *ent);

bool G_Move_GetDest(const struct entity *ent, vec2_t *out_xz, bool *out_attack);
bool G_Move_GetSurrounding(const struct entity *ent, uint32_t *out_uid);

void G_Move_Stop(const struct entity *ent);
void G_Move_SetSeekEnemies(const struct entity *ent);
void G_Move_SetSurroundEntity(const struct entity *ent,
                              const struct entity *target);
void G_Move_SetChangeDirection(const struct entity *ent, quat_t target);
void G_Move_SetEnterRange(const struct entity *ent, const struct entity *target,
                          float range);

void G_Move_UpdatePos(const struct entity *ent, vec2_t pos);
void G_Move_UpdateFactionID(const struct entity *ent, int oldfac, int newfac);
bool G_Move_InTargetMode(void);

bool G_Move_SaveState(struct SDL_RWops *stream);
bool G_Move_LoadState(struct SDL_RWops *stream);

#endif /* _MOVEMENT_H_ */
