#ifndef _REGION_H_
#define _REGION_H_

#include "../tc_math.h"

#include <stdbool.h>
#include <stdint.h>

struct map;
struct entity;
struct SDL_RWops;

bool G_Region_Init(const struct map *map);
void G_Region_Shutdown(void);
void G_Region_RemoveRef(uint32_t uid, vec2_t oldpos);
void G_Region_AddRef(uint32_t uid, vec2_t newpos);
void G_Region_RemoveEnt(uint32_t uid);
void G_Region_Update(void);

bool G_Region_SaveState(struct SDL_RWops *stream);
bool G_Region_LoadState(struct SDL_RWops *stream);

#endif /* _REGION_H_ */
