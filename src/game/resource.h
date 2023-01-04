#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <stdbool.h>
#include <stdint.h>

struct map;
struct entity;
struct SDL_RWops;

bool G_Resource_Init(const struct map *map);
void G_Resource_Shutdown(void);
bool G_Resource_AddEntity(const struct entity *ent);
void G_Resource_RemoveEntity(struct entity *ent);
void G_Resource_UpdateBounds(const struct entity *ent);
void G_Resource_UpdateFactionID(const struct entity *ent, int oldfac,
                                int newfac);

bool G_Resource_SaveState(struct SDL_RWops *stream);
bool G_Resource_LoadState(struct SDL_RWops *stream);

#endif /* _RESOURCE_H_ */
