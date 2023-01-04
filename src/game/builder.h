#ifndef _BUILDER_H_
#define _BUILDER_H_

#include <stdbool.h>
#include <stdint.h>

struct entity;
struct map;
struct SDL_RWops;

bool G_Builder_Init(struct map *map);
void G_Builder_Shutdown(void);
void G_Builder_AddEntity(struct entity *ent);
void G_Builder_RemoveEntity(const struct entity *ent);
bool G_Builder_InTargetMode(void);
int G_Builder_CurrContextualAction(void);
void G_Builder_Stop(uint32_t uid);

bool G_Builder_SaveState(struct SDL_RWops *stream);
bool G_Builder_LoadState(struct SDL_RWops *stream);

#endif /* _BUILDER_H_ */
