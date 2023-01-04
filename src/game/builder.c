#include "builder.h"
#include "../cursor.h"
#include "../entity.h"
#include "../event.h"
#include "../lib/public/attr.h"
#include "../lib/public/khash.h"
#include "../map/public/map.h"
#include "../perf.h"
#include "../phys/public/collision.h"
#include "building.h"
#include "game_private.h"
#include "harvester.h"
#include "movement.h"
#include "position.h"
#include "public/game.h"
#include "storage_site.h"

#include <assert.h>
#include <stdint.h>

#define UID_NONE (~((uint32_t)0))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CHK_TRUE_RET(_pred)                                                    \
  do {                                                                         \
    if (!(_pred))                                                              \
      return false;                                                            \
  } while (0)

struct builderstate {
  enum {
    STATE_NOT_BUILDING,
    STATE_MOVING_TO_TARGET,
    STATE_BUILDING,
  } state;
  int build_speed;
  uint32_t target_uid;
};

KHASH_MAP_INIT_INT(state, struct builderstate)

static void on_build_anim_finished(void *user, void *event);
static void on_motion_end(void *user, void *event);

/*****************************************************************************/
/* STATIC VARIABLES                                                          */
/*****************************************************************************/

static khash_t(state) * s_entity_state_table;
static struct map *s_map;
static bool s_build_on_lclick = false;

/*****************************************************************************/
/* STATIC FUNCTIONS                                                          */
/*****************************************************************************/

static struct builderstate *builderstate_get(uint32_t uid) {
  khiter_t k = kh_get(state, s_entity_state_table, uid);
  if (k == kh_end(s_entity_state_table))
    return NULL;

  return &kh_value(s_entity_state_table, k);
}

static void builderstate_set(const struct entity *ent, struct builderstate bs) {
  int ret;
  assert(ent->flags & ENTITY_FLAG_BUILDER);

  khiter_t k = kh_put(state, s_entity_state_table, ent->uid, &ret);
  assert(ret != -1 && ret != 0);
  kh_value(s_entity_state_table, k) = bs;
}

static void builderstate_remove(const struct entity *ent) {
  assert(ent->flags & ENTITY_FLAG_BUILDER);

  khiter_t k = kh_get(state, s_entity_state_table, ent->uid);
  if (k != kh_end(s_entity_state_table))
    kh_del(state, s_entity_state_table, k);
}

static void on_motion_begin(void *user, void *event) {
  uint32_t uid = (uintptr_t)user;
  struct entity *ent = G_EntityForUID(uid);

  struct builderstate *bs = builderstate_get(uid);
  assert(bs);
  assert(bs->state == STATE_BUILDING);

  E_Entity_Unregister(EVENT_MOTION_START, uid, on_motion_begin);
  E_Entity_Unregister(EVENT_ANIM_CYCLE_FINISHED, uid, on_build_anim_finished);

  bs->state = STATE_NOT_BUILDING;
  E_Entity_Notify(EVENT_BUILD_END, uid, NULL, ES_ENGINE);
}

static void finish_building(struct builderstate *bs, uint32_t uid) {
  E_Entity_Unregister(EVENT_ANIM_CYCLE_FINISHED, uid, on_build_anim_finished);
  E_Entity_Unregister(EVENT_MOTION_START, uid, on_motion_begin);
  E_Entity_Unregister(EVENT_MOTION_END, uid, on_motion_end);

  if (bs->state == STATE_BUILDING) {
    E_Entity_Notify(EVENT_BUILD_END, uid, NULL, ES_ENGINE);
  }

  bs->state = STATE_NOT_BUILDING;
  bs->target_uid = UID_NONE;
}

