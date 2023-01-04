#ifndef _FOG_OF_WAR_H_
#define _FOG_OF_WAR_H_

#include "../tc_math.h"
#include <stdbool.h>

struct map;
struct obb;
struct SDL_RWops;

bool G_Fog_Init(const struct map *map);
void G_Fog_Shutdown(void);

void G_Fog_AddVision(vec2_t xz_pos, int faction_id, float radius);
void G_Fog_RemoveVision(vec2_t xz_pos, int faction_id, float radius);
void G_Fog_UpdateVisionRange(vec2_t xz_pos, int faction_id, float oldr,
                             float newr);

bool G_Fog_CircleExplored(uint16_t fac_mask, vec2_t xz_pos, float radius);
bool G_Fog_RectExplored(uint16_t fac_mask, vec2_t xz_pos, float halfx,
                        float halfz);

void G_Fog_ExploreCircle(vec2_t xz_pos, int faction_id, float radius);
void G_Fog_ExploreRectangle(vec2_t xz_pos, int faction_id, float halfx,
                            float halfz);

void G_Fog_UpdateVisionState(void);
void G_Fog_ClearExploredCache(void);

bool G_Fog_SaveState(struct SDL_RWops *stream);
bool G_Fog_LoadState(struct SDL_RWops *stream);

bool G_Fog_Enabled(void);


#endif /* _FOG_OF_WAR_H_ */
