#ifndef _BUILDING_H_
#define _BUILDING_H_

#include <stdbool.h>

struct entity;
struct map;
struct SDL_RWops;

bool G_Building_Init(const struct map *map);
void G_Building_Shutdown(void);

bool G_Building_AddEntity(struct entity *ent);
void G_Building_RemoveEntity(const struct entity *ent);
void G_Building_UpdateProgress(struct entity *ent, float frac_done);
void G_Building_UpdateBounds(const struct entity *ent);
void G_Building_UpdateFactionID(const struct entity *ent, int oldfac,
                                int newfac);
bool G_Building_NeedsRepair(const struct entity *ent);

bool G_Building_SaveState(struct SDL_RWops *stream);
bool G_Building_LoadState(struct SDL_RWops *stream);

#endif /* _BUILDING_H_ */