static void on_build_anim_finished(void *user, void *event) {
  uint32_t uid = (uintptr_t)user;
  const struct entity *ent = G_EntityForUID(uid);

  struct builderstate *bs = builderstate_get(uid);
  assert(bs);

  struct entity *target = G_EntityForUID(bs->target_uid);
  if (!target || (target->flags & ENTITY_FLAG_ZOMBIE)) {
    finish_building(bs, uid);
    return;
  }

  if (!(target->flags & ENTITY_FLAG_COMBATABLE)) {
    G_Building_Complete(target);
    finish_building(bs, uid);
    return;
  }

  int max_hp = G_Combat_GetMaxHP(target);
  int hp = MIN(G_Combat_GetCurrentHP(target) + bs->build_speed, max_hp);

  G_Combat_SetCurrentHP(target, hp);
  G_Building_UpdateProgress(target, hp / (float)max_hp);

  if (hp == max_hp) {
    G_Building_Complete(target);
    finish_building(bs, uid);
    return;
  }
}

static void on_motion_end(void *user, void *event) {
  uint32_t uid = (uintptr_t)user;
  struct entity *ent = G_EntityForUID(uid);

  struct builderstate *bs = builderstate_get(uid);
  assert(bs);

  if (!G_Move_Still(ent))
    return;

  E_Entity_Unregister(EVENT_MOTION_END, uid, on_motion_end);
  assert(bs->target_uid != UID_NONE);
  struct entity *target = G_EntityForUID(bs->target_uid);

  if (!target || !M_NavObjAdjacent(s_map, ent, target)) {
    bs->state = STATE_NOT_BUILDING;
    bs->target_uid = UID_NONE;
    return; /* builder could not reach the building */
  }

  if (!G_Building_IsFounded(target)) {

    if (G_Building_Unobstructed(target) && G_Building_Found(target, true)) {
      E_Entity_Notify(EVENT_BUILDING_FOUNDED, target->uid, NULL, ES_ENGINE);
    } else {
      bs->state = STATE_NOT_BUILDING;
      bs->target_uid = UID_NONE;
      E_Entity_Notify(EVENT_BUILD_FAIL_FOUND, uid, NULL, ES_ENGINE);
      return;
    }
  }

  if (!G_Building_IsSupplied(target) &&
      G_StorageSite_IsSaturated(target->uid)) {
    G_Building_Supply(target);
  }

  if (!G_Building_IsSupplied(target)) {
    if (ent->flags & ENTITY_FLAG_HARVESTER) {
      G_Harvester_Stop(ent->uid);
      G_Harvester_SupplyBuilding(ent, target);
    }
    bs->state = STATE_NOT_BUILDING;
    return;
  }

  bs->state = STATE_BUILDING;
  E_Entity_Notify(EVENT_BUILD_BEGIN, uid, NULL, ES_ENGINE);
  E_Entity_Register(EVENT_MOTION_START, uid, on_motion_begin,
                    (void *)((uintptr_t)uid), G_RUNNING);
  E_Entity_Register(EVENT_ANIM_CYCLE_FINISHED, uid, on_build_anim_finished,
                    (void *)((uintptr_t)uid), G_RUNNING);
}

static void on_mousedown(void *user, void *event) {
  SDL_MouseButtonEvent *mouse_event = &(((SDL_Event *)event)->button);

  bool targeting = G_Builder_InTargetMode();
  bool right = (mouse_event->button == SDL_BUTTON_RIGHT);
  bool left = (mouse_event->button == SDL_BUTTON_LEFT);

  s_build_on_lclick = false;

  if (G_MouseOverMinimap())
    return;

  if (S_UI_MouseOverWindow(mouse_event->x, mouse_event->y))
    return;

  if (right && targeting)
    return;

  if (left && !targeting)
    return;

  if (right && G_CurrContextualAction() != CTX_ACTION_BUILD)
    return;

  struct entity *target = G_Sel_GetHovered();
  if (!target || !(target->flags & ENTITY_FLAG_BUILDING) ||
      !G_Building_NeedsRepair(target))
    return;

  enum selection_type sel_type;
  const vec_pentity_t *sel = G_Sel_Get(&sel_type);
  size_t nbuilding = 0;

  if (sel_type != SELECTION_TYPE_PLAYER)
    return;

  for (int i = 0; i < vec_size(sel); i++) {

    struct entity *curr = vec_AT(sel, i);
    if (!(curr->flags & ENTITY_FLAG_BUILDER))
      continue;

    struct builderstate *bs = builderstate_get(curr->uid);
    assert(bs);

    finish_building(bs, curr->uid);
    G_StopEntity(curr, true);
    G_Builder_Build(curr, target);
    G_NotifyOrderIssued(curr);
    nbuilding++;
  }

  if (nbuilding) {
    Entity_Ping(target);
  }
}

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

