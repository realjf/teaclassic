#ifndef _COMBAT_H_
#define _COMBAT_H_

#include "public/game.h"
#include <stdbool.h>
#include <stdint.h>

struct entity;
struct SDL_RWops;
struct map;

bool G_Combat_Init(const struct map *map);
void G_Combat_Shutdown(void);

void G_Combat_AddEntity(const struct entity *ent, enum combat_stance initial);
void G_Combat_RemoveEntity(const struct entity *ent);
void G_Combat_StopAttack(const struct entity *ent);
void G_Combat_ClearSavedMoveCmd(const struct entity *ent);
int G_Combat_CurrContextualAction(void);

void G_Combat_AddRef(int faction_id, vec2_t pos);
void G_Combat_RemoveRef(int faction_id, vec2_t pos);
void G_Combat_AddTimeDelta(uint32_t delta);

bool G_Combat_SaveState(struct SDL_RWops *stream);
bool G_Combat_LoadState(struct SDL_RWops *stream);

struct entity *G_Combat_ClosestEligibleEnemy(const struct entity *ent);

#endif /* _COMBAT_H_ */
