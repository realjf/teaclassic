#ifndef _HARVESTER_H_
#define _HARVESTER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_GATHER_SPEED (0)
#define DEFAULT_MAX_CARRY (0)

struct map;
struct entity;
struct SDL_RWops;

bool G_Harvester_Init(const struct map *map);
void G_Harvester_Shutdown(void);
bool G_Harvester_AddEntity(uint32_t uid);
void G_Harvester_RemoveEntity(uint32_t uid);
void G_Harvester_Stop(uint32_t uid);
bool G_Harvester_SupplyBuilding(struct entity *harvester,
                                struct entity *building);
bool G_Harvester_InTargetMode(void);
int G_Harvester_CurrContextualAction(void);
bool G_Harvester_GetContextualCursor(char *out, size_t maxout);
void G_Harvester_ClearQueuedCmd(uint32_t uid);

bool G_Harvester_SaveState(struct SDL_RWops *stream);
bool G_Harvester_LoadState(struct SDL_RWops *stream);

#endif /* _HARVESTER_H_ */