bool G_Builder_Init(struct map *map) {
  if (NULL == (s_entity_state_table = kh_init(state)))
    return false;

  s_map = map;
  s_build_on_lclick = false;
  E_Global_Register(SDL_MOUSEBUTTONDOWN, on_mousedown, NULL, G_RUNNING);
  return true;
}

void G_Builder_Shutdown(void) {
  s_map = NULL;
  E_Global_Unregister(SDL_MOUSEBUTTONDOWN, on_mousedown);
  kh_destroy(state, s_entity_state_table);
}

bool G_Builder_Build(struct entity *builder, struct entity *building) {
  struct builderstate *bs = builderstate_get(builder->uid);
  assert(bs);

  if (!(building->flags & ENTITY_FLAG_BUILDING))
    return false;

  E_Entity_Unregister(EVENT_MOTION_END, builder->uid, on_motion_end);
  E_Entity_Unregister(EVENT_MOTION_START, builder->uid, on_motion_begin);
  E_Entity_Unregister(EVENT_ANIM_CYCLE_FINISHED, builder->uid,
                      on_build_anim_finished);

  bs->state = STATE_MOVING_TO_TARGET;
  bs->target_uid = building->uid;
  E_Entity_Notify(EVENT_BUILD_TARGET_ACQUIRED, builder->uid, building,
                  ES_ENGINE);

  if (M_NavObjAdjacent(s_map, builder, building)) {
    on_motion_end((void *)((uintptr_t)builder->uid), NULL);
  } else {
    G_Move_SetSurroundEntity(builder, building);
    E_Entity_Register(EVENT_MOTION_END, builder->uid, on_motion_end,
                      (void *)((uintptr_t)builder->uid), G_RUNNING);
  }

  return true;
}

void G_Builder_AddEntity(struct entity *ent) {
  assert(builderstate_get(ent->uid) == NULL);
  assert(ent->flags & ENTITY_FLAG_BUILDER);

  builderstate_set(ent, (struct builderstate){
                            .state = STATE_NOT_BUILDING,
                            .build_speed = 0.0,
                            .target_uid = UID_NONE,
                        });
}

void G_Builder_RemoveEntity(const struct entity *ent) {
  if (!(ent->flags & ENTITY_FLAG_BUILDER))
    return;
  E_Entity_Unregister(EVENT_MOTION_END, ent->uid, on_motion_end);
  E_Entity_Unregister(EVENT_MOTION_START, ent->uid, on_motion_begin);
  E_Entity_Unregister(EVENT_ANIM_CYCLE_FINISHED, ent->uid,
                      on_build_anim_finished);
  builderstate_remove(ent);
}

void G_Builder_SetBuildSpeed(const struct entity *ent, int speed) {
  struct builderstate *bs = builderstate_get(ent->uid);
  assert(bs);
  bs->build_speed = speed;
}

int G_Builder_GetBuildSpeed(const struct entity *ent) {
  struct builderstate *bs = builderstate_get(ent->uid);
  assert(bs);
  return bs->build_speed;
}

void G_Builder_SetBuildOnLeftClick(void) { s_build_on_lclick = true; }

bool G_Builder_InTargetMode(void) { return s_build_on_lclick; }

bool G_Builder_HasRightClickAction(void) {
  struct entity *hovered = G_Sel_GetHovered();
  if (!hovered)
    return false;

  enum selection_type sel_type;
  const vec_pentity_t *sel = G_Sel_Get(&sel_type);

  if (vec_size(sel) == 0)
    return false;

  const struct entity *first = vec_AT(sel, 0);
  if (first->flags & ENTITY_FLAG_BUILDER &&
      hovered->flags & ENTITY_FLAG_BUILDING && G_Building_IsFounded(hovered))
    return true;

  return false;
}

