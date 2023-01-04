#ifndef _STORAGE_SITE_H_
#define _STORAGE_SITE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_CAPACITY (0)

struct entity;
struct SDL_RWops;

bool G_StorageSite_Init(void);
void G_StorageSite_Shutdown(void);
bool G_StorageSite_AddEntity(const struct entity *ent);
void G_StorageSite_RemoveEntity(const struct entity *ent);
bool G_StorageSite_IsSaturated(uint32_t uid);
void G_StorageSite_UpdateFaction(uint32_t uid, int oldfac, int newfac);
bool G_StorageSite_Desires(uint32_t uid, const char *rname);

bool G_StorageSite_SaveState(struct SDL_RWops *stream);
bool G_StorageSite_LoadState(struct SDL_RWops *stream);
void G_StorageSite_ClearState(void);

void G_StorageSite_SetUseAlt(const struct entity *ent, bool use);
bool G_StorageSite_GetUseAlt(uint32_t uid);
void G_StorageSite_ClearAlt(const struct entity *ent);
void G_StorageSite_ClearCurr(const struct entity *ent);

bool G_StorageSite_SetAltCapacity(const struct entity *ent, const char *rname,
                                  int max);
int G_StorageSite_GetAltCapacity(uint32_t uid, const char *rname);
bool G_StorageSite_SetAltDesired(uint32_t uid, const char *rname, int des);
int G_StorageSite_GetAltDesired(uint32_t uid, const char *rname);

#endif /* _STORAGE_SITE_H_ */
