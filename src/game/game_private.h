#ifndef _GAME_PRIVATE_H_
#define _GAME_PRIVATE_H_

#include "gamestate.h"

struct camera;
struct entity;

enum ctx_action {
  CTX_ACTION_NONE,
  CTX_ACTION_ATTACK,
  CTX_ACTION_NO_ATTACK,
  CTX_ACTION_BUILD,
  CTX_ACTION_GATHER,
  CTX_ACTION_DROP_OFF,
  CTX_ACTION_TRANSPORT,
};

const khash_t(entity) * G_GetDynamicEntsSet(void);
const khash_t(entity) * G_GetAllEntsSet(void);
struct entity *G_EntityForUID(uint32_t uid);
enum ctx_action G_CurrContextualAction(void);
void G_NotifyOrderIssued(const struct entity *ent);

#endif /* _GAME_PRIVATE_H_ */