int G_Builder_CurrContextualAction(void) {
  struct entity *hovered = G_Sel_GetHovered();
  if (!hovered)
    return CTX_ACTION_NONE;

  if (M_MouseOverMinimap(s_map))
    return CTX_ACTION_NONE;

  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  if (S_UI_MouseOverWindow(mouse_x, mouse_y))
    return CTX_ACTION_NONE;

  if (G_Builder_InTargetMode())
    return CTX_ACTION_NONE;

  enum selection_type sel_type;
  const vec_pentity_t *sel = G_Sel_Get(&sel_type);

  if (vec_size(sel) == 0 || sel_type != SELECTION_TYPE_PLAYER)
    return CTX_ACTION_NONE;

  const struct entity *first = vec_AT(sel, 0);
  if (!(first->flags & ENTITY_FLAG_BUILDER))
    return CTX_ACTION_NONE;

  if (G_GetFactionID(hovered->uid) != G_GetFactionID(first->uid))
    return false;

  if (hovered->flags & ENTITY_FLAG_BUILDING && G_Building_NeedsRepair(hovered))
    return CTX_ACTION_BUILD;

  return CTX_ACTION_NONE;
}

void G_Builder_Stop(uint32_t uid) {
  struct builderstate *bs = builderstate_get(uid);
  assert(bs);
  finish_building(bs, uid);
}

bool G_Builder_SaveState(struct SDL_RWops *stream) {
  struct attr num_builders = (struct attr){
      .type = TYPE_INT, .val.as_int = kh_size(s_entity_state_table)};
  CHK_TRUE_RET(Attr_Write(stream, &num_builders, "num_builders"));

  uint32_t uid;
  struct builderstate curr;

  kh_foreach(s_entity_state_table, uid, curr, {
    struct attr buid = (struct attr){.type = TYPE_INT, .val.as_int = uid};
    CHK_TRUE_RET(Attr_Write(stream, &buid, "builder_uid"));

    struct attr state =
        (struct attr){.type = TYPE_INT, .val.as_int = curr.state};
    CHK_TRUE_RET(Attr_Write(stream, &state, "builder_state"));

    struct attr speed =
        (struct attr){.type = TYPE_INT, .val.as_int = curr.build_speed};
    CHK_TRUE_RET(Attr_Write(stream, &speed, "builder_speed"));

    struct attr target =
        (struct attr){.type = TYPE_INT, .val.as_int = curr.target_uid};
    CHK_TRUE_RET(Attr_Write(stream, &target, "builder_target"));
  });

  return true;
}

bool G_Builder_LoadState(struct SDL_RWops *stream) {
  struct attr attr;

  CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
  CHK_TRUE_RET(attr.type == TYPE_INT);
  const int num_builders = attr.val.as_int;

  for (int i = 0; i < num_builders; i++) {

    CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
    CHK_TRUE_RET(attr.type == TYPE_INT);
    uint32_t uid = attr.val.as_int;

    CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
    CHK_TRUE_RET(attr.type == TYPE_INT);
    int state = attr.val.as_int;

    CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
    CHK_TRUE_RET(attr.type == TYPE_INT);
    int speed = attr.val.as_int;

    CHK_TRUE_RET(Attr_Parse(stream, &attr, true));
    CHK_TRUE_RET(attr.type == TYPE_INT);
    int target = attr.val.as_int;

    struct builderstate *bs = builderstate_get(uid);
    CHK_TRUE_RET(bs);

    bs->state = state;
    bs->build_speed = speed;
    bs->target_uid = target;

    switch (state) {
    case STATE_NOT_BUILDING:
      break;
    case STATE_MOVING_TO_TARGET:
      E_Entity_Register(EVENT_MOTION_END, uid, on_motion_end,
                        (void *)((uintptr_t)uid), G_RUNNING);
      break;
    case STATE_BUILDING:
      E_Entity_Register(EVENT_MOTION_START, uid, on_motion_begin,
                        (void *)((uintptr_t)uid), G_RUNNING);
      E_Entity_Register(EVENT_ANIM_CYCLE_FINISHED, uid, on_build_anim_finished,
                        (void *)((uintptr_t)uid), G_RUNNING);
      break;
    default:
      return false;
    }
  }
  return true;
}
