#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "../lib/public/vec.h"
#include "../render/public/render_ctrl.h"
#include "faction.h"
#include "public/game.h"
#include "selection.h"

#include <stdint.h>

KHASH_DECLARE(faction, khint32_t, int)
KHASH_DECLARE(range, khint32_t, float)

struct gamestate {
  enum simstate ss;
  /*-------------------------------------------------------------------------
   * The requested simulation state to change to at the end of the tick.
   *-------------------------------------------------------------------------
   */
  enum simstate requested_ss;
  /*-------------------------------------------------------------------------
   * The SDL tick during which we last changed simulation states.
   *-------------------------------------------------------------------------
   */
  uint32_t ss_change_tick;
  /*-------------------------------------------------------------------------
   * Currently loaded map. May be NULL.
   *-------------------------------------------------------------------------
   */
  struct map *map;
  /*-------------------------------------------------------------------------
   * Light position in worldspace coordinates.
   *-------------------------------------------------------------------------
   */
  vec3_t light_pos;
  /*-------------------------------------------------------------------------
   * Boolean to disable rendering of healthbars. Overrides the user-modifiable
   * setting.
   *-------------------------------------------------------------------------
   */
  bool hide_healthbars;
  /*-------------------------------------------------------------------------
   * Boolean to force rendering every single entity on the minimap.
   *-------------------------------------------------------------------------
   */
  bool minimap_render_all;
  /*-------------------------------------------------------------------------
   * The camera from which the scene is currently being rendered.
   *-------------------------------------------------------------------------
   */
  struct camera *active_cam;
  /*-------------------------------------------------------------------------
   * The camera mode determines which camera controller is installed.
   *-------------------------------------------------------------------------
   */
  enum cam_mode active_cam_mode;
  /*-------------------------------------------------------------------------
   * The set of all game entities currently taking part in the game simulation.
   *-------------------------------------------------------------------------
   */
  khash_t(entity) * active;
  /*-------------------------------------------------------------------------
   * Table mapping an entity to its' faction ID for all active entities.
   * Synchronized with 'active' table at function call boundaries.
   *-------------------------------------------------------------------------
   */
  khash_t(faction) * ent_faction_map;
  /*-------------------------------------------------------------------------
   * Table mapping an entity to its' vision range for active entities.
   * Synchronized with 'active' table at function call boundaries.
   *-------------------------------------------------------------------------
   */
  khash_t(range) * ent_visrange_map;
  /*-------------------------------------------------------------------------
   * Table mapping an entity to its' selection radius for active entities.
   * Synchronized with 'active' table at function call boundaries.
   *-------------------------------------------------------------------------
   */
  khash_t(range) * selection_radiuses;
  /*-------------------------------------------------------------------------
   * Up-to-date set of all non-static entities. (Subset of 'active' set).
   * Used for collision avoidance force computations.
   *-------------------------------------------------------------------------
   */
  khash_t(entity) * dynamic;
  /*-------------------------------------------------------------------------
   * The set of entities potentially visible by the active camera. Updated
   * every frame.
   *-------------------------------------------------------------------------
   */
  vec_pentity_t visible;
  /*-------------------------------------------------------------------------
   * The set of entities that should be rendered from the light's point of
   * view (for creating the shadow depth map).
   *-------------------------------------------------------------------------
   */
  vec_pentity_t light_visible;
  /*-------------------------------------------------------------------------
   * Cache of current-frame OBBs for visible entities.
   *-------------------------------------------------------------------------
   */
  vec_obb_t visible_obbs;
  /*-------------------------------------------------------------------------
   * The state of the factions in the current game. 'factions_allocd' has a
   * set bit for every faction index that's 'allocated'. Clear bits are 'free'.
   *-------------------------------------------------------------------------
   */
  uint16_t factions_allocd;
  struct faction factions[MAX_FACTIONS];
  /*-------------------------------------------------------------------------
   * Holds the relationships between every 2 factions. Note that diplomatic
   * relations are always symmetric (i.e always 'mutually' at war or peace).
   *-------------------------------------------------------------------------
   */
  enum diplomacy_state diplomacy_table[MAX_FACTIONS][MAX_FACTIONS];
  /*-------------------------------------------------------------------------
   * The index indo the 'ws' field, where the rendering commands are stored.
   * The previous frame workspace is owned by the render thread. The render
   * and simulatino workspaces are swapped at the end of every frame.
   *-------------------------------------------------------------------------
   */
  int curr_ws_idx;
  struct render_workspace ws[2];
  /*-------------------------------------------------------------------------
   * A readonly snapshot (copy) of the map from the previous simulation tick.
   * This is used by the render thread for making certain queries like size,
   * height at a point, etc.
   *-------------------------------------------------------------------------
   */
  const struct map *prev_tick_map;
  /*-------------------------------------------------------------------------
   * Entities currently scheduled for removal. They will be removed from the
   * game simulation at the end of the tick.
   *-------------------------------------------------------------------------
   */
  vec_pentity_t removed;
};


#endif /* _GAMESTATE_H_ */
