#ifndef _ASSET_LOAD_H_
#define _ASSET_LOAD_H_

#include <stdbool.h>
#include <stddef.h>

#include <SDL.h> /* for SDL_RWops */

#define MAX_ANIM_SETS 16
#define MAX_LINE_LEN 256

#define READ_LINE(rwops, buff, fail_label)                                     \
  do {                                                                         \
    if (!AL_ReadLine(rwops, buff))                                             \
      goto fail_label;                                                         \
    buff[MAX_LINE_LEN - 1] = '\0';                                             \
  } while (0)

struct entity;
struct map;
struct aabb;
struct obb;

struct tcobj_hdr {
  float version;
  unsigned num_verts;
  unsigned num_joints;
  unsigned num_materials;
  unsigned num_as;
  unsigned frame_counts[MAX_ANIM_SETS];
  bool has_collision;
};

struct tcmap_hdr {
  float version;
  unsigned num_materials;
  unsigned num_rows;
  unsigned num_cols;
};

bool AL_Init(void);
void AL_Shutdown(void);

struct entity *AL_EntityFromTCObj(const char *base_path, const char *tcobj_name,
                                  const char *name, uint32_t uid);
bool AL_EntitySetTCObj(struct entity *ent, const char *base_path,
                       const char *tcobj_name);
void AL_EntityFree(struct entity *entity);
void *AL_RenderPrivateForName(const char *base_path, const char *tcobj_name);
bool AL_NameForRenderPrivate(void *render_private, char out_dir[static 512],
                             char out_name[static 512]);
bool AL_PreloadTCObj(const char *base_path, const char *tcobj_name);

struct map *AL_MapFromTCMapStream(SDL_RWops *stream, bool update_navgrid);
void AL_MapFree(struct map *map);
size_t AL_MapShallowCopySize(SDL_RWops *stream);

bool AL_ReadLine(SDL_RWops *stream, char *outbuff);
bool AL_ParseAABB(SDL_RWops *stream, struct aabb *out);

bool AL_SaveOBB(SDL_RWops *stream, const struct obb *obb);
bool AL_LoadOBB(SDL_RWops *stream, struct obb *out);

#endif /* _ASSET_LOAD_H_ */
