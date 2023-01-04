#ifndef _SELECTION_H_
#define _SELECTION_H_

#include "../entity.h"
#include "../lib/public/vec.h"
#include "public/game.h"

#include <stdbool.h>

struct obb;
struct camera;
struct SDL_RWops;
struct entity;

VEC_TYPE(obb, struct obb)
VEC_PROTOTYPES(extern, obb, struct obb)

extern const vec3_t g_seltype_color_map[];

bool G_Sel_Init(void);
void G_Sel_Shutdown(void);
void G_Sel_Update(struct camera *cam, const vec_pentity_t *visible,
                  const vec_obb_t *visible_obbs);
bool G_Sel_SaveState(struct SDL_RWops *stream);
bool G_Sel_LoadState(struct SDL_RWops *stream);
void G_Sel_MarkHoveredDirty(void);
bool G_Sel_IsSelected(const struct entity *ent);

#endif /* _SELECTION_H_ */
